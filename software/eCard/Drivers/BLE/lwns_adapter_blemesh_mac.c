#include "lwns_adapter_blemesh_mac.h"
#include "lwns_sec.h"

//ÿ���ļ�����debug��ӡ�Ŀ��أ���0���Խ�ֹ���ļ��ڲ���ӡ
#define DEBUG_PRINT_IN_THIS_FILE 1
#if DEBUG_PRINT_IN_THIS_FILE
    #define PRINTF(...) PRINT(__VA_ARGS__)
#else
    #define PRINTF(...) do {} while (0)
#endif

#if LWNS_USE_BLEMESH_MAC//�Ƿ�ʹ��ģ��blemesh��macЭ�飬ע��ֻ��ʹ��һ��mac��Э�顣

//for lwns_packet_buffer save
__attribute__((aligned(4)))  static lwns_qbuf_list_t qbuf_memp[QBUF_MANUAL_NUM];

//for lwns_route_entry manage
#if ROUTE_ENTRY_MANUAL_NUM
    __attribute__((aligned(4)))  static lwns_route_entry_data_t route_entry_memp[ROUTE_ENTRY_MANUAL_NUM];
#endif

//for neighbor manage
__attribute__((aligned(4)))  static lwns_neighbor_list_t neighbor_memp[LWNS_NEIGHBOR_MAX_NUM];

static void ble_new_neighbor_callback(lwns_addr_t *n);          //�������ھӻص�����
static BOOL ble_phy_output(uint8_t *dataptr, uint8_t len);      //���ͽӿں���
static void RF_2G4StatusCallBack(uint8_t sta, uint8_t crc, uint8_t *rxBuf);

static uint8_t lwns_adapter_taskid;
static uint16_t lwns_adapter_ProcessEvent(uint8_t task_id, uint16_t events);
static uint8_t lwns_phyoutput_taskid;
static uint16_t lwns_phyoutput_ProcessEvent(uint8_t task_id, uint16_t events);


/**
 * lwns���õĺ����ӿڣ���ָ�봫�ݸ�lwns���ڲ�ʹ��
 */
static lwns_fuc_interface_t ble_lwns_fuc_interface =
{
    .lwns_phy_output  = ble_phy_output,
    .lwns_rand        = tmos_rand,
    .lwns_memcpy      = tmos_memcpy,
    .lwns_memcmp      = tmos_memcmp,
    .lwns_memset      = tmos_memset,
    .new_neighbor_callback  = NULL,
};

static const uint8_t ble_phy_channelmap[] = {8, 18, 28}; //����Ҫ�õ���ͨ�����ϣ��û����и���Ϊ��Ҫ���͵�ͨ�����ϡ�
static uint8_t ble_phy_channelmap_send_seq = 0, ble_phy_channelmap_receive_seq = 0, ble_phy_send_cnt = 0;//���ͽ��յ�ǰͨ����źͷ��ʹ�������
static struct blemesh_mac_phy_manage_struct    *blemesh_phy_manage_list_head = NULL; //mac������LIST�б�ָ��
static struct blemesh_mac_phy_manage_struct     blemesh_phy_manage_list[LWNS_MAC_SEND_PACKET_MAX_NUM]; //mac�������б��������

/*********************************************************************
 * @fn      RF_2G4StatusCallBack
 *
 * @brief   RF ״̬�ص���ע�⣺�����ڴ˺�����ֱ�ӵ���RF���ջ��߷���API����Ҫʹ���¼��ķ�ʽ����
 *
 * @param   sta     -   ״̬����
 * @param   crc     -   crcУ����
 * @param   rxBuf   -   ����bufָ��
 *
 * @return  None.
 */
static void RF_2G4StatusCallBack(uint8_t sta, uint8_t crc, uint8_t *rxBuf)  //rxBuf[0]Ϊ�ź�ǿ�ȣ�rxBuf[1]Ϊ�����յ����ݵĳ���
{
    switch (sta)
    {
    case RX_MODE_RX_DATA:
    {
        if (crc == 1)
        {
            PRINTF("crc error\n");
        }
        else if (crc == 2)
        {
            PRINTF("match type error\n");
        }
        else
        {
            uint8_t *pMsg;
#if LWNS_ENCRYPT_ENABLE//�Ƿ�������Ϣ����
            if (((rxBuf[1] % 16) == 1) && (rxBuf[1] >= 17) && (rxBuf[1] > rxBuf[2]))  //���������������16���ֽڣ�������ʵ���ݳ���һ�ֽ�
            {
                //����У��ͨ��������rxBuf[1] - 1��Ϊ16�ı���
                pMsg = tmos_msg_allocate(rxBuf[1]);//�����ڴ�ռ䣬��ʵ���ݳ��Ȳ���Ҫ����
                if (pMsg != NULL)
                {
                    lwns_msg_decrypt(rxBuf + 3, pMsg + 1, rxBuf[1] - 1);//��������
                    if ((rxBuf[2] ^ pMsg[rxBuf[2]]) == pMsg[rxBuf[2] + 1])
                    {
                        pMsg[0] = rxBuf[2];//У��ͨ�����洢��ʵ���ݳ���
                        PRINTF("send rx msg\n");//���ͽ��յ������ݵ����ս�����
                        tmos_msg_send(lwns_adapter_taskid, pMsg);
                    }
                    else
                    {
                        PRINTF("verify rx msg err\n");//У��ʧ��
                        tmos_msg_deallocate(pMsg);
                    }
                }
                else
                {
                    PRINTF("send rx msg failed\n");//�����ڴ�ʧ�ܣ��޷����ͽ��յ�������
                }
            }
            else
            {
                PRINTF("bad len\n");//�����Ȳ���
            }
#else
            if (rxBuf[1] >= LWNS_PHY_OUTPUT_MIN_SIZE)   //���ݳ��ȷ��ϣ��Żᷢ����Э��ջ�ڲ�����
            {
                pMsg = tmos_msg_allocate(rxBuf[1] + 1);
                if (pMsg != NULL)
                {
                    PRINTF("send rx msg\n");//���ͽ��յ������ݵ����ս�����
                    tmos_memcpy(pMsg, rxBuf + 1, rxBuf[1] + 1);
                    tmos_msg_send(lwns_adapter_taskid, pMsg);

                }
                else
                {
                    PRINTF("send rx msg failed\n");//�����ڴ�ʧ�ܣ��޷����ͽ��յ�������
                }
            }
            else
            {
                PRINTF("bad len\n");//�����Ȳ���
            }
#endif
        }
        tmos_set_event(lwns_adapter_taskid, LWNS_PHY_RX_OPEN_EVT);//���´򿪽���
        break;
    }
    case TX_MODE_TX_FINISH:
    case TX_MODE_TX_FAIL:
        tmos_stop_task(lwns_phyoutput_taskid, LWNS_PHY_OUTPUT_FINISH_EVT);//ֹͣ��ʱ����
        tmos_set_event(lwns_phyoutput_taskid, LWNS_PHY_OUTPUT_FINISH_EVT);//���뷢����ɴ���
        break;
    default:
        break;
    }
}

/*********************************************************************
 * @fn      RF_Init
 *
 * @brief   RF ��ʼ��.
 *
 * @param   None.
 *
 * @return  None.
 */
void RF_Init(void)
{
    uint8_t state;
    rfConfig_t rfConfig;
    tmos_memset(&rfConfig, 0, sizeof(rfConfig_t));
    rfConfig.accessAddress = 0x17267162; // ��ֹʹ��0x55555555�Լ�0xAAAAAAAA ( ���鲻����24��λ��ת���Ҳ�����������6��0��1 )����ȷ������Ӧ�����accessaddress�����ַԼ��23�ڸ�
    rfConfig.CRCInit = 0x555555;
    ble_phy_channelmap_receive_seq = 0;
    rfConfig.Channel = ble_phy_channelmap[0];
    rfConfig.LLEMode = LLE_MODE_BASIC | LLE_MODE_PHY_1M; //|LLE_MODE_EX_CHANNEL; // ʹ�� LLE_MODE_EX_CHANNEL ��ʾ ѡ�� rfConfig.Frequency ��Ϊͨ��Ƶ��
    rfConfig.rfStatusCB = RF_2G4StatusCallBack;
    state = RF_Config(&rfConfig);
    PRINTF("rf 2.4g init: %x\n", state);
}

/*********************************************************************
 * @fn      lwns_init
 *
 * @brief   lwns��ʼ��.
 *
 * @param   None.
 *
 * @return  None.
 */
void lwns_init(void)
{
    uint8_t s;
    lwns_config_t cfg;
    tmos_memset(&cfg, 0, sizeof(lwns_config_t));
    cfg.lwns_lib_name = (uint8_t *) VER_LWNS_FILE; //��֤���������ƣ���ֹ�汾����
    cfg.qbuf_num = QBUF_MANUAL_NUM; //������䣬����1���ڴ浥λ�������������ʹ�õĶ˿�����Ӧģ��ʹ�õ�qbuf��λ�����塣
    cfg.qbuf_ptr = qbuf_memp; //mesh���ʹ��3��qbuf��λ��(uni/multi)netflood���ʹ��2��������ģ�鶼ʹ��1����
    cfg.routetable_num = ROUTE_ENTRY_MANUAL_NUM; //�����Ҫʹ��mesh���������·�ɱ��ڴ�ռ䡣��Ȼmesh��ʼ������ɹ���
#if ROUTE_ENTRY_MANUAL_NUM
    cfg.routetable_ptr = route_entry_memp;
#else
    cfg.routetable_ptr = NULL;
#endif
    cfg.neighbor_num = LWNS_NEIGHBOR_MAX_NUM; //�ھӱ��������������
    cfg.neighbor_list_ptr = neighbor_memp; //�ھӱ��ڴ�ռ�
    cfg.neighbor_mod = LWNS_NEIGHBOR_AUTO_ADD_STATE_RECALL_ADDALL; //�ھӱ��ʼ��Ĭ�Ϲ���ģʽΪ�������а�����������ھӲ��ҹ����ظ�����ģʽ
#if LWNS_ADDR_USE_BLE_MAC
    FLASH_GetMACAddress(cfg.addr.v8); //����Ӳ����mac��ַ
#else
//���ж���ĵ�ַ
    uint8_t MacAddr[6] = {0, 0, 0, 0, 0, 2};
    tmos_memcpy(cfg.addr.v8, MacAddr, LWNS_ADDR_SIZE);
#endif
    s = lwns_lib_init(&ble_lwns_fuc_interface, &cfg); //lwns��ײ��ʼ��
    if (s)
    {
        PRINTF("%s init err:%d\n", VER_LWNS_FILE, s);
    }
    else
    {
        PRINTF("%s init ok\n", VER_LWNS_FILE);
    }
    lwns_adapter_taskid = TMOS_ProcessEventRegister(lwns_adapter_ProcessEvent);
    lwns_phyoutput_taskid = TMOS_ProcessEventRegister(lwns_phyoutput_ProcessEvent);
    tmos_start_reload_task(lwns_phyoutput_taskid, LWNS_HTIMER_PERIOD_EVT, MS1_TO_SYSTEM_TIME(LWNS_HTIMER_PERIOD_MS));
    tmos_memset(blemesh_phy_manage_list, 0, sizeof(blemesh_phy_manage_list));//������͹���ṹ��
    ble_phy_send_cnt = 0;//��շ��ʹ�������
    RF_Shut();
    RF_Rx(NULL, 0, USER_RF_RX_TX_TYPE, USER_RF_RX_TX_TYPE); //��RF���գ������Ҫ�͹��Ĺ����������ط��򿪡�
    tmos_start_task(lwns_adapter_taskid, LWNS_PHY_RX_CHANGE_CHANNEL_EVT, MS1_TO_SYSTEM_TIME(LWNS_MAC_PERIOD_MS));
}

/*********************************************************************
 * @fn      ble_phy_output
 *
 * @brief   lwns���ͺ����ӿ�
 *
 * @param   dataptr     - �����͵����ݻ���ͷָ��.
 * @param   len         - �����͵����ݻ��峤��.
 *
 * @return  TRUE if success, FALSE is failed.
 */
static BOOL ble_phy_output(uint8_t *dataptr, uint8_t len)
{
    uint8_t *pMsg, i;
    struct blemesh_mac_phy_manage_struct *p;
    for (i = 0; i < LWNS_MAC_SEND_PACKET_MAX_NUM; i++)
    {
        if (blemesh_phy_manage_list[i].data == NULL)
        {
            break; //Ѱ�ҵ���һ���յĽṹ�����ʹ�á�
        }
        else
        {
            if (i == (LWNS_MAC_SEND_PACKET_MAX_NUM - 1))
            {
                PRINTF("send failed!\n"); //�б����ˣ�����ʧ�ܣ�ֱ�ӷ��ء�
                return FALSE;
            }
        }
    }
#if LWNS_ENCRYPT_ENABLE
    pMsg = tmos_msg_allocate((((len + 1 + 15) & 0xf0) + 1 + 1)); //У��λ1λ���Ϻ��ٽ���16�ֽڶ��룬�洢���ͳ���+1����ʵ���ݳ���+1
#else
    pMsg = tmos_msg_allocate(len + 1); //�����ڴ�ռ�洢��Ϣ���洢���ͳ���+1
#endif
    if (pMsg != NULL)  //�ɹ�����
    {
        p = blemesh_phy_manage_list_head;
        if (p != NULL)
        {
            while (p->next != NULL)  //Ѱ�ҷ���������յ�
            {
                p = p->next;
            }
        }
#if LWNS_ENCRYPT_ENABLE
        //lwns buffer�ڲ�Ԥ�������ֽڣ��û���ֱ��ʹ��dataptr[len]���и�ֵ���ֽ�����
        dataptr[len] = dataptr[len - 1] ^ len;//У���ֽڽ�ȡ���һ���ֽںͳ��Ƚ���������㣬���ֽ���ͬport��һ���ģ�������Ӱ�졣��У��Ƚ��˷�ʱ�䣬���Բ�����
        pMsg[1] = len;//��ʵ���ݳ���ռһ�ֽڣ������ܣ�������������һ��У��
        pMsg[0] = lwns_msg_encrypt(dataptr, pMsg + 2, len + 1) + 1;//��ȡ���ݼ��ܺ�ĳ��ȣ�Ҳ������Ҫ���ͳ�ȥ���ֽ�������ʵ���ݳ��Ȳ�����
#else
        pMsg[0] = len;
        tmos_memcpy(pMsg + 1, dataptr, len);
#endif
        if (blemesh_phy_manage_list_head != NULL)
        {
            p->next = &blemesh_phy_manage_list[i];        //�������β���
        }
        else
        {
            blemesh_phy_manage_list_head = &blemesh_phy_manage_list[i];        //����Ϊ�գ���ڵ���Ϊͷ���
            tmos_set_event(lwns_phyoutput_taskid, LWNS_PHY_OUTPUT_PREPARE_EVT);//�������ͣ�����ͷ��㲻��Ҫ��������Ϊ�ڷ��������С�
        }
        blemesh_phy_manage_list[i].data = pMsg;        //����Ϣ
        blemesh_phy_manage_list[i].next = NULL;
        return TRUE;
    }
    else
    {
        PRINTF("send failed!\n");//�޷����뵽�ڴ棬���޷�����
    }
    return FALSE;
}

/*********************************************************************
 * @fn      lwns_adapter_ProcessEvent
 *
 * @brief   lwns adapter Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed.
 */
static uint16_t lwns_adapter_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if (events & LWNS_PHY_RX_CHANGE_CHANNEL_EVT)//�����������ͨ���¼�
    {
        ble_phy_channelmap_receive_seq++;
        if (ble_phy_channelmap_receive_seq >= sizeof(ble_phy_channelmap))
        {
            ble_phy_channelmap_receive_seq = 0;
        }
        RF_Shut();
        RF_SetChannel(ble_phy_channelmap[ble_phy_channelmap_receive_seq]);//�����Ը��ķ���ͨ��
        RF_Rx(NULL, 0, USER_RF_RX_TX_TYPE, USER_RF_RX_TX_TYPE); //���´򿪽���
        tmos_start_task(lwns_adapter_taskid, LWNS_PHY_RX_CHANGE_CHANNEL_EVT, MS1_TO_SYSTEM_TIME(LWNS_MAC_PERIOD_MS));
        return (events & (~(LWNS_PHY_RX_CHANGE_CHANNEL_EVT | LWNS_PHY_RX_OPEN_EVT)));//ֹͣ�����Ѿ���λ�ġ����ܻ�򿪽��յ�����
    }
    if (events & LWNS_PHY_RX_OPEN_EVT)   //���´򿪽����¼�
    {
        RF_Shut();
        RF_Rx(NULL, 0, USER_RF_RX_TX_TYPE, USER_RF_RX_TX_TYPE); //���´򿪽���
        return (events ^ LWNS_PHY_RX_OPEN_EVT);
    }
    if (events & SYS_EVENT_MSG)  //����򿪽��պ��ٴ�������
    {
        uint8_t *pMsg;
        if ((pMsg = tmos_msg_receive(lwns_adapter_taskid)) != NULL)
        {
            // Release the TMOS message,tmos_msg_allocate
            lwns_input(pMsg + 1, pMsg[0]); //�����ݴ���Э��ջ������
            tmos_msg_deallocate(pMsg); //���ͷ��ڴ棬�����ݴ���ǰ�ͷţ���ֹ���ݴ�������Ҫ�������ݣ����ڴ治����
            lwns_dataHandler(); //����Э��ջ�������ݺ���
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
    // Discard unknown events
    return 0;
}

/*********************************************************************
 * @fn      lwns_phyoutput_ProcessEvent
 *
 * @brief   lwns phyoutput Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed.
 */
static uint16_t lwns_phyoutput_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if (events & LWNS_HTIMER_PERIOD_EVT)
    {
        lwns_htimer_update(); //htimer���¡�
        return (events ^ LWNS_HTIMER_PERIOD_EVT);
    }
    if (events & LWNS_PHY_OUTPUT_PREPARE_EVT)  //׼�����͹���
    {
        uint8_t rand_delay;
        rand_delay = tmos_rand() % MS1_TO_SYSTEM_TIME(LWNS_MAC_SEND_DELAY_MAX_MS) + BLE_PHY_ONE_PACKET_MAX_625US;//����ӳ�
        ble_phy_channelmap_send_seq = 0;//����ͨ�����
        PRINTF("rand send:%d\n", rand_delay);
        tmos_start_task(lwns_phyoutput_taskid, LWNS_PHY_OUTPUT_FINISH_EVT, rand_delay);//����ӳٺ����̷���
        return (events ^ LWNS_PHY_OUTPUT_PREPARE_EVT);
    }
    if (events & LWNS_PHY_OUTPUT_FINISH_EVT)                      //�����������
    {
        if (ble_phy_channelmap_send_seq < sizeof(ble_phy_channelmap))
        {
            if (ble_phy_channelmap_send_seq == 0) //ֻ��Ҫ�ڿ�ʼ���͵�ʱ��ͣ����
            {
                tmos_stop_task(lwns_adapter_taskid, LWNS_PHY_RX_CHANGE_CHANNEL_EVT);//�����ڼ䣬ֹͣ�����л�����ͨ��
                tmos_clear_event(lwns_adapter_taskid, LWNS_PHY_RX_CHANGE_CHANNEL_EVT);//ֹͣ�����Ѿ���λ�ġ����ܻ�򿪽��յ�����
                tmos_clear_event(lwns_adapter_taskid, LWNS_PHY_RX_OPEN_EVT);//ֹͣ�����Ѿ���λ�ġ����ܻ�򿪽��յ�����
            }
            RF_Shut();
            RF_SetChannel(ble_phy_channelmap[ble_phy_channelmap_send_seq]);//�����Ը��ķ���ͨ��
            RF_Tx((uint8_t *)(blemesh_phy_manage_list_head->data + 1),
                  blemesh_phy_manage_list_head->data[0], USER_RF_RX_TX_TYPE,
                  USER_RF_RX_TX_TYPE);
            tmos_start_task(lwns_phyoutput_taskid, LWNS_PHY_OUTPUT_FINISH_EVT, MS1_TO_SYSTEM_TIME(LWNS_PHY_OUTPUT_TIMEOUT_MS));//��ʼ���ͳ�ʱ��������ֹ�������Ϸ��ͣ������޷���������
            ble_phy_channelmap_send_seq++;
        }
        else
        {
            //����ͨ��һ�η��ͽ�����
            ble_phy_send_cnt++;
            tmos_set_event(lwns_adapter_taskid, LWNS_PHY_RX_CHANGE_CHANNEL_EVT);//����ڼ��������
            if (ble_phy_send_cnt < LWNS_MAC_TRANSMIT_TIMES) //���ʹ�������
            {
                tmos_set_event(lwns_phyoutput_taskid, LWNS_PHY_OUTPUT_PREPARE_EVT);//��������
            }
            else
            {
                //ָ���������ͽ���
                ble_phy_send_cnt = 0;//��շ��ʹ�������
                tmos_msg_deallocate(blemesh_phy_manage_list_head->data); //�ͷ��ڴ�
                blemesh_phy_manage_list_head->data = NULL; //�ָ�Ĭ�ϲ���
                blemesh_phy_manage_list_head = blemesh_phy_manage_list_head->next; //����pop��ȥ������Ԫ��
                if (blemesh_phy_manage_list_head != NULL)
                {
                    tmos_set_event(lwns_phyoutput_taskid, LWNS_PHY_OUTPUT_PREPARE_EVT);//��������
                }
            }
        }
        return (events ^ LWNS_PHY_OUTPUT_FINISH_EVT);
    }
    if (events & SYS_EVENT_MSG)
    {
        uint8_t *pMsg;
        if ((pMsg = tmos_msg_receive(lwns_phyoutput_taskid)) != NULL)
        {
            // Release the TMOS message,tmos_msg_allocate
            tmos_msg_deallocate(pMsg); //�ͷ��ڴ�
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
    return 0;
}

/*********************************************************************
 * @fn      lwns_shut
 *
 * @brief   ֹͣlwns������������lwns_phyoutput_taskid��lwns_adapter_taskid��processEvent�е��á�
 *
 * @param   None.
 *
 * @return  None.
 */
void lwns_shut()
{
    uint8_t *pMsg;
    RF_Shut();//�ر�RF����
    while (blemesh_phy_manage_list_head != NULL)
    {
        /* ������л����д����͵���Ϣ */
        tmos_msg_deallocate(blemesh_phy_manage_list_head->data);
        blemesh_phy_manage_list_head->data = NULL;
        blemesh_phy_manage_list_head = blemesh_phy_manage_list_head->next;
    }
    /* ��ʱ�ط�ȫ����� */
    lwns_htimer_flush_all();
    tmos_stop_task(lwns_phyoutput_taskid, LWNS_HTIMER_PERIOD_EVT);//ֹͣHtimer����ʱ��
    tmos_clear_event(lwns_phyoutput_taskid, LWNS_HTIMER_PERIOD_EVT);
    tmos_stop_task(lwns_phyoutput_taskid, LWNS_PHY_OUTPUT_PREPARE_EVT);
    tmos_clear_event(lwns_phyoutput_taskid, LWNS_PHY_OUTPUT_PREPARE_EVT);
    tmos_stop_task(lwns_phyoutput_taskid, LWNS_PHY_OUTPUT_FINISH_EVT);
    tmos_clear_event(lwns_phyoutput_taskid, LWNS_PHY_OUTPUT_FINISH_EVT);
    while ((pMsg = tmos_msg_receive(lwns_adapter_taskid)) != NULL)
    {
        /* ������л������Ϣ */
        tmos_msg_deallocate(pMsg);
    }
    tmos_stop_task(lwns_adapter_taskid, LWNS_PHY_RX_CHANGE_CHANNEL_EVT);//ֹͣHtimer����ʱ��
    tmos_clear_event(lwns_adapter_taskid, LWNS_PHY_RX_CHANGE_CHANNEL_EVT);
    tmos_stop_task(lwns_adapter_taskid, LWNS_PHY_RX_OPEN_EVT);
    tmos_clear_event(lwns_adapter_taskid, LWNS_PHY_RX_OPEN_EVT);
    tmos_clear_event(lwns_adapter_taskid, SYS_EVENT_MSG);

    /* ��շ��ʹ������� */
    ble_phy_send_cnt = 0;
    /* ��շ������ */
    ble_phy_channelmap_send_seq = 0;
    /* ��ս���ͨ����� */
    ble_phy_channelmap_receive_seq = 0;
}

/*********************************************************************
 * @fn      lwns_start
 *
 * @brief   lwns��ʼ���У���ʹ��lwns_shut�����¿�ʼʱʹ�á�
 *
 * @param   None.
 *
 * @return  None.
 */
void lwns_start()
{
    RF_Shut();
    RF_SetChannel(ble_phy_channelmap[ble_phy_channelmap_receive_seq]);//�����Ը��ķ���ͨ��
    RF_Rx(NULL, 0, USER_RF_RX_TX_TYPE, USER_RF_RX_TX_TYPE); //��RF���գ������Ҫ�͹��Ĺ����������ط��򿪡�
    tmos_start_task(lwns_adapter_taskid, LWNS_PHY_RX_CHANGE_CHANNEL_EVT, MS1_TO_SYSTEM_TIME(LWNS_MAC_PERIOD_MS));
    tmos_start_reload_task(lwns_phyoutput_taskid, LWNS_HTIMER_PERIOD_EVT, MS1_TO_SYSTEM_TIME(LWNS_HTIMER_PERIOD_MS));
}

#endif  /* LWNS_USE_BLEMESH_MAC */


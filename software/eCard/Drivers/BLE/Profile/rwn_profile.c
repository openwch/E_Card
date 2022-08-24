/********************************** (C) COPYRIGHT *******************************
 * File Name          : rwn_profile.C
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2022/07/20
 * Description        : read write notify三通道profile

 *******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "CONFIG.h"
#include "rwn_profile.h"

/*********************************************************************
 * MACROS
 */
#define RWN_PROFILE_NOTIFY_VALUE_POS    8

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

const uint8_t rwn_profile_Serv_UUID[ATT_BT_UUID_SIZE] =
{
    LO_UINT16(RWN_PROFILE_SERVICE_UUID),
    HI_UINT16(RWN_PROFILE_SERVICE_UUID)
};

const uint8_t rwn_profile_Read_UUID[ATT_BT_UUID_SIZE] =
{
    LO_UINT16(RWN_PROFILE_READ_UUID),
    HI_UINT16(RWN_PROFILE_READ_UUID)
};

const uint8_t rwn_profile_Write_UUID[ATT_BT_UUID_SIZE] =
{
    LO_UINT16(RWN_PROFILE_WRITE_UUID),
    HI_UINT16(RWN_PROFILE_WRITE_UUID)
};

const uint8_t rwn_profile_Notify_UUID[ATT_BT_UUID_SIZE] =
{
    LO_UINT16(RWN_PROFILE_NOTIFY_UUID),
    HI_UINT16(RWN_PROFILE_NOTIFY_UUID)
};


/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static rwn_profile_CBs_t *rwn_profile_AppCBs = NULL;


/*********************************************************************
 * Profile Attributes - variables
 */
/******************************************************************************/
static const gattAttrType_t rwn_profile_service =
{
    ATT_BT_UUID_SIZE,
    rwn_profile_Serv_UUID
};

// Read Profile Characteristic Properties
static uint8_t rwn_profile_service_read_Props = GATT_PROP_READ;

// Read Characteristic Value
static uint8_t rwn_profile_service_read_value = 0;

// Read Profile Characteristic User Description
static uint8_t rwn_profile_service_read_UserDesp[20] = "Read Port\0";

// write Profile Characteristic Properties
static uint8_t rwn_profile_service_write_Props = GATT_PROP_WRITE;

// write Characteristic Value
static uint8_t rwn_profile_service_write_value = {0};

// write Profile Characteristic User Description
static uint8_t rwn_profile_service_write_UserDesp[] = "Write Port\0";

// Notify Profile Characteristic Properties
static uint8_t rwn_profile_service_notify_Props = GATT_PROP_NOTIFY;

// Notify Characteristic Value
static uint8_t rwn_profile_service_notify_value = 0;

// Notify Characteristic User Description
static uint8_t rwn_profile_service_notify_UserDesp[20] = "Notify Port\0";

static gattCharCfg_t rwn_profile_service_notify_Config[4];

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t rwn_profile_service_AttrTbl[] =
{
    // rwn Profile Service
    {
        { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
        GATT_PERMIT_READ, /* permissions */
        0, /* handle */
        (uint8_t *) &rwn_profile_service /* pValue */
    },

    // Read Characteristic Declaration
    {
        { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &rwn_profile_service_read_Props
    },

    // Read Characteristic Value
    {
        { ATT_BT_UUID_SIZE, rwn_profile_Read_UUID },
        GATT_PERMIT_READ,
        0,
        &rwn_profile_service_read_value
    },

    // Read Characteristic User Description
    {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        rwn_profile_service_read_UserDesp
    },


    // Write Characteristic Declaration
    {
        {ATT_BT_UUID_SIZE, characterUUID},
        GATT_PERMIT_READ,
        0,
        &rwn_profile_service_write_Props
    },

    // Write Characteristic Value
    {
        {ATT_BT_UUID_SIZE, rwn_profile_Write_UUID},
        GATT_PERMIT_WRITE,
        0,
        &rwn_profile_service_write_value
    },

    // Write Characteristic User Description
    {
        {ATT_BT_UUID_SIZE, charUserDescUUID},
        GATT_PERMIT_READ,
        0,
        rwn_profile_service_write_UserDesp
    },

    // Notify Characteristic Declaration
    {
        { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &rwn_profile_service_notify_Props
    },
    // Notify Characteristic Value
    {
        { ATT_BT_UUID_SIZE, rwn_profile_Notify_UUID },
        0,
        0,
        &rwn_profile_service_notify_value
    },

    // Notify Characteristic configuration
    {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 0,
        (uint8_t *) rwn_profile_service_notify_Config
    },

    // Notify Characteristic User Description
    {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 0, rwn_profile_service_notify_UserDesp
    },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t rwn_profile_Service_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr, uint8_t *pValue, uint16_t *pLen, uint16_t offset, uint16_t maxLen, uint8_t method);
static bStatus_t rwn_profile_Service_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr, uint8_t *pValue, uint16_t len, uint16_t offset, uint8_t method);

static void rwn_profile_Service_HandleConnStatusCB(uint16_t connHandle, uint8_t changeType);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
gattServiceCBs_t rwn_profile_ServiceCBs =
{
    rwn_profile_Service_ReadAttrCB,      // Read callback function pointer
    rwn_profile_Service_WriteAttrCB,     // Write callback function pointer
    NULL                                // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      rwn_profile_AddService
 *
 * @brief   Initializes the Simple Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t rwn_profile_AddService(uint32_t services)
{
    uint8_t status = SUCCESS;
    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg(INVALID_CONNHANDLE, rwn_profile_service_notify_Config);
    // Register with Link DB to receive link status change callback
    linkDB_Register(rwn_profile_Service_HandleConnStatusCB);

    if (services)
    {
        // Register GATT attribute list and CBs with GATT Server App
        status = GATTServApp_RegisterService(rwn_profile_service_AttrTbl,
                                             GATT_NUM_ATTRS(rwn_profile_service_AttrTbl),
                                             GATT_MAX_ENCRYPT_KEY_SIZE, &rwn_profile_ServiceCBs);
    }

    return (status);
}

/*********************************************************************
 * @fn      rwn_profile_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t rwn_profile_RegisterAppCBs(rwn_profile_CBs_t *appCallbacks)
{
    if (appCallbacks)
    {
        rwn_profile_AppCBs = appCallbacks;

        return (SUCCESS);
    }
    else
    {
        return (bleAlreadyInRequestedMode);
    }
}

/*********************************************************************
 * @fn          rwn_profile_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 *
 * @return      Success or Failure
 */
static bStatus_t rwn_profile_Service_ReadAttrCB(uint16_t connHandle,
        gattAttribute_t *pAttr, uint8_t *pValue, uint16_t *pLen, uint16_t offset,
        uint16_t maxLen, uint8_t method)
{
    bStatus_t status = SUCCESS;
    if (pAttr->type.len == ATT_BT_UUID_SIZE)
    {
        // 16-bit UUID
        uint16_t uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);
        switch (uuid)
        {
        case RWN_PROFILE_READ_UUID:
        {
            if (rwn_profile_AppCBs && rwn_profile_AppCBs->pfnrwn_profile_Read)
            {
                rwn_profile_AppCBs->pfnrwn_profile_Read(pValue, pLen);
            }
            break;
        }
        default:
            *pLen = 0;
            status = ATT_ERR_ATTR_NOT_FOUND;
            break;
        }
    }
    else
    {
        // 128-bit UUID
        *pLen = 0;
        status = ATT_ERR_INVALID_HANDLE;
    }

    return (status);
}

/*********************************************************************
 * @fn      rwn_profile_Service_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 *
 * @return  Success or Failure
 */
static bStatus_t rwn_profile_Service_WriteAttrCB(uint16_t connHandle,
        gattAttribute_t *pAttr, uint8_t *pValue, uint16_t len, uint16_t offset,
        uint8_t method)
{
    bStatus_t status = SUCCESS;

    // If attribute permissions require authorization to write, return error
    if (gattPermitAuthorWrite(pAttr->permissions))
    {
        // Insufficient authorization
        return (ATT_ERR_INSUFFICIENT_AUTHOR);
    }

    if (pAttr->type.len == ATT_BT_UUID_SIZE)
    {
        // 16-bit UUID
        uint16_t uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);
        switch (uuid)
        {
        case RWN_PROFILE_WRITE_UUID:
        {
            //Write the value
            if (status == SUCCESS)
            {
                if (rwn_profile_AppCBs && rwn_profile_AppCBs->pfnrwn_profile_Write)
                {
                    rwn_profile_AppCBs->pfnrwn_profile_Write(pValue, len);
                }
            }
            break;
        }
        case GATT_CLIENT_CHAR_CFG_UUID:
            status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len, offset, GATT_CLIENT_CFG_NOTIFY);
            break;
        default:
            // Should never get here! (characteristics 2 and 4 do not have write permissions)
            status = ATT_ERR_ATTR_NOT_FOUND;
            break;
        }
    }
    else
    {
        // 128-bit UUID
        status = ATT_ERR_INVALID_HANDLE;
    }

    return (status);
}

/*********************************************************************
 * @fn          rwn_profile_Notify
 *
 * @brief       Send a notification containing a heart rate
 *              measurement.
 *
 * @param       connHandle - connection handle
 * @param       pNoti - pointer to notification structure
 *
 * @return      Success or Failure
 */
bStatus_t rwn_profile_Notify(uint16_t connHandle, attHandleValueNoti_t *pNoti)
{
    uint16_t value = GATTServApp_ReadCharCfg(connHandle, rwn_profile_service_notify_Config);

    // If notifications enabled
    if (value & GATT_CLIENT_CFG_NOTIFY)
    {
        // Set the handle
        pNoti->handle = rwn_profile_service_AttrTbl[RWN_PROFILE_NOTIFY_VALUE_POS].handle;

        // Send the notification
        return GATT_Notification(connHandle, pNoti, FALSE);
    }
    return bleIncorrectMode;
}

/*******************************************************************************
* Function Name  : rwn_profile_SendData
* Description    : rwn_profile_通道发送数据
* Input          : paramID：OTA通道选择
                   p_data：数据指针
                   send_len：发送数据长度
* Output         : None
* Return         : 函数执行状态
*******************************************************************************/
bStatus_t rwn_profile_SendData(unsigned char paramID, unsigned char *p_data, unsigned char send_len)
{
    bStatus_t status = SUCCESS;

    /* 数据长度超出范围 */
    if (send_len > 20)
    {
        return 0xfe;
    }
    return status;
}

/*********************************************************************
 * @fn          rwn_profile_Service_HandleConnStatusCB
 *
 * @brief       Simple Profile link status change handler function.
 *
 * @param       connHandle - connection handle
 * @param       changeType - type of change
 *
 * @return      none
 */
static void rwn_profile_Service_HandleConnStatusCB(uint16_t connHandle, uint8_t changeType)
{
    // Make sure this is not loopback connection
    if (connHandle != LOOPBACK_CONNHANDLE)
    {
        // Reset Client Char Config if connection has dropped
        if ((changeType == LINKDB_STATUS_UPDATE_REMOVED)      ||
                ((changeType == LINKDB_STATUS_UPDATE_STATEFLAGS) &&
                 (!linkDB_Up(connHandle))))
        {
            GATTServApp_InitCharCfg(connHandle, rwn_profile_service_notify_Config);
        }
    }
}

/*********************************************************************
 *********************************************************************/

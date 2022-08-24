/********************************** (C) COPYRIGHT *******************************
* File Name          : rwn_profile.h
* Author             : WCH
* Version            : V1.0
* Date               : 2022/07/20
* Description        :

*******************************************************************************/

#ifndef _RWN_PROFILE_H_
#define _RWN_PROFILE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

#define RWN_PROFILE_SERVICE_UUID        0xfba0
#define RWN_PROFILE_READ_UUID           0xfba1
#define RWN_PROFILE_WRITE_UUID          0xfba2
#define RWN_PROFILE_NOTIFY_UUID         0xfba3

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

typedef void (*rwn_profile_Read_t)(uint8_t *pValue, uint16_t *pLen);
typedef void (*rwn_profile_Write_t)(uint8_t *pValue, uint16_t len);

typedef struct
{
    rwn_profile_Read_t           pfnrwn_profile_Read;
    rwn_profile_Write_t          pfnrwn_profile_Write;
} rwn_profile_CBs_t;



/*********************************************************************
 * API FUNCTIONS
 */


/*
 * rwn_profile_AddService- Initializes the Simple GATT Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */

extern bStatus_t rwn_profile_AddService(uint32_t services);

/*
 * rwn_profile_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t rwn_profile_RegisterAppCBs(rwn_profile_CBs_t *appCallbacks);


bStatus_t rwn_profile_Notify(uint16_t connHandle, attHandleValueNoti_t *pNoti);

bStatus_t rwn_profile_SendData(unsigned char paramID, unsigned char *p_data, unsigned char send_len);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif

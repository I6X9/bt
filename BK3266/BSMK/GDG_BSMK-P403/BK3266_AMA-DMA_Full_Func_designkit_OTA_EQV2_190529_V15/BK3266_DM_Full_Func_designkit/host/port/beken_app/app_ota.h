#ifndef _APP_OTA_H_
#define _APP_OTA_H_

#include "config.h"
#include "types.h"

#ifdef BEKEN_OTA

u_int16 App_Setup_OAD_GATT_DB(void);
void app_ota_erase_flash(uint8 half);
void app_ota_write_flash(void);

#ifdef BEKEN_OTA_BLE
void app_ota_ble_pdu_decode(uint16 handle, uint8 *pValue, uint16_t length);
#endif

#ifdef BEKEN_OTA_SPP
void app_ota_spp_pdu_reframe(uint8 *pValue, uint16_t length);
#endif

#endif
void app_ota_up_action(uint16 handle,uint8 *result,u_int16 len);
uint8 app_ota_is_ongoing(void);
#endif

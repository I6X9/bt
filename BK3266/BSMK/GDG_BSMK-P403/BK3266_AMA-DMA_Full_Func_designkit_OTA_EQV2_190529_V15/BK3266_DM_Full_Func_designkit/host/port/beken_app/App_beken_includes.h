#ifndef _APP_BEKEN_INCLUDES_H_
#define _APP_BEKEN_INCLUDES_H_

#include "types.h"
#include <config/config.h>
#include <jos.h>
#include <bluetooth.h>
#include <bluetooth\core\hci_internal.h>
#include <bt_a2dp_sink.h>
#include <bluetooth\bt_sco_backend_utils.h>
#include "app_sbc.h"
#include "Mp3dec.h"

#include "app_protocol.h"
#include "app_env.h"
#include "app_debug.h"
#include "app_aec.h"
#include "app_hfp.h"
#include "app_equ.h"

#include "app_player.h"
#include "driver_audio.h"
#include "app_linein.h"

#include "app_exception.h"
#include "app_button.h"

#include "app_msg.h"
#include "app_prompt_wav.h"
#include "audio_out_interface.h"
#include "app_bt.h"

#include "driver_flash.h"
#include "bt_app_internal.h"
#include "app_customer.h"



#if (USER_EX_EQ == 1)
#include "leadelity.h"
#endif


#ifdef USER_AUX_EQ
extern volatile uint8 aux_eq_deal;
extern volatile uint8 aux_adc_enable;
extern aux_ptr_t aux_ptr;
#endif



extern volatile uint32 BK3000_AUD_AUDIO_CONF_BAK;
#ifdef CONFIG_TWS
//extern uint32 audio_sync[3];

#endif

/* current transaction id. */
#define TID_A2DP_PROTOCOL_SSA_REQUEST   0x7000
#define TID_HFP_PROTOCOL_SSA_REQUEST    0x7001


extern volatile uint8 flag_sys_ledstate;
extern volatile uint8 flag_sys_powerdown;
extern volatile uint8 flag_ble_enable;
extern volatile uint8 flag_aili_sco_end;
extern volatile uint8 flag_aili_sco_active;
extern volatile uint8 flag_not_autoconn_ble_enable;

#if (CONFIG_XIAONIU_DEVICE_TEST == 1)

extern volatile uint8 flag_os_printf_disable;
extern volatile uint8 user_batt_charger_vlcf;
extern volatile uint16 eeprom_block_num;


extern void rf_test_user_set_reg(void);
extern void rf_test_user_clear_reg(void);
extern uint32 rf_test_user_read_reg(void);
extern void User_Eeprom_Vlcf_Read(void);
extern void User_Eeprom_Vlcf_Write(uint8 vlcf);
extern void User_Eeprom_Btaddr_Write(uint8 * Pbtaddr);
extern void User_Eeprom_Btaddr_Read(uint8 * Pbtaddr);
extern void User_Eeprom_CRC_Read(uint8 * Pcrc);
extern void flash_write_data_user (uint8 *buffer, uint32 address, uint32 len);

#endif



#endif

//EOF

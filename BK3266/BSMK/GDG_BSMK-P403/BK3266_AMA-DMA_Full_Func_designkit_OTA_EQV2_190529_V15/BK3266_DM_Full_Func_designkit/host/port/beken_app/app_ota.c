#include "config.h"

#include "app_ota.h"
#include "app_beken_includes.h"
#include "lmp_utils.h"
#include "le_connection.h"
#if(PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
#include "le_att.h"
#include "le_gatt_server.h"
#endif
#if(PRH_BS_CFG_SYS_LE_GAP_INCLUDED==1)
#include "le_gap.h"
#include "le_gap_const.h"
#endif
#include "le_config.h"
#ifdef BEKEN_OTA
#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

#define PSEC_40_CRC_IMAGE_A_ADDR          0x1000 
#define PSEC_40_CRC_IMAGE_B_ADDR          0x100000

#ifdef BEKEN_OTA_BLE
// OTA attribute handle
#define OTA_IDENTIFY_BLE_HANDLE	          0x1002
#define OTA_BLOCK_BLE_HANDLE	          0x1006

// OTA 16bit UUID
#define OTA_SERVICE_UUID                  0xFFC0
#define OTA_IDENTIFY_UUID                 0xFFC1
#define OTA_BLOCK_UUID                    0xFFC2

// TI Base 128-bit UUID format: F000XXXX-0451-4000-B000-000000000000
#define TI_BASE_UUID_128( uuid )  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xB0,0x00,0x40,0x51,0x04,LO_UINT16(uuid),HI_UINT16(uuid),0x00,0xF0

// OTA 128bit UUID
static uint8 ota_service_uuid[16]  = {TI_BASE_UUID_128(OTA_SERVICE_UUID)};
static uint8 ota_identify_uuid[16] = {TI_BASE_UUID_128(OTA_IDENTIFY_UUID)};
static uint8 ota_block_uuid[16]    = {TI_BASE_UUID_128(OTA_BLOCK_UUID)};

// OTA Characteristic Value
static uint8 ota_identify_char_val[25];
static uint8 ota_block_char_val[25];

// OTA Characteristic Config
static uint8 ota_identify_config[2] = {0x00, 0x00};
static uint8 ota_block_config[2]    = {0x00, 0x00};

// OTA Characteristic user description
static uint8 ota_identify_desc[] = "Img Identify";
static uint8 ota_block_desc[] = "Img Block";
#endif

#ifdef BEKEN_OTA_SPP
// OTA spp handle
#define OTA_SPP_HEADER                    0x858D
#define OTA_IDENTIFY_SPP_HANDLE           0x1001
#define OTA_BLOCK_SPP_HANDLE	          0x1005

// OTA spp variable
uint8 ota_spp_break_frame[24] = {0};
uint8 ota_spp_break_frame_num = 0;
#endif

typedef struct
{
    uint32_t crc;        
  	uint16_t ver;
	uint16_t len;       
	uint32_t  uid;       
	uint8_t  crc_status;     
	uint8_t  sec_status;   
	uint16_t  rom_ver;     
}__PACKED_POST__ img_hdr_t;

typedef struct
{
    uint32 crc;
    uint32 data_addr;
    uint32 data_offset; 
    uint8 data[512];
    uint16 data_len;
    uint16 frame_num;
    uint16 total_num;
    uint8 update_flag;
    uint8 flash_protect_flag;
}__PACKED_POST__ app_ota_param_s;

app_ota_param_s app_ota_param = {0,0,0,{0},0,0,0,0,0};

extern unsigned char spp_is_connected(void);
extern result_t spp_send( char *buff, uint8_t len );
extern u_int8 get_active_sw_section(void);
extern u_int16 get_software_version(u_int8 half);
extern void set_flash_protect(uint8 all);

void app_ota_finished(uint8 status);
#ifdef BEKEN_OTA
#define OADIMGIDENTIFY_BLE_HANDLE	   0x1002
uint8 otastate = 0;
uint8 app_hand[3] = {0x22,0x11,0x01};
uint8 bt_rand[5] = {0x22,0x11,0x02,0xff,0xff};
uint8 app_randxor[5] = {0x22,0x12,0x01,0xff,0xff};
uint8 bt_affirm[4] = {0x22,0x12,0x02,0x55};
uint8 ver_get[3] = {0x22,0x13,0x01};
uint8 ver_send[] = {0x22,0x13,0x02,'X','Z','X','_','V','1','0','0','1'};
uint8 addr_send[3+6] = {0x22,0x14,0x02,0xff,0xff,0xff,0xff,0xff,0xff};
void app_ota_up_action(uint16 handle,uint8 *result,u_int16 len)
{
	app_env_handle_t env_h = app_env_get_handle();
	 uint8 nn;
	 	
	if(OTA_IDENTIFY_BLE_HANDLE==handle)
	{
		//{
		//   os_printf("rev=");
		//   for(nn=0;nn<len;nn++)
		//   	os_printf(" %x",result[nn]);
		//	os_printf("\r\n");
	 	//}
	
		if((result[0]==app_hand[0])&&(result[1]==app_hand[1])&&(result[2]==app_hand[2])&&(len==3))
		{
			otastate = 1;
			otaupflag = 0;
			bt_rand[3] = (os_get_tick_counter()&0xff);
			bt_rand[4] = (os_get_tick_counter()&0xff00)>>8;
			app_randxor[3] = (bt_rand[3]^36)+15;
			app_randxor[4] = (bt_rand[4]^63)+12;
			if (spp_is_connected())
			{
				os_printf("bt spp send rand\r\n");
				spp_send(bt_rand,sizeof(bt_rand));
			}
			else if(LEconnection_LE_Connected())
			{
				os_printf("bt ble send rand\r\n");
				GATTserver_Characteristic_Write_Local_Value(OADIMGIDENTIFY_BLE_HANDLE, sizeof(bt_rand), bt_rand);
			}
		}
	//}
	else if(otastate==1)
	{
		if((result[0]==app_randxor[0])&&(result[1]==app_randxor[1])&&(result[2]==app_randxor[2])
			&&(result[3]==app_randxor[3])&&(result[4]==app_randxor[4])&&(len==5))
		{
			otastate = 2;
			if (spp_is_connected())
			{
				os_printf("bt spp send affirm\r\n");
				spp_send(bt_affirm,sizeof(bt_affirm));
			}
			else if(LEconnection_LE_Connected())
			{
				os_printf("bt ble send affirm\r\n");
				GATTserver_Characteristic_Write_Local_Value(OADIMGIDENTIFY_BLE_HANDLE, sizeof(bt_affirm), bt_affirm);
			}
			
			if(!spp_is_connected())
			{
				os_printf("a2dp_cmd_disconnect\r\n");
				a2dp_cmd_disconnect(); 
			}
		}
	}
	else if(otastate==2)
	{
		if((result[0]==ver_get[0])&&(result[1]==ver_get[1])&&(result[2]==ver_get[2])&&(len==3))
		{
			otastate = 0;
			addr_send[3]=env_h->env_cfg.bt_para.device_addr.b[5];
			addr_send[4]=env_h->env_cfg.bt_para.device_addr.b[4];
			addr_send[5]=env_h->env_cfg.bt_para.device_addr.b[3];
			addr_send[6]=env_h->env_cfg.bt_para.device_addr.b[2];
			addr_send[7]=env_h->env_cfg.bt_para.device_addr.b[1];
			addr_send[8]=env_h->env_cfg.bt_para.device_addr.b[0];
			if (spp_is_connected())
			{
				os_printf("bt spp send ver\r\n");
				spp_send(ver_send,sizeof(ver_send));
				os_printf("bt spp send addr\r\n");
				spp_send(addr_send,sizeof(addr_send));
			}
			else if(LEconnection_LE_Connected())
			{
				os_printf("bt ble send ver\r\n");
				GATTserver_Characteristic_Write_Local_Value(OADIMGIDENTIFY_BLE_HANDLE, sizeof(ver_send), ver_send);
				os_printf("bt ble send addr\r\n");
				GATTserver_Characteristic_Write_Local_Value(OADIMGIDENTIFY_BLE_HANDLE, sizeof(addr_send), addr_send);

			}
			otaupflag = 1;
		       flag_ota_enable = 1;
    			flag_ble_enable = 1;
		}
	}
	}
}
#endif


/*****************************add BLE service begin***********************************/
#ifdef BEKEN_OTA_BLE
u_int16 App_Setup_OAD_GATT_DB(void)
{
    u_int16 handle = 0x1000;

    // OTA Service
    _GATTserver_Add_Service(PRIMARY_SERVICE_128BIT, handle, ota_service_uuid, OTA_SERVICE_UUID);
    handle++;


    // OTA Identify Characteristic Declaration
    _GATTserver_Add_16ByteUUID_Characteristic_Declaration(handle, ota_identify_uuid, GATT_PROPERTIES_WRITE_WITHOUT_RESPONSE|GATT_PROPERTIES_WRITE|GATT_PROPERTIES_NOTIFY);
    handle++;

    // OTA Identify Characteristic Value
    _GATTserver_Add_16ByteUUID_Characteristic_Value(handle, ota_identify_uuid, USE_LOCAL_NOTIFY, GATT_PERMISSION_WRITE, sizeof(ota_identify_char_val)/sizeof(ota_identify_char_val[0]), ota_identify_char_val);
    handle++;

    // OTA Identify Characteristic Configuration
    _GATTserver_Add_Char_Descriptor(handle, CLIENT_CHARACTERISTIC_CONFIGURATION, GATT_PERMISSION_READ | GATT_PERMISSION_WRITE, sizeof(ota_identify_config)/sizeof(ota_identify_config[0]), ota_identify_config);
    handle++;

    // OTA Identify Characteristic Description
    _GATTserver_Add_Char_Descriptor(handle, CHARACTERISTIC_USER_DESCRIPTION, GATT_PERMISSION_READ, sizeof(ota_identify_desc)/sizeof(ota_identify_desc[0]), ota_identify_desc);
    handle++;

    // OTA Block Characteristic Declaration
    _GATTserver_Add_16ByteUUID_Characteristic_Declaration(handle, ota_block_uuid, GATT_PROPERTIES_WRITE_WITHOUT_RESPONSE|GATT_PROPERTIES_WRITE|GATT_PROPERTIES_NOTIFY);
    handle++;

    // OTA Block Characteristic Value
    _GATTserver_Add_16ByteUUID_Characteristic_Value(handle, ota_block_uuid, USE_LOCAL_NOTIFY, GATT_PERMISSION_WRITE, sizeof(ota_block_char_val)/sizeof(ota_block_char_val[0]), ota_block_char_val);
    handle++;

    // OTA Block Characteristic Configuration
    _GATTserver_Add_Char_Descriptor(handle, CLIENT_CHARACTERISTIC_CONFIGURATION, GATT_PERMISSION_READ | GATT_PERMISSION_WRITE, sizeof(ota_block_config)/sizeof(ota_block_config[0]), ota_block_config);
    handle++;

    // OTA Block Characteristic Description
    _GATTserver_Add_Char_Descriptor(handle, CHARACTERISTIC_USER_DESCRIPTION, GATT_PERMISSION_READ, sizeof(ota_block_desc)/sizeof(ota_block_desc[0]), ota_block_desc);
    handle++;

    return 0;
}
#endif
/*****************************add BLE service begin***********************************/

/*****************************calculate crc begin***********************************/
int make_crc32_table(uint32_t *p_crc32_table)
{
    uint32_t c;
    int i = 0;
    int bit = 0;
    for(i = 0; i < 256; i++)
    {
        c = (uint32_t)i;
        for(bit = 0; bit < 8; bit++)
        {
            if(c&1)
            {
                c = (c>>1)^(0xEDB88320);
            }
            else
            {
                c = c >> 1;
            }
        }
        p_crc32_table[i] = c;
    }
    return 0;
}

uint32_t make_crc32(uint32_t crc,unsigned char *string,uint32_t size, uint32_t *p_crc32_table)
{
    while(size--)
    {
        crc = (crc >> 8)^(p_crc32_table[(crc^*string++)&0xff]);
    }
    return crc;
}
/*****************************calculate crc end***********************************/

uint32_t app_ota_calc_crc(void)
{
    uint32_t i;
    uint8_t data[16];
    uint32_t read_addr;
    uint32_t calcuCrc = 0xFFFFFFFF;
    uint32_t crc32_table[256];
    
    read_addr = app_ota_param.data_addr;

    make_crc32_table(&crc32_table[0]);
    for (i = 0; i < (app_ota_param.total_num - 1); i++)
    {
        flash_read_data(data, read_addr, 16);
        calcuCrc = make_crc32(calcuCrc, data, 16, &crc32_table[0]);
        read_addr += 16;
    }
    
    os_printf("CRC addr: start = 0x%x, end = 0x%x\r\n", app_ota_param.data_addr, read_addr);

    return calcuCrc;
}

uint16 app_ota_end_result(void)
{
    uint8 i;
    uint32 crc = 0;
    uint16 result = 0;
    uint8 section_tag[32];

    for(i = 0; i < 32; i++)
        section_tag[i] = 0xFF;
        
    crc = app_ota_calc_crc();
    os_printf( "OTA end: crc = %08x, %08x\r\n", crc, app_ota_param.crc);

    if(crc == app_ota_param.crc)
    {
    	 app_ota_finished(1); // 1:SUCCESS 0:FAILED
        if(app_ota_param.data_addr == PSEC_40_CRC_IMAGE_B_ADDR)
        {
            section_tag[0] = 0x5a;
            section_tag[1] = 0xa5;
            flash_write_data(section_tag,PSEC_40_CRC_IMAGE_B_ADDR+0x20,32);     
            section_tag[0] = 0;
            section_tag[1] = 0;
            flash_write_data(section_tag,PSEC_40_CRC_IMAGE_A_ADDR+0x20,32); 
        }
        else
        {
            section_tag[0] = 0xa5;
            section_tag[1] = 0x5a;
            flash_write_data(section_tag,PSEC_40_CRC_IMAGE_A_ADDR+0x20,32); 
            section_tag[0] = 0;
            section_tag[1] = 0;
            flash_write_data(section_tag,PSEC_40_CRC_IMAGE_B_ADDR+0x20,32);     
        }
        //app_ota_finished(1); // 1:SUCCESS 0:FAILED
         set_flash_protect(1);
        result = 1;
        os_printf( "+++++++++++++++ OTA SUCCESS +++++++++++++++\r\n");
    }
    else
    {
    	 app_ota_finished(0); // 1:SUCCESS 0:FAILED
        if(app_ota_param.data_addr == PSEC_40_CRC_IMAGE_B_ADDR)
        {
            section_tag[0] = 0;
            section_tag[1] = 0xff;
            flash_write_data(section_tag,PSEC_40_CRC_IMAGE_B_ADDR+0x20,32); 
        }
        else
        {
            section_tag[0] = 0;
            section_tag[1] = 0xff;
            flash_write_data(section_tag,PSEC_40_CRC_IMAGE_A_ADDR+0x20,32); 
        }
        //app_ota_finished(0); // 1:SUCCESS 0:FAILED
        set_flash_protect(1);
        result = 0;
        os_printf( "--------------- OTA fail ---------------\r\n");
    }
    
    memset((uint8*)&app_ota_param, 0, sizeof(app_ota_param_s));
    
    return result;
}

uint8 app_ota_is_ongoing(void)
{
    return app_ota_param.update_flag; 
}
uint8 ota_erase_flash;
void app_ota_erase_flash(uint8 half)
{
    uint32 start_addr;
    uint32 end_addr;
    uint32 addr;
    os_printf("---Please Waiting...\r\n");
    if(half)
    {
        start_addr = 0x100000;
        end_addr = 0x200000;
    }
    else
    {
        start_addr = 0x1000;
        end_addr = 0x100000;
    }
    set_flash_protect(0);
    ota_erase_flash = 1;
    for(addr = start_addr; addr < end_addr ; )
    {
        flash_erase_sector(addr);
        addr+= 0x1000;
        CLEAR_WDT;
    }	
    set_flash_protect(1);
    ota_erase_flash = 0;
}

void app_ota_write_flash(void)
{
#ifdef BEKEN_OTA_BLE
    extern u_int8 le_mode;
    if(LEconnection_LE_Connected() && le_mode)
        return;
#endif

    if(app_ota_param.flash_protect_flag == 1)         //flash write unprotect
    {
        set_flash_protect(0);
        app_ota_param.flash_protect_flag = 0;
    }
    else if(app_ota_param.flash_protect_flag == 2)    //flash write protect
    {
        set_flash_protect(1);
        app_ota_param.flash_protect_flag = 0;
    }

    if(app_ota_is_ongoing())
    {
        if(app_ota_param.data_len && ((app_ota_param.data_len > 5) || (app_ota_param.frame_num == app_ota_param.total_num)))
        {
            flash_write_data (app_ota_param.data, app_ota_param.data_addr + app_ota_param.data_offset, app_ota_param.data_len*16);
            app_ota_param.data_offset += app_ota_param.data_len*16;
            app_ota_param.data_len = 0;

            if(app_ota_param.frame_num == app_ota_param.total_num)
            {
                app_ota_end_result();
            }
        }
    }
}

u_int8 app_ota_save_data(u_int8 *pValue )
{
    memcpy((uint8 *)app_ota_param.data + (app_ota_param.data_len*16), pValue + 2, 16);
    app_ota_param.data_len++;

    if(app_ota_param.data_len >= 32)
    {    	
        os_printf("app ota save data overflow!!!\r\n");
#ifdef BEKEN_OTA_SPP
        if(spp_is_connected())
        {
            app_ota_write_flash();
        }
#endif	

    }
    return 0;
}

void app_ota_pdu(uint16 handle, u_int16 param_len, u_int8 *param_ptr)
{
    if(spp_is_connected())
    {
        spp_send((char*)param_ptr, param_len);
    }
#ifdef BEKEN_OTA_BLE
    else if(LEconnection_LE_Connected())
    {
        if((handle == OTA_IDENTIFY_BLE_HANDLE)||(handle == OTA_BLOCK_BLE_HANDLE))
        {
            GATTserver_Characteristic_Write_Local_Value(handle, param_len, param_ptr);
        }
        else
        {
            os_printf("app_ota_pdu le handle error!!!");
        }
    }
#endif
    else
    {
        os_printf("app_ota_pdu error!!!");
    }
}

void app_ota_finished(uint8 status)
{
    uint16 handle = 0;
    uint8 pdu[10] = {0};
    uint16 pdu_len = 0;

    //os_printf("app_ota_finished\r\n");
#ifdef BEKEN_OTA_SPP
    if(spp_is_connected())
    {
        pdu[0] = LO_UINT16(OTA_SPP_HEADER);              //header
        pdu[1] = HI_UINT16(OTA_SPP_HEADER);
        pdu_len +=2;
        
        pdu[2] = LO_UINT16(OTA_IDENTIFY_SPP_HANDLE);     //handle
        pdu[3] = HI_UINT16(OTA_IDENTIFY_SPP_HANDLE);
        pdu_len +=2;

        pdu[4] = 0x01;                                   //length
        pdu[5] = 0x00;
        pdu_len +=2;

        pdu[6] = status;
        pdu_len +=1;
    }
    else
#endif
#ifdef BEKEN_OTA_BLE
    if(LEconnection_LE_Connected())
    {
        pdu[0] = status;
        pdu_len +=1;
        handle = OTA_IDENTIFY_BLE_HANDLE;
    }
#endif

    app_ota_param.flash_protect_flag = 2;    //flash write protcet
    
    app_ota_pdu(handle, pdu_len, pdu);
    //set_flash_protect(1);
}

void app_ota_block_resp(uint16 blkNum)
{
    uint16 handle = 0;
    uint8 pdu[10] = {0};
    uint16 pdu_len = 0;

    //os_printf("app_ota_block_resp\r\n");
#ifdef BEKEN_OTA_SPP
    if(spp_is_connected())
    {
        pdu[0] = LO_UINT16(OTA_SPP_HEADER);          //header
        pdu[1] = HI_UINT16(OTA_SPP_HEADER);
        pdu_len += 2;
        
        pdu[2] = LO_UINT16(OTA_BLOCK_SPP_HANDLE);    //handle
        pdu[3] = HI_UINT16(OTA_BLOCK_SPP_HANDLE);
        pdu_len += 2;

        pdu[4] = 0x02;
        pdu[5] = 0x00;
        pdu_len += 2;

        pdu[6] = LO_UINT16(blkNum);
        pdu[7] = HI_UINT16(blkNum);
        pdu_len += 2;
    }
    else
#endif
#ifdef BEKEN_OTA_BLE
    if(LEconnection_LE_Connected())
    {
        pdu[0] = LO_UINT16(blkNum);
        pdu[1] = HI_UINT16(blkNum);
        pdu_len += 2;
        
        handle = OTA_BLOCK_BLE_HANDLE;
    }
#endif

    app_ota_pdu(handle, pdu_len, pdu);
}

void app_ota_identify_resp(img_hdr_t *pImgHdr)
{
    uint16 handle = 0;
    uint8 pdu[20] = {0};
    uint16 pdu_len = 0;

    //os_printf("app_ota_identify_resp\r\n");
#ifdef BEKEN_OTA_SPP
    if(spp_is_connected())
    {
        pdu[0] = LO_UINT16(OTA_SPP_HEADER);             //header
        pdu[1] = HI_UINT16(OTA_SPP_HEADER);
        pdu_len += 2;
        
        pdu[2] = LO_UINT16(OTA_IDENTIFY_SPP_HANDLE);    //handle
        pdu[3] = HI_UINT16(OTA_IDENTIFY_SPP_HANDLE);
        pdu_len += 2;

        pdu[4] = 0x0A;                                  //length
        pdu[5] = 0x00;
        pdu_len += 2;

        pdu[6] = LO_UINT16(pImgHdr->ver);               //version
        pdu[7] = HI_UINT16(pImgHdr->ver);
        pdu_len += 2;

        pdu[8] = LO_UINT16(pImgHdr->len);               //length
        pdu[9] = HI_UINT16(pImgHdr->len);
        pdu_len += 2;

        pdu[10] = 'A';                                  //uuid
        pdu[11] = 'A';
        pdu[12] = 'A';
        pdu[13] = 'A';
        pdu_len += 4;

        pdu[14] = 0x66;                                 //rom_ver
        pdu[15] = 0x66;
        pdu_len += 2;
    }
    else
#endif
#ifdef BEKEN_OTA_BLE
    if(LEconnection_LE_Connected())
    {
        pdu[0] = LO_UINT16(pImgHdr->ver);
        pdu[1] = HI_UINT16(pImgHdr->ver);
        pdu_len += 2;

        pdu[2] = LO_UINT16(pImgHdr->len);
        pdu[3] = HI_UINT16(pImgHdr->len);
        pdu_len += 2;

        pdu[4] = 'A';
        pdu[5] = 'A';
        pdu[6] = 'A';
        pdu[7] = 'A';
	 pdu[8] = 0x06;
	 pdu[9] = 0x00;
        pdu_len = 10;
        
        handle = OTA_IDENTIFY_BLE_HANDLE;
    }
#endif
    app_ota_pdu(handle, pdu_len, pdu);
}

void app_ota_identify_handler(uint8 length ,uint8 *pValue )
{
    img_hdr_t rxHdr;
    img_hdr_t ImgHdr;
       
    //os_printf("app_ota_identify_handler\r\n");
	
    if(get_active_sw_section() == 0x41) // A section
    {
        ImgHdr.ver = get_software_version(0);
    }
    else
    {
        ImgHdr.ver = get_software_version(1);
    }

    if(length == 0x10)
    {
        rxHdr.ver = LMutils_Get_Uint16( pValue + 4);
        rxHdr.len = LMutils_Get_Uint16( pValue + 6);
        if (ImgHdr.ver !=  rxHdr.ver )
        {
            memset((uint8*)&app_ota_param, 0, sizeof(app_ota_param_s));
            app_ota_param.update_flag = 1;
            app_ota_param.flash_protect_flag = 1;         //flash write unprotect
            app_ota_block_resp(0);
            app_bt_write_sniff_link_policy(bt_sniff_get_handle_from_idx(0), 0);  //exit bt sniff mode
            os_printf("OTA header: total_num = %x, new_ver = %x, old_ver = %x\r\n", rxHdr.len, rxHdr.ver, ImgHdr.ver);
        }
        else
        {
            app_ota_identify_resp(&ImgHdr);
            os_printf("OTA version error!\r\n");
        }
    }
    else
    {
        os_printf("OTA get device version!\r\n");
#ifdef BEKEN_OTA_SPP    
        if(spp_is_connected()) //SPP
        {
            app_ota_identify_resp(&ImgHdr);
        }
        else
#endif
        {
#ifdef BEKEN_OTA_BLE
            app_ota_identify_resp(&ImgHdr);
#endif
        }
    }
}

void app_ota_block_handler(uint8 len, uint8 *pValue )
{
    uint16 blkNum = LMutils_Get_Uint16( pValue);
    if( blkNum == 0 )
    {
        uint16 ver = LMutils_Get_Uint16( pValue + 6);
        uint32 crc = LMutils_Get_Uint32(pValue + 2);
        uint16 blkTot = LMutils_Get_Uint16( pValue + 8 );

        os_printf("OTA initialize: frame_num = %x, total_num = %x, new_ver = %x\r\n", blkNum, blkTot, ver);
        
        // initialize ota parameter
        app_ota_param.crc = crc;
        app_ota_param.total_num = blkTot;
        if(get_active_sw_section() == 0x41)
        {
            app_ota_param.data_addr = PSEC_40_CRC_IMAGE_B_ADDR;
        }
        else
        {
            app_ota_param.data_addr = PSEC_40_CRC_IMAGE_A_ADDR;
        }    
    }
    
    if (app_ota_param.frame_num == blkNum)
    {
        if (app_ota_param.frame_num != 0)
        {
            app_ota_save_data(pValue);
        }
        app_ota_param.frame_num++;
    }
    else
    {     		
        app_ota_finished(0); // 1:SUCCESS 0:FAILED
        os_printf( "OTA exception:%x,%x!!!\r\n", blkNum, app_ota_param.frame_num);
    } 
}

#ifdef BEKEN_OTA_BLE
void app_ota_ble_pdu_decode(uint16 handle, uint8 *pValue, uint16_t length)
{
//if(flag_ota_enable)
    app_ota_up_action( handle,pValue, length);
    if(otaupflag == 0)
		return;
	
    switch(handle)
    {
        case OTA_IDENTIFY_BLE_HANDLE:
            app_ota_identify_handler(length, pValue);
            break;

        case OTA_BLOCK_BLE_HANDLE:
            app_ota_block_handler(length, pValue);
            break;
            
        default:
            break;
    }
}
#endif

#ifdef BEKEN_OTA_SPP
void app_ota_spp_pdu_decode(uint8 *pValue, uint16_t length)
{
    uint16_t i = 0;
    uint16 header = LMutils_Get_Uint16(pValue);

    for(i = 0; i < (length+23)/24; i++)
    {
        if(header == OTA_SPP_HEADER)
        {
            uint16 handle = LMutils_Get_Uint16(pValue + 2);
            uint16 len = LMutils_Get_Uint16(pValue + 4);
            uint8 *payload = (uint8 *)(pValue + 6);
            
            switch(handle)
            {
                case OTA_IDENTIFY_SPP_HANDLE:
                    app_ota_identify_handler(len, payload);
                    break;
                
                case OTA_BLOCK_SPP_HANDLE:
                    app_ota_block_handler(len, payload);
                    break;

                default :
                    break;
            }
        }
        pValue += 24;
    }
}

void app_ota_spp_pdu_reframe(uint8 *pValue, uint16_t length)
{
    volatile uint16_t i = 0;
    if((app_ota_param.frame_num)||((app_ota_param.frame_num == 0)&&(LMutils_Get_Uint32(pValue) == (OTA_SPP_HEADER|(OTA_BLOCK_SPP_HANDLE << 16)))))
    {
        do
        {
            if(ota_spp_break_frame_num == 0)
            {
                if(length%24)
                {
                    for(i = 0; i < (length%24); i++)
                    {
                        ota_spp_break_frame[i] = *(pValue+(length/24)*24+i);
                    }
                    ota_spp_break_frame_num = i;
                    length -= ota_spp_break_frame_num;
                }
            }
            else 
            {
                
                for(i = 0; i < (24 - ota_spp_break_frame_num); i++)
                {
                    ota_spp_break_frame[i+ota_spp_break_frame_num] = *(pValue + i); 
                }
                app_ota_spp_pdu_decode(ota_spp_break_frame, 24);
                ota_spp_break_frame_num = 0;
                pValue += i;
                length -= i;
            }
        }
        while(length%24);
    }
    app_ota_spp_pdu_decode(pValue, length);
}
#endif


#else
unsigned char app_ota_is_ongoing(void)
{
    return 0; 
}
#endif


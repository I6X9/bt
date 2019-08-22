#include "config.h"
#if(AMA_ENABLE==0)
#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"
#include "msg_pub.h"
#include "dma_audio.h"
#include "opus.h"

#include "dueros_dma.h"
#include "dma.pb-c.h"
#include "platform.h"





//#define PRODUCT_ID       "uGxRYGt4BxPOoETI6eMNQ8Diz5qwVdYx"
//#define PRODUCT_PASS     "BztwwvNkgI1mGArxPQuogAlKOnkiq20L"

#define PRODUCT_ID         "ocAKzby1xEly4b42iBzLKtYZ9aoFRfSa"
#define PRODUCT_PASS       "0hvrbxGb4jG1tVUcrGyZ8eWBzUPxHh5q"



#define DEV_NAME           "C-CHIP"
#define DEV_TYPE           "3P"
#define DEV_MANUFACT       "C-CHIP"
#define DEV_MODEL          "BK3266"
#define OTA_VION            "OTA-V12"

////////////////////////////////////////////////////////////////


extern uint8 voice_static;


typedef enum
{
	VOICE_RECORD_FREE =0,
	VOICE_RECORD_BUSY,
}voice_record_state;

uint16_t pcm_sample_col;
int16_t  pcm_sample_buffer[MAX_PCM_BUF_ROW][MAX_PCM_BUF_LEN];
uint8_t  pcm_index_in       = 0x0;
uint8_t  pcm_index_out      = 0x0;
uint8_t  voice_start_flag   = VOICE_RECORD_FREE;
uint8_t  tx_audio_frame_cnt = 0;

//Defines an array to store audio data that has been encoded
uint8_t encode_voice_data[TOTAL_BLOCK_NUM][EN_OPUS_NUM];
//reaord index
uint8_t pbuff_write = 0;
//read index
uint8_t pbuff_read = 0;

////////////////////////////////////////////////////////////////

MSG_QUE rev_dma_que;
volatile uint8 voice_upload_disable;
volatile uint8 f_opus_encoded=0;

uint8  call_num_flag=0;
uint8 call_delay_cout=0;

extern DUEROS_DMA dueros_dma;

extern uint8 sbc_disable;


extern void _Stack_Integrity_Check(void) ;

extern u_int8 user_define_notify_handle_rx;
extern u_int8 user_define_notify_handle_tx;
extern u_int8 LEconnection_LE_Connected(void);


OpusEncoder* gOpusEncoder = NULL;
OpusDecoder* gOpusDecoder = NULL;
//uint8 SAMPLE_ALIGN opus_tmp[16*1024];
void opus_init(void)
{
    int size;

	if(gOpusEncoder==NULL)
	{
		size = opus_encoder_get_size(1);	
		gOpusEncoder=(SAMPLE_ALIGN)jmalloc(size,0);

	     // memset(opus_tmp,0,sizeof(opus_tmp));
	    //  gOpusEncoder = (OpusEncoder*)opus_tmp;
		//os_printf("opus_encoder_get_size==%d\r\n",size);

		opus_encoder_init(gOpusEncoder, 16000, 1, OPUS_APPLICATION_RESTRICTED_LOWDELAY);  
		opus_encoder_ctl(gOpusEncoder, OPUS_SET_VBR_REQUEST,        0);
		opus_encoder_ctl(gOpusEncoder, OPUS_SET_BITRATE_REQUEST,    16000);
		opus_encoder_ctl(gOpusEncoder, OPUS_SET_SIGNAL_REQUEST,     OPUS_SIGNAL_VOICE);
		opus_encoder_ctl(gOpusEncoder, OPUS_SET_COMPLEXITY_REQUEST, 0);
	}
}



void opus_unint(void)
{
	if(gOpusEncoder!=NULL)
	{
		jfree(gOpusEncoder);
		gOpusEncoder=NULL;
	}
}






#if 0

 void en_opus(void)
{
opus_int32 res;
int i;


   uint16_t decoded_data[320];
   int frame_size;

app_env_handle_t env_h = app_env_get_handle();
app_handle_t app_h = app_get_sys_handler();
	
//  if(bt_flag1_is_set(APP_FLAG_A2DP_CONNECTION))
  	{
    //  os_printf("en_opus--start\r\n");
  //  _Stack_Integrity_Check();
	if(voice_start_flag)
 	{
		uint8_t delta;

		//Determines whether the ADC data to be encoded exists in the loop buffer.
		if(pcm_index_in >= pcm_index_out)
		{
			delta = pcm_index_in - pcm_index_out;
		}
		else
		{
			delta = MAX_PCM_BUF_ROW - pcm_index_out + pcm_index_in;
		}

		if(delta > 0)
		{
             uint8_t encoded_data[64];
		//gpio_config(9,1);
		//gpio_output(9, 1);
		opus_int32 res = opus_encode(gOpusEncoder, (const opus_int16*)pcm_sample_buffer[pcm_index_out], OPUS_SAMPLES_PER_FRAME, encoded_data, sizeof(encoded_data));

		//gpio_output(9, 0);
		os_printf("[OPUS]: %d\r\n",res);
		
             // for(i=0;i<64;i++)
		//os_printf("encoded_data[%d]==: %x\r\n",i,encoded_data[i]);

			if(++pcm_index_out > (MAX_PCM_BUF_ROW - 1))
			{
				pcm_index_out = 0;
			}

			//Store the data to be encoded into the loop buffer
			store_encode_data(encoded_data);




       frame_size = opus_decode(gOpusDecoder, (const unsigned char*)encoded_data, res, decoded_data, 320, 0);
	os_printf("opus_decoder_frame_size==%d\r\n",frame_size);

               aud_fill_buffer((uint8 *)&decoded_data[0],320*2);
			
		}
 	}

  	}
}

#endif

#if 1


uint8_t  SAMPLE_ALIGN encoded_data[64];
void en_opus(void)
{
	opus_int32 res;
	int i;
	app_env_handle_t env_h = app_env_get_handle();
	app_handle_t app_h = app_get_sys_handler();
	
	//if(bt_flag1_is_set(APP_FLAG_A2DP_CONNECTION))
  	{
    	//os_printf("en_opus--start\r\n");
  		//_Stack_Integrity_Check();
		if(voice_start_flag)
	 	{
			uint8_t delta;

			//Determines whether the ADC data to be encoded exists in the loop buffer.
			if(pcm_index_in >= pcm_index_out)
			{
				delta = pcm_index_in - pcm_index_out;
			}
			else
			{
				delta = MAX_PCM_BUF_ROW - pcm_index_out + pcm_index_in;
			}

			if(delta > 0)
			{
	             
			//gpio_config(9,1);
			//gpio_output(9, 1);
			opus_int32 res = opus_encode(gOpusEncoder, (const opus_int16*)pcm_sample_buffer[pcm_index_out], OPUS_SAMPLES_PER_FRAME, encoded_data, sizeof(encoded_data));

			//gpio_output(9, 0);
			//os_printf("[OPUS]: %d\r\n",res);
			
	          //   for(i=0;i<64;i++)
			//os_printf("encoded_data[%d]==: %x\r\n",i,encoded_data[i]);

				if(++pcm_index_out > (MAX_PCM_BUF_ROW - 1))
				{
					pcm_index_out = 0;
				}

				//Store the data to be encoded into the loop buffer
				store_encode_data(encoded_data);
			}
	 	}
  	}
}

#endif


/*******************************************************************************
 * Function: audio_isr
 * Description: audio_isr 
 * Input: void
 * Output: void
 * Return: void
 * Others: void
*******************************************************************************/
void audio_isr_get_data(int16 adc_data)////audio_isr call
{
	uint16_t data = 0;
	static uint16 ttt=0;
	
	//read adc fifo data until fifo is null.

	//  data = (uint16_t)(adc_data>>3);

	if(voice_start_flag==0)
		return ;

	data = (uint16_t)(adc_data);
 
	ttt++;
	if(ttt>2000)
	{
		ttt=0;
		//os_printf("audio_isr_get_data==%d\r\n",data);  
	}

#if 0
	if(data > 32767)
	{
		pcm_sample_buffer[pcm_index_in][pcm_sample_col]= data - 65536;
	}
	else
	{
		pcm_sample_buffer[pcm_index_in][pcm_sample_col]= data;
	}
#endif

	if(adc_data > 32767)
		adc_data = 32767;
	if(adc_data < -32767)
		adc_data = -32767;
	pcm_sample_buffer[pcm_index_in][pcm_sample_col]= adc_data;

	pcm_sample_col++;
	if(pcm_sample_col >(MAX_PCM_BUF_LEN - 1))
	{
		pcm_sample_col = 0;
		
		// os_printf("audio_isr_get_data==%x\r\n",data);  
		//send encode evt to app task
		app_send_encode_evt();
		
		if(++pcm_index_in > (MAX_PCM_BUF_ROW - 1))
		{
			pcm_index_in = 0;
		}
	}
}



#define ALI_MIC_ANA_GAIN		60
#define ALI_MIC_DIG_GAIN		55
extern uint8 le_connected_first;
/*******************************************************************************
 * Function: audio_start
 * Description: 
 * Input: void
 * Output: void
 * Return: void
 * Others: void
*******************************************************************************/
int  audio_start(void)
{
#if 1
	if(le_connected_first > 0)  //ble连接成功1s内不响应APP命令
	{
		return;
	}
    //app_set_led_event_action(LED_EVENT_LINEIN_PLAY);
		  	
	//if(voice_start_flag == VOICE_RECORD_FREE)
	{
		os_printf("!!!!!opus_init\r\n");

		#if (USER_EX_EQ == 1)
		leadelityExitProcessing();
		#endif
	
		voice_start_flag = VOICE_RECORD_BUSY;

		//if (bt_flag1_is_set(APP_FLAG_MUSIC_PLAY))
		sbc_mem_free();		

	    opus_init();
	    //aud_initial(44100, 2, 16 );

		//AILI SCO时使用独立的MIC模拟增益
		aud_mic_volume_set(ALI_MIC_ANA_GAIN);

#if (CONFIG_APP_TOOLKIT_5 == 1)
		BK3000_AUD_ADC_CONF0  = (((ALI_MIC_DIG_GAIN&0x3f)<<18)|(BK3000_AUD_ADC_CONF0&(~(0x3f<<18))));
#endif

		if(LEconnection_LE_Connected())
			sbc_disable=0;//1;
		else
			sbc_disable=0;
  
		encode_voice_init();
		pcm_sample_col = 0;
		pcm_index_in = 0;
		pcm_index_out = 0;
		BK3000_Ana_PLL_enable(120);
		//  BK3000_set_clock(3, 1);

		_Stack_Integrity_Check();
		os_printf("audio start\r\n");

		app_bt_sco_enable_wrap(0);
		voice_upload_disable=1;
		app_wave_file_play_start(APP_WAVE_FILE_ID_AILI_SCO_START);
		
		//voice_upload_disable=0;
		//app_bt_sco_enable_wrap(1);

		//bt_flag2_operate(APP_FLAG2_SW_MUTE,1);
		//sbc_mem_free();		
	}
#endif

//opus_test();

}



/*******************************************************************************
 * Function: audio_stop
 * Description: 
 * Input: void
 * Output: void
 * Return: void
 * Others: void
*******************************************************************************/
int audio_stop(void)
{
#if 1
	if(voice_start_flag == VOICE_RECORD_BUSY)
	{
	sbc_voice_reset();
	#ifdef CONFIG_DRIVER_ADC
	sdadc_enable(0);
	#endif
	pcm_index_in = 0;
	pcm_index_out = 0;
	sbc_disable=0;
	voice_upload_disable=1;

	//bt_flag2_operate(APP_FLAG2_SW_MUTE,0);
	os_printf("!!!!!opus_unint\r\n");
	voice_start_flag = VOICE_RECORD_FREE;
	#if (USER_EX_EQ == 1)
	opus_unint();
	#endif
	print_debug_status();

	#if (USER_EX_EQ == 1)
	user_leadelityEntryInit();
	modeSwitch(0);		//normal
	#endif

	print_debug_info();
	BK3000_set_clock(3, 1);
	os_printf("audio stop\r\n");

	app_set_led_event_action(LED_EVENT_CONN);
		}
#endif

}





 
 /*******************************************************************************
 * Function: read_encode_data
 * Description: read encoded data from loop buffer
 * Input: uint8_t*
 * Output: void
 * Return: uint8_t
 * Others: void
*******************************************************************************/
uint8_t  read_encode_data(uint8_t *buf)
{
	//Read 20 encode data from loop buffer to designated buffer 
	if(pbuff_write != pbuff_read)
	{
		memcpy(buf,encode_voice_data[pbuff_read],EN_OPUS_NUM);
		//Update the buffer index of the data 
		//(in fact, the index is between 0-79)
		pbuff_read = ((pbuff_read + 1 )% TOTAL_BLOCK_NUM);
		
		//uart_printf("read 0x%x\r\n",pbuff_read);
		return 1;
	}else
	{
		//uart_printf("buff empty!!0x%x\r\n",pbuff_read);
		return 0;
	} 
}


/*******************************************************************************
 * Function: store_encode_data
 * Description: store encoded data into loop buffer
 * Input: uint8_t*
 * Output: void
 * Return: uint8_t
 * Others: void
*******************************************************************************/
uint8_t store_encode_data(uint8_t *buf)
{
	uint8_t free_cnt;
	uint8_t status ;
	
	//Calculates the number of empty buffer in the circular queue (the 
	//data stored in this queue is encoded)
	if(pbuff_write >= pbuff_read)
	{
		free_cnt = (TOTAL_BLOCK_NUM - pbuff_write + pbuff_read);
	}
    else
	{
		free_cnt = pbuff_read - pbuff_write;
	}

	//UART_PRINTF("free cnt: %d\r\n", free_cnt);

	//If there are at least two empty buffer in the loop queue, the current 
	//encoded data will be stored in buffer. 
	if(free_cnt >= 2) 
	{
		memcpy(encode_voice_data[pbuff_write],buf,EN_OPUS_NUM);
		
		//Update the buffer index of the data 
		//(in fact, the index is between 0-79)
		pbuff_write = ((pbuff_write + 1 )% TOTAL_BLOCK_NUM);
		status = 1;
		//UART_PRINTF("buff write 0x%x,0x%x!!!\r\n",buf[0],pbuff_write);
	}else
	{
		status = 0;
	}
		
	return status;
}



/*******************************************************************************
 * Function: print_debug_info
 * Description: print debug info to UART
 * Input: uint8_t*
 * Output: void
 * Return: uint8_t
 * Others: void
*******************************************************************************/
void print_debug_info(void) //for test  debug
{
	os_printf("pbuff_write = %d,pbuff_read = %d\r\n",pbuff_write,pbuff_read);
} 


/*******************************************************************************
 * Function: encode_voice_init
 * Description:  encode variable init
 * Input: void
 * Output: void
 * Return: void
 * Others: void
*******************************************************************************/
void encode_voice_init(void) // for test debug
{
	os_printf("pbuff_write = %d,pbuff_read = %d\r\n",pbuff_write,pbuff_read);
	pbuff_read = 0;
	pbuff_write = 0;
} 


/*******************************************************************************
 * Function: app_send_encode_evt
 * Description:  send event to app_task and start encode.
 * Input: void
 * Output: void
 * Return: void
 * Others: void
*******************************************************************************/
void app_send_encode_evt(void)
{
	if (LEconnection_LE_Connected())
		msg_put(MSG_START_OPUS_ENCODE);

	f_opus_encoded = 1;	
}


/*******************************************************************************
 * Function: print_debug_status
 * Description:  
 * Input: void
 * Output: void
 * Return: void
 * Others: void
*******************************************************************************/
void print_debug_status(void)
{
}


/*******************************************************************************
 * Function: print_debug_status
 * Description:  
 * Input: void
 * Output: void
 * Return: void
 * Others: void
*******************************************************************************/
void app_ble_send_val(uint8_t byes,uint8_t *buf )
{

}

/*******************************************************************************
 * Function: print_debug_status
 * Description:  
 * Input: void
 * Output: void
 * Return: void
 * Others: void
*******************************************************************************/
void app_ble_ack_val(void )///////////////////手机接收完成回应
{
	static uint32 send_out=0;
	if(voice_start_flag)
	{			   
		// os_delay_ms(1);
		//  send_out++;
		// if(send_out>10)
		{
			// send_out=0;
			if(tx_audio_frame_cnt > 0)
			{
				tx_audio_frame_cnt--;
			}
		}
	}
}


/*******************************************************************************
 * Function: print_debug_status
 * Description:  
 * Input: void
 * Output: void
 * Return: void
 * Others: void
*******************************************************************************/
uint8_t SAMPLE_ALIGN send_buf[64];

void app_send_audio(void)
{
    uint8_t i;
	uint8_t empty_buf_count;

#if 1

	if(voice_start_flag&&(voice_upload_disable==0))
	{
		//if (bt_flag1_is_set(APP_FLAG_MUSIC_PLAY))
		//sbc_mem_free();	
	
		empty_buf_count = 4 - tx_audio_frame_cnt;
		if(empty_buf_count > 0)
		{
			for(i=0; i<empty_buf_count; i++)
			{
				if(read_encode_data(send_buf))
				{
					tx_audio_frame_cnt++;
					aud_data_callback(send_buf,EN_OPUS_NUM);           
				}
			}
		}
	}
#endif

     data_send_process_thread();
    // os_delay_ms(1);
     app_ble_ack_val();
}


/***********************************************************
	int (*sent_raw_audio_data)(uint8_t *buf, uint32_t len);
	//WORD32 (*receive_raw_aduio_data)();
	//WORD32 (*get_mic_raw_data)();
	int (*sent_raw_cmd)(uint8_t *buf,  uint32_t len);
	//WORD32 (*receive_raw_cmd)();
	int (*start_speech)(void);
	int (*stop_speech)(void);
	void (*get_setting)(SpeechSettings*);	
	void (*get_dev_info)(DeviceInformation *);
	void (*get_dev_config)(DeviceConfigration*);
	void (*set_state)(State *);
	void (*get_state)(uint32_t ,State *);
	void (*notify_speech_state)();
	int (*at_command)(const char *);
	int(*stop_clear)(void);


***********************************************************/
extern u_int8 user_define_notify_handle_rx;
extern u_int8 user_define_notify_handle_tx;

#define  BLE_MTU_SIZE  104


PROTOCOL_VER dma_version_c;
DUEROS_DMA_OPER  dma_oper_c;




void opus_dma_init(void)
{

#if(AMA_ENABLE==1)
	dma_version_c.magic=0xFE03;
#else
	dma_version_c.magic=0xFE04;
#endif

	dma_version_c.hVersion=0x01;
	dma_version_c.lVersion=0x01;

	dma_oper_c.sent_raw_audio_data=ble_send_audio;
	dma_oper_c.sent_raw_cmd	=ble_send_cmd;
	dma_oper_c.start_speech=audio_start;
	dma_oper_c.stop_speech=audio_stop;
#if 1
	dma_oper_c.get_setting=ble_get_setting;
	dma_oper_c.get_dev_info=ble_get_dev_info;	
	dma_oper_c.get_dev_config=ble_get_dev_config;
	dma_oper_c.get_pair_config=ble_get_pair_config;

	dma_oper_c.set_state	=ble_set_state;
	dma_oper_c.get_state=ble_get_state;
	dma_oper_c.notify_speech_state=ble_notify_speech_state;	
	dma_oper_c.at_command=ble_at_command;
	dma_oper_c.stop_clear=ble_stop_clear;	
#endif


	rev_dma_que.ReadCounter=0;
	rev_dma_que.WriteCounter=0;
	 memset( (uint8 *)rev_dma_que.Pool, 0, MAX_REV_QUE*REV_QUE_LEN);
	 memset( (uint8 *)rev_dma_que.pool_que_len, 0, MAX_REV_QUE*2);

	dueros_dma_init(&dma_version_c,&dma_oper_c);

}







int ble_send_audio(uint8_t *buf, uint32_t len)
{
	int8 ret=0;
	if(LEconnection_LE_Connected())
	{
		ret=GATTserver_Characteristic_Write_Local_Value(user_define_notify_handle_tx,len,buf);
		if(ret==0x09)
			return -1;
	}
	else if (spp_is_connected())
	{
		spp_send(buf,len);	
		return ret;
	}

	return ret;
}


/********************************************************



********************************************************/
int ble_send_cmd(uint8_t *buf, uint32_t len)
{
	int8 ret=0;
	if(LEconnection_LE_Connected())
	{
		ret=GATTserver_Characteristic_Write_Local_Value(user_define_notify_handle_tx,len,buf);
		if(ret==0x09)
			return -1;
	}
	else if (spp_is_connected())
	{
		spp_send(buf,len);	
		return ret;
	}

	return ret;
}


/************************************************************


**************************************************************/
void ble_get_setting(SpeechSettings*p)
{
   p->audio_profile=(AudioProfile)1;
   p->audio_format=(AudioFormat)2;
   p->audio_source=(AudioSource)0;
}



/***************************************************************

Transport dma_transport[]={TRANSPORT__BLUETOOTH_LOW_ENERGY,TRANSPORT__BLUETOOTH_RFCOMM};	
AudioFormat dma_audioformat[] = {AUDIO_FORMAT__PCM_L16_16KHZ_MONO,AUDIO_FORMAT__OPUS_16KHZ_32KBPS_CBR_0_20MS};	
void dma_get_dev_info(DeviceInformation * device_info)
{		if(device_info == NULL)		{			return ;		}		
char * inner_build_info ="12345678";		
device_info->firmware_version = inner_build_info;		
device_info->serial_number = DMA_SERIAL_NUM;		
device_info->name= DMA_DEVICE_NAME;		
device_info->device_type  = DMA_DEVICE_TYPE;		
device_info->manufacturer =DMA_MANUFACTURER;		
device_info->firmware_version = DMA_FIRMWARE_VERSION;		
device_info->software_version = DMA_SOFTWARE_VERSION;		
device_info->initiator_type  = INITIATOR_TYPE__TAP;		
device_info->n_supported_transports =( sizeof(dma_transport)/sizeof(Transport));		
device_info->supported_transports = dma_transport;		
device_info->n_supported_audio_formats = (sizeof(dma_audioformat)/sizeof(dma_audioformat));		
device_info->supported_audio_formats = dma_audioformat;	}


*************************************************************/



char SAMPLE_ALIGN addr[18];
char SAMPLE_ALIGN serial_num[7] ;
Transport SAMPLE_ALIGN dma_transport[]={TRANSPORT__BLUETOOTH_LOW_ENERGY,TRANSPORT__BLUETOOTH_RFCOMM};	
AudioFormat SAMPLE_ALIGN dma_audioformat[] = {AUDIO_FORMAT__OPUS_16KHZ_16KBPS_CBR_0_20MS};	

void ble_get_dev_info(DeviceInformation *p)
{
	app_env_handle_t  env_h = app_env_get_handle();

	memset(addr,0,18);
	sprintf(addr,BTADDR_FORMAT,
	env_h->env_cfg.bt_para.device_addr.b[5],
	env_h->env_cfg.bt_para.device_addr.b[4],
	env_h->env_cfg.bt_para.device_addr.b[3],
	env_h->env_cfg.bt_para.device_addr.b[2],
	env_h->env_cfg.bt_para.device_addr.b[1],
	env_h->env_cfg.bt_para.device_addr.b[0]);


	sprintf(serial_num, "%02x%02x%02x",
	env_h->env_cfg.bt_para.device_addr.b[2],
	env_h->env_cfg.bt_para.device_addr.b[1],
	env_h->env_cfg.bt_para.device_addr.b[0]);


	char * inner_build_info ="12345678";		
	p->firmware_version = inner_build_info;	

	p->serial_number=&serial_num[1] ;

	p->name=DEV_NAME ;         

	p->n_supported_transports =( sizeof(dma_transport)/sizeof(Transport));		
	p->supported_transports = dma_transport;		
	p->n_supported_audio_formats = (sizeof(dma_audioformat)/sizeof(dma_audioformat));		
	p->supported_audio_formats = dma_audioformat;	


	p->device_type=DEV_TYPE ;
	p->manufacturer=DEV_MANUFACT  ;     
	p->model=DEV_MODEL  ;         
	p->firmware_version=(   char  *)"1.0";
	p->software_version=(   char  *)"1.0";
	p->initiator_type=(InitiatorType)INITIATOR_TYPE__TAP;


	p->product_id=PRODUCT_ID ;     
	p->classic_bluetooth_mac=addr;////(   char  *)"11:22:33:44:55:66";//.addr;//
	p->disable_heart_beat=(protobuf_c_boolean)1;

/////////////////////////////////////////////////////
	p->enable_advanced_security=0;
	p->support_fm=0;//////suport fm  set 1    是否支持FM 发射
	p->ota_version=OTA_VION;
	//////////////////////////////only ble set 1
	p->no_a2dp=0;
	p->no_at_command=0;	

      //////////////////////////////////

	
	
}



void ble_get_dev_config(DeviceConfiguration*p)
{
	p->needs_assistant_override=(protobuf_c_boolean)0;
	p->needs_setup=(protobuf_c_boolean)0;
}

/*********************************************************




*********************************************************/

uint32 pair_request_id_nu = 0;
char SAMPLE_ALIGN	pair_char_request_id[9];//={"12345678"};
char SAMPLE_ALIGN	pair_char_request_id2[9];//={"12345678"};

char    SAMPLE_ALIGN pair_char_request_id_bak[90];
char   SAMPLE_ALIGN  sha256[65];

const char *p1=PRODUCT_PASS  ;  
const char *p2=PRODUCT_ID;     
const char *p3=&serial_num[1] ;
char *p4=NULL;


void ble_get_pair_config(PairInformation*p)
{
	p3=&serial_num[1] ;

	//pair_request_id_nu = rand();
	//if(pair_request_id_nu==0)
	//pair_request_id_nu=0x12345678;
	//sprintf(pair_char_request_id, "%x",pair_request_id_nu);
	dueros_get_random_string(9, &pair_char_request_id[0]);


	p->rand=pair_char_request_id;
	dueros_dma.rand=pair_char_request_id;
	os_printf("dueros_dma.rand=%s\r\n",dueros_dma.rand);

	strncpy(&pair_char_request_id_bak[0],pair_char_request_id,8);
	strncpy(&pair_char_request_id_bak[8],p1,32);
	strncpy(&pair_char_request_id_bak[8+32],p2,32);
	strncpy(&pair_char_request_id_bak[8+32+32],p3,5);
	p4= pair_char_request_id_bak;


	os_printf("p4=%s\r\n",p4);

	StrSHA256(p4,(8+32+32+5),sha256);//sign = sha256(rand + key + product_id + serial_number), 
	p->sign=sha256;

	os_printf(" p->sign=%s\r\n", p->sign);

	os_printf(" p->rand=%s\r\n", pair_char_request_id);
	p->signmethod=SIGN_METHOD__SHA256;
}



/*********************************************************


*********************************************************/

void dma_clear_rand(void)
{
	memset(pair_char_request_id,0,sizeof(pair_char_request_id));
	dueros_dma.rand=NULL;
}


/*********************************************************


*********************************************************/


uint8 dma_sign2(ControlEnvelope *p)
{
	uint8 value=0;
	p4=NULL;
	p3=&serial_num[1] ;

	strncpy(pair_char_request_id2,p->rand2,8);
	memset(pair_char_request_id_bak,0,sizeof(pair_char_request_id_bak));

	strncpy(&pair_char_request_id_bak[0],pair_char_request_id2,8);
	strncpy(&pair_char_request_id_bak[8],pair_char_request_id,8);
	strncpy(&pair_char_request_id_bak[16],p1,32);
	strncpy(&pair_char_request_id_bak[16+32],p2,32);
	strncpy(&pair_char_request_id_bak[16+32+32],p3,5);
	p4= pair_char_request_id_bak;

	//os_printf("p4=%s\r\n",p4);
	StrSHA256(p4,(16+32+32+5),sha256);//sign = sha256(rand + key + product_id + serial_number), 
	os_printf("sha256=%s\r\n",sha256);
	os_printf("sign2=%s\r\n",p->sign2);
	value=os_strncmp(sha256,p->sign2,64);
	os_printf("dma_result==%d",value);
	return value;
}


uint32 app_fm_preq;
void ble_set_state(State *p)	//FM频点获取
{
	app_fm_preq = p->integer;
	os_printf("app_fm_preq=%d\r\n",app_fm_preq);
}



void ble_get_state(uint32_t feature ,State *p)
{
	p->feature=feature;////1;//feature;
	p->value_case=STATE__VALUE_BOOLEAN;
	p->boolean=(protobuf_c_boolean)1;	

	//os_printf("feature==%d\r",p->feature);
	//os_printf("boolean==%d\r",p->boolean);
}




void ble_notify_speech_state(uint8 sta)    //返回APP状态
{
  	voice_static = sta;
  	os_printf("ble_notify_speech_state=%d\r\n",voice_static);
}



extern uint8 string_size(uint8 *ptr);
char call_id[32];

int ble_at_command(const char *p)
{
	uint8 j;

  	os_printf("at_com=%s\r\n",p);
	p=p+3;
	j = string_size((uint8*)p);	
	memset(&call_id[0],0,32);
       os_memcpy((uint8 *)call_id, (uint8 *)p, j);

	while (j)
	{
		j--;
		if ((call_id[j] >= 0x30) && (call_id[j] <= 0x39))
			break;

		call_id[j] = 0;
	}

  	os_printf("at_com=%s\r\n",call_id);

	call_num_flag=1;
	call_delay_cout=100;/////1s 后，打电话 
}

void call_num_start(void)
{
	if(call_num_flag)
	{  
		if(call_delay_cout==0)
		{
			call_num_flag=0;
			hf_cmd_place_call(call_id);
		}
	}
}


int ble_stop_clear(void)
{
}



/***************************************************************************



***************************************************************************/
#define SHA256_ROTL(a,b) (((a>>(32-b))&(0x7fffffff>>(31-b)))|(a<<b))
#define SHA256_SR(a,b) ((a>>b)&(0x7fffffff>>(b-1)))
#define SHA256_Ch(x,y,z) ((x&y)^((~x)&z))
#define SHA256_Maj(x,y,z) ((x&y)^(x&z)^(y&z))
#define SHA256_E0(x) (SHA256_ROTL(x,30)^SHA256_ROTL(x,19)^SHA256_ROTL(x,10))
#define SHA256_E1(x) (SHA256_ROTL(x,26)^SHA256_ROTL(x,21)^SHA256_ROTL(x,7))
#define SHA256_O0(x) (SHA256_ROTL(x,25)^SHA256_ROTL(x,14)^SHA256_SR(x,3))
#define SHA256_O1(x) (SHA256_ROTL(x,15)^SHA256_ROTL(x,13)^SHA256_SR(x,10))



SAMPLE_ALIGN CONST long KK[64] = 
{
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};




char* StrSHA256( const char* str, long long length, char* sha256)
{
    /*
    计算字符串SHA-256
    参数说明：
    str         字符串指针
    length      字符串长度
    sha256         用于保存SHA-256的字符串指针
    返回值为参数sha256
    */
    char *pp, *ppend;
    long l, i, W[64], T1, T2, A, B, C, D, E, F, G, H, H0, H1, H2, H3, H4, H5, H6, H7;
    H0 = 0x6a09e667, H1 = 0xbb67ae85, H2 = 0x3c6ef372, H3 = 0xa54ff53a;
    H4 = 0x510e527f, H5 = 0x9b05688c, H6 = 0x1f83d9ab, H7 = 0x5be0cd19;

    l = length + ((length % 64 >= 56) ? (128 - length % 64) : (64 - length % 64));
    if (!(pp = (char*)jmalloc((unsigned long)l,0))) return 0;
    for (i = 0; i < length; pp[i + 3 - 2 * (i % 4)] = str[i], i++);
    for (pp[i + 3 - 2 * (i % 4)] = 128, i++; i < l; pp[i + 3 - 2 * (i % 4)] = 0, i++);
    *((long*)(pp + l - 4)) = length << 3;
    *((long*)(pp + l - 8)) = length >> 29;
    for (ppend = pp + l; pp < ppend; pp += 64){
        for (i = 0; i < 16; W[i] = ((long*)pp)[i], i++);
        for (i = 16; i < 64; W[i] = (SHA256_O1(W[i - 2]) + W[i - 7] + SHA256_O0(W[i - 15]) + W[i - 16]), i++);
        A = H0, B = H1, C = H2, D = H3, E = H4, F = H5, G = H6, H = H7;
        for (i = 0; i < 64; i++){
            T1 = H + SHA256_E1(E) + SHA256_Ch(E, F, G) + KK[i] + W[i];
            T2 = SHA256_E0(A) + SHA256_Maj(A, B, C);
            H = G, G = F, F = E, E = D + T1, D = C, C = B, B = A, A = T1 + T2;
        }
        H0 += A, H1 += B, H2 += C, H3 += D, H4 += E, H5 += F, H6 += G, H7 += H;
    }
    jfree(pp - l);
    sprintf(sha256, "%08x%08x%08x%08x%08x%08x%08x%08x", H0, H1, H2, H3, H4, H5, H6, H7);
    return sha256;
}

/*******************************************************************************




********************************************************************************/
void put_que(uint16  len,uint8 * buf)
{
    rev_dma_que.pool_que_len[rev_dma_que.WriteCounter]=len;
    memcpy(rev_dma_que.Pool[rev_dma_que.WriteCounter],buf,len);
	
    rev_dma_que.WriteCounter++;
    if(rev_dma_que.WriteCounter == MAX_REV_QUE)
        rev_dma_que.WriteCounter = 0;  

}
/*******************************************************************************




********************************************************************************/
uint8 get_que(uint8 * buf)
{
    uint16 len=0;
   
    if(rev_dma_que.WriteCounter == rev_dma_que.ReadCounter)
    {
        return 0;
    }    
	
    len=rev_dma_que.pool_que_len[rev_dma_que.ReadCounter];
    memcpy(buf,rev_dma_que.Pool[rev_dma_que.ReadCounter],len);	
  
    rev_dma_que.ReadCounter++;
    if(rev_dma_que.ReadCounter == MAX_REV_QUE)
        rev_dma_que.ReadCounter = 0;    
	
    return len; 
}

void* my_malloc(unsigned int size)
{
	return jmalloc_s(size, 0);
}

void my_free(void* ptr)
{
	jfree_s(ptr);
}

#endif


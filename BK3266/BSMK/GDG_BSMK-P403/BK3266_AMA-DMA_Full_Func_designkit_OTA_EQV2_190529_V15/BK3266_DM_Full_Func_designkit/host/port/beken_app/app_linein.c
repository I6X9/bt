#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"
#include "msg_pub.h"

#include "bt_mini_sched.h"


#ifdef USER_AUX_EQ
volatile uint8 aux_eq_deal = 0;
volatile uint8 aux_adc_enable = 0;
aux_ptr_t aux_ptr;
#endif


volatile uint8 linePlay=0;
static linein_cfg_t linein_cfg;
volatile uint8_t linein_audio_flag = 0;
//extern RAM_CODE void Beken_Uart_Rx(void);
extern RAM_CODE t_error BTms_Sched(u_int32 num_interations);

#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE 
extern u_int8 syspwr_cpu_halt;
#endif

void linein_audio_open(void)
{
    if(linein_audio_flag) 
        return;

    os_printf("linein_audio_open\r\n");
    aud_PAmute_oper(1);
    os_delay_ms(20);
//#ifndef CONFIG_AUDIO_DAC_ALWAYSOPEN
//    if(!app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_DAC_ALWAYS_ON))
//        BK3000_Ana_Dac_enable(1);
//#endif
    BK3000_Ana_Line_enable(1);


    aud_volume_set(player_vol_bt);




    if((player_vol_bt==0)||(linePlay==0))
        aud_PAmute_oper(1);
    else
        aud_PAmute_oper(0);
    linein_audio_flag = 1;
}

void linein_audio_close(void)
{
    os_printf("linein_audio_close\r\n");
	//aud_PAmute_oper(1);
    //os_delay_ms(20);
//#ifndef CONFIG_AUDIO_DAC_ALWAYSOPEN
//    if(!app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_DAC_ALWAYS_ON))
//        BK3000_Ana_Dac_enable(0);
//#endif
    BK3000_Ana_Line_enable(0);
    linein_audio_flag = 0;
}

static inline void linein_attach_judge(void)
{
    if(LINEIN_STATUS_OFF == linein_cfg.status)
    {
        linein_cfg.plugin_count++;
        linein_cfg.pullout_count = 0;
    #ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
        app_exit_sniff_mode();
    #endif
        if( linein_cfg.plugin_count > LINEIN_GPIO_DEBOUNCE_COUNT )
        {
            linein_cfg.status = LINEIN_STATUS_ON;
            os_printf("line in insert\r\n");
        msg_put(MSG_LINEIN_ATTACH);
        }
    }	
}
static inline void linein_detach_judge(void)
{
    if(LINEIN_STATUS_ON == linein_cfg.status)
    {
        linein_cfg.pullout_count++;
        linein_cfg.plugin_count = 0;
    #ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
        app_exit_sniff_mode();
    #endif
        if(linein_cfg.pullout_count > ( LINEIN_GPIO_DEBOUNCE_COUNT-30) )
        {		   	
            linein_cfg.status = LINEIN_STATUS_OFF;
            os_printf("line in detach\r\n");
            //BK3000_AUD_FIFO_CONF &= ~(1 << sft_AUD_ADC_INT_EN);
            msg_put(MSG_LINEIN_DETACH);
        }
    }
}

static void linein_detect(void)
{
    uint32 linein_gpio_status;
    app_env_handle_t  env_h = app_env_get_handle();

    if(!(env_h->env_cfg.system_para.system_flag &APP_ENV_SYS_FLAG_AD_LINEIN_ENA))
    {
        linein_gpio_status = gpio_input(linein_cfg.pin);

        if(linein_cfg.high_flag == linein_gpio_status)//在线
        {
            linein_attach_judge();
        }
        else
        {
            linein_detach_judge();
        }
    }
    else
    {

    }
}

void app_linein_init(void)
{

    app_env_handle_t  env_h = app_env_get_handle();
	
    memset(&linein_cfg, 0, sizeof(linein_cfg_t));
	
    if(env_h->env_cfg.used == 0x01)
    {
        linein_cfg.enable = (env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_LINEIN_ENA) >> 3;
        linein_cfg.high_flag = (env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_LINEIN_HIGH) >> 4;
    #ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
        sniffmode_wakeup_dly = 1000;
    #endif
    }
    else
    {
        linein_cfg.enable = 0;//无配置表则无linein功能，避免默认引脚配置
        linein_cfg.high_flag = LINEIN_GPIO_LOW_VALID;
    }
    
    if(linein_cfg.enable)    
    {
        if(!(env_h->env_cfg.system_para.system_flag &APP_ENV_SYS_FLAG_AD_LINEIN_ENA))
        {
            linein_cfg.pin = env_h->env_cfg.system_para.linein_pin;

            if(LINEIN_GPIO_LOW_VALID == linein_cfg.high_flag)//low valid
                gpio_config(linein_cfg.pin,3);
            else
                gpio_config(linein_cfg.pin,0);
        }
        linein_cfg.detect_func = linein_detect;
        
        linein_cfg.status = LINEIN_STATUS_OFF;
        BK3000_Ana_Line_enable(0);//防止串音
        os_printf("app_linein_init pin:%d\r\n",linein_cfg.pin);

    }
}

int app_linein_powerup_check(void)
{
    int ret = 0;
    uint32 linein_gpio_status;
    /* app_env_handle_t env_h = app_env_get_handle(); */

    if(linein_cfg.enable)
    {
        linein_gpio_status = gpio_input(linein_cfg.pin);
        if(linein_cfg.high_flag == linein_gpio_status)
        {
            ret = 1;
        }
    }

    return ret;
}


void app_linein_enter(void *arg)
{   
    app_handle_t app_h = app_get_sys_handler();
#if defined(CONFIG_LINE_SD_SNIFF)
    bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,1);
#endif

#ifdef CONFIG_APP_EQUANLIZER
    app_equalizer_a2dp_apply();
#endif
    app_wave_file_play_stop();
    app_h->flag_sm1 |= APP_FLAG_LINEIN;
    Delay(4000);
    linein_audio_open();
    aud_PAmute_oper(0);
}

void app_linein_exit(void)
{   
    app_handle_t app_h = app_get_sys_handler();

#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    app_exit_sniff_mode();
#endif

    linein_audio_close();

#ifdef USER_AUX_EQ
	aux_adc_enable = 0;
	os_delay_ms(1);

    if(aux_ptr.ptr_buf0)
    {
	    jfree(aux_ptr.ptr_buf0);
        aux_ptr.ptr_buf0 = 0;
    }

    if(aux_ptr.ptr_buf1)
    {
	    jfree(aux_ptr.ptr_buf1);
        aux_ptr.ptr_buf1 = 0;
    }

    if(aux_ptr.ptr_buf2)
    {
	    jfree(aux_ptr.ptr_buf2);
        aux_ptr.ptr_buf2 = 0;
    }
	
#endif

    app_h->flag_sm1 &= ~APP_FLAG_LINEIN;
    Delay(2000);
    os_printf("app_linein_exit\r\n");
}

void app_linein_scanning( void )
{
    if(linein_cfg.detect_func)
    {
        (*linein_cfg.detect_func)();
    }
}

static void playwav_resumelinein(uint32 fieldId)
{
    if((!app_wave_playing()) && (0 == check_wave_file_correct(fieldId)))
    {
        /* close linein audio*/
        linein_audio_close();
        /* start wave*/
        start_wave_and_action(fieldId, NULL); // linein_audio_open
    }
}

void app_playwav_resumelinein(uint32 fieldId)
{
    playwav_resumelinein(fieldId);
}

static int app_linein_vol_up(void)
{
    if(player_vol_bt != AUDIO_VOLUME_MAX)
    {
        player_vol_bt ++;
    }
    if(!app_wave_playing())
        aud_volume_set(player_vol_bt);
    if(AUDIO_VOLUME_MAX == player_vol_bt)
    {
        playwav_resumelinein(APP_WAVE_FILE_ID_VOL_MAX);
    }
#if (SYS_VOL_SAVE == 1)
    app_save_volume(ENV_LINE_VOL_INFO);
#else
	app_vol_data();
#endif
    return 0;
}

static int app_linein_vol_down(void)
{ 
    if(player_vol_bt != AUDIO_VOLUME_MIN)
    {
        player_vol_bt --;
    }
    if(!app_wave_playing())
        aud_volume_set(player_vol_bt);

    if(AUDIO_VOLUME_MIN == player_vol_bt)
    {
        playwav_resumelinein(APP_WAVE_FILE_ID_VOL_MIN);
    }
#if (SYS_VOL_SAVE == 1)
    app_save_volume(ENV_LINE_VOL_INFO);
#else
	app_vol_data();
#endif
    return 0;
}

static int app_linein_mute_unmute(void)
{
    return 0;
}

uint8 linein_is_attached(void)
{
    return(linein_cfg.status == LINEIN_STATUS_ON);
}

uint8 linein_get_play_status(void)
{
    return linePlay;
}

void linein_mode_msg_handler(void)
{
    int ret;
    uint8 exit_flag = 0;
    MSG_T msg;
    app_handle_t app_h = app_get_sys_handler();
    app_set_led_event_action(LED_EVENT_LINEIN_PLAY);


#if defined(CONFIG_LINE_SD_SNIFF)
#ifdef CONFIG_APP_USB 
    app_env_handle_t  env_h = app_env_get_handle();
    if(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_USB_ENA)
    {
        bt_unit_set_scan_enable( app_h->unit, HCI_NO_SCAN_ENABLE);
    }
    else
#endif
        bt_unit_set_scan_enable( app_h->unit, HCI_PAGE_SCAN_ENABLE); // HCI_NO_SCAN_ENABLE isr ceva: 20s  
#else
    bt_unit_set_scan_enable(app_h->unit,HCI_NO_SCAN_ENABLE);
#endif
    if(pre_enter_mode_handle(app_h->sys_work_mode))
        return;
	
    app_restore_volume(ENV_LINE_VOL_INFO);
    enter_work_mode();

    if(!linein_is_attached())
    {
        //BK3000_AUD_FIFO_CONF &= ~(1 << sft_AUD_ADC_INT_EN);
        msg_put(MSG_LINEIN_DETACH);
    }
#if defined(CONFIG_LINE_SD_SNIFF)
    bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,1);
#endif	


#ifdef USER_AUX_EQ
	aux_ptr.ptr_buf0 = (int32 *)jmalloc(AUX_BUF_BYTE_SIZE*sizeof(uint8), M_ZERO);
	aux_ptr.ptr_buf1 = (int32 *)jmalloc(AUX_BUF_BYTE_SIZE*sizeof(uint8), M_ZERO);
	aux_ptr.ptr_buf2 = (int32 *)jmalloc(AUX_BUF_BYTE_SIZE*sizeof(uint8), M_ZERO);

    memset( aux_ptr.ptr_buf0, 0, AUX_BUF_BYTE_SIZE*sizeof(uint8));
    memset( aux_ptr.ptr_buf1, 0, AUX_BUF_BYTE_SIZE*sizeof(uint8));
    memset( aux_ptr.ptr_buf2, 0, AUX_BUF_BYTE_SIZE*sizeof(uint8));

	aux_ptr.buf_cnt = 0;
	aux_ptr.ptr_cnt = 0;
	aux_ptr.in_ptr =   aux_ptr.ptr_buf0;
	aux_ptr.deal_ptr = aux_ptr.ptr_buf1;
	aux_ptr.out_ptr =  aux_ptr.ptr_buf2;

	os_delay_ms(1);
	aux_eq_deal = 0;	
	aux_adc_enable = 1;
#endif	

    linePlay=1;
#if defined(CONFIG_BLUETOOTH_SPP)&& defined(SUPPORT_SPP_IN_MP3PLAYER)
    if(app_mp3player_ble_spp_is_enable()) 
    {
        bt_unit_set_scan_enable( app_h->unit, HCI_PAGE_SCAN_ENABLE); 
        //bt_unit_set_scan_enable( app_h->unit, HCI_INQUIRY_SCAN_ENABLE | HCI_PAGE_SCAN_ENABLE );
    }
#endif

    while(!exit_flag)
    {

    #ifdef USER_AUX_EQ
		if (aux_eq_deal)
		{
			aux_eq_deal = 0;
    		BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);	

		#if (USER_EX_EQ == 1)
		    app_env_handle_t  env_h = app_env_get_handle();
		    uint8 flag_L_is_LplusR = !!(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_L_is_LplusR);

			if (flag_L_is_LplusR)
				LeadelityMono((short*)aux_ptr.deal_ptr, (short*)aux_ptr.deal_ptr, AUX_BUF_BYTE_SIZE/4, 44100, 0);
			else
				Leadelity((short*)aux_ptr.deal_ptr, (short*)aux_ptr.deal_ptr, AUX_BUF_BYTE_SIZE/4, 44100, 0);
		#endif

		#if (CONFIG_PRE_EQ == 1)
			int32 data_tmp[AUX_BUF_BYTE_SIZE/2],p;
			int16 *ptr;
			ptr = (int16 *)aux_ptr.deal_ptr;
			if((aud_pre_eqe.online_flag == 0x5a)||(aud_pre_eqe.online_flag == 1))
			{
				for(p=0;p<(AUX_BUF_BYTE_SIZE/2);p++)
					data_tmp[p] = ptr[p]*aud_pre_eqe.globle_gain;
				func_sw_eq((int32 *)data_tmp,(int32 *)data_tmp, AUX_BUF_BYTE_SIZE/4, aud_pre_eqe.totle_EQ,0);
				
				for(p=0;p<(AUX_BUF_BYTE_SIZE/2);p++)
					ptr[p] = data_tmp[p]>>14;
			}
		#endif
		
		#if (CONFIG_DRC == 1)
			extern DRCContext app_drc;
			if(!app_drc.init)
			{
				drc_init(&app_drc,44100,2);
			}
			else if(app_drc.flag)
			{
				int16 *ptr,p;
				ptr = (int16 *)aux_ptr.deal_ptr;
				
				if((app_drc.flag)&(1<<0))
				{
					for(p=0;p<(AUX_BUF_BYTE_SIZE/2);p++)
						ptr[p]=f_sat(((int)ptr[p]*app_drc.pre_vol)>>14);
				}

				if((app_drc.flag)&(1<<2))
					drc_apply(&app_drc, (int16_t*)aux_ptr.deal_ptr, (int16_t*)aux_ptr.deal_ptr, AUX_BUF_BYTE_SIZE/4);

				if((app_drc.flag)&(1<<1))
				{
					for(p=0;p<(AUX_BUF_BYTE_SIZE/2);p++)
						ptr[p]=f_sat(((int)ptr[p]*app_drc.post_vol)>>14);
				}
			}
		#endif
		
		}
	#endif
	
        ret = msg_get(&msg);
        if(MSG_FAILURE_RET == ret)
            msg.id = MSG_NULL;
        else
        {
            switch(msg.id)
            {
                case MSG_KEY_PLAY:
                    if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_AUX_MODE_PAUSE_MUTE))
                    {
                        if(linePlay)
                        {
                            os_printf("line in pause...\r\n");
                            linePlay = 0;
                        #if defined(CONFIG_LINE_SD_SNIFF)
                            bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,0);
                        #endif
                            app_sleep_func(1);
                            linein_audio_close();
                            aud_volume_mute(1);
                            
                        }
                        else
                        {
                            os_printf("line in playing...\r\n");
                            linePlay = 1;
                        #if defined(CONFIG_LINE_SD_SNIFF)
                            bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,1);
                        #endif
                            linein_audio_open();
                            
                        }
                    }
                    break;

                case MSG_KEY_NEXT:
                    app_linein_vol_up();					
                    break;
                case MSG_KEY_PREV:
                    app_linein_vol_down();					
                    break;
                case MSG_KEY_VOL_DOWN:
                    app_linein_vol_down();
                    break;
                case MSG_KEY_VOL_UP:
                    app_linein_vol_up();
                    break;	
                case MSG_HALF_SECOND:			
                    break;
                default:
                    exit_flag = common_msg_handler(&msg);
                    break;
            }
        }
        if(app_wave_playing())
            app_wave_file_play();	

        BTms_Sched(1);//controller的处理
        //Beken_Uart_Rx();//调试串口命令处理

    #if (CONFIG_CPU_CLK_OPTIMIZATION == 1)&& defined(CONFIG_LINE_SD_SNIFF)
        if(( !bt_flag1_is_set( APP_FLAG_WAVE_PLAYING ) )&&( !bt_flag2_is_set( APP_FLAG2_LINEIN_SD_PLAYING ) ))
        {
            BK3000_set_clock(1, 0);
        }
    #endif
        timer_clear_watch_dog();
    }
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
#if defined(CONFIG_LINE_SD_SNIFF)
    bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,0);
#endif
    app_exit_sniff_mode();
#endif

}

// EOF

// EOF

#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"
#include "bt_mini_sched.h"
#include "bk3000_gpio.h"
#include "sys_irq.h"
#include "hw_leds.h"
//#include "bootloader.h"

volatile uint32 sleep_tick  = 0;
volatile uint32 pwdown_tick = 0;
volatile uint8 usb_tf_aux_in_out_flag = 0;//防止在关机过程中拔掉U盘、TF卡及line-in产生的问题
volatile uint8 player_vol_bt = 0;
volatile uint8 player_vol_hfp = 0;

volatile uint8 flag_power_charge = 0;
volatile uint16 adcmute_cnt = 0;

volatile uint8 flag_sys_ledstate = 0;
volatile uint8 flag_sys_powerdown = 0;

volatile uint8 flag_ble_enable = 0;
volatile uint8 flag_aili_sco_end = 0;
volatile uint8 flag_aili_sco_active = 0;
volatile uint8 flag_not_autoconn_ble_enable = 0;
uint8 voice_static=0 ;

uint8 sbc_disable=0;
#ifdef BEKEN_OTA
#include "app_ota.h"
volatile uint32 FLASH_ENVDATA2_DEF_ADDR = 0;
volatile uint32 FLASH_ENVDATA_DEF_ADDR = 0;
volatile uint32 FLASH_CHANGED_BT_ADDR = 0;
volatile uint32 FLASH_ENVCFG_DEF_ADDR = 0;
extern BOOT_CODE u_int32 get_software_version(u_int8 half);
extern BOOT_CODE u_int16 get_software_mark(u_int8 half);
volatile uint8 flag_ota_enable = 0;
volatile uint16 flag_ota_wdt_rst = 0;
volatile uint8 otaupflag;
#endif

extern void controller_init(void);
extern void uart_initialise(u_int32 baud_rate);
extern RAM_CODE void Beken_Uart_Rx(void);
extern RAM_CODE int msg_get(MSG_T *msg_ptr);
extern void eq_data_deal(uint8 * str);

static void bsp_init(void) {
#ifdef CONFIG_IRDA
    IrDA_init();
#endif
#ifdef CONFIG_DRIVER_I2C
    i2c_init(100000, 1);
#endif
    flash_init();
#if defined(CONFIG_BLUETOOTH_HFP) && defined(CONFIG_DRIVER_ADC)
    sdadc_init();
#endif
    //saradc_reset();

#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
    dma_initial();
#endif
    //os_printf("exit bsp_init\r\n");
}

static void host_init(void) {
#ifdef BEKEN_OTA
    uint16 soft_version;
#endif
    app_env_handle_t  env_h = app_env_get_handle();
#ifdef WROK_AROUND_DCACHE_BUG
    app_Dcache_initial();
#endif

#if (CONFIG_XIAONIU_DEVICE_TEST == 1)
	flag_os_printf_disable = 0;
	user_batt_charger_vlcf = 0xff;
#endif

	flag_aili_sco_end = 0;
	flag_aili_sco_active = 0;
	flag_sys_powerdown = 0;
	flag_not_autoconn_ble_enable = 1;

#if (USER_BLE_ALWAYS_ENABLE == 1)
    flag_ble_enable = 1;
#else
    flag_ble_enable = 0;
#endif


	bsp_init();

#ifdef CONFIG_DRIVER_I2S
    i2s_init();
#else
#ifdef CONFIG_DRIVER_ADC
    // Only initial adc once
    aud_adc_initial(8000,1,16);
#endif
#endif
    app_init();
#ifdef CONFIG_TWS
    app_tws_set_stereo_role();
#endif

    j_stack_init(NULL);
    app_post_init();
    app_env_post_init();
    msg_init();
    msg_clear_all();
    //low power detect
    saradc_calibration_first();
    /* WDT CLK 1/16 32K */
    CLEAR_WDT;
    BK3000_start_wdt(0xFFFF);				//wdt reset 0xc000:10s
    
#if (CONFIG_CHARGE_EN == 1)
    flag_power_charge = 0;
    bt_flag2_operate(APP_FLAG2_CHARGE_POWERDOWN,0);
#endif

    app_linein_init();

#if (CONFIG_XIAONIU_DEVICE_TEST == 1)
	//改地址重启后的自动开机条件
	if (0 == rf_test_user_read_reg())
#endif
    app_env_power_on_check();

#if (CONFIG_XIAONIU_DEVICE_TEST == 1)
	//清除条件
	rf_test_user_clear_reg();
#endif

    app_set_led_event_action( LED_EVENT_POWER_ON );
#if (CONFIG_APP_MP3PLAYER == 1)
#ifdef CONFIG_APP_SDCARD
    app_sd_init();
#endif
#ifdef CONFIG_APP_USB
    if(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_USB_ENA)
    {
        pre_usb_init();
        unsigned int oldmask = get_spr(SPR_VICMR(0));//read old spr_vicmr
        set_spr(SPR_VICMR(0), 0x00);  //mask all/low priority interrupt.
        oldmask |= (1<<	VIC_USB_ISR_INDEX);
        oldmask |= (1<< VIC_SADC_ISR_INDEX);
        set_spr(SPR_VICMR(0), oldmask); //recover the old spr_vicmr.
        post_usb_init();
    }
#endif
#endif

    audio_dac_analog_init(!(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_DAC_DIFFER));

#ifdef FM_ENABLE
    FM_IC_PowerDown();
#endif
#ifdef BEKEN_OTA
    soft_version = get_software_version(1);
    if((soft_version & 0xff00) != 0x6600)
        soft_version = get_software_version(0);
    os_printf("| SW Version: %04x,%04x,%02d.%02d.%02d.%02d\r\n",get_software_mark(0),get_software_mark(1),(soft_version&0xf000)>>12,(soft_version&0x0f00)>>8,(soft_version&0x00f0)>>4,(soft_version&0x000f));
#endif

#if (CONFIG_CHARGE_EN == 1)
    if(!bt_flag2_is_set(APP_FLAG2_CHARGE_POWERDOWN)||(!app_env_check_Charge_Mode_PwrDown() && (flag_power_charge==0)))
#endif
        app_wave_file_play_start(APP_WAVE_FILE_ID_START);

    os_printf("SW finished %s, %s\r\n", __TIME__, __DATE__);
}

#ifdef BEKEN_OTA
static void load_env_config(void)
{
    extern uint32 _FLASH_ENVCFG_DEF_ADDR; 	  
    extern uint32 _FLASH_ENVDATA2_DEF_ADDR;		 
    extern uint32 _FLASH_ENVDATA_DEF_ADDR;	 
    extern uint32 _FLASH_CHANGED_BT_ADDR;

    FLASH_ENVCFG_DEF_ADDR    = (uint32)&_FLASH_ENVCFG_DEF_ADDR; 	  
    FLASH_ENVDATA2_DEF_ADDR  = (uint32)&_FLASH_ENVDATA2_DEF_ADDR;		 
    FLASH_ENVDATA_DEF_ADDR   = (uint32)&_FLASH_ENVDATA_DEF_ADDR;	 
    FLASH_CHANGED_BT_ADDR    = (uint32)&_FLASH_CHANGED_BT_ADDR;
    uart_initialise(UART_BAUDRATE_115200);
    os_printf("===cfg:%08x\r\n",FLASH_ENVCFG_DEF_ADDR);
 }
#endif

static void public_init(void) {
    BK3000_Ana_PLL_enable(CPU_DCO_CLK); /* CPU clk = 96MHz */ //  
    BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
    BK3000_GPIO_Initial();
    SYSirq_Initialise();
#ifdef BEKEN_OTA
    load_env_config();
#endif
//#if (CONFIG_DEBUG_PCM_TO_UART == 1)
//    uart_initialise(UART_BAUDRATE_460800);//UART_BAUDRATE_460800
//#else
//    uart_initialise(UART_BAUDRATE_115200);
//#endif
}

#ifdef BEKEN_DEBUG
#if 1
 void _Stack_Integrity_Check(void) {
    extern uint32 _sbss_end;
    extern uint32 _stack;
    //volatile uint32 *p_sbss = (volatile uint32 *)((uint32)&_sbss_end  & (~3));
    //volatile uint32 *p_dram_code  = (volatile uint32 *)((uint32) &_stack);
    os_printf("===system stack size:%p,%p,%p\r\n",&_stack,&_sbss_end,(uint32)&_stack - (uint32)&_sbss_end);
#if 0
    if (p_sbss[0] != 0XDEADBEEF) {
        os_printf("ShowStack:%p:%p\r\n",  &_sbss_end, &_stack);
        os_printf("Stack overflow!\r\n");
        while(1);
    }
#endif
#if 0
    if(p_dram_code[0] != 0xDEADBEEF)
    {
        os_printf("DRAM_CODE is polluted\r\n");
        while(1);
    }
#endif
}
#endif
#endif


int OPTIMIZE_O0 main(void) {
    uint32 mode;
    app_handle_t app_h = app_get_sys_handler();

    public_init();
    controller_init();
	
    if(SW_PWR_KEY_MOS_CTRL==app_env_check_pwrCtrl_mode())
        gpio_config( 7, 5 ); 					// 32pin ,GPIO3&GPIO15.

    usb_tf_aux_in_out_flag=0;
    host_init();
#ifdef ENABLE_PWM
    PWMxinit(0, 1,PWM_PERIOD,PWM_HIGH);      //GPIO6
    //PWMxinit(1, 1,PWM_PERIOD,PWM_HIGH); 
    //PWMxinit(2, 1,PWM_PERIOD,PWM_HIGH); 
    //PWMxinit(3, 1,PWM_PERIOD,PWM_HIGH);
    PWMxinit(4, 1,PWM_PERIOD,PWM_HIGH);//GPIO13
    PWMxinit(5, 1,PWM_PERIOD,PWM_HIGH);//GPIO14
#endif
    bk3000_set_afc_disable(); //  频偏自动调整

	timer_pt1_start(2);
    
#ifdef JMALLOC_STATISTICAL
    os_printf("JMALLOC_STATISTICAL: POWER ON\r\n");
    memory_usage_show();
#endif
    /* WDT CLK 1/16 32K */
    CLEAR_WDT;
    BK3000_start_wdt(0xFFFF);				//wdt reset 0xc000:10s
    
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    app_exit_sniff_mode();
#endif
    
#if defined(CONFIG_APP_SDCARD) && defined(CONFIG_BK_QFN56_DEMO) //Only BK3266 56PIN DEMO 复用CLK Det    
    gpio_config(9,5);
#endif
    _Stack_Integrity_Check();


    opus_dma_init();

	power_rgb_init();

    //wait 2 second for BT initialization  && SD/UDISK/LINEIN detection complete
    //then decide enter which mode
    uint64_t tmp =os_get_tick_counter();
    while(1)
    {
        if(((os_get_tick_counter() - tmp) > 200)&&(!app_wave_playing()))
        {
        #if (CONFIG_APP_MP3PLAYER == 1)
        #ifdef CONFIG_APP_USB
            if(udisk_is_attached())	
            {
                app_h->sys_work_mode = SYS_WM_UDISK_MUSIC_MODE;
                //bt_auto_connect_stop();
            }
            else 
        #endif
        #if defined(CONFIG_APP_SDCARD)
            if(sd_is_attached())
            {
                app_h->sys_work_mode = SYS_WM_SD_MUSIC_MODE;
                //bt_auto_connect_stop();
            }
            else 
        #endif
        #endif
            if(linein_is_attached())
            {
                app_h->sys_work_mode = SYS_WM_LINEIN_MODE;
                //bt_auto_connect_stop();
            }
            else
            {
                app_h->sys_work_mode = SYS_WM_BT_MODE;
            }
            break;
        }
        app_wave_file_play();
        BT_msg_handler();
        CLEAR_WDT;
    }

#ifdef BEKEN_OTA
    flag_ota_enable = 0;
    flag_ota_wdt_rst = 0;

    /* Erase flash section for OTA upgrade */
    if(0x5aa5 == get_software_mark(0) && (get_software_mark(1) != 0xffff)) /* if active section is upper one,erased the lower section */
        app_ota_erase_flash(1);
    else if(0xa55a == get_software_mark(1) && get_software_mark(0) != 0xffff)					   /* else erase the upper section */
        app_ota_erase_flash(0);
    else if(0x5aff == get_software_mark(0) && (get_software_mark(1) == 0xa5ff))
		app_ota_erase_flash(1);
	else if(0x5aff == get_software_mark(0) && (get_software_mark(1) == 0xa500))
		app_ota_erase_flash(1);
#endif

    app_h->sys_work_mode = SYS_WM_BT_MODE;

#if (USER_EX_EQ == 1)
	user_leadelityEntryInit();
	modeSwitch(0);					//normal
#endif
    os_printf("ssssssssssssssssssssssssssssssssssssssssss: POWER ON\r\n");

    os_printf("sys_work_mode:%d\r\n",app_h->sys_work_mode);
    while(1) 
    {
        mode = get_app_mode();
        switch(mode) 
        {
            case SYS_WM_LINEIN_MODE:
                linein_mode_msg_handler();
                break;
                
        #if (CONFIG_APP_MP3PLAYER == 1)
            case SYS_WM_SD_MUSIC_MODE:
        #ifdef CONFIG_APP_USB 
            case SYS_WM_UDISK_MUSIC_MODE:
        #endif
                music_mode_msg_handler();
                break;
        #ifdef FM_ENABLE			
            case SYS_WM_FM_MODE:
                fm_msg_handler();
                break;
        #endif				
        #endif  
        
            case SYS_WM_BT_MODE:
            default:
				//opus_init();		
				bt_mode_msg_handler();
				opus_unint();		
                break;
        }
    }
    return 0;
}

#include "config.h"
#if(AMA_ENABLE==0)
#ifndef DMA_AUDIO_H_
#define  DMA_AUDIO_H_

#include "dueros_dma.h"
#include "dma.pb-c.h"
#include "platform.h"


#if 1

#define MAX_REV_QUE    	7//20
#define REV_QUE_LEN     160//32

#define NO_QUE     0xff

typedef struct {
uint8 WriteCounter;
uint8  ReadCounter;
uint8  Pool[MAX_REV_QUE][REV_QUE_LEN];
uint16  pool_que_len[MAX_REV_QUE];
}MSG_QUE;


void put_que(uint16  len,uint8 * buf);
uint8 get_que(uint8 * buf);

#endif


/////////////////////////////////////////////////////////////




#define  TOTAL_BLOCK_NUM  			 (40)

#define  EN_OPUS_NUM               40

#define OPUS_SAMPLES_PER_FRAME  (320)
#define OPUS_PCM_LEN_PER_FRAME  (OPUS_SAMPLES_PER_FRAME * 2)

#define MAX_PCM_BUF_ROW		 3
#define MAX_PCM_BUF_LEN      OPUS_SAMPLES_PER_FRAME

extern int16_t pcm_sample_buffer[MAX_PCM_BUF_ROW][MAX_PCM_BUF_LEN];

extern uint8_t pcm_index_in;
extern uint8_t pcm_index_out;
extern uint8_t voice_start_flag;
extern uint8_t tx_audio_frame_cnt;
extern volatile uint8 voice_upload_disable;

void audio_isr_get_data(int16 adc_data);
int audio_start(void);
int audio_stop(void);

void app_send_audio(void);
void app_ble_ack_val(void );
void app_ble_send_val(uint8_t byes,uint8_t *buf );
void print_debug_status(void);
void app_send_encode_evt(void);
void encode_voice_init(void); 
void print_debug_info(void);
uint8_t store_encode_data(uint8_t *buf);
uint8_t  read_encode_data(uint8_t *buf);
void opus_init(void);
 void en_opus(void);


void opus_dma_init(void);

int ble_send_audio(uint8_t *buf, uint32_t len);
int ble_send_cmd(uint8_t *buf, uint32_t len);
void ble_get_setting(SpeechSettings*P);
void ble_get_dev_info(DeviceInformation *P);
void ble_get_dev_config(DeviceConfiguration*P);
void ble_set_state(State *P);
void ble_get_state(uint32_t feature ,State *p);
void ble_notify_speech_state(uint8 sta);
int ble_at_command(const char *P);
int ble_stop_clear(void);

 char* StrSHA256(const char* str, long long length, char* sha256);

void ble_get_pair_config(PairInformation*p);

uint8 dma_sign2(ControlEnvelope *p);

void opus_unint(void);

void call_num_start(void);
void dma_clear_rand(void);
 

#endif

#endif




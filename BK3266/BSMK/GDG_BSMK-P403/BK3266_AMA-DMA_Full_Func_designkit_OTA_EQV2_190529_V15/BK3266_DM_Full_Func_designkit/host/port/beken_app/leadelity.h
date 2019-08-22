#ifndef _LEADELITY_H_
#define _LEADELITY_H_
//input format is L/R L/ R, FrameSize is a  channel 's length
/*parameter: pIn,Input buffer
			 pOut,Output buffer
			 FrameSize:amount for one channel.shuole be times of 8.e.g.16 32 48...
			 sampleRate:sample rate of the input
			 audioStreamType:{0 == 16bit in short ,1 == 24bit in int}
	usage   :put it in the audio processing task.
*/			 
extern void Leadelity(uint8_t *pIn, uint8_t*pOut, int FrameSize, int sampleRate,short audioStreamType);
extern void LeadelityMono(uint8_t *pIn, uint8_t*pOut, int FrameSize, int sampleRate, short audioStreamType);
//extern void LeadelityInit(void);
extern void tunningApi(uint8_t *d);
extern void volumeSet(int vol);//vol is in Q5.27 format  liniear.
extern void modeSwitch(int m);//{normal,musicVoiceCanncel,virturl
extern void resetWhenSampleRateChanged(int* image,int size);//the image is form the tuning tool
//call this at first time ,it will allcoat memory .
//call this after leadelityExitProcessing,it will do it as the first time do .otherwise it only copy the imag 
extern void leadelityEntryInit(int* image,int size);
extern void leadelityExitProcessing(void);



#endif // _LEADELITY_H_

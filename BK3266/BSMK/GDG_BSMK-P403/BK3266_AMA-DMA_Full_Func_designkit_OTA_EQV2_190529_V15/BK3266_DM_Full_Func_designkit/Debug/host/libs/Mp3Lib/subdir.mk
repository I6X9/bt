################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../host/libs/Mp3Lib/bitstream.c \
../host/libs/Mp3Lib/bkmp3_resample.c \
../host/libs/Mp3Lib/buffers.c \
../host/libs/Mp3Lib/dct32.c \
../host/libs/Mp3Lib/dequant.c \
../host/libs/Mp3Lib/dqchan.c \
../host/libs/Mp3Lib/huffman.c \
../host/libs/Mp3Lib/hufftabs.c \
../host/libs/Mp3Lib/imdct.c \
../host/libs/Mp3Lib/layer21.c \
../host/libs/Mp3Lib/mp3_play.c \
../host/libs/Mp3Lib/mp3dec.c \
../host/libs/Mp3Lib/mp3tabs.c \
../host/libs/Mp3Lib/scalfact.c \
../host/libs/Mp3Lib/stproc.c \
../host/libs/Mp3Lib/subband.c \
../host/libs/Mp3Lib/trigtabs.c \
../host/libs/Mp3Lib/wavtabs.c 

S_UPPER_SRCS += \
../host/libs/Mp3Lib/asmmisc.S \
../host/libs/Mp3Lib/asmpoly_thumb2.S 

OBJS += \
./host/libs/Mp3Lib/asmmisc.o \
./host/libs/Mp3Lib/asmpoly_thumb2.o \
./host/libs/Mp3Lib/bitstream.o \
./host/libs/Mp3Lib/bkmp3_resample.o \
./host/libs/Mp3Lib/buffers.o \
./host/libs/Mp3Lib/dct32.o \
./host/libs/Mp3Lib/dequant.o \
./host/libs/Mp3Lib/dqchan.o \
./host/libs/Mp3Lib/huffman.o \
./host/libs/Mp3Lib/hufftabs.o \
./host/libs/Mp3Lib/imdct.o \
./host/libs/Mp3Lib/layer21.o \
./host/libs/Mp3Lib/mp3_play.o \
./host/libs/Mp3Lib/mp3dec.o \
./host/libs/Mp3Lib/mp3tabs.o \
./host/libs/Mp3Lib/scalfact.o \
./host/libs/Mp3Lib/stproc.o \
./host/libs/Mp3Lib/subband.o \
./host/libs/Mp3Lib/trigtabs.o \
./host/libs/Mp3Lib/wavtabs.o 

C_DEPS += \
./host/libs/Mp3Lib/bitstream.d \
./host/libs/Mp3Lib/bkmp3_resample.d \
./host/libs/Mp3Lib/buffers.d \
./host/libs/Mp3Lib/dct32.d \
./host/libs/Mp3Lib/dequant.d \
./host/libs/Mp3Lib/dqchan.d \
./host/libs/Mp3Lib/huffman.d \
./host/libs/Mp3Lib/hufftabs.d \
./host/libs/Mp3Lib/imdct.d \
./host/libs/Mp3Lib/layer21.d \
./host/libs/Mp3Lib/mp3_play.d \
./host/libs/Mp3Lib/mp3dec.d \
./host/libs/Mp3Lib/mp3tabs.d \
./host/libs/Mp3Lib/scalfact.d \
./host/libs/Mp3Lib/stproc.d \
./host/libs/Mp3Lib/subband.d \
./host/libs/Mp3Lib/trigtabs.d \
./host/libs/Mp3Lib/wavtabs.d 

S_UPPER_DEPS += \
./host/libs/Mp3Lib/asmmisc.d \
./host/libs/Mp3Lib/asmpoly_thumb2.d 


# Each subdirectory must supply rules for building sources it contributes
host/libs/Mp3Lib/%.o: ../host/libs/Mp3Lib/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: BA ELF GNU C compiler'
	ba-elf-gcc -DBA22_DEee -DHAVE_CONFIG_H -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth\protocol\avctp" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth\protocol\avdtp" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\le\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\libs\FatLib" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\include\os" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\libs\Mp3Lib" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\config" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\jos" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\pkg\sbc" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\jos\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\beken_app" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\beken_driver" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\beken_no_os" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\common" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\common\bluetooth" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\include\bluetooth" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\bt\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\hc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\dl\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\uslc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\lslc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lmp\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\sys\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\tc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\transport\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\hw\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\hw\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\hw\radio\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\beken\hw\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\beken\sys\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\include\class" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems\none" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems\none\afs" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\drivers\msd" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\opus\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\dma" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\ama" -O1 -flto -G 100000 -g3 -Wall -c -fmessage-length=0  -mno-hard-float -ffast-math -march=ba2 -mle -mabi=3 -mtj-rodata -msingle-float -mdsp -mmac -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

host/libs/Mp3Lib/%.o: ../host/libs/Mp3Lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: BA ELF GNU C compiler'
	ba-elf-gcc -DBA22_DEee -DHAVE_CONFIG_H -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth\protocol\avctp" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth\protocol\avdtp" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\le\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\libs\FatLib" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\include\os" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\libs\Mp3Lib" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\config" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\jos" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\pkg\sbc" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\jos\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\beken_app" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\beken_driver" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\beken_no_os" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\common" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\common\bluetooth" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\include\bluetooth" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\bt\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\hc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\dl\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\uslc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\lslc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lmp\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\sys\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\tc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\transport\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\hw\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\hw\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\hw\radio\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\beken\hw\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\beken\sys\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\include\class" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems\none" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems\none\afs" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\drivers\msd" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\opus\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\dma" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\ama" -O1 -flto -G 100000 -g3 -Wall -c -fmessage-length=0  -mno-hard-float -ffast-math -march=ba2 -mle -mabi=3 -mtj-rodata -msingle-float -mdsp -mmac -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../host/port/beken_app/ama_audio.c \
../host/port/beken_app/app_aec.c \
../host/port/beken_app/app_bt.c \
../host/port/beken_app/app_button.c \
../host/port/beken_app/app_customer.c \
../host/port/beken_app/app_drc.c \
../host/port/beken_app/app_env.c \
../host/port/beken_app/app_equ.c \
../host/port/beken_app/app_exception.c \
../host/port/beken_app/app_fat.c \
../host/port/beken_app/app_hfp.c \
../host/port/beken_app/app_i2c_fm.c \
../host/port/beken_app/app_linein.c \
../host/port/beken_app/app_msg.c \
../host/port/beken_app/app_ota.c \
../host/port/beken_app/app_player.c \
../host/port/beken_app/app_prompt_wav.c \
../host/port/beken_app/app_protocol.c \
../host/port/beken_app/app_sbc.c \
../host/port/beken_app/app_udisk.c \
../host/port/beken_app/dma_audio.c \
../host/port/beken_app/protobuf-c.c 

S_UPPER_SRCS += \
../host/port/beken_app/app_asm_sw_eq.S 

OBJS += \
./host/port/beken_app/ama_audio.o \
./host/port/beken_app/app_aec.o \
./host/port/beken_app/app_asm_sw_eq.o \
./host/port/beken_app/app_bt.o \
./host/port/beken_app/app_button.o \
./host/port/beken_app/app_customer.o \
./host/port/beken_app/app_drc.o \
./host/port/beken_app/app_env.o \
./host/port/beken_app/app_equ.o \
./host/port/beken_app/app_exception.o \
./host/port/beken_app/app_fat.o \
./host/port/beken_app/app_hfp.o \
./host/port/beken_app/app_i2c_fm.o \
./host/port/beken_app/app_linein.o \
./host/port/beken_app/app_msg.o \
./host/port/beken_app/app_ota.o \
./host/port/beken_app/app_player.o \
./host/port/beken_app/app_prompt_wav.o \
./host/port/beken_app/app_protocol.o \
./host/port/beken_app/app_sbc.o \
./host/port/beken_app/app_udisk.o \
./host/port/beken_app/dma_audio.o \
./host/port/beken_app/protobuf-c.o 

C_DEPS += \
./host/port/beken_app/ama_audio.d \
./host/port/beken_app/app_aec.d \
./host/port/beken_app/app_bt.d \
./host/port/beken_app/app_button.d \
./host/port/beken_app/app_customer.d \
./host/port/beken_app/app_drc.d \
./host/port/beken_app/app_env.d \
./host/port/beken_app/app_equ.d \
./host/port/beken_app/app_exception.d \
./host/port/beken_app/app_fat.d \
./host/port/beken_app/app_hfp.d \
./host/port/beken_app/app_i2c_fm.d \
./host/port/beken_app/app_linein.d \
./host/port/beken_app/app_msg.d \
./host/port/beken_app/app_ota.d \
./host/port/beken_app/app_player.d \
./host/port/beken_app/app_prompt_wav.d \
./host/port/beken_app/app_protocol.d \
./host/port/beken_app/app_sbc.d \
./host/port/beken_app/app_udisk.d \
./host/port/beken_app/dma_audio.d \
./host/port/beken_app/protobuf-c.d 

S_UPPER_DEPS += \
./host/port/beken_app/app_asm_sw_eq.d 


# Each subdirectory must supply rules for building sources it contributes
host/port/beken_app/%.o: ../host/port/beken_app/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: BA ELF GNU C compiler'
	ba-elf-gcc -DBA22_DEee -DHAVE_CONFIG_H -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth\protocol\avctp" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth\protocol\avdtp" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\le\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\libs\FatLib" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\include\os" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\libs\Mp3Lib" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\config" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\jos" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\pkg\sbc" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\jos\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\beken_app" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\beken_driver" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\beken_no_os" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\common" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\common\bluetooth" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\include\bluetooth" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\bt\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\hc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\dl\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\uslc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\lslc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lmp\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\sys\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\tc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\transport\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\hw\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\hw\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\hw\radio\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\beken\hw\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\beken\sys\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\include\class" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems\none" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems\none\afs" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\drivers\msd" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\opus\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\dma" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\ama" -O1 -flto -G 100000 -g3 -Wall -c -fmessage-length=0  -mno-hard-float -ffast-math -march=ba2 -mle -mabi=3 -mtj-rodata -msingle-float -mdsp -mmac -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

host/port/beken_app/%.o: ../host/port/beken_app/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: BA ELF GNU C compiler'
	ba-elf-gcc -DBA22_DEee -DHAVE_CONFIG_H -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth\protocol\avctp" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth\protocol\avdtp" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\le\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\libs\FatLib" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\include\os" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\libs\Mp3Lib" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\bluetooth\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\config" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\jos" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\pkg\sbc" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\jos\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\beken_app" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\beken_driver" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\beken_no_os" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\common" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\common\bluetooth" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\include\bluetooth" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\bt\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\hc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\dl\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\uslc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lc\lslc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\lmp\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\sys\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\tc\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\transport\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\core\hw\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\hw\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\hw\radio\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\beken\hw\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\controller\hal\beken\sys\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\include\class" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems\none" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems\none\afs" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\usb\src\drivers\msd" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\opus\include" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\dma" -I"E:\MyProject\BK3266\BSMK\Code\GDG_BSMK-P403\BK3266_AMA-DMA_Full_Func_designkit_OTA_EQV2_190529_V15\BK3266_DM_Full_Func_designkit\host\port\ama" -O1 -flto -G 100000 -g3 -Wall -c -fmessage-length=0  -mno-hard-float -ffast-math -march=ba2 -mle -mabi=3 -mtj-rodata -msingle-float -mdsp -mmac -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



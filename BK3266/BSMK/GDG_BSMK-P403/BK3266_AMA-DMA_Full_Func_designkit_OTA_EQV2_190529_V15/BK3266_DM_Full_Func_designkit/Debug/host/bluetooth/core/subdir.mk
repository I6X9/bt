################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../host/bluetooth/core/l2cap_signal.c 

OBJS += \
./host/bluetooth/core/l2cap_signal.o 

C_DEPS += \
./host/bluetooth/core/l2cap_signal.d 


# Each subdirectory must supply rules for building sources it contributes
host/bluetooth/core/%.o: ../host/bluetooth/core/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: BA ELF GNU C compiler'
	ba-elf-gcc -DBA22_DEee -DHAVE_CONFIG_H -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\bluetooth\protocol\avctp" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\bluetooth\protocol\avdtp" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\core\le\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\libs\FatLib" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\include\os" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\libs\Mp3Lib" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\bluetooth" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\bluetooth\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\config" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\jos" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\pkg\sbc" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\jos\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\beken_app" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\beken_driver" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\beken_no_os" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\common" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\common\bluetooth" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\include\bluetooth" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\core\bt\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\core\hc\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\core\lc\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\core\lc\dl\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\core\lc\uslc\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\core\lc\lslc\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\core\lmp\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\core\sys\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\core\tc\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\core\transport\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\core\hw\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\hal\hw\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\hal\hw\radio\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\hal\beken\hw\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\controller\hal\beken\sys\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\usb\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\usb\include\class" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems\none" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems\none\afs" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\usb\src\drivers\msd" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\opus\include" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\dma" -I"D:\WY\BEKEN\BK3266\DMA\WDS\BIAOZHUN\OTA\LIB\BK3266_AMA-DMA_Full_Func_designkit_181127-OTA_V0\BK3266_DM_Full_Func_designkit\host\port\ama" -O1 -flto -G 100000 -g3 -Wall -c -fmessage-length=0  -mno-hard-float -ffast-math -march=ba2 -mle -mabi=3 -mtj-rodata -msingle-float -mdsp -mmac -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


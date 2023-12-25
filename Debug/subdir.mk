################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BH1750.c \
../DS18B20.c \
../DataLogger.c \
../EKG.c \
../File_Handling.c \
../GCode.c \
../GFX_FUNCTIONS.c \
../HD44780.c \
../Nextion.c \
../RTC.c \
../SICL.c \
../SMotionplanner.c \
../ST7735.c \
../ST7920_SERIAL.c \
../Servo.c \
../Smoothstep.c \
../Utils.c \
../bmp280.c \
../calibrations.c \
../fatfs_sd.c \
../fonts.c \
../joystick.c \
../pid_controller.c \
../pin.c \
../stepper.c \
../w25qxx.c 

OBJS += \
./BH1750.o \
./DS18B20.o \
./DataLogger.o \
./EKG.o \
./File_Handling.o \
./GCode.o \
./GFX_FUNCTIONS.o \
./HD44780.o \
./Nextion.o \
./RTC.o \
./SICL.o \
./SMotionplanner.o \
./ST7735.o \
./ST7920_SERIAL.o \
./Servo.o \
./Smoothstep.o \
./Utils.o \
./bmp280.o \
./calibrations.o \
./fatfs_sd.o \
./fonts.o \
./joystick.o \
./pid_controller.o \
./pin.o \
./stepper.o \
./w25qxx.o 

C_DEPS += \
./BH1750.d \
./DS18B20.d \
./DataLogger.d \
./EKG.d \
./File_Handling.d \
./GCode.d \
./GFX_FUNCTIONS.d \
./HD44780.d \
./Nextion.d \
./RTC.d \
./SICL.d \
./SMotionplanner.d \
./ST7735.d \
./ST7920_SERIAL.d \
./Servo.d \
./Smoothstep.d \
./Utils.d \
./bmp280.d \
./calibrations.d \
./fatfs_sd.d \
./fonts.d \
./joystick.d \
./pid_controller.d \
./pin.d \
./stepper.d \
./w25qxx.d 


# Each subdirectory must supply rules for building sources it contributes
%.o %.su: ../%.c subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DSTM32 -DSTM32F0 -DDEBUG -DSTM32F030C6Tx -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean--2e-

clean--2e-:
	-$(RM) ./BH1750.d ./BH1750.o ./BH1750.su ./DS18B20.d ./DS18B20.o ./DS18B20.su ./DataLogger.d ./DataLogger.o ./DataLogger.su ./EKG.d ./EKG.o ./EKG.su ./File_Handling.d ./File_Handling.o ./File_Handling.su ./GCode.d ./GCode.o ./GCode.su ./GFX_FUNCTIONS.d ./GFX_FUNCTIONS.o ./GFX_FUNCTIONS.su ./HD44780.d ./HD44780.o ./HD44780.su ./Nextion.d ./Nextion.o ./Nextion.su ./RTC.d ./RTC.o ./RTC.su ./SICL.d ./SICL.o ./SICL.su ./SMotionplanner.d ./SMotionplanner.o ./SMotionplanner.su ./ST7735.d ./ST7735.o ./ST7735.su ./ST7920_SERIAL.d ./ST7920_SERIAL.o ./ST7920_SERIAL.su ./Servo.d ./Servo.o ./Servo.su ./Smoothstep.d ./Smoothstep.o ./Smoothstep.su ./Utils.d ./Utils.o ./Utils.su ./bmp280.d ./bmp280.o ./bmp280.su ./calibrations.d ./calibrations.o ./calibrations.su ./fatfs_sd.d ./fatfs_sd.o ./fatfs_sd.su ./fonts.d ./fonts.o ./fonts.su ./joystick.d ./joystick.o ./joystick.su ./pid_controller.d ./pid_controller.o ./pid_controller.su ./pin.d ./pin.o ./pin.su ./stepper.d ./stepper.o ./stepper.su ./w25qxx.d ./w25qxx.o ./w25qxx.su

.PHONY: clean--2e-


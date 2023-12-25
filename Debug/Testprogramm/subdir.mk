################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Testprogramm/main.c 

OBJS += \
./Testprogramm/main.o 

C_DEPS += \
./Testprogramm/main.d 


# Each subdirectory must supply rules for building sources it contributes
Testprogramm/%.o Testprogramm/%.su: ../Testprogramm/%.c Testprogramm/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DSTM32 -DSTM32F0 -DDEBUG -DSTM32F030C6Tx -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Testprogramm

clean-Testprogramm:
	-$(RM) ./Testprogramm/main.d ./Testprogramm/main.o ./Testprogramm/main.su

.PHONY: clean-Testprogramm


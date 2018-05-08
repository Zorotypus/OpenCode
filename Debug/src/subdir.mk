################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BlinkLed.c \
../src/LoRa.c \
../src/LowPowerCtrl.c \
../src/SPI.c \
../src/Timer.c \
../src/_write.c \
../src/command.c \
../src/config.c \
../src/delay.c \
../src/kx122.c \
../src/main.c \
../src/mymalloc.c \
../src/stmflash.c \
../src/usart.c \
../src/w25qxx.c 

OBJS += \
./src/BlinkLed.o \
./src/LoRa.o \
./src/LowPowerCtrl.o \
./src/SPI.o \
./src/Timer.o \
./src/_write.o \
./src/command.o \
./src/config.o \
./src/delay.o \
./src/kx122.o \
./src/main.o \
./src/mymalloc.o \
./src/stmflash.o \
./src/usart.o \
./src/w25qxx.o 

C_DEPS += \
./src/BlinkLed.d \
./src/LoRa.d \
./src/LowPowerCtrl.d \
./src/SPI.d \
./src/Timer.d \
./src/_write.d \
./src/command.d \
./src/config.d \
./src/delay.d \
./src/kx122.d \
./src/main.d \
./src/mymalloc.d \
./src/stmflash.d \
./src/usart.d \
./src/w25qxx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mno-unaligned-access -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



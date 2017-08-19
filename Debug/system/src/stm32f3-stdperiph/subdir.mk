################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/stm32f3-stdperiph/stm32f30x_gpio.c \
../system/src/stm32f3-stdperiph/stm32f30x_rcc.c 

OBJS += \
./system/src/stm32f3-stdperiph/stm32f30x_gpio.o \
./system/src/stm32f3-stdperiph/stm32f30x_rcc.o 

C_DEPS += \
./system/src/stm32f3-stdperiph/stm32f30x_gpio.d \
./system/src/stm32f3-stdperiph/stm32f30x_rcc.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/stm32f3-stdperiph/%.o: ../system/src/stm32f3-stdperiph/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DOS_USE_SEMIHOSTING -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F30X -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -DSTM32F3=1 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f3-stdperiph" -I/home/bilal/Desktop/Embedded_Projects/Libraries/newstuff/libopencm3/include -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main/app_main_dipcoater.c 

OBJS += \
./src/main/app_main_dipcoater.o 

C_DEPS += \
./src/main/app_main_dipcoater.d 


# Each subdirectory must supply rules for building sources it contributes
src/main/%.o: ../src/main/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



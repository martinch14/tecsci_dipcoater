################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/components/api/handlers.c \
../src/components/api/mod_fms.c \
../src/components/api/mod_message.c \
../src/components/api/mod_queue.c \
../src/components/api/mod_ringbuffer.c \
../src/components/api/mod_stack.c \
../src/components/api/process.c 

OBJS += \
./src/components/api/handlers.o \
./src/components/api/mod_fms.o \
./src/components/api/mod_message.o \
./src/components/api/mod_queue.o \
./src/components/api/mod_ringbuffer.o \
./src/components/api/mod_stack.o \
./src/components/api/process.o 

C_DEPS += \
./src/components/api/handlers.d \
./src/components/api/mod_fms.d \
./src/components/api/mod_message.d \
./src/components/api/mod_queue.d \
./src/components/api/mod_ringbuffer.d \
./src/components/api/mod_stack.d \
./src/components/api/process.d 


# Each subdirectory must supply rules for building sources it contributes
src/components/api/%.o: ../src/components/api/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



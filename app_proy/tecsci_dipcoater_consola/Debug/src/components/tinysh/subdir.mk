################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/martin/Escritorio/EspEmbebidos/ESP32_DipCoater/proyectos/app_proy/esp_framework/src/componets/tinysh/tinysh_core.c \
/home/martin/Escritorio/EspEmbebidos/ESP32_DipCoater/proyectos/app_proy/esp_framework/src/componets/tinysh/tinysh_custom.c \
/home/martin/Escritorio/EspEmbebidos/ESP32_DipCoater/proyectos/app_proy/esp_framework/src/componets/tinysh/tinysh_utils.c 

OBJS += \
./src/components/tinysh/tinysh_core.o \
./src/components/tinysh/tinysh_custom.o \
./src/components/tinysh/tinysh_utils.o 

C_DEPS += \
./src/components/tinysh/tinysh_core.d \
./src/components/tinysh/tinysh_custom.d \
./src/components/tinysh/tinysh_utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/components/tinysh/tinysh_core.o: /home/martin/Escritorio/EspEmbebidos/ESP32_DipCoater/proyectos/app_proy/esp_framework/src/componets/tinysh/tinysh_core.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/components/tinysh/tinysh_custom.o: /home/martin/Escritorio/EspEmbebidos/ESP32_DipCoater/proyectos/app_proy/esp_framework/src/componets/tinysh/tinysh_custom.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/components/tinysh/tinysh_utils.o: /home/martin/Escritorio/EspEmbebidos/ESP32_DipCoater/proyectos/app_proy/esp_framework/src/componets/tinysh/tinysh_utils.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



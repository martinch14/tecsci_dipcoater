/*
 * handlers.c
 *
 *  Created on: 1 Abril. 2020
 *  Author: Martin Gambarotta
 *
*/


#include <stdio.h>

#ifndef TECSCI_ARCH_X86
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


#include "TMCMotionController.h"
#include "TMC5130.h"


#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "app_main_spincoater.h"


#endif

#include "handlers.h"
#include <unistd.h>

#ifndef TECSCI_ARCH_X86
#define sleep(x) vTaskDelay((x) * 1000 / portTICK_RATE_MS)
#endif

//Variable Global con la direccion del socket creado, sirve para enviar mensajes al sokcet desde los handlers
extern int sock_global;



// Constante de dezplazamiento lineal   , expresadda en mm/micropasos         ->   una vuelta   1 paso  ==   0.00007851  mm     -->    (una vuelta) 51200   ==   4.02 mm
#define  K_DEZPLAZAMIENTO_LINEAL   0.00007851   //  78.51E-6


int HandlerRight(processCommandArgSpin_t*	arg) {

	Evalboards.ch1.writeRegister(0,0x5F, 	arg->acceleration); 		// writing value 0x000003E8 = 1000 = 0.0 to address 63 = 0x5F(PID_ACCELERATION_LIMIT)-----------------------> Limite Aceleration 1000 rpm/s
	Evalboards.ch1.writeRegister(0,0x66, 	arg->velocity); 		// writing value 0x00000000 = 0 = 0.0 to address 70 = 0x66(PID_VELOCITY_TARGET)-----------------------> Velocity Target

	return 0;
}

int HandlerRight_without_program(processCommandArgSpin_t*	arg) {

	Evalboards.ch1.enableDriver(DRIVER_ENABLE);

	Evalboards.ch1.writeRegister(0,0x5F, 	arg->acceleration); 		// writing value 0x000003E8 = 1000 = 0.0 to address 63 = 0x5F(PID_ACCELERATION_LIMIT)-----------------------> Limite Aceleration 1000 rpm/s
	Evalboards.ch1.writeRegister(0,0x66,~(arg->velocity)); 		// writing value 0x00000000 = 0 = 0.0 to address 70 = 0x66(PID_VELOCITY_TARGET)-----------------------> Velocity Target

	printf("Motor Girando\r\n");
	printf("Velocidad a Derecha:%d /t  Aceleracion:%d\r\n", arg->velocity, arg->acceleration );


	return 0;
}

int HandlerLeft(processCommandArgSpin_t*	arg) {

	Evalboards.ch1.writeRegister(0,0x5F, 	arg->acceleration); 		// writing value 0x000003E8 = 1000 = 0.0 to address 63 = 0x5F(PID_ACCELERATION_LIMIT)-----------------------> Limite Aceleration 1000 rpm/s
	Evalboards.ch1.writeRegister(0,0x66, 	arg->velocity); 		// writing value 0x00000000 = 0 = 0.0 to address 70 = 0x66(PID_VELOCITY_TARGET)-----------------------> Velocity Target

	printf("Paso por handler Left!\r\n");

	return 0;
}

int HandlerLeft_without_program(processCommandArgSpin_t*	arg) {

	Evalboards.ch1.enableDriver(DRIVER_ENABLE);

	Evalboards.ch1.writeRegister(0,0x5F, 	arg->acceleration); 		// writing value 0x000003E8 = 1000 = 0.0 to address 63 = 0x5F(PID_ACCELERATION_LIMIT)-----------------------> Limite Aceleration 1000 rpm/s
	Evalboards.ch1.writeRegister(0,0x66, 	arg->velocity); 		// writing value 0x00000000 = 0 = 0.0 to address 70 = 0x66(PID_VELOCITY_TARGET)-----------------------> Velocity Target

	printf("Motor Girando\r\n");
	printf("Velocidad a Izquierda:%d /t  Aceleracion:%d\r\n", arg->velocity, arg->acceleration );

	return 0;
}


int HandlerRun(void) {

	printf("Motor Encendido\r\n");
	Evalboards.ch1.enableDriver(DRIVER_ENABLE);

	processSpinCoating.state.flags= RUN;

	return 0;
}


int HandlerWait(processCommandArgSpin_t *arg){
	uint8_t i ;
	for (i =1 ; i <= arg->wait ; i ++ ){

		printf("Wait:%d\r\n",i);
		sleep(1); //Espera 1 segundo
	}
	printf("Paso por handler Wait!\r\n");
	return 0;

}

int HandlerStop(processCommandArgSpin_t*	arg) {

	Evalboards.ch1.writeRegister(0,0x66, 	arg->velocity); 		// writing value 0x00000000 = 0 = 0.0 to address 70 = 0x66(PID_VELOCITY_TARGET)-----------------------> Velocity Target
	Evalboards.ch1.enableDriver(DRIVER_DISABLE);
	printf("Proceso Finalizado\r\n");

	return 0;
}



int HandlerStop_without_program(processCommandArgSpin_t*	arg) {


	Evalboards.ch1.writeRegister(0,0x66, 	arg->velocity); 		// writing value 0x00000000 = 0 = 0.0 to address 70 = 0x66(PID_VELOCITY_TARGET)-----------------------> Velocity Target
	Evalboards.ch1.enableDriver(DRIVER_DISABLE);


	printf("Motor Parado\r\n");
	printf("Velocidad:%d\r\n",arg->velocity);

	return 0;
}



int HandlerREADDATA() {

	int32_t i;

	for (i=0; i<32 ;i++){
		printf( "NumberComand:%d\tAceleracion:%d\tVelocidad:%d\tWait:%d\r\n",processSpinCoating.command[i].commandnumber,processSpinCoating.command[i].spin.acceleration,processSpinCoating.command[i].spin.velocity,processSpinCoating.command[i].spin.wait  );
	}

	return 0;
}




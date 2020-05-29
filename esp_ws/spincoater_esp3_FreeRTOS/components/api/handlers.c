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

//Variable Global con la direccion del socket creado, sirve para enviar mensajes al socket desde los handlers
extern int sock_global;



int HandlerRight(processCommandArgSpin_t*	arg) {

	Evalboards.ch1.writeRegister(0,0x5F, 	arg->acceleration); 		// writing value 0x000003E8 = 1000 = 0.0 to address 63 = 0x5F(PID_ACCELERATION_LIMIT)-----------------------> Limite Aceleration 1000 rpm/s
	Evalboards.ch1.writeRegister(0,0x66, 	~(arg->velocity)); 		// writing value 0x00000000 = 0 = 0.0 to address 70 = 0x66(PID_VELOCITY_TARGET)-----------------------> Velocity Target

	return 0;
}

int HandlerRight_without_program(processCommandArgSpin_t*	arg) {

	Evalboards.ch1.enableDriver(DRIVER_ENABLE);

	rampGenerator[1].maxVelocity  = 4000;
	rampGenerator[1].acceleration = (arg->acceleration);
	rampGenerator[1].targetVelocity= (arg->velocity);
	rampGenerator[1].rampVelocity=0;
	rampGenerator[1].rampEnabled=1;

	Evalboards.ch1.writeRegister(0,0x5F, 	arg->acceleration); 		// writing value 0x000003E8 = 1000 = 0.0 to address 63 = 0x5F(PID_ACCELERATION_LIMIT)-----------------------> Limite Aceleration 1000 rpm/s

	while (1){
	if (rampGenerator[1].targetVelocity > rampGenerator[1].rampVelocity ){

	tmc_linearRamp_computeRampVelocity(&rampGenerator[1]);
	Evalboards.ch1.writeRegister(0,0x66,~(rampGenerator[1].rampVelocity));

	//handler cada 10 ms
	//sleep(0.001);
	vTaskDelay( 10 / portTICK_RATE_MS);

	}
	else break;

	}
	tmc_linearRamp_init(rampGenerator);
	printf("Salgo del handler sin procesar rampa!\r\n");
	printf("maxVelocity:%d\r\n", rampGenerator[1].maxVelocity);
	printf("targetVelocity:%d\r\n", rampGenerator[1].targetVelocity);
	printf("rampVelocity:%d\r\n", rampGenerator[1].rampVelocity);
	printf("lastdVRest:%d\r\n", rampGenerator[1].lastdVRest);
	printf("Motor Girando:\r\n");
	printf("Velocidad a Derecha:%d /t  Aceleracion:%d\r\n", arg->velocity,arg->acceleration);

	return 0;
}

int HandlerLeft(processCommandArgSpin_t *arg) {

	rampGenerator[1].maxVelocity = 4000;
	rampGenerator[1].acceleration = arg->acceleration;
	rampGenerator[1].targetVelocity = arg->velocity;
//	rampGenerator[1].rampVelocity = 0;
//	rampGenerator[1].rampEnabled = 1;

	Evalboards.ch1.writeRegister(0, 0x5F, arg->acceleration); // writing value 0x000003E8 = 1000 = 0.0 to address 63 = 0x5F(PID_ACCELERATION_LIMIT)-----------------------> Limite Aceleration 1000 rpm/s

	while (1) {
		if (rampGenerator[1].targetVelocity > rampGenerator[1].rampVelocity) {

			tmc_linearRamp_computeRampVelocity(&rampGenerator[1]);
			Evalboards.ch1.writeRegister(0, 0x66,
					rampGenerator[1].rampVelocity);

			//handler cada 10 ms
			//sleep(0.001);
			vTaskDelay(10 / portTICK_RATE_MS);

		} else if (rampGenerator[1].targetVelocity
				< rampGenerator[1].rampVelocity) {
			tmc_linearRamp_computeRampVelocity(&rampGenerator[1]);
			Evalboards.ch1.writeRegister(0, 0x66,
					rampGenerator[1].rampVelocity);

			//handler cada 10 ms
			//sleep(0.001);
			vTaskDelay(10 / portTICK_RATE_MS);
		} else{
			printf("targetVelocity:%d\r\n", rampGenerator[1].targetVelocity);
			printf("rampVelocity:%d\r\n", rampGenerator[1].rampVelocity);
			break;
		}
	}

	return 0;
}

int HandlerLeft_without_program(processCommandArgSpin_t *arg) {

	Evalboards.ch1.enableDriver(DRIVER_ENABLE);

	rampGenerator[1].maxVelocity = 4000;
	rampGenerator[1].acceleration = arg->acceleration;
	rampGenerator[1].targetVelocity = arg->velocity;
	rampGenerator[1].rampVelocity = 0;
	rampGenerator[1].rampEnabled = 1;

	Evalboards.ch1.writeRegister(0, 0x5F, arg->acceleration); // writing value 0x000003E8 = 1000 = 0.0 to address 63 = 0x5F(PID_ACCELERATION_LIMIT)-----------------------> Limite Aceleration 1000 rpm/s

	while (1) {
		if (rampGenerator[1].targetVelocity > rampGenerator[1].rampVelocity) {

			tmc_linearRamp_computeRampVelocity(&rampGenerator[1]);
			Evalboards.ch1.writeRegister(0, 0x66,
					rampGenerator[1].rampVelocity);

			//handler cada 10 ms
			//sleep(0.001);
			vTaskDelay(10 / portTICK_RATE_MS);

		} else
			break;

	}
	tmc_linearRamp_init(rampGenerator);
	printf("Salgo del handler sin procesar rampa!\r\n");
	printf("maxVelocity:%d\r\n", rampGenerator[1].maxVelocity);
	printf("targetVelocity:%d\r\n", rampGenerator[1].targetVelocity);
	printf("rampVelocity:%d\r\n", rampGenerator[1].rampVelocity);
	printf("lastdVRest:%d\r\n", rampGenerator[1].lastdVRest);
	printf("acceleration:%d\r\n", rampGenerator[1].acceleration);
	printf("Motor Girando:\r\n");
	printf("Velocidad a Izquierda:%d\tAceleracion:%d\r\n", arg->velocity,
			arg->acceleration);

	return 0;
}


int HandlerRun(void) {

	printf("Motor Encendido\r\n");
	Evalboards.ch1.enableDriver(DRIVER_ENABLE);

	tmc_linearRamp_init(rampGenerator);

	rampGenerator[1].rampVelocity = 0;
	rampGenerator[1].rampEnabled = 1;


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


	printf("Motor Parado!\r\n");
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




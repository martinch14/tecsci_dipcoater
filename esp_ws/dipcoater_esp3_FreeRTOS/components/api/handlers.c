/**************************************************************************************************
**  (c) Copyright 2019: Martin Abel Gambarotta <magambarotta@gmail.com>
**  This file is part of DipCoater_Tecsci.
**
**  DipCoater_Tecsci is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  DipCoater_Tecsci is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with DipCoater_Tecsci.  If not, see <https://www.gnu.org/licenses/>.
*************************************************************************************************/


/** @file 	handlers.c
 ** @brief 	Implementacion
 **
 **| REV | YYYY.MM.DD | Autor           | Descripción de los cambios                              |
 **|-----|------------|-----------------|---------------------------------------------------------|
 **|   1 | 2020.05.28 | magambarotta    | Version inicial 									      |
 ** @addtogroup aplicacion
 ** @{ */


/*=====[Inclusion de su propia cabecera]=====================================*/
#include "handlers.h"



/*=====[Inclusiones de dependencias de funciones privadas]===================*/
#include <stdio.h>
//#ifndef TECSCI_ARCH_X86
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

#include "app_main_dipcoater.h"

#include <unistd.h>




/*=====[Macros de definicion de constantes privadas]=========================*/
// Constante de dezplazamiento lineal   , expresadda en mm/micropasos         ->   una vuelta   1 paso  ==   0.00007851  mm     -->    (una vuelta) 51200   ==   4.02 mm
#define  K_DEZPLAZAMIENTO_LINEAL   0.00007851   //  78.51E-6


/*=====[Macros estilo funcion privadas]======================================*/
#define sleep(x) vTaskDelay((x) * 1000 / portTICK_RATE_MS)


/*=====[Definiciones de tipos de datos privados]=============================*/


/*=====[Definiciones de Variables globales publicas externas]================*/
//Variable Global con la direccion del socket creado, sirve para enviar mensajes al socket desde los handlers
extern int sock_global;


/*=====[Definiciones de Variables globales publicas]=========================*/
/*=====[Definiciones de Variables globales privadas]=========================*/
/*=====[Prototipos de funciones privadas]====================================*/

//Control del limite mecanico (recorrido) de la maquina, los valores aqui dependen del largo del eje Z
static uint8_t controlLimit(void);
//static void funPrivada(void);
/*=====[Implementaciones de funciones publicas]==============================*/



//Handler Functions
int HandlerCeroMachine(processCommandArgSpin_t *arg) {

	int32_t lectura, lecturaActualTarget;

	printf("Buscando Cero Machine!!\t");
	printf("velocidad:%d\t", arg->velocity);
	printf("aceleracion:%d\r\n", arg->acceleration);

	Evalboards.ch1.enableDriver(DRIVER_ENABLE);

	// Activo flag para encender STALLGUARD
	Evalboards.ch1.writeRegister(0, 0x34, 0x00000400);
	Evalboards.ch1.writeRegister(0, TMC5130_RAMPMODE, 0x00000000);

	Evalboards.ch1.readRegister(0, 0x21, &lectura);
	printf("Posicion ACTUAL TARGET :%d\r\n", lectura);

	Evalboards.ch1.writeRegister(0, 0x2D, 0x00000000);
	Evalboards.ch1.readRegister(0, 0x2D, &lectura);
	printf("Posicion XTARGET :%d\r\n", lectura);

	Evalboards.ch1.writeRegister(0, 0x2D, 0xFFB1E000);
	Evalboards.ch1.readRegister(0, 0x2D, &lectura);
	printf("Posicion XTARGET :%d\r\n", lectura);


	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);
	// Seteo desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  arg->acceleration);


	//Detecto flag de stallguard
	while (1) {

		Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
		Evalboards.ch1.readRegister(0, 0x21, &lecturaActualTarget);
		printf("Posicion ActualTarget: :%d\r\n", lecturaActualTarget);

		if (lectura & 0x0000040) {

			Evalboards.ch1.writeRegister(0, TMC5130_RAMPMODE, 0x00000000);
			Evalboards.ch1.writeRegister(0, 0x21, 0x00000000);
			Evalboards.ch1.writeRegister(0, 0x2D, 127372);
			Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
			Evalboards.ch1.writeRegister(0, 0x34, 0x00000000);
			Evalboards.ch1.writeRegister(0, 0x34, 0x00000400);
			Evalboards.ch1.writeRegister(0, 0x34, 0x00000000);

			break;
		} else
			sleep(0.2);

	}

	//Una vez detectado el cero realizo un movimiento fijo con 127372 (10 mm) pasos hacia abajo, detecto el flag que detecta XACTUAL=XTARGET
	while (1) {

		Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
		if (lectura & 0x00000200)
			break;
		else
			sleep(0.1);

	}

	Evalboards.ch1.enableDriver(DRIVER_DISABLE);

	printf("Cero Machine Detectado!!!!!\r\n");
	Evalboards.ch1.readRegister(0, 0x2D, &lectura);
	printf("Posicion XTARGET :%d\r\n", lectura);

	Evalboards.ch1.readRegister(0, 0x21, &lectura);
	printf("Posicion ACTUAL_TARGET :%d\r\n", lectura);


	return 0;

}

int HandlerSpin(processCommandArgSpin_t*	arg) {
	printf("Spinning!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);
	sleep(2);
	return 0;
}



/*********************************************************UP*********************************************************************************************************/

int HandlerUp(processCommandArgSpin_t*	arg) {

	int32_t lectura;

	printf("\r\n");
	printf("Subiendo muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\t",arg->acceleration);


	Evalboards.ch1.readRegister(0, 0x2D, &lectura);
	printf("Posicion ATARGET :%d\r\n", lectura);


	// Seteo de registro XTARGET
	Evalboards.ch1.writeRegister(0, 0x2D, lectura - arg->displacement_z );

	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);

	// Seteo desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  arg->acceleration);

	//Una vez detectado el cero realizo un movimiento fijo con 512000 pasos hacia abajo, detecto el flag que detecta XACTUAL=XTARGET
	while (processDipCoating.config.status) {

		Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
		if (lectura & 0x00000200)
			break;
		else
			sleep(0.1);

	}

	Evalboards.ch1.readRegister(0, 0x2D, &lectura);
	printf("Posicion ATARGET :%d\r\n", lectura);

	return 0;
}

//Subida dentro del proceso de ejecucion de programas

int HandlerUpLoop(processCommandArgSpin_t*	arg) {
	int32_t lectura;

	printf("Subiendo muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);

	//Leo Actual

	Evalboards.ch1.readRegister(0, 0x21, &lectura);

	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);

	//desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  arg->acceleration);


	// Seteo de registro XTARGET
	Evalboards.ch1.writeRegister(0, 0x2D, lectura - processDipCoating.config.displacement_delta_sample  );


	//Una vez detectado el cero realizo un movimiento fijo con 512000 pasos hacia abajo, detecto el flag que detecta XACTUAL=XTARGET
	while (processDipCoating.config.status) {

		Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
		if (lectura & 0x00000200)
			break;
		else
			sleep(0.1);
	}
	return 0;
}


int HandlerUp_without_program(processCommandArgSpin_t*	arg) {

	int32_t lectura;

	printf("Subiendo muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);

	processDipCoating.config.status=1;

	Evalboards.ch1.enableDriver(DRIVER_ENABLE);

	//Leo Actual

	Evalboards.ch1.readRegister(0, 0x21, &lectura);

	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);

	// Seteo desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  arg->acceleration);


	// Seteo de registro XTARGET
	Evalboards.ch1.writeRegister(0, 0x2D, lectura - arg->displacement_z );

	// Detecto el flag que detecta XACTUAL=XTARGET y apago driver
	while (processDipCoating.config.status) {
		Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
		if (lectura & 0x00000200){
			Evalboards.ch1.enableDriver(DRIVER_DISABLE);
			break;
		}
		else{
			if (controlLimit())break;
			else sleep(0.1);
		}
	}

	processDipCoating.config.status=0;

	return 0;
}


/*********************************************************DOWN*********************************************************************************************************/


int HandlerDown(processCommandArgSpin_t*	arg) {
	int32_t lectura;

	printf("\r\n");
	printf("Bajando muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\t",arg->acceleration);

	// Seteo de registro XTARGET
	arg->displacement_z = processDipCoating.config.displacement_to_sample;

	printf("desplazamiento hasta fluido en pasos:%d\r\n",arg->displacement_z);

	Evalboards.ch1.readRegister(0, 0x21, &lectura);
	printf("Posicion ACTUAL TARGET :%d\r\n", lectura);


	Evalboards.ch1.writeRegister(0, 0x2D, arg->displacement_z );

	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);

	// Seteo desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  arg->acceleration);

	//Una vez detectado el cero realizo un movimiento fijo con 512000 pasos hacia abajo, detecto el flag que detecta XACTUAL=XTARGET
	while (processDipCoating.config.status) {

		Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
		if (lectura & 0x00000200)
			break;
		else{
			sleep(0.1);
		}
	}
	Evalboards.ch1.readRegister(0, 0x21, &lectura);
	printf("Posicion ACTUAL TARGET :%d\r\n", lectura);

	return 0;
}


int HandlerDownLoop(processCommandArgSpin_t*	arg) {
	int32_t lectura;

	printf("Bajando muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);

	//Leo Actual

	Evalboards.ch1.readRegister(0, 0x21, &lectura);

	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);

	//desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  arg->acceleration);

	// Seteo de registro XTARGET
	Evalboards.ch1.writeRegister(0, 0x2D, lectura + processDipCoating.config.displacement_delta_sample  );

	//Una vez detectado el cero realizo un movimiento fijo con 512000 pasos hacia abajo, detecto el flag que detecta XACTUAL=XTARGET
	while (processDipCoating.config.status) {

		Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
		if (lectura & 0x00000200)
			break;
		else
			sleep(0.1);
	}
	return 0;
}


int HandlerDown_without_program(processCommandArgSpin_t*	arg) {
	int32_t lectura;

	printf("Bajando muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);


	processDipCoating.config.status=1;

	Evalboards.ch1.enableDriver(DRIVER_ENABLE);

	//Leo Actual

	Evalboards.ch1.readRegister(0, 0x21, &lectura);

	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);

	// Seteo desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  arg->acceleration);

	// Seteo de registro XTARGET
	Evalboards.ch1.writeRegister(0, 0x2D, lectura + arg->displacement_z );

	// Detecto el flag que detecta XACTUAL=XTARGET y apago driver
		while (processDipCoating.config.status) {

			Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
			if (lectura & 0x00000200){
				Evalboards.ch1.enableDriver(DRIVER_DISABLE);
				break;
			}
			else{
				if (controlLimit())break;
				else sleep(0.1);

				//sleep(0.5);
				//vTaskDelay(0.2 / portTICK_RATE_MS);
			}
		}

	processDipCoating.config.status=0;
	printf("BAJANDO ESP32D!\r\n");

	return 0;

}

int HandlerWait(processCommandArgSpin_t*	arg) {
	printf("Muestra en espera!!\r\n");
//	Evalboards.ch1.rotate(0,0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)

	sleep(2);
	return 0;
}

int HandlerWaitDown(processCommandArgSpin_t*	arg) {
	printf("Muestra en espera!!\r\n");
//	Evalboards.ch1.rotate(0,0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)

	sleep(processDipCoating.command[3].argument.wait.time);
	return 0;
}


int HandlerWaitUp(processCommandArgSpin_t*	arg) {
	printf("Muestra en espera!!\r\n");
//	Evalboards.ch1.rotate(0,0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)

	sleep(processDipCoating.command[5].argument.wait.time);
	return 0;
}


int HandlerRun() {

	Evalboards.ch1.enableDriver(DRIVER_ENABLE);
//	vTaskDelay(250 / portTICK_RATE_MS);
	return 0;
}


int HandlerStop(processCommandArgSpin_t*	arg) {
	printf("Comando Stop!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);

	printf("handler STOP Command a cola\r\n ");
	// Seteo aceleracion y velocidad en cero
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);
	Evalboards.ch1.enableDriver(DRIVER_DISABLE);

	return 0;
}


int HandlerFinish(processCommandArgSpin_t*	arg) {

	Evalboards.ch1.enableDriver(DRIVER_DISABLE);
	printf("Proceso Finalizado - Motor Apagado\r\n");
	printf("\r\n");

	return 0;
}

int HandlerREADDATA() {

	int32_t lectura1,lectura2, i;

	int32_t lectura;

	/*Lectura de datos configurados para el funcionamiento de la maquina*/

	for (i=0; i<8 ;i++){
		printf( "%d %d %d %d \r\n" ,processDipCoating.command[i].commandnumber,processDipCoating.command[i].argument.spin.velocity ,processDipCoating.command[i].argument.spin.acceleration,processDipCoating.command[i].argument.spin.displacement_z  );
	}
	printf ("DELTADIP:%d\r\n",processDipCoating.config.displacement_delta_sample);
	printf ("CERO_SAMPLE:%d\r\n",processDipCoating.config.displacement_to_sample);
	printf ("STATUS:%d\r\n",processDipCoating.config.status);

	//Leo Actual

	Evalboards.ch1.readRegister(0, 0x21, &lectura);
	printf("Posicion ACTUAL TARGET:%d\r\n", lectura);

	Evalboards.ch1.readRegister(0, 0x2D, &lectura);
	printf("Posicion  XTARGET:%d\r\n", lectura);


	return 0;
}


int HandlerENA_DRIVER() {
	Evalboards.ch1.enableDriver(DRIVER_ENABLE);
	printf("\r\n");
	return 0;
}

int HandlerDIS_DRIVER() {
	Evalboards.ch1.enableDriver(DRIVER_DISABLE);
	printf("Driver Deshabilitado!\r\n");
	return 0;
}



int HandlerCERO_SAMPLE() {

	int32_t lectura;
	static char datos[32];

	printf("\r\n");
	Evalboards.ch1.readRegister(0, 0x21, &lectura);     //
	printf("CERO_SAMPLE %d\r\n", lectura / 12737  );              //12737   --> 1 mm

	if (sock_global > 0){
		sprintf(datos,"CERO_SAMPLE %d\r\n", lectura / 12737);
		send(sock_global, &datos, sizeof(datos) , 0);
	}

	return lectura;
}


int HandlerDELTADIP() {

	printf("Profundidad de dip cargada!\r\n");
	return 0;
}


//Funcion que limita los dezplazamientos fuera de la zona segura de trabajo    en TOP   5 mm en BOTTON algunos cm
// LIMITES EN EL RECORRIDO

uint8_t controlLimit(void){
	int32_t lectura,lecturaXtarget;

	Evalboards.ch1.readRegister(0, 0x21, &lectura);
	printf("Posicion ACTUAL :%d\r\n", lectura);
	Evalboards.ch1.readRegister(0, 0x2D, &lecturaXtarget);
	printf("Posicion XTARGET :%d\r\n", lecturaXtarget);



		if (lectura  <  0x0000F8C6  ){
				Evalboards.ch1.enableDriver(DRIVER_DISABLE);
				ProcessCeroMachineCommand();
				return 1;
			}
		else if ( lectura > 0x003E3186){
				Evalboards.ch1.enableDriver(DRIVER_DISABLE);
				ProcessCeroMachineCommand();
				return 1;
			}
		else return 0;
}


int HandlerRESET() {

	esp_restart();
	return 0;
}


/** @} Final de la definición del modulo para doxygen */








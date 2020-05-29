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


/** @file 	process.h
 ** @brief 	Declaracion
 **
 **| REV | YYYY.MM.DD | Autor           | Descripción de los cambios                              |
 **|-----|------------|-----------------|---------------------------------------------------------|
 **|   1 | 2020.05.28 | magambarotta    | Version inicial 									      |
 ** @addtogroup aplicacion
 ** @{ */


/*=====[Evitar inclusion multiple comienzo]==================================*/

#ifndef COMPONENTS_API_INCLUDE_PROCESS_H_
#define COMPONENTS_API_INCLUDE_PROCESS_H_


/*=====[C++ comienzo]========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Inclusiones de dependencias de funciones publicas]===================*/
#include <stdint.h>



/*  Estructura del Proceso + Comando
 *
 *
 *
 *
 *						|Arreglo de comandos del tipo  processCommand_t |
 *						|												|
 *						|-----------------------------------------------|
 *						|												|
 *						|Estructura de configuracion  processConfig_t	|
 * Estructura process_t	|												|
 *						|-----------------------------------------------|
 *						|												|
 *						|Estructura de estado  processState_t			|
 *						|												|
 *						|-----------------------------------------------|
 *
 *
 *
 */

/*=====[Macros de definicion de constantes publicas]=========================*/
#define _FLAG_ERROR          	1<< 8

/*=====[Macros estilo funcion publicas]======================================*/
//#define printInt(printer,number) printIntFormat((printer),(number),(DEC_FORMAT))



/*=====[Definiciones de tipos de datos publicos]=============================*/

typedef struct {
	float values[4];
} processCommandArg4Float_t;

typedef struct {
	int val;
} processCommandArgUint8_t;

typedef struct {
	uint32_t values[4];
} processCommandArg4Int32_t;

typedef struct {
	uint32_t velocity;
	uint32_t acceleration;
	uint32_t displacement_z;

} processCommandArgSpin_t;

typedef struct {
	float time;
} processCommandArgWait_t;

typedef enum {
	PROCESS_COMMAND_CERO_MACHINE,	/*0*/
	PROCESS_COMMAND_SPIN,			/*1*/
	PROCESS_COMMAND_UP,				/*2*/
	PROCESS_COMMAND_DOWN,			/*3*/
	PROCESS_COMMAND_WAIT,			/*4*/
	PROCESS_COMMAND_STOP,			/*5*/
	PROCESS_COMMAND_START,			/*6*/
	PROCESS_COMMAND_FINISH,			/*7*/
	PROCESS_COMMAND_LOOP,			/*8*/
	PROCESS_COMMAND__N,				/*9*/
	PROCESS_COMMAND__EMPTY,			/*10*/
	PROCESS_COMMAND_RUN,
	PROCESS_COMMAND_UPFAST,
	PROCESS_COMMAND_UPSLOW,
	PROCESS_COMMAND_DOWNFAST,
	PROCESS_COMMAND_DOWNSLOW,
	PROCESS_COMMAND_READDATA,
	PROCESS_COMMAND_ENA_DRIVER,
	PROCESS_COMMAND_DIS_DRIVER,
	PROCESS_COMMAND_CERO_SAMPLE,
	PROCESS_COMMAND_DELTADIP,
	PROCESS_COMMAND_RESET


} proccesCommandNumber_t;


typedef int (*processCommandHandler_t)(processCommandArgSpin_t* arg);


typedef union {
	processCommandArgSpin_t spin;
	processCommandArgWait_t wait;
	processCommandArg4Float_t floats;
	processCommandArgUint8_t value;
} processCommandArgument_t;

typedef struct {
	proccesCommandNumber_t commandnumber;
	processCommandHandler_t fpcommandhandler;

	union {
		processCommandArgSpin_t spin;
		processCommandArgWait_t wait;
		processCommandArg4Float_t floats;
		processCommandArgUint8_t value;
	} argument;

} processCommand_t;

typedef struct {

	/*Configuracion ->  status  -> 1  cuando se esta ejecutando un programa    -> 0 cunado no se esta ejecutando ningun programa        desplazamiento en eje Y segun pasos del motor   */
	uint8_t	status;


	/*Todos estas variables deben estar en 1 antes de que se ejecute cada programa*/
	uint8_t cero_machine_ok;
	uint8_t cero_sample_ok;
	uint8_t delta_dip_ok;


	uint32_t displacement_delta_sample;    	/*Depende del tamaño de la muestra*/
	uint32_t displacement_to_sample;    	/*Depende del tamaño del recipiente*/



} processConfig_t;

typedef struct {

	uint8_t commandIndex;
	uint8_t commandActualIndex;
	uint8_t flags;

} processState_t;

typedef struct {
	processConfig_t 	config; 		/*Parametros estructurales de la maquina   -  Limites*/
	processCommand_t*	command;
	processState_t 		state; 			/*Exteriorizar el estado actual*/
	uint8_t				commandlen;
} process_t;


/*=====[Prototipos de funciones publicas]====================================*/

/*
* @brief	funcion_thread_serial_educiaa  -> Esta funcion hace un polling al sobre el puerto Serial
* 		conectado a la EDUCIAA y si llega un mensaje lo retransmite a traves de un FD hacia el TCP client
* @param[]  Referencia a estructura del tipo pthread_t
* @returns  0 Creacion correcta
*/
void ProcessInit(process_t *process);
void ProcessRun(process_t *process);
void ProcessLoadProgramStandard(process_t *process);
void ProcessLoadProgramCustom(process_t *process);
void ProcessSetProgramCustom();
void ProcessSetProgramStandard();
void ProcessCeroMachineCommand();
void ProcessUpFastCommand();
void ProcessUpCommand();
void ProcessUpSlowCommand();
void ProcessDownFastCommand();
void ProcessDownCommand();
void ProcessDownSlowCommand();
void ProcessStopCommand();
void ProcessRunCommand();
void ProcessREADDATACommand();
void ProcessENA_DRIVERCommand();
void ProcessDIS_DRIVERCommand();

void ProcessCERO_SAMPLECommand();
void ProcessDELTADIPCommand();

void ProcessRESETCommand();


void ProcessCommand();



/*=====[Prototipos de funciones publicas futuro desarrollo]====================================*/
//void ProcessLoadProgramDinamic(process_t *process);
//void ProcessCleanDinamic();
//int  ProcessDinamicLen();
//void ProcessAddSetProgramDinamic();

/*=====[C++ fin]=============================================================*/

#ifdef __cplusplus
}
#endif

/** @} Final de la definición del modulo para doxygen */

/*=====[Evitar inclusion multiple fin]=======================================*/

#endif /* COMPONENTS_API_INCLUDE_PROCESS_H_ */

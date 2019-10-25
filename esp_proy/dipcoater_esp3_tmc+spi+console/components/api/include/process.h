/*
 * process.h
 *
 *  Created on: 6 sep. 2019
 *      Author: martin
 */

#ifndef COMPONENTS_API_INCLUDE_PROCESS_H_
#define COMPONENTS_API_INCLUDE_PROCESS_H_

#include <stdint.h>

/*DEFINED VALUES*/
#define _FLAG_ERROR          	1<< 8

/*DEFINED STRUCTS*/

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
	int velocity;
	int acceleration;
	int test;

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
} proccesCommandNumber_t;


typedef int (*processCommandHandler_t)(processCommandArgSpin_t* arg);


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

	/*Configuracion de Variables con el tamaño de la muestra y el tamaño del recipiente   en mm   y desplazamiento en eje Y segun pasos del motor   */
	uint16_t	samplesize;
	uint16_t 	recipientsize;
	uint32_t 	axisYDisplacement;    /*segun parametro constructivo de la maquina */

} processConfig_t;

typedef struct {

	uint8_t commandIndex;
	uint8_t flags;

} processState_t;

typedef struct {
	processConfig_t 	config; 		/*Parametros estructurales de la maquina   -  Limites*/
	processCommand_t*	command;
	processState_t 		state; 			/*Exteriorizar el estado actual*/
	uint8_t				commandlen;
} process_t;


//FUNCTION DECLARATION
void ProcessInit(process_t *process);
void ProcessRun(process_t *process);
void ProcessLoadProgramStandard(process_t *process);
void ProcessLoadProgramCustom(process_t *process);
//void ProcessLoadProgramDinamic(process_t *process);
//void ProcessCleanDinamic();
//int  ProcessDinamicLen();
//void ProcessAddSetProgramDinamic();
void ProcessSetProgramCustom();
void ProcessSetProgramStandard();
void ProcessUpFastCommand();
void ProcessUpCommand();
void ProcessUpSlowCommand();
void ProcessDownFastCommand();
void ProcessDownCommand();
void ProcessDownSlowCommand();
void ProcessStopCommand();
void ProcessRunCommand();


#endif /* COMPONENTS_API_INCLUDE_PROCESS_H_ */

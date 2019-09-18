/*
 * process.h
 *
 *  Created on: 6 sep. 2019
 *      Author: martin
 */

#ifndef COMPONENTS_API_INCLUDE_PROCESS_H_
#define COMPONENTS_API_INCLUDE_PROCESS_H_


#include <stdint.h>
#define _FLAG_ERROR          	1<< 8


typedef struct {
	float values[4];
} processCommandArg4Float_t;

typedef struct {
	uint32_t values[4];
} processCommandArg4Int32_t;

typedef struct {
	int velocity;
	int acceleration;

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
	PROCESS_COMMAND__N,				/*7*/
} proccesCommandNumber_t;

typedef struct {
	proccesCommandNumber_t commandnumber;
	union {
		processCommandArgSpin_t spin;
		processCommandArgWait_t wait;
		processCommandArg4Float_t floats;
	} argument;
} processCommand_t;

typedef struct {

	/*Largo con la cantidad de comandos del cada proceso*/

} processConfig_t;

typedef struct {

	uint8_t commandIndex;
	uint8_t flags;

} processState_t;

typedef struct {
	processConfig_t 	config; 		/*Parametros estructurales de la maquina   -  Limites*/
	processCommand_t*	command;
	uint8_t				commandlen;
	processState_t 		state; 			/*Exteriorizar el estado actual*/
} process_t;


typedef enum {
	PROCESS_RET_OK,
	PROCESS_RET_ERROR,
}processReturn_e;



void ProcessInit(process_t *process);
void ProcessNextCommand(process_t *process);
void ProcessCommandAdd(process_t *process, processCommand_t *cmd);
void ProcessCommandUpdate(process_t *process);
void ProcessLoadProgramStandar(process_t *process);
processReturn_e  ProcessRun(process_t *process);

void handlerEmpty(void);

/* Proceso_cargar_configuracion   */
/* Proceso_leer_configuracion   */
/*Estructura con la configuracion del usuario para cargar sobre el proceso*/



#endif /* COMPONENTS_API_INCLUDE_PROCESS_H_ */

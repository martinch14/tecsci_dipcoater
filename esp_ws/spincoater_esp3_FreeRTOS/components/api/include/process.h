/*
 * process.h
 *
 *  Created on: 6 sep. 2019
 *      Author: martin
 */

#ifndef COMPONENTS_API_INCLUDE_PROCESS_H_
#define COMPONENTS_API_INCLUDE_PROCESS_H_

#include <stdint.h>



/*  Estructura del Proceso + Comando
 *
 *
 *
 *
 *						|Arreglo de comando  processCommand_t			|
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

/*DEFINED VALUES*/
#define _FLAG_ERROR          	1<< 8

/*DEFINED STRUCTS*/

typedef struct {
	uint32_t velocity;
	uint32_t acceleration;
	uint32_t wait;

} processCommandArgSpin_t;


typedef enum {

	PROCESS_COMMAND_RIGHT,			/*0*/
	PROCESS_COMMAND_RIGHTFAST,		/*1*/
	PROCESS_COMMAND_RIGHTSLOW,		/*2*/
	PROCESS_COMMAND_LEFT,			/*3*/
	PROCESS_COMMAND_LEFTFAST,		/*4*/
	PROCESS_COMMAND_LEFTSLOW,		/*5*/
	PROCESS_COMMAND_READDATA,		/*6*/
	PROCESS_COMMAND_RUN,			/*7*/
	PROCESS_COMMAND_STOP,			/*8*/
	PROCESS_COMMAND_WAIT,			/*9*/


} proccesCommandNumber_t;


typedef int (*processCommandHandler_t)(processCommandArgSpin_t* arg);


/*Estructura para definir un arreglos de comandos con datos y funciones asociadas*/
typedef struct {
	proccesCommandNumber_t commandnumber;
	processCommandHandler_t fpcommandhandler;
	processCommandArgSpin_t spin;

} processCommand_t;

typedef struct {

	uint8_t		status; /*Configuracion ->  STATUS=1 cuando se esta ejecutando un programa    STATUS=0 cuando NO se esta ejecutando ningun programa*/
	uint8_t 	direction;  /*0   Direccion de giro Izquierda       1  Direccion de giro Derecha*/
} processConfig_t;


typedef enum {
	STOP,
	RUN,
}flagRun_t;  /*Flag que define si se esta corriendo o no un proceso */

typedef struct {

	uint8_t commandIndex;       /*Cantidad de comandos totales del proceso*/
	uint8_t commandActualIndex; /*Comando actual del proceso ejecutandose*/
	flagRun_t flags;

} processState_t;

typedef struct {
	processConfig_t 	config; 		/*Parametros estructurales de la maquina   -  Limites*/
	processCommand_t*	command;
	processState_t 		state; 			/*Exteriorizar el estado actual*/

} process_t;


//FUNCTION DECLARATION
void ProcessInit(process_t *process);
void ProcessRun(process_t *process);
void ProcessRightFastCommand();
void ProcessRightCommand();
void ProcessRightSlowCommand();
void ProcessLeftFastCommand();
void ProcessLeftCommand();
void ProcessLeftSlowCommand();
void ProcessStopCommand();
void ProcessRunCommand();
void ProcessREADDATACommand();
void ProcessCommand();

#endif /* COMPONENTS_API_INCLUDE_PROCESS_H_ */

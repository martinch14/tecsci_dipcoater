/*
 * command.c
 *
 *  Created on: 1 Abril. 2020
 *  Author: Martin Gambarotta
 *
*/


/*PROCESS STANDARD HANDLERS*/

/*Handler to LOAD the standard process */
#include <stdio.h>

#include "process.h"
#include "tinysh.h"
#include "mod_queue.h"
#include "command.h"
#include "app_main_spincoater.h"


extern process_t processSpinCoating;

//#define MAX_ESTATIC_COMMAND 	8




/*Handler to set up APP command of the custom Process
 4 arguments must be passed: command number (see manual), velocity, acceleration and the test parameter */
void CommandSETCOMMANDCUSTOMPROGRAMAPPHandler(int argc, char **argv) {
	processCommand_t aux_process_comand;
	uint8_t i,j, numberDinamicStep;

	printf("\r\n");

	/*SETCUSTOMPROGRAMAPP 2 XX XX XX XX XX XX   	*/
	/*
	 *
	 * 	2 representa la cantidad de pasos  , puede ser la cantidad que el usuario desee segun el programa
	*	Casa paso es definido por 3 valores   aceleracion velocidad tiempo_espera
	*
	*	Por lo tanto la cantidad de comando a pasar deberan ser , por ejemplo para este caso:
	*	Con 1 steps ->  se pasan 5  argumentos totales
	*	Con 2 steps ->  se pasan 8  argumentos totales
	*   Con 3 steps ->  se pasan 11 argumentos totales
	*   Con 4 steps ->  se pasan 14 argumentos totales
	*
	*	2+ canttdad_step *3
	*
	*
	*	SETCUSTOMPROGRAMAPP 	3		 XX 	XX 		XX		XX 		XX 		XX		XX 		XX 		XX
	*		(0)					(1)		(2)  	(3) 	(4)		(5)		(6)		(7)		(8)		(9)		(10)
	*									|__________________|    |__________________|	|__________________|
	*
	*
	*
	*
	*	ACELERACION VELOCIDAD ESPERA
	*
	*
	*/



	if ( argc != (2 + (tinysh_get_arg_int(argc, argv,1)*3) ) ){
		printf ("Comando mal ensamblado!\r\n");
	}

	else {


		numberDinamicStep= tinysh_get_arg_int(argc, argv,1);

		processSpinCoating.state.commandIndex = numberDinamicStep*2 +1 ;   // +1 por el comando stop que se agraga al final

		for (i=0 , j=0; i < (numberDinamicStep*2) ; i=i+2, j=j+3){
			processSpinCoating.command[i].commandnumber= PROCESS_COMMAND_LEFT;
			processSpinCoating.command[i].fpcommandhandler= HandlerLeft;
			processSpinCoating.command[i].spin.acceleration= tinysh_get_arg_int(argc, argv,j+2) ;
			processSpinCoating.command[i].spin.velocity=  tinysh_get_arg_int(argc, argv,j+3) ;

			processSpinCoating.command[i+1].commandnumber= PROCESS_COMMAND_WAIT;
			processSpinCoating.command[i+1].fpcommandhandler= HandlerWait;
			processSpinCoating.command[i+1].spin.wait=  tinysh_get_arg_int(argc, argv,j+4) ;

		}

		processSpinCoating.command[numberDinamicStep*2 ].commandnumber= PROCESS_COMMAND_STOP;
		processSpinCoating.command[numberDinamicStep*2 ].fpcommandhandler= HandlerStop;

		printf ("Datos cargados!\r\n");

	}

}



///////////////////////////////////////////////////////////////////////******Comandos que mandan por cola *****************////////////////////////////////////////////////////


/*SINGLE MOVEMENT HANDLERS*/
/*this tiny handlers only call the corresponding process function*/
void CommandRIGHTFASTHandler(int argc, char **argv) {

	printf("\r\n");
	processCommand_t aux_process_comand;
	aux_process_comand.commandnumber= PROCESS_COMMAND_RIGHTFAST;

	if (processSpinCoating.config.status == 0){
		modQueue_Write(&queueconsolareception, &aux_process_comand);
	}
	else printf("Ejecutando, comando descartado!\r\n");

}

void CommandRIGHTHandler(int argc, char **argv) {

	processCommand_t aux_process_comand;
	aux_process_comand.commandnumber= PROCESS_COMMAND_RIGHT;

	printf("\r\n");
	if (processSpinCoating.config.status == 0){
		modQueue_Write(&queueconsolareception, &aux_process_comand);
	}
	else printf("Ejecutando, comando descartado!\r\n");

}

void CommandRIGHTSLOWHandler(int argc, char **argv) {


	processCommand_t aux_process_comand;
	aux_process_comand.commandnumber= PROCESS_COMMAND_RIGHTSLOW;

	printf("\r\n");
	if (processSpinCoating.config.status == 0){
		modQueue_Write(&queueconsolareception, &aux_process_comand);
	}
	else printf("Ejecutando, comando descartado!\r\n");

}

void CommandLEFTFASTHandler(int argc, char **argv) {

	processCommand_t aux_process_comand;
	aux_process_comand.commandnumber = PROCESS_COMMAND_LEFTFAST;

	printf("\r\n");
	if (processSpinCoating.config.status == 0) {
		modQueue_Write(&queueconsolareception, &aux_process_comand);
	} else
		printf("Ejecutando, comando descartado!\r\n");

}

void CommandLEFTHandler(int argc, char **argv) {

	processCommand_t aux_process_comand;
	aux_process_comand.commandnumber = PROCESS_COMMAND_LEFT;

	printf("\r\n");
	if (processSpinCoating.config.status == 0) {
		modQueue_Write(&queueconsolareception, &aux_process_comand);
	} else
		printf("Ejecutando, comando descartado!\r\n");

}

void CommandLEFTSLOWHandler(int argc, char **argv) {

	processCommand_t aux_process_comand;
	aux_process_comand.commandnumber = PROCESS_COMMAND_LEFTSLOW;

	printf("\r\n");
	if (processSpinCoating.config.status == 0) {
		modQueue_Write(&queueconsolareception, &aux_process_comand);
	} else
		printf("Ejecutando, comando descartado!\r\n");



}


void CommandSTOPHandler(int argc, char **argv) {

	processCommand_t aux_process_comand;
	aux_process_comand.commandnumber = PROCESS_COMMAND_STOP;

	printf("\r\n");
	if (processSpinCoating.config.status == 0) {
		modQueue_Write(&queueconsolareception, &aux_process_comand);
	} else
		printf("Ejecutando, comando descartado!\r\n");



}


void CommandRUNHandler(int argc, char **argv) {

	processCommand_t aux_process_comand;
	aux_process_comand.commandnumber = PROCESS_COMMAND_RUN;

	printf("\r\n");
	if (processSpinCoating.config.status == 0) {

		modQueue_Write(&queueconsolareception, &aux_process_comand);
	} else
		printf("Ejecutando, comando descartado!\r\n");

}




void CommandREADDATAHandler(int argc, char **argv) {

	processCommand_t aux_process_comand;
	aux_process_comand.commandnumber= PROCESS_COMMAND_READDATA;

	printf("\r\n");
	if (processSpinCoating.config.status == 0){
		modQueue_Write(&queueconsolareception, &aux_process_comand);
	}
	else printf("Ejecutando, comando descartado!\r\n");


}







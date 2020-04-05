/*
 * process.c
 *
 *  Created on: 1 Abril. 2020
 *  Author: Martin Gambarotta
 *
*/


#include <stdio.h>
#include "process.h"

#include "app_main_spincoater.h"
#include "handlers.h"
#include "mod_queue.h"



#define MAX_ESTATIC_COMMAND 	3
#define MAX_DINAMIC_COMMAND 	32

/**
 * @brief Function to initialize the Process
 * Function that receives a pointer to process, and initialize the structure
 * of the process.
 *
 * @param pointer to process
 * @return nothing
 */

//PROCESSES AVAILABLE


/*CmdProcessCustom, the arguments can be modified to make a load and run after*/
processCommand_t cmdProcessCustomLeft[MAX_ESTATIC_COMMAND] = {

		/*Al setear el custom program arrancamos desde el comando numero 0*/

		{ .commandnumber = PROCESS_COMMAND_LEFT, 		.spin.velocity = 2000,	.spin.acceleration = 1000 , 	.fpcommandhandler = HandlerLeft },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.spin.wait = 10,									.fpcommandhandler = HandlerWait},
		{ .commandnumber = PROCESS_COMMAND_STOP,		.spin.velocity = 0,		.spin.acceleration = 0, 		.fpcommandhandler = HandlerStop},

};

processCommand_t cmdProcessCustomRight[MAX_ESTATIC_COMMAND] = {

		/*Al setear el custom program arrancamos desde el comando numero 0*/

		{ .commandnumber = PROCESS_COMMAND_LEFT, 		.spin.velocity = ~(2000),	.spin.acceleration = 1000 , 	.fpcommandhandler = HandlerLeft },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.spin.wait = 10,												  	.fpcommandhandler = HandlerWait},
		{ .commandnumber = PROCESS_COMMAND_STOP,		.spin.velocity = 0,			.spin.acceleration = 0, 			.fpcommandhandler = HandlerStop},

};



processCommand_t cmdProcessDinamic[MAX_DINAMIC_COMMAND];


void ProcessInit(process_t* process){
	/*Crear un array de comandos vacios fijo*/
	//process->command = NULL;
	//Cargo directamente al iniciar el proceso Custom

	process->command= cmdProcessDinamic;
	process->state.commandIndex=MAX_DINAMIC_COMMAND;
	process->state.commandActualIndex=0;
	process->state.flags= STOP;
}



//PROCESS RUN FUNCTION

void ProcessRun(process_t *process) {

	uint8_t ci;

	process->state.commandActualIndex = 0;

	if (process->command != NULL) {
		ci = process->state.commandIndex; // command index
		if (process->config.status){
			while (process->state.commandActualIndex < ci) {
					process->command[process->state.commandActualIndex].fpcommandhandler(&(process->command[process->state.commandActualIndex].spin));
					process->state.commandActualIndex++;
				}
			}

	} else  printf("Without program to execute!!\r\n");
}


// Process all individual Command

void ProcessCommand(){
	processCommand_t readed_Command;

	if (modQueue_Read(&queueconsolareception, &readed_Command)){
		switch (readed_Command.commandnumber){

		case PROCESS_COMMAND_LEFTFAST:
			ProcessLeftFastCommand();
			break;

		case PROCESS_COMMAND_LEFTSLOW:
			ProcessLeftSlowCommand();
			break;

		case PROCESS_COMMAND_LEFT:
			ProcessLeftCommand();
			break;

		case PROCESS_COMMAND_RIGHTFAST:
			ProcessRightFastCommand();
			break;

		case PROCESS_COMMAND_RIGHTSLOW:
			ProcessRightSlowCommand();
			break;

		case PROCESS_COMMAND_RIGHT:
			ProcessRightCommand();
			break;

		case PROCESS_COMMAND_RUN:
			ProcessRunCommand();
			break;

		case PROCESS_COMMAND_STOP:
			ProcessStopCommand();
			break;

		case PROCESS_COMMAND_READDATA:
			ProcessREADDATACommand();
			break;

		default:
			break;

		}
	}

}

//SINGLE MOVEMENT COMMAND FUNCTIONS

/*++++++++++++++++++++++++++++++++++++FAST+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void ProcessRightFastCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=2800;
	parameters.acceleration=1000;


	HandlerRight_without_program(&parameters);
}

void ProcessRightCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=1800;
	parameters.acceleration=1000;


	HandlerRight_without_program(&parameters);
}

void ProcessRightSlowCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=800;
	parameters.acceleration=1000;


	HandlerRight_without_program(&parameters);
}

/*+++++++++++++++++++++++++++++++++++DOWN++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void ProcessLeftFastCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=2800;
	parameters.acceleration=1000;


	HandlerLeft_without_program(&parameters);
}

void ProcessLeftCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=1800;
	parameters.acceleration=1000;


	HandlerLeft_without_program(&parameters);
}


void ProcessLeftSlowCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=800;
	parameters.acceleration=1000;


	HandlerLeft_without_program(&parameters);
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void ProcessStopCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity= 0;
//	parameters.acceleration=0;

	HandlerStop_without_program(&parameters);
}

void ProcessREADDATACommand(){

	HandlerREADDATA();
}

void ProcessRunCommand(){

	HandlerRun();
}




//Process DINAMIC

///* cmdProcessDinamic is an empty process that it can fill to get a personalized process, up to MAX_DINAMIC_COMMAND commands can be added and set up */
//processCommand_t cmdProcessDinamic[MAX_DINAMIC_COMMAND] ={
//			/*Carga dinamica de COMANDOS*/
//};

//void ProcessLoadProgramDinamic(process_t *process) {
//	process->command= cmdProcessDinamic;
//	process->state.commandIndex=ProcessDinamicLen();
//	printf("Program Dinamic Load!!\r\n");
//}


//void ProcessAddSetProgramDinamic() {
//	processCommand_t readed_Command;
//	modQueue_Read(&queueconsolareception, &readed_Command);
//	printf("%d\n",ProcessDinamicLen());
//
//	if ( ProcessDinamicLen() < MAX_DINAMIC_COMMAND) {
//		printf("%d\n",ProcessDinamicLen());
//
//		cmdProcessDinamic[ProcessDinamicLen()].fpcommandhandler = Handlers_vector[readed_Command.commandnumber];
//		cmdProcessDinamic[ProcessDinamicLen()].argument = readed_Command.argument;
//		cmdProcessDinamic[ProcessDinamicLen()].commandnumber = readed_Command.commandnumber;
//
//
//	} else {
//		printf("Comandos maximos alcanzados!\r\n");
//	}
//}
//void ProcessCleanDinamic(){
//	int i=0;
//
//	for(i=0;i<MAX_DINAMIC_COMMAND;i++){
//		cmdProcessDinamic[i].commandnumber = PROCESS_COMMAND__EMPTY;
//	}
//}
//
//int ProcessDinamicLen(){
//	int i=0;
//
//	for(i=0;i<MAX_DINAMIC_COMMAND;i++){
//		if(cmdProcessDinamic[i].commandnumber == PROCESS_COMMAND__EMPTY){
//			return i;
//		}
//	}
//	return MAX_DINAMIC_COMMAND;
//}


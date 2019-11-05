#include <stdio.h>
#include "process.h"
#include "handlers.h"
#include "mod_queue.h"
#include "app_main_dipcoater.h"



#define MAX_ESTATIC_COMMAND 	10
#define MAX_DINAMIC_COMMAND 	32


//TODO ver con lucas para que era esto!
//processCommandHandler_t Handlers_vector[]={HandlerCeroMachine,HandlerSpin,HandlerUp,HandlerDown,HandlerWait,HandlerStop,HandlerFinish};


/**
 * @brief Function to initialize the Process
 * Function that receives a pointer to process, and initialize the structure
 * of the process.
 *
 * @param pointer to process
 * @return nothing
 */

void ProcessInit(process_t* process){
	/*Crear un array de comandos vacios fijo*/
	process->command = NULL;
	process->commandlen=0;
	process->state.commandIndex=MAX_ESTATIC_COMMAND;
	process->state.flags=0;
}

//PROCESSES AVAILABLE

/*cmdProcessStandard has the predefined values and only the N of the loop can be modified*/
processCommand_t cmdProcessStandard[MAX_ESTATIC_COMMAND] = {
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine },

		//DEBE BAJAR HASTA ANTES DE INGRESAR LA MUESTRA EN EL RECIPIENTE
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0 , .fpcommandhandler = HandlerWait},
		/*start of the DWUW cycle*/
		//DEBE BAJAR LA MUESTRA Y SUMERGIRLA EN EL  RECIPIENTE
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0 , .fpcommandhandler = HandlerWait },
		{ .commandnumber = PROCESS_COMMAND_UP, 			.argument.spin.velocity = 2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerUp },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0 , .fpcommandhandler = HandlerWait },
		/*LOOP repeats N times the DWUW cycle */
		{ .commandnumber =  PROCESS_COMMAND_LOOP, 		.argument.value.val = 0 },

		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine},
		{ .commandnumber = PROCESS_COMMAND_FINISH,		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0, .fpcommandhandler = HandlerFinish},

};

/*CmdProcessCustom, the arguments can be modified to make a load and run after*/
processCommand_t cmdProcessCustom[MAX_ESTATIC_COMMAND] = {
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 6,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0 , .fpcommandhandler = HandlerWait},
		/*start of the DWUW cycle*/
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0 , .fpcommandhandler = HandlerWait },
		{ .commandnumber = PROCESS_COMMAND_UP, 			.argument.spin.velocity = 2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerUp },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0 , .fpcommandhandler = HandlerWait },
		/*LOOP repeats N times the DWUW cycle */
		{ .commandnumber =  PROCESS_COMMAND_LOOP, 		.argument.value.val = 0 },


		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine},
		{ .commandnumber = PROCESS_COMMAND_FINISH,		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0, .fpcommandhandler = HandlerFinish},

};


//PROCESS RUN FUNCTION

void ProcessRun(process_t *process) {

	uint8_t ci;
	uint8_t index = 0;
	uint8_t loop_ori,loop;

	if (process->command != NULL) {
		ci = process->state.commandIndex; // command index
		loop_ori = process->command[PROCESS_COMMAND_LOOP-1].argument.value.val;
		loop=loop_ori;
		printf("\r\nProceso Iniciado - Motor Encendido\r\n");
		while (index < ci) {
			if (process->command[index].commandnumber != PROCESS_COMMAND_LOOP) {
				//VER LO QUE ESTAMOS PONIENDO EN EL PUNTERO A FUNCION (PASAR DIRECTAMENTE EL ARGUMENTO )
				process->command[index].fpcommandhandler(&(process->command[index].argument.spin));
				index++;
			}
			if (process->command[index].commandnumber == PROCESS_COMMAND_LOOP){
				if (loop > 0 ){
					loop--;
					printf("Loop number:%d\r\n",loop_ori - loop);

					//Desde comando LOOP vuelve 4 comandos hacia atras para hacer secuencia DWUW
					index -=4;
			}
				else index++;
			}
		}
	} else {
		printf("Without program to execute!!\r\n");
	}
}


//PROCESS LOAD FUNCTIONS

void ProcessLoadProgramStandard(process_t *process) {
	process->command= cmdProcessStandard;
	process->state.commandIndex=MAX_ESTATIC_COMMAND;
	printf("Program Standard Load!!\r\n");
}

void ProcessLoadProgramCustom(process_t *process) {
	process->command= cmdProcessCustom;
	process->state.commandIndex=MAX_ESTATIC_COMMAND;
	printf("Program Custom Load!!\r\n");
}


//PROCESS SET FUNCTIONS

void ProcessSetProgramStandard() {
	processCommand_t readed_Command;
	modQueue_Read(&queueconsolareception, &readed_Command);
	cmdProcessStandard[readed_Command.commandnumber].argument.value.val =
			readed_Command.argument.value.val;

}

void ProcessSetProgramCustom() {

	processCommand_t readed_Command;
	modQueue_Read(&queueconsolareception, &readed_Command);
	cmdProcessCustom[readed_Command.commandnumber].argument =
			readed_Command.argument;

}


//SINGLE MOVEMENT COMMAND FUNCTIONS

void ProcessUpFastCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=3;
	parameters.acceleration=3;
	parameters.test=3;

	HandlerUp(&parameters);
}

void ProcessUpCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=2;
	parameters.acceleration=2;
	parameters.test=2;

	HandlerUp(&parameters);
}

void ProcessUpSlowCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=1;
	parameters.acceleration=1;
	parameters.test=1;

//	HandlerUp(&parameters);
	HandlerUp_without_program(&parameters);

}

void ProcessDownFastCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=3;
	parameters.acceleration=3;
	parameters.test=3;

	HandlerDown(&parameters);
}
void ProcessDownCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=2;
	parameters.acceleration=2;
	parameters.test=2;

	HandlerDown(&parameters);
}
void ProcessDownSlowCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=1;
	parameters.acceleration=1;
	parameters.test=1;

	HandlerDown(&parameters);
}

void ProcessStopCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=0;
	parameters.acceleration=0;
	parameters.test=0;

	HandlerStop(&parameters);
}

void ProcessREADDATACommand(){


	HandlerREADDATA();
}



void ProcessRunCommand(){

	HandlerRun();
}


//Process DINAMICO

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



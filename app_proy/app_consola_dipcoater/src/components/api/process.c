#include <stdio.h>
#include "../../components/api/include/process.h"
#include "../../components/api/include/handlers.h"
#include "../../components/api/include/mod_queue.h"
#include "../../main/app_main_dipcoater.h"



#define MAX_ESTATIC_COMMAND 	10
#define MAX_DINAMIC_COMMAND 	32

processCommandHandler_t Handlers_vector[]={HandlerCeroMachine,HandlerSpin,HandlerUp,HandlerDown,HandlerWait,HandlerStop,HandlerFinish};

//PROCESS INIT FUNCTION

void ProcessInit(process_t* process){
	/*Crear un array de comandos vacios fijo*/
	process->command = NULL;
	process->commandlen=0;
	process->state.commandIndex=MAX_ESTATIC_COMMAND;
	process->state.flags=0;
}


//PROCESSES AVALIABLE

/*cmdProcessStandard has the predifined values and only the N of the loop can be modified*/
processCommand_t cmdProcessStandard[MAX_ESTATIC_COMMAND] = {
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait},
		/*start of the DWUW cycle*/
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait },
		{ .commandnumber = PROCESS_COMMAND_UP, 			.argument.spin.velocity = 2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerUp },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait },
		/*LOOP repeates N times the DWUW cycle */
		{ .commandnumber =  PROCESS_COMMAND_LOOP, 		.argument.value.val = 1 },


		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine},
		{ .commandnumber = PROCESS_COMMAND_FINISH,		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0, .fpcommandhandler = HandlerFinish},

};

/*CmdProcessCustom, the arguments can be modified to make a load and run after*/
processCommand_t cmdProcessCustom[MAX_ESTATIC_COMMAND] = {
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait},
		/*start of the DWUW cycle*/
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait },
		{ .commandnumber = PROCESS_COMMAND_UP, 			.argument.spin.velocity = 2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerUp },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait },
		/*LOOP repeates N times the DWUW cycle */
		{ .commandnumber =  PROCESS_COMMAND_LOOP, 		.argument.value.val = 1 },


		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine},
		{ .commandnumber = PROCESS_COMMAND_FINISH,		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0, .fpcommandhandler = HandlerFinish},

};


/* cmdProcessDinamic is an empty process that it can fill to get a personalized process, up to MAX_DINAMIC_COMMAND commands can be added and set up */
/*  SE VA CUSTOMIZAR EN PROXIMA VERSION*/
processCommand_t cmdProcessDinamic[MAX_DINAMIC_COMMAND] ={
			/*Carga dinamica de COMANDOS*/
};





//PROCESS RUN FUNCTION

void ProcessRun(process_t *process) {

	uint8_t ci;
	uint8_t index = 0;
	uint8_t loop;


	if (process->command != NULL) {
		ci = process->state.commandIndex; 				// command index
		loop = process->command[PROCESS_COMMAND_LOOP-1].argument.value.val;
		while (index < ci) {
			if (process->command[index].commandnumber != PROCESS_COMMAND_LOOP) {
				process->command[index].fpcommandhandler(&(process->command[index].argument.spin));
				index++;
			}
			if (process->command[index].commandnumber == PROCESS_COMMAND_LOOP){
				if (loop > 0 ){
					loop--;
					printf("Loop number:%d\r\n",loop);
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
	printf("Program Standar Load!!\r\n");
}

void ProcessLoadProgramCustom(process_t *process) {
	process->command= cmdProcessCustom;
	process->state.commandIndex=10;
	printf("Program Custom Load!!\r\n");
}
void ProcessLoadProgramDinamic(process_t *process) {
	process->command= cmdProcessDinamic;
	process->state.commandIndex=ProcessDinamicLen();
	printf("Program Dinamic Load!!\r\n");
}


//PROCESS SET FUNCTIONS

void ProcessSetProgramStandard(){
		processCommand_t readed_Command;
		modQueue_Read(&queueconsolareception, &readed_Command);
		cmdProcessStandard[readed_Command.commandnumber].argument.value=readed_Command.argument.value;
}


void ProcessSetProgramCustom(){


	processCommand_t readed_Command;
	modQueue_Read(&queueconsolareception, &readed_Command);

	cmdProcessCustom[readed_Command.commandnumber].argument=readed_Command.argument;

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

	HandlerUp(&parameters);
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


/*PROCESO DINAMICO DE CARGA DE COMANDOS  -> REVISAR Y DEJAR A PUNTO EN PROXIMA VERSION   */

void ProcessAddSetProgramDinamic() {
	processCommand_t readed_Command;
	modQueue_Read(&queueconsolareception, &readed_Command);
	printf("%d\n",ProcessDinamicLen());

	if ( ProcessDinamicLen() < MAX_DINAMIC_COMMAND) {
		printf("%d\n",ProcessDinamicLen());

		cmdProcessDinamic[ProcessDinamicLen()].fpcommandhandler = Handlers_vector[readed_Command.commandnumber];
		cmdProcessDinamic[ProcessDinamicLen()].argument = readed_Command.argument;
		cmdProcessDinamic[ProcessDinamicLen()].commandnumber = readed_Command.commandnumber;


	} else {
		printf("Comandos maximos alcanzados!\r\n");
	}
}

void ProcessCleanDinamic(){
	int i=0;

	for(i=0;i<MAX_DINAMIC_COMMAND;i++){
		cmdProcessDinamic[i].commandnumber = PROCESS_COMMAND__EMPTY;
	}
}

int ProcessDinamicLen(){
	int i=0;

	for(i=0;i<MAX_DINAMIC_COMMAND;i++){
		if(cmdProcessDinamic[i].commandnumber == PROCESS_COMMAND__EMPTY){
			return i;
		}
	}
	return MAX_DINAMIC_COMMAND;
}




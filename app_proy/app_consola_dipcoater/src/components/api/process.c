#include <stdio.h>
#include "../../components/api/include/process.h"
#include "../../components/api/include/handlers.h"
#include "../../components/api/include/mod_queue.h"
#include "../../main/app_main_dipcoater.h"



#define MAX_ESTATIC_COMMAND 	10
#define MAX_DINAMIC_COMMAND 	32

/*Definicion del Proceso Estático Comando Velocidad Aceleracion---   SE DEBE BUSCAR VELOCIDAD Y ACELERACION MEDIA DEL SISTEMA , */
processCommand_t cmdProcessStandard[MAX_ESTATIC_COMMAND] = {
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait},
		/*Baja hasta que la muestra quede sobre el recipiente*/
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait },
		{ .commandnumber = PROCESS_COMMAND_UP, 			.argument.spin.velocity = 2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerUp },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait },
		/*Test comando LOOP   */
		{ .commandnumber =  PROCESS_COMMAND_LOOP, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0 , .argument.value.val = 1 },


		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine},
		{ .commandnumber = PROCESS_COMMAND_FINISH,		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0, .fpcommandhandler = HandlerFinish},

};

/*Definicion del Proceso Estático Comando Velocidad Aceleracion */
processCommand_t cmdProcessCustom[MAX_ESTATIC_COMMAND] = {
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait},
		/*Baja hasta que la muestra quede sobre el recipiente*/
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait },
		{ .commandnumber = PROCESS_COMMAND_UP, 			.argument.spin.velocity = 2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerUp },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine},
		{ .commandnumber = PROCESS_COMMAND_FINISH,		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0, .fpcommandhandler = HandlerFinish},
/*TODO: Agregar comando LOOP */
};


/*Definicion del Proceso Estático Comando Velocidad Aceleracion */
processCommand_t cmdProcessDinamic[MAX_DINAMIC_COMMAND] = {

			/*Carga dinamica de COMANDOS*/
};


void ProcessInit(process_t* process){

	/*Crear un array de comandos vacios fijo*/

	process->command = NULL;
	process->commandlen=0;
	process->state.commandIndex=MAX_ESTATIC_COMMAND;
	process->state.flags=0;

}


/* HACER PRUEBAS CON ESTA FUNCION*/
void ProcessCommandAdd(process_t *process, processCommand_t *cmd) {
	if (process->state.commandIndex < MAX_DINAMIC_COMMAND) {
		cmdProcessDinamic[process->state.commandIndex] = *cmd;
		process->state.commandIndex++;
		process->command=cmdProcessDinamic;
	} else {
		printf("Comandos maximos alcanzados!\r\n");
	}
}

void ProcessCommandRemove(process_t *process, processCommand_t *cmd) {
}




//arrayHandlers[PROCESS_COMMAND__N];
void ProcessRun(process_t *process) {

	uint8_t ci = process->state.commandIndex; 				// command index
	uint8_t index = 0;

	if (process->command != NULL) {
		while (index < ci) {
			if (process->command[index].commandnumber != PROCESS_COMMAND_LOOP) {
				process->command[index].fpcommandhandler(&(process->command[index].argument.spin));
				index++;
				//printf("Comando:%d \r\n", index);
			}
			if (process->command[index].commandnumber == PROCESS_COMMAND_LOOP){
				if (0  < process->command->argument.value.val ){
					process->command->argument.value.val--;
					printf("Loop number:%d\r\n",process->command->argument.value.val);
					index -=4;
				}
				else index++;
			}
		}
	} else {
		printf("Without program to execute!!\r\n");
	}
}

void ProcessNextCommand(process_t*	process){
//	process->state.commandIndex++;
}



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

void ProcessSetProgramCustom(){

	processCommand_t readed_Command;
	modQueue_Read(&queueconsolareception, &readed_Command);

	cmdProcessCustom[readed_Command.commandnumber].argument.spin.velocity=readed_Command.argument.spin.velocity;
	cmdProcessCustom[readed_Command.commandnumber].argument.spin.acceleration=readed_Command.argument.spin.acceleration;
	cmdProcessCustom[readed_Command.commandnumber].argument.spin.test=readed_Command.argument.spin.test;


}

/*
 * Que sea una tarea que ejecute funciones del módulo process.c    ; podría estar en el app_main_dipcoater.c
 * podria ser  Taskprocess
 * processProcesarr
 *
 * switch ()
 *
 * 	case UPDATE;
 *
 * 	case RUN;
 *
 */

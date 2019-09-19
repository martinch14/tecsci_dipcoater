#include <stdio.h>
#include "../../components/api/include/process.h"
#include "../../components/api/include/handlers.h"
#include "../../components/api/include/mod_queue.h"
#include "../../main/app_main_dipcoater.h"



#define MAX_ESTATIC_COMMAND 	9
#define MAX_DINAMIC_COMMAND 	32

/*Definicion del Proceso Estático Comando Velocidad Aceleracion */
processCommand_t cmdProcesoEstandar[MAX_ESTATIC_COMMAND] = {
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait},
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait },
		{ .commandnumber = PROCESS_COMMAND_UP, 			.argument.spin.velocity = 2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerUp },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine},
/*TODO: Agregar comando LOOP */

};

/*Definicion del Proceso Estático Comando Velocidad Aceleracion */
processCommand_t cmdProcesoCustom[MAX_ESTATIC_COMMAND] = {
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait},
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait },
		{ .commandnumber = PROCESS_COMMAND_UP, 			.argument.spin.velocity = 2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerUp },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerWait },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 , .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine},
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
			process->command[index].fpcommandhandler(
					&(process->command[index].argument.spin));
			index++;
		}
	} else {
		printf("Comandos no cargados!!\r\n");
	}
}

void ProcessNextCommand(process_t*	process){
	process->state.commandIndex++;
}



void ProcessLoadProgramStandard(process_t *process) {
	process->command= cmdProcesoEstandar;
}

void ProcessLoadProgramCustom(process_t *process) {
	process->command= cmdProcesoCustom;
}

void ProcessSetProgramCustom(){

	processCommand_t readed_Command;
	modQueue_Read(&queueconsolareception, &readed_Command);

	cmdProcesoCustom[readed_Command.commandnumber].argument.spin.velocity=readed_Command.argument.spin.velocity;
	cmdProcesoCustom[readed_Command.commandnumber].argument.spin.acceleration=readed_Command.argument.spin.acceleration;
	cmdProcesoCustom[readed_Command.commandnumber].argument.spin.test=readed_Command.argument.spin.test;


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








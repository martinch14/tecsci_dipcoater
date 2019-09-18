#include <stdio.h>
#include "../../components/api/include/process.h"
#include "../../components/api/include/handlers.h"
#include "../../components/api/include/mod_queue.h"
#include "../../main/app_main_dipcoater.h"



#define MAX_ESTATIC_COMMAND 	9

/*Definicion del Proceso Estático Comando Velocidad Aceleracion */
processCommand_t cmdProcesoEstandar[MAX_ESTATIC_COMMAND] = {
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10 },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 },
		{ .commandnumber = PROCESS_COMMAND_UP, 			.argument.spin.velocity = 2,	.argument.spin.acceleration = 1 },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 },
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10 },
/*TODO: Agregar comando LOOP */
};
///*Definicion del Proceso Dinámico - recibe parametros de configuracion desde el módulo de mensajería */
//processCommand_t cmd_dinamic[] = {
//		{ .commandnumber = PROCESS_COMMAND_SPIN,.argument.spin.velocity = 0.1 },
//		{ },
//		{ },
//		{ },
//};



void handlerEmpty(void){
	printf("Reportar Error");
}

//inline void ProcessNextCommand(process_t process){
//    process->state.ci;
//}
void ProcessInit(process_t* process){

	process->command = NULL;
	process->commandlen=0;
	process->state.commandIndex=0;
	process->state.flags=0;

//	while(1){
//		arrayHandlers[a todos] = handlerEmpty
//
//	}
}

void ProcessNextCommand(process_t*	process){

}

void ProcessCommandAdd(process_t *process, processCommand_t *cmd) {
}

int ExitCommand(void) {
	return 0;
}
int InitCommand(void) {
	return 0;
}


//arrayHandlers[PROCESS_COMMAND__N];


processReturn_e  ProcessRun(process_t *process) {
//	uint8_t ci = process->state.commandIndex; 				// command index
//	processCommand_t *cmd = &process->command[ci];
	processCommand_t *cmd = process->command ;
	if (InitCommand()) {
	}

	switch (cmd->commandnumber) {

	case PROCESS_COMMAND_CERO_MACHINE:
		HandlerCeroMachine(&(cmd->argument.spin));
//		ProcessNextCommand(process);
		break;
	case PROCESS_COMMAND_SPIN:
		HandlerSpin(&(cmd->argument.spin));
//		ProcessNextCommand(process);
		break;
	case PROCESS_COMMAND_UP:
		HandlerUp(&(cmd->argument.spin));
//		ProcessNextCommand(process);
		break;
	case PROCESS_COMMAND_DOWN:
		HandlerDown(&(cmd->argument.spin));
//		ProcessNextCommand(process);
		break;
	case PROCESS_COMMAND_WAIT:
		HandlerWait(&(cmd->argument.spin));
//		ProcessNextCommand(process);
		break;
	case PROCESS_COMMAND_STOP:
		HandlerStop(&(cmd->argument.spin));
//		ProcessNextCommand(process);
		break;
	default:
		process->state.flags |= _FLAG_ERROR;
		break;
	}
//#else
//	arrayHandlers[cmd->commandnumber]();
//#endif
	if (ExitCommand()) {
	}
	return PROCESS_RET_OK;
}

void ProcessCommandUpdate(process_t *processDipCoating){
	uint8_t index=0;
	int codeCommand;
	int vel;
	int acc;

	//TODO:
	//Cargar con los valores que vienen de la cola de mensajeria   queueconsolareception    (luego se podría reportar estado en cola queueconsolatransmit)
	//ejecutar
	//termiar proceso
	//cargar de nuevo o repetir

	if (processDipCoating->command != NULL) {
		while (processDipCoating->state.commandIndex < 9) {
			modQueue_Read(&queueconsolareception, &codeCommand);
			processDipCoating->command->commandnumber = codeCommand;
			/*TENER EN CUENTA QUE EL READ NO SACA DATOS DE LA COLA, TAMPOCO CAMBIA EL PUNTERO A LA POSICION*/
			modQueue_Read(&queueconsolareception, &vel);
			processDipCoating->command->argument.spin.velocity = vel;
			/*TENER EN CUENTA QUE EL READ NO SACA DATOS DE LA COLA, TAMPOCO CAMBIA EL PUNTERO A LA POSICION*/
			modQueue_Read(&queueconsolareception, &acc);
			processDipCoating->command->argument.spin.acceleration = acc;

			ProcessRun(processDipCoating);
			processDipCoating->state.commandIndex++;
			index++;
		}
		processDipCoating->state.commandIndex = 0;
		index = 0;
	}
	else {   /* Avisar que no hay ningun proceso seteado!!!!*/
		printf("No hay procedimiento cargado!\r\n");
	}


}

/*Se carga el programa estandar*/

void ProcessLoadProgramStandar(process_t *process) {
	process->command= cmdProcesoEstandar;
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
 * */





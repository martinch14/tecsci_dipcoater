#include <stdio.h>
#include "../../components/api/include/process.h"
#include "../../components/api/include/handlers.h"




void handlerEmpty(void){
	printf("Reportar Error");
}

//inline void ProcessNextCommand(process_t process){
//    process->state.ci;
//}

void ProcessInit(process_t* process){
//	process->command->argument.spin.acceleration=0;
//	process->command->argument.spin.velocity=0;
	process->command->commandnumber=0;
	process->commandlen=0;
	process->state.commandIndex=0;
	process->state.flags=0;

//	while(1){
//		arrayHandlers[a todos] = handlerEmpty
//
//	}
}

void ProcessNextCommand(process_t*	process){
//	if(process->state.commandIndex < 8){    /*Primera prueba con el proceso estatico*/
//	process->state.commandIndex++;
//	}
//	else process->state.commandIndex = 0 ;
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
//#if 0
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

void ProcessCommandUpdate(process_t *processDipCoating,processCommand_t* cmdProcesoEstandar){
	uint8_t index=0;

	//TODO:
	//Cargar con los valores que vienen de la cola de mensajeria   queueconsolareception    (luego se podría reportar estado en cola queueconsolatransmit)
	//ejecutar
	//termiar proceso
	//cargar de nuevo o repetir


	while (processDipCoating->state.commandIndex < 9){
		processDipCoating->command->commandnumber = cmdProcesoEstandar[index].commandnumber;
		processDipCoating->command->argument.spin.velocity = cmdProcesoEstandar[index].argument.spin.velocity;
		processDipCoating->command->argument.spin.acceleration = cmdProcesoEstandar[index].argument.spin.acceleration;
		ProcessRun(processDipCoating);
		processDipCoating->state.commandIndex++;
		index++;
	}
	processDipCoating->state.commandIndex=0;
	index=0;
}



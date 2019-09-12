#include "../../components/api/include/process.h"

#include <stdio.h>

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


processReturn_t  ProcessRun(process_t *process) {
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

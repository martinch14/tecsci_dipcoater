#include <stdio.h>
#include "process.h"
#include "handlers.h"
#include "mod_queue.h"
#include "app_main_dipcoater.h"



#define MAX_ESTATIC_COMMAND 	8
//#define MAX_DINAMIC_COMMAND 	32

/**
 * @brief Function to initialize the Process
 * Function that receives a pointer to process, and initialize the structure
 * of the process.
 *
 * @param pointer to process
 * @return nothing
 */


//PROCESSES AVAILABLE

/*cmdProcessStandard has the predefined values and only the N of the loop can be modified*/
processCommand_t cmdProcessStandard[MAX_ESTATIC_COMMAND] = {
//		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 222000,	.argument.spin.acceleration = 1000, .fpcommandhandler = HandlerCeroMachine },


		//Definir Z0 como la altura entre la interseccion del fluido con la muestra
		//Bajar hasta Z0 + 10 mm
		//DEBE BAJAR HASTA ANTES DE INGRESAR LA MUESTRA EN EL RECIPIENTE
		/*{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = 222000,	.argument.spin.acceleration = 1000, .fpcommandhandler = array[PROCESS_COMMAND_DOWN] },*/


		/*Al setear el custom program arrancamos desde el comando numero 0*/

//		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = 222000,	.argument.spin.acceleration = 1000, .fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0 , .fpcommandhandler = HandlerWait},

		/*start of the DWUW cycle*/
		//DEBE BAJAR LA MUESTRA Y SUMERGIRLA EN EL  RECIPIENTE
		//Tener valor de cuanto debe ingresar la muestra en el fluido -> ir hasta posicion Z0 - profundidad de inmersion
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = 22258,	.argument.spin.acceleration = 32000 , .fpcommandhandler = HandlerDownLoop },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0 , .fpcommandhandler = HandlerWait },
		{ .commandnumber = PROCESS_COMMAND_UP, 			.argument.spin.velocity = 22258,	.argument.spin.acceleration = 32000 , .fpcommandhandler = HandlerUpLoop },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0 , .fpcommandhandler = HandlerWait },
		/*LOOP repeats N times the DWUW cycle */
		{ .commandnumber =  PROCESS_COMMAND_LOOP, 		.argument.value.val = 0 },

//		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine},
		{ .commandnumber = PROCESS_COMMAND_FINISH,		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0, .fpcommandhandler = HandlerFinish},

};

/*CmdProcessCustom, the arguments can be modified to make a load and run after*/
processCommand_t cmdProcessCustom[MAX_ESTATIC_COMMAND] = {

		/*Al setear el custom program arrancamos desde el comando numero 0*/

//		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 200000,	.argument.spin.acceleration = 20000, .fpcommandhandler = HandlerCeroMachine },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = 400000,	.argument.spin.acceleration = 40000 , 	.fpcommandhandler = HandlerDown },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.wait.time = 0,												  	.fpcommandhandler = HandlerWait},
		/*start of the DWUW cycle*/
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = 400000,	.argument.spin.acceleration = 40000 , 	.fpcommandhandler = HandlerDownLoop },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.wait.time = 0,													.fpcommandhandler = HandlerWaitDown },
		{ .commandnumber = PROCESS_COMMAND_UP, 			.argument.spin.velocity = 400000,	.argument.spin.acceleration = 40000 , 	.fpcommandhandler = HandlerUpLoop },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.wait.time = 0,													.fpcommandhandler = HandlerWaitUp },
		/*LOOP repeats N times the DWUW cycle */
		{ .commandnumber =  PROCESS_COMMAND_LOOP, 		.argument.value.val = 0 },


//		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10, .fpcommandhandler = HandlerCeroMachine},
		{ .commandnumber = PROCESS_COMMAND_FINISH,		.argument.spin.velocity = 0,	.argument.spin.acceleration = 0, .fpcommandhandler = HandlerFinish},

};



void ProcessInit(process_t* process){
	/*Crear un array de comandos vacios fijo*/
	//process->command = NULL;
	//Cargo directamente al iniciar el proceso Custom
	process->command= cmdProcessCustom;

	process->commandlen=0;
	process->state.commandIndex=MAX_ESTATIC_COMMAND;
	process->state.commandActualIndex=0;
	process->state.flags=0;
}



//PROCESS RUN FUNCTION

void ProcessRun(process_t *process) {

	uint8_t ci;
	//uint8_t index = 0;
	uint8_t loop_ori,loop;

	process->state.commandActualIndex = 0;

	if (process->command != NULL) {
		ci = process->state.commandIndex; // command index
		loop_ori = process->command[PROCESS_COMMAND_LOOP-2].argument.value.val;
		loop=loop_ori;
		printf("\r\nProceso Iniciado - Motor Encendido\r\n");
		while (process->state.commandActualIndex < ci) {
			if (process->command[process->state.commandActualIndex].commandnumber != PROCESS_COMMAND_LOOP) {
				//VER LO QUE ESTAMOS PONIENDO EN EL PUNTERO A FUNCION (PASAR DIRECTAMENTE EL ARGUMENTO )
				//process->command[index].fpcommandhandler(&(process->command[index].argument.spin));
				process->command[process->state.commandActualIndex].fpcommandhandler(&(process->command[process->state.commandActualIndex].argument));
				//index++;
				process->state.commandActualIndex++;
			}
			if (process->command[process->state.commandActualIndex].commandnumber == PROCESS_COMMAND_LOOP){
				if (loop > 0 ){
					loop--;
					printf("Loop number:%d\r\n",loop_ori - loop);

					//Desde comando LOOP vuelve 4 comandos hacia atras para hacer secuencia DWUW
					//index -=4;
					process->state.commandActualIndex-=4;


			}
					//else index++;
				else process->state.commandActualIndex++;
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
	cmdProcessStandard[readed_Command.commandnumber].argument.value.val = readed_Command.argument.value.val;

}

void ProcessSetProgramCustom() {

	printf("\r\n");
	processCommand_t readed_Command;
	modQueue_Read(&queueconsolareception, &readed_Command);
	cmdProcessCustom[readed_Command.commandnumber].argument = readed_Command.argument;

}



// Process all individual Command

void ProcessCommand(){
	processCommand_t readed_Command;

	if (modQueue_Read(&queueconsolareception, &readed_Command)){
		switch (readed_Command.commandnumber){

		case PROCESS_COMMAND_CERO_MACHINE:
			ProcessCeroMachineCommand();
			break;

		case PROCESS_COMMAND_DOWNFAST:
			ProcessDownFastCommand();
			break;

		case PROCESS_COMMAND_DOWNSLOW:
			ProcessDownSlowCommand();
			break;

		case PROCESS_COMMAND_DOWN:
			ProcessDownCommand();
			break;

		case PROCESS_COMMAND_UPFAST:
			ProcessUpFastCommand();
			break;

		case PROCESS_COMMAND_UPSLOW:
			ProcessUpSlowCommand();
			break;

		case PROCESS_COMMAND_UP:
			ProcessUpCommand();
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

		case PROCESS_COMMAND_ENA_DRIVER:
			ProcessENA_DRIVERCommand();
			break;

		case PROCESS_COMMAND_DIS_DRIVER:
			ProcessDIS_DRIVERCommand();
			break;

		default:
			break;

		}
	}

}


//SINGLE MOVEMENT COMMAND FUNCTIONS

void ProcessCeroMachineCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=222000;
	parameters.acceleration=32000;
//	parameters.displacement_z=3;

	HandlerCeroMachine(&parameters);
}



//SINGLE MOVEMENT COMMAND FUNCTIONS

/*++++++++++++++++++++++++++++++++++++FAST+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void ProcessUpFastCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=450000;
	parameters.acceleration=32000;
//	parameters.displacement_z=1273722;  // Desplazamiento de 100 mm
	parameters.displacement_z=636861;  // Desplazamiento de 50 mm

	HandlerUp_without_program(&parameters);
}

void ProcessUpCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=50000;
	parameters.acceleration=32000;
	parameters.displacement_z= 127372;  // Desplazamiento de 10 mm

	HandlerUp_without_program(&parameters);
}

void ProcessUpSlowCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=50000;
	parameters.acceleration=32000;
	parameters.displacement_z=12737;    //Desplazamiento de 1 mm

	HandlerUp_without_program(&parameters);
}

/*+++++++++++++++++++++++++++++++++++DOWN++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void ProcessDownFastCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=450000;
	parameters.acceleration=32000;
//	parameters.displacement_z=1273722;  // Desplazamiento de 100 mm
	parameters.displacement_z=636861;  // Desplazamiento de 50 mm

	HandlerDown_without_program(&parameters);
}

void ProcessDownCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=50000;
	parameters.acceleration=32000;
	parameters.displacement_z= 127372;  // Desplazamiento de 10 mm

	HandlerDown_without_program(&parameters);
}


void ProcessDownSlowCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=50000;
	parameters.acceleration=32000;
	parameters.displacement_z=12737;    //Desplazamiento de 1 mm

	HandlerDown_without_program(&parameters);
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void ProcessStopCommand(){

	processCommandArgSpin_t parameters;

	parameters.velocity=0;
	parameters.acceleration=0;
	parameters.displacement_z=0;

	printf("process Command a cola\r\n ");
	HandlerStop(&parameters);
}

void ProcessREADDATACommand(){

	HandlerREADDATA();
}



void ProcessENA_DRIVERCommand(){

	HandlerENA_DRIVER();

}

void ProcessDIS_DRIVERCommand(){

	HandlerDIS_DRIVER();

}

/* Parametros de configuracion de proceso */

void ProcessCERO_SAMPLECommand(){

	processDipCoating.config.displacement_to_sample = HandlerCERO_SAMPLE();

}

void ProcessDELTADIPCommand(){

	/*Profundidad de dip coating */
	HandlerDELTADIP();

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



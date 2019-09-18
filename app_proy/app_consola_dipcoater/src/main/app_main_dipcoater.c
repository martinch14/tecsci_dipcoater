/*
 ============================================================================
 Name        : spincoater_consola.c
 Author      : Martin Gambarotta
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

#include "app_main_dipcoater.h"


//flag para ejecucion del procesamiento de datos
uint8_t entry =0;

// Lo defino global para que lo pueda usar desde los handler de tinysh
process_t processDipCoating;




/*handler LOADPROGRAM to process  desde el comando recibido en  tinysh*/
static void CommandLOADPROGRAMSTANDARDHandler(int argc, char **argv){

	ProcessLoadProgramStandard(&processDipCoating);

}


static void CommandLOADPROGRAMCUSTOMHandler(int argc, char **argv){

	ProcessLoadProgramCustom(&processDipCoating);

}

/*handler SETCUSTOMPROGRAM para el tiny*/
static void CommandSETCUSTOMPROGRAMHandler(int argc, char **argv){
	 processCommand_t aux_process_comand;

 if(5 == argc){	/*SET XX XX XX representa 4 comandos 	argc = 4*/
	 aux_process_comand.commandnumber=tinysh_get_arg_int(argc, argv, 1);
	 aux_process_comand.argument.spin.velocity=tinysh_get_arg_int(argc, argv, 2);
	 aux_process_comand.argument.spin.acceleration=tinysh_get_arg_int(argc, argv, 3);
	 aux_process_comand.argument.spin.test=tinysh_get_arg_int(argc, argv, 4);
	 modQueue_Write(&queueconsolareception,&aux_process_comand);
	 ProcessSetProgramCustom();

 }

}

static void CommandRUNHandler(int argc, char **argv){

	entry=1;

}



static tinysh_cmd_t commandLOADPROGRAMSTANDARD = 	{NULL,"LOADPROGRAMSTANDARD", NULL, NULL, CommandLOADPROGRAMSTANDARDHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLOADPROGRAMCUSTOM = 	{NULL,"LOADPROGRAMCUSTOM", NULL, NULL, CommandLOADPROGRAMCUSTOMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSETCUSTOMPROGRAM = 	{NULL,"SETCUSTOMPROGRAM", NULL, NULL, CommandSETCUSTOMPROGRAMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandRUN = 	{NULL,"RUN", NULL, NULL, CommandRUNHandler, NULL, NULL, NULL};


void HandlerConsolePutchar(unsigned char c){
	putchar(c);
}


int app_main_dipcoater(void) {
	char c=0;

//	Inicialicion de los comandos

	tinysh_add_command(&commandLOADPROGRAMSTANDARD);
	tinysh_add_command(&commandSETCUSTOMPROGRAM);
	tinysh_add_command(&commandLOADPROGRAMCUSTOM);
	tinysh_add_command(&commandRUN);


	tinysh_set_putchar(HandlerConsolePutchar);

	tinysh_init();

//  Inicializacion de la colas de mensajerias

	/* Si casteo pincha todo!*/
	modQueue_Init(&queueconsolareception, bufferreception, 10, sizeof(processCommand_t));
	modQueue_Init(&queueconsolatransmit, buffertransmit, 10, sizeof(int));

//	Inicializacion del modulo de procesamiento de comandos
	ProcessInit(&processDipCoating);

//	Inicializacion del modulo motor
	while (1){

		/*Task Tiny   ->  setea y carga los valores que llegan por comando a la estructura de procesamiento*/
		task_tinysh(c);

		/*Task Procesamiento -> recorre la estructura de procesamiento con los datos cargados */
		task_process(&processDipCoating);

		/*Task Motor  ->TODO según el estado en donde se encuentre ejecuta la ordenes de accionamiento del motor*/
		task_motor();
	}

	return EXIT_SUCCESS;
}


void task_tinysh(char c){
	c = getchar();
	tinysh_char_in(c);
}

void task_process(process_t *processDipCoating){

//	switch (){
//	case UPDATE:    //update with new process or arguments
//			ProcessCommandUpdate();
//			break;
//	case RUN;
//			ProcessRun();
//			break;
//	}
	if (entry){
	//ProcessCommandSet(processDipCoating);
	ProcessRun(processDipCoating);
	entry=0;
	}


}

void task_motor(void){

}



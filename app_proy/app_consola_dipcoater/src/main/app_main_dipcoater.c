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


/*Definiciones de comandos y handler para el tiny*/
static void CommandReadHandler(int argc, char **argv){
	printf("Siempre entra al handler del comando! \r\n");
	if(4 == argc){	/*UP XX XX XX representa 4 comandos 	argc = 4*/
		int 	codeCommand = 	tinysh_get_arg_int(argc, argv, 0);
		int		velocity =	 	tinysh_get_arg_int(argc, argv, 1);
		int 	acceleration = 	tinysh_get_arg_int(argc, argv, 2);
		int 	test = 			tinysh_get_arg_int(argc, argv, 3);
//		struct{
//		comando.velocidad =
//		}
		modQueue_Write(&queueconsolareception,&codeCommand);
		modQueue_Write(&queueconsolareception,&velocity);
		modQueue_Write(&queueconsolareception,&acceleration);
		printf("%d_codeCommand",codeCommand);
		printf("%d",velocity);
		printf("%d",acceleration);
		printf("%d\r\n",test);
		entry=1;
	}
}

/*handler LOADPROGRAM to process  desde el comando recibido en  tinysh*/
static void CommandLOADPROGRAMHandler(int argc, char **argv){

	ProcessLoadProgramStandar(&processDipCoating);

}

/*handler SETCUSTOMPROGRAM para el tiny*/
static void CommandSETCUSTOMPROGRAMHandler(int argc, char **argv){
}


static tinysh_cmd_t commandSTART = 			{NULL,"START", NULL, NULL, CommandReadHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSTOP = 			{NULL,"STOP", NULL, NULL, CommandReadHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandUP = 			{NULL,"UP", NULL, NULL, CommandReadHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDOWN = 			{NULL,"DOWN", NULL, NULL, CommandReadHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandWAIT = 			{NULL,"WAIT", NULL, NULL, CommandReadHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSPIN = 			{NULL,"SPIN", NULL, NULL, CommandReadHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandCEROMACHINE = 	{NULL,"CEROMACHINE", NULL, NULL, CommandReadHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLOADPROGRAM = 	{NULL,"LOADPROGRAM", NULL, NULL, CommandLOADPROGRAMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSETCUSTOMPROGRAM = 	{NULL,"SETCUSTOMPROGRAM", NULL, NULL, CommandSETCUSTOMPROGRAMHandler, NULL, NULL, NULL};



void HandlerConsolePutchar(unsigned char c){
	putchar(c);
}


int app_main_dipcoater(void) {
	char c=0;

//	Inicialicion de los comandos
	tinysh_add_command(&commandDOWN);
	tinysh_add_command(&commandUP);
	tinysh_add_command(&commandSPIN);
	tinysh_add_command(&commandWAIT);
	tinysh_add_command(&commandSTOP);
	tinysh_add_command(&commandCEROMACHINE);
	tinysh_add_command(&commandSTART);
	tinysh_add_command(&commandLOADPROGRAM);
	tinysh_add_command(&commandSETCUSTOMPROGRAM);

	tinysh_set_putchar(HandlerConsolePutchar);

	tinysh_init();

//  Inicializacion de la colas de mensajerias

	/* Si casteo pincha todo!*/
	modQueue_Init(&queueconsolareception, bufferreception, 10, sizeof(int));
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
	ProcessCommandUpdate(processDipCoating);
	entry=0;
	}


}

void task_tinysh(char c){
	c = getchar();
	tinysh_char_in(c);
}

void task_motor(void){

}



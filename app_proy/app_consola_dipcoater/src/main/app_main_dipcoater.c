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
#define MAX_ESTATIC_COMMAND 9


//flag para ejecucion del procesamiento de datos
uint8_t entry =0;


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


		/*Agregar comando LOOP */

};

/*Definicion del Proceso Dinámico - recibe parametros de configuracion desde el módulo de mensajería */
processCommand_t cmd_dinamic[] = {
		{ .commandnumber = PROCESS_COMMAND_SPIN,.argument.spin.velocity = 0.1 },
		{ },
		{ },
		{ },
};






/*Definiciones de comandos y handler para el tiny*/


static void CommandReadHandler(int argc, char **argv){
	if(4 == argc){	/*UP XX XX XX representa 4 comandos 	argc = 4*/
		char* 	name = 			tinysh_get_arg_string(argc, argv, 0);
		float	velocity =	 	tinysh_get_arg_float(argc, argv, 1);
		float 	acceleration = 	tinysh_get_arg_float(argc, argv, 2);

		struct{}
		comando.velocidad =

		modQueue_Write(&queueconsolareception,&velocity);
		modQueue_Write(&queueconsolareception,&acceleration);
		printf("%s_CommandReadHandler",name);
		printf("%f",velocity);
		printf("%f\r\n",acceleration);
		entry=1;

	}
}


static tinysh_cmd_t commandSTART = 			{NULL,"START", NULL, NULL, CommandReadHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSTOP = 			{NULL,"STOP", NULL, NULL, CommandReadHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandUP = 			{NULL,"UP", NULL, NULL, CommandReadHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDOWN = 			{NULL,"DOWN", NULL, NULL, CommandReadHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandWAIT = 			{NULL,"WAIT", NULL, NULL, CommandReadHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSPIN = 			{NULL,"SPIN", NULL, NULL, CommandReadHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandCEROMACHINE = 	{NULL,"CEROMACHINE", NULL, NULL, CommandReadHandler, NULL, NULL, NULL};


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

	tinysh_set_putchar(HandlerConsolePutchar);

	tinysh_init();

//  Inicializacion de la colas de mensajerias

	modQueue_Init(&queueconsolareception, (*uint8_t)bufferreception, 10, sizeof(float));
	modQueue_Init(&queueconsolatransmit, (*uint8_t)buffertransmit, 10, sizeof(float));

//	Inicializacion del modulo de procesamiento de comandos
	process_t processDipCoating;
	processDipCoating.command= cmdProcesoEstandar; // le cargo el proceso definido en el archivo estatico
	ProcessInit(&processDipCoating);



	while (1){
		tinysh_update(c);
		if (entry){
		ProcessCommandUpdate(&processDipCoating, cmdProcesoEstandar);
		entry=0;
		}

		/*ProcesoActivarMotor*/

	}

	return EXIT_SUCCESS;
}








void tinysh_update(char c){
	c = getchar();
	tinysh_char_in(c);
}

//	uint8_t index=0;
//	process_t processDipCoating;
////	processCommand_t cmd;
////	cmd.commandnumber = 0;
////	cmd.argument.spin.velocity = 0.1;
////	cmd.argument.spin.time = 1.0;
////	cmd.argument.wait.time = 10.0;
////	printf("%f", cmd.argument.spin.velocity);
////	ProcessCommandAdd(&processDipCoating, &cmd);
//	uint8_t number_procedure;
//	while (1){
//	printf("1 (uno) para operar el procedimiento estándar <-\r\n2 (dos) para operar procedimiento personalizado <-\r\n3 (tres) para apagar equipo <-\r\n");
////	scanf("%c",&number_procedure);
////	number_procedure= getchar();
//	while ((number_procedure = getchar()) != '\n' && number_procedure != EOF){
//	if (number_procedure == '1'){
//		processDipCoating.command= cmdProcesoEstandar;
//		ProcessInit(&processDipCoating);
//		while (processDipCoating.state.commandIndex < MAX_ESTATIC_COMMAND){
//			processDipCoating.command->commandnumber = cmdProcesoEstandar[index].commandnumber;
//			processDipCoating.command->argument.spin.velocity = cmdProcesoEstandar[index].argument.spin.velocity;
//			processDipCoating.command->argument.spin.acceleration = cmdProcesoEstandar[index].argument.spin.acceleration;
//			ProcessRun(&processDipCoating);
//			processDipCoating.state.commandIndex++;
//			index++;
//		}
//		processDipCoating.state.commandIndex=0;
//		index=0;
//	}
//	else if (number_procedure == '2'){
//
//		processDipCoating.command= cmd_dinamic;
//		ProcessInit(&processDipCoating);
//		}
//	else if (number_procedure == '3'){
//		return EXIT_SUCCESS;
//	}
//	printf("END PROCESS\r\n");
//	}
//	}


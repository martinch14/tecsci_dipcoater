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

#include "../components/api/include/handlers.h"
#include "../components/api/include/process.h"



/*ver porque tira un warning al incluirla*/
//#include "../components/tinysh/include/tinysh.h"

#define MAX_ESTATIC_COMMAND 9


/*COMANDO START!!*/


/*Proceso Estático, velocidades y tiempos preestablecidos!*/
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

};

/*Proceso Dinámico - recibe parametros de configuracion externa */
processCommand_t cmd[] = {
		{ .commandnumber = PROCESS_COMMAND_SPIN,.argument.spin.velocity = 0.1 },
		{ },
		{ },
		{ },
};


int app_main_dipcoater(void) {

	uint8_t index=0;
	process_t processDipCoating;

//	processCommand_t cmd;
//	cmd.commandnumber = 0;
//	cmd.argument.spin.velocity = 0.1;
//	cmd.argument.spin.time = 1.0;
//	cmd.argument.wait.time = 10.0;
//	printf("%f", cmd.argument.spin.velocity);
//	ProcessCommandAdd(&processDipCoating, &cmd);
	uint8_t number_procedure;



	while (1){

	printf("1 (uno) para operar el procedimiento estándar <-\r\n2 (dos) para operar procedimiento personalizado <-\r\n3 (tres) para apagar equipo <-\r\n");
//	scanf("%c",&number_procedure);
//	number_procedure= getchar();
	while ((number_procedure = getchar()) != '\n' && number_procedure != EOF){

	if (number_procedure == '1'){

		processDipCoating.command= cmdProcesoEstandar;
		ProcessInit(&processDipCoating);

		while (processDipCoating.state.commandIndex < MAX_ESTATIC_COMMAND){
			processDipCoating.command->commandnumber = cmdProcesoEstandar[index].commandnumber;
			processDipCoating.command->argument.spin.velocity = cmdProcesoEstandar[index].argument.spin.velocity;
			processDipCoating.command->argument.spin.acceleration = cmdProcesoEstandar[index].argument.spin.acceleration;
			ProcessRun(&processDipCoating);
			processDipCoating.state.commandIndex++;
			index++;
		}
		processDipCoating.state.commandIndex=0;
		index=0;
	}
	else if (number_procedure == '2'){

		processDipCoating.command= cmd;
		ProcessInit(&processDipCoating);

		tinysh_init();


		}
	else if (number_procedure == '3'){
		return EXIT_SUCCESS;
	}

	printf("END PROCESS\r\n");


	}
	}
	//	processDipCoating.state.commandIndex=0;
//	while (PROCESS_RET_OK == ProcessRun(&processDipCoating));
//	while (1){
//	float valor;
//	printf("Ingrese un valor:\r\n");
//	scanf("%f",&valor);
//	printf("Usted ingreso -> %f\r\n",valor);
//	sleep(1);
//	printf("Se espero 1 seg\r\n");
//	}

	return EXIT_SUCCESS;
}


/*
 ============================================================================
 Name        : dip coater_consola.c
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


/*TODO:
 * Al ingresar el comando setstandarprogram verificar que ingrese  un argumento con el loop
 *
 *
 * */

//Flag to Run the pŕogram loaded
flagRun_t entry = STOP;


int app_main_dipcoater(void) {
	char c=0;

	//Module tiny inicialization
	tinysh_init();
	//queue initialization
	modQueue_Init(&queueconsolareception, bufferreception, 10, sizeof(processCommand_t));
	modQueue_Init(&queueconsolatransmit, buffertransmit, 10, sizeof(int));

	//Process
	ProcessInit(&processDipCoating);

	//Motor module initialization
	//MotorInit();

	//Enviromental chamber initialization
	EnviromentalChamberInit(&enviromentalChamberDipCoating);



	while (1){ //infinite loop

		/*Task Tiny*/
		task_tinysh(c);

		/*Task Process */
		task_process(&processDipCoating);

		/*Task Motor  ->TODO según el estado en donde se encuentre ejecuta la ordenes de accionamiento del motor*/
		task_motor();
		/*Task Enviromental Chamber*/
		task_enviromental_chamber();
	}

	return EXIT_SUCCESS;
}

/*Futuras tareas de FreeRTOS*/

void task_tinysh(char c){
	c = getchar();
	tinysh_char_in(c);
}

void task_process(process_t *processDipCoating){
	if (entry==RUN){
		ProcessRun(processDipCoating);
		entry=STOP;
	}
}

void task_motor(void){

}
void task_enviromental_chamber(void){
	EnviromentalChamberRun(&enviromentalChamberDipCoating);
}



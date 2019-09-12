/*
 * app_main.c
 *
 *  Created on: 9 sep. 2019
 *      Author: martin
 */

#include <stdio.h>
#include <stdlib.h>
#include "tinysh.h"

#if __ARCH_x86__
__CLEANSTIN getchar():
#else
//__CLEANSTIN(__VARGS__) prtinf__VARGS__
#endif

static void commandHolahanlder(int argc, char **argv){
	if(4 <= argc){
		char* name = tinysh_get_arg_string(argc, argv, 0);
		int		entero 	= tinysh_get_arg_int(argc, argv, 1);
		float	decimal = tinysh_get_arg_float(argc, argv, 2);
		char*	texto 	= tinysh_get_arg_string(argc, argv, 3);
		char*	texto2 	= tinysh_get_arg_string(argc, argv, 4);
		char*	texto3 	= tinysh_get_arg_string(argc, argv, 5);
		char*	texto4 	= tinysh_get_arg_string(argc, argv, 6);


//		api_print("asd %d");
//		printf("asd\0");
//		__CLEANSTIN();
	}
}

static tinysh_cmd_t commandDOWN = 	{NULL,"DOWN"				, NULL, NULL, commandHolahanlder, NULL, NULL, NULL};
static tinysh_cmd_t commandUP = 	{NULL,"UP"					, NULL, NULL, commandHolahanlder, NULL, NULL, NULL};
static tinysh_cmd_t commandSPIN = 	{NULL,"SPIN"				, NULL, NULL, commandHolahanlder, NULL, NULL, NULL};
static tinysh_cmd_t commandWAIT = 	{NULL,"WAIT"				, NULL, NULL, commandHolahanlder, NULL, NULL, NULL};
static tinysh_cmd_t commandSTOP = 	{NULL,"STOP"				, NULL, NULL, commandHolahanlder, NULL, NULL, NULL};
static tinysh_cmd_t commandCEROMACHINE = {NULL,"CEROMACHINE"	, NULL, NULL, commandHolahanlder, NULL, NULL, NULL};
static tinysh_cmd_t commandSTART = {NULL,"START"				, NULL, NULL, commandHolahanlder, NULL, NULL, NULL};


void HandlerConsolePutchar(unsigned char c){
	putchar(c);
}

typedef struct{
	int numero;
	int baud;
}console_cfg;

typedef struct{
	console_cfg consola;
}app_cfg;

int app_main(void) {

	tinysh_add_command(&commandDOWN);
	tinysh_add_command(&commandUP);
	tinysh_add_command(&commandSPIN);
	tinysh_add_command(&commandWAIT);
	tinysh_add_command(&commandSTOP);
	tinysh_add_command(&commandCEROMACHINE);
	tinysh_add_command(&commandSTART);

	tinysh_set_putchar(HandlerConsolePutchar);

	tinysh_init();
	char c;
	while(1){
		c = getchar();
		tinysh_char_in(c);
		printf(ddsds->name);

	}
	return 0;
}


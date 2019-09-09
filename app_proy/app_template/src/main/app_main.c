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

//		api_print("asd %d");
		printf("asd\0");
		__CLEANSTIN();
	}
}

static tinysh_cmd_t commandHola = {NULL, "hola", NULL, NULL, commandHolahanlder, NULL, NULL, NULL};

void HandlerConsolePutchar(unsigned char c){
	putchar(c);
}

int app_main(void) {
	tinysh_add_command(&commandHola);
	tinysh_set_putchar(HandlerConsolePutchar);
	tinysh_init();

	char c;
	while(1){
		c = getchar();
		tinysh_char_in(c);
	}

	return 0;
}


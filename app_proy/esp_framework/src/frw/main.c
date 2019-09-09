/*
 ============================================================================
 Name        : esp_framework.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "app_main.h"

int main(void) {

	appcfg.uart1.baud= 9600;

	return app_main();
}

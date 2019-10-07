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


/*TODO:
 * Al ingresar el comando setstandarprogram verificar que ingre un argumento con el loop
 *
 *
 *
 *
 *
 *
 * */




//Flag to Run the pŕogram loaded
flagRun_t entry = STOP;

/*MOVEMENT COMMANDS*/
//PROCESS STANDARD:
static tinysh_cmd_t commandSETSTANDARDPROGRAM = 		{NULL,"SETSTANDARDPROGRAM", NULL, NULL, CommandSETSTANDARDPROGRAMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLOADPROGRAMSTANDARD = 		{NULL,"LOADPROGRAMSTANDARD", NULL, NULL, CommandLOADPROGRAMSTANDARDHandler, NULL, NULL, NULL};
//PROCESS CUSTOM:
static tinysh_cmd_t commandSETCOMMANDCUSTOMPROGRAM = 	{NULL,"SETCUSTOMPROGRAM", NULL, NULL, CommandSETCOMMANDCUSTOMPROGRAMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSETALLCUSTOMPROGRAM = 		{NULL,"SETALLCUSTOMPROGRAM", NULL, NULL, CommandSETALLCUSTOMPROGRAMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLOADPROGRAMCUSTOM = 			{NULL,"LOADPROGRAMCUSTOM", NULL, NULL, CommandLOADPROGRAMCUSTOMHandler, NULL, NULL, NULL};
//PROCESS DINAMIC:
static tinysh_cmd_t commandCLEANPROGRAMDINAMIC = 		{NULL,"CLEANPROGRAMDINAMIC", NULL, NULL, CommandCLEANPROGRAMDINAMICHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandADDSETALLCOMANDDINAMIC = 	{NULL,"ADDSETALLCOMANDDINAMIC", NULL, NULL, CommandADDSETALLCOMANDDINAMICHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLOADPROGRAMDINAMIC = 		{NULL,"LOADPROGRAMDINAMIC", NULL, NULL, CommandLOADPROGRAMDINAMICHandler, NULL, NULL, NULL};
//RUN LOADED PROCESS:
static tinysh_cmd_t commandRUN = 						{NULL,"RUN", NULL, NULL, CommandRUNHandler, NULL, NULL, NULL};
//SINGLE MOVEMENTS:
static tinysh_cmd_t commandUPFAST = 					{NULL,"UPFAST", NULL, NULL, CommandUPFASTHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandUP = 						{NULL,"UP", NULL, NULL, CommandUPHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandUPSLOW = 					{NULL,"UPSLOW", NULL, NULL, CommandUPSLOWHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDOWNFAST = 					{NULL,"DOWNFAST", NULL, NULL, CommandDOWNFASTHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDOWN = 						{NULL,"DOWN", NULL, NULL, CommandDOWNHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDOWNSLOW = 					{NULL,"DOWNSLOW", NULL, NULL, CommandDOWNSLOWHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSTOP = 						{NULL,"STOP", NULL, NULL, CommandSTOPHandler, NULL, NULL, NULL};
/*CONFIGURATION COMMANDS*/
static tinysh_cmd_t commandSAMPLE = 					{NULL,"LOADSAMPLE", NULL, NULL, commandSAMPLEHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandRECIPIENT = 					{NULL,"LOADRECIPIENT", NULL, NULL, commandRECIPIENTMHandler, NULL, NULL, NULL};

//ENVIROMENTAL CHAMBER:
static tinysh_cmd_t commandSETRH = 						{NULL,"SETRH", NULL, NULL, CommandSETRHHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSETTEMP = 					{NULL,"SETTEMP", NULL, NULL, CommandSETTEMPHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandACTIVATEENVIROMENTALCHAMBER ={NULL,"ACTIVATEENVIROMENTALCHAMBER", NULL, NULL, CommandACTIVATEENVIROMENTALCHAMBERHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDEACTIVATEENVIROMENTALCHAMBER ={NULL,"DEACTIVATEENVIROMENTALCHAMBER", NULL, NULL, CommandDEACTIVATEENVIROMENTALCHAMBERHandler, NULL, NULL, NULL};



void HandlerConsolePutchar(unsigned char c){
	putchar(c);
}


int app_main_dipcoater(void) {
	char c=0;

	//command initialization
	tinysh_add_command(&commandLOADPROGRAMSTANDARD);
	tinysh_add_command(&commandLOADPROGRAMCUSTOM);
	tinysh_add_command(&commandLOADPROGRAMDINAMIC);
	tinysh_add_command(&commandSETCOMMANDCUSTOMPROGRAM);
	tinysh_add_command(&commandSETSTANDARDPROGRAM);
	tinysh_add_command(&commandSETALLCUSTOMPROGRAM);
	tinysh_add_command(&commandCLEANPROGRAMDINAMIC);
	tinysh_add_command(&commandADDSETALLCOMANDDINAMIC);
	tinysh_add_command(&commandRUN);
	tinysh_add_command(&commandUPFAST);
	tinysh_add_command(&commandUP);
	tinysh_add_command(&commandUPSLOW);
	tinysh_add_command(&commandDOWNFAST);
	tinysh_add_command(&commandDOWN);
	tinysh_add_command(&commandDOWNSLOW);
	tinysh_add_command(&commandSTOP);
	tinysh_add_command(&commandSAMPLE);
	tinysh_add_command(&commandRECIPIENT);
	tinysh_add_command(&commandSETRH);
	tinysh_add_command(&commandSETTEMP);
	tinysh_add_command(&commandACTIVATEENVIROMENTALCHAMBER);
	tinysh_add_command(&commandDEACTIVATEENVIROMENTALCHAMBER);


	tinysh_set_putchar(HandlerConsolePutchar);

	tinysh_init();

	//queue initialization
	modQueue_Init(&queueconsolareception, bufferreception, 10, sizeof(processCommand_t));
	modQueue_Init(&queueconsolatransmit, buffertransmit, 10, sizeof(int));

	//Process & enviromental chamber initialization
	ProcessInit(&processDipCoating);
	EnviromentalChamberInit(&enviromentalChamberDipCoating);

	//Motor module initialization
	//MotorInit();

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



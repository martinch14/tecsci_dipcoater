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

// Global process_t for tinysh handler use
process_t processDipCoating;
enviromental_chamber_t enviromentalChamberDipCoating;


//Flag to Run the pŕogram loaded
flagRun_t entry = STOP;


/*PROCESS STANDARD HANDLERS*/

/*Handler to LOAD the standard process */
static void CommandLOADPROGRAMSTANDARDHandler(int argc, char **argv){
	ProcessLoadProgramStandard(&processDipCoating);
}

/*Handler to set up the LOOP COMMAND of the process standard
 1 arguments must be passed: N corresponding to the number of repetitions of the DWUW cycle  */
static void CommandSETSTANDARDPROGRAMHandler(int argc, char **argv){
	 processCommand_t aux_process_comand;

 if(2 == argc){
	 aux_process_comand.commandnumber=7;
	 aux_process_comand.argument.value.val=tinysh_get_arg_int(argc, argv, 1);
	 modQueue_Write(&queueconsolareception,&aux_process_comand);
	 ProcessSetProgramStandard();

 }
}

/*PROCESS CUSTOM HANDLERS*/

/*Handler to LOAD the custom process,
 the process it should have been set up before (if there is no set up the standard parameters will be loaded)  */
static void CommandLOADPROGRAMCUSTOMHandler(int argc, char **argv){

	ProcessLoadProgramCustom(&processDipCoating);
}

/*Handler to set up a single command of the custom Process
 4 arguments must be passed: command number (see manual), velocity, acceleration and the test parameter */
static void CommandSETCOMMANDCUSTOMPROGRAMHandler(int argc, char **argv){
	 processCommand_t aux_process_comand;

 if(5 == argc){	/*SET XX XX XX XX representa 5 comandos 	argc = 5*/
	 aux_process_comand.commandnumber=tinysh_get_arg_int(argc, argv, 1);
	 aux_process_comand.argument.spin.velocity=tinysh_get_arg_int(argc, argv, 2);
	 aux_process_comand.argument.spin.acceleration=tinysh_get_arg_int(argc, argv, 3);
	 aux_process_comand.argument.spin.test=tinysh_get_arg_int(argc, argv, 4);

	 modQueue_Write(&queueconsolareception,&aux_process_comand);
	 ProcessSetProgramCustom();
 }
}

/*Handler to set up a all commands of the custom Process
 30 arguments must be passed: the 3 parameters for each command in increasing order
 obs: for the LOOP comand it must be passed 3 parameters,
 but only the first one will be used the other two will be discarded
  */
static void CommandSETALLCUSTOMPROGRAMHandler(int argc, char **argv){
	 processCommand_t aux_process_comand;
	 uint8_t i=1;
	 uint8_t j=0;
	 uint8_t k=1;
	 uint8_t l=2;

	 printf("%d\n",argc);

	 if(31 == argc){

		 for(i=1;i<=9;i++){

			 aux_process_comand.commandnumber=tinysh_get_arg_int(argc, argv, i-1);
			 aux_process_comand.argument.spin.velocity=tinysh_get_arg_int(argc, argv, i+j);
			 aux_process_comand.argument.spin.acceleration=tinysh_get_arg_int(argc, argv, i+k);
			 aux_process_comand.argument.spin.test=tinysh_get_arg_int(argc, argv, i+l);
			 modQueue_Write(&queueconsolareception,&aux_process_comand);
			 ProcessSetProgramCustom();

			 j+=2;
			 k+=2;
			 l+=2;
		 }
	 }
}

/*PROCESS DINAMIC HANDLERS*/

static void CommandLOADPROGRAMDINAMICHandler(int argc, char **argv){
		ProcessLoadProgramDinamic(&processDipCoating);
}

/*Handler to add and set all the dinamic program
 * the arguments can be up to 129, in groups of 4:
 * 	comand number, velocity acceleration and test*/
static void CommandADDSETALLCOMANDDINAMICHandler(int argc, char **argv) {
	processCommand_t aux_process_comand;
	int i=1;
	int a=1;
	int b=2;
	int c=3;
	int d=4;

	ProcessCleanDinamic();

	if (5<argc<129) {
		 for(i=1;i<=argc;i++){

			 if(i==a){
				 aux_process_comand.commandnumber=tinysh_get_arg_int(argc, argv, i);
				 a+=4;
			 }
			 if(i==b){
				 aux_process_comand.argument.spin.velocity=tinysh_get_arg_int(argc, argv, i);
				 b+=4;
			 }
			 if(i==c){
				 aux_process_comand.argument.spin.acceleration=tinysh_get_arg_int(argc, argv, i);
				 c+=4;
			 }

			 if(i==d){
			 aux_process_comand.argument.spin.test=tinysh_get_arg_int(argc, argv, i);

			 modQueue_Write(&queueconsolareception,&aux_process_comand);
			 ProcessAddSetProgramDinamic();
			 d+=4;
			 }

		 }
	}
}

/*Handler to Clean all the dinamic program*/
static void CommandCLEANPROGRAMDINAMICHandler(int argc, char **argv) {
	ProcessCleanDinamic();
}


/*RUN PROCESS HANDLER*/

/*Handler to RUn the LOADED process,
 * turn on the RUN flag to RUN*/
static void CommandRUNHandler(int argc, char **argv){
	entry=RUN;
}

/*SINGLE MOVEMENT HANDLERS*/
/*this tiny handlers only call the corresponding process function*/
static void CommandUPFASTHandler(int argc, char **argv) {
	ProcessUpFastCommand();
}

static void CommandUPHandler(int argc, char **argv) {
	ProcessUpCommand();
}

static void CommandUPSLOWHandler(int argc, char **argv) {
	ProcessUpSlowCommand();
}

static void CommandDOWNFASTHandler(int argc, char **argv) {
	ProcessDownFastCommand();
}

static void CommandDOWNHandler(int argc, char **argv) {
	ProcessDownCommand();
}

static void CommandDOWNSLOWHandler(int argc, char **argv) {
	ProcessDownSlowCommand();
}

static void CommandSTOPHandler(int argc, char **argv) {
	ProcessStopCommand();
}
/*CONFIGURATION HANDLERS*/
static void commandSAMPLEHandler(int argc, char **argv) {

}

static void commandRECIPIENTMHandler(int argc, char **argv) {
}

/*ENVIROMENTAL CHAMBER HANDLERS*/
static void CommandSETRHHandler(int argc, char **argv) {
	 if(2 == argc){
		 EnviromentalChamberSetRH(&enviromentalChamberDipCoating,(uint8_t)tinysh_get_arg_int(argc, argv, 1));
	 }
}
static void CommandSETTEMPHandler(int argc, char **argv) {
	 if(2 == argc){
		 EnviromentalChamberSetTemp(&enviromentalChamberDipCoating,(uint8_t)tinysh_get_arg_int(argc, argv, 1));
	 }
}
static void CommandACTIVATEENVIROMENTALCHAMBERHandler(int argc, char **argv) {
	 if(1 == argc){
		 EnviromentalChamberActivateRH(&enviromentalChamberDipCoating);
	 }
}
static void CommandDEACTIVATEENVIROMENTALCHAMBERHandler(int argc, char **argv) {
	 if(1 == argc){
		 EnviromentalChamberDeactivateRH(&enviromentalChamberDipCoating);
	 }
}




/*MOVEMENT COMMANDS*/
//PROCESS STANDARD:
static tinysh_cmd_t commandSETSTANDARDPROGRAM = 		{NULL,"SETSTANDARDPROGRAM", NULL, NULL, CommandSETSTANDARDPROGRAMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLOADPROGRAMSTANDARD = 	{NULL,"LOADPROGRAMSTANDARD", NULL, NULL, CommandLOADPROGRAMSTANDARDHandler, NULL, NULL, NULL};
//PROCESS CUSTOM:
static tinysh_cmd_t commandSETCOMMANDCUSTOMPROGRAM = 		{NULL,"SETCUSTOMPROGRAM", NULL, NULL, CommandSETCOMMANDCUSTOMPROGRAMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSETALLCUSTOMPROGRAM = 	{NULL,"SETALLCUSTOMPROGRAM", NULL, NULL, CommandSETALLCUSTOMPROGRAMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLOADPROGRAMCUSTOM = 		{NULL,"LOADPROGRAMCUSTOM", NULL, NULL, CommandLOADPROGRAMCUSTOMHandler, NULL, NULL, NULL};
//PROCESS DINAMIC:
static tinysh_cmd_t commandCLEANPROGRAMDINAMIC = 		{NULL,"CLEANPROGRAMDINAMIC", NULL, NULL, CommandCLEANPROGRAMDINAMICHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandADDSETALLCOMANDDINAMIC = 		{NULL,"ADDSETALLCOMANDDINAMIC", NULL, NULL, CommandADDSETALLCOMANDDINAMICHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLOADPROGRAMDINAMIC = 		{NULL,"LOADPROGRAMDINAMIC", NULL, NULL, CommandLOADPROGRAMDINAMICHandler, NULL, NULL, NULL};
//RUN LOADED PROCESS:
static tinysh_cmd_t commandRUN = 					{NULL,"RUN", NULL, NULL, CommandRUNHandler, NULL, NULL, NULL};
//SINGLE MOVEMENTS:
static tinysh_cmd_t commandUPFAST = 					{NULL,"UPFAST", NULL, NULL, CommandUPFASTHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandUP = 					{NULL,"UP", NULL, NULL, CommandUPHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandUPSLOW = 					{NULL,"UPSLOW", NULL, NULL, CommandUPSLOWHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDOWNFAST = 					{NULL,"DOWNFAST", NULL, NULL, CommandDOWNFASTHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDOWN = 					{NULL,"DOWN", NULL, NULL, CommandDOWNHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDOWNSLOW = 					{NULL,"DOWNSLOW", NULL, NULL, CommandDOWNSLOWHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSTOP = 					{NULL,"STOP", NULL, NULL, CommandSTOPHandler, NULL, NULL, NULL};
/*CONFIGURATION COMMANDS*/
static tinysh_cmd_t commandSAMPLE = 	{NULL,"LOADSAMPLE", NULL, NULL, commandSAMPLEHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandRECIPIENT = 		{NULL,"LOADRECIPIENT", NULL, NULL, commandRECIPIENTMHandler, NULL, NULL, NULL};

//ENVIROMENTAL CHAMBER:
static tinysh_cmd_t commandSETRH = 					{NULL,"SETRH", NULL, NULL, CommandSETRHHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSETTEMP = 					{NULL,"SETTEMP", NULL, NULL, CommandSETTEMPHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandACTIVATEENVIROMENTALCHAMBER = 					{NULL,"ACTIVATEENVIROMENTALCHAMBER", NULL, NULL, CommandACTIVATEENVIROMENTALCHAMBERHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDEACTIVATEENVIROMENTALCHAMBER = 					{NULL,"DEACTIVATEENVIROMENTALCHAMBER", NULL, NULL, CommandDEACTIVATEENVIROMENTALCHAMBERHandler, NULL, NULL, NULL};



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
	//	switch (){
	//	case UPDATE:    //update with new process or arguments
	//			ProcessCommandUpdate();
	//			break;
	//	case RUN;
	//			ProcessRun();
	//			break;
	//	}
}

void task_motor(void){

}
void task_enviromental_chamber(void){
	EnviromentalChamberRun(&enviromentalChamberDipCoating);
}



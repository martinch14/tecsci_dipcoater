/*
 * command.c
 *
 *  Created on: 7 oct. 2019
 *      Author: martin
 */


/*PROCESS STANDARD HANDLERS*/

/*Handler to LOAD the standard process */

#include "../../components/api/include/process.h"
#include "../../components/api/include/mod_queue.h"
#include "../../main/app_main_dipcoater.h"

extern process_t processDipCoating;
extern flagRun_t entry;


void CommandLOADPROGRAMSTANDARDHandler(int argc, char **argv){
	ProcessLoadProgramStandard(&processDipCoating);
}

/*Handler to set up the LOOP COMMAND of the process standard
 1 arguments must be passed: N corresponding to the number of repetitions of the DWUW cycle  */
void CommandSETSTANDARDPROGRAMHandler(int argc, char **argv){
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
void CommandLOADPROGRAMCUSTOMHandler(int argc, char **argv){

	ProcessLoadProgramCustom(&processDipCoating);
}

/*Handler to set up a single command of the custom Process
 4 arguments must be passed: command number (see manual), velocity, acceleration and the test parameter */
void CommandSETCOMMANDCUSTOMPROGRAMHandler(int argc, char **argv){
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
void CommandSETALLCUSTOMPROGRAMHandler(int argc, char **argv){
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

void CommandLOADPROGRAMDINAMICHandler(int argc, char **argv){
		ProcessLoadProgramDinamic(&processDipCoating);
}

/*Handler to add and set all the dinamic program
 * the arguments can be up to 129, in groups of 4:
 * 	comand number, velocity acceleration and test*/
void CommandADDSETALLCOMANDDINAMICHandler(int argc, char **argv) {
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
void CommandCLEANPROGRAMDINAMICHandler(int argc, char **argv) {
	ProcessCleanDinamic();
}


/*RUN PROCESS HANDLER*/

/*Handler to RUn the LOADED process,
 * turn on the RUN flag to RUN*/
void CommandRUNHandler(int argc, char **argv){
	entry=RUN;
}

/*SINGLE MOVEMENT HANDLERS*/
/*this tiny handlers only call the corresponding process function*/
void CommandUPFASTHandler(int argc, char **argv) {
	ProcessUpFastCommand();
}

void CommandUPHandler(int argc, char **argv) {
	ProcessUpCommand();
}

void CommandUPSLOWHandler(int argc, char **argv) {
	ProcessUpSlowCommand();
}

void CommandDOWNFASTHandler(int argc, char **argv) {
	ProcessDownFastCommand();
}

void CommandDOWNHandler(int argc, char **argv) {
	ProcessDownCommand();
}

void CommandDOWNSLOWHandler(int argc, char **argv) {
	ProcessDownSlowCommand();
}

void CommandSTOPHandler(int argc, char **argv) {
	ProcessStopCommand();
}
/*CONFIGURATION HANDLERS*/
void commandSAMPLEHandler(int argc, char **argv) {

}

void commandRECIPIENTMHandler(int argc, char **argv) {
}

/*ENVIROMENTAL CHAMBER HANDLERS*/
void CommandSETRHHandler(int argc, char **argv) {
	 if(2 == argc){
		 EnviromentalChamberSetRH(&enviromentalChamberDipCoating,(uint8_t)tinysh_get_arg_int(argc, argv, 1));
	 }
}
void CommandSETTEMPHandler(int argc, char **argv) {
	 if(2 == argc){
		 EnviromentalChamberSetTemp(&enviromentalChamberDipCoating,(uint8_t)tinysh_get_arg_int(argc, argv, 1));
	 }
}
void CommandACTIVATEENVIROMENTALCHAMBERHandler(int argc, char **argv) {
	 if(1 == argc){
		 EnviromentalChamberActivateRH(&enviromentalChamberDipCoating);
	 }
}
void CommandDEACTIVATEENVIROMENTALCHAMBERHandler(int argc, char **argv) {
	 if(1 == argc){
		 EnviromentalChamberDeactivateRH(&enviromentalChamberDipCoating);
	 }
}


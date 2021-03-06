/*
 * command.c
 *
 *  Created on: 7 oct. 2019
 *      Author: martin
 */


/*PROCESS STANDARD HANDLERS*/

/*Handler to LOAD the standard process */
#include <stdio.h>

#include "process.h"
#include "tinysh.h"
#include "mod_queue.h"
#include "app_main_dipcoater.h"

#include "command.h"


extern process_t processDipCoating;
extern flagRun_t entry;


#define MAX_ESTATIC_COMMAND 	8


/*PROCESS STANDARD HANDLERS*/

void CommandLOADPROGRAMSTANDARDHandler(int argc, char **argv){
	printf("\r\n");
	ProcessLoadProgramStandard(&processDipCoating);
}

/*Handler to set up the LOOP COMMAND of the process standard
 1 arguments must be passed: N corresponding to the number of repetitions of the DWUW cycle  */
void CommandSETSTANDARDPROGRAMHandler(int argc, char **argv) {
	processCommand_t aux_process_comand;
	printf("\r\n");
	if (2 == argc) {
		aux_process_comand.commandnumber = PROCESS_COMMAND_LOOP - 2;
		aux_process_comand.argument.value.val = tinysh_get_arg_int(argc, argv,
				1);
		modQueue_Write(&queueconsolareception, &aux_process_comand);
		ProcessSetProgramStandard();

	} else {
		printf(
				"Ingrese 2 argumentos -> SETSTANDARDPROGRAM X(numero de loops)\r\n");
	}

}

/*PROCESS CUSTOM HANDLERS*/

/*Handler to LOAD the custom process,
 the process it should have been set up before (if there is no set up the standard parameters will be loaded)  */
void CommandLOADPROGRAMCUSTOMHandler(int argc, char **argv){
	printf("\r\n");
	ProcessLoadProgramCustom(&processDipCoating);
}


/*Handler to set up a single command of the custom Process
 4 arguments must be passed: command number (see manual), velocity, acceleration and the test parameter */
void CommandSETCOMMANDCUSTOMPROGRAMHandler(int argc, char **argv) {
	processCommand_t aux_process_comand;
	printf("\r\n");
	if (5 == argc) { /*SETCUSTOMPROGRAM  XX XX XX XX representa 5 comandos 	argc = 5*/
		aux_process_comand.commandnumber = tinysh_get_arg_int(argc, argv, 1);

		if (aux_process_comand.commandnumber == 6) {
			aux_process_comand.argument.value.val = tinysh_get_arg_int(argc,argv, 2);
		}

		else {
			aux_process_comand.argument.spin.velocity = tinysh_get_arg_int(argc,argv, 2);
			aux_process_comand.argument.spin.acceleration = tinysh_get_arg_int(argc,argv, 3);
			aux_process_comand.argument.spin.displacement_z = tinysh_get_arg_int(argc, argv,4);
		}

		modQueue_Write(&queueconsolareception, &aux_process_comand);
		ProcessSetProgramCustom();
	}
}



/*Handler to set up APP command of the custom Process
 4 arguments must be passed: command number (see manual), velocity, acceleration and the test parameter */
void CommandSETCOMMANDCUSTOMPROGRAMAPPHandler(int argc, char **argv) {
	processCommand_t aux_process_comand;
	printf("\r\n");
	if (15 == argc) { /*SETCUSTOMPROGRAMAPP 2 XX XX 3 XX 4 XX XX 5 XX 6 X X X  	*/

		//vel acel down loop
		processDipCoating.command[2].argument.spin.velocity = (tinysh_get_arg_int(argc, argv,2) * 212.2719735 * 1.34 );
		processDipCoating.command[2].argument.spin.acceleration = (tinysh_get_arg_int(argc, argv,3) * 3.537866);

		//wait down
		processDipCoating.command[3].argument.wait.time = tinysh_get_arg_int(argc, argv,5);

		//vel acel up loop
		processDipCoating.command[4].argument.spin.velocity = (tinysh_get_arg_int(argc, argv,7) * 212.2719735 * 1.34 );


		processDipCoating.command[4].argument.spin.acceleration = (tinysh_get_arg_int(argc, argv,8) * 3.537866);

		//wait up
		processDipCoating.command[5].argument.wait.time = tinysh_get_arg_int(argc, argv,10);


		//Loop
		processDipCoating.command[6].argument.spin.velocity = tinysh_get_arg_int(argc, argv,12);


		//Desplazamiento hasta muestra
		processDipCoating.config.displacement_to_sample =    tinysh_get_arg_int(argc, argv,13);


		//Profundidad DIP
		processDipCoating.config.displacement_delta_sample = ( tinysh_get_arg_int(argc, argv,14) / 0.00007851 );

		printf("Datos Cargados!!!\r\n");

	}
}


/*Handler to set up a all commands of the custom Process
 25 arguments must be passed: the 3 parameters for each command in increasing order
 obs: for the LOOP comand it must be passed 3 parameters,
 but only the first one will be used the other two will be discarded
  Example
  SETALLCUSTOMPROGRAM   x x x  y y y c c c d d d s s s m m m w w w q q q
  	  	  command ->  	  0		1		2	3		4	5		6	 7

  */
void CommandSETALLCUSTOMPROGRAMHandler(int argc, char **argv){
	 processCommand_t aux_process_comand;
	 uint8_t i=1;
	 uint8_t j=0;
	 uint8_t k=1;
	 uint8_t l=2;

	 printf("\r\n");
	 printf("%d\n",argc);

	 if(  25 == argc){

		 printf("setall ok/r/n");

		 for(i=1;i<=MAX_ESTATIC_COMMAND;i++){


			 aux_process_comand.commandnumber=tinysh_get_arg_int(argc, argv, i-1);
			 printf("%d \t %d\r\n",i-1,aux_process_comand.commandnumber);
			 aux_process_comand.argument.spin.velocity=tinysh_get_arg_int(argc, argv, i+j);
			 printf("%d \t %d\r\n",i+j, aux_process_comand.argument.spin.velocity);
			 aux_process_comand.argument.spin.acceleration=tinysh_get_arg_int(argc, argv, i+k);
			 printf("%d \t %d\r\n",i+k,aux_process_comand.argument.spin.acceleration);
			 aux_process_comand.argument.spin.displacement_z=tinysh_get_arg_int(argc, argv, i+l);
			 printf("%d \t %d\r\n",i+l,aux_process_comand.argument.spin.displacement_z);
			 modQueue_Write(&queueconsolareception,&aux_process_comand);
			 ProcessSetProgramCustom();

			 j+=2;
			 k+=2;
			 l+=2;
		 }
	 }
}


/*Handler to found Cero Machine Target */
void CommandCEROMACHINEHandler(int argc, char **argv){
	printf("\r\n");
	ProcessCeroMachineCommand();
}




/*RUN PROCESS HANDLER*/

/*Handler to RUn the LOADED process,
 * turn on the RUN flag to RUN*/
void CommandRUNHandler(int argc, char **argv){
	entry=RUN;
	printf("\r\n");
	ProcessRunCommand();
}

/*SINGLE MOVEMENT HANDLERS*/
/*this tiny handlers only call the corresponding process function*/
void CommandUPFASTHandler(int argc, char **argv) {
	printf("\r\n");
	ProcessUpFastCommand();
}

void CommandUPHandler(int argc, char **argv) {
	printf("\r\n");
	ProcessUpCommand();
}

void CommandUPSLOWHandler(int argc, char **argv) {
	printf("\r\n");
	ProcessUpSlowCommand();
}

void CommandDOWNFASTHandler(int argc, char **argv) {
	printf("\r\n");
	/*
	 * comando nuevo
	 * comando.head.number =  numero de este comando
	 * comadno.contenido = le cargo los argumentos
	 */
	/*validar comando , limites de arg*/
	/* escribir en cola  */

	ProcessDownFastCommand();
}

void CommandDOWNHandler(int argc, char **argv) {
	printf("\r\n");
	ProcessDownCommand();
}

void CommandDOWNSLOWHandler(int argc, char **argv) {
	printf("\r\n");
	ProcessDownSlowCommand();
}

void CommandSTOPHandler(int argc, char **argv) {
	printf("\r\n");
	ProcessStopCommand();
}
/*CONFIGURATION HANDLERS*/
void commandSAMPLEHandler(int argc, char **argv) {
	printf("\r\n");
}

void commandRECIPIENTMHandler(int argc, char **argv) {
	printf("\r\n");
}

void CommandREADDATAHandler(int argc, char **argv) {
	printf("\r\n");
	ProcessREADDATACommand();
}

void CommandENA_DRIVERHandler(int argc, char **argv) {
	printf("\r\n");
	ProcessENA_DRIVERCommand();
}

void CommandDIS_DRIVERHandler(int argc, char **argv) {
	printf("\r\n");
	ProcessDIS_DRIVERCommand();
}


void CommandCERO_SAMPLEHandler(int argc, char **argv) {

	processConfig_t aux_process_config;

	printf("\r\n");
	if (2 == argc) {
		processDipCoating.config.displacement_to_sample = tinysh_get_arg_int(argc, argv,1);

	}
	else{
	ProcessCERO_SAMPLECommand();
	}
}

void CommandDELTADIPHandler(int argc, char **argv) {

	processConfig_t aux_process_config;

	printf("\r\n");
	if (2 == argc) {

		aux_process_config.displacement_delta_sample = tinysh_get_arg_int(argc, argv,1);
		modQueue_Write(&queueconsolareception, &aux_process_config);
		ProcessDELTADIPCommand();

	} else {
		printf(
				"Ingrese 2 argumentos -> DELTADIP X( Profundidad de Inmersion de la muenstra en mm)\r\n");
	}


}




/*ENVIROMENTAL CHAMBER HANDLERS*/
//void CommandSETRHHandler(int argc, char **argv) {
//	printf("\r\n");
//	if (2 == argc) {
//		EnviromentalChamberSetRH(&enviromentalChamberDipCoating,
//				(uint8_t) tinysh_get_arg_int(argc, argv, 1));
//	}
//}
//void CommandSETTEMPHandler(int argc, char **argv) {
//	printf("\r\n");
//	if (2 == argc) {
//		EnviromentalChamberSetTemp(&enviromentalChamberDipCoating,
//				(uint8_t) tinysh_get_arg_int(argc, argv, 1));
//	}
//}
//void CommandACTIVATEENVIROMENTALCHAMBERHandler(int argc, char **argv) {
//	printf("\r\n");
//	if (1 == argc) {
//		EnviromentalChamberActivateRH(&enviromentalChamberDipCoating);
//	}
//}
//void CommandDEACTIVATEENVIROMENTALCHAMBERHandler(int argc, char **argv) {
//	printf("\r\n");
//	if (1 == argc) {
//		EnviromentalChamberDeactivateRH(&enviromentalChamberDipCoating);
//	}
//}


/*PROCESS DINAMIC HANDLERS*/

//void CommandLOADPROGRAMDINAMICHandler(int argc, char **argv){
//		ProcessLoadProgramDinamic(&processDipCoating);
//}

/*Handler to add and set all the dinamic program
 * the arguments can be up to 129, in groups of 4:
 * 	comand number, velocity acceleration and test*/
//void CommandADDSETALLCOMANDDINAMICHandler(int argc, char **argv) {
//	processCommand_t aux_process_comand;
//	int i=1;
//	int a=1;
//	int b=2;
//	int c=3;
//	int d=4;
//
//	ProcessCleanDinamic();
//
//	if (5<argc<129) {
//		 for(i=1;i<=argc;i++){
//
//			 if(i==a){
//				 aux_process_comand.commandnumber=tinysh_get_arg_int(argc, argv, i);
//				 a+=4;
//			 }
//			 if(i==b){
//				 aux_process_comand.argument.spin.velocity=tinysh_get_arg_int(argc, argv, i);
//				 b+=4;
//			 }
//			 if(i==c){
//				 aux_process_comand.argument.spin.acceleration=tinysh_get_arg_int(argc, argv, i);
//				 c+=4;
//			 }
//
//			 if(i==d){
//			 aux_process_comand.argument.spin.test=tinysh_get_arg_int(argc, argv, i);
//
//			 modQueue_Write(&queueconsolareception,&aux_process_comand);
//			 ProcessAddSetProgramDinamic();
//			 d+=4;
//			 }
//
//		 }
//	}
//}

/*Handler to Clean all the dinamic program*/
//void CommandCLEANPROGRAMDINAMICHandler(int argc, char **argv) {
//	ProcessCleanDinamic();
//}

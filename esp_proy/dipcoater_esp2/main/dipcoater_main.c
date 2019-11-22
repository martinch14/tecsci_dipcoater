/*
 * dipcoater_main.c
 *
 *  Created on: 12 sep. 2019
 *      Author: martin
 */

#include <stdio.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_system.h"
#include "esp_spi_flash.h"


#include "include/app_main_dipcoater.h"


/*TODO    BUGS
 * LOADPROGRAMSTANDARD    se tiene q cargar antes de SETSTANDARDPROGRAM para que se pueda ejecutar
 *
 * */

flagRun_t entry = STOP;


//void task_process(process_t *processDipCoating){
//	if (entry==RUN){
//		ProcessRun(processDipCoating);
//		entry=STOP;
//	}
//}

void xtaskprocess(void *pvParameter) {

	ProcessInit(&processDipCoating);
//	ProcessLoadProgramStandard(&processDipCoating);

	while (1) {
		if (entry==RUN){
			ProcessRun(&processDipCoating);
			entry=STOP;
		}
		vTaskDelay(50 / portTICK_RATE_MS);
	}
}

void xtasktinysh(void *pvParameter) {

	static char c = 0;


	tinysh_init();
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

	//EMRGENCY STOP
	static tinysh_cmd_t commandSTOP = 						{NULL,"STOP", NULL, NULL, CommandSTOPHandler, NULL, NULL, NULL};

	/*CONFIGURATION COMMANDS*/
	static tinysh_cmd_t commandSAMPLE = 					{NULL,"LOADSAMPLE", NULL, NULL, commandSAMPLEHandler, NULL, NULL, NULL};
	static tinysh_cmd_t commandRECIPIENT = 					{NULL,"LOADRECIPIENT", NULL, NULL, commandRECIPIENTMHandler, NULL, NULL, NULL};

	//ENVIROMENTAL CHAMBER:
	static tinysh_cmd_t commandSETRH = 						{NULL,"SETRH", NULL, NULL, CommandSETRHHandler, NULL, NULL, NULL};
	static tinysh_cmd_t commandSETTEMP = 					{NULL,"SETTEMP", NULL, NULL, CommandSETTEMPHandler, NULL, NULL, NULL};
	static tinysh_cmd_t commandACTIVATEENVIROMENTALCHAMBER ={NULL,"ACTIVATEENVIROMENTALCHAMBER", NULL, NULL, CommandACTIVATEENVIROMENTALCHAMBERHandler, NULL, NULL, NULL};
	static tinysh_cmd_t commandDEACTIVATEENVIROMENTALCHAMBER ={NULL,"DEACTIVATEENVIROMENTALCHAMBER", NULL, NULL, CommandDEACTIVATEENVIROMENTALCHAMBERHandler, NULL, NULL, NULL};

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


	while (1) {
		c = getchar();
		if (c != 0xFF){
		tinysh_char_in(c);
		}
		vTaskDelay(50 / portTICK_RATE_MS);

	}
}


void app_main(void) {


	xTaskCreate(&xtasktinysh,"Tinysh Task",16384,NULL,2,NULL );
	xTaskCreate(&xtaskprocess,"Process Task",8192,NULL,2,NULL );


}













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


#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"


#include "TMCMotionController.h"
#include "TMC5130.h"
#include "SysTick.h"



#include "include/app_main_dipcoater.h"


/*TODO    BUGS
 * LOADPROGRAMSTANDARD    se tiene q cargar antes de SETSTANDARDPROGRAM para que se pueda ejecutar
 * COMANDO STOP O FINISH  VER DE DARLE MAS PRIORIDAD PARA QUE SE EJECUTE SIN ESPERAR A QUE TERMINE EL CICLO DE TRABAJO
 * -->>>  EN COMANDO STOP AUMENTAR EL INDEX HASTA 10
 *
 *
 *
 *
 * */

flagRun_t entry = STOP;

extern void TMC5130_init(void);



spi_device_handle_t  spi_dev;



void xtaskprocess(void *pvParameter) {

	ProcessInit(&processDipCoating);

	while (1) {
		if (entry == RUN) {
			ProcessRun(&processDipCoating);
			entry = STOP;
		}
		vTaskDelay(20 / portTICK_RATE_MS);
	}
}

void xtasktinysh(void *pvParameter) {

//	static char c = 0;
	char c = 0;
	tinysh_init();
	//queue initialization
	modQueue_Init(&queueconsolareception, bufferreception, 10, sizeof(processCommand_t));
	modQueue_Init(&queueconsolatransmit,  buffertransmit, 10, sizeof(int));
	/*MOVEMENT COMMANDS*/
	//PROCESS STANDARD:
	static tinysh_cmd_t commandSETSTANDARDPROGRAM = 		{NULL,"SETSTANDARDPROGRAM", NULL, NULL, CommandSETSTANDARDPROGRAMHandler, NULL, NULL, NULL};
	static tinysh_cmd_t commandLOADPROGRAMSTANDARD = 		{NULL,"LOADPROGRAMSTANDARD", NULL, NULL, CommandLOADPROGRAMSTANDARDHandler, NULL, NULL, NULL};
	//PROCESS CUSTOM:
	static tinysh_cmd_t commandSETCOMMANDCUSTOMPROGRAM = 	{NULL,"SETCUSTOMPROGRAM", NULL, NULL, CommandSETCOMMANDCUSTOMPROGRAMHandler, NULL, NULL, NULL};
	static tinysh_cmd_t commandSETALLCUSTOMPROGRAM = 		{NULL,"SETALLCUSTOMPROGRAM", NULL, NULL, CommandSETALLCUSTOMPROGRAMHandler, NULL, NULL, NULL};
	static tinysh_cmd_t commandLOADPROGRAMCUSTOM = 			{NULL,"LOADPROGRAMCUSTOM", NULL, NULL, CommandLOADPROGRAMCUSTOMHandler, NULL, NULL, NULL};
	//PROCESS DINAMIC:

//	static tinysh_cmd_t commandCLEANPROGRAMDINAMIC = 		{NULL,"CLEANPROGRAMDINAMIC", NULL, NULL, CommandCLEANPROGRAMDINAMICHandler, NULL, NULL, NULL};
//	static tinysh_cmd_t commandADDSETALLCOMANDDINAMIC = 	{NULL,"ADDSETALLCOMANDDINAMIC", NULL, NULL, CommandADDSETALLCOMANDDINAMICHandler, NULL, NULL, NULL};
//	static tinysh_cmd_t commandLOADPROGRAMDINAMIC = 		{NULL,"LOADPROGRAMDINAMIC", NULL, NULL, CommandLOADPROGRAMDINAMICHandler, NULL, NULL, NULL};

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
//	static tinysh_cmd_t commandSETRH = 						{NULL,"SETRH", NULL, NULL, CommandSETRHHandler, NULL, NULL, NULL};
//	static tinysh_cmd_t commandSETTEMP = 					{NULL,"SETTEMP", NULL, NULL, CommandSETTEMPHandler, NULL, NULL, NULL};
//	static tinysh_cmd_t commandACTIVATEENVIROMENTALCHAMBER ={NULL,"ACTIVATEENVIROMENTALCHAMBER", NULL, NULL, CommandACTIVATEENVIROMENTALCHAMBERHandler, NULL, NULL, NULL};
//	static tinysh_cmd_t commandDEACTIVATEENVIROMENTALCHAMBER ={NULL,"DEACTIVATEENVIROMENTALCHAMBER", NULL, NULL, CommandDEACTIVATEENVIROMENTALCHAMBERHandler, NULL, NULL, NULL};

	//command initialization
	tinysh_add_command(&commandLOADPROGRAMSTANDARD);
	tinysh_add_command(&commandLOADPROGRAMCUSTOM);
//	tinysh_add_command(&commandLOADPROGRAMDINAMIC);
	tinysh_add_command(&commandSETCOMMANDCUSTOMPROGRAM);
	tinysh_add_command(&commandSETSTANDARDPROGRAM);
	tinysh_add_command(&commandSETALLCUSTOMPROGRAM);
//	tinysh_add_command(&commandCLEANPROGRAMDINAMIC);
//	tinysh_add_command(&commandADDSETALLCOMANDDINAMIC);
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
//	tinysh_add_command(&commandSETRH);
//	tinysh_add_command(&commandSETTEMP);
//	tinysh_add_command(&commandACTIVATEENVIROMENTALCHAMBER);
//	tinysh_add_command(&commandDEACTIVATEENVIROMENTALCHAMBER);


	while (1) {
		c = getchar();
		//Para que no devuelva basura por la consola
		if (c != 0xFF){
		tinysh_char_in(c);
		}
		vTaskDelay(20 / portTICK_RATE_MS);

	}
}


void xtaskmotor(void *pvParameter) {


	spi_dev = tmc_spi_init();
	tmcmotioncontroller_init();
	TMC5130_init();

	Evalboards.driverEnable = DRIVER_ENABLE;
	Evalboards.ch1.enableDriver(DRIVER_DISABLE);
	vTaskDelay(250 / portTICK_RATE_MS);
	Evalboards.ch1.enableDriver(DRIVER_ENABLE);
	vTaskDelay(250 / portTICK_RATE_MS);

	Evalboards.ch1.writeRegister(0, 0x00, 0x00002680); // writing value 0x00002680 = 9856 = 0.0 to address 0 = 0x00(GCONF)
	Evalboards.ch1.writeRegister(0, 0x03, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 1 = 0x03(SLAVECONF)
	Evalboards.ch1.writeRegister(0, 0x05, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 2 = 0x05(X_COMPARE)
	Evalboards.ch1.writeRegister(0, 0x10, 0x00070A03); // writing value 0x00070A03 = 461315 = 0.0 to address 3 = 0x10(IHOLD_IRUN)
	Evalboards.ch1.writeRegister(0, 0x11, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 4 = 0x11(TPOWERDOWN)
	Evalboards.ch1.writeRegister(0, 0x13, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 5 = 0x13(TPWMTHRS)
	Evalboards.ch1.writeRegister(0, 0x14, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 6 = 0x14(TCOOLTHRS)
	Evalboards.ch1.writeRegister(0, 0x15, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 7 = 0x15(THIGH)
	Evalboards.ch1.writeRegister(0, 0x20, 0x00000001); // writing value 0x00000001 = 1 = 0.0 to address 8 = 0x20(RAMPMODE)
	Evalboards.ch1.writeRegister(0, 0x21, 0xFFED8910); // writing value 0xFFED8910 = 0 = 0.0 to address 9 = 0x21(XACTUAL)
	Evalboards.ch1.writeRegister(0, 0x23, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 10 = 0x23(VSTART)
	Evalboards.ch1.writeRegister(0, 0x24, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 11 = 0x24(A1)
	Evalboards.ch1.writeRegister(0, 0x25, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 12 = 0x25(V1)
	Evalboards.ch1.writeRegister(0, 0x26, 0x00002710); // writing value 0x00001388 = 5000 = 0.0 to address 13 = 0x26(AMAX)	   //10000  2710
	Evalboards.ch1.writeRegister(0, 0x27, 0x00000000); // writing value 0x00002710 = 10000 = 0.0 to address 14 = 0x27(VMAX)    //SETEO DE VELOCIDAD 0
	Evalboards.ch1.writeRegister(0, 0x28, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 15 = 0x28(DMAX)
	Evalboards.ch1.writeRegister(0, 0x2A, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 16 = 0x2A(D1)
	Evalboards.ch1.writeRegister(0, 0x2B, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 17 = 0x2B(VSTOP)
	Evalboards.ch1.writeRegister(0, 0x2C, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)
	Evalboards.ch1.writeRegister(0, 0x2D, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 19 = 0x2D(XTARGET)
	Evalboards.ch1.writeRegister(0, 0x33, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 20 = 0x33(VDCMIN)
	Evalboards.ch1.writeRegister(0, 0x34, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 21 = 0x34(SW_MODE)
	Evalboards.ch1.writeRegister(0, 0x38, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 22 = 0x38(ENCMODE)
	Evalboards.ch1.writeRegister(0, 0x39, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 23 = 0x39(X_ENC)
	Evalboards.ch1.writeRegister(0, 0x3A, 0x00010000); // writing value 0x00010000 = 65536 = 0.0 to address 24 = 0x3A(ENC_CONST)
	Evalboards.ch1.writeRegister(0, 0x60, 0xAAAAB554); // writing value 0xAAAAB554 = 0 = 0.0 to address 25 = 0x60(MSLUT[0])
	Evalboards.ch1.writeRegister(0, 0x61, 0x4A9554AA); // writing value 0x4A9554AA = 1251300522 = 0.0 to address 26 = 0x61(MSLUT[1])
	Evalboards.ch1.writeRegister(0, 0x62, 0x24492929); // writing value 0x24492929 = 608774441 = 0.0 to address 27 = 0x62(MSLUT[2])
	Evalboards.ch1.writeRegister(0, 0x63, 0x10104222); // writing value 0x10104222 = 269500962 = 0.0 to address 28 = 0x63(MSLUT[3])
	Evalboards.ch1.writeRegister(0, 0x64, 0xFBFFFFFF); // writing value 0xFBFFFFFF = 0 = 0.0 to address 29 = 0x64(MSLUT[4])
	Evalboards.ch1.writeRegister(0, 0x65, 0xB5BB777D); // writing value 0xB5BB777D = 0 = 0.0 to address 30 = 0x65(MSLUT[5])
	Evalboards.ch1.writeRegister(0, 0x66, 0x49295556); // writing value 0x49295556 = 1227445590 = 0.0 to address 31 = 0x66(MSLUT[6])
	Evalboards.ch1.writeRegister(0, 0x67, 0x00404222); // writing value 0x00404222 = 4211234 = 0.0 to address 32 = 0x67(MSLUT[7])
	Evalboards.ch1.writeRegister(0, 0x68, 0xFFFF8056); // writing value 0xFFFF8056 = 0 = 0.0 to address 33 = 0x68(MSLUTSEL)
	Evalboards.ch1.writeRegister(0, 0x69, 0x00F70000); // writing value 0x00F70000 = 16187392 = 0.0 to address 34 = 0x69(MSLUTSTART)
	Evalboards.ch1.writeRegister(0, 0x6C, 0x000101D5); // writing value 0x000101D5 = 66005 = 0.0 to address 35 = 0x6C(CHOPCONF)
	Evalboards.ch1.writeRegister(0, 0x6D, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 36 = 0x6D(COOLCONF)
	Evalboards.ch1.writeRegister(0, 0x6E, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 37 = 0x6E(DCCTRL)
	Evalboards.ch1.writeRegister(0, 0x70, 0x000504C8); // writing value 0x000504C8 = 328904 = 0.0 to address 38 = 0x70(PWMCONF)
	Evalboards.ch1.writeRegister(0, 0x72, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 39 = 0x72(ENCM_CTRL)
	vTaskDelay(250 / portTICK_RATE_MS);


	while (1) {

//			Evalboards.ch1.enableDriver(DRIVER_ENABLE);
//			Evalboards.ch1.left(0,0x00009C40); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)
//			printf("IZQUIERDA!!\r\n");
//			vTaskDelay(3000 / portTICK_RATE_MS);
//			Evalboards.ch1.right(0,0x00009C40); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)
//			printf("DERECHA!!\n\r");
			vTaskDelay(10000 / portTICK_RATE_MS);



	}
}



void app_main(void) {


	xTaskCreate(&xtasktinysh,"Tinysh Task",16384,NULL,2,NULL );
	xTaskCreate(&xtaskprocess,"Process Task",8192,NULL,2,NULL );
	xTaskCreate(&xtaskmotor,"Process Motor",16384,NULL,3,NULL );

}













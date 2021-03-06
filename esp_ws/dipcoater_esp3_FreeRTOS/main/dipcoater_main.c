/**************************************************************************************************
**  (c) Copyright 2019: Martin Abel Gambarotta <magambarotta@gmail.com>
**  This file is part of DipCoater_Tecsci.
**
**  DipCoater_Tecsci is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  DipCoater_Tecsci is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with DipCoater_Tecsci.  If not, see <https://www.gnu.org/licenses/>.
*************************************************************************************************/


/** @file 	dipcoater_main.c
 ** @brief 	Implementacion main principal
 **
 **| REV | YYYY.MM.DD | Autor           | Descripción de los cambios                              |
 **|-----|------------|-----------------|---------------------------------------------------------|
 **|   1 | 2020.05.28 | magambarotta    | Version inicial 									      |
 ** @addtogroup aplicacion
 ** @{ */


/*=====[Inclusion de su propia cabecera]=====================================*/
#include "include/app_main_dipcoater.h"

/*=====[Inclusiones de dependencias de funciones privadas]===================*/

#include <stdio.h>
#include <stdlib.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


#include "freertos/semphr.h"

#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_system.h"
#include "esp_spi_flash.h"


#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>


#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"


#include "TMCMotionController.h"
#include "TMC5130.h"
//#include "SysTick.h"


#include "http_server.h"
#include "dns_server.h"

#include "wifi_manager.h"


#include <cJSON.h>
#include "socket_tcp_server.h"


/*TODO
 *
 * CAMBIOs
 *
 * x) pasar a queue de RTOS    OK
 * x) agregar comentarios y licencias
 * x) analizar software respirador trinamic y ver si al resetear borran todos los parametros de la configruacion inicial
 *
 * BUGS
 *
 * x) ejecucion RUN desde consola sin programa cargado
 * x) perdida de pasos al tirar comandos individuales
 * x) stop cuanto se esta ejecutando el programa      COMANDO STOP CUANDO SE ESTA EJECUTANDO EL PROCESO -> (SE DEBERIA REINICIAR EL PROCESO)
 * ver pequeña diferencia en la posicion luego del STOP
 * x) los comandos individuales van por cola, pero el seteo de los procesos no ,,,, todo debe ir por cola      DELTADIP  y CERO_SAMPLE tmb por cola
 *
 *
 * x) agregar validacion de todos los argumentos en CommandSETCOMMANDCUSTOMPROGRAMAPPHandler
 *
 * --------------------------------APP----------------------------------------
 * PARA AGREGAR APP
 * modulo bluetooth
 *
 * FALLA de APP
 * x) cuando STATUS en 1 la aplicacion muestra como que se desconecto , 1 significa que esta ejecutando un comando o corriendo un programa , no deberia aparecer desconectado
 * x) boton establecer ( sin movimiento anterior)
 * x) cuando vulevo de ajustes se desconecta
 *
 * */



/*=====[Macros de definición de constantes privadas]=========================*/

//#define PORT CONFIG_EXAMPLE_PORT

//#define PORT 3333


//#define CONFIG_EXAMPLE_IPV4
//#define CONFIG_EXAMPLE_IPV4 192.168.100.110


/*=====[Definiciones de variables globales privadas]=========================*/


//static const char *TAG_TASK_SOCKET = "task_socket";
//static const char *TAG_TASK_WIFI_MANAGER = "task_wifi_manager_main";



flagRun_t entry = STOP;

//Funciones para futuros desarrollos
//PROCESS STANDARD:
//static tinysh_cmd_t commandSETSTANDARDPROGRAM = 		{NULL,"SETSTANDARDPROGRAM", NULL, NULL, CommandSETSTANDARDPROGRAMHandler, NULL, NULL, NULL};
//static tinysh_cmd_t commandLOADPROGRAMSTANDARD = 		{NULL,"LOADPROGRAMSTANDARD", NULL, NULL, CommandLOADPROGRAMSTANDARDHandler, NULL, NULL, NULL};
//static tinysh_cmd_t commandSETCOMMANDCUSTOMPROGRAM = 	{NULL,"SETCUSTOMPROGRAM", NULL, NULL, CommandSETCOMMANDCUSTOMPROGRAMHandler, NULL, NULL, NULL};
//static tinysh_cmd_t commandSETALLCUSTOMPROGRAM = 		{NULL,"SETALLCUSTOMPROGRAM", NULL, NULL, CommandSETALLCUSTOMPROGRAMHandler, NULL, NULL, NULL};
//PROCESS DINAMIC:
//	static tinysh_cmd_t commandCLEANPROGRAMDINAMIC = 		{NULL,"CLEANPROGRAMDINAMIC", NULL, NULL, CommandCLEANPROGRAMDINAMICHandler, NULL, NULL, NULL};
//	static tinysh_cmd_t commandADDSETALLCOMANDDINAMIC = 	{NULL,"ADDSETALLCOMANDDINAMIC", NULL, NULL, CommandADDSETALLCOMANDDINAMICHandler, NULL, NULL, NULL};
//	static tinysh_cmd_t commandLOADPROGRAMDINAMIC = 		{NULL,"LOADPROGRAMDINAMIC", NULL, NULL, CommandLOADPROGRAMDINAMICHandler, NULL, NULL, NULL};




//PROCESS CUSTOM APP:

static tinysh_cmd_t commandLOADPROGRAMCUSTOM = 			{NULL,"LOADPROGRAMCUSTOM", NULL, NULL, CommandLOADPROGRAMCUSTOMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSETCOMMANDCUSTOMPROGRAMAPP = {NULL,"SETCUSTOMPROGRAMAPP", NULL, NULL, CommandSETCOMMANDCUSTOMPROGRAMAPPHandler, NULL, NULL, NULL};


//RUN LOADED PROCESS:
static tinysh_cmd_t commandRUN = 						{NULL,"RUN", NULL, NULL, CommandRUNHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandCERO_MACHINE = 				{NULL,"CERO_MACHINE", NULL, NULL, CommandCEROMACHINEHandler, NULL, NULL, NULL};
//SINGLE MOVEMENTS:
static tinysh_cmd_t commandUPFAST = 					{NULL,"UPFAST", NULL, NULL, CommandUPFASTHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandUP = 						{NULL,"UP", NULL, NULL, CommandUPHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandUPSLOW = 					{NULL,"UPSLOW", NULL, NULL, CommandUPSLOWHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDOWNFAST = 					{NULL,"DOWNFAST", NULL, NULL, CommandDOWNFASTHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDOWN = 						{NULL,"DOWN", NULL, NULL, CommandDOWNHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDOWNSLOW = 					{NULL,"DOWNSLOW", NULL, NULL, CommandDOWNSLOWHandler, NULL, NULL, NULL};

//EMRGENCY STOP
static tinysh_cmd_t commandSTOP = 						{NULL,"STOP", NULL, NULL, CommandSTOPHandler, NULL, NULL, NULL};

//EMRGENCY READDATA
static tinysh_cmd_t commandREADDATA = 					{NULL,"READDATA", NULL, NULL, CommandREADDATAHandler, NULL, NULL, NULL};

/*CONFIGURATION COMMANDS*/
static tinysh_cmd_t commandCERO_SAMPLE = 					{NULL,"CERO_SAMPLE", NULL, NULL, CommandCERO_SAMPLEHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDELTADIP = 					{NULL,"DELTADIP", NULL, NULL, CommandDELTADIPHandler, NULL, NULL, NULL};

/* HABILITACION Y DESHABILITCAION GENERAL DEL DRIVER DEL MOTOR*/
static tinysh_cmd_t commandENA_DRIVER = 					{NULL,"ENA_DRIVER", NULL, NULL, CommandENA_DRIVERHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDIS_DRIVER = 					{NULL,"DIS_DRIVER", NULL, NULL, CommandDIS_DRIVERHandler, NULL, NULL, NULL};

static tinysh_cmd_t commandRESET= 					{NULL,"RESET", NULL, NULL, CommandRESETHandler, NULL, NULL, NULL};




/*=====[Definiciones de Variables globales publicas externas]================*/
extern void TMC5130_init(void);



/*=====[Definiciones de Variables globales publicas]=========================*/
/*=====[Definiciones de Variables globales privadas]=========================*/
/*=====[Prototipos de funciones privadas]====================================*/



/*=====[Implementaciones de funciones publicas]==============================*/


void xtaskprocess(void *pvParameter) {

	ProcessInit(&processDipCoating);
	processDipCoating.config.status=0;

	vTaskDelay(1000 / portTICK_RATE_MS);
	ProcessCeroMachineCommand();

	xQueueConsolaReceptionPrograma= xQueueCreate( 5, sizeof( process_t) );

	while (TRUE) {

		ProcessCommand();

		if (entry == RUN ) {
			processDipCoating.config.status=1;
			ProcessRun(&processDipCoating);
			entry = STOP;
			processDipCoating.config.status=0;
		}
		vTaskDelay(200 / portTICK_RATE_MS);
	}
}




// Task para el procesamiento de todos los comandos que llegan por consola o por tcp port
void xtasktinysh(void *pvParameter) {

	char c = 0;
	tinysh_init();

	/* Creo Cola de mensajes para los comandos individuales recibidos por pantalla
	 * los comandos pueden llegar por el puerto serie, con por el socket TCP
	 * */
	xQueueConsolaReception= xQueueCreate( 10, sizeof( processCommand_t) );


	/*MOVEMENT COMMANDS*/
	//command initialization
//	tinysh_add_command(&commandLOADPROGRAMSTANDARD);
//	tinysh_add_command(&commandSETCOMMANDCUSTOMPROGRAM);
//	tinysh_add_command(&commandSETSTANDARDPROGRAM);
//	tinysh_add_command(&commandSETALLCUSTOMPROGRAM);
//

	tinysh_add_command(&commandLOADPROGRAMCUSTOM);
	tinysh_add_command(&commandSETCOMMANDCUSTOMPROGRAMAPP);
	tinysh_add_command(&commandRUN);
	tinysh_add_command(&commandCERO_MACHINE);
	tinysh_add_command(&commandUPFAST);
	tinysh_add_command(&commandUP);
	tinysh_add_command(&commandUPSLOW);
	tinysh_add_command(&commandDOWNFAST);
	tinysh_add_command(&commandDOWN);
	tinysh_add_command(&commandDOWNSLOW);
	tinysh_add_command(&commandSTOP);
	tinysh_add_command(&commandREADDATA);
	tinysh_add_command(&commandCERO_SAMPLE);
	tinysh_add_command(&commandDELTADIP);
	tinysh_add_command(&commandENA_DRIVER);
	tinysh_add_command(&commandDIS_DRIVER);
	tinysh_add_command(&commandRESET);


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

	/*Inicializacion del puerto SPI para la transmision de datos con el Circuito Integrado TMC5130*/
	spi_dev = tmc_spi_init();

	/*Inicializacion de funciones de la API Trinamic*/
	tmcmotioncontroller_init();
	TMC5130_init();

	Evalboards.driverEnable = DRIVER_ENABLE;
	Evalboards.ch1.enableDriver(DRIVER_DISABLE);
	vTaskDelay(250 / portTICK_RATE_MS);
	Evalboards.ch1.enableDriver(DRIVER_ENABLE);
	vTaskDelay(250 / portTICK_RATE_MS);


	Evalboards.ch1.writeRegister(0,0x00, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 0 = 0x00(GCONF)
	Evalboards.ch1.writeRegister(0,0x03, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 1 = 0x03(SLAVECONF)
	Evalboards.ch1.writeRegister(0,0x05, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 2 = 0x05(X_COMPARE)
	Evalboards.ch1.writeRegister(0,0x10, 	0x00070A01); 		// writing value 0x00070A00 = 461312 = 0.0 to address 3 = 0x10(IHOLD_IRUN)
	Evalboards.ch1.writeRegister(0,0x11, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 4 = 0x11(TPOWERDOWN)
	Evalboards.ch1.writeRegister(0,0x13, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 5 = 0x13(TPWMTHRS)
	Evalboards.ch1.writeRegister(0,0x14, 	0x00000D1B); 		// writing value 0x00000D1B = 3355 = 0.0 to address 6 = 0x14(TCOOLTHRS)
	Evalboards.ch1.writeRegister(0,0x15, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 7 = 0x15(THIGH)
	Evalboards.ch1.writeRegister(0,0x20, 	0x00000000); 		// writing value 0x00000001 = 1 = 0.0 to address 8 = 0x20(RAMPMODE)
	Evalboards.ch1.writeRegister(0,0x21, 	0x00000000); 		// writing value 0xFFFA6B8C = 0 = 0.0 to address 9 = 0x21(XACTUAL)
	Evalboards.ch1.writeRegister(0,0x23, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 10 = 0x23(VSTART)
	Evalboards.ch1.writeRegister(0,0x24, 	0x000003E8); 		// writing value 0x000003E8 = 1000 = 0.0 to address 11 = 0x24(A1)
	Evalboards.ch1.writeRegister(0,0x25, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 12 = 0x25(V1)
	Evalboards.ch1.writeRegister(0,0x26, 	0x000003E8); 		// writing value 0x000003E8 = 1000 = 0.0 to address 13 = 0x26(AMAX)
	Evalboards.ch1.writeRegister(0,0x27, 	0x0000C350); 		// writing value 0x00000000 = 0 = 0.0 to address 14 = 0x27(VMAX)
	Evalboards.ch1.writeRegister(0,0x28, 	0x000002BC); 		// writing value 0x000002BC = 700 = 0.0 to address 15 = 0x28(DMAX)
	Evalboards.ch1.writeRegister(0,0x2A, 	0x00000578); 		// writing value 0x00000578 = 1400 = 0.0 to address 16 = 0x2A(D1)
	Evalboards.ch1.writeRegister(0,0x2B, 	0x0000000A); 		// writing value 0x0000000A = 10 = 0.0 to address 17 = 0x2B(VSTOP)
	Evalboards.ch1.writeRegister(0,0x2C, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)
	Evalboards.ch1.writeRegister(0,0x2D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 19 = 0x2D(XTARGET)
	Evalboards.ch1.writeRegister(0,0x33, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 20 = 0x33(VDCMIN)
	Evalboards.ch1.writeRegister(0,0x34, 	0x00000400); 		// writing value 0x00000400 = 1024 = 0.0 to address 21 = 0x34(SW_MODE)
	Evalboards.ch1.writeRegister(0,0x38, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 22 = 0x38(ENCMODE)
	Evalboards.ch1.writeRegister(0,0x39, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 23 = 0x39(X_ENC)
	Evalboards.ch1.writeRegister(0,0x3A, 	0x00010000); 		// writing value 0x00010000 = 65536 = 0.0 to address 24 = 0x3A(ENC_CONST)
	Evalboards.ch1.writeRegister(0,0x60, 	0xAAAAB554); 		// writing value 0xAAAAB554 = 0 = 0.0 to address 25 = 0x60(MSLUT[0])
	Evalboards.ch1.writeRegister(0,0x61, 	0x4A9554AA); 		// writing value 0x4A9554AA = 1251300522 = 0.0 to address 26 = 0x61(MSLUT[1])
	Evalboards.ch1.writeRegister(0,0x62, 	0x24492929); 		// writing value 0x24492929 = 608774441 = 0.0 to address 27 = 0x62(MSLUT[2])
	Evalboards.ch1.writeRegister(0,0x63, 	0x10104222); 		// writing value 0x10104222 = 269500962 = 0.0 to address 28 = 0x63(MSLUT[3])
	Evalboards.ch1.writeRegister(0,0x64, 	0xFBFFFFFF); 		// writing value 0xFBFFFFFF = 0 = 0.0 to address 29 = 0x64(MSLUT[4])
	Evalboards.ch1.writeRegister(0,0x65, 	0xB5BB777D); 		// writing value 0xB5BB777D = 0 = 0.0 to address 30 = 0x65(MSLUT[5])
	Evalboards.ch1.writeRegister(0,0x66, 	0x49295556); 		// writing value 0x49295556 = 1227445590 = 0.0 to address 31 = 0x66(MSLUT[6])
	Evalboards.ch1.writeRegister(0,0x67, 	0x00404222); 		// writing value 0x00404222 = 4211234 = 0.0 to address 32 = 0x67(MSLUT[7])
	Evalboards.ch1.writeRegister(0,0x68, 	0xFFFF8056); 		// writing value 0xFFFF8056 = 0 = 0.0 to address 33 = 0x68(MSLUTSEL)
	Evalboards.ch1.writeRegister(0,0x69, 	0x00F70000); 		// writing value 0x00F70000 = 16187392 = 0.0 to address 34 = 0x69(MSLUTSTART)
	Evalboards.ch1.writeRegister(0,0x6C, 	0x000101D5); 		// writing value 0x000101D5 = 66005 = 0.0 to address 35 = 0x6C(CHOPCONF)
	Evalboards.ch1.writeRegister(0,0x6D, 	0x01030000); 		// writing value 0x01030000 = 16973824 = 0.0 to address 36 = 0x6D(COOLCONF)
	Evalboards.ch1.writeRegister(0,0x6E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 37 = 0x6E(DCCTRL)
	Evalboards.ch1.writeRegister(0,0x70, 	0x000504C8); 		// writing value 0x000504C8 = 328904 = 0.0 to address 38 = 0x70(PWMCONF)
	Evalboards.ch1.writeRegister(0,0x72, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 39 = 0x72(ENCM_CTRL)


	vTaskDelay(250 / portTICK_RATE_MS);
	Evalboards.ch1.enableDriver(DRIVER_DISABLE);
	//borro la tarea, solo se necesita para la configuracion inicial con delay entre habilitaciones y envios por spi de conf
	vTaskDelete(NULL);

}

void xtaskTcpSocketServer(void *pvParameters) {
		socket_tcp_run();
}




void xtaskmonitorstatus(void *pvParameter) {

	char datos[16];

	while (1) {
		vTaskDelay(1000 / portTICK_RATE_MS);
		if (sock_global > 0){
//		itoa(processDipCoating.config.status,datos,10);


		sprintf(datos,"STATUS %d\r\n",processDipCoating.config.status);

		/*Configuracion ->  processDipCoating.config.status  =  1(UNO)  cuando se esta ejecutando un programa o se corre un comando individual   o  0(CERO) cuando no se esta ejecutando ningun programa */


		send(sock_global, &datos, sizeof(datos) , 0);
		//send(sock_global, &processDipCoating.config.status, sizeof(processDipCoating.config.status) , 0);

		}
	}
}


void app_main(void) {

//	/* start the wifi manager */
	wifi_manager_start();

//	/* register a callback as an example to how you can integrate your code with the wifi manager */
//	wifi_manager_set_callback(EVENT_STA_GOT_IP, &cb_connection_ok);


	xTaskCreate(&xtasktinysh, "Tinysh Task", 8192, NULL, 4, NULL);
	xTaskCreate(&xtaskprocess, "Process Task", 8192, NULL, 2, NULL);
	xTaskCreate(&xtaskmotor, "Process Motor Task", 8192, NULL, 2, NULL);
	xTaskCreate(&xtaskTcpSocketServer,"Tcp Socket Server Task", 8192, NULL, 2, NULL);
	xTaskCreate(&xtaskmonitorstatus, "Monitor Status Task", 8192, NULL, 2, NULL);


}




///* brief this is an exemple of a callback that you can setup in your own app to get notified of wifi manager event */
//void cb_connection_ok(void *pvParameter){
//	ESP_LOGI(TAG_TASK_WIFI_MANAGER, "I have a connection!");
//	//Se ejecuta esta funcion cuando el dispositivo se conecta a la red WIFI configurada
//	//No se ejecuta si esta en modo Access Point
//
//}




/** @} Final de la definición del modulo para doxygen */






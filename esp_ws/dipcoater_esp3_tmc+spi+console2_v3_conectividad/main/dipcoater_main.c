/*
 * dipcoater_main.c
 *
 *  Created on: 12 sep. 2019
 *      Author: martin
 */

#include <stdio.h>
#include <stdlib.h>
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


#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>


#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"


#include "TMCMotionController.h"
#include "TMC5130.h"
#include "SysTick.h"

//#include <esp_http_server.h>


#include "http_server.h"
#include "dns_server.h"
#include "json.h"
#include "wifi_manager.h"

#include "include/app_main_dipcoater.h"


/*TODO    BUGS
 * LOADPROGRAMSTANDARD    se tiene q cargar antes de SETSTANDARDPROGRAM para que se pueda ejecutar
 * Al ingresar el comando setstandarprogram verificar que ingrese  un argumento con el loop
 *
 *
 * revisar CUSTOM ALL
 * multiplicacion x constantes de desplazamiento
 * json
 * carga de comando wait
 *
 * COMANDO STOP CUANDO SE ESTA EJECUTANDO EL PROCESO -> (SE DEBERIA REINICIAR EL PROCESO)
 *
 * */


//#define PORT CONFIG_EXAMPLE_PORT
#define PORT 3333
//#define CONFIG_EXAMPLE_IPV4
//#define CONFIG_EXAMPLE_IPV4 192.168.100.110


static const char *TAG_TASK_SOCKET = "task_socket";
static const char *TAG_TASK_WIFI_MANAGER = "task_wifi_manager_main";
flagRun_t entry = STOP;



extern void TMC5130_init(void);
//PROCESS STANDARD:
static tinysh_cmd_t commandSETSTANDARDPROGRAM = 		{NULL,"SETSTANDARDPROGRAM", NULL, NULL, CommandSETSTANDARDPROGRAMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLOADPROGRAMSTANDARD = 		{NULL,"LOADPROGRAMSTANDARD", NULL, NULL, CommandLOADPROGRAMSTANDARDHandler, NULL, NULL, NULL};

//PROCESS CUSTOM:

static tinysh_cmd_t commandLOADPROGRAMCUSTOM = 			{NULL,"LOADPROGRAMCUSTOM", NULL, NULL, CommandLOADPROGRAMCUSTOMHandler, NULL, NULL, NULL};

static tinysh_cmd_t commandSETCOMMANDCUSTOMPROGRAM = 	{NULL,"SETCUSTOMPROGRAM", NULL, NULL, CommandSETCOMMANDCUSTOMPROGRAMHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSETCOMMANDCUSTOMPROGRAMAPP = {NULL,"SETCUSTOMPROGRAMAPP", NULL, NULL, CommandSETCOMMANDCUSTOMPROGRAMAPPHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandSETALLCUSTOMPROGRAM = 		{NULL,"SETALLCUSTOMPROGRAM", NULL, NULL, CommandSETALLCUSTOMPROGRAMHandler, NULL, NULL, NULL};


//PROCESS DINAMIC:
//	static tinysh_cmd_t commandCLEANPROGRAMDINAMIC = 		{NULL,"CLEANPROGRAMDINAMIC", NULL, NULL, CommandCLEANPROGRAMDINAMICHandler, NULL, NULL, NULL};
//	static tinysh_cmd_t commandADDSETALLCOMANDDINAMIC = 	{NULL,"ADDSETALLCOMANDDINAMIC", NULL, NULL, CommandADDSETALLCOMANDDINAMICHandler, NULL, NULL, NULL};
//	static tinysh_cmd_t commandLOADPROGRAMDINAMIC = 		{NULL,"LOADPROGRAMDINAMIC", NULL, NULL, CommandLOADPROGRAMDINAMICHandler, NULL, NULL, NULL};

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


/* HABILITACION Y DESHABILITCAION GENERAL DEL DRIVER DEL MOTOR*/
static tinysh_cmd_t commandENA_DRIVER = 					{NULL,"ENA_DRIVER", NULL, NULL, CommandENA_DRIVERHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDIS_DRIVER = 					{NULL,"DIS_DRIVER", NULL, NULL, CommandDIS_DRIVERHandler, NULL, NULL, NULL};


static tinysh_cmd_t commandCERO_SAMPLE = 					{NULL,"CERO_SAMPLE", NULL, NULL, CommandCERO_SAMPLEHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandDELTADIP = 					{NULL,"DELTADIP", NULL, NULL, CommandDELTADIPHandler, NULL, NULL, NULL};


void xtaskprocess(void *pvParameter) {

	ProcessInit(&processDipCoating);
	processDipCoating.config.status=0;

	while (1) {

		if (entry == RUN ) {
			processDipCoating.config.status=1;
			ProcessRun(&processDipCoating);
			entry = STOP;
			processDipCoating.config.status=0;
		}
		vTaskDelay(500 / portTICK_RATE_MS);
	}
}

void xtasktinysh(void *pvParameter) {

	char c = 0;
	tinysh_init();
	//queue initialization
	modQueue_Init(&queueconsolareception, bufferreception, 10, sizeof(processCommand_t));
	modQueue_Init(&queueconsolatransmit,  buffertransmit, 10, sizeof(int));
	/*MOVEMENT COMMANDS*/


	//command initialization
	tinysh_add_command(&commandLOADPROGRAMSTANDARD);
	tinysh_add_command(&commandLOADPROGRAMCUSTOM);

//	tinysh_add_command(&commandLOADPROGRAMDINAMIC);
	tinysh_add_command(&commandSETCOMMANDCUSTOMPROGRAM);
	tinysh_add_command(&commandSETCOMMANDCUSTOMPROGRAMAPP);


	tinysh_add_command(&commandSETSTANDARDPROGRAM);
	tinysh_add_command(&commandSETALLCUSTOMPROGRAM);
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


	tinysh_add_command(&commandENA_DRIVER);
	tinysh_add_command(&commandDIS_DRIVER);

	tinysh_add_command(&commandCERO_SAMPLE);
	tinysh_add_command(&commandDELTADIP);

	while (1) {
				c = getchar();
				//Para que no devuelva basura por la consola
				if (c != 0xFF){
					tinysh_char_in(c);
				}
		vTaskDelay(100 / portTICK_RATE_MS);
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

static void tcp_server_task(void *pvParameters) {
	char rx_buffer[128];
	char addr_str[128];
	int addr_family;
	int ip_protocol;
	int i;

	while (1) {

//#ifdef CONFIG_EXAMPLE_IPV4
		struct sockaddr_in dest_addr;
		dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(PORT);
		addr_family = AF_INET;
		ip_protocol = IPPROTO_IP;
		inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);

//#else 	//IPV6
//        struct sockaddr_in6 dest_addr;
//        bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
//        dest_addr.sin6_family = AF_INET6;
//        dest_addr.sin6_port = htons(PORT);
//        addr_family = AF_INET6;
//        ip_protocol = IPPROTO_IPV6;
//        inet6_ntoa_r(dest_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
//#endif

		int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
		if (listen_sock < 0) {
			ESP_LOGE(TAG_TASK_SOCKET, "Unable to create socket: errno %d", errno);
			break;
		}
		ESP_LOGI(TAG_TASK_SOCKET, "Socket created");

		int err = bind(listen_sock, (struct sockaddr*) &dest_addr,
				sizeof(dest_addr));
		if (err != 0) {
			ESP_LOGE(TAG_TASK_SOCKET, "Socket unable to bind: errno %d", errno);
			break;
		}
		ESP_LOGI(TAG_TASK_SOCKET, "Socket bound, port %d", PORT);

		err = listen(listen_sock, 1);
		if (err != 0) {
			ESP_LOGE(TAG_TASK_SOCKET, "Error occurred during listen: errno %d", errno);
			break;
		}
		ESP_LOGI(TAG_TASK_SOCKET, "Socket listening");

		struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
		uint addr_len = sizeof(source_addr);
//      int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
//      int sock_global = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
		sock_global = accept(listen_sock, (struct sockaddr*) &source_addr,
				&addr_len);

		printf("Numero de Socket: %d\r\n", sock_global);

		if (sock_global < 0) {
			ESP_LOGE(TAG_TASK_SOCKET, "Unable to accept connection: errno %d", errno);
			break;
		}
		ESP_LOGI(TAG_TASK_SOCKET, "Socket accepted");

		while (1) {

			int len = recv(sock_global, rx_buffer, sizeof(rx_buffer) - 1, 0);

			// Error occurred during receiving
			if (len < 0) {
				ESP_LOGE(TAG_TASK_SOCKET, "recv failed: errno %d", errno);
				break;
			}
			// Connection closed
			else if (len == 0) {
				ESP_LOGI(TAG_TASK_SOCKET, "Connection closed");
				break;
			}
			// Data received
			else {
				// Get the sender's ip address as string
				if (source_addr.sin6_family == PF_INET) {
					inet_ntoa_r(
							((struct sockaddr_in*) &source_addr)->sin_addr.s_addr,
							addr_str, sizeof(addr_str) - 1);
				} else if (source_addr.sin6_family == PF_INET6) {
					inet6_ntoa_r(source_addr.sin6_addr, addr_str,
							sizeof(addr_str) - 1);
				}

				rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
				ESP_LOGI(TAG_TASK_SOCKET, "Received %d bytes from %s:", len, addr_str);
				ESP_LOGI(TAG_TASK_SOCKET, "%s", rx_buffer);

//				if (processDipCoating.config.status == 0){
//						c = getchar();
//
//						//Para que no devuelva basura por la consola
//						if (c != 0xFF){
//
//							tinysh_char_in(c);
//								}
//
//				}
//				else {
//					// si llega algun comando mientras el config.status == 1 lo descarto porque la maquina ya esta realizando alguna accion
//					c = getchar();
//
//				}

///				if (processDipCoating.config.status == 0){

				for (i = 0; i < len; i++) {
					tinysh_char_in(rx_buffer[i]);
				}
				//Para que  valide lo recibido!
				tinysh_char_in('\n');

				//test para devolucion de datos -- devuelvo un dato de la estructura del proceso , el index
				// int err = send(sock_global, rx_buffer, len, 0);
///				}
///				else tinysh_char_in('\n');

				if (err < 0) {
				ESP_LOGE(TAG_TASK_SOCKET, "Error occurred during sending: errno %d",errno);
				break;
				}
			}
		}

		if (sock_global != -1) {
			ESP_LOGE(TAG_TASK_SOCKET, "Shutting down socket and restarting...");
			//shutdown(sock_global, 0);   /*TEST MARTES SOCKET*/
			close(sock_global); /*TEST MARTES SOCKET*/

			////FIX sacado de internet  https://www.esp32.com/viewtopic.php?t=10335
			//shutdown(listen_sock,0);  /*TEST MARTES SOCKET*/
			close(listen_sock);
			vTaskDelay(10);

		}
	}

	//vTaskDelete(NULL); /*TEST MARTES SOCKET*/

}


void xtaskemergencystop(void *pvParameter) {
	int32_t lectura;
	vTaskDelay(1000 / portTICK_RATE_MS);
	ProcessCeroMachineCommand();
//	vTaskDelay(15000 / portTICK_RATE_MS);
	//Limites superior e inferior de la maquina, si se sobrepasan los limites ejecuto un CEROMACHINE
//	while (1){
//		//LIMITE SUPERIOR en posicion: 127372 +1      x2 = 254744
//		//LIMITE INFERIOR en posicion: 4445287
//		Evalboards.ch1.readRegister(0, 0x21, &lectura);
//		if (lectura  <=  0x0000F8C6  ){
//			ProcessCeroMachineCommand();
//			vTaskDelay(10000 / portTICK_RATE_MS);
//		}
//		if ( lectura >= 0x0043d467){
//			ProcessCeroMachineCommand();
//		}
//		vTaskDelay(250 / portTICK_RATE_MS);
//		}
		//Testear el estado de algun pin, deshabilitar driver ,reiniciar dispositivo
	vTaskDelete(NULL);
}



void xtaskmonitorstatus(void *pvParameter) {

	char datos[16];

	while (1) {
		vTaskDelay(1000 / portTICK_RATE_MS);
		if (sock_global > 0){
//		itoa(processDipCoating.config.status,datos,10);


		sprintf(datos,"STATUS %d\r\n",processDipCoating.config.status);

		send(sock_global, &datos, sizeof(datos) , 0);
		//send(sock_global, &processDipCoating.config.status, sizeof(processDipCoating.config.status) , 0);

		}
	}
}

void xtaskcommand(void *pvParameter) {

	while (1) {

		ProcessCommand();
		vTaskDelay(200 / portTICK_RATE_MS);
		}

}



/* brief this is an exemple of a callback that you can setup in your own app to get notified of wifi manager event */
void cb_connection_ok(void *pvParameter){
	ESP_LOGI(TAG_TASK_WIFI_MANAGER, "I have a connection!");
	//Se ejecuta esta funcion cuando el dispositivo se conecta a la red WIFI configurada
	//No se ejecuta si esta en modo Access Point

}



void app_main(void) {

	/* start the wifi manager */
	wifi_manager_start();

	/* register a callback as an example to how you can integrate your code with the wifi manager */
	wifi_manager_set_callback(EVENT_STA_GOT_IP, &cb_connection_ok);

	xTaskCreate(&xtasktinysh, "Tinysh Task", 8192, NULL, 2, NULL);
	xTaskCreate(&xtaskprocess, "Process Task", 8192, NULL, 4, NULL);
	xTaskCreate(&xtaskcommand, "Command Task", 8192, NULL, 2, NULL);
	xTaskCreate(&xtaskmotor, "Process Motor Task", 8192, NULL, 2, NULL);
	xTaskCreate(&tcp_server_task, "tcp_server Task", 8192, NULL, 2, NULL);
	xTaskCreate(&xtaskmonitorstatus, "Monitor Status Task", 8192, NULL, 2, NULL);
	xTaskCreate(&xtaskemergencystop, "Emergency STOP  Task", 8192, NULL, 2, NULL);

}













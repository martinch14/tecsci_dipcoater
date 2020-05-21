/*
 * dipcoater_main.c
 *
 *  Created on: 1 Abril. 2020
 *  Author: Martin Gambarotta
 *
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

#include "TMC4671.h"

#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"


#include "TMCMotionController.h"
#include "TMC5130.h"
#include "SysTick.h"



#include "http_server.h"
#include "dns_server.h"
#include "include/app_main_spincoater.h"
#include "json.h"
#include "wifi_manager.h"


/*TODO    BUGS
 *
 *
 *
 *Crear proceso para ejecutar desde el comando SETCOMMAND..APP
 *Luego lanzarlo desde comando RUN
 *Reportar tiempo y velocidad (buscar registro de velocidad actual)
 *Tunear el pid del motor
 *
 *
 *
 *A DEFINIR:
 *Comando Stop que se agrega al final ,, definir mejor finalizacion del proceso
 *
 *
 * */


//#define PORT CONFIG_EXAMPLE_PORT
#define PORT 3333
//#define CONFIG_EXAMPLE_IPV4
//#define CONFIG_EXAMPLE_IPV4 192.168.100.110


static const char *TAG_TASK_SOCKET = "task_socket";
static const char *TAG_TASK_WIFI_MANAGER = "task_wifi_manager_main";


//extern void TMC5130_init(void);
extern void TMC4671_init();


//PROCESS CUSTOM:
static tinysh_cmd_t commandSETCOMMANDCUSTOMPROGRAMAPP = {NULL,"SETCUSTOMPROGRAMAPP", NULL, NULL, CommandSETCOMMANDCUSTOMPROGRAMAPPHandler, NULL, NULL, NULL};



//RUN LOADED PROCESS:
static tinysh_cmd_t commandRUN = 						{NULL,"RUN", NULL, NULL, CommandRUNHandler, NULL, NULL, NULL};

//EMRGENCY STOP
static tinysh_cmd_t commandSTOP = 						{NULL,"STOP", NULL, NULL, CommandSTOPHandler, NULL, NULL, NULL};




//SINGLE MOVEMENTS:
static tinysh_cmd_t commandRIGHTFAST = 					{NULL,"RIGHTFAST", NULL, NULL, CommandRIGHTFASTHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandRIGHT = 						{NULL,"RIGHT", NULL, NULL, CommandRIGHTHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandRIGHTSLOW = 					{NULL,"RIGHTSLOW", NULL, NULL, CommandRIGHTSLOWHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLEFTFAST = 					{NULL,"LEFTFAST", NULL, NULL, CommandLEFTFASTHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLEFT = 						{NULL,"LEFT", NULL, NULL, CommandLEFTHandler, NULL, NULL, NULL};
static tinysh_cmd_t commandLEFTSLOW = 					{NULL,"LEFTSLOW", NULL, NULL, CommandLEFTSLOWHandler, NULL, NULL, NULL};



//EMRGENCY READDATA
static tinysh_cmd_t commandREADDATA = 					{NULL,"READDATA", NULL, NULL, CommandREADDATAHandler, NULL, NULL, NULL};

/*CONFIGURATION COMMANDS*/



void xtaskprocess(void *pvParameter) {

	ProcessInit(&processSpinCoating);
	processSpinCoating.config.status=0;

	QueueHandle_t



	vTaskDelay(1000 / portTICK_RATE_MS);

	while (1) {

		ProcessCommand();
		if (processSpinCoating.state.flags == RUN ) {
			processSpinCoating.config.status=1;
			ProcessRun(&processSpinCoating);
			processSpinCoating.state.flags = STOP;
			processSpinCoating.config.status=0;
		}
		vTaskDelay(200 / portTICK_RATE_MS);
	}
}




// Task para el procesamiento de todos los comandos que llegan por consola o por tcp port
void xtasktinysh(void *pvParameter) {

	char c = 0;
	tinysh_init();
	//queue initialization
	modQueue_Init(&queueconsolareception, bufferreception, 10, sizeof(processCommand_t));
	modQueue_Init(&queueconsolatransmit,  buffertransmit, 10, sizeof(int));


	tinysh_add_command(&commandSETCOMMANDCUSTOMPROGRAMAPP);

	tinysh_add_command(&commandRUN);
	tinysh_add_command(&commandRIGHTFAST);
	tinysh_add_command(&commandRIGHT);
	tinysh_add_command(&commandRIGHTSLOW);
	tinysh_add_command(&commandLEFTFAST);
	tinysh_add_command(&commandLEFT);
	tinysh_add_command(&commandLEFTSLOW);
	tinysh_add_command(&commandSTOP);
	tinysh_add_command(&commandREADDATA);

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
	TMC4671_init();

	Evalboards.driverEnable = DRIVER_ENABLE;
	Evalboards.ch1.enableDriver(DRIVER_DISABLE);
	vTaskDelay(250 / portTICK_RATE_MS);
	Evalboards.ch1.enableDriver(DRIVER_ENABLE);
	vTaskDelay(1000 / portTICK_RATE_MS);

	//====================================================================================================//
	// ACTUAL SETTINGS FOR TMC4671 (created: 2020/04/03 14:50:55)                                        //
	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv//

		Evalboards.ch1.writeRegister(0,0x01, 	0x00000005); 		// writing value 0x00000005 = 5 = 0.0 to address 0 = 0x01(CHIPINFO_ADDR)
		Evalboards.ch1.writeRegister(0,0x03, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 1 = 0x03(ADC_RAW_ADDR)
		Evalboards.ch1.writeRegister(0,0x04, 	0x00100010); 		// writing value 0x00100010 = 1048592 = 0.0 to address 2 = 0x04(dsADC_MCFG_B_MCFG_A)
		Evalboards.ch1.writeRegister(0,0x05, 	0x20000000); 		// writing value 0x20000000 = 536870912 = 0.0 to address 3 = 0x05(dsADC_MCLK_A)
		Evalboards.ch1.writeRegister(0,0x06, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 4 = 0x06(dsADC_MCLK_B)
		Evalboards.ch1.writeRegister(0,0x07, 	0x014E014E); 		// writing value 0x014E014E = 21889358 = 0.0 to address 5 = 0x07(dsADC_MDEC_B_MDEC_A)
		Evalboards.ch1.writeRegister(0,0x08, 	0x00ED82A2); 		// writing value 0x00ED82A2 = 15565474 = 0.0 to address 6 = 0x08(ADC_I1_SCALE_OFFSET)
		Evalboards.ch1.writeRegister(0,0x09, 	0x0100823F); 		// writing value 0x0100823F = 16810559 = 0.0 to address 7 = 0x09(ADC_I0_SCALE_OFFSET)
		Evalboards.ch1.writeRegister(0,0x0A, 	0x18000100); 		// writing value 0x18000100 = 402653440 = 0.0 to address 8 = 0x0A(ADC_I_SELECT)
		Evalboards.ch1.writeRegister(0,0x0B, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 9 = 0x0B(ADC_I1_I0_EXT)
		Evalboards.ch1.writeRegister(0,0x0C, 	0x00044400); 		// writing value 0x00044400 = 279552 = 0.0 to address 10 = 0x0C(DS_ANALOG_INPUT_STAGE_CFG)
		Evalboards.ch1.writeRegister(0,0x0D, 	0x01000000); 		// writing value 0x01000000 = 16777216 = 0.0 to address 11 = 0x0D(AENC_0_SCALE_OFFSET)
		Evalboards.ch1.writeRegister(0,0x0E, 	0x01000000); 		// writing value 0x01000000 = 16777216 = 0.0 to address 12 = 0x0E(AENC_1_SCALE_OFFSET)
		Evalboards.ch1.writeRegister(0,0x0F, 	0x01000000); 		// writing value 0x01000000 = 16777216 = 0.0 to address 13 = 0x0F(AENC_2_SCALE_OFFSET)
		Evalboards.ch1.writeRegister(0,0x11, 	0x03020100); 		// writing value 0x03020100 = 50462976 = 0.0 to address 14 = 0x11(AENC_SELECT)
		Evalboards.ch1.writeRegister(0,0x17, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 15 = 0x17(PWM_POLARITIES)
		Evalboards.ch1.writeRegister(0,0x18, 	0x00000F9F); 		// writing value 0x00000F9F = 3999 = 0.0 to address 16 = 0x18(PWM_MAXCNT)
		Evalboards.ch1.writeRegister(0,0x19, 	0x00000505); 		// writing value 0x00000505 = 1285 = 0.0 to address 17 = 0x19(PWM_BBM_H_BBM_L)
		Evalboards.ch1.writeRegister(0,0x1A, 	0x00000007); 		// writing value 0x00000007 = 7 = 0.0 to address 18 = 0x1A(PWM_SV_CHOP)
		Evalboards.ch1.writeRegister(0,0x1B, 	0x00030004); 		// writing value 0x00030004 = 196612 = 0.0 to address 19 = 0x1B(MOTOR_TYPE_N_POLE_PAIRS)
		Evalboards.ch1.writeRegister(0,0x1C, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 20 = 0x1C(PHI_E_EXT)
		Evalboards.ch1.writeRegister(0,0x1D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 21 = 0x1D(PHI_M_EXT)
		Evalboards.ch1.writeRegister(0,0x1E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 22 = 0x1E(POSITION_EXT)
		Evalboards.ch1.writeRegister(0,0x1F, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 23 = 0x1F(OPENLOOP_MODE)
		Evalboards.ch1.writeRegister(0,0x20, 	0x0000003C); 		// writing value 0x0000003C = 60 = 0.0 to address 24 = 0x20(OPENLOOP_ACCELERATION)
		Evalboards.ch1.writeRegister(0,0x21, 	0xFFFFFFFB); 		// writing value 0xFFFFFFFB = 0 = 0.0 to address 25 = 0x21(OPENLOOP_VELOCITY_TARGET)
		Evalboards.ch1.writeRegister(0,0x22, 	0xFFFFFFFB); 		// writing value 0xFFFFFFFB = 0 = 0.0 to address 26 = 0x22(OPENLOOP_VELOCITY_ACTUAL)
		Evalboards.ch1.writeRegister(0,0x23, 	0x00003A9C); 		// writing value 0x00003A9C = 15004 = 0.0 to address 27 = 0x23(OPENLOOP_PHI)
		Evalboards.ch1.writeRegister(0,0x24, 	0x00000C8B); 		// writing value 0x00000C8B = 3211 = 0.0 to address 28 = 0x24(UQ_UD_EXT)
		Evalboards.ch1.writeRegister(0,0x25, 	0x00001000); 		// writing value 0x00001000 = 4096 = 0.0 to address 29 = 0x25(ABN_DECODER_MODE)
		Evalboards.ch1.writeRegister(0,0x26, 	0x00004000); 		// writing value 0x00004000 = 16384 = 0.0 to address 30 = 0x26(ABN_DECODER_PPR)
		Evalboards.ch1.writeRegister(0,0x27, 	0x00001583); 		// writing value 0x00001583 = 5507 = 0.0 to address 31 = 0x27(ABN_DECODER_COUNT)
		Evalboards.ch1.writeRegister(0,0x28, 	0x00000211); 		// writing value 0x00000211 = 529 = 0.0 to address 32 = 0x28(ABN_DECODER_COUNT_N)
		Evalboards.ch1.writeRegister(0,0x29, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 33 = 0x29(ABN_DECODER_PHI_E_PHI_M_OFFSET)
		Evalboards.ch1.writeRegister(0,0x2C, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 34 = 0x2C(ABN_2_DECODER_MODE)
		Evalboards.ch1.writeRegister(0,0x2D, 	0x00010000); 		// writing value 0x00010000 = 65536 = 0.0 to address 35 = 0x2D(ABN_2_DECODER_PPR)
		Evalboards.ch1.writeRegister(0,0x2E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 36 = 0x2E(ABN_2_DECODER_COUNT)
		vTaskDelay(250 / portTICK_RATE_MS);
		Evalboards.ch1.writeRegister(0,0x2F, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 37 = 0x2F(ABN_2_DECODER_COUNT_N)
		Evalboards.ch1.writeRegister(0,0x30, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 38 = 0x30(ABN_2_DECODER_PHI_M_OFFSET)
		Evalboards.ch1.writeRegister(0,0x33, 	0x00000001); 		// writing value 0x00000001 = 1 = 0.0 to address 39 = 0x33(HALL_MODE)
		Evalboards.ch1.writeRegister(0,0x34, 	0x2AAA0000); 		// writing value 0x2AAA0000 = 715784192 = 0.0 to address 40 = 0x34(HALL_POSITION_060_000)
		Evalboards.ch1.writeRegister(0,0x35, 	0x80005555); 		// writing value 0x80005555 = 0 = 0.0 to address 41 = 0x35(HALL_POSITION_180_120)
		Evalboards.ch1.writeRegister(0,0x36, 	0xD555AAAA); 		// writing value 0xD555AAAA = 0 = 0.0 to address 42 = 0x36(HALL_POSITION_300_240)
		Evalboards.ch1.writeRegister(0,0x37, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 43 = 0x37(HALL_PHI_E_PHI_M_OFFSET)
		Evalboards.ch1.writeRegister(0,0x38, 	0x00002AAA); 		// writing value 0x00002AAA = 10922 = 0.0 to address 44 = 0x38(HALL_DPHI_MAX)
		Evalboards.ch1.writeRegister(0,0x3B, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 45 = 0x3B(AENC_DECODER_MODE)
		Evalboards.ch1.writeRegister(0,0x3C, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 46 = 0x3C(AENC_DECODER_N_THRESHOLD)
		Evalboards.ch1.writeRegister(0,0x3E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 47 = 0x3E(AENC_DECODER_PHI_A_OFFSET)
		Evalboards.ch1.writeRegister(0,0x40, 	0x00000001); 		// writing value 0x00000001 = 1 = 0.0 to address 48 = 0x40(AENC_DECODER_PPR)
		Evalboards.ch1.writeRegister(0,0x42, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 49 = 0x42(AENC_DECODER_COUNT_N)
		Evalboards.ch1.writeRegister(0,0x45, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 50 = 0x45(AENC_DECODER_PHI_E_PHI_M_OFFSET)
		Evalboards.ch1.writeRegister(0,0x4D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 51 = 0x4D(CONFIG_DATA)
		Evalboards.ch1.writeRegister(0,0x4E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 52 = 0x4E(CONFIG_ADDR)
		Evalboards.ch1.writeRegister(0,0x50, 	0x0000000C); 		// writing value 0x0000000C = 12 = 0.0 to address 53 = 0x50(VELOCITY_SELECTION)
		Evalboards.ch1.writeRegister(0,0x51, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 54 = 0x51(POSITION_SELECTION)
		Evalboards.ch1.writeRegister(0,0x52, 	0x00000003); 		// writing value 0x00000003 = 3 = 0.0 to address 55 = 0x52(PHI_E_SELECTION)
		Evalboards.ch1.writeRegister(0,0x54, 	0x01000100); 		// writing value 0x01000100 = 16777472 = 0.0 to address 56 = 0x54(PID_FLUX_P_FLUX_I)
		Evalboards.ch1.writeRegister(0,0x56, 	0x01000100); 		// writing value 0x01000100 = 16777472 = 0.0 to address 57 = 0x56(PID_TORQUE_P_TORQUE_I)
		Evalboards.ch1.writeRegister(0,0x58, 	0x01000100); 		// writing value 0x01000100 = 16777472 = 0.0 to address 58 = 0x58(PID_VELOCITY_P_VELOCITY_I)
		Evalboards.ch1.writeRegister(0,0x5A, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 59 = 0x5A(PID_POSITION_P_POSITION_I)
		Evalboards.ch1.writeRegister(0,0x5C, 	0x00007FFF); 		// writing value 0x00007FFF = 32767 = 0.0 to address 60 = 0x5C(PID_TORQUE_FLUX_TARGET_DDT_LIMITS)
		Evalboards.ch1.writeRegister(0,0x5D, 	0x00007FFF); 		// writing value 0x00007FFF = 32767 = 0.0 to address 61 = 0x5D(PIDOUT_UQ_UD_LIMITS)
		Evalboards.ch1.writeRegister(0,0x5E, 	0x00000E74); 		// writing value 0x00000E74 = 3700 = 0.0 to address 62 = 0x5E(PID_TORQUE_FLUX_LIMITS)
		Evalboards.ch1.writeRegister(0,0x5F, 	0x000003E8); 		// writing value 0x000003E8 = 1000 = 0.0 to address 63 = 0x5F(PID_ACCELERATION_LIMIT)
		Evalboards.ch1.writeRegister(0,0x60, 	0x00001770); 		// writing value 0x00001770 = 6000 = 0.0 to address 64 = 0x60(PID_VELOCITY_LIMIT)
		Evalboards.ch1.writeRegister(0,0x61, 	0x80000001); 		// writing value 0x80000001 = 0 = 0.0 to address 65 = 0x61(PID_POSITION_LIMIT_LOW)
		Evalboards.ch1.writeRegister(0,0x62, 	0x7FFFFFFF); 		// writing value 0x7FFFFFFF = 2147483647 = 0.0 to address 66 = 0x62(PID_POSITION_LIMIT_HIGH)
		Evalboards.ch1.writeRegister(0,0x63, 	0x00000002); 		// writing value 0x00000002 = 2 = 0.0 to address 67 = 0x63(MODE_RAMP_MODE_MOTION)
		Evalboards.ch1.writeRegister(0,0x64, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 68 = 0x64(PID_TORQUE_FLUX_TARGET)
		Evalboards.ch1.writeRegister(0,0x65, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 69 = 0x65(PID_TORQUE_FLUX_OFFSET)
		Evalboards.ch1.writeRegister(0,0x66, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 70 = 0x66(PID_VELOCITY_TARGET)
		Evalboards.ch1.writeRegister(0,0x67, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 71 = 0x67(PID_VELOCITY_OFFSET)
		Evalboards.ch1.writeRegister(0,0x68, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 72 = 0x68(PID_POSITION_TARGET)
		Evalboards.ch1.writeRegister(0,0x6B, 	0x93855830); 		// writing value 0x93855830 = 0 = 0.0 to address 73 = 0x6B(PID_POSITION_ACTUAL)
		Evalboards.ch1.writeRegister(0,0x6D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 74 = 0x6D(PID_ERROR_ADDR)
		Evalboards.ch1.writeRegister(0,0x6E, 	0x830081EE); 		// writing value 0x830081EE = 0 = 0.0 to address 75 = 0x6E(INTERIM_DATA)
		Evalboards.ch1.writeRegister(0,0x6F, 	0x00000013); 		// writing value 0x00000013 = 19 = 0.0 to address 76 = 0x6F(INTERIM_ADDR)
		Evalboards.ch1.writeRegister(0,0x74, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 77 = 0x74(WATCHDOG_CFG)
		Evalboards.ch1.writeRegister(0,0x75, 	0xFFFFFFFF); 		// writing value 0xFFFFFFFF = 0 = 0.0 to address 78 = 0x75(ADC_VM_LIMITS)
		Evalboards.ch1.writeRegister(0,0x78, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 79 = 0x78(STEP_WIDTH)
		Evalboards.ch1.writeRegister(0,0x79, 	0x00009600); 		// writing value 0x00009600 = 38400 = 0.0 to address 80 = 0x79(UART_BPS)
		Evalboards.ch1.writeRegister(0,0x7A, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 81 = 0x7A(UART_ADDRS)
		Evalboards.ch1.writeRegister(0,0x7B, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 82 = 0x7B(GPIO_dsADCI_CONFIG)
		Evalboards.ch1.writeRegister(0,0x7C, 	0xC77C8080); 		// writing value 0xC77C8080 = 0 = 0.0 to address 83 = 0x7C(STATUS_FLAGS)
		Evalboards.ch1.writeRegister(0,0x7D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 84 = 0x7D(STATUS_MASK)

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//


		//CON MOTOR ENCENDIDO
//
//
//
//		TMC4671_SPIWriteInt(0x01, 	0x00000005); 		// writing value 0x00000005 = 5 = 0.0 to address 0 = 0x01(CHIPINFO_ADDR)
//		TMC4671_SPIWriteInt(0x03, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 1 = 0x03(ADC_RAW_ADDR)
//		TMC4671_SPIWriteInt(0x04, 	0x00100010); 		// writing value 0x00100010 = 1048592 = 0.0 to address 2 = 0x04(dsADC_MCFG_B_MCFG_A)
//		TMC4671_SPIWriteInt(0x05, 	0x20000000); 		// writing value 0x20000000 = 536870912 = 0.0 to address 3 = 0x05(dsADC_MCLK_A)
//		TMC4671_SPIWriteInt(0x06, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 4 = 0x06(dsADC_MCLK_B)
//		TMC4671_SPIWriteInt(0x07, 	0x014E014E); 		// writing value 0x014E014E = 21889358 = 0.0 to address 5 = 0x07(dsADC_MDEC_B_MDEC_A)
//		TMC4671_SPIWriteInt(0x08, 	0x00ED82A2); 		// writing value 0x00ED82A2 = 15565474 = 0.0 to address 6 = 0x08(ADC_I1_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x09, 	0x0100823F); 		// writing value 0x0100823F = 16810559 = 0.0 to address 7 = 0x09(ADC_I0_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x0A, 	0x18000100); 		// writing value 0x18000100 = 402653440 = 0.0 to address 8 = 0x0A(ADC_I_SELECT)
//		TMC4671_SPIWriteInt(0x0B, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 9 = 0x0B(ADC_I1_I0_EXT)
//		TMC4671_SPIWriteInt(0x0C, 	0x00044400); 		// writing value 0x00044400 = 279552 = 0.0 to address 10 = 0x0C(DS_ANALOG_INPUT_STAGE_CFG)
//		TMC4671_SPIWriteInt(0x0D, 	0x01000000); 		// writing value 0x01000000 = 16777216 = 0.0 to address 11 = 0x0D(AENC_0_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x0E, 	0x01000000); 		// writing value 0x01000000 = 16777216 = 0.0 to address 12 = 0x0E(AENC_1_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x0F, 	0x01000000); 		// writing value 0x01000000 = 16777216 = 0.0 to address 13 = 0x0F(AENC_2_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x11, 	0x03020100); 		// writing value 0x03020100 = 50462976 = 0.0 to address 14 = 0x11(AENC_SELECT)
//		TMC4671_SPIWriteInt(0x17, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 15 = 0x17(PWM_POLARITIES)
//		TMC4671_SPIWriteInt(0x18, 	0x00000F9F); 		// writing value 0x00000F9F = 3999 = 0.0 to address 16 = 0x18(PWM_MAXCNT)
//		TMC4671_SPIWriteInt(0x19, 	0x00000505); 		// writing value 0x00000505 = 1285 = 0.0 to address 17 = 0x19(PWM_BBM_H_BBM_L)
//		TMC4671_SPIWriteInt(0x1A, 	0x00000007); 		// writing value 0x00000007 = 7 = 0.0 to address 18 = 0x1A(PWM_SV_CHOP)
//		TMC4671_SPIWriteInt(0x1B, 	0x00030004); 		// writing value 0x00030004 = 196612 = 0.0 to address 19 = 0x1B(MOTOR_TYPE_N_POLE_PAIRS)
//		TMC4671_SPIWriteInt(0x1C, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 20 = 0x1C(PHI_E_EXT)
//		TMC4671_SPIWriteInt(0x1D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 21 = 0x1D(PHI_M_EXT)
//		TMC4671_SPIWriteInt(0x1E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 22 = 0x1E(POSITION_EXT)
//		TMC4671_SPIWriteInt(0x1F, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 23 = 0x1F(OPENLOOP_MODE)
//		TMC4671_SPIWriteInt(0x20, 	0x0000003C); 		// writing value 0x0000003C = 60 = 0.0 to address 24 = 0x20(OPENLOOP_ACCELERATION)
//		TMC4671_SPIWriteInt(0x21, 	0xFFFFFFFB); 		// writing value 0xFFFFFFFB = 0 = 0.0 to address 25 = 0x21(OPENLOOP_VELOCITY_TARGET)
//		TMC4671_SPIWriteInt(0x22, 	0xFFFFFFFB); 		// writing value 0xFFFFFFFB = 0 = 0.0 to address 26 = 0x22(OPENLOOP_VELOCITY_ACTUAL)
//		TMC4671_SPIWriteInt(0x23, 	0x0000D2B4); 		// writing value 0x0000D2B4 = 53940 = 0.0 to address 27 = 0x23(OPENLOOP_PHI)
//		TMC4671_SPIWriteInt(0x24, 	0x00000C8B); 		// writing value 0x00000C8B = 3211 = 0.0 to address 28 = 0x24(UQ_UD_EXT)
//		TMC4671_SPIWriteInt(0x25, 	0x00001000); 		// writing value 0x00001000 = 4096 = 0.0 to address 29 = 0x25(ABN_DECODER_MODE)
//		TMC4671_SPIWriteInt(0x26, 	0x00004000); 		// writing value 0x00004000 = 16384 = 0.0 to address 30 = 0x26(ABN_DECODER_PPR)
//		TMC4671_SPIWriteInt(0x27, 	0x0000384C); 		// writing value 0x0000384C = 14412 = 0.0 to address 31 = 0x27(ABN_DECODER_COUNT)
//		TMC4671_SPIWriteInt(0x28, 	0x00000211); 		// writing value 0x00000211 = 529 = 0.0 to address 32 = 0x28(ABN_DECODER_COUNT_N)
//		TMC4671_SPIWriteInt(0x29, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 33 = 0x29(ABN_DECODER_PHI_E_PHI_M_OFFSET)
//		TMC4671_SPIWriteInt(0x2C, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 34 = 0x2C(ABN_2_DECODER_MODE)
//		TMC4671_SPIWriteInt(0x2D, 	0x00010000); 		// writing value 0x00010000 = 65536 = 0.0 to address 35 = 0x2D(ABN_2_DECODER_PPR)
//		TMC4671_SPIWriteInt(0x2E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 36 = 0x2E(ABN_2_DECODER_COUNT)
//		TMC4671_SPIWriteInt(0x2F, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 37 = 0x2F(ABN_2_DECODER_COUNT_N)
//		TMC4671_SPIWriteInt(0x30, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 38 = 0x30(ABN_2_DECODER_PHI_M_OFFSET)
//		TMC4671_SPIWriteInt(0x33, 	0x00000001); 		// writing value 0x00000001 = 1 = 0.0 to address 39 = 0x33(HALL_MODE)
//		TMC4671_SPIWriteInt(0x34, 	0x2AAA0000); 		// writing value 0x2AAA0000 = 715784192 = 0.0 to address 40 = 0x34(HALL_POSITION_060_000)
//		TMC4671_SPIWriteInt(0x35, 	0x80005555); 		// writing value 0x80005555 = 0 = 0.0 to address 41 = 0x35(HALL_POSITION_180_120)
//		TMC4671_SPIWriteInt(0x36, 	0xD555AAAA); 		// writing value 0xD555AAAA = 0 = 0.0 to address 42 = 0x36(HALL_POSITION_300_240)
//		TMC4671_SPIWriteInt(0x37, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 43 = 0x37(HALL_PHI_E_PHI_M_OFFSET)
//		TMC4671_SPIWriteInt(0x38, 	0x00002AAA); 		// writing value 0x00002AAA = 10922 = 0.0 to address 44 = 0x38(HALL_DPHI_MAX)
//		TMC4671_SPIWriteInt(0x3B, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 45 = 0x3B(AENC_DECODER_MODE)
//		TMC4671_SPIWriteInt(0x3C, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 46 = 0x3C(AENC_DECODER_N_THRESHOLD)
//		TMC4671_SPIWriteInt(0x3E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 47 = 0x3E(AENC_DECODER_PHI_A_OFFSET)
//		TMC4671_SPIWriteInt(0x40, 	0x00000001); 		// writing value 0x00000001 = 1 = 0.0 to address 48 = 0x40(AENC_DECODER_PPR)
//		TMC4671_SPIWriteInt(0x42, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 49 = 0x42(AENC_DECODER_COUNT_N)
//		TMC4671_SPIWriteInt(0x45, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 50 = 0x45(AENC_DECODER_PHI_E_PHI_M_OFFSET)
//		TMC4671_SPIWriteInt(0x4D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 51 = 0x4D(CONFIG_DATA)
//		TMC4671_SPIWriteInt(0x4E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 52 = 0x4E(CONFIG_ADDR)
//		TMC4671_SPIWriteInt(0x50, 	0x0000000C); 		// writing value 0x0000000C = 12 = 0.0 to address 53 = 0x50(VELOCITY_SELECTION)
//		TMC4671_SPIWriteInt(0x51, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 54 = 0x51(POSITION_SELECTION)
//		TMC4671_SPIWriteInt(0x52, 	0x00000003); 		// writing value 0x00000003 = 3 = 0.0 to address 55 = 0x52(PHI_E_SELECTION)
//		TMC4671_SPIWriteInt(0x54, 	0x01000100); 		// writing value 0x01000100 = 16777472 = 0.0 to address 56 = 0x54(PID_FLUX_P_FLUX_I)
//		TMC4671_SPIWriteInt(0x56, 	0x01000100); 		// writing value 0x01000100 = 16777472 = 0.0 to address 57 = 0x56(PID_TORQUE_P_TORQUE_I)
//		TMC4671_SPIWriteInt(0x58, 	0x01000100); 		// writing value 0x01000100 = 16777472 = 0.0 to address 58 = 0x58(PID_VELOCITY_P_VELOCITY_I)
//		TMC4671_SPIWriteInt(0x5A, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 59 = 0x5A(PID_POSITION_P_POSITION_I)
//		TMC4671_SPIWriteInt(0x5C, 	0x00007FFF); 		// writing value 0x00007FFF = 32767 = 0.0 to address 60 = 0x5C(PID_TORQUE_FLUX_TARGET_DDT_LIMITS)
//		TMC4671_SPIWriteInt(0x5D, 	0x00007FFF); 		// writing value 0x00007FFF = 32767 = 0.0 to address 61 = 0x5D(PIDOUT_UQ_UD_LIMITS)
//		TMC4671_SPIWriteInt(0x5E, 	0x00000E74); 		// writing value 0x00000E74 = 3700 = 0.0 to address 62 = 0x5E(PID_TORQUE_FLUX_LIMITS)
//		TMC4671_SPIWriteInt(0x5F, 	0x000003E8); 		// writing value 0x000003E8 = 1000 = 0.0 to address 63 = 0x5F(PID_ACCELERATION_LIMIT)
//		TMC4671_SPIWriteInt(0x60, 	0x00001770); 		// writing value 0x00001770 = 6000 = 0.0 to address 64 = 0x60(PID_VELOCITY_LIMIT)
//		TMC4671_SPIWriteInt(0x61, 	0x80000001); 		// writing value 0x80000001 = 0 = 0.0 to address 65 = 0x61(PID_POSITION_LIMIT_LOW)
//		TMC4671_SPIWriteInt(0x62, 	0x7FFFFFFF); 		// writing value 0x7FFFFFFF = 2147483647 = 0.0 to address 66 = 0x62(PID_POSITION_LIMIT_HIGH)
//		TMC4671_SPIWriteInt(0x63, 	0x00000002); 		// writing value 0x00000002 = 2 = 0.0 to address 67 = 0x63(MODE_RAMP_MODE_MOTION)
//		TMC4671_SPIWriteInt(0x64, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 68 = 0x64(PID_TORQUE_FLUX_TARGET)
//		TMC4671_SPIWriteInt(0x65, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 69 = 0x65(PID_TORQUE_FLUX_OFFSET)
//		TMC4671_SPIWriteInt(0x66, 	0x00000AF0); 		// writing value 0x00000AF0 = 2800 = 0.0 to address 70 = 0x66(PID_VELOCITY_TARGET)
//		TMC4671_SPIWriteInt(0x67, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 71 = 0x67(PID_VELOCITY_OFFSET)
//		TMC4671_SPIWriteInt(0x68, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 72 = 0x68(PID_POSITION_TARGET)
//		TMC4671_SPIWriteInt(0x6B, 	0xCA148110); 		// writing value 0xCA148110 = 0 = 0.0 to address 73 = 0x6B(PID_POSITION_ACTUAL)
//		TMC4671_SPIWriteInt(0x6D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 74 = 0x6D(PID_ERROR_ADDR)
//		TMC4671_SPIWriteInt(0x6E, 	0x83D581DD); 		// writing value 0x83D581DD = 0 = 0.0 to address 75 = 0x6E(INTERIM_DATA)
//		TMC4671_SPIWriteInt(0x6F, 	0x00000013); 		// writing value 0x00000013 = 19 = 0.0 to address 76 = 0x6F(INTERIM_ADDR)
//		TMC4671_SPIWriteInt(0x74, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 77 = 0x74(WATCHDOG_CFG)
//		TMC4671_SPIWriteInt(0x75, 	0xFFFFFFFF); 		// writing value 0xFFFFFFFF = 0 = 0.0 to address 78 = 0x75(ADC_VM_LIMITS)
//		TMC4671_SPIWriteInt(0x78, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 79 = 0x78(STEP_WIDTH)
//		TMC4671_SPIWriteInt(0x79, 	0x00009600); 		// writing value 0x00009600 = 38400 = 0.0 to address 80 = 0x79(UART_BPS)
//		TMC4671_SPIWriteInt(0x7A, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 81 = 0x7A(UART_ADDRS)
//		TMC4671_SPIWriteInt(0x7B, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 82 = 0x7B(GPIO_dsADCI_CONFIG)
//		TMC4671_SPIWriteInt(0x7C, 	0xC77C8080); 		// writing value 0xC77C8080 = 0 = 0.0 to address 83 = 0x7C(STATUS_FLAGS)
//		TMC4671_SPIWriteInt(0x7D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 84 = 0x7D(STATUS_MASK)





		// CON MOTOR ENCENDIDO Y VELOCITY RAMP APAGADO


//		TMC4671_SPIWriteInt(0x01, 	0x00000005); 		// writing value 0x00000005 = 5 = 0.0 to address 0 = 0x01(CHIPINFO_ADDR)
//		TMC4671_SPIWriteInt(0x03, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 1 = 0x03(ADC_RAW_ADDR)
//		TMC4671_SPIWriteInt(0x04, 	0x00100010); 		// writing value 0x00100010 = 1048592 = 0.0 to address 2 = 0x04(dsADC_MCFG_B_MCFG_A)
//		TMC4671_SPIWriteInt(0x05, 	0x20000000); 		// writing value 0x20000000 = 536870912 = 0.0 to address 3 = 0x05(dsADC_MCLK_A)
//		TMC4671_SPIWriteInt(0x06, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 4 = 0x06(dsADC_MCLK_B)
//		TMC4671_SPIWriteInt(0x07, 	0x014E014E); 		// writing value 0x014E014E = 21889358 = 0.0 to address 5 = 0x07(dsADC_MDEC_B_MDEC_A)
//		TMC4671_SPIWriteInt(0x08, 	0x00ED82A2); 		// writing value 0x00ED82A2 = 15565474 = 0.0 to address 6 = 0x08(ADC_I1_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x09, 	0x0100823F); 		// writing value 0x0100823F = 16810559 = 0.0 to address 7 = 0x09(ADC_I0_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x0A, 	0x18000100); 		// writing value 0x18000100 = 402653440 = 0.0 to address 8 = 0x0A(ADC_I_SELECT)
//		TMC4671_SPIWriteInt(0x0B, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 9 = 0x0B(ADC_I1_I0_EXT)
//		TMC4671_SPIWriteInt(0x0C, 	0x00044400); 		// writing value 0x00044400 = 279552 = 0.0 to address 10 = 0x0C(DS_ANALOG_INPUT_STAGE_CFG)
//		TMC4671_SPIWriteInt(0x0D, 	0x01000000); 		// writing value 0x01000000 = 16777216 = 0.0 to address 11 = 0x0D(AENC_0_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x0E, 	0x01000000); 		// writing value 0x01000000 = 16777216 = 0.0 to address 12 = 0x0E(AENC_1_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x0F, 	0x01000000); 		// writing value 0x01000000 = 16777216 = 0.0 to address 13 = 0x0F(AENC_2_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x11, 	0x03020100); 		// writing value 0x03020100 = 50462976 = 0.0 to address 14 = 0x11(AENC_SELECT)
//		TMC4671_SPIWriteInt(0x17, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 15 = 0x17(PWM_POLARITIES)
//		TMC4671_SPIWriteInt(0x18, 	0x00000F9F); 		// writing value 0x00000F9F = 3999 = 0.0 to address 16 = 0x18(PWM_MAXCNT)
//		TMC4671_SPIWriteInt(0x19, 	0x00000505); 		// writing value 0x00000505 = 1285 = 0.0 to address 17 = 0x19(PWM_BBM_H_BBM_L)
//		TMC4671_SPIWriteInt(0x1A, 	0x00000007); 		// writing value 0x00000007 = 7 = 0.0 to address 18 = 0x1A(PWM_SV_CHOP)
//		TMC4671_SPIWriteInt(0x1B, 	0x00030004); 		// writing value 0x00030004 = 196612 = 0.0 to address 19 = 0x1B(MOTOR_TYPE_N_POLE_PAIRS)
//		TMC4671_SPIWriteInt(0x1C, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 20 = 0x1C(PHI_E_EXT)
//		TMC4671_SPIWriteInt(0x1D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 21 = 0x1D(PHI_M_EXT)
//		TMC4671_SPIWriteInt(0x1E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 22 = 0x1E(POSITION_EXT)
//		TMC4671_SPIWriteInt(0x1F, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 23 = 0x1F(OPENLOOP_MODE)
//		TMC4671_SPIWriteInt(0x20, 	0x0000003C); 		// writing value 0x0000003C = 60 = 0.0 to address 24 = 0x20(OPENLOOP_ACCELERATION)
//		TMC4671_SPIWriteInt(0x21, 	0xFFFFFFFB); 		// writing value 0xFFFFFFFB = 0 = 0.0 to address 25 = 0x21(OPENLOOP_VELOCITY_TARGET)
//		TMC4671_SPIWriteInt(0x22, 	0xFFFFFFFB); 		// writing value 0xFFFFFFFB = 0 = 0.0 to address 26 = 0x22(OPENLOOP_VELOCITY_ACTUAL)
//		TMC4671_SPIWriteInt(0x23, 	0x0000118C); 		// writing value 0x0000118C = 4492 = 0.0 to address 27 = 0x23(OPENLOOP_PHI)
//		TMC4671_SPIWriteInt(0x24, 	0x00000C8B); 		// writing value 0x00000C8B = 3211 = 0.0 to address 28 = 0x24(UQ_UD_EXT)
//		TMC4671_SPIWriteInt(0x25, 	0x00001000); 		// writing value 0x00001000 = 4096 = 0.0 to address 29 = 0x25(ABN_DECODER_MODE)
//		TMC4671_SPIWriteInt(0x26, 	0x00004000); 		// writing value 0x00004000 = 16384 = 0.0 to address 30 = 0x26(ABN_DECODER_PPR)
//		TMC4671_SPIWriteInt(0x27, 	0x000006C2); 		// writing value 0x000006C2 = 1730 = 0.0 to address 31 = 0x27(ABN_DECODER_COUNT)
//		TMC4671_SPIWriteInt(0x28, 	0x00000211); 		// writing value 0x00000211 = 529 = 0.0 to address 32 = 0x28(ABN_DECODER_COUNT_N)
//		TMC4671_SPIWriteInt(0x29, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 33 = 0x29(ABN_DECODER_PHI_E_PHI_M_OFFSET)
//		TMC4671_SPIWriteInt(0x2C, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 34 = 0x2C(ABN_2_DECODER_MODE)
//		TMC4671_SPIWriteInt(0x2D, 	0x00010000); 		// writing value 0x00010000 = 65536 = 0.0 to address 35 = 0x2D(ABN_2_DECODER_PPR)
//		TMC4671_SPIWriteInt(0x2E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 36 = 0x2E(ABN_2_DECODER_COUNT)
//		TMC4671_SPIWriteInt(0x2F, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 37 = 0x2F(ABN_2_DECODER_COUNT_N)
//		TMC4671_SPIWriteInt(0x30, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 38 = 0x30(ABN_2_DECODER_PHI_M_OFFSET)
//		TMC4671_SPIWriteInt(0x33, 	0x00000001); 		// writing value 0x00000001 = 1 = 0.0 to address 39 = 0x33(HALL_MODE)
//		TMC4671_SPIWriteInt(0x34, 	0x2AAA0000); 		// writing value 0x2AAA0000 = 715784192 = 0.0 to address 40 = 0x34(HALL_POSITION_060_000)
//		TMC4671_SPIWriteInt(0x35, 	0x80005555); 		// writing value 0x80005555 = 0 = 0.0 to address 41 = 0x35(HALL_POSITION_180_120)
//		TMC4671_SPIWriteInt(0x36, 	0xD555AAAA); 		// writing value 0xD555AAAA = 0 = 0.0 to address 42 = 0x36(HALL_POSITION_300_240)
//		TMC4671_SPIWriteInt(0x37, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 43 = 0x37(HALL_PHI_E_PHI_M_OFFSET)
//		TMC4671_SPIWriteInt(0x38, 	0x00002AAA); 		// writing value 0x00002AAA = 10922 = 0.0 to address 44 = 0x38(HALL_DPHI_MAX)
//		TMC4671_SPIWriteInt(0x3B, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 45 = 0x3B(AENC_DECODER_MODE)
//		TMC4671_SPIWriteInt(0x3C, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 46 = 0x3C(AENC_DECODER_N_THRESHOLD)
//		TMC4671_SPIWriteInt(0x3E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 47 = 0x3E(AENC_DECODER_PHI_A_OFFSET)
//		TMC4671_SPIWriteInt(0x40, 	0x00000001); 		// writing value 0x00000001 = 1 = 0.0 to address 48 = 0x40(AENC_DECODER_PPR)
//		TMC4671_SPIWriteInt(0x42, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 49 = 0x42(AENC_DECODER_COUNT_N)
//		TMC4671_SPIWriteInt(0x45, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 50 = 0x45(AENC_DECODER_PHI_E_PHI_M_OFFSET)
//		TMC4671_SPIWriteInt(0x4D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 51 = 0x4D(CONFIG_DATA)
//		TMC4671_SPIWriteInt(0x4E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 52 = 0x4E(CONFIG_ADDR)
//		TMC4671_SPIWriteInt(0x50, 	0x0000000C); 		// writing value 0x0000000C = 12 = 0.0 to address 53 = 0x50(VELOCITY_SELECTION)
//		TMC4671_SPIWriteInt(0x51, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 54 = 0x51(POSITION_SELECTION)
//		TMC4671_SPIWriteInt(0x52, 	0x00000003); 		// writing value 0x00000003 = 3 = 0.0 to address 55 = 0x52(PHI_E_SELECTION)
//		TMC4671_SPIWriteInt(0x54, 	0x01000100); 		// writing value 0x01000100 = 16777472 = 0.0 to address 56 = 0x54(PID_FLUX_P_FLUX_I)
//		TMC4671_SPIWriteInt(0x56, 	0x01000100); 		// writing value 0x01000100 = 16777472 = 0.0 to address 57 = 0x56(PID_TORQUE_P_TORQUE_I)
//		TMC4671_SPIWriteInt(0x58, 	0x01000100); 		// writing value 0x01000100 = 16777472 = 0.0 to address 58 = 0x58(PID_VELOCITY_P_VELOCITY_I)
//		TMC4671_SPIWriteInt(0x5A, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 59 = 0x5A(PID_POSITION_P_POSITION_I)
//		TMC4671_SPIWriteInt(0x5C, 	0x00007FFF); 		// writing value 0x00007FFF = 32767 = 0.0 to address 60 = 0x5C(PID_TORQUE_FLUX_TARGET_DDT_LIMITS)
//		TMC4671_SPIWriteInt(0x5D, 	0x00007FFF); 		// writing value 0x00007FFF = 32767 = 0.0 to address 61 = 0x5D(PIDOUT_UQ_UD_LIMITS)
//		TMC4671_SPIWriteInt(0x5E, 	0x00000E74); 		// writing value 0x00000E74 = 3700 = 0.0 to address 62 = 0x5E(PID_TORQUE_FLUX_LIMITS)
//		TMC4671_SPIWriteInt(0x5F, 	0x000003E8); 		// writing value 0x000003E8 = 1000 = 0.0 to address 63 = 0x5F(PID_ACCELERATION_LIMIT)
//		TMC4671_SPIWriteInt(0x60, 	0x00001770); 		// writing value 0x00001770 = 6000 = 0.0 to address 64 = 0x60(PID_VELOCITY_LIMIT)
//		TMC4671_SPIWriteInt(0x61, 	0x80000001); 		// writing value 0x80000001 = 0 = 0.0 to address 65 = 0x61(PID_POSITION_LIMIT_LOW)
//		TMC4671_SPIWriteInt(0x62, 	0x7FFFFFFF); 		// writing value 0x7FFFFFFF = 2147483647 = 0.0 to address 66 = 0x62(PID_POSITION_LIMIT_HIGH)
//		TMC4671_SPIWriteInt(0x63, 	0x00000002); 		// writing value 0x00000002 = 2 = 0.0 to address 67 = 0x63(MODE_RAMP_MODE_MOTION)
//		TMC4671_SPIWriteInt(0x64, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 68 = 0x64(PID_TORQUE_FLUX_TARGET)
//		TMC4671_SPIWriteInt(0x65, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 69 = 0x65(PID_TORQUE_FLUX_OFFSET)
//		TMC4671_SPIWriteInt(0x66, 	0x00000AF0); 		// writing value 0x00000AF0 = 2800 = 0.0 to address 70 = 0x66(PID_VELOCITY_TARGET)
//		TMC4671_SPIWriteInt(0x67, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 71 = 0x67(PID_VELOCITY_OFFSET)
//		TMC4671_SPIWriteInt(0x68, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 72 = 0x68(PID_POSITION_TARGET)
//		TMC4671_SPIWriteInt(0x6B, 	0x16C909D0); 		// writing value 0x16C909D0 = 382274000 = 0.0 to address 73 = 0x6B(PID_POSITION_ACTUAL)
//		TMC4671_SPIWriteInt(0x6D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 74 = 0x6D(PID_ERROR_ADDR)
//		TMC4671_SPIWriteInt(0x6E, 	0x82F982A2); 		// writing value 0x82F982A2 = 0 = 0.0 to address 75 = 0x6E(INTERIM_DATA)
//		TMC4671_SPIWriteInt(0x6F, 	0x00000013); 		// writing value 0x00000013 = 19 = 0.0 to address 76 = 0x6F(INTERIM_ADDR)
//		TMC4671_SPIWriteInt(0x74, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 77 = 0x74(WATCHDOG_CFG)
//		TMC4671_SPIWriteInt(0x75, 	0xFFFFFFFF); 		// writing value 0xFFFFFFFF = 0 = 0.0 to address 78 = 0x75(ADC_VM_LIMITS)
//		TMC4671_SPIWriteInt(0x78, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 79 = 0x78(STEP_WIDTH)
//		TMC4671_SPIWriteInt(0x79, 	0x00009600); 		// writing value 0x00009600 = 38400 = 0.0 to address 80 = 0x79(UART_BPS)
//		TMC4671_SPIWriteInt(0x7A, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 81 = 0x7A(UART_ADDRS)
//		TMC4671_SPIWriteInt(0x7B, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 82 = 0x7B(GPIO_dsADCI_CONFIG)
//		TMC4671_SPIWriteInt(0x7C, 	0xC77C8080); 		// writing value 0xC77C8080 = 0 = 0.0 to address 83 = 0x7C(STATUS_FLAGS)
//		TMC4671_SPIWriteInt(0x7D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 84 = 0x7D(STATUS_MASK)
//
//





		// con 2800 RPM pero para el otro lado , rampa configurada en 1000
//
//		TMC4671_SPIWriteInt(0x01, 	0x00000005); 		// writing value 0x00000005 = 5 = 0.0 to address 0 = 0x01(CHIPINFO_ADDR)
//		TMC4671_SPIWriteInt(0x03, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 1 = 0x03(ADC_RAW_ADDR)
//		TMC4671_SPIWriteInt(0x04, 	0x00100010); 		// writing value 0x00100010 = 1048592 = 0.0 to address 2 = 0x04(dsADC_MCFG_B_MCFG_A)
//		TMC4671_SPIWriteInt(0x05, 	0x20000000); 		// writing value 0x20000000 = 536870912 = 0.0 to address 3 = 0x05(dsADC_MCLK_A)
//		TMC4671_SPIWriteInt(0x06, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 4 = 0x06(dsADC_MCLK_B)
//		TMC4671_SPIWriteInt(0x07, 	0x014E014E); 		// writing value 0x014E014E = 21889358 = 0.0 to address 5 = 0x07(dsADC_MDEC_B_MDEC_A)
//		TMC4671_SPIWriteInt(0x08, 	0x00ED82A2); 		// writing value 0x00ED82A2 = 15565474 = 0.0 to address 6 = 0x08(ADC_I1_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x09, 	0x0100823F); 		// writing value 0x0100823F = 16810559 = 0.0 to address 7 = 0x09(ADC_I0_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x0A, 	0x18000100); 		// writing value 0x18000100 = 402653440 = 0.0 to address 8 = 0x0A(ADC_I_SELECT)
//		TMC4671_SPIWriteInt(0x0B, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 9 = 0x0B(ADC_I1_I0_EXT)
//		TMC4671_SPIWriteInt(0x0C, 	0x00044400); 		// writing value 0x00044400 = 279552 = 0.0 to address 10 = 0x0C(DS_ANALOG_INPUT_STAGE_CFG)
//		TMC4671_SPIWriteInt(0x0D, 	0x01000000); 		// writing value 0x01000000 = 16777216 = 0.0 to address 11 = 0x0D(AENC_0_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x0E, 	0x01000000); 		// writing value 0x01000000 = 16777216 = 0.0 to address 12 = 0x0E(AENC_1_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x0F, 	0x01000000); 		// writing value 0x01000000 = 16777216 = 0.0 to address 13 = 0x0F(AENC_2_SCALE_OFFSET)
//		TMC4671_SPIWriteInt(0x11, 	0x03020100); 		// writing value 0x03020100 = 50462976 = 0.0 to address 14 = 0x11(AENC_SELECT)
//		TMC4671_SPIWriteInt(0x17, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 15 = 0x17(PWM_POLARITIES)
//		TMC4671_SPIWriteInt(0x18, 	0x00000F9F); 		// writing value 0x00000F9F = 3999 = 0.0 to address 16 = 0x18(PWM_MAXCNT)
//		TMC4671_SPIWriteInt(0x19, 	0x00000505); 		// writing value 0x00000505 = 1285 = 0.0 to address 17 = 0x19(PWM_BBM_H_BBM_L)
//		TMC4671_SPIWriteInt(0x1A, 	0x00000007); 		// writing value 0x00000007 = 7 = 0.0 to address 18 = 0x1A(PWM_SV_CHOP)
//		TMC4671_SPIWriteInt(0x1B, 	0x00030004); 		// writing value 0x00030004 = 196612 = 0.0 to address 19 = 0x1B(MOTOR_TYPE_N_POLE_PAIRS)
//		TMC4671_SPIWriteInt(0x1C, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 20 = 0x1C(PHI_E_EXT)
//		TMC4671_SPIWriteInt(0x1D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 21 = 0x1D(PHI_M_EXT)
//		TMC4671_SPIWriteInt(0x1E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 22 = 0x1E(POSITION_EXT)
//		TMC4671_SPIWriteInt(0x1F, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 23 = 0x1F(OPENLOOP_MODE)
//		TMC4671_SPIWriteInt(0x20, 	0x0000003C); 		// writing value 0x0000003C = 60 = 0.0 to address 24 = 0x20(OPENLOOP_ACCELERATION)
//		TMC4671_SPIWriteInt(0x21, 	0xFFFFFFFB); 		// writing value 0xFFFFFFFB = 0 = 0.0 to address 25 = 0x21(OPENLOOP_VELOCITY_TARGET)
//		TMC4671_SPIWriteInt(0x22, 	0xFFFFFFFB); 		// writing value 0xFFFFFFFB = 0 = 0.0 to address 26 = 0x22(OPENLOOP_VELOCITY_ACTUAL)
//		TMC4671_SPIWriteInt(0x23, 	0x0000F581); 		// writing value 0x0000F581 = 62849 = 0.0 to address 27 = 0x23(OPENLOOP_PHI)
//		TMC4671_SPIWriteInt(0x24, 	0x00000C8B); 		// writing value 0x00000C8B = 3211 = 0.0 to address 28 = 0x24(UQ_UD_EXT)
//		TMC4671_SPIWriteInt(0x25, 	0x00001000); 		// writing value 0x00001000 = 4096 = 0.0 to address 29 = 0x25(ABN_DECODER_MODE)
//		TMC4671_SPIWriteInt(0x26, 	0x00004000); 		// writing value 0x00004000 = 16384 = 0.0 to address 30 = 0x26(ABN_DECODER_PPR)
//		TMC4671_SPIWriteInt(0x27, 	0x00003AFB); 		// writing value 0x00003AFB = 15099 = 0.0 to address 31 = 0x27(ABN_DECODER_COUNT)
//		TMC4671_SPIWriteInt(0x28, 	0x00000210); 		// writing value 0x00000210 = 528 = 0.0 to address 32 = 0x28(ABN_DECODER_COUNT_N)
//		TMC4671_SPIWriteInt(0x29, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 33 = 0x29(ABN_DECODER_PHI_E_PHI_M_OFFSET)
//		TMC4671_SPIWriteInt(0x2C, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 34 = 0x2C(ABN_2_DECODER_MODE)
//		TMC4671_SPIWriteInt(0x2D, 	0x00010000); 		// writing value 0x00010000 = 65536 = 0.0 to address 35 = 0x2D(ABN_2_DECODER_PPR)
//		TMC4671_SPIWriteInt(0x2E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 36 = 0x2E(ABN_2_DECODER_COUNT)
//		TMC4671_SPIWriteInt(0x2F, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 37 = 0x2F(ABN_2_DECODER_COUNT_N)
//		TMC4671_SPIWriteInt(0x30, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 38 = 0x30(ABN_2_DECODER_PHI_M_OFFSET)
//		TMC4671_SPIWriteInt(0x33, 	0x00000001); 		// writing value 0x00000001 = 1 = 0.0 to address 39 = 0x33(HALL_MODE)
//		TMC4671_SPIWriteInt(0x34, 	0x2AAA0000); 		// writing value 0x2AAA0000 = 715784192 = 0.0 to address 40 = 0x34(HALL_POSITION_060_000)
//		TMC4671_SPIWriteInt(0x35, 	0x80005555); 		// writing value 0x80005555 = 0 = 0.0 to address 41 = 0x35(HALL_POSITION_180_120)
//		TMC4671_SPIWriteInt(0x36, 	0xD555AAAA); 		// writing value 0xD555AAAA = 0 = 0.0 to address 42 = 0x36(HALL_POSITION_300_240)
//		TMC4671_SPIWriteInt(0x37, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 43 = 0x37(HALL_PHI_E_PHI_M_OFFSET)
//		TMC4671_SPIWriteInt(0x38, 	0x00002AAA); 		// writing value 0x00002AAA = 10922 = 0.0 to address 44 = 0x38(HALL_DPHI_MAX)
//		TMC4671_SPIWriteInt(0x3B, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 45 = 0x3B(AENC_DECODER_MODE)
//		TMC4671_SPIWriteInt(0x3C, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 46 = 0x3C(AENC_DECODER_N_THRESHOLD)
//		TMC4671_SPIWriteInt(0x3E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 47 = 0x3E(AENC_DECODER_PHI_A_OFFSET)
//		TMC4671_SPIWriteInt(0x40, 	0x00000001); 		// writing value 0x00000001 = 1 = 0.0 to address 48 = 0x40(AENC_DECODER_PPR)
//		TMC4671_SPIWriteInt(0x42, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 49 = 0x42(AENC_DECODER_COUNT_N)
//		TMC4671_SPIWriteInt(0x45, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 50 = 0x45(AENC_DECODER_PHI_E_PHI_M_OFFSET)
//		TMC4671_SPIWriteInt(0x4D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 51 = 0x4D(CONFIG_DATA)
//		TMC4671_SPIWriteInt(0x4E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 52 = 0x4E(CONFIG_ADDR)
//		TMC4671_SPIWriteInt(0x50, 	0x0000000C); 		// writing value 0x0000000C = 12 = 0.0 to address 53 = 0x50(VELOCITY_SELECTION)
//		TMC4671_SPIWriteInt(0x51, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 54 = 0x51(POSITION_SELECTION)
//		TMC4671_SPIWriteInt(0x52, 	0x00000003); 		// writing value 0x00000003 = 3 = 0.0 to address 55 = 0x52(PHI_E_SELECTION)
//		TMC4671_SPIWriteInt(0x54, 	0x01000100); 		// writing value 0x01000100 = 16777472 = 0.0 to address 56 = 0x54(PID_FLUX_P_FLUX_I)
//		TMC4671_SPIWriteInt(0x56, 	0x01000100); 		// writing value 0x01000100 = 16777472 = 0.0 to address 57 = 0x56(PID_TORQUE_P_TORQUE_I)
//		TMC4671_SPIWriteInt(0x58, 	0x01000100); 		// writing value 0x01000100 = 16777472 = 0.0 to address 58 = 0x58(PID_VELOCITY_P_VELOCITY_I)
//		TMC4671_SPIWriteInt(0x5A, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 59 = 0x5A(PID_POSITION_P_POSITION_I)
//		TMC4671_SPIWriteInt(0x5C, 	0x00007FFF); 		// writing value 0x00007FFF = 32767 = 0.0 to address 60 = 0x5C(PID_TORQUE_FLUX_TARGET_DDT_LIMITS)
//		TMC4671_SPIWriteInt(0x5D, 	0x00007FFF); 		// writing value 0x00007FFF = 32767 = 0.0 to address 61 = 0x5D(PIDOUT_UQ_UD_LIMITS)
//		TMC4671_SPIWriteInt(0x5E, 	0x00000E74); 		// writing value 0x00000E74 = 3700 = 0.0 to address 62 = 0x5E(PID_TORQUE_FLUX_LIMITS)
//		TMC4671_SPIWriteInt(0x5F, 	0x000003E8); 		// writing value 0x000003E8 = 1000 = 0.0 to address 63 = 0x5F(PID_ACCELERATION_LIMIT)
//		TMC4671_SPIWriteInt(0x60, 	0x00001770); 		// writing value 0x00001770 = 6000 = 0.0 to address 64 = 0x60(PID_VELOCITY_LIMIT)
//		TMC4671_SPIWriteInt(0x61, 	0x80000001); 		// writing value 0x80000001 = 0 = 0.0 to address 65 = 0x61(PID_POSITION_LIMIT_LOW)
//		TMC4671_SPIWriteInt(0x62, 	0x7FFFFFFF); 		// writing value 0x7FFFFFFF = 2147483647 = 0.0 to address 66 = 0x62(PID_POSITION_LIMIT_HIGH)
//		TMC4671_SPIWriteInt(0x63, 	0x00000002); 		// writing value 0x00000002 = 2 = 0.0 to address 67 = 0x63(MODE_RAMP_MODE_MOTION)
//		TMC4671_SPIWriteInt(0x64, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 68 = 0x64(PID_TORQUE_FLUX_TARGET)
//		TMC4671_SPIWriteInt(0x65, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 69 = 0x65(PID_TORQUE_FLUX_OFFSET)
//		TMC4671_SPIWriteInt(0x66, 	0xFFFFF510); 		// writing value 0xFFFFF510 = 0 = 0.0 to address 70 = 0x66(PID_VELOCITY_TARGET)
//		TMC4671_SPIWriteInt(0x67, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 71 = 0x67(PID_VELOCITY_OFFSET)
//		TMC4671_SPIWriteInt(0x68, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 72 = 0x68(PID_POSITION_TARGET)
//		TMC4671_SPIWriteInt(0x6B, 	0x64466740); 		// writing value 0x64466740 = 1682335552 = 0.0 to address 73 = 0x6B(PID_POSITION_ACTUAL)
//		TMC4671_SPIWriteInt(0x6D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 74 = 0x6D(PID_ERROR_ADDR)
//		TMC4671_SPIWriteInt(0x6E, 	0x839A817C); 		// writing value 0x839A817C = 0 = 0.0 to address 75 = 0x6E(INTERIM_DATA)
//		TMC4671_SPIWriteInt(0x6F, 	0x00000013); 		// writing value 0x00000013 = 19 = 0.0 to address 76 = 0x6F(INTERIM_ADDR)
//		TMC4671_SPIWriteInt(0x74, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 77 = 0x74(WATCHDOG_CFG)
//		TMC4671_SPIWriteInt(0x75, 	0xFFFFFFFF); 		// writing value 0xFFFFFFFF = 0 = 0.0 to address 78 = 0x75(ADC_VM_LIMITS)
//		TMC4671_SPIWriteInt(0x78, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 79 = 0x78(STEP_WIDTH)
//		TMC4671_SPIWriteInt(0x79, 	0x00009600); 		// writing value 0x00009600 = 38400 = 0.0 to address 80 = 0x79(UART_BPS)
//		TMC4671_SPIWriteInt(0x7A, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 81 = 0x7A(UART_ADDRS)
//		TMC4671_SPIWriteInt(0x7B, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 82 = 0x7B(GPIO_dsADCI_CONFIG)
//		TMC4671_SPIWriteInt(0x7C, 	0xC77C8080); 		// writing value 0xC77C8080 = 0 = 0.0 to address 83 = 0x7C(STATUS_FLAGS)
//		TMC4671_SPIWriteInt(0x7D, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 84 = 0x7D(STATUS_MASK)
//
//




//		Evalboards.ch1.writeRegister(0,0x66, 	0x00000FA0); 		// writing value 0x00000000 = 0 = 0.0 to address 70 = 0x66(PID_VELOCITY_TARGET)-----------------------> Velocity Target  4000 rpm
		//borro la tarea, solo se necesita para la configuracion inicial con delay entre habilitaciones y envios por spi de conf

		vTaskDelay(250 / portTICK_RATE_MS);
		Evalboards.ch1.enableDriver(DRIVER_DISABLE);

		vTaskDelete(NULL);

}

static void tcp_server_task(void *pvParameters) {
	char rx_buffer[128];
	char addr_str[128];
	int addr_family;
	int ip_protocol;
	int i;

	while (1) {

		//CONFIG_IPV4
		struct sockaddr_in dest_addr;
		dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(PORT);
		addr_family = AF_INET;
		ip_protocol = IPPROTO_IP;
		inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);

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

				///////////****************////////////////////////////
				/////prueba por desconexion aleatoria mensaje -->  recv failed: errno 104
				close(sock_global);
				close(listen_sock);
				///////////////***********/////////////////////////
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

				for (i = 0; i < len; i++) {
					tinysh_char_in(rx_buffer[i]);
				}
				//Para que  valide lo recibido!
				tinysh_char_in('\n');

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


void xtaskmonitorstatus(void *pvParameter) {

	char datos[16];

	while (1) {
		vTaskDelay(1000 / portTICK_RATE_MS);
		if (sock_global > 0){
//		itoa(processDipCoating.config.status,datos,10);
		sprintf(datos,"STATUS %d\r\n",processSpinCoating.config.status);

		send(sock_global, &datos, sizeof(datos) , 0);
		//send(sock_global, &processDipCoating.config.status, sizeof(processDipCoating.config.status) , 0);

		}
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
//	wifi_manager_start();

	/* register a callback as an example to how you can integrate your code with the wifi manager */
//	wifi_manager_set_callback(EVENT_STA_GOT_IP, &cb_connection_ok);


	/*   Inicializacion Rampa de Aceleracion*/
	tmc_linearRamp_init(rampGenerator);


	xTaskCreate(&xtasktinysh, "Tinysh Task", 8192, NULL, 4, NULL);
	xTaskCreate(&xtaskprocess, "Process Task", 8192, NULL, 2, NULL);
	xTaskCreate(&xtaskmotor, "Process Motor Task", 8192, NULL, 2, NULL);
//	xTaskCreate(&tcp_server_task, "tcp_server Task", 8192, NULL, 2, NULL);
//	xTaskCreate(&xtaskmonitorstatus, "Monitor Status Task", 8192, NULL, 2, NULL);


}













/* Hello World Example

 This example code is in the Public Domain (or CC0 licensed, at your option.)
 Unless required by applicable law or agreed to in writing, this
 software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied.
 */

#include <stdio.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "../components/protocol_examples_common/include/protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "../components/api/include/handlers.h"
#include "../components/api/include/process.h"
#include "app_main_dipcoater.h"


flagRun_t entry = STOP;

void task_process(process_t *processDipCoating){
	if (entry==RUN){
		ProcessRun(processDipCoating);
		entry=STOP;
	}
}

void xtaskprocess(void *pvParameter) {

	ProcessInit(&processDipCoating);
	ProcessLoadProgramStandard(&processDipCoating);
	entry = RUN;

	while (1) {
		task_process(&processDipCoating);
		vTaskDelay(1000 / portTICK_RATE_MS);
	}

}

void xtasktinysh(void *pvParameter) {

	static char c = 0;
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

	while (1) {
		c = getchar();
		tinysh_char_in(c);

	}

}


void app_main(void) {



	xTaskCreate(&xtaskprocess,"Process Task",2048,NULL,2,NULL );
	xTaskCreate(&xtasktinysh,"Tinysh Task",2048,NULL,2,NULL );


}














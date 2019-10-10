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


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_system.h"
#include "esp_spi_flash.h"


#include "inc/app_main_dipcoater.h"



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
//	entry = RUN;

	while (1) {
		task_process(&processDipCoating);
		vTaskDelay(100 / portTICK_RATE_MS);
	}

}

void xtasktinysh(void *pvParameter) {

	static char c = 0;

	tinysh_init();

	while (1) {
		c = getchar();
		tinysh_char_in(c);

		vTaskDelay(200 / portTICK_RATE_MS);

	}

}



void app_main(void) {


	xTaskCreate(&xtasktinysh,"Tinysh Task",2048,NULL,2,NULL );
	xTaskCreate(&xtaskprocess,"Process Task",2048,NULL,2,NULL );


}













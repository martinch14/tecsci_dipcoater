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

process_t processDipCoating;
flagRun_t entry = STOP;

void task_process(process_t *processDipCoating){
	if (entry==RUN){
		ProcessRun(processDipCoating);
		entry=STOP;
	}
}

void app_main(void) {
//	ESP_LOGI(TAG, "[APP] Startup...");
//	ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
//	ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

	printf("hola\n");

	ProcessInit(&processDipCoating);
	ProcessLoadProgramStandard(&processDipCoating);
	entry=RUN;

	while(1){

		task_process(&processDipCoating);

		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}


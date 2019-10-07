#include <stdio.h>

#ifndef TECSCI_ARCH_X86
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#endif

#include "../../components/api/include/handlers.h"

#include <stdio.h>
#include <unistd.h>

#ifndef TECSCI_ARCH_X86
#define sleep(x) vTaskDelay((x) * 1000 / portTICK_RATE_MS)
#endif

//Handler Functions
int HandlerCeroMachine(processCommandArgSpin_t*	arg) {
	printf("Buscando Cero Machine!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);
	sleep(5);
	return 0;
}
int HandlerSpin(processCommandArgSpin_t*	arg) {
	printf("Spinning!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);
	sleep(2);
	return 0;
}

int HandlerUp(processCommandArgSpin_t*	arg) {
	printf("Subiendo muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);
	sleep(2);
	return 0;
}

int HandlerDown(processCommandArgSpin_t*	arg) {
	printf("Bajando muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);
	sleep(2);
	return 0;
}

int HandlerWait(processCommandArgSpin_t*	arg) {
	printf("Muestra en espera!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);
	sleep(2);
	return 0;
}

int HandlerStop(processCommandArgSpin_t*	arg) {
	printf("Buscando Cero Machine!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);
	sleep(2);
	return 0;
}
int HandlerFinish(processCommandArgSpin_t*	arg) {
	printf("Proceso Finalizado!!\r\n");
	return 0;
}


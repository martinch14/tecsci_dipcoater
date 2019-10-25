#include <stdio.h>

#ifndef TECSCI_ARCH_X86
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


#include "TMCMotionController.h"
#include "TMC5130.h"



#endif

#include "handlers.h"
#include <unistd.h>

#ifndef TECSCI_ARCH_X86
#define sleep(x) vTaskDelay((x) * 1000 / portTICK_RATE_MS)
#endif

//Handler Functions
int HandlerCeroMachine(processCommandArgSpin_t*	arg) {
	Evalboards.ch1.rotate(0,0x00000000);
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
	Evalboards.ch1.left(0,0x00009C40); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)
	sleep(2);
	return 0;
}


int HandlerUp_without_program(processCommandArgSpin_t*	arg) {
	Evalboards.ch1.enableDriver(DRIVER_ENABLE);
	printf("Subiendo muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);
	Evalboards.ch1.left(0,0x00009C40); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)
	sleep(2);
	Evalboards.ch1.enableDriver(DRIVER_DISABLE);
	return 0;
}


int HandlerDown(processCommandArgSpin_t*	arg) {
	printf("Bajando muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);
	Evalboards.ch1.right(0,0x00009C40); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)
	sleep(2);
	return 0;
}

int HandlerWait(processCommandArgSpin_t*	arg) {
	printf("Muestra en espera!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);
	Evalboards.ch1.rotate(0,0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)
	sleep(2);
	return 0;
}

int HandlerRun() {

	Evalboards.ch1.enableDriver(DRIVER_ENABLE);
	return 0;
}


int HandlerStop(processCommandArgSpin_t*	arg) {
	printf("Comando Stop!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);
	Evalboards.ch1.rotate(0,0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)
	sleep(2);
	return 0;
}
int HandlerFinish(processCommandArgSpin_t*	arg) {

	Evalboards.ch1.enableDriver(DRIVER_DISABLE);
	printf("Proceso Finalizado - Motor Apagado\r\n");

	return 0;
}


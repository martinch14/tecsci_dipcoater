/*
 * app_main_dipcoater.h
 *
 *  Created on: 12 sep. 2019
 *      Author: martin
 */

#ifndef MAIN_APP_MAIN_DIPCOATER_H_
#define MAIN_APP_MAIN_DIPCOATER_H_


#include "handlers.h"
#include "process.h"
#include "command.h"
#include "enviromental_chamber_control.h"
#include "mod_queue.h"
#include "protocol_examples_common.h"
#include "tinysh.h"
#include "TMCL.h"

#include "LinearRamp.h"


#include "freertos/queue.h"


spi_device_handle_t  spi_dev , spi_dev6100;



// Global process_t for tinysh handler use
process_t processSpinCoating;

/*Creo queue de FreRTOS*/
//Cola de mensajes para pasar los comandos recibidos por la consola
QueueHandle_t xQueueConsolaReception;



// Socket TCP-IP global para envíar desde los handlers de los comandos para informar estado a la aplicación
int sock_global;

extern TMC_LinearRamp rampGenerator[1];



//void init(void);
//void deinit(void);
void reset(void);
//void app_main(void);



#endif /* MAIN_APP_MAIN_DIPCOATER_H_ */

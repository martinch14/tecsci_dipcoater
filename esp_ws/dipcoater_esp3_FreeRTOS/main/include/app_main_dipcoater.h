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

spi_device_handle_t  spi_dev;

typedef enum{RUN,STOP}flagRun_t;

// Global process_t for tinysh handler use
process_t processDipCoating;



// Global process_t for tinysh handler use

enviromental_chamber_t enviromentalChamberDipCoating;


/*Semaphore */




/*Queue for comunication with Process */
mod_queue_t queueconsolareception;
mod_queue_t queueconsolatransmit;
processCommand_t bufferreception[10];


/*Creo queue de FreRTOS*/




int buffertransmit[10];

// Socket TCP-IP global para envíar desde los handlers de los comandos para informar estado a la aplicación
int sock_global;



void init(void);
void deinit(void);
void reset(void);
void app_main(void);



#endif /* MAIN_APP_MAIN_DIPCOATER_H_ */

/*
 * app_main_dipcoater.h
 *
 *  Created on: 12 sep. 2019
 *      Author: martin
 */

#ifndef MAIN_APP_MAIN_DIPCOATER_H_
#define MAIN_APP_MAIN_DIPCOATER_H_

#include "tinysh.h"
#include "handlers.h"
#include "process.h"
#include "command.h"
#include "enviromental_chamber_control.h"
#include "mod_queue.h"
#include "protocol_examples_common.h"


typedef enum{RUN,STOP}flagRun_t;

// Global process_t for tinysh handler use
process_t processDipCoating;


// Global process_t for tinysh handler use

enviromental_chamber_t enviromentalChamberDipCoating;



/*Queue for comunication with Process */
mod_queue_t queueconsolareception;
mod_queue_t queueconsolatransmit;
processCommand_t bufferreception[10];
int buffertransmit[10];


int app_main_dipcoater(void);


void task_process(process_t *processDipCoating);
//void task_motor(void);
//void task_enviromental_chamber(void);


void xtaskprocess(void *pvParameter);
void xtasktinysh(void *pvParameter);




#endif /* MAIN_APP_MAIN_DIPCOATER_H_ */
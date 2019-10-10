/*
 * app_main_dipcoater.h
 *
 *  Created on: 12 sep. 2019
 *      Author: martin
 */

#ifndef MAIN_APP_MAIN_DIPCOATER_H_
#define MAIN_APP_MAIN_DIPCOATER_H_


#include "../../components/api/include/handlers.h"
#include "../../components/api/include/process.h"
#include "../../components/api/include/command.h"
#include "../../components/tinysh/include/tinysh.h"
//#include "../../components/tinysh/include/tinysh_custom.h"
//#include "../../components/tinysh/include/tinysh_core.h"
//#include "../../components/tinysh/include/tinysh_utils.h"
#include "../../components/api/include/enviromental_chamber_control.h"
#include "../../components/api/include/mod_queue.h"
#include "../../components/protocol_examples_common/include/protocol_examples_common.h"


typedef enum{RUN,STOP}flagRun_t;

// Global process_t for tinysh handler use
process_t processDipCoating;


// Global process_t for tinysh handler use
//process_t processDipCoating;
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

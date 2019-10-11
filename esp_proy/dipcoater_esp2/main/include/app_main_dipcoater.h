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

//#include "tinysh_core.h"
//#include "tinysh_custom.h"
//#include "tinysh_utils.h"
putchar_handler_t putcharHandler = NULL;

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





#endif /* MAIN_APP_MAIN_DIPCOATER_H_ */

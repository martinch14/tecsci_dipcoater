/*
 * app_main_dipcoater.h
 *
 *  Created on: 12 sep. 2019
 *      Author: martin
 */

#ifndef MAIN_APP_MAIN_DIPCOATER_H_
#define MAIN_APP_MAIN_DIPCOATER_H_


#include "../components/api/include/handlers.h"
#include "../components/api/include/process.h"
#include "../components/api/include/mod_queue.h"


/*ver porque no lo puedo incluir*/
//#include "../tinysh.h"

#include "../components/tinysh/include/tinysh.h"


/*Definiciones de colas y buffer  para la mensajeria, las mismas se manejan a nivel de variable Global*/

mod_queue_t queueconsolareception,queueconsolatransmit;
processCommand_t bufferreception[10];
int buffertransmit[10];


int app_main_dipcoater(void);

void task_tinysh(char c);
void task_process(process_t *processDipCoating);
void task_motor(void);


void HandlerConsolePutchar(unsigned char c);

#endif /* MAIN_APP_MAIN_DIPCOATER_H_ */

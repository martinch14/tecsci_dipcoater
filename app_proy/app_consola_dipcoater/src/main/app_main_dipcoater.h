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


/*Definiciones de colas y buffer  para la mensajeria*/

mod_queue_t queueconsolareception,queueconsolatransmit;
float bufferreception[10];
float buffertransmit[10];


int app_main_dipcoater(void);
void HandlerConsolePutchar(unsigned char c);
void tinysh_update(char c);





#endif /* MAIN_APP_MAIN_DIPCOATER_H_ */

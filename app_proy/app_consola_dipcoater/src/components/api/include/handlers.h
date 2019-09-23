/*
 * handlers.h
 *
 *  Created on: 6 sep. 2019
 *      Author: martin
 */

#ifndef COMPONENTS_API_INCLUDE_HANDLERS_H_
#define COMPONENTS_API_INCLUDE_HANDLERS_H_


#include "../../../components/api/include/process.h"


int HandlerCeroMachine(processCommandArgSpin_t	*arg);
int HandlerSpin(processCommandArgSpin_t		*arg);
int HandlerUp(processCommandArgSpin_t 	*arg);
int HandlerDown(processCommandArgSpin_t *arg);
int HandlerWait(processCommandArgSpin_t *arg);
int HandlerStop(processCommandArgSpin_t *arg);
int HandlerFinish(processCommandArgSpin_t *arg);

#endif /* COMPONENTS_API_INCLUDE_HANDLERS_H_ */

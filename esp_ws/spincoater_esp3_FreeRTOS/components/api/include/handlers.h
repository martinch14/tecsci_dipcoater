/*
 * handlers.h
 *
 *  Created on: 6 sep. 2019
 *      Author: martin
 */

#ifndef COMPONENTS_API_INCLUDE_HANDLERS_H_
#define COMPONENTS_API_INCLUDE_HANDLERS_H_


#include "process.h"
#include "LinearRamp.h"

extern TMC_LinearRamp rampGenerator[1];


int HandlerRight(processCommandArgSpin_t 	*arg);
int HandlerRight_without_program(processCommandArgSpin_t*	arg);
int HandlerLeft(processCommandArgSpin_t *arg);
int HandlerLeft_without_program(processCommandArgSpin_t*	arg);
int HandlerWait(processCommandArgSpin_t *arg);
int HandlerStop_without_program(processCommandArgSpin_t *arg);
int HandlerStop(processCommandArgSpin_t *arg);
int HandlerRun(void);
//Handler para realizar TESTS
int HandlerREADDATA();





#endif /* COMPONENTS_API_INCLUDE_HANDLERS_H_ */

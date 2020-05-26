/*
 * handlers.h
 *
 *  Created on: 6 sep. 2019
 *      Author: martin
 */

#ifndef COMPONENTS_API_INCLUDE_HANDLERS_H_
#define COMPONENTS_API_INCLUDE_HANDLERS_H_


#include "process.h"


int HandlerCeroMachine(processCommandArgSpin_t	*arg);
int HandlerSpin(processCommandArgSpin_t		*arg);

int HandlerUp(processCommandArgSpin_t 	*arg);
int HandlerUpLoop(processCommandArgSpin_t 	*arg);
int HandlerUp_without_program(processCommandArgSpin_t*	arg);

int HandlerDown(processCommandArgSpin_t *arg);
int HandlerDownLoop(processCommandArgSpin_t *arg);
int HandlerDown_without_program(processCommandArgSpin_t*	arg);



int HandlerWait(processCommandArgSpin_t *arg);
int HandlerWaitDown(processCommandArgSpin_t *arg);
int HandlerWaitUp(processCommandArgSpin_t *arg);
int HandlerStop(processCommandArgSpin_t *arg);
int HandlerFinish(processCommandArgSpin_t *arg);
int HandlerRun();

int HandlerENA_DRIVER();
int HandlerDIS_DRIVER();
int HandlerCERO_SAMPLE();
int HandlerDELTADIP();

//Handler para realizar TESTS
int HandlerREADDATA();


//Control del limite mecanico (recorrido) de la maquina, los valores aqui dependen del largo del eje Z
uint8_t controlLimit(void);



#endif /* COMPONENTS_API_INCLUDE_HANDLERS_H_ */

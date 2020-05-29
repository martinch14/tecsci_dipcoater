/**************************************************************************************************
**  (c) Copyright 2019: Martin Abel Gambarotta <magambarotta@gmail.com>
**  This file is part of DipCoater_Tecsci.
**
**  DipCoater_Tecsci is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  DipCoater_Tecsci is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with DipCoater_Tecsci.  If not, see <https://www.gnu.org/licenses/>.
*************************************************************************************************/


/** @file 	handlers.h
 ** @brief 	Declaracion
 **
 **| REV | YYYY.MM.DD | Autor           | Descripción de los cambios                              |
 **|-----|------------|-----------------|---------------------------------------------------------|
 **|   1 | 2020.05.28 | magambarotta    | Version inicial 									      |
 ** @addtogroup aplicacion
 ** @{ */


/*=====[Evitar inclusion multiple comienzo]==================================*/
#ifndef COMPONENTS_API_INCLUDE_HANDLERS_H_
#define COMPONENTS_API_INCLUDE_HANDLERS_H_

/*=====[C++ comienzo]========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Inclusiones de dependencias de funciones publicas]===================*/
#include "process.h"


/*=====[Macros de definicion de constantes publicas]=========================*/
/*=====[Macros estilo funcion publicas]======================================*/
/*=====[Definiciones de tipos de datos publicos]=============================*/
/*=====[Prototipos de funciones publicas]====================================*/

/*
* @brief	funcion_thread_serial_educiaa  -> Esta funcion hace un polling al sobre el puerto Serial
* 		conectado a la EDUCIAA y si llega un mensaje lo retransmite a traves de un FD hacia el TCP client
* @param[]  Referencia a estructura del tipo pthread_t
* @returns  0 Creacion correcta
*/


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

int HandlerRESET();

//Handler para realizar TESTS
int HandlerREADDATA();


//Control del limite mecanico (recorrido) de la maquina, los valores aqui dependen del largo del eje Z
//uint8_t controlLimit(void);


/*=====[C++ fin]=============================================================*/

#ifdef __cplusplus
}
#endif

/** @} Final de la definición del modulo para doxygen */

/*=====[Evitar inclusion multiple fin]=======================================*/



#endif /* COMPONENTS_API_INCLUDE_HANDLERS_H_ */

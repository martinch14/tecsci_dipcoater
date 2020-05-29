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


/** @file 	dipcoater_main.h
 ** @brief 	Declaracion
 **
 **| REV | YYYY.MM.DD | Autor           | Descripción de los cambios                              |
 **|-----|------------|-----------------|---------------------------------------------------------|
 **|   1 | 2020.05.28 | magambarotta    | Version inicial 									      |
 ** @addtogroup aplicacion
 ** @{ */


/*=====[Evitar inclusion multiple comienzo]==================================*/

#ifndef MAIN_APP_MAIN_DIPCOATER_H_
#define MAIN_APP_MAIN_DIPCOATER_H_


/*=====[Inclusiones de dependencias de funciones publicas]===================*/

#include "handlers.h"
#include "process.h"
#include "command.h"

//#include "mod_queue.h"
#include "protocol_examples_common.h"
#include "tinysh.h"
#include "TMCL.h"
#include "freertos/queue.h"


/*=====[C++ comienzo]========================================================*/

#ifdef __cplusplus
extern "C" {
#endif


/*=====[Macros de definicion de constantes publicas]=========================*/


/*=====[Definiciones de tipos de datos publicos]=============================*/

spi_device_handle_t  spi_dev;


// Tipo de datos enumerado
// Tipo de datos estructua, union o campo de bits

typedef enum{RUN,STOP}flagRun_t;

// Global process_t for tinysh handler use
process_t processDipCoating;



/*Creo queue de FreRTOS*/
//Cola de mensajes para pasar los comandos recibidos por la consola
QueueHandle_t xQueueConsolaReception;
QueueHandle_t xQueueConsolaReceptionPrograma;


int buffertransmit[10];

// Socket TCP-IP global para envíar desde los handlers de los comandos para informar estado a la aplicación
int sock_global;


/*=====[Prototipos de funciones publicas]====================================*/
/*
* @brief	funcion_thread_serial_educiaa  -> Esta funcion hace un polling al sobre el puerto Serial
* 		conectado a la EDUCIAA y si llega un mensaje lo retransmite a traves de un FD hacia el TCP client
* @param[]  Referencia a estructura del tipo pthread_t
* @returns  0 Creacion correcta
*/


void init(void);
void deinit(void);
void reset(void);
void app_main(void);

/*=====[C++ fin]=============================================================*/

#ifdef __cplusplus
}
#endif

/** @} Final de la definición del modulo para doxygen */

/*=====[Evitar inclusion multiple fin]=======================================*/

#endif /* MAIN_APP_MAIN_DIPCOATER_H_ */

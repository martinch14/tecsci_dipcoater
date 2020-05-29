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


/** @file 	process.h
 ** @brief 	Declaracion
 **
 **| REV | YYYY.MM.DD | Autor           | Descripción de los cambios                              |
 **|-----|------------|-----------------|---------------------------------------------------------|
 **|   1 | 2020.05.28 | magambarotta    | Version inicial 									      |
 ** @addtogroup aplicacion
 ** @{ */


/*=====[Evitar inclusion multiple comienzo]==================================*/

#ifndef PORT_INCLUDE_TMC_PORT_H_
#define PORT_INCLUDE_TMC_PORT_H_


/*=====[C++ comienzo]========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Inclusiones de dependencias de funciones publicas]===================*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "driver/spi_master.h"


/*=====[Macros de definicion de constantes publicas]=========================*/
#define ON (1)
#define OFF (0)
#define TRUE (1)
#define FALSE (~TRUE)

/*=====[Macros estilo funcion publicas]======================================*/
//#define printInt(printer,number) printIntFormat((printer),(number),(DEC_FORMAT))



/*=====[Definiciones de tipos de datos publicos]=============================*/

typedef uint32_t tick_t;
typedef uint8_t bool_t;



/*=====[Prototipos de funciones publicas]====================================*/

/*
* @brief	funcion_thread_serial_educiaa  -> Esta funcion hace un polling al sobre el puerto Serial
* 		conectado a la EDUCIAA y si llega un mensaje lo retransmite a traves de un FD hacia el TCP client
* @param[]  Referencia a estructura del tipo pthread_t
* @returns  0 Creacion correcta
*/

#if 0
enum
{
	PORT_GPIO_SPI1_CSN 		= ENET_TXD0,
	PORT_GPIO_DRV_ENN 		= GPIO6,
	PORT_GPIO_AIN_REF_SW 	= RS232_TXD,
	PORT_GPIO_SWSEL 		= T_FIL3,
	PORT_GPIO_AIN_REF_PWM 	= RS232_RXD,
};

enum{
	PORT_GPIO_CONFIG_OUTPUT = GPIO_OUTPUT,
};
#else
enum
{
	PORT_GPIO_DRV_ENN = 16,
	PORT_GPIO_AIN_REF_SW = 18,
	PORT_GPIO_SWSEL = 19,
	PORT_GPIO_AIN_REF_PWM = 21,

	PORT_CLK  =12,
	PORT_MOSI =14,
	PORT_GPIO_SPI1_CSN = 15,
	PORT_MISO =27

};

enum{
	PORT_GPIO_CONFIG_OUTPUT,
};
#endif

bool_t 						tmc_gpio_config			(int pin, int config);
bool_t 						tmc_gpio_write			(int pin, bool_t value);
bool_t 						tmc_gpio_read			(int pin);
spi_device_handle_t 		tmc_spi_init			(void);
uint8_t 					tmc_spi_readWrite		(uint8_t channel, uint8_t data, bool lastTransfer);
void 						tmc_spi_readWriteArray	(uint8_t channel, uint8_t *tx,uint8_t *rx, size_t length);

/*=====[C++ fin]=============================================================*/

#ifdef __cplusplus
}
#endif

/** @} Final de la definición del modulo para doxygen */

/*=====[Evitar inclusion multiple fin]=======================================*/

#endif /* PORT_INCLUDE_TMC_PORT_H_ */

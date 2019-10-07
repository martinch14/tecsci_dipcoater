/*
 * tmc_port.h
 *
 *  Created on: Oct 7, 2019
 *      Author: seb
 */

#ifndef PORT_INCLUDE_TMC_PORT_H_
#define PORT_INCLUDE_TMC_PORT_H_

#include "chip.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "sapi_gpio.h"

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

bool_t 		tmc_gpio_config			(int pin, int config);
bool_t 		tmc_gpio_write			(int pin, bool_t value);
bool_t 		tmc_gpio_read			(int pin);
void 		tmc_spi_init			(void);
uint8_t 	tmc_spi_readWrite		(uint8_t channel, uint8_t data, bool lastTransfer);
void 		tmc_spi_readWriteArray	(uint8_t channel, uint8_t *data, size_t length);

#endif /* PORT_INCLUDE_TMC_PORT_H_ */

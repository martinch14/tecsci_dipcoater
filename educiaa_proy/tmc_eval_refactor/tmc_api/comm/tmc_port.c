/*
 * tmc_port.c
 *
 *  Created on: Oct 7, 2019
 *      Author: seb
 */

#include "chip.h"
#include "tmc_port.h"

bool_t tmc_gpio_config(int pin, int config) {
	return gpioConfig((gpioMap_t)pin, (gpioConfig_t)config);
}

bool_t tmc_gpio_write(int pin, bool_t value) {
	return gpioWrite((gpioMap_t)pin, value);
}

bool_t tmc_gpio_read(int pin) {
	return gpioRead((gpioMap_t)pin);
}

uint8_t tmc_spi_readWrite(uint8_t channel, uint8_t writeData, bool lastTransfer)
{
	tmc_gpio_write(PORT_GPIO_SPI1_CSN, OFF);
	uint8_t ret = 0;

	Chip_SSP_DATA_SETUP_T config = {
		.tx_data = &writeData,
		.tx_cnt  = 0,
		.rx_data = &ret,
		.rx_cnt  = 0,
		.length  = 1
	};

	Chip_SSP_RWFrames_Blocking(LPC_SSP1, &config);

	if(lastTransfer)
		tmc_gpio_write(PORT_GPIO_SPI1_CSN, ON);

	return ret;
}

void tmc_spi_readWriteArray(uint8_t channel, uint8_t *data, size_t length)
{
	tmc_gpio_write(PORT_GPIO_SPI1_CSN, OFF);

	Chip_SSP_DATA_SETUP_T config = {
		.tx_data = data,
		.tx_cnt  = 0,
		.rx_data = data,
		.rx_cnt  = 0,
		.length  = length
	};

	Chip_SSP_RWFrames_Blocking(LPC_SSP1, &config);

	tmc_gpio_write(PORT_GPIO_SPI1_CSN, ON);
//	for(size_t i = 0; i < length; i++)
//		data[i] = spi_readWrite(channel, data[i], (i == length - 1) ? true : false);
}

void tmc_spi_init()
{
	tmc_gpio_config(PORT_GPIO_SPI1_CSN, PORT_GPIO_CONFIG_OUTPUT);

	Chip_SCU_PinMuxSet(0xf, 4, (SCU_MODE_PULLUP | SCU_MODE_FUNC0)); /* CLK0 */
	Chip_SCU_PinMuxSet(0x1, 3, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5)); /* MISO1 */
	Chip_SCU_PinMuxSet(0x1, 4, (SCU_MODE_PULLUP | SCU_MODE_FUNC5)); /* MOSI1 */

	Chip_SSP_Init(LPC_SSP1);
	Chip_SSP_SetFormat(LPC_SSP1, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA1_CPOL1);
	Chip_SSP_SetBitRate(LPC_SSP1, 2666666);
	Chip_SSP_Enable(LPC_SSP1);
}

/*
 * tmc_port.c
 *
 *  Created on: Oct 7, 2019
 *      Author: seb
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tmc_port.h"

extern spi_device_handle_t  spi_dev;

extern uint8_t rxx[];

bool_t tmc_gpio_config(int pin, int config) {
#if 0
	return gpioConfig((gpioMap_t)pin, (gpioConfig_t)config);
#else
	printf("tmc_gpio_config\n");
	esp_err_t ret;
	gpio_config_t io_conf;

	//bit mask of the pins that you want to set,e.g.GPIO18/19
	io_conf.pin_bit_mask = (1ULL << pin); //GPIO_OUTPUT_PIN_SEL/GPIO_INPUT_PIN_SEL;

	//set as output or input mode
	if (config == 0) {
		io_conf.mode = GPIO_MODE_OUTPUT;
	} else if (config == 1) {
		io_conf.mode = GPIO_MODE_INPUT;
	}

	//disable the functions we are not using
	io_conf.intr_type = GPIO_PIN_INTR_DISABLE; 	//disable interrupt
	io_conf.pull_down_en = 0;	//disable pull-down mode
	io_conf.pull_up_en = 0;	//disable pull-up mode

	ret = gpio_config(&io_conf); //configure GPIO with the given settings

	if (ret == ESP_OK) {
		return TRUE;
	} else if (ret == ESP_ERR_INVALID_ARG) {
		return FALSE;
	}
	return FALSE;

#endif
}

bool_t tmc_gpio_write(int pin, bool_t value) {
#if 0
	return gpioWrite((gpioMap_t)pin, value);
#else
	printf("tmc_gpio_write\n");
	esp_err_t ret;
	ret=gpio_set_level((gpio_num_t)pin, value);

	if(ret == ESP_OK){
		return TRUE;
	}
	else if(ret == ESP_ERR_INVALID_ARG){
		return FALSE;
	}
  return FALSE;

#endif
}

bool_t tmc_gpio_read(int pin) {
#if 0
	return gpioRead((gpioMap_t)pin);
#else
	printf("tmc_gpio_read\n");
	return (bool_t)gpio_get_level((gpio_num_t)pin);

#endif
}

uint8_t tmc_spi_readWrite(uint8_t channel, uint8_t writeData, bool lastTransfer)
{
#if 0
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
#else
	printf("tmc_spi_readWrite\n");
	return 0;
#endif
}

void tmc_spi_readWriteArray(uint8_t channel, uint8_t *tx, size_t length)
{
#if 0
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
#else
	printf("tmc_spi_readWriteArray\n");
	spi_transaction_t t;
	esp_err_t ret;
//	size_t i;

	//LENGTH set
	t.length = (5*8);  /*transaction length is in bits.*/

	/*This is to enable the use of the static structure inside the spi_transaction_t (up to: 32 bits == 4 bytes)*/
	//TX set
//	if(length<=32){ /*If the data to be transferred is 32 bits or less, it can be stored in the transaction struct itself.*/
//		 							/*For transmitted data, use the tx_data member for this and set the SPI_TRANS_USE_TXDATA*/
//		t.flags=SPI_TRANS_USE_TXDATA; //flag to enable
//		for(i=0;i<(length/8);i++){
//			t.tx_data[i]=tx[i];
//		}
//	}
//	else if (length>32) { //void *tx_buffer
	t.tx_buffer = tx;
//	}
	//TX set
	//t.tx_buffer = tx;

	//RX set
	t.rx_buffer = rxx;
//	ret=spi_device_polling_transmit(spi_dev, &t);
	ret=spi_device_transmit(spi_dev, &t);
	ESP_ERROR_CHECK(ret);
	#endif
}

spi_device_handle_t tmc_spi_init()
{
#if 0
	tmc_gpio_config(PORT_GPIO_SPI1_CSN, PORT_GPIO_CONFIG_OUTPUT);

	Chip_SCU_PinMuxSet(0xf, 4, (SCU_MODE_PULLUP | SCU_MODE_FUNC0)); /* CLK0 */
	Chip_SCU_PinMuxSet(0x1, 3, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5)); /* MISO1 */
	Chip_SCU_PinMuxSet(0x1, 4, (SCU_MODE_PULLUP | SCU_MODE_FUNC5)); /* MOSI1 */

	Chip_SSP_Init(LPC_SSP1);
	Chip_SSP_SetFormat(LPC_SSP1, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA1_CPOL1);
	Chip_SSP_SetBitRate(LPC_SSP1, 2666666);
	Chip_SSP_Enable(LPC_SSP1);
#else
	printf("tmc_spi_init\n");
	esp_err_t ret;
	spi_device_handle_t spi_dev_i;
	spi_bus_config_t buscfg={
			.miso_io_num=27,
			.mosi_io_num=14,
			.sclk_io_num=12,
			.quadwp_io_num=-1,
			.quadhd_io_num=-1,
			//.max_transfer_sz=PARALLEL_LINES*320*2+8
	};
	spi_device_interface_config_t devcfg={
			.clock_speed_hz=100000,
			.mode=3,                            //SPI mode 3
			.spics_io_num=15,       			//CS pin
			.queue_size=1                       //We want to be able to queue 7 transactions at a time
			//.pre_cb=lcd_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
	};

	//Initialize the SPI bus
	ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
	ESP_ERROR_CHECK(ret);

	ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi_dev_i);
	ESP_ERROR_CHECK(ret);
return spi_dev_i;

#endif
}

#include "chip.h"
#include "spi.h"
#include "SysTick.h"

typedef struct {
	gpioMap_t SPI1_CSN;
} SPI_Pins;

static SPI_Pins pins;

uint8_t spi_readWrite(spiMap_t channel, uint8_t writeData, bool lastTransfer)
{
	gpioWrite(pins.SPI1_CSN, OFF);
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
		gpioWrite(pins.SPI1_CSN, ON);

	return ret;
}

void spi_readWriteArray(spiMap_t channel, uint8_t *data, size_t length)
{
	gpioWrite(pins.SPI1_CSN, OFF);

	Chip_SSP_DATA_SETUP_T config = {
		.tx_data = data,
		.tx_cnt  = 0,
		.rx_data = data,
		.rx_cnt  = 0,
		.length  = length
	};

	Chip_SSP_RWFrames_Blocking(LPC_SSP1, &config);

	gpioWrite(pins.SPI1_CSN, ON);
//	for(size_t i = 0; i < length; i++)
//		data[i] = spi_readWrite(channel, data[i], (i == length - 1) ? true : false);
}

void Board_SSP_Init(LPC_SSP_T *pSSP)
{
	if (pSSP == LPC_SSP1) {
		/* Set up clock and power for SSP1 module */
		/* Configure SSP1 pins */
		Chip_SCU_PinMuxSet(0xf, 4, (SCU_MODE_PULLUP | SCU_MODE_FUNC0)); /* CLK0 */
		Chip_SCU_PinMuxSet(0x1, 3, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5)); /* MISO1 */
		Chip_SCU_PinMuxSet(0x1, 4, (SCU_MODE_PULLUP | SCU_MODE_FUNC5)); /* MOSI1 */

		Chip_SCU_PinMuxSet(0x6, 1, (SCU_MODE_PULLUP | SCU_MODE_FUNC0)); /* CS1 configured as GPIO */
		Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 3, 0);
	}
	else {
		return;
	}
}

void spi_init()
{
	pins.SPI1_CSN = ENET_TXD0;
	gpioConfig(pins.SPI1_CSN, GPIO_OUTPUT);
	spiConfig(SPI0);

	/* SPI configuration */
	Board_SSP_Init(LPC_SSP1);
	Chip_SSP_Init(LPC_SSP1);
	Chip_SSP_SetFormat(LPC_SSP1, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA1_CPOL1);
#if defined(TMC5130_eval)
	Chip_SSP_SetBitRate(LPC_SSP1, 2666666);
#elif defined(TMC4671_eval)
	Chip_SSP_SetBitRate(LPC_SSP1, 1000000);
#endif
	Chip_SSP_Enable(LPC_SSP1);
}

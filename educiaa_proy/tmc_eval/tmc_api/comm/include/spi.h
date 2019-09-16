#ifndef TMC_SPI_H
#define TMC_SPI_H

#include "chip.h"
#include "port.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void spi_init(void);
uint8_t spi_readWrite(spiMap_t channel, uint8_t data, bool lastTransfer);
void spi_readWriteArray(spiMap_t channel, uint8_t *data, size_t length);

#endif /* TMC_SPI_H */

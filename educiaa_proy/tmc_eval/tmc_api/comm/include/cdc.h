#ifndef TMC_CDC_H
#define TMC_CDC_H

#include <stddef.h>
#include "cdc_vcom.h"

void usb_init(void);
void usb_periodicJob(void);

#endif /* TMC_CDC_H */

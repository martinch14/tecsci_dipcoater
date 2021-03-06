/*
 * tmc_header.h
 *
 *  Created on: 29.09.2016
 *      Author: ed
 */

#ifndef TMC_API_HEADER_H_
#define TMC_API_HEADER_H_

#include <stdlib.h>
#include "tmc_port.h"

#include "Bits.h"
#include "Config.h"
#include "CRC.h"
#include "Macros.h"
#include "RegisterAccess.h"
#include "Constants.h"
#include "Types.h"

	// TODO: Restructure these.
	/*
	 * Goal: Just give these values here as status back to the IDE when used with EvalSystem.
	 * Currently, this is obtained by just leaving out implementation specific error bits here.
	 */
	typedef enum {
		TMC_ERROR_NONE      = 0x00,
		TMC_ERROR_GENERIC   = 0x01,
		TMC_ERROR_FUNCTION  = 0x02,
		TMC_ERROR_MOTOR     = 0x08,
		TMC_ERROR_VALUE     = 0x10,
		TMC_ERROR_CHIP      = 0x40
	} TMCError;

#endif /* TMC_API_HEADER_H_ */

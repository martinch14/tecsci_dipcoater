#include "Board.h"
#include <stdbool.h>
#include "TMC5130.h"
#include <string.h>

#define VM_MIN  50   // VM[V/10] min
#define VM_MAX  480  // VM[V/10] max

// SPI Channel selection
#define DEFAULT_CHANNEL  0

static uint32_t rotate(uint8_t motor, int32_t velocity);
static uint32_t right(uint8_t motor, int32_t velocity);
static uint32_t left(uint8_t motor, int32_t velocity);
static uint32_t stop(uint8_t motor);
static uint32_t moveTo(uint8_t motor, int32_t position);
static uint32_t moveBy(uint8_t motor, int32_t *ticks);

static uint32_t GAP(uint8_t type, uint8_t motor, int32_t *value);
static uint32_t SAP(uint8_t type, uint8_t motor, int32_t value);
static void readRegister(uint8_t motor, uint8_t address, int32_t *value);
static void writeRegister(uint8_t motor, uint8_t address, int32_t value);
static uint32_t getMeasuredSpeed(uint8_t motor, int32_t *value);

static void periodicJob(tick_t tick);
static void checkErrors(tick_t tick);
static void deInit(void);
static uint32_t userFunction(uint8_t type, uint8_t motor, int32_t *value);

static uint8_t reset();
static void enableDriver(DriverState state);

static TMC5130TypeDef TMC5130;
static uint32_t vmax_position;
//static LPC_SSP_T *TMC5130_SPIChannel;
//static Chip_SSP_DATA_SETUP_T data;

typedef struct {
	gpioMap_t DRV_ENN;
	gpioMap_t AIN_REF_SW;
	gpioMap_t SWSEL;
	gpioMap_t AIN_REF_PWM;
} TMC5130_Pins;

static TMC5130_Pins pins;

// Translate motor number to TMC5130TypeDef
// When using multiple ICs you can map them here
static inline TMC5130TypeDef *motorToIC(uint8_t motor) {
	UNUSED(motor);

	return &TMC5130;
}

#if 0
static inline spiMap_t channelToSPI(uint8 channel)
{
	UNUSED(channel);

	return SPI0;
}
#endif

// SPI Wrapper for API
void tmc5130_readWriteArray(uint8_t channel, uint8_t *data, size_t length) {
// Map the channel to the corresponding SPI channel
//channelToSPI(channel)->readWriteArray(data, length);
//uint32_t Chip_SSP_RWFrames_Blocking(LPC_SSP_T *pSSP, Chip_SSP_DATA_SETUP_T *xf_setup);
//	typedef struct {
//		void      *tx_data;	/*!< Pointer to transmit data */
//		uint32_t  tx_cnt;	/*!< Transmit counter */
//		void      *rx_data;	/*!< Pointer to transmit data */
//		uint32_t  rx_cnt;	/*!< Receive counter */
//		uint32_t  length;	/*!< Length of transfer data */
//	} Chip_SSP_DATA_SETUP_T;
//Chip_SSP_RWFrames_Blocking(channelToSPI(channel), );
	//memset(data, 0, length);
#if 0
	tmc_spi_readWriteArray(channelToSPI(channel), data, length);
#endif
	tmc_spi_readWriteArray(channel, data, length);
}

// => Functions forwarded to API
static uint32_t rotate(uint8_t motor, int32_t velocity) {
	tmc5130_rotate(motorToIC(motor), velocity);

	return 0;
}

static uint32_t right(uint8_t motor, int32_t velocity) {
	tmc5130_right(motorToIC(motor), velocity);

	return 0;
}

static uint32_t left(uint8_t motor, int32_t velocity) {
	tmc5130_left(motorToIC(motor), velocity);

	return 0;
}

static uint32_t stop(uint8_t motor) {
	tmc5130_stop(motorToIC(motor));

	return 0;
}

static uint32_t moveTo(uint8_t motor, int32_t position) {
	tmc5130_moveTo(motorToIC(motor), position, vmax_position);

	return 0;
}

static uint32_t moveBy(uint8_t motor, int32_t *ticks) {
	tmc5130_moveBy(motorToIC(motor), ticks, vmax_position);

	return 0;
}
// <= Functions forwarded to API

static uint32_t handleParameter(uint8_t readWrite, uint8_t motor, uint8_t type, int32_t *value)
{
	uint32_t buffer;
	uint32_t errors = TMC_ERROR_NONE;

	if (motor >= TMC5130_MOTORS)
		return TMC_ERROR_MOTOR;

	switch (type) {
	case 0:
		// Target position
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_XTARGET);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_XTARGET, *value);
		}
		break;
	case 1:
		// Actual position
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_XACTUAL);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_XACTUAL, *value);
		}
		break;
	case 2:
		// Target speed
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_VMAX);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_VMAX, *value);
		}
		break;
	case 3:
		// Actual speed
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_VACTUAL);
			*value = CAST_Sn_TO_S32(*value, 24);
		} else if (readWrite == WRITE) {
			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 4:
		// Maximum speed
		if (readWrite == READ) {
			*value = vmax_position;
		} else if (readWrite == WRITE) {
			vmax_position = abs(*value);
			if (tmc5130_readInt(motorToIC(motor),
					TMC5130_RAMPMODE) == TMC5130_MODE_POSITION)
				tmc5130_writeInt(motorToIC(motor), TMC5130_VMAX, vmax_position);
		}
		break;
	case 5:
		// Maximum acceleration
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_AMAX);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_AMAX, *value);
		}
		break;
	case 6:
		// Maximum current
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_IHOLD_IRUN,
					TMC5130_IRUN_MASK, TMC5130_IRUN_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_IHOLD_IRUN,
					TMC5130_IRUN_MASK, TMC5130_IRUN_SHIFT, *value);
		}
		break;
	case 7:
		// Standby current
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_IHOLD_IRUN, TMC5130_IHOLD_MASK, TMC5130_IHOLD_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_IHOLD_IRUN, TMC5130_IHOLD_MASK, TMC5130_IHOLD_SHIFT, *value);
		}
		break;
	case 8:
		// Position reached flag
		if (readWrite == READ) {
			*value =
					(tmc5130_readInt(motorToIC(motor), TMC5130_RAMPSTAT)
							& TMC5130_RS_POSREACHED) ? 1 : 0;
		} else if (readWrite == WRITE) {
			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 10:
		// Right endstop
		if (readWrite == READ) {
			*value =
					(tmc5130_readInt(motorToIC(motor), TMC5130_RAMPSTAT)
							& TMC5130_RS_STOPR) ? 0 : 1;
		} else if (readWrite == WRITE) {
			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 11:
		// Left endstop
		if (readWrite == READ) {
			*value =
					(tmc5130_readInt(motorToIC(motor), TMC5130_RAMPSTAT)
							& TMC5130_RS_STOPL) ? 0 : 1;
		} else if (readWrite == WRITE) {
			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 12:
		// Automatic right stop
		if (readWrite == READ) {
			*value =
					(tmc5130_readInt(motorToIC(motor), TMC5130_SWMODE)
							& TMC5130_SW_STOPR_ENABLE) ? 1 : 0;
		} else if (readWrite == WRITE) {
			buffer = tmc5130_readInt(motorToIC(motor), TMC5130_SWMODE);
			if (*value == 0)
				tmc5130_writeInt(motorToIC(motor), TMC5130_SWMODE,
						buffer | TMC5130_SW_STOPR_ENABLE);
			else
				tmc5130_writeInt(motorToIC(motor), TMC5130_SWMODE,
						buffer & ~TMC5130_SW_STOPR_ENABLE);
		}
		break;
	case 13:
		// Automatic left stop
		if (readWrite == READ) {
			*value =
					(tmc5130_readInt(motorToIC(motor), TMC5130_SWMODE)
							& TMC5130_SW_STOPL_ENABLE) ? 1 : 0;
		} else if (readWrite == WRITE) {
			buffer = tmc5130_readInt(motorToIC(motor), TMC5130_SWMODE);
			if (*value == 0)
				tmc5130_writeInt(motorToIC(motor), TMC5130_SWMODE,
						buffer | TMC5130_SW_STOPL_ENABLE);
			else
				tmc5130_writeInt(motorToIC(motor), TMC5130_SWMODE,
						buffer & ~TMC5130_SW_STOPL_ENABLE);
		}
		break;
	case 14:
		// SW_MODE Register
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_SWMODE);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_SWMODE, *value);
		}
		break;
	case 15:
		// Acceleration A1
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_A1);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_A1, *value);
		}
		break;
	case 16:
		// Velocity V1
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_V1);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_V1, *value);
		}
		break;
	case 17:
		// Maximum Deceleration
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_DMAX);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_DMAX, *value);
		}
		break;
	case 18:
		// Deceleration D1
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_D1);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_D1, *value);
		}
		break;
	case 19:
		// Velocity VSTART
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_VSTART);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_VSTART, *value);
		}
		break;
	case 20:
		// Velocity VSTOP
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_VSTOP);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_VSTOP, *value);
		}
		break;
	case 21:
		// Waiting time after ramp down
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_TZEROWAIT);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_TZEROWAIT, *value);
		}
		break;

	case 23:
		// Speed threshold for high speed mode
		if (readWrite == READ) {
			buffer = tmc5130_readInt(motorToIC(motor), TMC5130_THIGH);
			*value = MIN(0xFFFFF, (1 << 24) / ((buffer) ? buffer : 1));
		} else if (readWrite == WRITE) {
			*value = MIN(0xFFFFF, (1 << 24) / ((*value) ? *value : 1));
			tmc5130_writeInt(motorToIC(motor), TMC5130_THIGH, *value);
		}
		break;
	case 24:
		// Minimum speed for switching to dcStep
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_VDCMIN);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_VDCMIN, *value);
		}
		break;
	case 27:
		// High speed chopper mode
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_VHIGHCHM_MASK, TMC5130_VHIGHCHM_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_VHIGHCHM_MASK, TMC5130_VHIGHCHM_SHIFT, *value);
		}
		break;
	case 28:
		// High speed fullstep mode
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_VHIGHFS_MASK, TMC5130_VHIGHFS_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_VHIGHFS_MASK, TMC5130_VHIGHFS_SHIFT, *value);
		}
		break;
	case 29:
		// Measured Speed
		if (readWrite == READ) {
			*value = motorToIC(motor)->velocity;
		} else if (readWrite == WRITE) {
			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 33:
		// Analog I Scale
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_GCONF,
					TMC5130_I_SCALE_ANALOG_MASK, TMC5130_I_SCALE_ANALOG_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_GCONF,
					TMC5130_I_SCALE_ANALOG_MASK, TMC5130_I_SCALE_ANALOG_SHIFT,
					*value);
		}
		break;
	case 34:
		// Internal RSense
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_GCONF,
					TMC5130_INTERNAL_RSENSE_MASK,
					TMC5130_INTERNAL_RSENSE_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_GCONF,
					TMC5130_INTERNAL_RSENSE_MASK, TMC5130_INTERNAL_RSENSE_SHIFT,
					*value);
		}
		break;
	case 140:
		// Microstep Resolution
		if (readWrite == READ) {
			*value = 256
					>> TMC5130_FIELD_READ(motorToIC(motor), TMC5130_CHOPCONF,
							TMC5130_MRES_MASK, TMC5130_MRES_SHIFT);
		} else if (readWrite == WRITE) {
			switch (*value) {
			case 1:
				*value = 8;
				break;
			case 2:
				*value = 7;
				break;
			case 4:
				*value = 6;
				break;
			case 8:
				*value = 5;
				break;
			case 16:
				*value = 4;
				break;
			case 32:
				*value = 3;
				break;
			case 64:
				*value = 2;
				break;
			case 128:
				*value = 1;
				break;
			case 256:
				*value = 0;
				break;
			default:
				*value = -1;
				break;
			}

			if (*value != -1) {
				TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_CHOPCONF,
						TMC5130_MRES_MASK, TMC5130_MRES_SHIFT, *value);
			} else
				errors |= TMC_ERROR_VALUE;
		}
		break;
	case 162:
		// Chopper blank time
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_TBL_MASK, TMC5130_TBL_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_TBL_MASK, TMC5130_TBL_SHIFT, *value);
		}
		break;
	case 163:
		// Constant TOff Mode
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_CHM_MASK, TMC5130_CHM_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_CHM_MASK, TMC5130_CHM_SHIFT, *value);
		}
		break;
	case 164:
		// Disable fast decay comparator
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_DISFDCC_MASK, TMC5130_DISFDCC_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_DISFDCC_MASK, TMC5130_DISFDCC_SHIFT, *value);
		}
		break;
	case 165:
		// Chopper hysteresis end / fast decay time
		buffer = tmc5130_readInt(motorToIC(motor), TMC5130_CHOPCONF);
		if (readWrite == READ) {
			if (buffer & (1 << 14)) {
				*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_CHOPCONF,
						TMC5130_HEND_MASK, TMC5130_HEND_SHIFT);
			} else {
				*value =
						((buffer >> 4) & 0x07) | (buffer & (1 << 11)) ?
								(1 << 3) : 0;
			}
		} else if (readWrite == WRITE) {
			if (buffer & (1 << 14)) {
				TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_CHOPCONF,
						TMC5130_HEND_MASK, TMC5130_HEND_SHIFT, *value);
			} else {
				if (*value & (1 << 3))
					buffer |= (0x01 << 11);
				else
					buffer &= ~(0x01 << 11);

				buffer &= ~(0x07 << 4);
				buffer |= (*value & 0x0F) << 4;

				tmc5130_writeInt(motorToIC(motor), TMC5130_CHOPCONF, buffer);
			}
		}
		break;
	case 166:
		// Chopper hysteresis start / sine wave offset
		buffer = tmc5130_readInt(motorToIC(motor), TMC5130_CHOPCONF);
		if (readWrite == READ) {
			if (buffer & (1 << 14)) {
				*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_CHOPCONF,
						TMC5130_HSTRT_MASK, TMC5130_HSTRT_SHIFT);
			} else {
				*value =
						((buffer >> 7) & 0x0F) | (buffer & (1 << 11)) ?
								1 << 3 : 0;
			}
		} else if (readWrite == WRITE) {
			if (tmc5130_readInt(motorToIC(motor), TMC5130_CHOPCONF)
					& (1 << 14)) {
				TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_CHOPCONF,
						TMC5130_HSTRT_MASK, TMC5130_HSTRT_SHIFT, *value);
			} else {
				TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_CHOPCONF,
						TMC5130_OFFSET_MASK, TMC5130_OFFSET_SHIFT, *value);
			}
		}
		break;
	case 167:
		// Chopper off time
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_TOFF_MASK, TMC5130_TOFF_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_TOFF_MASK, TMC5130_TOFF_SHIFT, *value);
		}
		break;
	case 168:
		// smartEnergy current minimum (SEIMIN)
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_COOLCONF,
					TMC5130_SEIMIN_MASK, TMC5130_SEIMIN_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_COOLCONF,
					TMC5130_SEIMIN_MASK, TMC5130_SEIMIN_SHIFT, *value);
		}
		break;
	case 169:
		// smartEnergy current down step
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_COOLCONF,
					TMC5130_SEDN_MASK, TMC5130_SEDN_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_COOLCONF,
					TMC5130_SEDN_MASK, TMC5130_SEDN_SHIFT, *value);
		}
		break;
	case 170:
		// smartEnergy hysteresis
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_COOLCONF,
					TMC5130_SEMAX_MASK, TMC5130_SEMAX_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_COOLCONF,
					TMC5130_SEMAX_MASK, TMC5130_SEMAX_SHIFT, *value);
		}
		break;
	case 171:
		// smartEnergy current up step
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_COOLCONF,
					TMC5130_SEUP_MASK, TMC5130_SEUP_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_COOLCONF,
					TMC5130_SEUP_MASK, TMC5130_SEUP_SHIFT, *value);
		}
		break;
	case 172:
		// smartEnergy hysteresis start
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_COOLCONF,
					TMC5130_SEMIN_MASK, TMC5130_SEMIN_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_COOLCONF,
					TMC5130_SEMIN_MASK, TMC5130_SEMIN_SHIFT, *value);
		}
		break;
	case 173:
		// stallGuard2 filter enable
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_COOLCONF,
					TMC5130_SFILT_MASK, TMC5130_SFILT_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_COOLCONF,
					TMC5130_SFILT_MASK, TMC5130_SFILT_SHIFT, *value);
		}
		break;
	case 174:
		// stallGuard2 threshold
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_COOLCONF,
					TMC5130_SGT_MASK, TMC5130_SGT_SHIFT);
			*value = CAST_Sn_TO_S32(*value, 7);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_COOLCONF,
					TMC5130_SGT_MASK, TMC5130_SGT_SHIFT, *value);
		}
		break;
	case 179:
		// VSense
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_VSENSE_MASK, TMC5130_VSENSE_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_VSENSE_MASK, TMC5130_VSENSE_SHIFT, *value);
		}
		break;
	case 180:
		// smartEnergy actual current
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_DRVSTATUS,
					TMC5130_CS_ACTUAL_MASK, TMC5130_CS_ACTUAL_SHIFT);
		} else if (readWrite == WRITE) {
			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 181:
		// smartEnergy stall velocity
		//this function sort of doubles with 182 but is necessary to allow cross chip compliance
		if (readWrite == READ) {
			if (TMC5130_FIELD_READ(motorToIC(motor), TMC5130_SWMODE,
					TMC5130_SG_STOP_MASK, TMC5130_SG_STOP_SHIFT)) {
				buffer = tmc5130_readInt(motorToIC(motor), TMC5130_TCOOLTHRS);
				*value = MIN(0xFFFFF, (1 << 24) / ((buffer) ? buffer : 1));
			} else
				*value = 0;
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_SWMODE,
					TMC5130_SG_STOP_MASK, TMC5130_SG_STOP_SHIFT,
					(*value) ? 1 : 0);
			*value = MIN(0xFFFFF, (1 << 24) / ((*value) ? *value : 1));
			tmc5130_writeInt(motorToIC(motor), TMC5130_TCOOLTHRS, *value);
		}
		break;
	case 182:
		// smartEnergy threshold speed
		if (readWrite == READ) {
			buffer = tmc5130_readInt(motorToIC(motor), TMC5130_TCOOLTHRS);
			*value = MIN(0xFFFFF, (1 << 24) / ((buffer) ? buffer : 1));
		} else if (readWrite == WRITE) {
			buffer = MIN(0xFFFFF, (1 << 24) / ((*value) ? *value : 1));
			tmc5130_writeInt(motorToIC(motor), TMC5130_TCOOLTHRS, buffer);
		}
		break;
	case 184:
		// Random TOff mode
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_RNDTF_MASK, TMC5130_RNDTF_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_RNDTF_MASK, TMC5130_RNDTF_SHIFT, *value);
		}
		break;
	case 185:
		// Chopper synchronization
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_SYNC_MASK, TMC5130_SYNC_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_CHOPCONF,
					TMC5130_SYNC_MASK, TMC5130_SYNC_SHIFT, *value);
		}
		break;
	case 186:
		// PWM threshold speed
		if (readWrite == READ) {
			buffer = tmc5130_readInt(motorToIC(motor), TMC5130_TPWMTHRS);
			*value = MIN(0xFFFFF, (1 << 24) / ((buffer) ? buffer : 1));
		} else if (readWrite == WRITE) {
			*value = MIN(0xFFFFF, (1 << 24) / ((*value) ? *value : 1));
			tmc5130_writeInt(motorToIC(motor), TMC5130_TPWMTHRS, *value);
		}
		break;
	case 187:
		// PWM gradient
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_PWMCONF,
					TMC5130_PWM_GRAD_MASK, TMC5130_PWM_GRAD_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_PWMCONF,
					TMC5130_PWM_GRAD_MASK, TMC5130_PWM_GRAD_SHIFT, *value);
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_GCONF,
					TMC5130_EN_PWM_MODE_MASK, TMC5130_EN_PWM_MODE_SHIFT,
					(*value) ? 1 : 0);
		}
		break;
	case 188:
		// PWM amplitude
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_PWMCONF,
					TMC5130_PWM_AMPL_MASK, TMC5130_PWM_AMPL_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_PWMCONF,
					TMC5130_PWM_AMPL_MASK, TMC5130_PWM_AMPL_SHIFT, *value);
		}
		break;
	case 191:
		// PWM frequency
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_PWMCONF,
					TMC5130_PWM_FREQ_MASK, TMC5130_PWM_FREQ_SHIFT);
		} else if (readWrite == WRITE) {
			if (*value >= 0 && *value < 4) {
				TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_PWMCONF,
						TMC5130_PWM_FREQ_MASK, TMC5130_PWM_FREQ_SHIFT, *value);
			} else {
				errors |= TMC_ERROR_VALUE;
			}
		}
		break;
	case 192:
		// PWM autoscale
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_PWMCONF,
					TMC5130_PWM_AUTOSCALE_MASK, TMC5130_PWM_AUTOSCALE_SHIFT);
		} else if (readWrite == WRITE) {
			if (*value >= 0 && *value < 2) {
				TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_PWMCONF,
						TMC5130_PWM_AUTOSCALE_MASK, TMC5130_PWM_AUTOSCALE_SHIFT,
						*value);
			} else
				errors |= TMC_ERROR_VALUE;
		}
		break;
	case 204:
		// Freewheeling mode
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_PWMCONF,
					TMC5130_FREEWHEEL_MASK, TMC5130_FREEWHEEL_SHIFT);
		} else if (readWrite == WRITE) {
			TMC5130_FIELD_WRITE(motorToIC(motor), TMC5130_PWMCONF,
					TMC5130_FREEWHEEL_MASK, TMC5130_FREEWHEEL_SHIFT, *value);
		}
		break;
	case 206:
		// Load value
		if (readWrite == READ) {
			*value = TMC5130_FIELD_READ(motorToIC(motor), TMC5130_DRVSTATUS,
					TMC5130_SG_RESULT_MASK, TMC5130_SG_RESULT_SHIFT);
		} else if (readWrite == WRITE) {
			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 209:
		// Encoder position
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_XENC);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_XENC, *value);
		}
		break;
	case 210:
		// Encoder Resolution
		if (readWrite == READ) {
			*value = tmc5130_readInt(motorToIC(motor), TMC5130_ENC_CONST);
		} else if (readWrite == WRITE) {
			tmc5130_writeInt(motorToIC(motor), TMC5130_ENC_CONST, *value);
		}
		break;
	default:
		errors |= TMC_ERROR_TYPE;
		break;
	}
	return errors;
}

static uint32_t SAP(uint8_t type, uint8_t motor, int32_t value) {
	return handleParameter(WRITE, motor, type, &value);
}

static uint32_t GAP(uint8_t type, uint8_t motor, int32_t *value) {
	return handleParameter(READ, motor, type, value);
}

static uint32_t getMeasuredSpeed(uint8_t motor, int32_t *value) {
	if (motor >= TMC5130_MOTORS)
		return TMC_ERROR_MOTOR;

	*value = TMC5130.velocity;
	return TMC_ERROR_NONE;
}

static void writeRegister(uint8_t motor, uint8_t address, int32_t value) {
	UNUSED(motor);
	tmc5130_writeInt(&TMC5130, address, value);
}

static void readRegister(uint8_t motor, uint8_t address, int32_t *value) {
	UNUSED(motor);
	*value = tmc5130_readInt(&TMC5130, address);
}

static void periodicJob(tick_t tick)
{
	tmc5130_periodicJob(&TMC5130, tick);
#if 0
	if(tickRead() > 10000)
		tmc_gpio_write(LED2, ON);
#endif
}

static void checkErrors(tick_t tick) {
	UNUSED(tick);
	Evalboards.ch1.errors = 0;
}

static uint32_t userFunction(uint8_t type, uint8_t motor, int32_t *value)
{
	uint32_t errors = 0;
	errors |= TMC_ERROR_TYPE;

	UNUSED(motor);

	return errors;
}

static void deInit(void)
{
	tmc_gpio_write(pins.DRV_ENN, OFF);
}
;

static uint8_t reset() {
	if (!tmc5130_readInt(&TMC5130, TMC5130_VACTUAL))
		tmc5130_reset(&TMC5130);

	tmc_gpio_write(pins.AIN_REF_SW, OFF);
	return 1;
}

static uint8_t restore() {
	return tmc5130_restore(&TMC5130);
}

static void configCallback(TMC5130TypeDef *tmc5130, ConfigState state) {
	if (state == CONFIG_RESET) {	// Change hardware-preset registers here
		tmc5130_writeInt(tmc5130, TMC5130_PWMCONF, 0x000504C8);

		// Fill missing shadow registers (hardware preset registers)
		tmc5130_fillShadowRegisters(&TMC5130);
	}

}

static void enableDriver(DriverState state) {
	if (state == DRIVER_USE_GLOBAL_ENABLE)
		state = Evalboards.driverEnable;

	if (state == DRIVER_DISABLE)
		tmc_gpio_write(pins.DRV_ENN, ON);
	else if ((state == DRIVER_ENABLE) && (Evalboards.driverEnable == DRIVER_ENABLE))
		tmc_gpio_write(pins.DRV_ENN, OFF);
}

void TMC5130_init(void)
{
	tmc5130_init(&TMC5130, 0, Evalboards.ch1.config, &tmc5130_defaultRegisterResetState[0]);
	tmc5130_setCallback(&TMC5130, configCallback);

	pins.DRV_ENN 		= PORT_GPIO_DRV_ENN;
	pins.AIN_REF_SW 	= PORT_GPIO_AIN_REF_SW;
	pins.SWSEL 			= PORT_GPIO_SWSEL;
	pins.AIN_REF_PWM 	= PORT_GPIO_AIN_REF_PWM;

	tmc_gpio_config(pins.DRV_ENN, PORT_GPIO_CONFIG_OUTPUT);
	tmc_gpio_config(pins.AIN_REF_SW, PORT_GPIO_CONFIG_OUTPUT);
	tmc_gpio_config(pins.SWSEL, PORT_GPIO_CONFIG_OUTPUT);
	tmc_gpio_config(pins.AIN_REF_PWM, PORT_GPIO_CONFIG_OUTPUT);

	tmc_gpio_write(pins.SWSEL, OFF);

	Evalboards.ch1.config->reset = reset;
	Evalboards.ch1.config->restore = restore;
	Evalboards.ch1.config->state = CONFIG_RESET;
	Evalboards.ch1.config->configIndex = 0;

	Evalboards.ch1.rotate = rotate;
	Evalboards.ch1.right = right;
	Evalboards.ch1.left = left;
	Evalboards.ch1.stop = stop;
	Evalboards.ch1.GAP = GAP;
	Evalboards.ch1.SAP = SAP;
	Evalboards.ch1.moveTo = moveTo;
	Evalboards.ch1.moveBy = moveBy;
	Evalboards.ch1.writeRegister = writeRegister;
	Evalboards.ch1.readRegister = readRegister;
	Evalboards.ch1.periodicJob = periodicJob;
	Evalboards.ch1.userFunction = userFunction;
	Evalboards.ch1.getMeasuredSpeed = getMeasuredSpeed;
	Evalboards.ch1.enableDriver = enableDriver;
	Evalboards.ch1.checkErrors = checkErrors;
	Evalboards.ch1.numberOfMotors = TMC5130_MOTORS;
	Evalboards.ch1.VMMin = VM_MIN;
	Evalboards.ch1.VMMax = VM_MAX;
	Evalboards.ch1.deInit = deInit;

	vmax_position = TMC5130.config->shadowRegister[TMC5130_VMAX];

	enableDriver(DRIVER_USE_GLOBAL_ENABLE);

	tmc_gpio_write(pins.AIN_REF_PWM, OFF);

	tmc_gpio_write(LED3, ON);
}

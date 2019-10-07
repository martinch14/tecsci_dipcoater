/*
 ===============================================================================
 Name        : tmc_eval.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#include "chip.h"
#include <cr_section_macros.h>

#include "main.h"
#include "sapi_tick.h"
//#include "spi.h"
#include "Board.h"
#include "TMCMotionController.h"
#include "TMC4671.h"
#include "TMC5130.h"
#include "SysTick.h"

extern void TMC5130_init(void);
void MotorSetup(void);

void init(void) {
	SystemCoreClockUpdate();
	tickInit(1);

	// Inicializar GPIOs
	gpioConfig(0, GPIO_ENABLE);

	// Configuracion de pines de entrada para Teclas de la EDU-CIAA-NXP
	gpioConfig(TEC1, GPIO_INPUT);
	gpioConfig(TEC2, GPIO_INPUT);
	gpioConfig(TEC3, GPIO_INPUT);
	gpioConfig(TEC4, GPIO_INPUT);

	// Configuracion de pines de salida para Leds de la EDU-CIAA-NXP
	gpioConfig(LEDR, GPIO_OUTPUT);
	gpioConfig(LEDG, GPIO_OUTPUT);
	gpioConfig(LEDB, GPIO_OUTPUT);
	gpioConfig(LED1, GPIO_OUTPUT);
	gpioConfig(LED2, GPIO_OUTPUT);
	gpioConfig(LED3, GPIO_OUTPUT);
}

void deinit(void) {

}

void reset(void) {
	deinit();
	main();
}

int main(void) {
	init();

	tmc_spi_init();
	tmcmotioncontroller_init();
	TMC5130_init();

	Evalboards.driverEnable = DRIVER_ENABLE;
	Evalboards.ch1.enableDriver(DRIVER_DISABLE);

	while (1) {
		Evalboards.ch1.periodicJob(tickRead());

		if (0 == gpioRead(TEC1)) {
			gpioWrite(LED1, ON);
			Evalboards.ch1.enableDriver(DRIVER_ENABLE);
			MotorSetup();
		}

		if (0 == gpioRead(TEC2)) {
			gpioWrite(LED1, OFF);
			Evalboards.ch1.enableDriver(DRIVER_DISABLE);
		}
	}

	return 0;
}

void MotorSetup(void)
{
	Evalboards.ch1.writeRegister(0, 0x00, 0x00002680); // writing value 0x00002680 = 9856 = 0.0 to address 0 = 0x00(GCONF)
	Evalboards.ch1.writeRegister(0, 0x03, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 1 = 0x03(SLAVECONF)
	Evalboards.ch1.writeRegister(0, 0x05, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 2 = 0x05(X_COMPARE)
	Evalboards.ch1.writeRegister(0, 0x10, 0x00070A03); // writing value 0x00070A03 = 461315 = 0.0 to address 3 = 0x10(IHOLD_IRUN)
	Evalboards.ch1.writeRegister(0, 0x11, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 4 = 0x11(TPOWERDOWN)
	Evalboards.ch1.writeRegister(0, 0x13, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 5 = 0x13(TPWMTHRS)
	Evalboards.ch1.writeRegister(0, 0x14, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 6 = 0x14(TCOOLTHRS)
	Evalboards.ch1.writeRegister(0, 0x15, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 7 = 0x15(THIGH)
	Evalboards.ch1.writeRegister(0, 0x20, 0x00000001); // writing value 0x00000001 = 1 = 0.0 to address 8 = 0x20(RAMPMODE)
	Evalboards.ch1.writeRegister(0, 0x21, 0xFFED8910); // writing value 0xFFED8910 = 0 = 0.0 to address 9 = 0x21(XACTUAL)
	Evalboards.ch1.writeRegister(0, 0x23, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 10 = 0x23(VSTART)
	Evalboards.ch1.writeRegister(0, 0x24, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 11 = 0x24(A1)
	Evalboards.ch1.writeRegister(0, 0x25, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 12 = 0x25(V1)
	Evalboards.ch1.writeRegister(0, 0x26, 0x00001388); // writing value 0x00001388 = 5000 = 0.0 to address 13 = 0x26(AMAX)
	Evalboards.ch1.writeRegister(0, 0x27, 0x00002710); // writing value 0x00002710 = 10000 = 0.0 to address 14 = 0x27(VMAX)
	Evalboards.ch1.writeRegister(0, 0x28, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 15 = 0x28(DMAX)
	Evalboards.ch1.writeRegister(0, 0x2A, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 16 = 0x2A(D1)
	Evalboards.ch1.writeRegister(0, 0x2B, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 17 = 0x2B(VSTOP)
	Evalboards.ch1.writeRegister(0, 0x2C, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)
	Evalboards.ch1.writeRegister(0, 0x2D, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 19 = 0x2D(XTARGET)
	Evalboards.ch1.writeRegister(0, 0x33, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 20 = 0x33(VDCMIN)
	Evalboards.ch1.writeRegister(0, 0x34, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 21 = 0x34(SW_MODE)
	Evalboards.ch1.writeRegister(0, 0x38, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 22 = 0x38(ENCMODE)
	Evalboards.ch1.writeRegister(0, 0x39, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 23 = 0x39(X_ENC)
	Evalboards.ch1.writeRegister(0, 0x3A, 0x00010000); // writing value 0x00010000 = 65536 = 0.0 to address 24 = 0x3A(ENC_CONST)
	Evalboards.ch1.writeRegister(0, 0x60, 0xAAAAB554); // writing value 0xAAAAB554 = 0 = 0.0 to address 25 = 0x60(MSLUT[0])
	Evalboards.ch1.writeRegister(0, 0x61, 0x4A9554AA); // writing value 0x4A9554AA = 1251300522 = 0.0 to address 26 = 0x61(MSLUT[1])
	Evalboards.ch1.writeRegister(0, 0x62, 0x24492929); // writing value 0x24492929 = 608774441 = 0.0 to address 27 = 0x62(MSLUT[2])
	Evalboards.ch1.writeRegister(0, 0x63, 0x10104222); // writing value 0x10104222 = 269500962 = 0.0 to address 28 = 0x63(MSLUT[3])
	Evalboards.ch1.writeRegister(0, 0x64, 0xFBFFFFFF); // writing value 0xFBFFFFFF = 0 = 0.0 to address 29 = 0x64(MSLUT[4])
	Evalboards.ch1.writeRegister(0, 0x65, 0xB5BB777D); // writing value 0xB5BB777D = 0 = 0.0 to address 30 = 0x65(MSLUT[5])
	Evalboards.ch1.writeRegister(0, 0x66, 0x49295556); // writing value 0x49295556 = 1227445590 = 0.0 to address 31 = 0x66(MSLUT[6])
	Evalboards.ch1.writeRegister(0, 0x67, 0x00404222); // writing value 0x00404222 = 4211234 = 0.0 to address 32 = 0x67(MSLUT[7])
	Evalboards.ch1.writeRegister(0, 0x68, 0xFFFF8056); // writing value 0xFFFF8056 = 0 = 0.0 to address 33 = 0x68(MSLUTSEL)
	Evalboards.ch1.writeRegister(0, 0x69, 0x00F70000); // writing value 0x00F70000 = 16187392 = 0.0 to address 34 = 0x69(MSLUTSTART)
	Evalboards.ch1.writeRegister(0, 0x6C, 0x000101D5); // writing value 0x000101D5 = 66005 = 0.0 to address 35 = 0x6C(CHOPCONF)
	Evalboards.ch1.writeRegister(0, 0x6D, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 36 = 0x6D(COOLCONF)
	Evalboards.ch1.writeRegister(0, 0x6E, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 37 = 0x6E(DCCTRL)
	Evalboards.ch1.writeRegister(0, 0x70, 0x000504C8); // writing value 0x000504C8 = 328904 = 0.0 to address 38 = 0x70(PWMCONF)
	Evalboards.ch1.writeRegister(0, 0x72, 0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 39 = 0x72(ENCM_CTRL)
}

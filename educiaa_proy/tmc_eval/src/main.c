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
#include "Board.h"
#include "TMCMotionController.h"
#include "spi.h"
#include "TMC4671.h"
#include "SysTick.h"
//#include "../../lpc_board_nxp_lpcxpresso_4337/inc/board.h"


/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

#if defined(TMC5130_eval)
extern void TMC5130_init(void);
#elif defined(TMC4671_eval)
extern void TMC4671_init(void);
#endif

/*==================[internal functions definition]==========================*/

void init(void)
{


	SystemCoreClockUpdate();

	tickInit(1);

	spi_init();
//	usb_init();

	spiConfig(SPI0);
	tmcmotioncontroller_init();

//#if defined(TMC5130_eval)
//	TMC5130_init();
//#elif defined(TMC4671_eval)
	TMC4671_init();
//#endif

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

	// Configuracion de pines de entrada de la CIAA-NXP
	gpioConfig(DI0, GPIO_INPUT);
	gpioConfig(DI1, GPIO_INPUT);
	gpioConfig(DI2, GPIO_INPUT);
	gpioConfig(DI3, GPIO_INPUT);
	gpioConfig(DI4, GPIO_INPUT);
	gpioConfig(DI5, GPIO_INPUT);
	gpioConfig(DI6, GPIO_INPUT);
	gpioConfig(DI7, GPIO_INPUT);

	// Configuracion de pines de salida de la CIAA-NXP
	gpioConfig(DO0, GPIO_OUTPUT);
	gpioConfig(DO1, GPIO_OUTPUT);
	gpioConfig(DO2, GPIO_OUTPUT);
	gpioConfig(DO3, GPIO_OUTPUT);
	gpioConfig(DO4, GPIO_OUTPUT);
	gpioConfig(DO5, GPIO_OUTPUT);
	gpioConfig(DO6, GPIO_OUTPUT);
	gpioConfig(DO7, GPIO_OUTPUT);





}

void deinit(void)
{

}

void reset(void)
{ // May does not work this way, IRQ does not get reset.
	deinit();
	main();
}

/*==================[external functions definition]==========================*/

int main(void) {
	static tick_t old_tick = 0;
	init();
//	tmcl_init();

	Evalboards.driverEnable = DRIVER_ENABLE;
	Evalboards.ch1.enableDriver(DRIVER_USE_GLOBAL_ENABLE);

	while (1) {
		//usb_periodicJob();
//		tmcl_process();
//		Evalboards.ch1.periodicJob(tickRead());
		Evalboards.ch1.periodicJob(tickRead());
		if (0 == gpioRead(TEC1)) {

			gpioWrite(LED1,ON);
			// Motor type &  PWM configuration
			Evalboards.ch1.writeRegister(0, TMC4671_MOTOR_TYPE_N_POLE_PAIRS, 0x00030004);
			Evalboards.ch1.writeRegister(0, TMC4671_PWM_POLARITIES, 0x00000000);
			Evalboards.ch1.writeRegister(0, TMC4671_PWM_MAXCNT, 0x00000F9F);
			Evalboards.ch1.writeRegister(0, TMC4671_PWM_BBM_H_BBM_L, 0x00000505);
			Evalboards.ch1.writeRegister(0, TMC4671_PWM_SV_CHOP, 0x00000007);

			// ADC configuration
			Evalboards.ch1.writeRegister(0, TMC4671_ADC_I_SELECT, 0x18000100);
			Evalboards.ch1.writeRegister(0, TMC4671_dsADC_MCFG_B_MCFG_A, 0x00100010);
			Evalboards.ch1.writeRegister(0, TMC4671_dsADC_MCLK_A, 0x20000000);
			Evalboards.ch1.writeRegister(0, TMC4671_dsADC_MCLK_B, 0x00000000);
			Evalboards.ch1.writeRegister(0, TMC4671_dsADC_MDEC_B_MDEC_A, 0x014E014E);
			Evalboards.ch1.writeRegister(0, TMC4671_ADC_I0_SCALE_OFFSET, 0x0100825D);
			Evalboards.ch1.writeRegister(0, TMC4671_ADC_I1_SCALE_OFFSET, 0x010082CE);

			// ABN encoder settings
			Evalboards.ch1.writeRegister(0, TMC4671_ABN_DECODER_MODE, 0x00001000);
			Evalboards.ch1.writeRegister(0, TMC4671_ABN_DECODER_PPR, 0x00004000);
			Evalboards.ch1.writeRegister(0, TMC4671_ABN_DECODER_COUNT, 0x000017F0);
			Evalboards.ch1.writeRegister(0, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, 0x00000000);

			// Limits
			Evalboards.ch1.writeRegister(0, TMC4671_PID_TORQUE_FLUX_LIMITS, 0x000003E8);

			// PI settings
			Evalboards.ch1.writeRegister(0, TMC4671_PID_TORQUE_P_TORQUE_I, 0x01000100);
			Evalboards.ch1.writeRegister(0, TMC4671_PID_FLUX_P_FLUX_I, 0x01000100);


			// Init encoder (mode 0)
			Evalboards.ch1.writeRegister(0, TMC4671_MODE_RAMP_MODE_MOTION, 0x00000008);
			Evalboards.ch1.writeRegister(0, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, 0x00000000);
			Evalboards.ch1.writeRegister(0, TMC4671_PHI_E_SELECTION, 0x00000001);
			Evalboards.ch1.writeRegister(0, TMC4671_PHI_E_EXT, 0x00000000);
			Evalboards.ch1.writeRegister(0, TMC4671_UQ_UD_EXT, 0x000007D0);
			Evalboards.ch1.writeRegister(0, TMC4671_ABN_DECODER_COUNT, 0x00000000);

			// Feedback selection
			Evalboards.ch1.writeRegister(0, TMC4671_PHI_E_SELECTION, 0x00000003);
			Evalboards.ch1.writeRegister(0, TMC4671_VELOCITY_SELECTION, 0x00000009);

			// Switch to torque mode
			Evalboards.ch1.writeRegister(0, TMC4671_MODE_RAMP_MODE_MOTION, 0x00000001);

		}
		if (0 == gpioRead(TEC2)) {

			Evalboards.ch1.writeRegister(0, TMC4671_PID_TORQUE_FLUX_TARGET, 0UL);
		}
		if (0 == gpioRead(TEC3)) {

			Evalboards.ch1.writeRegister(0, TMC4671_PID_TORQUE_FLUX_TARGET, 0xFC180000);
//			tmc_wait(15000);
		}

//		if (0 == gpioRead(TEC4)) {
//
//			Evalboards.ch1.writeRegister(0, TMC4671_PID_TORQUE_FLUX_TARGET, 0x0000FFFF);
////			tmc_wait(15000);
//		}



		old_tick = tickRead();
		while (old_tick == tickRead());
	}

	return 0;
}

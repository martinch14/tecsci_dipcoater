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
	tickInit(1);
	SystemCoreClockUpdate();
	spi_init();
	usb_init();

	spiConfig(SPI0);
	tmcmotioncontroller_init();

//#if defined(TMC5130_eval)
//	TMC5130_init();
//#elif defined(TMC4671_eval)
	TMC4671_init();
//#endif


	//Configuración:

	// Motor type &  PWM configuration
	//tmc4671_writeInt(0, TMC4671_MOTOR_TYPE_N_POLE_PAIRS, 0x00030004);
	Evalboards.ch1.writeRegister(0, TMC4671_MOTOR_TYPE_N_POLE_PAIRS, 0x00030004);
	//tmc4671_writeInt(0, TMC4671_PWM_POLARITIES, 0x00000000);
	Evalboards.ch1.writeRegister(0, TMC4671_PWM_POLARITIES, 0x00000000);
	//tmc4671_writeInt(0, TMC4671_PWM_MAXCNT, 0x00000F9F);
	Evalboards.ch1.writeRegister(0, TMC4671_PWM_MAXCNT, 0x00000F9F);
	//tmc4671_writeInt(0, TMC4671_PWM_BBM_H_BBM_L, 0x00000505);
	Evalboards.ch1.writeRegister(0, TMC4671_PWM_BBM_H_BBM_L, 0x00000505);
	//tmc4671_writeInt(0, TMC4671_PWM_SV_CHOP, 0x00000007);
	Evalboards.ch1.writeRegister(0, TMC4671_PWM_SV_CHOP, 0x00000007);

	// ADC configuration
	//tmc4671_writeInt(0, TMC4671_ADC_I_SELECT, 0x18000100);
	Evalboards.ch1.writeRegister(0, TMC4671_ADC_I_SELECT, 0x18000100);
	//tmc4671_writeInt(0, TMC4671_dsADC_MCFG_B_MCFG_A, 0x00100010);
	Evalboards.ch1.writeRegister(0, TMC4671_dsADC_MCFG_B_MCFG_A, 0x00100010);
	//tmc4671_writeInt(0, TMC4671_dsADC_MCLK_A, 0x20000000);
	Evalboards.ch1.writeRegister(0, TMC4671_dsADC_MCLK_A, 0x20000000);
	//tmc4671_writeInt(0, TMC4671_dsADC_MCLK_B, 0x00000000);
	Evalboards.ch1.writeRegister(0, TMC4671_dsADC_MCLK_B, 0x00000000);
	//tmc4671_writeInt(0, TMC4671_dsADC_MDEC_B_MDEC_A, 0x014E014E);
	Evalboards.ch1.writeRegister(0, TMC4671_dsADC_MDEC_B_MDEC_A, 0x014E014E);
	//tmc4671_writeInt(0, TMC4671_ADC_I0_SCALE_OFFSET, 0x01008001);
	Evalboards.ch1.writeRegister(0, TMC4671_ADC_I0_SCALE_OFFSET, 0x01008001);
	//tmc4671_writeInt(0, TMC4671_ADC_I1_SCALE_OFFSET, 0x01008001);
	Evalboards.ch1.writeRegister(0, TMC4671_ADC_I1_SCALE_OFFSET, 0x01008001);

	// Open loop settings
	//tmc4671_writeInt(0, TMC4671_OPENLOOP_MODE, 0x00000000);
	Evalboards.ch1.writeRegister(0, TMC4671_OPENLOOP_MODE, 0x00000000);
	//tmc4671_writeInt(0, TMC4671_OPENLOOP_ACCELERATION, 0x0000003C);
	Evalboards.ch1.writeRegister(0, TMC4671_OPENLOOP_ACCELERATION, 0x0000003C);
	//tmc4671_writeInt(0, TMC4671_OPENLOOP_VELOCITY_TARGET, 0xFFFFFFF6);
	Evalboards.ch1.writeRegister(0, TMC4671_OPENLOOP_VELOCITY_TARGET, 0xFFFFFFF6);

	// Feedback selection
	//tmc4671_writeInt(0, TMC4671_PHI_E_SELECTION, 0x00000002);
	Evalboards.ch1.writeRegister(0, TMC4671_PHI_E_SELECTION, 0x00000002);
	//tmc4671_writeInt(0, TMC4671_UQ_UD_EXT, 0x00000830);
	Evalboards.ch1.writeRegister(0, TMC4671_UQ_UD_EXT, 0x00000830);


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

int main(void)
{
	static tick_t old_tick = 0;
	init();
//	tmcl_init();

	while (1)
	{
		//usb_periodicJob();
//		tmcl_process();
//		Evalboards.ch1.periodicJob(tickRead());


		tmc_wait(2000);
		// ===== Open loop test drive =====
		// Switch to open loop velocity mode
//		tmc4671_writeInt(0, TMC4671_MODE_RAMP_MODE_MOTION, 0x00000008);
		Evalboards.ch1.writeRegister(0, TMC4671_MODE_RAMP_MODE_MOTION, 0x00000008);

		// Rotate right
//		tmc4671_writeInt(0, TMC4671_OPENLOOP_VELOCITY_TARGET, 0x0000003C);
		Evalboards.ch1.writeRegister(0, TMC4671_OPENLOOP_VELOCITY_TARGET, 0x0000003C);
		tmc_wait(2000);

		// Rotate left
		//tmc4671_writeInt(0, TMC4671_OPENLOOP_VELOCITY_TARGET, 0xFFFFFFC4);
		Evalboards.ch1.writeRegister(0, TMC4671_OPENLOOP_VELOCITY_TARGET, 0xFFFFFFC4);
		tmc_wait(4000);

		// Stop
		//tmc4671_writeInt(0, TMC4671_OPENLOOP_VELOCITY_TARGET, 0x00000000);
		Evalboards.ch1.writeRegister(0, TMC4671_OPENLOOP_VELOCITY_TARGET, 0x00000000);
		tmc_wait(2000);
		//tmc4671_writeInt(0, TMC4671_UQ_UD_EXT, 0x00000000);
		Evalboards.ch1.writeRegister(0, TMC4671_UQ_UD_EXT, 0x00000000);

//		old_tick = tickRead();
//		while(old_tick == tickRead());
	}

	return 0;
}

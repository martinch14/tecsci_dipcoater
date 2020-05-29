/*
 * TMC_LinearRamp.c
 *
 *  Created on: 23.07.2018
 *      Author: ed
 *
 *  This is a basic proof-of-concept implementation of a linear motion ramp
 *  generator. It is designed to run with 1 calculation / ms.
 *
 *
 */
#include "LinearRamp.h"

void tmc_linearRamp_init(TMC_LinearRamp *linearRamp)
{
	linearRamp->maxVelocity     = 0;
	linearRamp->targetPosition  = 0;
	linearRamp->targetVelocity  = 0;
	linearRamp->rampVelocity    = 0;
	linearRamp->acceleration    = 0;
	linearRamp->encoderSteps	= u16_MAX;
	linearRamp->lastdVRest      = 0;
	linearRamp->lastdXRest      = 0;
	linearRamp->rampEnabled     = false;

	printf("Paso por funcion : Ramp_init\r\n");
}

void tmc_linearRamp_computeRampVelocity(TMC_LinearRamp *linearRamp) {

	if (linearRamp->rampEnabled) {
		// update target velocity according actual set acceleration
		// (scaling pre-factor of 1000 used for 1ms velocity ramp handling)

//		printf(
//				"Paso por funcion :  IF computeRampVelocity_init   Habilitada!\r\n");

		int dV = linearRamp->acceleration;

		// to ensure that small velocity changes at high set acceleration are also possible

		// Utilizo un pre factor de 100 y un velocity ramp handling de 10ms  - De esta manera de confirma la rampa a traves de cronometro
		// con un handling de 1ms por alguna razon se acelera mas rapido la cronometracion es erronae


		int maxDTV = abs(linearRamp->targetVelocity - linearRamp->rampVelocity);
		if (maxDTV < (dV/1000))
			dV = maxDTV*1000;

		dV =   dV +  linearRamp->lastdVRest;
		linearRamp->lastdVRest = dV % 100;

		if (linearRamp->rampVelocity < linearRamp->targetVelocity)
		{
			// accelerate motor
			linearRamp->rampVelocity += dV/100;	// divide with pre-factor
		}
		else if (linearRamp->rampVelocity > linearRamp->targetVelocity)
		{
			// decelerate motor
			linearRamp->rampVelocity -= dV/100;	// divide with pre-factor
		}

//		int dV = linearRamp->acceleration;
//
//		// to ensure that small velocity changes at high set acceleration are also possible
//		int maxDTV = abs(linearRamp->targetVelocity - linearRamp->rampVelocity);
//		if (maxDTV < (dV / 1000))
//			dV = maxDTV * 1000;
//
//		dV += linearRamp->lastdVRest;
//		linearRamp->lastdVRest = dV % 1;
//
//		if (linearRamp->rampVelocity < linearRamp->targetVelocity) {
//			// accelerate motor
//			linearRamp->rampVelocity += dV / 1;	// divide with pre-factor
//		} else if (linearRamp->rampVelocity > linearRamp->targetVelocity) {
//			// decelerate motor
//			linearRamp->rampVelocity -= dV / 1;	// divide with pre-factor
//		}

	} else {
		// use target velocity directly
		linearRamp->rampVelocity = linearRamp->targetVelocity;
//		printf(
//				"Paso por funcion  ELSE : computeRampVelocity_init   Habilitada!\r\n");

	}

	// limit ramp velocity
	linearRamp->rampVelocity = tmc_limitInt(linearRamp->rampVelocity,
			-linearRamp->maxVelocity, linearRamp->maxVelocity);

//	printf(
//			"Paso por funcion  FINAL : computeRampVelocity_init   Habilitada!\r\n");

}

void tmc_linearRamp_computeRampPosition(TMC_LinearRamp *linearRamp)
{
	if (linearRamp->rampEnabled)
	{
		// update target position according actual set acceleration and max velocity
		// (scaling pre-factor of 1000 used for 1ms position ramp handling)

		// limit position difference for further computations
		int32_t targetPositionsDifference = linearRamp->targetPosition-linearRamp->rampPosition;

		// limit the sqrti value in case of high position differences
		int64_t sqrtiValue = tmc_limitS64(((int64_t)120 * (int64_t)linearRamp->acceleration * (int64_t)(abs(targetPositionsDifference))) / (int64_t)linearRamp->encoderSteps, 0, (int64_t)linearRamp->maxVelocity*(int64_t)linearRamp->maxVelocity);

		// compute max allowed ramp velocity to ramp down to target
		int32_t maxRampStop = tmc_sqrti(sqrtiValue);

		// compute max allowed ramp velocity
		int32_t maxRampTargetVelocity = 0;
		if (targetPositionsDifference > 0)
		{
			maxRampTargetVelocity = tmc_limitInt(maxRampStop, 0, (int32_t)linearRamp->maxVelocity);
		}
		else if (targetPositionsDifference < 0)
		{
			maxRampTargetVelocity = tmc_limitInt(-maxRampStop, -(int32_t)linearRamp->maxVelocity, 0);
		}
		else
		{
			//maxRampTargetVelocity = 0;
		}

		int dV = linearRamp->acceleration;  // pre-factor ~ 1/1000

		// to ensure that small velocity changes at high set acceleration are also possible
		int maxDTV = abs(maxRampTargetVelocity - linearRamp->rampVelocity);
		if (maxDTV < (dV / 1000))
			dV = maxDTV * 1000;

		dV += linearRamp->lastdVRest;
		linearRamp->lastdVRest = dV % 1000;

		// do velocity ramping
		if (maxRampTargetVelocity > linearRamp->rampVelocity)
		{
			linearRamp->rampVelocity += dV / 1000;
		}
		else if (maxRampTargetVelocity < linearRamp->rampVelocity)
		{
			linearRamp->rampVelocity -= dV / 1000;
		}

		// limit positionRampTargetVelocity to maxRampTargetVelocity
		linearRamp->rampVelocity = tmc_limitInt(linearRamp->rampVelocity, -abs(maxRampTargetVelocity), abs(maxRampTargetVelocity));

		// do position ramping using actual ramp velocity to update dX
		int64_t dX = ((int64_t)linearRamp->rampVelocity * (int64_t)linearRamp->encoderSteps) / ((int64_t)60) + linearRamp->lastdXRest;

		// scale actual target position
		int64_t tempActualTargetPosition = (int64_t)linearRamp->rampPosition * 1000;

		// update actual target position
		tempActualTargetPosition += dX;

		if (tempActualTargetPosition >= 0)
			linearRamp->lastdXRest = (abs(tempActualTargetPosition) % 1000);
		else if (tempActualTargetPosition < 0)
			linearRamp->lastdXRest = -(abs(tempActualTargetPosition) % 1000);

		// scale actual target position back
		linearRamp->rampPosition = tempActualTargetPosition / 1000;
	}
	else
	{
		// use target position directly
		linearRamp->rampPosition = linearRamp->targetPosition;
	}
}

#include "enviromental_chamber_control.h"

//EMVIROMENTAL CHAMBER INIT FUNCTION

void EnviromentalChamberInit(enviromental_chamber_t *p_echamber){
	p_echamber->run_state=CHAMBER_STOPPED;
	p_echamber->targetRH=TARGET_RH_INIT_VALUE;
	p_echamber->targetTemp=TARGET_TEMP_INIT_VALUE;
	p_echamber->config.rh_activation=CHAMBER_RH_DEACTIVATED;
	p_echamber->config.temp_activation=CHAMBER_TEMP_DEACTIVATED;
}

void EnviromentalChamberSetRH(enviromental_chamber_t *p_echamber,uint8_t rh_to_set){
	//check_rh_value(rh_to_set);
	p_echamber->targetRH=rh_to_set;
}

void EnviromentalChamberSetTemp(enviromental_chamber_t *p_echamber,uint8_t temp_to_set){
	//check_rh_value(temp_to_set);
	p_echamber->targetTemp=temp_to_set;
}

void EnviromentalChamberActivateRH(enviromental_chamber_t *p_echamber){
	p_echamber->config.rh_activation=CHAMBER_RH_ACTIVATED;
}

void EnviromentalChamberDeactivateRH(enviromental_chamber_t *p_echamber){
	p_echamber->config.rh_activation=CHAMBER_RH_DEACTIVATED;
}

void EnviromentalChamberRun(enviromental_chamber_t *p_echamber){
	if(p_echamber->config.rh_activation == CHAMBER_RH_ACTIVATED ){
		//handler_control(p_echamber->targetRH,p_echamber->targetTemp);
		//printf("controlando RH\n");
	}
	if(p_echamber->config.rh_activation == CHAMBER_RH_DEACTIVATED ){

	}
}

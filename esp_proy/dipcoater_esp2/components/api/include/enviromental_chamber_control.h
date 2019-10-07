#ifndef COMPONENTS_API_INCLUDE_ENVIROMENTALCHAMBER_H_
#define COMPONENTS_API_INCLUDE_ENVIROMENTALCHAMBER_H_

#include <stdint.h>

/*DEFINED VALUES*/
#define _FLAG_ERROR          	1<< 8
#define TARGET_RH_INIT_VALUE 70 //%RH
#define TARGET_TEMP_INIT_VALUE 25 //°C

/*DEFINED STRUCTS*/

typedef enum{CHAMBER_STOPPED=0,CHAMBER_HUMIDIFYING=1,CHAMBER_DRYINGUP=2,CHAMBER_HEATING=3,CHAMBER_COOLING=4} chamberRunState_t;
typedef enum{CHAMBER_RH_DEACTIVATED=0,CHAMBER_TEMP_DEACTIVATED=1,CHAMBER_RH_ACTIVATED=2,CHAMBER_TEMP_ACTIVATED=3} chamberactivation_t;

typedef struct {
	chamberactivation_t rh_activation;
	chamberactivation_t temp_activation;
} chamberConfig_t;

typedef struct {
	uint8_t targetRH;
	uint8_t targetTemp;
	chamberRunState_t 	run_state;
	chamberConfig_t config;
} enviromental_chamber_t;

//FUNCTION DECLARATION
void EnviromentalChamberInit(enviromental_chamber_t *p_echamber);
void EnviromentalChamberSetRH(enviromental_chamber_t *p_echamber,uint8_t rh_to_set);
void EnviromentalChamberSetTemp(enviromental_chamber_t *p_echamber,uint8_t temp_to_set);
void EnviromentalChamberActivateRH(enviromental_chamber_t *p_echamber);
void EnviromentalChamberDeactivateRH(enviromental_chamber_t *p_echamber);
void EnviromentalChamberRun(enviromental_chamber_t *p_echamber);

#endif /* COMPONENTS_API_INCLUDE_PROCESS_H_ */

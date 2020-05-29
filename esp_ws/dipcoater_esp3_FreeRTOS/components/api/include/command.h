/**************************************************************************************************
**  (c) Copyright 2019: Martin Abel Gambarotta <magambarotta@gmail.com>
**  This file is part of DipCoater_Tecsci.
**
**  DipCoater_Tecsci is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  DipCoater_Tecsci is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with DipCoater_Tecsci.  If not, see <https://www.gnu.org/licenses/>.
*************************************************************************************************/


/** @file 	command.h
 ** @brief 	Declaracion de funciones relacionadas con los comandos recibidos
 **
 **| REV | YYYY.MM.DD | Autor           | Descripción de los cambios                              |
 **|-----|------------|-----------------|---------------------------------------------------------|
 **|   1 | 2020.05.28 | magambarotta    | Version inicial 									      |
 ** @addtogroup aplicacion
 ** @{ */


/*=====[Evitar inclusion multiple comienzo]==================================*/
#ifndef COMPONENTS_API_INCLUDE_COMMAND_H_
#define COMPONENTS_API_INCLUDE_COMMAND_H_


/* === Cabecera C++ ============================================================================ */
#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Función para configurar los tiempos y opciones de una puerta
 *
 * @param   control         Referencia al objeto control de puerta que se desea configurar
 * @param   configuracion   Referencia a la estructura con la configuracion del control de puerta
 * @return  true            El control de puerta se configuro correctamente con los nuevos valores
 * @return  false           No se pudo configurar el control de puerta porque alguno de los valores es erroneo
 */
/*Handler to LOAD the standard process */
void CommandLOADPROGRAMSTANDARDHandler(int argc, char **argv);



/*Handler to set up the LOOP COMMAND of the process standard
 1 arguments must be passed: N corresponding to the number of repetitions of the DWUW cycle  */
void CommandSETSTANDARDPROGRAMHandler(int argc, char **argv);
/*PROCESS CUSTOM HANDLERS*/



/**
 * @brief Función para configurar los tiempos y opciones de una puerta
 *
 * @param   control         Referencia al objeto control de puerta que se desea configurar
 * @param   configuracion   Referencia a la estructura con la configuracion del control de puerta
 * @return  true            El control de puerta se configuro correctamente con los nuevos valores
 * @return  false           No se pudo configurar el control de puerta porque alguno de los valores es erroneo
 */
/*Handler to LOAD the custom process,
 the process it should have been set up before (if there is no set up the standard parameters will be loaded)  */
void CommandLOADPROGRAMCUSTOMHandler(int argc, char **argv);
/*Handler to set up a single command of the custom Process
 4 arguments must be passed: command number (see manual), velocity, acceleration and the test parameter */
void CommandSETCOMMANDCUSTOMPROGRAMHandler(int argc, char **argv);/*Handler to set up a all commands of the custom Process
 30 arguments must be passed: the 3 parameters for each command in increasing order
 obs: for the LOOP comand it must be passed 3 parameters,
 but only the first one will be used the other two will be discarded
  */
void CommandSETALLCUSTOMPROGRAMHandler(int argc, char **argv);
/*PROCESS DINAMIC HANDLERS*/

void CommandLOADPROGRAMDINAMICHandler(int argc, char **argv);
/*Handler to add and set all the dinamic program
 * the arguments can be up to 129, in groups of 4:
 * 	comand number, velocity acceleration and test*/
void CommandADDSETALLCOMANDDINAMICHandler(int argc, char **argv);/*Handler to Clean all the dinamic program*/
void CommandCLEANPROGRAMDINAMICHandler(int argc, char **argv);/*RUN PROCESS HANDLER*/

/*Handler to RUn the LOADED process,
 * turn on the RUN flag to RUN*/
void CommandRUNHandler(int argc, char **argv);/*SINGLE MOVEMENT HANDLERS*/
/*this tiny handlers only call the corresponding process function*/
void CommandUPFASTHandler(int argc, char **argv);
void CommandUPHandler(int argc, char **argv);
void CommandUPSLOWHandler(int argc, char **argv);
void CommandDOWNFASTHandler(int argc, char **argv);
void CommandDOWNHandler(int argc, char **argv);
void CommandDOWNSLOWHandler(int argc, char **argv);
void CommandSTOPHandler(int argc, char **argv);
/*CONFIGURATION HANDLERS*/

void CommandREADDATAHandler(int argc, char **argv);


void CommandENA_DRIVERHandler(int argc, char **argv);
void CommandDIS_DRIVERHandler(int argc, char **argv);
void CommandCEROMACHINEHandler(int argc, char **argv);

void CommandCERO_SAMPLEHandler(int argc, char **argv);
void CommandDELTADIPHandler(int argc, char **argv);
void CommandRESETHandler(int argc, char **argv);

void CommandSETCOMMANDCUSTOMPROGRAMAPPHandler(int argc, char **argv);


/* === Ciere de documentacion ================================================================== */
#ifdef __cplusplus
}
#endif

/** @} Final de la definición del modulo para doxygen */

#endif /* COMPONENTS_API_INCLUDE_COMMAND_H_ */

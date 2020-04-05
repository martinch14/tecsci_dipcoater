/*
 * command.h
 *
 *  Created on: 7 oct. 2019
 *      Author: martin
 */

#ifndef COMPONENTS_API_INCLUDE_COMMAND_H_
#define COMPONENTS_API_INCLUDE_COMMAND_H_





/*Handler to RUn the LOADED process,
 * turn on the RUN flag to RUN*/
void CommandRUNHandler(int argc, char **argv);/*SINGLE MOVEMENT HANDLERS*/
/*this tiny handlers only call the corresponding process function*/
void CommandRIGHTFASTHandler(int argc, char **argv);
void CommandRIGHTHandler(int argc, char **argv);
void CommandRIGHTSLOWHandler(int argc, char **argv);
void CommandLEFTFASTHandler(int argc, char **argv);
void CommandLEFTHandler(int argc, char **argv);
void CommandLEFTSLOWHandler(int argc, char **argv);
void CommandSTOPHandler(int argc, char **argv);
/*CONFIGURATION HANDLERS*/

void CommandREADDATAHandler(int argc, char **argv);


void CommandSETCOMMANDCUSTOMPROGRAMAPPHandler(int argc, char **argv);


#endif /* COMPONENTS_API_INCLUDE_COMMAND_H_ */

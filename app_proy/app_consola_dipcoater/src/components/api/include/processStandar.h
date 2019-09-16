/*
 * processStandar.h
 *
 *  Created on: 16 sep. 2019
 *      Author: martin
 */


#ifndef COMPONENTS_API_INCLUDE_PROCESSSTANDAR_H_
#define COMPONENTS_API_INCLUDE_PROCESSSTANDAR_H_

#define MAX_ESTATIC_COMMAND 9


/*Definicion del Proceso Estático Comando Velocidad Aceleracion */
processCommand_t cmdProcesoEstandar2[MAX_ESTATIC_COMMAND] = {
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10 },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 },
		{ .commandnumber = PROCESS_COMMAND_UP, 			.argument.spin.velocity = 2,	.argument.spin.acceleration = 1 },
		{ .commandnumber = PROCESS_COMMAND_WAIT, 		.argument.spin.velocity = 0,	.argument.spin.acceleration = 1 },
		{ .commandnumber = PROCESS_COMMAND_DOWN, 		.argument.spin.velocity = -2,	.argument.spin.acceleration = 1 },
		{ .commandnumber = PROCESS_COMMAND_CERO_MACHINE,.argument.spin.velocity = 5,	.argument.spin.acceleration = 10 },

		/*Agregar comando LOOP */

};



///*Definicion del Proceso Dinámico - recibe parametros de configuracion desde el módulo de mensajería */
//processCommand_t cmd_dinamic[] = {
//		{ .commandnumber = PROCESS_COMMAND_SPIN,.argument.spin.velocity = 0.1 },
//		{ },
//		{ },
//		{ },
//};


#endif /* COMPONENTS_API_INCLUDE_PROCESSSTANDAR_H_ */

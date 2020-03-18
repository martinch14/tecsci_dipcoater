#include <stdio.h>

#ifndef TECSCI_ARCH_X86
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


#include "TMCMotionController.h"
#include "TMC5130.h"


#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "app_main_dipcoater.h"


#endif

#include "handlers.h"
#include <unistd.h>

#ifndef TECSCI_ARCH_X86
#define sleep(x) vTaskDelay((x) * 1000 / portTICK_RATE_MS)
#endif


/*TODO
 *
 *
 * ver fix cuando ejecuto 2 veces seguidas la busqueda de cero machine, la segunda baja en vez de subir nuevamente
 *
 *
 * */

//Variable Global con la direccion del socket creado, sirve para enviar mensajes al sokcet desde los handlers
extern int sock_global;



// Constante de dezplazamiento lineal   , expresadda en mm/micropasos         ->   una vuelta   1 paso  ==   0.00007851  mm     -->    (una vuelta) 51200   ==   4.02 mm
#define  K_DEZPLAZAMIENTO_LINEAL   0.00007851   //  78.51E-6

// Constante de diferencia de velocidad debido a que no esta el clk de 16MHZ externo en nuestro diseño

#define K_VELOCIDAD  1.34



// Constante de convesion de velocidad y desplazamineto   de mm/s a o mm/min    -> pasos



//Handler Functions
int HandlerCeroMachine(processCommandArgSpin_t *arg) {

	int32_t lectura;

	printf("Buscando Cero Machine!!\t");
	printf("velocidad:%d\t", arg->velocity);
	printf("aceleracion:%d\r\n", arg->acceleration);

	Evalboards.ch1.enableDriver(DRIVER_ENABLE);

	//TEST DEZPLAZAMINETO LINEAL CONFIGURADO DESDE MODO POSICION
//	Evalboards.ch1.writeRegister(0, 0x21, 0x00000000);
//	Evalboards.ch1.readRegister(0,0x21,&lectura);
//	printf("Posicion XACTUAL :%d\r\n",lectura);
//	Evalboards.ch1.writeRegister(0, 0x2D, 0x00000000);
//	Evalboards.ch1.readRegister(0,0x2D,&lectura);
//	printf("Posicion XTARGET :%d\r\n",lectura);
//	//MODO POSICION
//	Evalboards.ch1.writeRegister(0, 0x2D, 0x0007D000);
//	printf("Write XTARGET!\r\n");
//	Evalboards.ch1.readRegister(0,0x27,&lectura);
//	printf("Velocidad leida:%d  ------> deberia ser 200.000\r\n",lectura);
//	Evalboards.ch1.readRegister(0,0x26,&lectura);
//	printf("Aceleracion leida:%d ---------> deberia ser 20.000\r\n",lectura);
//	while (1){
//
//		Evalboards.ch1.readRegister(0,TMC5130_RAMPSTAT,&lectura);
//		if ( lectura & 0x00000200 )break;
//		else sleep(0.1);
//
//	}
//	Evalboards.ch1.readRegister(0,0x2D,&lectura);
//	printf("Termino movimiento y continuo programa...   Posicion XTARGET::%d\r\n",lectura);
//	sleep(0.1);

// Activo flag para encender StallGuard

	Evalboards.ch1.writeRegister(0, 0x34, 0x00000400);
	Evalboards.ch1.writeRegister(0, TMC5130_RAMPMODE, 0x00000000);

	// Me muevo para arriba  hasta encontrar el limite superior 360 mm para arriba   360/K_DEZPLAZAMIENTO_LINEAL

//	Evalboards.ch1.writeRegister(0, 0x2D, -(360/K_DEZPLAZAMIENTO_LINEAL));
//	Evalboards.ch1.writeRegister(0, 0x2D, ~(360/K_DEZPLAZAMIENTO_LINEAL));

	Evalboards.ch1.readRegister(0, 0x21, &lectura);
	printf("Posicion ACTUAL TARGET :%d\r\n", lectura);

	Evalboards.ch1.writeRegister(0, 0x2D, 0x00000000);
	Evalboards.ch1.readRegister(0, 0x2D, &lectura);
	printf("Posicion XTARGET :%d\r\n", lectura);

	Evalboards.ch1.writeRegister(0, 0x2D, 0xFFB1E000);
	Evalboards.ch1.readRegister(0, 0x2D, &lectura);
	printf("Posicion XTARGET :%d\r\n", lectura);


	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);
	// Seteo desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  arg->acceleration);


	//Detecto flag de stallguard
	while (1) {

		Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
		printf("Posicion RAMPSTAT :%x\r\n", lectura);

		if (lectura & 0x0000040) {

			Evalboards.ch1.writeRegister(0, TMC5130_RAMPMODE, 0x00000000);
			Evalboards.ch1.writeRegister(0, 0x21, 0x00000000);
//			Evalboards.ch1.writeRegister(0, 0x2D, 0x0007D000);  //512000    127372  -> 10mm
			Evalboards.ch1.writeRegister(0, 0x2D, 127372);
			Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
			Evalboards.ch1.writeRegister(0, 0x34, 0x00000000);
			Evalboards.ch1.writeRegister(0, 0x34, 0x00000400);
			Evalboards.ch1.writeRegister(0, 0x34, 0x00000000);

			break;
		} else
			sleep(0.2);

	}

	//Una vez detectado el cero realizo un movimiento fijo con 127372 (10 mm) pasos hacia abajo, detecto el flag que detecta XACTUAL=XTARGET
	while (1) {

		Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
		if (lectura & 0x00000200)
			break;
		else
			sleep(0.1);

	}

	Evalboards.ch1.enableDriver(DRIVER_DISABLE);

	printf("Cero Machine Detectado!!!!!\r\n");
	Evalboards.ch1.readRegister(0, 0x2D, &lectura);
	printf("Posicion XTARGET :%d\r\n", lectura);

	Evalboards.ch1.readRegister(0, 0x21, &lectura);
	printf("Posicion ACTUAL_TARGET :%d\r\n", lectura);


	return 0;

}

int HandlerSpin(processCommandArgSpin_t*	arg) {
	printf("Spinning!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);
	sleep(2);
	return 0;
}

int HandlerUp(processCommandArgSpin_t*	arg) {

	int32_t lectura;

	printf("\r\n");
	printf("Subiendo muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\t",arg->acceleration);


//	send(sock_global, &datos, sizeof(unsigned char), 0);
//	Evalboards.ch1.left(0,0x00009C40); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)

	//Lectura
	//Evalboards.ch1.writeRegister(0, 0x21, 0xFFED8910); // writing value 0xFFED8910 = 0 = 0.0 to address 9 = 0x21(XACTUAL)

//	Evalboards.ch1.readRegister(0,0x21,&lectura);
//	printf("valor actual :%d\t",lectura);

	Evalboards.ch1.readRegister(0, 0x2D, &lectura);
	printf("Posicion ATARGET :%d\r\n", lectura);


	// Seteo de registro XTARGET
	Evalboards.ch1.writeRegister(0, 0x2D, lectura - arg->displacement_z );

	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);

	// Seteo desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  arg->acceleration);

	//Una vez detectado el cero realizo un movimiento fijo con 512000 pasos hacia abajo, detecto el flag que detecta XACTUAL=XTARGET
	while (1) {

		Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
		if (lectura & 0x00000200)
			break;
		else
			sleep(0.1);

	}

	Evalboards.ch1.readRegister(0, 0x2D, &lectura);
	printf("Posicion ATARGET :%d\r\n", lectura);

	return 0;
}


int HandlerUp_without_program(processCommandArgSpin_t*	arg) {

	int32_t lectura;

	printf("Subiendo muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);

	processDipCoating.config.status=1;

	Evalboards.ch1.enableDriver(DRIVER_ENABLE);

	//Leo Actual

	Evalboards.ch1.readRegister(0, 0x21, &lectura);

	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);

	// Seteo desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  arg->acceleration);


	// Seteo de registro XTARGET
	Evalboards.ch1.writeRegister(0, 0x2D, lectura - arg->displacement_z );

	// Detecto el flag que detecta XACTUAL=XTARGET y apago driver
	while (1) {
		Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
		if (lectura & 0x00000200){
			Evalboards.ch1.enableDriver(DRIVER_DISABLE);
			break;
		}
		else
			sleep(0.1);
	}

	processDipCoating.config.status=0;

	return 0;
}


int HandlerDown_without_program(processCommandArgSpin_t*	arg) {

	int32_t lectura;

	printf("Bajando muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);


	processDipCoating.config.status=1;

	Evalboards.ch1.enableDriver(DRIVER_ENABLE);

	//Leo Actual

	Evalboards.ch1.readRegister(0, 0x21, &lectura);

	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);

	// Seteo desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  arg->acceleration);

	// Seteo de registro XTARGET
	Evalboards.ch1.writeRegister(0, 0x2D, lectura + arg->displacement_z );

	// Detecto el flag que detecta XACTUAL=XTARGET y apago driver
		while (1) {

			Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
			if (lectura & 0x00000200){
				Evalboards.ch1.enableDriver(DRIVER_DISABLE);
				break;
			}
			else
				sleep(0.1);

		}

	processDipCoating.config.status=0;

	return 0;
}




int HandlerDown(processCommandArgSpin_t*	arg) {
	int32_t lectura;

	printf("\r\n");
	printf("Bajando muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\t",arg->acceleration);

	// Seteo de registro XTARGET
	arg->displacement_z = processDipCoating.config.displacement_to_sample;

	printf("desplazamiento hasta fluido en pasos:%d\r\n",arg->displacement_z);
//	send(sock_global, &datob, sizeof(unsigned char)*7, 0);
//	Evalboards.ch1.right(0,0x00009C40); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)
//
//
//	int32_t lectura;
//	Evalboards.ch1.readRegister(0,0x21,&lectura);
//	printf("valor actual :%d\r\n",lectura);

	Evalboards.ch1.readRegister(0, 0x21, &lectura);
	printf("Posicion ACTUAL TARGET :%d\r\n", lectura);


	Evalboards.ch1.writeRegister(0, 0x2D, arg->displacement_z );

	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);

	// Seteo desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  arg->acceleration);

	//Una vez detectado el cero realizo un movimiento fijo con 512000 pasos hacia abajo, detecto el flag que detecta XACTUAL=XTARGET
	while (1) {

		Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
		if (lectura & 0x00000200)
			break;
		else
			sleep(0.1);

	}

	Evalboards.ch1.readRegister(0, 0x21, &lectura);
	printf("Posicion ACTUAL TARGET :%d\r\n", lectura);

	return 0;
}


int HandlerDownLoop(processCommandArgSpin_t*	arg) {

	int32_t lectura;

	printf("Bajando muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);

	//Leo Actual

	Evalboards.ch1.readRegister(0, 0x21, &lectura);

	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);

	//desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  arg->acceleration);

	// Seteo de registro XTARGET
	Evalboards.ch1.writeRegister(0, 0x2D, lectura + processDipCoating.config.displacement_delta_sample  );

	//Una vez detectado el cero realizo un movimiento fijo con 512000 pasos hacia abajo, detecto el flag que detecta XACTUAL=XTARGET
	while (1) {

		Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
		if (lectura & 0x00000200)
			break;
		else
			sleep(0.1);

	}

	return 0;


}

int HandlerUpLoop(processCommandArgSpin_t*	arg) {


	int32_t lectura;

	printf("Subiendo muestra!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);

	//Leo Actual

	Evalboards.ch1.readRegister(0, 0x21, &lectura);

	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);

	//desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  arg->acceleration);


	// Seteo de registro XTARGET
	Evalboards.ch1.writeRegister(0, 0x2D, lectura - processDipCoating.config.displacement_delta_sample  );


	//Una vez detectado el cero realizo un movimiento fijo con 512000 pasos hacia abajo, detecto el flag que detecta XACTUAL=XTARGET
	while (1) {

		Evalboards.ch1.readRegister(0, TMC5130_RAMPSTAT, &lectura);
		if (lectura & 0x00000200)
			break;
		else
			sleep(0.1);

	}


	return 0;


}

int HandlerWait(processCommandArgSpin_t*	arg) {
	printf("Muestra en espera!!\r\n");
//	Evalboards.ch1.rotate(0,0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)

	sleep(2);
	return 0;
}

int HandlerWaitDown(processCommandArgSpin_t*	arg) {
	printf("Muestra en espera!!\r\n");
//	Evalboards.ch1.rotate(0,0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)

	sleep(processDipCoating.command[3].argument.wait.time);
	return 0;
}


int HandlerWaitUp(processCommandArgSpin_t*	arg) {
	printf("Muestra en espera!!\r\n");
//	Evalboards.ch1.rotate(0,0x00000000); // writing value 0x00000000 = 0 = 0.0 to address 18 = 0x2C(TZEROWAIT)

	sleep(processDipCoating.command[5].argument.wait.time);
	return 0;
}


int HandlerRun() {

	Evalboards.ch1.enableDriver(DRIVER_ENABLE);
	vTaskDelay(250 / portTICK_RATE_MS);
	return 0;
}


int HandlerStop(processCommandArgSpin_t*	arg) {
	printf("Comando Stop!!\t");
	printf("velocidad:%d\t",arg->velocity);
	printf("aceleracion:%d\r\n",arg->acceleration);

	// Seteo aceleracion y velocidad en cero
	Evalboards.ch1.writeRegister(0, 0x26,  arg->acceleration);
	Evalboards.ch1.writeRegister(0, 0x27, arg->velocity);

	return 0;
}


int HandlerFinish(processCommandArgSpin_t*	arg) {

	Evalboards.ch1.enableDriver(DRIVER_DISABLE);
	printf("Proceso Finalizado - Motor Apagado\r\n");
	printf("\r\n");

	return 0;
}

int HandlerREADDATA() {

	int32_t lectura1,lectura2, i;

	int32_t lectura;

//	for (i=0; i<8 ;i++){
//		printf( "%d %d %d %d \r\n" ,processDipCoating.command[i].commandnumber,processDipCoating.command[i].argument.spin.velocity ,processDipCoating.command[i].argument.spin.acceleration,processDipCoating.command[i].argument.spin.displacement_z  );
//	}
	Evalboards.ch1.enableDriver(DRIVER_ENABLE);

	//Leo Actual

	Evalboards.ch1.readRegister(0, 0x21, &lectura);
	printf("Posicion inicial%d\r\n",lectura);

	// Seteo aceleracion y velocidad
	Evalboards.ch1.writeRegister(0, 0x26,  60000);
	Evalboards.ch1.writeRegister(0, 0x27, 100000);

	// Seteo desaceleracion
	Evalboards.ch1.writeRegister(0, 0x28,  60000);



	// Seteo de registro XTARGET
	Evalboards.ch1.writeRegister(0, 0x2D, lectura + 1273722 );

	sleep(2);


	Evalboards.ch1.readRegister(0, 0x21, &lectura);
	Evalboards.ch1.writeRegister(0, 0x2D, lectura );
	printf("Posicion final%d\r\n",lectura);

	Evalboards.ch1.enableDriver(DRIVER_DISABLE);




	return 0;
}


int HandlerENA_DRIVER() {
	Evalboards.ch1.enableDriver(DRIVER_ENABLE);
	printf("\r\n");
	return 0;
}

int HandlerDIS_DRIVER() {
	Evalboards.ch1.enableDriver(DRIVER_DISABLE);
	printf("\r\n");
	return 0;
}



int HandlerCERO_SAMPLE() {

	int32_t lectura;
	static char datos[32];

	printf("\r\n");
	Evalboards.ch1.readRegister(0, 0x21, &lectura);     //
	printf("CERO_SAMPLE %d\r\n", lectura);

	if (sock_global > 0){
		sprintf(datos,"CERO_SAMPLE %d\r\n", lectura);
		send(sock_global, &datos, sizeof(datos) , 0);
	}


	return lectura;
}


int HandlerDELTADIP() {

	printf("Profundidad de dip cargada!\r\n");
	return 0;
}






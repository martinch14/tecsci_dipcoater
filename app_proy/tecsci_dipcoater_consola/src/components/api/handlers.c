#include "../../components/api/include/handlers.h"

#include <stdio.h>
#include <unistd.h>



int HandlerCeroMachine(processCommandArgSpin_t*	arg) {
	printf("Buscando Cero Machine!!\t");
	printf("velocidad:%f\t",arg->velocity);
	printf("aceleracion:%f\r\n",arg->acceleration);
	sleep(5);
	return 0;
}
int HandlerSpin(processCommandArgSpin_t*	arg) {
	printf("Spinning!!\t");
	printf("velocidad:%f\t",arg->velocity);
	printf("aceleracion:%f\r\n",arg->acceleration);
	sleep(2);
	return 0;
}

int HandlerUp(processCommandArgSpin_t*	arg) {
	printf("Subiendo muestra!!\t");
	printf("velocidad:%f\t",arg->velocity);
	printf("aceleracion:%f\r\n",arg->acceleration);
	sleep(2);
	return 0;
}

int HandlerDown(processCommandArgSpin_t*	arg) {
	printf("Bajando muestra!!\t");
	printf("velocidad:%f\t",arg->velocity);
	printf("aceleracion:%f\r\n",arg->acceleration);
	sleep(2);
	return 0;
}

int HandlerWait(processCommandArgSpin_t*	arg) {
	printf("Muestra en espera!!\t");
	printf("velocidad:%f\t",arg->velocity);
	printf("aceleracion:%f\r\n",arg->acceleration);
	sleep(2);
	return 0;
}

int HandlerStop(processCommandArgSpin_t*	arg) {
	printf("Buscando Cero Machine!!\t");
	printf("velocidad:%f\t",arg->velocity);
	printf("aceleracion:%f\r\n",arg->acceleration);
	sleep(2);
	return 0;
}

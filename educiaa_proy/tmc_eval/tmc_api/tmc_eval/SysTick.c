#include "SysTick.h"

void tmc_wait(tick_t ticks) {
	static tick_t old_tick = 0;
	old_tick = tickRead();
	while((tickRead() - old_tick) < ticks);
}

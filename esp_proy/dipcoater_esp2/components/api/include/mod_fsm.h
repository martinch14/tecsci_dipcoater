/*
 * Copyright 2019 Sebastián Pablo Bedín <sebabedin@gmail.com> All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *        list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *        this list of conditions and the following disclaimer in the documentation
 *        and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY SEBASTIÁN PABLO BEDÍN “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL SEBASTIÁN PABLO BEDÍN OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of
 * the authors and should not be interpreted as representing official policies, either
 * expressed or implied, of Sebastián Pablo Bedín <sebabedin@gmail.com>.
 */

#ifndef MOD_FSM_H_
#define MOD_FSM_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct mod_fsm_s mod_fsm_t;

typedef void(*mod_fsm_state_handler_t)(mod_fsm_t *fsm, void *param);

typedef struct mod_fsm_state_s{
	mod_fsm_state_handler_t handler;
}mod_fsm_state_t;

typedef void(*mod_fsm_logger_t)(char *str);

struct mod_fsm_s{
	mod_fsm_state_t *states;
	uint8_t statesLen;
	uint8_t current;
	uint8_t last;
	uint8_t next;
	void *param;
	char *loggerName;
	mod_fsm_logger_t logger;
};

int8_t 		modFSM_Init			(mod_fsm_t *fsm, mod_fsm_state_t *states, uint8_t len, void *param);
void 		modFSM_Logger		(mod_fsm_t *fsm, char *name, mod_fsm_logger_t logger);
int8_t 		modFSM_Loop			(mod_fsm_t *fsm);
bool 		modFSM_Entry		(const mod_fsm_t *fsm);
bool 		modFSM_Exit			(const mod_fsm_t *fsm);
void 		modFSM_GoTo			(mod_fsm_t *fsm, uint8_t nextState);

#endif /* MOD_FSM_H_ */

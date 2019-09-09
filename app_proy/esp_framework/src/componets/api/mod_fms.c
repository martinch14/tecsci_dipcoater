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

#include "include/mod_fsm.h"
//#include "mod/inc/mod_fsm.h"


#define modFSM_STATE_RESET (0)

int8_t modFSM_Init(mod_fsm_t *fsm, mod_fsm_state_t *states, uint8_t len, void *param){
	fsm->states 	= states;
	fsm->statesLen 	= len;
	fsm->last 		= ~modFSM_STATE_RESET;
	fsm->current 	= ~modFSM_STATE_RESET;
	fsm->next 		= modFSM_STATE_RESET;
	fsm->param 		= param;
	fsm->logger 	= NULL;
	return 0;
}

void modFSM_Logger(mod_fsm_t *fsm, char *name, mod_fsm_logger_t logger){
	if(NULL != logger){
		fsm->loggerName 	= name;
		fsm->logger 		= logger;
	}
}

static char _str[128];

int8_t modFSM_Loop(mod_fsm_t *fsm){
	fsm->current = fsm->next;

	if(modFSM_Entry(fsm) && fsm->logger){
		sprintf(_str,
				"fsm.%s > %d.Entry",
				fsm->loggerName,
				fsm->current);
		fsm->logger(_str);
	}

	fsm->states[fsm->current].handler(fsm, fsm->param);

	if(modFSM_Exit(fsm) && fsm->logger){
		sprintf(_str,
				"fsm_%s > %d.Exit",
				fsm->loggerName,
				fsm->current);
		fsm->logger(_str);
	}

	fsm->last = fsm->current;
	return 0;
}

bool modFSM_Entry(const mod_fsm_t *fsm){
	if(fsm->last != fsm->current){
		return true;
	}
	return false;
}

bool modFSM_Exit(const mod_fsm_t *fsm){
	if(fsm->next != fsm->current){
		return true;
	}
	return false;
}

void modFSM_GoTo(mod_fsm_t *fsm, uint8_t nextState){
	fsm->next = nextState;
}


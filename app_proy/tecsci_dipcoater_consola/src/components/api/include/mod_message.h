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

#ifndef MOD_MESSAGE_H_
#define MOD_MESSAGE_H_

#include <stdint.h>

#include "../../../components/api/include/mod_queue.h"

/*
 * Config
 */
#define _MOD_MESSAGE_CFG_LOGGER_ENABLE 			(1)
#define _MOD_MESSAGE_CFG_NODE_PRINT_ENABLE 		(1)

typedef enum  mod_message_signal_e{
	MOD_MESSAGE_SIGNAL_ERROR,
	MOD_MESSAGE_SIGNAL_RECEIVE_OK,
	MOD_MESSAGE_SIGNAL_RECEIVE_ERROR,
	MOD_MESSAGE_SIGNAL_RECEIVE_EMPTY,
	MOD_MESSAGE_SIGNAL_SEND_OK,
	MOD_MESSAGE_SIGNAL_SEND_ERROR,
}mod_message_signal_t;

typedef void (*mod_message_print_t)(uint8_t, mod_message_signal_t, void*);
typedef void (*mod_message_logger_t)(uint8_t, mod_message_signal_t);
typedef struct mod_message_channel_s mod_message_channel_t;

#define MOD_MESSAGE_NODE_PRINT_RECEIVE 	(0x01 << 0)
#define MOD_MESSAGE_NODE_PRINT_SEND 	(0x01 << 1)

struct mod_message_channel_s{
	mod_queue_t queue;
	uint8_t flags;
#if _MOD_MESSAGE_CFG_NODE_PRINT_ENABLE
	mod_message_print_t print;
#endif
};

int8_t 					modMessageService_Init	(mod_message_channel_t *channels, uint8_t channelsLen, mod_message_logger_t looger);
mod_message_signal_t 	modMessage_Init			(uint8_t id, uint8_t *buffer, uint32_t bufferLen, uint32_t itemSize, mod_message_print_t messagePrint);
uint8_t					modMessage_CfgGet		(uint8_t id);
void 					modMessage_CfgSet		(uint8_t id, uint8_t flags);
mod_message_signal_t 	modMessage_Receive		(uint8_t id, void *msg);
mod_message_signal_t 	modMessage_Send			(uint8_t id, void *msg);

#endif /* MOD_MESSAGE_H_ */

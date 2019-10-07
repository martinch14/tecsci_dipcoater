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

#include "../../components/api/include/mod_message.h"

#include <stdint.h>

#include "../../components/api/include/mod_queue.h"

struct mod_message_service_s{
	mod_message_channel_t *channels;
	uint8_t channelsLen;
#if _MOD_MESSAGE_CFG_LOGGER_ENABLE
	mod_message_logger_t logger;
#endif
}_self;

#define _pChannel 	(&(_self.channels[id]))
#define _pQueue (&(_pChannel->queue))

#if _MOD_MESSAGE_CFG_NODE_PRINT_ENABLE
#define _PRINT(val) \
	if(_pChannel->print && (_pChannel->flags & (val))){\
		_pChannel->print(id, ret, msg);\
	}
#else
#define _PRINT(val)
#endif

#if _MOD_MESSAGE_CFG_LOGGER_ENABLE
#define _RETURN(val) \
	if(_self.logger){\
		_self.logger(id, (val));\
	}\
	return (val)
#else
#define _RETURN(val) \
	return (val)
#endif


int8_t modMessageService_Init(mod_message_channel_t *channels, uint8_t channelsLen, mod_message_logger_t looger){
	_self.channels 	= channels;
	_self.channelsLen 	= channelsLen;
#if _MOD_MESSAGE_CFG_LOGGER_ENABLE
	_self.logger 	= looger;
#endif
	return 0;
}

mod_message_signal_t modMessage_Init(uint8_t id, uint8_t *buffer, uint32_t bufferLen, uint32_t itemSize, mod_message_print_t messagePrint){
	if(_self.channelsLen <= id){
		_RETURN(MOD_MESSAGE_SIGNAL_ERROR);
	}
#if _MOD_MESSAGE_CFG_NODE_PRINT_ENABLE
	_pChannel->print = messagePrint;
#endif
	return modQueue_Init(_pQueue, buffer, bufferLen, itemSize);
}

uint8_t modMessage_CfgGet(uint8_t id){
	return _pChannel->flags;
}

void modMessage_CfgSet(uint8_t id, uint8_t flags){
	_pChannel->flags = flags;
}

mod_message_signal_t modMessage_Receive(uint8_t id, void *msg){
	mod_message_signal_t ret = MOD_MESSAGE_SIGNAL_ERROR;
	if(_self.channelsLen <= id){
		_RETURN(ret);
	}

	uint32_t bytes = modQueue_Read(_pQueue, msg);

	if(_pQueue->rb.itemSize == bytes){
		ret = MOD_MESSAGE_SIGNAL_RECEIVE_OK;
		_PRINT(MOD_MESSAGE_NODE_PRINT_RECEIVE);
	}else{
		if(0 == bytes){
			ret = MOD_MESSAGE_SIGNAL_RECEIVE_EMPTY;
		}else{
			ret = MOD_MESSAGE_SIGNAL_RECEIVE_ERROR;
			_PRINT(MOD_MESSAGE_NODE_PRINT_RECEIVE);
		}
	}
	_RETURN(ret);
}

mod_message_signal_t modMessage_Send(uint8_t id, void *msg){
	mod_message_signal_t ret = MOD_MESSAGE_SIGNAL_ERROR;
	if(_self.channelsLen <= id){
		_RETURN(ret);
	}

	uint32_t bytes = modQueue_Write(_pQueue, msg);

	if(_pQueue->rb.itemSize == bytes){
		ret = MOD_MESSAGE_SIGNAL_SEND_OK;
	}else{
		ret = MOD_MESSAGE_SIGNAL_SEND_ERROR;
	}
	_PRINT(MOD_MESSAGE_NODE_PRINT_SEND);
	_RETURN(ret);
}

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

#include "../../components/api/include/mod_ringbuffer.h"

static inline void _inc(mod_rinfbuffer_t *rb, uint8_t *i){
	if(*i < (rb->bufferLen - 1)){
		(*i)++;
	}else{
		*i = 0;
	}
}

static inline void _dec(mod_rinfbuffer_t *rb, uint8_t *i){
	if(0 < *i){
		(*i)--;
	}else{
		*i = (rb->bufferLen - 1);
	}
}

int8_t modRingBuffer_Init(mod_rinfbuffer_t *rb, uint8_t *buffer, uint32_t bufferLen, uint32_t itemSize){
	rb->buffer 		= buffer;
	rb->bufferLen 	= bufferLen;
	rb->itemSize 	= itemSize;
	rb->len 		= 0;
	rb->head 		= 0;
	rb->tail 		= 0;
	_dec(rb, &rb->tail);
	return 0;
}

uint32_t modRingBuffer_Read_Back(mod_rinfbuffer_t *rb, void *out){
	if(rb->len){
		rb->len--;
		_inc(rb, &rb->tail);
		memcpy(out, (void*)&(rb->buffer[rb->tail*rb->itemSize]), rb->itemSize);
		return rb->itemSize;
	}
	return 0;
}

uint32_t modRingBuffer_Read_Front(mod_rinfbuffer_t *rb, void *out){
	if(rb->len){
		rb->len--;
		_dec(rb, &rb->head);
		memcpy(out, (void*)&(rb->buffer[rb->head*rb->itemSize]), rb->itemSize);
		return rb->itemSize;
	}
	return 0;
}

uint32_t modRingBuffer_Write_Back(mod_rinfbuffer_t *rb, void *in){
	if(rb->len < rb->bufferLen){
		rb->len++;
		memcpy((void*)&(rb->buffer[rb->tail*rb->itemSize]), in, rb->itemSize);
		_dec(rb, &rb->tail);
		return rb->itemSize;
	}
	return 0;
}

uint32_t modRingBuffer_Write_Front(mod_rinfbuffer_t *rb, void *in){
	if(rb->len < rb->bufferLen){
		rb->len++;
		memcpy((void*)&(rb->buffer[rb->head*rb->itemSize]), in, rb->itemSize);
		_inc(rb, &rb->head);
		return rb->itemSize;
	}
	return 0;
}

uint32_t modRingBuffer_Len(mod_rinfbuffer_t *rb){
	return rb->len;
}

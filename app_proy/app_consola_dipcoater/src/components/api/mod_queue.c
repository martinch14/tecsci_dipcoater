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

#include "../../components/api/include/mod_queue.h"

int8_t modQueue_Init(mod_queue_t *queue, uint8_t *buffer, uint32_t bufferLen, uint32_t itemSize){
	return modRingBuffer_Init(&(queue->rb), buffer, bufferLen, itemSize);
}

uint32_t modQueue_Read(mod_queue_t *queue, void *data){
	return modRingBuffer_Read_Back(&(queue->rb), data);
}

uint32_t modQueue_Write(mod_queue_t *queue, void *data){
	return modRingBuffer_Write_Front(&(queue->rb), data);
}

uint32_t modQueue_Len(mod_queue_t *queue){
	return modRingBuffer_Len(&(queue->rb));
}

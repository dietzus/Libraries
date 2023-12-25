/*
 * DMA_Buffer.h
 *
 *  Created on: 03.06.2023
 *      Author: Dietz
 */

#include "main.h"

#ifndef INC_DMA_BUFFER_H_
#define INC_DMA_BUFFER_H_

typedef struct {
	uint16_t buffersize;
	uint8_t varsize;
	uint32_t *BufFH;
	uint32_t *BufSH;


} DMA_Buffer_t;

#endif /* INC_DMA_BUFFER_H_ */

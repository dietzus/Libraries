/*
 * DMA_Buffer.c
 *
 *  Created on: 03.06.2023
 *      Author: Dietz
 */

#include "DMA_Buffer.h"
#ifdef USEDMABUFFER
#include "stdlib.h"

uint8_t DMABuff_Init(DMA_Buffer_t buffer, uint8_t varsize, uint16_t bufsize) {
	uint8_ tempsize;
	if(varsize <= 8) {
		tempsize = 8;
	} else if (varsize <= 16) {
		tempsize = 16;
	} else {
		tempsize = 32;
	}
	buffer.varsize = tempsize;

	buffer.BufFH = malloc(tempsize * bufsize);
	buffer.BufSH = &buffer.BufFH[(tempsize/2) * bufsize];

	return 0;
}





#endif

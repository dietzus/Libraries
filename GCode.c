#include "GCode.h"
#include "main.h"

#ifdef USEGCODE
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "Utils.h"

#define DTZ '.'
#define BUFFER_SIZE 32

GCodePara_t GCodeBuffer[BUFFER_SIZE];
int8_t curPara = 0;
int8_t newPara = 0;
int8_t paraCount = 0;

void initGCode(GCodePara_t *gcodepara) {
	gcodepara->valid = 0;
	gcodepara->validI = 0;
	gcodepara->validJ = 0;
	gcodepara->validK = 0;
	gcodepara->validP = 0;
	gcodepara->validS = 0;
	gcodepara->validO = 0;
	gcodepara->validX = 0;
	gcodepara->validY = 0;
	gcodepara->validZ = 0;
	gcodepara->validE = 0;
	gcodepara->validF = 0;
}

GCodePara_t parseGCString(uint8_t *origstring) {
	unsigned char *string = malloc(64 * sizeof(char));
	strcpy((char*)string, (char*)origstring);

	GCodePara_t temppara;
	initGCode(&temppara);

	uint8_t i = 0;

	if(string[i] == 'G' || string[i] == 'g') {
		temppara.character = 'G';
	} else if (string[i] == 'M' || string[i] == 'm') {
		temppara.character = 'M';
	} else {
		free(string);
		return temppara;
	}

	i++;
	if((temppara.valid = isdigit(string[i])) > 0) {
		temppara.number = (string[i] - 48);
	}

	i++;
	while(isdigit(string[i])) {
		temppara.number = (temppara.number * 10) + (string[i] - 48);
		i++;
	}

//	i++;
	for (; i < strlen(string); i++) {
		if(isalpha(string[i])) {
			int valid = 0;
			if(string[i] == 'I' || string[i] == 'i') {
				valid = temppara.validI = findParaInt(string, i+1, &(temppara.I));
			} else if(string[i] == 'J' || string[i] == 'j') {
				valid = temppara.validJ = findParaInt(string, i+1, &(temppara.J));
			} else if(string[i] == 'K' || string[i] == 'k') {
				valid = temppara.validK = findParaInt(string, i+1, &(temppara.K));
			} else if(string[i] == 'P' || string[i] == 'p') {
				valid = temppara.validP = findParaInt(string, i+1, &(temppara.P));
			} else if(string[i] == 'S' || string[i] == 's') {
				valid = temppara.validS = findParaInt(string, i+1, &(temppara.S));
			} else if(string[i] == 'O' || string[i] == 'o') {
				valid = temppara.validO = findParaFloat(string, i+1, &(temppara.O));
			} else if(string[i] == 'X' || string[i] == 'x') {
				valid = temppara.validX = findParaFloat(string, i+1, &(temppara.X));
			} else if(string[i] == 'Y' || string[i] == 'y') {
				valid = temppara.validY = findParaFloat(string, i+1, &(temppara.Y));
			} else if(string[i] == 'Z' || string[i] == 'z') {
				valid = temppara.validZ = findParaFloat(string, i+1, &(temppara.Z));
			} else if(string[i] == 'E' || string[i] == 'e') {
				valid = temppara.validE = findParaFloat(string, i+1, &(temppara.E));
			} else if(string[i] == 'F' || string[i] == 'f') {
				valid = temppara.validF = findParaInt(string, i+1, &(temppara.F));
			} else {
//				USBD_CDC_TransmitPacket(pdev);
			}
			i += valid;
		}
	}

	free(string);
	return temppara;
}

void printGCString(GCodePara_t para, uint8_t *origstring) {
	char *string = (char*)origstring;
	char tempstring[200] = "";
	char temp[20] = "";

	if(para.valid) {
		strcat(tempstring, "It is GCode with the following parameters:\n");
		sprintf(temp, "%c%d\n", para.character, para.number);
		strcat(tempstring, temp);
	} else {
		strcat(tempstring, "The GCode was NOT valid.");
		return;
	}

	if(para.validI) appendIntPara(tempstring, 'I', para.I, 0);
	if(para.validJ) appendIntPara(tempstring, 'J', para.J, 0);
	if(para.validK) appendIntPara(tempstring, 'K', para.K, 0);
	if(para.validP) appendIntPara(tempstring, 'P', para.P, 0);
	if(para.validS) appendIntPara(tempstring, 'S', para.S, 0);

	if(para.validO) appendFloatPara(tempstring, 'O', para.O, 0);
	if(para.validX) appendFloatPara(tempstring, 'X', para.X, 0);
	if(para.validY) appendFloatPara(tempstring, 'Y', para.Y, 0);
	if(para.validZ) appendFloatPara(tempstring, 'Z', para.Z, 0);
	if(para.validE) appendFloatPara(tempstring, 'E', para.E, 0);

	strcpy(string, tempstring);
}

uint8_t createGCString(GCodePara_t *para, uint8_t *origstring) {
	char tempstring[200] = "";

	if(para->valid) {
		sprintf(tempstring, "%c%d", para->character, para->number);
	} else {
		strcat(tempstring, "The GCode was NOT valid.");
		return 0;
	}

	if(para->validI) appendIntPara(tempstring, 'I', para->I, 1);
	if(para->validJ) appendIntPara(tempstring, 'J', para->J, 1);
	if(para->validK) appendIntPara(tempstring, 'K', para->K, 1);
	if(para->validP) appendIntPara(tempstring, 'P', para->P, 1);
	if(para->validS) appendIntPara(tempstring, 'S', para->S, 1);

	if(para->validO) appendFloatPara(tempstring, 'O', para->O, 1);
	if(para->validX) appendFloatPara(tempstring, 'X', para->X, 1);
	if(para->validY) appendFloatPara(tempstring, 'Y', para->Y, 1);
	if(para->validZ) appendFloatPara(tempstring, 'Z', para->Z, 1);
	if(para->validE) appendFloatPara(tempstring, 'E', para->E, 1);

//	origstring = malloc((strlen(tempstring) + 1));
	if(origstring != 0) {
		strcpy((char*)origstring, tempstring);
		return 1;
	}
	return 0;
}

void appendIntPara(char *string, char toappend, int32_t para, uint8_t serial) {
	char temp[20] = "";

	if (serial) {
		sprintf(temp, "%c%d", toappend, (int)para);
	} else {
		sprintf(temp, "%c=%d\n", toappend, (int)para);
	}
	strcat(string, temp);
}

void appendFloatPara(char *string, char toappend, float para, uint8_t serial) {
	char temp[20] = "";

	if (serial) {
		sprintf(temp, "%c%.2f", toappend, para);
	} else {
		sprintf(temp, "%c=%.2f\n", toappend, para);
	}
	strcat(string, temp);
}

uint8_t addParaGC(GCodePara_t para) {
	if(isFullGC()) {
		return 0;
	}

	if(isEmergencyGCPara(&para)) {
// ALARM ... ALARM !!!
	} else if(isGCQueueClear(&para)) {
		clearQueueGC();
	} else {
		copyGCode(&GCodeBuffer[newPara], &para);
//		GCodeBuffer[newPara] = para;
		newPara++;
		if(newPara > (BUFFER_SIZE - 1)) {
			newPara = 0;
		}
		paraCount++;
	}

	return 1;
}

uint8_t getParaGC(GCodePara_t *temppara) {
	if(paraCount == 0) {
		return 0;
	}

	int tempPos = curPara;
	curPara++;
	if(curPara > (BUFFER_SIZE - 1)) {
		curPara = 0;
	}
	if(paraCount > 0) paraCount--;

	*temppara = GCodeBuffer[tempPos];
//	int32_t tempX = temppara->X;
	return 1;
}

uint8_t isEmptyGC() {
	if(paraCount == 0) {
		return 1;
	}
	return 0;
}

uint8_t isFullGC() {
	return (paraCount == (BUFFER_SIZE - 1));
}

uint8_t clearQueueGC() {
	if(isEmptyGC()) {
		return 0;
	}
	curPara = 0;
	newPara = 0;
	paraCount = 0;

	return 1;
}

uint8_t isMovementGCPara(GCodePara_t *para) {
	if(para->character == 'G') {
		return 1;
	}
	return 0;
}

uint8_t isNextMovementGCPara() {
	if(!isEmptyGC() && isMovementGCPara(&GCodeBuffer[curPara])) {
		return 1;
	}
	return 0;
}

uint8_t isImmediateGCPara(GCodePara_t *para) {

	return 0;
}

uint8_t isEmergencyGCPara(GCodePara_t *para) {
	return 0;
}

uint8_t isGCQueueClear(GCodePara_t *para) {
	return 0;
}

uint8_t getGCMovementCom(GCodePara_t *temppara) {
	if(isNextMovementGCPara()) {
		return getParaGC(temppara);
	}
	return 0;
}

void exeNonMoveGCPara() {

}

uint8_t GCMovementDone(GCodePara_t *temppara) {
	while(!isEmptyGC()) {
		if(isNextMovementGCPara()) {
			getParaGC(temppara);
			return 1;
		} else {
			exeNonMoveGCPara();
		}
	}
	return 0;
}

void copyGCode(GCodePara_t *paraToSet, GCodePara_t *paraToGet) {
	if(!(paraToSet->valid = paraToGet->valid)) {
		return;
	}

	paraToSet->character = paraToGet->character;
	paraToSet->number = paraToGet->number;

	if((paraToSet->validI = paraToGet->validI)) {
		paraToSet->I = paraToGet->I;
	}
	if((paraToSet->validI = paraToGet->validI)) {
		paraToSet->I = paraToGet->I;
	}
	if((paraToSet->validJ = paraToGet->validJ)) {
		paraToSet->J = paraToGet->J;
	}
	if((paraToSet->validK = paraToGet->validK)) {
		paraToSet->K = paraToGet->K;
	}
	if((paraToSet->validP = paraToGet->validP)) {
		paraToSet->P = paraToGet->P;
	}
	if((paraToSet->validS = paraToGet->validS)) {
		paraToSet->S = paraToGet->S;
	}
	if((paraToSet->validO = paraToGet->validO)) {
		paraToSet->O = paraToGet->O;
	}
	if((paraToSet->validX = paraToGet->validX)) {
		paraToSet->X = paraToGet->X;
	}
	if((paraToSet->validY = paraToGet->validY)) {
		paraToSet->Y = paraToGet->Y;
	}
	if((paraToSet->validZ = paraToGet->validZ)) {
		paraToSet->Z = paraToGet->Z;
	}
	if((paraToSet->validE = paraToGet->validE)) {
		paraToSet->E = paraToGet->E;
	}
	if((paraToSet->validF = paraToGet->validF)) {
		paraToSet->F = paraToGet->F;
	}
}

void createM0String(uint8_t *buffer, float curX, float curY, uint8_t isMoving) {
	sprintf((char*)buffer, "M0;%05.1f;%05.1f;%d", curX, curY, isMoving);
}

uint8_t parseM0String(uint8_t *string, float *curX, float *curY, uint8_t *isMoving) {
	if((string[0] == 'M' || string[0] == 'm') && string[1] == '0') {
		float tempx, tempy;
		if(!findParaFloat((char*)string, 3, &tempx)) return 1;
		if(!findParaFloat((char*)string, 9, &tempy)) return 1;
		if(string[11] != 1 && string[11] != 0) return 1;

		*isMoving = string[11];
		*curX = tempx;
		*curY = tempy;

	} else {
		return 1;
	}
	return 0;
}
#endif

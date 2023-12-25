/*
 * File_Handling_RTOS.h
 *
 *  Created on: 14-May-2020
 *      Author: Controllerstech
 */

#ifndef FILE_HANDLING_RTOS_H_
#define FILE_HANDLING_RTOS_H_

#include "main.h"
#ifdef USESDIO

#include "fatfs.h"
#include "string.h"
#include "stdio.h"

/* mounts the sd card*/
FRESULT FileH_MountSD (const TCHAR* path);

/* unmounts the sd card*/
FRESULT FileH_UnmountSD (const TCHAR* path);

/* Start node to be scanned (***also used as work area***) */
FRESULT FileH_ScanSD (char* pat);

/* Only supports removing files from home directory. Directory remover to be added soon */
FRESULT FileH_FormatSD (void);

/* write the data to the file
 * @ name : is the path to the file*/
FRESULT FileH_WriteFile (char *name, char *data);

/* read data from the file
 * @ name : is the path to the file*/
FRESULT FileH_ReadFile (char *name);

/* creates the file, if it does not exists
 * @ name : is the path to the file*/
FRESULT FileH_CreateFile (char *name);

/* Removes the file from the sd card
 * @ name : is the path to the file*/
FRESULT FileH_RemoveFile (char *name);

/* creates a directory
 * @ name: is the path to the directory
 */
FRESULT FileH_CreateDir (char *name);

/* checks the free space in the sd card*/
uint32_t FileH_CheckSDSpace (void);

/* updates the file. write pointer is set to the end of the file
 * @ name : is the path to the file
 */
FRESULT FileH_UpdateFile (char *name, char *data);

#endif
#endif /* FILE_HANDLING_RTOS_H_ */

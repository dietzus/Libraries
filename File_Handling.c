/*
 * File_Handling_RTOS.c
 *
 *  Created on: 14-May-2020
 *      Author: Controllerstech
 */

#ifdef USESDIO
#include <File_Handling.h>

#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F3
#include "stm32f3xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#endif
#ifdef STM32H7
#include "stm32h7xx_hal.h"
#endif

#include "usbd_cdc_if.h"

extern UART_HandleTypeDef huart1;

#define UART &huart1

/* =============================>>>>>>>> NO CHANGES AFTER THIS LINE =====================================>>>>>>> */

FATFS fs;  // file system
FIL fil; // File
FILINFO fno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

/**** capacity related *****/
FATFS *pfs;
DWORD fre_clust;
uint32_t total, free_space;


#if FILEHANDLINGDEBUG
void FileHandling_Send (char *string)
{
	HAL_UART_Transmit(UART, (uint8_t *)string, strlen (string), HAL_MAX_DELAY);
	CDC_Transmit_FS((uint8_t*)string, strlen(string));
	HAL_Delay(1);
}
#endif


FRESULT FileH_MountSD (const TCHAR* path)
{
	fresult = f_mount(&fs, path, 1);
#if FILEHANDLINGDEBUG
	if (fresult != FR_OK) FileHandling_Send ("ERROR!!! in mounting SD CARD...\n");
	else FileHandling_Send("SD CARD mounted successfully...\n");
#endif
	return fresult;
}

FRESULT FileH_UnmountSD (const TCHAR* path)
{
	fresult = f_mount(NULL, path, 1);
#if FILEHANDLINGDEBUG
	if (fresult == FR_OK) FileHandling_Send ("SD CARD UNMOUNTED successfully...\n\n");
	else FileHandling_Send("ERROR!!! in UNMOUNTING SD CARD\n\n");
#endif
	return fresult;
}

/* Start node to be scanned (***also used as work area***) */
FRESULT FileH_ScanSD (char* pat)
{
    DIR dir;
    UINT i;
    char *path = malloc(20*sizeof (char));
    sprintf (path, "%s",pat);

    fresult = f_opendir(&dir, path);                       /* Open the directory */
    if (fresult == FR_OK)
    {
        for (;;)
        {
            fresult = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (fresult != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR)     /* It is a directory */
            {
            	if (!(strcmp ("SYSTEM~1", fno.fname))) continue;
#if FILEHANDLINGDEBUG
            	char *buf = malloc(30*sizeof(char));
            	sprintf (buf, "Dir: %s\r\n", fno.fname);
            	FileHandling_Send(buf);
            	free(buf);
#endif
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                fresult = FileH_ScanSD(path);                     /* Enter the directory */
                if (fresult != FR_OK) break;
                path[i] = 0;
            }
#if FILEHANDLINGDEBUG
            else
            {   /* It is a file. */
           	   char *buf = malloc(30*sizeof(char));
               sprintf(buf,"File: %s/%s\n", path, fno.fname);
               FileHandling_Send(buf);
               free(buf);
            }
#endif
        }
        f_closedir(&dir);
    }
    free(path);
    return fresult;
}

/* Only supports removing files from home directory */
FRESULT FileH_FormatSD (void)
{
    DIR dir;
    char *path = malloc(20*sizeof (char));
    sprintf (path, "%s","/");

    fresult = f_opendir(&dir, path);                       		/* Open the directory */
    if (fresult == FR_OK)
    {
        for (;;)
        {
            fresult = f_readdir(&dir, &fno);                   	/* Read a directory item */
            if (fresult != FR_OK || fno.fname[0] == 0) break;  	/* Break on error or end of dir */
            if (fno.fattrib & AM_DIR)     						/* It is a directory */
            {
            	if (!(strcmp ("SYSTEM~1", fno.fname))) continue;
            	fresult = f_unlink(fno.fname);
            	if (fresult == FR_DENIED) continue;
            }
            else
            {   /* It is a file. */
               fresult = f_unlink(fno.fname);
            }
        }
        f_closedir(&dir);
    }
    free(path);
    return fresult;
}




FRESULT FileH_WriteFile (char *name, char *data)
{

	/**** check whether the file exists or not ****/
	fresult = f_stat (name, &fno);
	if (fresult != FR_OK)
	{
#if FILEHANDLINGDEBUG
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERROR!!! *%s* does not exists\n\n", name);
		FileHandling_Send (buf);
	    free(buf);
#endif
	    return fresult;
	}
	else
	{
	    /* Create a file with read write access and open it */
	    fresult = f_open(&fil, name, FA_OPEN_EXISTING | FA_WRITE);
	    if (fresult != FR_OK)
	    {
#if FILEHANDLINGDEBUG
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "ERROR!!! No. %d in opening file *%s*\n\n", fresult, name);
	    	FileHandling_Send(buf);
	        free(buf);
#endif
	        return fresult;
	    }

	    else
	    {
	    	fresult = f_write(&fil, data, strlen(data), &bw);
#if FILEHANDLINGDEBUG
	    	if (fresult != FR_OK)
	    	{
	    		char *buf = malloc(100*sizeof(char));
	    		sprintf (buf, "ERROR!!! No. %d while writing to the FILE *%s*\n\n", fresult, name);
	    		FileHandling_Send(buf);
	    		free(buf);
	    	}
#endif

	    	/* Close file */
	    	fresult = f_close(&fil);
#if FILEHANDLINGDEBUG
	    	if (fresult != FR_OK)
	    	{
	    		char *buf = malloc(100*sizeof(char));
	    		sprintf (buf, "ERROR!!! No. %d in closing file *%s* after writing it\n\n", fresult, name);
	    		FileHandling_Send(buf);
	    		free(buf);
	    	}
	    	else
	    	{
	    		char *buf = malloc(100*sizeof(char));
	    		sprintf (buf, "File *%s* is WRITTEN and CLOSED successfully\n", name);
	    		FileHandling_Send(buf);
	    		free(buf);
	    	}
#endif
	    }
	    return fresult;
	}
}

FRESULT FileH_ReadFile (char *name)
{
	/**** check whether the file exists or not ****/
	fresult = f_stat (name, &fno);
	if (fresult != FR_OK)
	{
#if FILEHANDLINGDEBUG
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERRROR!!! *%s* does not exists\n\n", name);
		FileHandling_Send (buf);
		free(buf);
#endif
	    return fresult;
	}

	else
	{
		/* Open file to read */
		fresult = f_open(&fil, name, FA_READ);

		if (fresult != FR_OK)
		{
#if FILEHANDLINGDEBUG
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "ERROR!!! No. %d in opening file *%s*\n\n", fresult, name);
		    FileHandling_Send(buf);
		    free(buf);
#endif
		    return fresult;
		}

		/* Read data from the file
		* see the function details for the arguments */

		char *buffer = malloc(sizeof(f_size(&fil)));
		fresult = f_read (&fil, buffer, f_size(&fil), &br);
		if (fresult != FR_OK)
		{
			free(buffer);
#if FILEHANDLINGDEBUG
			char *buf = malloc(100*sizeof(char));
		 	sprintf (buf, "ERROR!!! No. %d in reading file *%s*\n\n", fresult, name);
		  	FileHandling_Send(buffer);
		  	free(buf);
#endif
		}

		else
		{
#if FILEHANDLINGDEBUG
			FileHandling_Send(buffer);
			free(buffer);
#endif

			/* Close file */
			fresult = f_close(&fil);
#if FILEHANDLINGDEBUG
			if (fresult != FR_OK)
			{
				char *buf = malloc(100*sizeof(char));
				sprintf (buf, "ERROR!!! No. %d in closing file *%s*\n\n", fresult, name);
				FileHandling_Send(buf);
				free(buf);
			}
			else
			{
				char *buf = malloc(100*sizeof(char));
				sprintf (buf, "File *%s* CLOSED successfully\n", name);
				FileHandling_Send(buf);
				free(buf);
			}
#endif
		}
	    return fresult;
	}
}

FRESULT FileH_CreateFile (char *name)
{
	fresult = f_stat (name, &fno);
	if (fresult == FR_OK)
	{
#if FILEHANDLINGDEBUG
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERROR!!! *%s* already exists!!!!\n use Update_File \n\n",name);
		FileHandling_Send(buf);
		free(buf);
#endif
	    return fresult;
	}
	else
	{
		fresult = f_open(&fil, name, FA_CREATE_ALWAYS|FA_READ|FA_WRITE);
		if (fresult != FR_OK)
		{
#if FILEHANDLINGDEBUG
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "ERROR!!! No. %d in creating file *%s*\n\n", fresult, name);
			FileHandling_Send(buf);
			free(buf);
#endif
		    return fresult;
		}
#if FILEHANDLINGDEBUG
		else
		{
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "*%s* created successfully\n Now use Write_File to write data\n",name);
			FileHandling_Send(buf);
			free(buf);
		}
#endif

		fresult = f_close(&fil);
#if FILEHANDLINGDEBUG
		if (fresult != FR_OK)
		{
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "ERROR No. %d in closing file *%s*\n\n", fresult, name);
			FileHandling_Send(buf);
			free(buf);
		}
		else
		{
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "File *%s* CLOSED successfully\n", name);
			FileHandling_Send(buf);
			free(buf);
		}
#endif
	}
    return fresult;
}

FRESULT FileH_UpdateFile (char *name, char *data)
{
	/**** check whether the file exists or not ****/
	fresult = f_stat (name, &fno);
	if (fresult != FR_OK)
	{
#if FILEHANDLINGDEBUG
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERROR!!! *%s* does not exists\n\n", name);
		FileHandling_Send (buf);
		free(buf);
#endif
	    return fresult;
	}

	else
	{
		 /* Create a file with read write access and open it */
	    fresult = f_open(&fil, name, FA_OPEN_APPEND | FA_WRITE);
	    if (fresult != FR_OK)
	    {
#if FILEHANDLINGDEBUG
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "ERROR!!! No. %d in opening file *%s*\n\n", fresult, name);
	    	FileHandling_Send(buf);
	        free(buf);
#endif
	        return fresult;
	    }

	    /* Writing text */
	    fresult = f_write(&fil, data, strlen (data), &bw);
#if FILEHANDLINGDEBUG
	    if (fresult != FR_OK)
	    {
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "ERROR!!! No. %d in writing file *%s*\n\n", fresult, name);
	    	FileHandling_Send(buf);
	    	free(buf);
	    }

	    else
	    {
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "*%s* UPDATED successfully\n", name);
	    	FileHandling_Send(buf);
	    	free(buf);
	    }
#endif

	    /* Close file */
	    fresult = f_close(&fil);
#if FILEHANDLINGDEBUG
	    if (fresult != FR_OK)
	    {
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "ERROR!!! No. %d in closing file *%s*\n\n", fresult, name);
	    	FileHandling_Send(buf);
	    	free(buf);
	    }
	    else
	    {
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "File *%s* CLOSED successfully\n", name);
	    	FileHandling_Send(buf);
	    	free(buf);
	     }
#endif
	}
    return fresult;
}

FRESULT FileH_RemoveFile (char *name)
{
	/**** check whether the file exists or not ****/
	fresult = f_stat (name, &fno);
	if (fresult != FR_OK)
	{
#if FILEHANDLINGDEBUG
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERROR!!! *%s* does not exists\n\n", name);
		FileHandling_Send (buf);
		free(buf);
#endif
		return fresult;
	}

	else
	{
		fresult = f_unlink (name);
#if FILEHANDLINGDEBUG
		if (fresult == FR_OK)
		{
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "*%s* has been removed successfully\n", name);
			FileHandling_Send (buf);
			free(buf);
		}

		else
		{
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "ERROR No. %d in removing *%s*\n\n",fresult, name);
			FileHandling_Send (buf);
			free(buf);
		}
#endif
	}
	return fresult;
}

FRESULT FileH_CreateDir (char *name)
{
    fresult = f_mkdir(name);
#if FILEHANDLINGDEBUG
    if (fresult == FR_OK)
    {
    	char *buf = malloc(100*sizeof(char));
    	sprintf (buf, "*%s* has been created successfully\n", name);
    	FileHandling_Send (buf);
    	free(buf);
    }
    else
    {
    	char *buf = malloc(100*sizeof(char));
    	sprintf (buf, "ERROR No. %d in creating directory *%s*\n\n", fresult,name);
    	FileHandling_Send(buf);
    	free(buf);
    }
#endif
    return fresult;
}

uint32_t FileH_CheckSDSpace (void)
{
    /* Check free space */
    f_getfree("", &fre_clust, &pfs);

    total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
#if FILEHANDLINGDEBUG
    char *buf = malloc(30*sizeof(char));
    sprintf (buf, "SD CARD Total Size: \t%lu\n",total);
    FileHandling_Send(buf);
    free(buf);
#endif
    free_space = (uint32_t)(fre_clust * pfs->csize * 0.5);
#if FILEHANDLINGDEBUG
    buf = malloc(30*sizeof(char));
    sprintf (buf, "SD CARD Free Space: \t%lu\n",free_space);
    FileHandling_Send(buf);
    free(buf);
#endif
    return free_space;
}

#endif

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **																FILE INFORMATION													   **
 **																																	   **
 **		FILENAME -- R4.c																											   **	
 **      AUTHORS -- Jonroy Canaday, Adam Trainer, Robert Wayland																	   **
 **      VERSION -- 1.0																												   **
 ** LAST UPDATED -- March 08, 2010																									   **
 **																																	   **
 **   COMPONENTS --  load_prog, terminate, load																						   **				   **
 **																																	   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **																   CHANGE LOG														   **
 **																																	   **
 **		  03/08/2010  JC          template of R4.c created																			   **
 **       03/10/2010  JC, RW      load_prog, terminate, load completed																	   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */

/* Included ANSI C Files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>

/* Included Support Files */
#include "mpx_supt.h"
#include "R12.h"

/* Macros */
#define FILE_LENGTH = 9;

/* Function Prototypes */
int load_prog(char fname[], int pri);
int terminate();
int load();

/* Global Variables */
int err4 = 0;

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- load_prog																							       **
 **               Purpose -- The load_prog function allocates and setups a new PCB. In addition, it allocates program memory using the **
 **							 support procedure sys_alloc_mem, and loads a program into that memory using the support procedure		   **
 **					         sys_load_program.																						   **
 **            Parameters -- char [], int																							   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- sys_check_program, allocate_PCB, setup_PCB, sys_alloc_mem, sys_load_program, insert					   **
 **  Global Data Accessed -- err4 																								       **
 **  Summary of Algorithm -- The load_prog function checks arguments, checks program, create and setups a PCB, allocates program	   **
 **							 memory, and loads the program.																			   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int load_prog(char fname[], int pri) 
{
	int offset_p;
	int progLength;
	struct PCB *newNode;
	struct context *cp;
	err4 = 0;

	if (pri < -127 || pri > 127) 
	{
		pri = 0;
        printf("\nInvalid priority level.  Priority defaulted to 0.");
    }
	
    newNode = findPCB(fname, newNode);
    
	if (newNode != NULL) return ERR_NAMEAE;

	err4 = sys_check_program("PROCS", fname, &progLength, &offset_p);
	
	if (err4 < OK) return err4;
	
	if (NULL == (newNode = allocate_PCB())) return ERR_UCPCB;

	setup_PCB(newNode,fname,APP,pri);
	newNode->suspended = SUSP;

    newNode->mem_size		   = progLength;
	newNode->load_address	   = (unsigned char*)sys_alloc_mem(progLength);
	newNode->execution_address = newNode->load_address + offset_p;

	cp        = (struct context *)newNode->stack_top;
	cp->IP    = FP_OFF(newNode->execution_address);
	cp->CS    = FP_SEG(newNode->execution_address);
	cp->DS    = _DS;
	cp->ES    = _ES;
	cp->FLAGS = 0x200;
	
	err4 = sys_load_program(newNode->load_address, newNode->mem_size, "PROCS", fname);
	
	if (err4>=OK) err4 = insert(newNode,RUNNING);
	
	return err4;
}

/**
 */
int load() {
    char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	char name[PROCESS_NAME_LENGTH];
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);
	
	err4 = 0;
    printf("\nPlease enter the name of the process to be created (9 char limit; no extension): ");
	err4 = sys_req(READ, TERMINAL, buff, &buffsize);
	if (err4 < OK) return err4;
	trimx(buff);
    if (strlen(buff)>9) return ERR_PRONTL;
    temppcb = findPCB(buff,temppcb);
	if (temppcb != NULL) return ERR_NAMEAE;
    strncpy(name,buff,PROCESS_NAME_LENGTH);
    load_prog(name,0);
    return err4;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- terminate																							       **
 **               Purpose -- The terminate function terminates a process by deallocating its PCB and releasing its allocated		   **
 **							program memory.																							   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- memset, printf, sys_req, trimx, qRemove, free_PCB														   **
 **  Global Data Accessed -- err4																									   **
 **  Summary of Algorithm -- The terminate function prompts the user for the name of the process to be terminated, processes the user  **
 **							 input, removes it from the queue, and deallocates the memory.											   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int terminate() 
{
    char buff[BIGBUFF];
	struct PCB *tmp = NULL;
	int buffsize    = BIGBUFF;
	memset(buff, '\0', BIGBUFF);

    err4 = 0;
	printf("Please enter the name of the process to delete: ");
	err4 = sys_req(READ, TERMINAL, buff, &buffsize);	
	
	if (err4 >= OK) 
	{
		if (tmp->proc_class != SYSTEM) 
		{
		  trimx(buff);
		  tmp = qRemove(buff,tmp);
		  free_PCB(tmp);
		  printf("\nTermination successful!");
		}
		
		else return ERR_UTDSC;
	}
	
	return err4;
}
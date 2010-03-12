/**
*/
#include "R12.h"
#include "mpx_supt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILE_LENGTH = 9;

void load_prog(char fname[], int pri);
int terminate();
int load();

int err4 = 0;

void load_prog(char fname[], int pri) {
	int offset_p;
	int progLength;
	struct PCB *newNode;
	struct context *cp;

	if(pri < -127 || pri > 127) {
		pri = 0;
        printf("\nInvalid priority level.  Priority defaulted to 0.");
    }
    newNode = findPCB(fname, newNode);
    if(newNode != NULL) return ERR_NAMEAE;

	err4 = sys_check_program("\PROCS",fname,&progLength,&offset_p)
	if(err < OK)
		return err4;
	if(NULL == (newNode = allocate_PCB)
		return ERR_UCPCB;

	setup_PCB(newNode,fname,APPLICATION,pri);
	newNode->suspended = 1;

    newNode->mem_size = progLength;
	newNode->load_address = (unsigned char*)sys_alloc_mem(progLength);
	newNode->execution_address = load_address + offset_p;

	cp = (struct context *)newNode->stack_top;
	cp->IP = FP_OFF(newNode->execution_address);
	cp->CS = FP_SEG(newNode->execution_address);
	cp->DS = _DS;
	cp->ES = _ES;
	cp->flags = 0x200;
	
	err4 = sys_load_program(newNode->load_address, newNode->mem_size, "\PROCS",fname);
	if(err4>=OK) err4 = insert(newNode,RUNNING);
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
    printf("\nPlease enter the name of the process to be created (9 character limit; no extension): ");
	err4 = sys_req(READ, TERMINAL, buff, &buffsize);
	if (err4 < OK) return errx;
	trimx(buff);
    if (strlen(buff)>9) return ERR_PRONTL;
    temppcb = findPCB(buff,temppcb);
	if (temppcb != NULL) return ERR_NAMEAE;
    strncpy(name,buff,PROCESS_NAME_LENGTH);
    load_prog(name,0);
    return err4;
}

/**
 */	
int terminate() {
    char buff[BIGBUFF];
	struct PCB *tmp = NULL;
	int buffsize = BIGBUFF;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter the name of the process to delete: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);	
	if(errx >= OK) {
		if(tmp->proc_class != SYSTEM) {
		  trimx(buff);
		  tmp = qRemove(buff,tmp);
		  free_PCB(tmp);
		  printf("\nTermination successful!");
		}
		else return ERR_UTDSC;
	}
	return errx;
}




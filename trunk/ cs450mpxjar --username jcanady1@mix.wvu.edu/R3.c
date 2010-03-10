/**
* File Name: 
* \title JAROS
* \author Jonroy Canady
* \author Adam Trainer
* \author Rob Wayland   
* \date 3/8/2010
* \version: 1.0
*
* Components: 
*
*******************************************************************************
* Change Log:
*
*        3/8/2010  JC           File created with placeholders and dispatch started
*/

// Included ANSI C Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Included Support Files
#include "mpx_supt.h"
#include <dos.h>

// Constants
#define BIGBUFF 80
#define SMALLBUFF 10
#define TINYBUFF 2
#define OK 0

/* Service operation codes */
#define IDLE	0
#define	READ	1
#define	WRITE	2
#define CLEAR	3
#define GOTOXY	4
#define EXIT	5

/* Device ID codes */
#define NO_DEV		0
#define	TERMINAL	1
#define	PRINTER		2
#define	COM_PORT	3
#define NUM_DEVS	3

// Status and Error Codes
#define ERR_INVCOM (-201)    // Invalid command
#define ERR_INVYR  (-202)    // Invalid year
#define ERR_INVMON (-203)    // Invalid month
#define ERR_INVDAY (-204)    // Invalid day
#define ERR_PCBNF  (-205)    //PCB Not Found
#define ERR_QUEEMP (-206)    //Queue is Empty
#define ERR_UCPCB  (-207)    //Unable to Create PCB
#define ERR_PRONTL (-208)    //Process Name too Long
#define ERR_NAMEAE (-209)    //Process Name Already Exists
#define ERR_INVCLS (-210)    //Invalid Class

// Constants


// Global Variables
int err = 0;

// Structures
typedef struct params {
	int op_code;
	int device_id;
	byte *buf_addr;
	int *count_addr;
}params;
params *param_P;

PCB *cop;

// Function Prototypes
int init_r3();
int cleanup_r3();


/** Procedure Name: init_r3
*/
int init_r3() {
    return 0;
}

/** Procedure Name: init_r3
*/
int cleanup_r3() {
}




/*static unsigned short ss_save;
static unsigned short sp_save;
static unsigned short ss_save_temp;
static unsigned short sp_save_temp;
static unsigned short new_ss;
static unsigned short new_sp;
static unsigned char sys_stack[STACK_SIZE];
static pcb_node *cop;
static pcb_node* tempnode;


/**
  * \brief Assigns the next ready, non-suspended process to be the current operating process.
  * Searches ready queue for the next non-suspended process. When found, removes the process from the queue.
  * <BR>Then, assigns to the COP and changes state to running. Finally, performs a context switch and completes.
  */
void interrupt dispatcher() {
	if(sp_save == NULL) {
		ss_save = _SS;
		sp_save = _SP;
	}
	tempnode = getReadyHead();	
	while(tempnode != NULL) { //scan for a non-suspended process
		if(tempnode->process_suspended == PROC_NOT_SUSP) break;
		tempnode = tempnode -> next;
	}
	if(tempnode != NULL) { //found a ready, non-suspended process
		cop = tempnode;
		removePCB(tempnode);
		//reset tempnode so it doesn't interfere with other interrupt/function calls
		tempnode = NULL;
		cop -> process_state = PROC_RUNNING;
		new_ss = FP_SEG(cop -> stack_base);
		new_sp = FP_OFF(cop -> stack_top);
		_SS = new_ss;
		_SP = new_sp;
	}
	else { //no ready, unsuspended processes; restore state
		cop = NULL;
		_SS = ss_save;
		_SP = sp_save;
		ss_save = NULL;
		sp_save = NULL;
	}
}
void interrupt sys_call()
{	
	param_p = (params*)(MK_FP(_SS,_SP)) + sizeof(context));

	if(param_p->op_code == IDLE)
	{
		insert(runPCB,1)
		cop = NULL;
	}
	else if(param_p->code == EXIT)
	{
		free_pcb(runPCB);
		cop = NULL;
	}
	else
		context_p->AX = param_p->op_code;

	dispatcher();
}

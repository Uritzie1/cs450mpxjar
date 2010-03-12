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
*        3/08/2010  JC           File created with placeholders and dispatch started
*        3/10/2010  JC, RW       Finished writing dispatch and sys_call
*/

// Included ANSI C Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Included Support Files
#include "mpx_supt.h"
#include <dos.h>
#include "R12.h"

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


// Structures
typedef struct context {
	unsigned int BP, DI, SI, DS, ES;
	unsigned int DX, CX, BX, AX;
	unsigned int IP, CS, FLAGS;
} context;

// Function Prototypes
int init_r3();
int cleanup_r3();

// Global Variables
int err = 0;
static unsigned short ss_save;
static unsigned short sp_save;
static unsigned short ss_save_temp;
static unsigned short sp_save_temp;
static unsigned short new_ss;
static unsigned short new_sp;
static unsigned char sys_stack[STACK_SIZE];
static struct PCB *cop;
static struct PCB *tempnode;
struct context *context_p;
struct params *param_p;

/** Procedure Name: init_r3
*/
int init_r3() {
    return 0;
}

/** Procedure Name: init_r3
*/
int cleanup_r3() {
}

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
	tempnode = head1;	
	while(tempnode != NULL) { //look for a non-suspended process
		if(tempnode->suspended == NOTSUSP) break;
		tempnode = tempnode -> next;
	}
	if(tempnode != NULL) { //found a ready, non-suspended process
		cop = qRemove(tempnode->name,tempnode);
		//reset tempnode so it doesn't interfere with other interrupt/function calls
		tempnode = NULL;
		cop->state = RUNNING;
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

/**
 */
void interrupt sys_call() {	
	ss_save = _SS;
	sp_save = _SP;
	new_ss = FP_SEG(sys_stack);
	new_sp = FP_OFF(sys_stack);
	new_sp += SYS_STACK_SIZE;
	_SS = new_ss;
	_SP = new_sp;

	param_p = (params*)(MK_FP(_SS,_SP)) + sizeof(context));

	if(param_p->op_code == IDLE)
	{
		insert(cop,1)
		cop = NULL;
	}
	else if(param_p->code == EXIT)
	{
		free_pcb(cop);
		cop = NULL;
	}
	else
		context_p->AX = param_p->op_code;

	dispatcher();
}

/**
 */
int load_test() {	
	PCB *np;
	context *npc;

	np = allocate_PCB();
	if (np == NULL) errx = ERR_UCPCB;
	else {
		errx = setup_PCB(np, "test1",0,0);
		if (errx < OK) return errx;{
			npc = (context*) np->stack_top;
			npc->IP = FP_OFF(&test1_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test1_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			errx = insert(np,RUNNING);}
	}

	np = allocate_PCB();
	if (np == NULL) errx = ERR_UCPCB;
	else {
		errx = setup_PCB(np, "test2",0,0);
		if (errx < OK) return errx;{
			npc = (context*) np->stack_top;
			npc->IP = FP_OFF(&test2_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test2_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			errx = insert(np,RUNNING);}
	}
	
	np = allocate_PCB();
	if (np == NULL) errx = ERR_UCPCB;
	else {
		errx = setup_PCB(np, "test3",0,0);
		if (errx < OK) return errx;{
			npc = (context*) np->stack_top;
			npc->IP = FP_OFF(&test3_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test3_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			errx = insert(np,RUNNING);}
	}
	
	np = allocate_PCB();
	if (np == NULL) errx = ERR_UCPCB;
	else {
		errx = setup_PCB(np, "test4",0,0);
		if (errx < OK) return errx;{
			npc = (context*) np->stack_top;
			npc->IP = FP_OFF(&test4_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test4_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			errx = insert(np,RUNNING);}
	}
	
	np = allocate_PCB();
	if (np == NULL) errx = ERR_UCPCB;
	else {
		errx = setup_PCB(np, "test5",0,0);
		if (errx < OK) return errx;{
			npc = (context*) np->stack_top;
			npc->IP = FP_OFF(&test5_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test5_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			errx = insert(np,RUNNING);}
	}
}


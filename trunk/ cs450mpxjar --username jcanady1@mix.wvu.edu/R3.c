/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **																FILE INFORMATION													   **
 **																																	   **
 **		FILENAME -- R3.c																											   **	
 **      AUTHORS -- Jonroy Canaday, Adam Trainer, Robert Wayland																	   **
 **      VERSION -- 1.0																												   **
 ** LAST UPDATED -- March 08, 2010																									   **
 **																																	   **
 **   COMPONENTS --  init_r3, cleanup_r3, test1_R3, test2_R3, test3_R3, test4_R3, test5_R3, interrupt sys_call, interrupt dispatcher   **
 **					 load_test																										   **
 **																																	   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **																   CHANGE LOG														   **
 **																																	   **
 **		  03/08/2010  JC          template of R3.c created																			   **
 **       03/10/2010  JC, RW      dispatch and sys_call completed																	   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */

/* Included ANSI C Files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Included Support Files */
#include "mpx_supt.h"
#include <dos.h>
#include "R12.h"

/* Macros */
#define BIGBUFF 80
#define SMALLBUFF 10
#define TINYBUFF 2
#define OK 0

/* Service Operation Codes */
#define IDLE	0
#define	READ	1
#define	WRITE	2
#define CLEAR	3
#define GOTOXY	4
#define EXIT	5

/* Device ID Codes */
#define NO_DEV		0
#define	TERMINAL	1
#define	PRINTER		2
#define	COM_PORT	3
#define NUM_DEVS	3

/* Status and Error Codes */
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



/* Data Structures */
typedef struct context 
{
	unsigned int BP, DI, SI, DS, ES;
	unsigned int DX, CX, BX, AX;
	unsigned int IP, CS, FLAGS;
};

/* Function Prototypes */
int init_r3();
int cleanup_r3();
void test1_R3();
void test2_R3();
void test3_R3();
void test4_R3();
void test5_R3();
void interrupt sys_call();
void interrupt dispatcher();
int load_test();

/* Global Variables */
static unsigned short ss_save;
static unsigned short sp_save;
static unsigned short ss_save_temp;
static unsigned short sp_save_temp;
static unsigned short new_ss;
static unsigned short new_sp;
static unsigned char sys_stack[SYS_STACK_SIZE];
static struct PCB *cop;
static struct PCB *tempnode;
struct context *context_p;
struct params *param_p;
int err3 = 0;

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- init_r3																							       **
 **               Purpose -- The init_r3 function initializes all global variables for R3.											   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int																									   **
 **     Procedures Called -- sys_set_vec																							   **
 **  Global Data Accessed -- ss_save, sp_save, new_ss, new_sp, cop, tempnode													       **
 **  Summary of Algorithm -- The init_r3 function initializes all global variables for R3.											   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int init_r3() 
{
	ss_save  = NULL;
	sp_save  = NULL;
	new_ss   = NULL;
	new_sp   = NULL;
	cop      = NULL;
	tempnode = NULL;
	sys_set_vec(sys_call);
	
    return 0;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- cleanup_r3																							       **
 **               Purpose -- The cleanup_r3 function does nothing.																	   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int																									   **
 **     Procedures Called -- N/A																									   **
 **  Global Data Accessed -- N/A																								       **
 **  Summary of Algorithm -- The cleanup_r3 function does nothing.																				   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int cleanup_r3() 
{
    return 0;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- interrupt dispatcher																					   **
 **               Purpose -- The interrupt dispatcher function identifies the next READY process, if any, and dispatches it by		   **
 **							 loading its context from its PCB.																		   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- void																									   **
 **     Procedures Called -- getRHead, qRemove																						   **
 **  Global Data Accessed -- sp_save, ss_save, tempnode, cop, new_ss, new_sp, _SS, _SP											       **
 **  Summary of Algorithm -- The interrupt dispatcher function first looks for a non-SUSPENDED process.  If it finds a READY process,  **
 **							 it removes it from the queue and changes the process' state to RUNNING.  If it doesn't find a READY	   **
 **							 process, it restores the process' state.																   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
void interrupt dispatcher() 
{
	if (sp_save == NULL) 
	{
		ss_save = _SS;
		sp_save = _SP;
	}
	
	tempnode = getRHead();	
	
	while (tempnode != NULL) //look for a non-suspended process
	{ 
		if (tempnode->suspended == NOTSUSP) break;
		tempnode = tempnode -> next;
	}
	
	if (tempnode != NULL) //found a ready, non-suspended process
	{ 
		cop = qRemove(tempnode->name,cop);
		//reset tempnode so it doesn't interfere with other interrupt/function calls
		tempnode   = NULL;
		cop->state = RUNNING;
		new_ss	   = FP_SEG(cop -> stack_base);
		new_sp	   = FP_OFF(cop -> stack_top);
		_SS        = new_ss;
		_SP		   = new_sp;
	}
	
	else //no ready, unsuspended processes; restore state
	{ 
		cop      = NULL;
		tempnode = NULL;
		_SS      = ss_save;
		_SP      = sp_save;
		ss_save  = NULL;
		sp_save  = NULL;
	}
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- interrupt sys_call																						   ** 
 **               Purpose -- The interrupt sys_call interprets system call parameters and invokes the dispatcher as necessary.		   **										  
 **            Parameters -- N/A																									   **
 **			 Return Value -- void																									   **
 **     Procedures Called -- sizeof, insert, free_pcb, dispatcher																	   **							   
 **  Global Data Accessed -- ss_save_temp, sp_save_temp, new_ss, new_sp, _SS, _SP												       **
 **  Summary of Algorithm -- The interrupt sys_call interprets system call parameters and invokes the dispatcher as necessary.		   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
void interrupt sys_call() 
{	
	ss_save_temp = _SS;
	sp_save_temp = _SP;
	param_p		 = (struct params *)((unsigned char *)MK_FP(ss_save_temp, sp_save_temp) + sizeof(struct context));
    new_ss		 = FP_SEG(sys_stack);
	new_sp		 = FP_OFF(sys_stack) + SYS_STACK_SIZE - 1;
    _SS			 = new_ss;
	_SP			 = new_sp;
	
	if (param_p->op_code == IDLE)
	{
		cop->state = READY;
        insert(cop,1);
		cop = NULL;
	}
	
	else if (param_p->op_code == EXIT)
	{
		free_pcb(cop);
		cop = NULL;
	}
	
	else context_p->AX = param_p->op_code;

	dispatcher();
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- load_test																								   ** 
 **               Purpose -- The load_test function provides test cases for R3.														   **										   
 **            Parameters -- N/A																									   **
 **			 Return Value -- int																									   **
 **     Procedures Called -- allocate_PCB, setup_PCB, insert																		   **							   
 **  Global Data Accessed -- err3																									   **
 **  Summary of Algorithm -- The load_test function provides test cases for R3.														   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int load_test() 
{	
	struct PCB *np;
	struct context *npc;

	np = allocate_PCB();
	
	if (np == NULL) err3 = ERR_UCPCB;
	
	else 
	{
		err3 = setup_PCB(np, "test1", 0, 0);
		
		if (err3 < OK) return err3;
		
		npc                   = (struct context*) np->stack_top;
		npc->IP               = FP_OFF(&test1_R3); 
		np->execution_address = FP_OFF(&test1_R3);
		npc->CS				  = FP_SEG(&test1_R3);
		npc->FLAGS			  = 0x200;
		npc->DS			      = _DS;
		npc->ES				  = _ES;
		err3			      = insert(np, RUNNING);
	}

	np = allocate_PCB();
	
	if (np == NULL) err3 = ERR_UCPCB;
	else 
	{
		err3 = setup_PCB(np, "test2", 0, 0);
		
		if (err3 < OK) return err3;
		
		npc                   = (struct context*) np->stack_top;
		npc->IP               = FP_OFF(&test2_R3); 
		np->execution_address = FP_OFF(&test2_R3);
		npc->CS               = FP_SEG(&test2_R3);
		npc->FLAGS            = 0x200;
		npc->DS				  = _DS;
		npc->ES				  = _ES;
		err3			      = insert(np,RUNNING);
	}
	
	np = allocate_PCB();
	
	if (np == NULL) err3 = ERR_UCPCB;
	
	else 
	{
		err3 = setup_PCB(np, "test3", 0, 0);
		
		if (err3 < OK) return err3;
		
		npc                   = (struct context*) np->stack_top;
		npc->IP				  = FP_OFF(&test3_R3); 
		np->execution_address = FP_OFF(&test3_R3);
		npc->CS				  = FP_SEG(&test3_R3);
		npc->FLAGS			  = 0x200;
		npc->DS				  = _DS;
		npc->ES				  = _ES;
		err3				  = insert(np,RUNNING);
	}
	
	np = allocate_PCB();
	
	if (np == NULL) err3 = ERR_UCPCB;
	
	else 
	{
		err3 = setup_PCB(np, "test4", 0, 0);
		
		if (err3 < OK) return err3;
		
		npc					  = (struct context*) np->stack_top;
		npc->IP               = FP_OFF(&test4_R3); 
		np->execution_address = FP_OFF(&test4_R3);
		npc->CS				  = FP_SEG(&test4_R3);
		npc->FLAGS			  = 0x200;
		npc->DS				  = _DS;
		npc->ES				  = _ES;
		err3				  = insert(np,RUNNING);
	}
	
	np = allocate_PCB();
	
	if (np == NULL) err3 = ERR_UCPCB;
	
	else 
	{
		err3 = setup_PCB(np, "test5", 0, 0);
		
		if (err3 < OK) return err3;
		
		npc					  = (struct context*) np->stack_top;
		npc->IP				  = FP_OFF(&test5_R3); 
		np->execution_address = FP_OFF(&test5_R3);
		npc->CS				  = FP_SEG(&test5_R3);
		npc->FLAGS			  = 0x200;
		npc->DS				  = _DS;
		npc->ES				  = _ES;
		err3				  = insert(np,RUNNING);
	}
	
	return 0;
}
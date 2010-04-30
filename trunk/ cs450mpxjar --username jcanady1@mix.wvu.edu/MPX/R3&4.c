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
 **       04/29/2010  JC, RW      upgrade to R6: sys_call mods, IOschedule, enqueue, dequeue
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */

// Included ANSI C Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Included Support Files
#include "mpx_supt.h"
#include "trmdrive.h"
#include "MPXlump.h"
#include <dos.h>


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
int init_r3() {
	ss_save = NULL;
	sp_save = NULL;
	new_ss = NULL;
	new_sp = NULL;
	cop = NULL;
	tempnode = NULL;
	err3 = sys_set_vec(sys_call);
	if(err3<OK) err_hand(err3);
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
void interrupt dispatcher() {
	if(sp_save == NULL) {
		ss_save = _SS;
		sp_save = _SP;
	}
	tempnode = getRHead();	
	while(tempnode != NULL) { //look for a non-suspended process
		if(tempnode->suspended == NOTSUSP) break;
		tempnode = tempnode -> prev;
	}
	if(tempnode != NULL) { //found a ready, non-suspended process
		cop = qRemove(tempnode->name,cop);
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
		tempnode = NULL;
		_SS = ss_save;
		_SP = sp_save;
		ss_save = NULL;
		sp_save = NULL;
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
void interrupt sys_call() {	
	ss_save_temp = _SS;
	sp_save_temp = _SP;
    
    cop->stack_top = (unsigned char *)MK_FP(ss_save_temp, sp_save_temp);
    /*new_ss = FP_SEG(sys_stack);
	new_sp = FP_OFF(sys_stack)+SYS_STACK_SIZE;
    _SS = new_ss;
	_SP = new_sp;*/
	param_p = (params *)(cop -> stack_top + sizeof(struct context));
	
	
    trm_getc();
    //check for comport request completion
	if(*(comport->event_flag) == 1 && comport->active != NULL) {
        *(comport->event_flag) = 0;
        tmpIOD = dequeue(comport);
        tempnode = qRemove((tmpIOD->requestor)->name, tempnode);
        tempnode->state = READY;
	    insert(tempnode, READY+1);	    
        //process nxt IO req for this dev
        if(comport->count > 0) process_com();
	}
	//check for terminal request completion
	if(*(terminal->event_flag) == 1) {
		*(terminal->event_flag) = 0;
		tmpIOD = dequeue(terminal);
        tempnode = qRemove((tmpIOD->requestor)->name, tempnode);
        tempnode->state = READY;
	    insert(tempnode, READY+1);	    
        //process nxt IO req for this dev
        if(terminal->count > 0) process_trm();
	}
		
	if(param_p->op_code == IDLE) {
        cop->state = READY;
		insert(cop,READY+1);
		cop = NULL;
	}
	else if(param_p->op_code == EXIT) {
		free_pcb(cop);
		cop = NULL;
	}
	else if(param_p->op_code == READ || param_p->op_code == WRITE || param_p->op_code == CLEAR || param_p->op_code == GOTOXY) {
      IOschedule();
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
int load_test() {	
	struct PCB *np;
	struct context *npc;

	findPCB("test1",np);
	if(errx == ERR_PCBNF){
		np = allocate_PCB();
		if (np == NULL) err3 = ERR_UCPCB;
		else {
			err3 = setup_PCB(np, "test1",APP,0);
			if (err3 < OK) return err3;
			npc = (struct context*) np->stack_top;
			npc->IP = FP_OFF(&test1_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test1_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			err3 = insert(np,READY+1);
		}
	}
	else printf("\nProcess with name 'test1' already exists.");

	findPCB("test2",np);
	if(errx == ERR_PCBNF){
		np = allocate_PCB();
		if (np == NULL) err3 = ERR_UCPCB;
		else {
			err3 = setup_PCB(np, "test2",APP,0);
			if (err3 < OK) return err3;
			npc = (struct context*) np->stack_top;
			npc->IP = FP_OFF(&test2_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test2_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			err3 = insert(np,READY+1);
		}
	}
	else printf("\nProcess with name 'test2' already exists.");

	findPCB("test3",np);
	if(errx == ERR_PCBNF){
		np = allocate_PCB();
		if (np == NULL) err3 = ERR_UCPCB;
		else {
			err3 = setup_PCB(np, "test3",APP,0);
			if (err3 < OK) return err3;
			npc = (struct context*) np->stack_top;
			npc->IP = FP_OFF(&test3_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test3_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			err3 = insert(np,READY+1);
		}
	}
	else printf("\nProcess with name 'test3' already exists.");

	findPCB("test4",np);
	if(errx == ERR_PCBNF){
		np = allocate_PCB();
		if (np == NULL) err3 = ERR_UCPCB;
		else {
			err3 = setup_PCB(np, "test4",APP,0);
			if (err3 < OK) return err3;
			npc = (struct context*) np->stack_top;
			npc->IP = FP_OFF(&test4_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test4_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			err3 = insert(np,READY+1);
		}
	}
    else printf("\nProcess with name 'test4' already exists.");

	findPCB("test5",np);
	if(errx == ERR_PCBNF){
		np = allocate_PCB();
		if (np == NULL) err3 = ERR_UCPCB;
		else {
			err3 = setup_PCB(np, "test5",APP,0);
			if (err3 < OK) return err3;
			npc = (struct context*) np->stack_top;
			npc->IP = FP_OFF(&test5_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test5_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			err3 = insert(np,READY+1);
		}
	}
	else printf("\nProcess with name 'test5' already exists.");
	if(err3==OK) printf("\nTest processes loaded successfully!");
	return err3;
}

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
int load_prog(char * fname, int pri, int procClass) {
	int offset_p;
	int progLength;
	struct PCB *newNode;
	struct context *cp;
	err4 = 0;

	if(pri < -128 || pri > 127) {
		pri = 0;
        printf("\nInvalid priority level.  Priority defaulted to 0.");
    }
    newNode = findPCB(fname, newNode);
    if(newNode != NULL) return ERR_NAMEAE;

	err4 = sys_check_program("MPXFILES",fname,&progLength,&offset_p);
	if(err4 < OK) return err4;
	if(NULL == (newNode = allocate_PCB())) return ERR_UCPCB;

	setup_PCB(newNode,fname,procClass,pri);
	if(!strncmp(fname, "idle", 5)) newNode->suspended = NOTSUSP;
	else newNode->suspended = SUSP;

    newNode->mem_size = progLength;
	newNode->load_address = (unsigned char*)sys_alloc_mem(progLength);
	newNode->execution_address = newNode->load_address + offset_p;

	cp = (struct context *)newNode->stack_top;
	cp->IP = FP_OFF(newNode->execution_address);
	cp->CS = FP_SEG(newNode->execution_address);
	cp->DS = _DS;
	cp->ES = _ES;
	cp->FLAGS = 0x200;
	
	err4 = sys_load_program(newNode->load_address, newNode->mem_size, "MPXFILES",fname);
	if(err4>=OK) err4 = insert(newNode,READY+1);
	if(strncmp(fname, "idle", 5)) {if(err4>=OK) printf("Program successfully loaded!");}
	return err4;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- load																							           **
 **               Purpose -- The load function gives the user an interface to the load_prog function in order to allocate and setup    **
 **                          a new PCB.         																					   **
 **            Parameters -- none																		        					   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- sys_req, trim, toLowerCase, findPCB, load_prog		       			                                       **
 **  Global Data Accessed -- err4 																								       **
 **  Summary of Algorithm -- The load function takes in user input in the form of a process name and uses that to call load_prog.	   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
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
	trim(buff);
	toLowerCase(buff);
    if (strlen(buff)>9) return ERR_PRONTL;
    temppcb = findPCB(buff,temppcb);
	if (temppcb != NULL) return ERR_NAMEAE;
    strncpy(name,buff,PROCESS_NAME_LENGTH);
    load_prog(name,0,APP);
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
int terminate() {
    char buff[BIGBUFF];
	struct PCB *tmp = NULL;
	int buffsize = BIGBUFF;
	memset(buff, '\0', BIGBUFF);

    err4 = 0;
	printf("Please enter the name of the process to delete: ");
	err4 = sys_req(READ, TERMINAL, buff, &buffsize);
    trimx(buff);
    toLowerCase(buff);	
	if(err4 >= OK) {
        tmp = findPCB(buff,tmp);
		if(tmp->proc_class != SYSTEM) { 
		  tmp = qRemove(buff,tmp);
		  free_PCB(tmp);
		  printf("\nTermination successful!");
		}
		else return ERR_UTDSC;
	}
	return err4;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- init_f																							           **
 **               Purpose -- Initializes the terminal and comport devices for R6.        											   **
 **            Parameters -- none																		        					   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- trm_open, com_open                          		       			                                       **
 **  Global Data Accessed -- none																								       **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int init_f() {
  tmpIOD=NULL;  
  *(terminal->event_flag) = 1;
  terminal->count = 0;
  terminal->head = NULL;
  terminal->tail = NULL;
	
  *(comport->event_flag) = 1;
  comport->count = 0;
  comport->head = NULL;
  comport->tail = NULL;
  
  trm_open(terminal->event_flag);
  com_open(comport->event_flag, 1200);
  return 0;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- cleanup_f																						           **
 **               Purpose -- Closes the terminal and comport devices and frees the IODs.       										   **
 **            Parameters -- none																		        					   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- trm_close, com_close, sys_free_mem	       			                                                       **
 **  Global Data Accessed -- none																								       **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int cleanup_f() {
  trm_close();
  com_close();

  //free IODs
  tmpIOD = comport->head;
  while (tmpIOD != NULL) {
    comport->head = (comport->head)->next;
    sys_free_mem(tmpIOD);
    tmpIOD = comport->head;   
  }
  tmpIOD = terminal->head;
  while (tmpIOD != NULL) {
    terminal->head = (terminal->head)->next;
    sys_free_mem(tmpIOD);
    tmpIOD = terminal->head;
  } 
  return OK;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- IOschedule																				    	           **
 **               Purpose -- Schedules I/O interactions with the terminal and comport devices.        								   **
 **            Parameters -- none																		        					   **
 **			 Return Value -- none                																					   **
 **     Procedures Called -- createIOD, insert, enqueue, process_com, process_trm		       	                                       **
 **  Global Data Accessed -- cop 																								       **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
void IOschedule() {
	int retq = 0;
	int dev_id = param_p->device_id;
	struct IOD * newIOD = createIOD();

    cop->state = BLOCKED;
    insert(cop, BLOCKED);

	if(dev_id == COM_PORT) {
		retq = enqueue(newIOD,comport);
		if(retq == 1) process_com();
	}
	else if(dev_id == TERMINAL) {
	    retq = enqueue(newIOD,terminal);
		if(retq == 1) process_trm();
    }
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- process_com																     				           **
 **               Purpose -- Processes I/O requests for the comport.        	    												   **
 **            Parameters -- none																		        					   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- com_read, com_write                             	       			                                       **
 **  Global Data Accessed -- none 																								       **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int process_com() {
  *(comport->event_flag) = 0;
  switch((comport->head)->request) {
  case READ: {
		com_read((comport->head)->tran_buff, (comport->head)->buff_count);
		break;}
  case WRITE: {
		com_write((comport->head)->tran_buff, (comport->head)->buff_count);
		break;}
  default: {
		return ERR_UNKN_REQUEST;}
  }
  return OK;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- process_trm																					           **
 **               Purpose -- Processes I/O requests for the terminal device.        												   **
 **            Parameters -- none																		        					   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- trm_read, trm_write, trm_clear, trm_gotoxy     	       			                                       **
 **  Global Data Accessed -- none 																								       **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int process_trm() {
  *(terminal->event_flag) = 0;
  switch((terminal->head)->request) {
  case READ: {
		trm_read((terminal->head)->tran_buff, (terminal->head)->buff_count);
		break;}
  case WRITE: {
		trm_write((terminal->head)->tran_buff, (terminal->head)->buff_count);
		break;}
  case CLEAR: {
        trm_clear();
		break;}
  case GOTOXY: {
        trm_gotoxy(0,0);  //NOT DONE!
		break;}
  default: {
		return ERR_UNKN_REQUEST;}
  }
  return OK;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- enqueue																						           **
 **               Purpose -- Enqueues an IOD to the queue of an IOCB in FIFO order.     											   **
 **            Parameters -- IOD pointer, IOCB pointer													        					   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- none                                              	       			                                       **
 **  Global Data Accessed -- none																								       **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int enqueue(struct IOD * nIOD, struct IOCB * queue) {
	int retv = 0;

	if(queue->count == 0) {
		queue->head = nIOD;
		queue->tail = nIOD;
		retv = 1;
	}
	else {
		(queue->tail)->next = nIOD;
		queue->tail = nIOD;
	}
	queue->count++;
	return retv;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- dequeue																						           **
 **               Purpose -- Dequeues an IOD from the queue of an IOCB in FIFO order.     											   **
 **            Parameters -- IOCB pointer           													        					   **
 **			 Return Value -- none						                															   **
 **     Procedures Called -- none                                              	       			                                       **
 **  Global Data Accessed -- none																								       **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
struct IOD * dequeue(struct IOCB * queue) {
	struct IOD * tempIOD;
	tempIOD = queue->head;

    if(queue->count == 0) return NULL;
	if(queue->count == 1) {
	 queue->head = NULL;
	 queue->tail = NULL;
	}
	else queue->head = (queue->head)->next;
	queue->count--;

	return tempIOD;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- createIOD																						           **
 **               Purpose -- Sets up an IOD and returns it.              															   **
 **            Parameters -- none                     													        					   **
 **			 Return Value -- IOD pointer         																					   **
 **     Procedures Called -- sys_alloc_mem                                    	       			                                       **
 **  Global Data Accessed -- none																								       **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
struct IOD * createIOD() {
	struct IOD *newIOD = NULL;
	newIOD = (struct IOD *)sys_alloc_mem((sizeof(struct IOD)));
	//newIOD->name = cop->name;
	strncpy(newIOD->name, cop->name, PROCESS_NAME_LENGTH);
	newIOD->requestor = cop;
	newIOD->tran_buff = param_p->buf_p;
	newIOD->buff_count = param_p->count_p;
	newIOD->request = param_p->op_code;
	newIOD->next = NULL;
	return newIOD;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **																FILE INFORMATION													   **
 **																																	   **
 **		FILENAME -- R2.c																											   **	
 **      AUTHORS -- Jonroy Canaday, Adam Trainer, Robert Wayland																	   **
 **      VERSION -- 2.71																											   **
 ** LAST UPDATED -- April 24, 2010																									   **
 **																																	   **
 **   COMPONENTS --  allocate_PCB, block, cleanup_r2, create_PCB, delete_PCB, findPCB, free_PCB, getRHead, init_r2, insert, isEmpty,   ** 
 **					 resume, qRemove, set_Priority, setup_PCB, show_All, show_Blocked, show_PCB, show_Ready, suspend unblock,		   **
 **					 toLowerCase, trim 																					           **
 **																																	   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **																   CHANGE LOG														   **
 **																																	   **
 **		  02/12/2010  JC           template of R2.c created																			   **
 **       02/15/2010  JC           block, unblock, suspend, resume, set_Priority, show_PCB, show_All, show_Ready, show_Blocked		   **
 **								   functions have been added to R2.c																   **
 **       02/16/2010  AT		   create_PCB, free_PCB, allocate_PCB functions have been added to R2.c								   **
 **       02/18/2010  RW,JC, AT    all functions are complete; documentation added													   **
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
 **        Procedure Name -- cleanup_r2																							       **
 **               Purpose -- The cleanup_r2 function releases all allocated PCB.													   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int																									   **
 **     Procedures Called -- free_PCB																								   **
 **  Global Data Accessed -- PCB *tail1 - a PCB, PCB *tail2 - a PCB																       **
 **  Summary of Algorithm -- The cleanup function loops through all of the PCBs and releases all allocated PCBs.					   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int cleanup_r2() {
    struct PCB *temppcb = tail1;
    while (temppcb != NULL) {
      tail1 = temppcb;
      temppcb = tail1->next;
      free_PCB(tail1);
    }
    temppcb = tail2;
    while (temppcb != NULL) {
      tail2 = temppcb;
      temppcb = tail2->next;
      free_PCB(tail2);
    }
    return 0;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- block																									   **
 **               Purpose -- The block function moves the specified PCB from the READY to BLOCKED queue.							   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- memset, printf, sys_req, trim, toLowerCase, findPCB qRemove, insert									   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The block function prompts the user for the name of the process to be block, processes the input, and	   **
 **						     moves the process from the READY to the BLOCKED queue.													   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int block() {  //temp command
	char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter the name of the process to be blocked: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trim(buff);
	toLowerCase(buff);
	temppcb = findPCB(buff, temppcb);
	if (errx < OK) return errx;
	if(temppcb->state != BLOCKED) {
	  temppcb = qRemove(buff, temppcb);
	  temppcb->state = BLOCKED;
	  insert(temppcb, BLOCKED);
	  if(temppcb!=NULL) printf("\nPCB successfully blocked!");
	}
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- unblock																								   **
 **               Purpose -- The unblock function places a specified process in the READY state, while retaining its SUSPENDED		   **
 **							 status.  The process is removed from the BLOCKED queue, if necessary, and inserted in the READY queue.    **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- memset, printf, sys_req, trim, toLowerCase, findPCB, qRemove, insert									   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The unblock function prompts the user for the name of the process to be unblocked, processes the input,   ** 
 **							 and moves the process from the BLOCKED to the RUNNING queue.											   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int unblock() {
    char buff[BIGBUFF];
    int buffsize = BIGBUFF;
    struct PCB *temppcb = NULL;
    memset(buff, '\0', BIGBUFF);
    
    errx = 0;
    printf("Please enter the name of the process to be unblocked: ");
    errx = sys_req(READ, TERMINAL, buff, &buffsize);
    if (errx < OK) return errx;
    trim(buff);
    toLowerCase(buff);
    temppcb = findPCB(buff, temppcb);
    if(temppcb->state == BLOCKED) {
	  temppcb = qRemove(buff, temppcb);
      temppcb->state = READY;
	  insert(temppcb, READY+1);
	  if(temppcb!=NULL) printf("\nPCB successfully unblocked!");
    }
    return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- suspend																								   **
 **               Purpose -- The suspend function places a specified process in a SUSPENDED state. The state chosen will			   **
 **							 be either SUSPENDED-READY or SUSPENDED-BLOCKED, depending on its previous state.						   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- memset, printf, sys_req, trim, toLowerCase, findPCB, qRemove, insert									   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The unblock function prompts the user for the name of the process to be suspended, processes the input,   ** 
 **							 and moves the process from	the SUSPENDED-READY or SUSPENDED-BLOCKED state, depending on its previous      **
 **							 state.																									   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int suspend() {
    char buff[BIGBUFF];
    int buffsize = BIGBUFF;
    struct PCB *temppcb = NULL;
    memset(buff, '\0', BIGBUFF);
    
    errx = 0;
    printf("Please enter the name of the process to be suspended: ");
    errx = sys_req(READ, TERMINAL, buff, &buffsize);
    if (errx < OK) return errx;
    trim(buff);
    toLowerCase(buff);
    temppcb = findPCB(buff, temppcb);
    if (errx < OK) return errx;
    if(temppcb->proc_class == SYSTEM) return ERR_CNESP;
    if(temppcb->state != SUSP) temppcb->suspended = SUSP;
    if(temppcb!=NULL) printf("\nPCB successfully suspended!");
    return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- resume																									   **
 **               Purpose -- The resume function places a specified process in a non-SUSPENDED state. The state chosen will			   **
 **							 be either READY or BLOCKED, depending on its previous state.											   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- memset, printf, sys_req, trim, toLowerCase, findPCB													   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The unblock function prompts the user for the name of the process to be resumed, processes the input,     ** 
 **							 and moves the process from	the READY or BLOCKED state, depending on its previous state.				   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int resume() {
	char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter the name of the process to be resumed: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trim(buff);
	toLowerCase(buff);
	temppcb = findPCB(buff, temppcb);
	if (errx < OK) return errx;
	if(temppcb->proc_class == SYSTEM) return ERR_CNESP;
	temppcb->suspended = NOTSUSP;
	if(temppcb!=NULL) printf("\nPCB successfully resumed!");
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- set_Priority																							   **
 **               Purpose -- The set_Priority function changes the priority of a specified process.									   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- memset, printf, sys_req, trim, toLowerCase, findPCB, atoi, qRemove, insert 							   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The set_Priority function prompts the user for the name of the process to be reprioritized, its new	   **
 **							 priority level, removes the process from either the READY or BLOCKED queue, reprioritizes the process,	   **
 **							 and inserts it back in the READY or BLOCKED queue.														   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int set_Priority() {
	char buff[BIGBUFF];
	int buffsize = BIGBUFF, temp;
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter the name of the process to be reprioritized: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trim(buff);
	toLowerCase(buff);
	temppcb = findPCB(buff, temppcb);
	if (errx < OK) return errx;
	if(temppcb->proc_class == SYSTEM) return ERR_CNESP;
	printf("Please enter the new priority level where 127 is the highest(-128 to 127): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	temp = atoi(buff);
	temppcb = qRemove(temppcb->name, temppcb);	
	if(temp<=127 && temp>=-128) temppcb->priority = temp;
	else {
      temppcb->priority = 0;
      printf("\nInvalid priority level.  Priority defaulted to 0.");
    }
	if (temppcb->state == BLOCKED) insert(temppcb, BLOCKED);
	else insert(temppcb, READY+1);
	printf("\nPriority for %s successfully set to %d",temppcb->name,temppcb->priority);
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- show_PCB																								   **
 **               Purpose -- The show_PCB function displays all information contained in a single PCB for a process specified by	   **
 **							 name.																									   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- memset, printf, sys_req, trim, toLowerCase, findPCB													   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The show_PCB function displays all information contained in a single PCB for a process specified by name      **
 **							 in a table.																							   **				
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int show_PCB() {
	char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter a process name: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trim(buff);
	toLowerCase(buff);
	temppcb = findPCB(buff,temppcb);
	if (errx < OK) return errx;
	printf("\nPROCESS PROPERTIES\n------------------------");
	printf("\nName: %s", temppcb->name);
	if(temppcb->proc_class == SYSTEM) printf("\nClass: System");
	else printf("\nClass: Application");
	printf("\nPriority: %d", temppcb->priority);
	if(temppcb->state == READY) printf("\nState: Ready");
	else if(temppcb->state == RUNNING) printf("\nState: Running"); 
	else printf("\nState: Blocked");
	if(temppcb->suspended == SUSP) printf("\nSuspended?: Yes");
	else printf("\nSuspended?: No");
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- show_All																								   **
 **               Purpose -- The show_All function displays all information about all PCBs which are currently in use.				   **
 **							 name.																									   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- printf, sys_req																						   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The show function displays all information about all PCBs which are currently in use.					   **				
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int show_All() {
    struct PCB *temppcb = NULL;
    int bufsize = BIGBUFF;
    int i = 2, x = 0;
    char buffer[BIGBUFF] = {0};
	temppcb = head1;	
	errx = 0;
	printf("\nPROCESS PROPERTIES------------------------");
	if(cop != NULL) printf("\nCOP: %-8s\n",cop->name);
	else printf("\nNo COP\n");
	for (x;x<=1;x++) {
	  while(temppcb != NULL) {
	printf("\nName: %-8s", temppcb->name);
	printf("    Priority: %-3d",temppcb->priority);
	    if(temppcb->state == READY) printf("    State: %-7s","Ready");
	    else if(temppcb->state == RUNNING) printf("    State: %-7s","Running");
		else printf("    State: %-7s","Blocked");
		if(temppcb->suspended == SUSP) printf("    Suspended?: Yes\n");
		else printf("    Suspended?: No\n");
		temppcb = temppcb->prev;
		i=i++;
		if(i > 23) {        //paging
	      printf("Press any key to continue");
	      errx = sys_req(READ, TERMINAL, buffer, &bufsize);
	      i = 0;
	    }
      }
      temppcb = tail2;
	}
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- show_Ready																								   **
 **               Purpose -- The show_Ready function displays information about all processes which are currently in the READY or the	   **
 **							  SUSPENDED-READY state.																				   **
 **							 name.																									   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- printf, sys_req																						   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The show_Ready function displays information about all processes which are currently in the READY or the	   **
 **							 SUSPENDED-READY state. 																				   **				
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int show_Ready() {
	struct PCB *temppcb = NULL;
    int bufsize = BIGBUFF;
    int i = 2;
    char buffer[BIGBUFF] = {0};
	temppcb = tail1;
	
	errx = 0;
	printf("\nPROCESS PROPERTIES------------------------");
	  while(temppcb != NULL) {
        printf("\n\nName: %s", temppcb->name);
	    printf("     Priority: %d",temppcb->priority);
		if(temppcb->suspended == SUSP) printf("     Suspended?: Yes");
		else printf("     Suspended?: No\n");
		temppcb = temppcb->next;
		i=i+4;
		if(i > 20) {        //paging
	      printf("Press any key to continue");
	      errx = sys_req(READ, TERMINAL, buffer, &bufsize);
	      i = 0;
	    }
      }
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- delete_PCB																								   **
 **               Purpose -- The delete_PCB function deallocates an existing PCB.													   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- memset, printf, sys_req																				   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The delete_PCB function removes a PCB from the queue and deallocates it.								   **				
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int delete_PCB() { //temp function	
	char buff[BIGBUFF];
	struct PCB *tmp = NULL;
	int buffsize = BIGBUFF;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter the name of the Process to delete: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);	
	if(errx >= OK) {
	//	if(tmp->proc_class != SYSTEM){
		trim(buff);
		tmp = qRemove(buff,tmp);
		free_PCB(tmp);
		printf("PCB successfully deleted!");
	//	}
		//else
		//	return ERR_UTDSC;
	}
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- show_Blocked																							   **
 **               Purpose -- The show_Blocked function displays information about all processes which are currently in the READY or	   **
 **						     the BLOCKED state.																						   **
 **							 name.																									   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- printf, sys_req																						   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The show_Blocked function displays information about all processes which are currently in the READY or	   **
 **						     the BLOCKED state.																						   **				
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int show_Blocked() {
	struct PCB *temppcb = NULL;
    int bufsize = BIGBUFF;
    int i = 2;
    char buffer[BIGBUFF] = {0};
	temppcb = tail2;
	
	errx = 0;
	printf("\nPROCESS PROPERTIES------------------------");
	  while(temppcb != NULL) {
        printf("\n\nName: %s", temppcb->name);
		if(temppcb->suspended == SUSP) printf("     Suspended?: Yes");
		else printf("     Suspended?: No\n");
		temppcb = temppcb->next;
		i=i+4;
		if(i > 20) {        //paging
	      printf("Press any key to continue");
	      errx = sys_req(READ, TERMINAL, buffer, &bufsize);
	      i = 0;
	    }
      }
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- create_PCB																								   **
 **               Purpose -- The create_PCB function allocates and setups a new PCB.  By default the process is initially in the READY **
 **							 (not SUSPENDED) state.																					   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- printf, sys_req, findPCB, strncpy, trim, atoi, allocate_PCB, insert									   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The create_PCB function prompts the user for the process name, the process class, and the process		   **
 **							 priority.  create_PCB processes the user input, allocates a new PCB and inserts it into the RUNNING	   **
 **							 queue.																									   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */ 
int create_PCB() { //temp fcn
    char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	char name[PROCESS_NAME_LENGTH];
	int proc_class, priority;
	struct PCB *temppcb = NULL;
	struct PCB *newPCBptr = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
    printf("Please enter the name of the process to be created (9 character limit): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trim(buff);
	toLowerCase(buff);
    if (strlen(buff)>9) return ERR_PRONTL;
    temppcb = findPCB(buff,temppcb);
	if (temppcb != NULL) return ERR_NAMEAE;
    strncpy(name,buff,PROCESS_NAME_LENGTH);
    
    printf("Please enter the class of the process to be created\n('0' = Application, '1' = System): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trim(buff);
    if (strncmp(buff,"0\0",2) && strncmp(buff,"1\0",2)) return ERR_INVCLS;
    proc_class = atoi(buff);
    
    printf("Please enter the priority of the process to be created where 127 is high\n(-128 to 127): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trim(buff);
    priority = atoi(buff);
	newPCBptr = allocate_PCB();
	if (newPCBptr == NULL) errx = ERR_UCPCB;
    else {
	  errx = setup_PCB(newPCBptr, name, proc_class, priority);
	  if (errx < OK) return errx;
	  errx = insert(newPCBptr,READY+1);
	  printf("\nPCB successfully created!");
	}
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- allocate_PCB																							   **
 **               Purpose -- The allocate_PCB function allocates an available PCB, marks it as in use, and returns a PCB			   **
 **							 pointer or identifier.																					   **		
 **            Parameters -- N/A																									   **
 **			 Return Value -- PCB - a new PCB pointer																				   **
 **     Procedures Called -- sys_alloc_mem, sizeof, malloc																			   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The allocate_PCB function allocates an available PCB, marks it as in use, and returns a			       **
 **							 PCB pointer or identifier.																				   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */ 
struct PCB * allocate_PCB() {
	struct PCB *newPCBptr = NULL;
	newPCBptr = sys_alloc_mem((sizeof(struct PCB)));
	newPCBptr->stack_base = (unsigned char *)sys_alloc_mem(STACK_SIZE * sizeof(unsigned char));
	newPCBptr->stack_top = newPCBptr->stack_base + STACK_SIZE-sizeof(struct context);
	//newPCBptr = malloc(sizeof(struct PCB));
	return newPCBptr;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- free_PCB																								   **
 **               Purpose -- The free_PCB function releases an allocated PCB.														   **
 **            Parameters -- struct PCB *PCBptr																						   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- sys_free_mem																							   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The free_PCB function releases an allocated PCB.														   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */ 
int free_PCB(struct PCB *PCBptr) {
    errx = 0;
	errx=sys_free_mem(PCBptr -> stack_base);
	errx=sys_free_mem(PCBptr -> load_address);
	errx=sys_free_mem(PCBptr -> execution_address);
	errx=sys_free_mem(PCBptr);
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- setup_PCB																								   **
 **               Purpose -- The setup_PCB function initializes the content of a PCB, which is assumed to be newly allocated		   **
 **							 using allocate_PCB.																					   **
 **            Parameters -- struct PCB *, char [], int, int																		   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- strncpy																								   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The setup_PCB function initializes the content of a PCB.												   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int setup_PCB(struct PCB *PCBptr, char name[PROCESS_NAME_LENGTH], int proc_class, int priority) {
    errx = 0;
	strncpy((PCBptr->name), name,PROCESS_NAME_LENGTH);
	(PCBptr->proc_class) = proc_class;
	(PCBptr->priority) = priority;
	(PCBptr->state) = READY;
	PCBptr->suspended = NOTSUSP;
	(PCBptr->prev) = NULL;
	(PCBptr->next) = NULL;
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- isEmpty																								   **
 **               Purpose -- The isEmpty function checks to see if the queue is empty.												   **
 **            Parameters -- int - represents the queue																				   **
 **			 Return Value -- int - boolean																							   **
 **     Procedures Called -- strncpy																								   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The isEmpty function checks to see if the queue is empty.  It returns '1' if it is empty.				   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int isEmpty(int q) {
    int ret = 0;
    if(q == 1) {if(head1 == NULL && tail1 == NULL) ret = 1;}
    else if(head2 == NULL && tail2 == NULL) ret = 1;
    return ret;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- insert																									   **
 **               Purpose -- The insert function inserts a PCB into a specified queue. The queue is specified by an identifier which   **
 **							 could for example, be the address of its descriptor.													   **
 **            Parameters -- struct PCB *, int																						   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- isEmpty																								   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The insert function checks to see if the queue is empty and inserts a PCB into a specified queue.		   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int insert(struct PCB *newPCB,int q) {
    struct PCB *tmp = NULL;
    errx = 0;
    if(q == READY+1) {  //For Ready Queue
      if(isEmpty(q)) {
        tail1 = newPCB;
        head1 = tail1;
      }
      else {
  	    tmp = tail1;
	    while((newPCB->priority) > (tmp->priority)) {
          if (tmp == head1) {
		    (tmp->next) = newPCB;
		    (newPCB->prev) = tmp;
		    head1 = newPCB;
	      }
		  else tmp = (tmp->next);
        }
        if(tmp==tail1){ //if inserted at tail
          tail1->prev = newPCB;
          newPCB->next = tail1;
          tail1 = newPCB;
        }
        else if(head1 != newPCB) {
          ((tmp->prev)->next) = newPCB;
          (newPCB->prev) = (tmp->prev);
		  (tmp->prev) = newPCB;
		  (newPCB->next)= tmp;
        }
      }
    }
    else {  //For Blocked Queue
      if(isEmpty(q)) {
        tail2 = newPCB;
        head2 = tail2;
      }
      else {
        tmp = tail2;
        while((newPCB->priority) > (tmp->priority)) {
          if (tmp == head2) {
		    (tmp->next) = newPCB;
		    (newPCB->prev) = tmp;
		    head2 = newPCB;
		  }
		  else tmp = (tmp->next);
        }
        if(tmp==tail2){ //if inserted at tail
          tail2->prev = newPCB;
          newPCB->next = tail2;
          tail2 = newPCB;
        }
        else if(head2 != newPCB) {
	      ((tmp->prev)->next) = newPCB;
		  (newPCB->prev) = (tmp->prev);
		  (tmp->prev) = newPCB;
		  (newPCB->next)= tmp;
        }
      }
    }
    return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- findPCB																								   **
 **               Purpose -- The findPCB function searches the PCBs for a process having a specified name.							   **
 **            Parameters -- char *, struct PCB *																					   **
 **			 Return Value -- PCB - a PCB																							   **
 **     Procedures Called -- strncmp																								   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The findPCB function searches the PCBs for a process having a specified name.							   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
struct PCB* findPCB(char *name,struct PCB *PCBptr) {
    struct PCB *tmp = tail1;
    errx = 0;
    while((tmp != NULL) && strncmp((tmp->name),name,PROCESS_NAME_LENGTH)) tmp = (tmp->next);
    PCBptr = tmp;
    if (PCBptr == NULL) { //if not found yet, search queue2
      tmp = tail2;
      while((tmp != NULL) && strncmp((tmp->name),name,PROCESS_NAME_LENGTH)) tmp = (tmp->next);
      if(tmp == NULL) errx = ERR_PCBNF; //PCB not found
      else if(tmp != NULL) PCBptr = tmp;
    }
    return PCBptr;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- qRemove																								   **
 **               Purpose -- The qRemove function searches the PCBs for a process having a specified name and removes it from the	   **
 **							 queue.																									   **
 **            Parameters -- char *, struct PCB *																					   **
 **			 Return Value -- PCB * - a PCB																							   **
 **     Procedures Called -- findPCB																								   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The qRemove function searches the PCBs for a process having a specified name and removes it from the	   **
 **							 queue.																									   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
struct PCB* qRemove(char *name,struct PCB *set) {
	struct PCB *del = NULL;
    del = findPCB(name,del);
    if(del->next == NULL && del->prev == NULL) { //if last one left
      if(del==tail1) tail1 = head1 = NULL;
      else tail2 = head2 = NULL;
    }
    else if(del->next == NULL && del->prev != NULL) { //if head
      if(del == head1) {
        head1 = del->prev;
        head1->next = NULL;
        del->prev = NULL;
      }
      else {
        head2 = del->prev;
        head2->next = NULL;
        del->prev = NULL;
      }
    }
    else if(del->next != NULL && del->prev == NULL) { //if tail
      if(del == tail1) {
        tail1 = del->next;
        tail1->prev = NULL;
        del->next = NULL;
      }
      else {
        tail2 = del->next;
        tail2->prev = NULL;
        del->next = NULL;
      }
    }
	else if(del != NULL) {
      ((del->prev)->next) = (del->next);
      ((del->next)->prev) = (del->prev);
      (del->next) = NULL;
	  (del->prev) = NULL;
	  //set = del;
    }
	return del;
}

//Returns pointer to the head of the READY queue
struct PCB* getRHead() {
       return head1;
}

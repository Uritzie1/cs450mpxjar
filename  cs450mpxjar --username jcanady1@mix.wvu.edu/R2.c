/**
* File Name: 
* \title JAROS
* \author Jonroy Canady
* \author Adam Trainer
* \author Rob Wayland   
* \date 2/12/2010
* \version: 1.0
*
* Components: 
*
*******************************************************************************
* Change Log:
*
*        2/12/2010  JC           File created with placeholders
*        2/15/2010  JC           block, unblock, suspend, resume, set_Priority, 
*                                show_PCB, show_All, show_Ready, show_Blocked functions all added
*		 2/16/2010  AT			 create_PCB, free_PCB, allocate_PCB
*        2/18/2010  AT,JC        Completed all functions and added all comments
*/


// Included ANSI C Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Included Support Files
#include "mpx_supt.h"

// Status and Error Codes
#define ERR_INVPRI (-205)
#define ERR_INVPRI (-205)    //Invalid Priority
#define ERR_PCBNF  (-206)    //PCB Not Found
#define ERR_QUEEMP (-207)    //Queue is Empty
#define ERR_UCPCB  (-208)    //Unable to Create PCB

// Constants
#define PROCESS_NAME_LENGTH 10
#define SYSTEM 1
#define APP 0
#define READY 0
#define RUNNING 1
#define BLOCKED 2
#define NOTSUSP 0
#define SUSP 1
#define STACK_SIZE 1024;


// Global Variables
PCB *tail1, *tail2, *head1, *head2;


// Structures
/** \struct PCB
* The PCB represents a process control block, containing all information about a process and pointers to the next/prev PCBs in a queue.
*/
typedef struct PCB {
	char name[PROCESS_NAME_LENGTH];         /**<Process Name*/
	int id;                                 /**<Process ID#*/
	int proc_class;						    /**<Process Class*/
	int priority;					        /**<Priority Value (-128 to 127)*/
	int state;						        /**<Process State Flag (Running, Ready, Blocked)*/
	int suspended;					        /**<Process Suspended Flag*/
	unsigned char stack[STACK_SIZE];        /**<PCB Stack*/
	unsigned char* stack_base;				/**<Pointer to base of stack*/
	unsigned char* stack_top;				/**<Pointer to top of stack*/
	int mem_size;							/**<Memory size*/
	unsigned char* load_address;			/**<Pointer to loading address*/
	unsigned char* execution_address;		/**<Pointer to execution address*/
	struct PCB *prev;				        /**<Pointer to previous PCB node*/
	struct PCB *next;				       	/**<Pointer to next PCB node*/
} PCB;

// Function Prototypes
int init_r2();
int cleanup_r2();
int block();
int unblock();
int suspend();
int resume();
int set_Priority();
int show_PCB();
int show_All();
int show_Ready();
int show_Blocked();
struct PCB * allocate_PCB();
struct PCB * setup_PCB(char [], int, int);
int free_PCB(PCB*);
int create_PCB(char [], int class, int priority);
int delete_PCB(char name[]);

/** Procedure Name: init_r2
* \param none
* \return err an integer error code (0 for now)
* Procedures Called: _getdcwd
* Globals Used: 
* @var wd
* \brief Description/Purpose: finds the working directory and writes it to the global
*/
int init_r2() {
    return 0;
}

/** Procedure Name: init_r2
* \param none
* \return  an integer error code (0 for now)
* Procedures Called: none
* Globals Used: none
* \brief Description/Purpose: none for now
*/
int cleanup_r2() {
    return 0;
}

/**
*/
int block() {  //temp command
	char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	PCB* temppcb;
	memset(buff, '\0', BIGBUFF);

	printf("Please enter the name of the process to be blocked: ");
	err = sys_req(READ, TERMINAL, buff, &buffsize);
	if (err < OK) return err;
	trim(buff);
	toLowerCase(buff);
	err = findPCB(buff, temppcb);
	if (err < OK) return err;
	if(temppcb->state != BLOCKED) {
	removePCB(temppcb);
	temppcb->state = BLOCKED;
	insertPCB(temppcb);
	}
	return err;
}

/**
*/
int unblock() {
    char buff[BIGBUFF];
    int buffsize = BIGBUFF;
    PCB* temppcb;
    memset(buff, '\0', BIGBUFF);
    
    printf("Please enter the name of the process to be unblocked: ");
    err = sys_req(READ, TERMINAL, buff, &buffsize);
    if (err < OK) return err;
    trim(buff);
    toLowerCase(buff);
    err = findPCB(buff, temppcb);
    if (err < OK) return err;
    if(temppcb->state == BLOCKED) {
        removePCB(temppcb);
        temppcb->state = READY;
        insertPCB(temppcb);
    }
    return err;
}

/**
*/
int suspend() {
    char buff[BIGBUFF];
    int buffsize = BIGBUFF;
    PCB* temppcb;
    memset(buff, '\0', BIGBUFF);
    
    printf("Please enter the name of the process to be suspended: ");
    err = sys_req(READ, TERMINAL, buff, &buffsize);
    if (err < OK) return err;
    trim(buff);
    toLowerCase(buff);
    err = findPCB(buff, temppcb);
    if (err < OK) return err;
    if(temppcb->state != SUSP) temppcb->suspended = SUSP;
    return err;
}

/**
*/
int resume() {
	char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	PCB* temppcb;
	memset(buff, '\0', BIGBUFF);

	printf("Please enter the name of the process to be resumed: ");
	err = sys_req(READ, TERMINAL, buff, &buffsize);
	if (err < OK) return err;
	trim(buff);
	toLowerCase(buff);
	err = findPCB(buff, temppcb);
	if (err < OK) return err;
	if(temppcb->state == BLOCKED) temppcb->suspended = NOTSUSP;
	return err;
}

/**
*/
int set_Priority() {
	char buff[BIGBUFF];
	int buffsize = BIGBUFF, temp;
	PCB* temppcb;
	memset(buff, '\0', BIGBUFF);

	printf("Please enter the name of the process to be reprioritized: ");
	err = sys_req(READ, TERMINAL, buff, &buffsize);
	if (err < OK) return err;
	trim(buff);
	toLowerCase(buff);
	err = findPCB(buff, temppcb);
	if (err < OK) return err;
	printf("Please enter the new priority level (-128 to 127): ");
	err = sys_req(READ, TERMINAL, buff, &buffsize);
	if (err < OK) return err;
	temp = atoi(buff);
	if (temp==0) err = ERR_INVPRI;  //validate input ???***
	else if (temp >= -128 && temp <= 127) {
		removePCB(temppcb);
		temppcb->priority = temp;
		insertPCB(temppcb);
		printf("Priority for %s successfully set to %d",temppcb->name,temppcb->priority);
	}
	else err = ERR_INVPRI;
	return err;
}

/**
*/
int show_PCB() {
	char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	PCB* temppcb;
	memset(buff, '\0', BIGBUFF);

	printf("Please enter a process name: ");
	err = sys_req(READ, TERMINAL, buff, &buffsize);
	if (err < OK) return err;
	trim(buff);
	toLowerCase(buff);
	err = findPCB(buff, temppcb);
	if (err < OK) return err;
	printf("\nPROCESS PROPERTIES\n------------------------");
	printf("\nName: %s", temppcb->name);
	if(temppcb->proc_class == SYSTEM) printf("\nClass: System");
	else printf("\nClass: Application");
	printf("\nPriority: %d", temppcb->priority;
	if(temppcb->state == READY) printf("\nState: Ready");
	else if(temppcb->state == RUNNING) printf("\nState: Running"); 
	else printf("\nState: Blocked");
	if(temppcb->suspended == SUSP) printf("\nSuspended?: Yes");
	else printf("\nSuspended?: No");
	return err;
}

/**
*/
int show_All() {
	PCB* temppcb;
    int *bufsize = BIGBUFF;
    int i = 2;
    char buffer[BIGBUFF] = {0};
   
	//set temppcb to queue root
	//hit all queues
	printf("\nPROCESS PROPERTIES------------------------");
	while(temppcb->next != NULL) {
		printf("\n\nName: %s", temppcb->name);
		if(temppcb->state == READY) printf("\nState: Ready");
		else if(temppcb->state == RUNNING) printf("\nState: Running"); 
		else printf("\nState: Blocked");
		if(temppcb->suspended == SUSP) printf("\nSuspended?: Yes");
		else printf("\nSuspended?: No\n");
		temppcb = temppcb->next;
		i=i+4;
		if(i > 20) {        //paging
	      printf("Press any key to continue");
	      err = sys_req(READ, TERMINAL, buffer, &bufsize);
	      i = 0;
	    }
	}
	return err;
}

/**
*/
int show_Ready() {
	PCB* temppcb;
    int *bufsize = BIGBUFF;
    int i = 2;
    char buffer[BIGBUFF] = {0};
     
	//set temppcb to queue root
	printf("\nPROCESS PROPERTIES\n------------------------");
	while(temppcb->next != NULL) {
	  if(temppcb->state == READY) {
	    printf("\n\nName: %s", temppcb->name);
	    printf("\nPriority: %d", temppcb->priority;
	    if(temppcb->suspended == SUSP) printf("\nSuspended?: Yes");
	    else printf("\nSuspended?: No\n");
	  }
      temppcb = temppcb->next;
      i=i+4;
	  if(i > 20) {        //paging
        printf("Press any key to continue");
        err = sys_req(READ, TERMINAL, buffer, &bufsize);
        i = 0;
      }
	}
	return err;
}

/**
*/
int delete_PCB() { //temp function	
	char buff[BIGBUFF];
	PCB tmp;
	int buffsize = BIGBUFF;
	memset(buff, '\0', BIGBUFF);

	printf("Please enter the name of the PCB to delete: ");
	err = sys_req(READ, TERMINAL, buff, &buffsize);	
	if(err < OK) {
		trim(buff);
		remove(buff,tmp);
		free_PCB(tmp);
	}
	return err;
}

/**
*/
int show_Blocked() {
	PCB* temppcb;
    int *bufsize = BIGBUFF;
    int i = 2;
    char buffer[BIGBUFF] = {0};
     
	//set temppcb to queue root
	printf("\nPROCESS PROPERTIES\n------------------------");
	while(temppcb->next != NULL) {
	  if(temppcb->state == BLOCKED) {
	    printf("\n\nName: %s", temppcb->name);
	    if(temppcb->suspended == SUSP) printf("\nSuspended?: Yes");
	    else printf("\nSuspended?: No\n");
	  }
	  temppcb = temppcb->next;
	  i=i+3;
	  if(i > 21) {        //paging
        printf("Press any key to continue");
        err = sys_req(READ, TERMINAL, buffer, &bufsize);
        i = 0;
      }
	}	
	return err;
}

/**
*/	   
int create_PCB(char process[], int class, int priority) { //temp fcn
	PCB *newPCBptr = allocate_PCB();
	if (newPCBptr == NULL) err = ERR_UCPCB;
    else {
	  err = setup_PCB(newPCBptr, process, class, priority);
	  insert(newPCBptr,1);
	}
	return err;
}

/**
*/
PCB * allocate_PCB () {
	PCB *newPCBptr;
	newPCBptr = (PCB*)sys_alloc_mem((sizeof(PCB)));
	return newPCBptr;
}

/**
*/
int free_PCB (PCB *PCBptr) {
	err = sys_free_mem(PCBptr);
	return err;
}

/**
*/
int setup_PCB (PCB *PCBptr, char name[PROCESS_NAME_LENGTH], int proc_class, int priority) {
	(PCBptr->name) = name;
	(PCBptr->proc_class) = proc_class;
	(PCBptr->priority) = priority;
	(PCBptr->state) = READY;
	(PCBptr->suspended) = NOTSUSP;
	(PCBptr->stack_base) = (PCBptr->stack)[STACK_SIZE] ;
	(PCBptr->stack_top) = (PCBptr->stack)[STACK_SIZE + 1];
	//(PCBptr->mem_size) = ;
	//(PCBptr->load_address) = ;
	//(PCBptr->execution_address) = ;
	(PCBptr->prev) = NULL;
	(PCBptr->next) = NULL;
	return err;
}

/**
*/
int isEmpty(int q) {
    int ret = 0;
    if(q == 1) if(head1 == null && tail1 == null) ret = 1;
    else if(head2 == null && tail2 == null) ret = 1;
    return ret;
}

/**
*/
int insert(PCB *newPCB,int q) {
    PCB *tmp;
    if(q == 1) {
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
        if(head1 != newPCB) {
          ((tmp->previous)->next) = newPCB;
          (newPCB->prev) = (tmp->prev);
		  (tmp->prev) = newPCB;
		  (newPCB->next)= tmp;
        }
      }
    }
    else {
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
        if(head2 != newPCB) {
          ((tmp->previous)->next) = newPCB;
		  (newPCB->prev) = (tmp->prev);
		  (tmp->prev) = newPCB;
		  (newPCB->next)= tmp;
        }
      }
    }
    return err;
}

/**
*/
int findPCB(char *name, PCB *PCBptr) {
    PCB *tmp = tail1;
    while((tmp != null) && strcmp((tmp->name),name)) tmp = (tmp->next);
    PCBptr = tmp;
    if (PCBptr == null) { //if not found yet, search queue2
      tmp = tail2;
      while((tmp != null) && strcmp((tmp->name),name)) tmp = (tmp->next);
      if(tmp == null) err = 2; //PCB not found
      else if(tmp != null) PCBptr = tmp;
    }
    return err;
}

/**
*/
int qRemove(char *name,int q,PCB set) {
	PCB del;
    err = findPCB(name,del);
    if(q == 1) {
      if(isEmpty(q)) err = 2; //queue is empty
      else { //delete from proper queue
        ((del->prev)->next) = (del->next);
        ((del->next)->prev) = (del->prev);
        (del->next) = null;
        (del->prev) = null;
      }
	}
	else {
	  if(isEmpty(q)) err = 2; //queue is empty
	  else { //delete from proper queue
	    ((del->prev)->next) = (del->next);
	    ((del->next)->prev) = (del->prev);
	    (del->next) = null;
	    (del->prev) = null;
	  }
	else {
	  if(isEmpty(q)) err = 2; //queue is empty
	  else { //delete from proper queue
	    ((del->prev)->next) = (del->next);
	    ((del->next)->prev) = (del->prev);
	    (del->next) = null;
	    (del->prev) = null;
	  }
	}
	set = del;
	return err;
}

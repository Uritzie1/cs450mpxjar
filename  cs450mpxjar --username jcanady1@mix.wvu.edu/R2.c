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
*        2/18/2010  RW,JC        Completed all functions and added all comments
*/


// Included ANSI C Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Included Support Files
#include "mpx_supt.h"
#include "COMHAN.c"

// Status and Error Codes
#define ERR_PCBNF  (-205)    //PCB Not Found
#define ERR_QUEEMP (-206)    //Queue is Empty
#define ERR_UCPCB  (-207)    //Unable to Create PCB
#define ERR_PRONTL (-208)    //Process Name too Long
#define ERR_NAMEAE (-209)    //Process Name Already Exists

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
PCB * allocate_PCB();
int setup_PCB(PCB *PCBptr, char name[PROCESS_NAME_LENGTH], int proc_class, int priority);
int free_PCB(PCB *PCBptr);
int create_PCB();
int delete_PCB();
int isEmpty(int q);
int insert(PCB *newPCB,int q);
int findPCB(char *name, PCB *PCBptr);
int qRemove(char *name,PCB *set);

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
* \breef Description/Purpose: none for now
*/
int cleanup_r2() {
    PCB* temppcb = tail1;
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

/**  Procedure Name: block
* \param none
* \return an integer error code 
* Procedures Called: trim, toLowerCase, findPCB, qRemove, insertPCB, sys_req
* Globals Used: err
* \brief Description: moves a PCB from ready to blocked queue 
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
	  err = qRemove(buff, temppcb);
	  temppcb->state = BLOCKED;
	  insertPCB(temppcb, BLOCKED);
	}
	return err;
}

/**  Procedure Name: unblock
* \param none
* \return an integer error code 
* Procedures Called: findPCB, insertPCB, removePCB, toLowerCase, trim
* Globals Used: err
* \brief Description: moves a PCB from blocked to ready queue 
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
        err = qRemove(buff, temppcb);
        temppcb->state = READY;
        insertPCB(temppcb, RUNNING);
    }
    return err;
}

/**  Procedure Name: suspend
* \param none
* \return an integer error code
* Procedures Called: findPCB, sys_req, trim, toLowerCase
* Globals Used: err
* \brief Description: suspends a PCB
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

/**  Procedure Name: resume
* \param none
* \return an integer error code
* Procedures Called: sys_req, trim, toLowerCase, findPCB
* Globals Used: err
* \brief Description: sets PCB to not suspended 
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

/**  Procedure Name: set_Priority
* \param none
* \return an integer error code
* Procedures Called: sys_req, trim, toLowerCase, findPCB, inset, qremove
* Globals Used: err
* \brief Description: sets the priority level for a specified PCB 
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
	printf("Please enter the new priority level where 127 is the highest(-128 to 127): ");
	err = sys_req(READ, TERMINAL, buff, &buffsize);
	if (err < OK) return err;
	temp = atoi(buff);
	qRemove(buff, temppcb);
	temppcb->priority = temp;
	if (temppcb->state == BLOCKED) insertPCB(temppcb, BLOCKED);
	else insertPCB(temppcb, RUNNING);
	printf("Priority for %s successfully set to %d",temppcb->name,temppcb->priority);
	return err;
}

/**  Procedure Name: show_PCB
* \param none
* \return an integer error code
* Procedures Called: sys_req, trim, toLowerCase, findPCB
* Globals Used: err
* \brief Description: prints a specific PCB 
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

/**  Procedure Name: show_all
* \param none
* \return an integer error code
* Procedures Called: sys_req
* Globals Used: err
* \brief Description: prints all PCB
*/
int show_All() {
	PCB* temppcb;
    int *bufsize = BIGBUFF;
    int i = 2, x = 0;
    char buffer[BIGBUFF] = {0};
	temppcb = tail1;
	
	printf("\nPROCESS PROPERTIES------------------------");
	for (x;x<=1;x++) {
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
      temppcb = tail2;
	}
	return err;
}

/**  Procedure Name: show_Ready
* \param none
* \return an integer error code
* Procedures Called: sys_req
* Globals Used: err
* \brief Description: prints all PCB in ready queue
*/
int show_Ready() {
	PCB* temppcb;
    int *bufsize = BIGBUFF;
    int i = 2;
    char buffer[BIGBUFF] = {0};
    temppcb = tail1; 

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
/**  Procedure Name: delete
* \param none
* \return an integer error code
* Procedures Called: sys_req, trim, remove, free_PCB
* Globals Used: err
* \brief Description: moves a PCB from ready to blocked queue 
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

/**  Procedure Name: show_Blocked
* \param none
* \return an integer error code (o for now)
* Procedures Called: sys_req, trim, toLowerCase, findPCB
* Globals Used: err
* \brief Description: moves a PCB from ready to blocked queue 
*/
int show_Blocked() {
	PCB* temppcb;
    int *bufsize = BIGBUFF;
    int i = 2;
    char buffer[BIGBUFF] = {0};
    temppcb = tail2;
     
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

/** Procedure Name: create_PCB
* \param none
* \return an integer error code
* Procedures Called: sys_req, trim, toLowerCase, setup_PCB, allocate_PCB
* Globals Used: err
* \brief Description: moves a PCB from ready to blocked queue 
*/   
int create_PCB() { //temp fcn
    char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	char name[PROCESS_NAME_LENGTH];
	int proc_class, priority;
	PCB* temppcb;
	memset(buff, '\0', BIGBUFF);

    printf("Please enter the name of the process to be created (9 character limit): ");
	err = sys_req(READ, TERMINAL, buff, &buffsize);
	if (err < OK) return err;
    if (strlen(buff)>9) return ERR_PRONTL;
    err = findPCB(buff, temppcb);
	if (err < OK) return err;
	if (temppcb != NULL) return ERR_NAMEAE;
    name = buff;
    
    printf("Please enter the class of the process to be created ('0' = Application, '1' = System): ");
	err = sys_req(READ, TERMINAL, buff, &buffsize);
	if (err < OK) return err;
    if (strncmp(buff,"0\0",2) && strncmp(buff,"1\0",2)) return ERR_PRONTL;
    proc_class = atoi(buff);
    
    printf("Please enter the priority of the process to be created where 127 is high(-128 to 127): ");
	err = sys_req(READ, TERMINAL, buff, &buffsize);
	if (err < OK) return err;
    temp = atoi(buff);
	
	PCB *newPCBptr = allocate_PCB();
	if (newPCBptr == NULL) err = ERR_UCPCB;
    else {
	  err = setup_PCB(newPCBptr, name, proc_class, priority);
	  if (err < OK) return err;
	  err = insert(newPCBptr,RUNNING);
	}
	return err;
}

/**  Procedure Name: allocate_PCB 
* \param none
* \return ptr to the new PCB
* Procedures Called: sys_alloc_mem
* Globals Used: err
* \brief Description: allocates memory for a PCB 
*/
PCB * allocate_PCB() {
	PCB *newPCBptr;
	newPCBptr = (PCB*)sys_alloc_mem((sizeof(PCB)));
	return newPCBptr;
}

/**  Procedure Name: free_PCB
* \param *PCBptr pointer to a PCB struct
* \return an integer error code (o for now)
* Procedures Called: sys_free_mem
* Globals Used: err
* \brief Description: frees PCB from memory
*/
int free_PCB(PCB *PCBptr) {
	err = sys_free_mem(PCBptr);
	return err;
}

/**  Procedure Name: setup_PCB
* \param *PCBptr pointer to PCB
* \param name : char array with PCB name
* \param proc_class : class of the PCB
* \param priority : int that set priority
* \return an integer error code (o for now)
* Procedures Called: sys_req, trim, toLowerCase, findPCB
* Globals Used: err
* \brief Description: sets the contents of a PCB
*/
int setup_PCB(PCB *PCBptr, char name[PROCESS_NAME_LENGTH], int proc_class, int priority) {
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

/**  Procedure Name: isEmpty
* \param q :int representing the queue
* \return an integer error code (o for now)
* Procedures Called: none
* Globals Used: err
* \brief Description: checks if a queue is empty 
*/
int isEmpty(int q) {
    int ret = 0;
    if(q == 1) if(head1 == null && tail1 == null) ret = 1;
    else if(head2 == null && tail2 == null) ret = 1;
    return ret;
}

/**  Procedure Name: insert
* \param newPCB: pointer to a PCB
* \param q : int the specifies a queue
* \return an integer error code (o for now)
* Procedures Called: isEmpty
* Globals Used: err
* \brief Description: inserts a PCB into the queue 
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

/**  Procedure Name: findPCB
* \param name : string containing name
* \param PCBptr: a pointer to a PCB struct
* \return an integer error code
* Procedures Called: none
* Globals Used: err
* \brief Description: find a PCB pointer given a name
*/
int findPCB(char *name, PCB *PCBptr) {
    PCB *tmp = tail1;
    while((tmp != null) && strncmp((tmp->name),name,strlen(name)+1)) tmp = (tmp->next);
    PCBptr = tmp;
    if (PCBptr == null) { //if not found yet, search queue2
      tmp = tail2;
      while((tmp != null) && strncmp((tmp->name),name,strlen(name)+1)) tmp = (tmp->next);
      if(tmp == null) err = ERR_PCBNF; //PCB not found
      else if(tmp != null) PCBptr = tmp;
    }
    return err;
}

/**  Procedure Name: remove
* \param name: string containing PCB name
* \param set: a pointer to a PCB
* \return an integer error code (o for now)
* \return sets set to removed PCB
* Procedures Called: findPCB
* Globals Used: err
* \breif Description: removes a PCB from queue
*/
int qRemove(char *name,PCB *set) {
	PCB *del;
    err = findPCB(name,del);
	
	if(err < OK){
    ((del->prev)->next) = (del->next);
    ((del->next)->prev) = (del->prev);
    (del->next) = null;
	(del->prev) = null;
	set = del;}
    
	return err;
}

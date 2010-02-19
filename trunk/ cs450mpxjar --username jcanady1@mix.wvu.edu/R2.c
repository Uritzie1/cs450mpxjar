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

// Constants
#define PROCESS_NAME_LENGTH 10
#define SYSTEM 1
#define APP 0
#define READY 0
#define RUNNING 1
#define BLOCKED 2
#define NOTSUSP 0
#define SUSP 1
#define STACK_SIZE 1024

// Global Variables
PCB *tail1, *tail2, *head1, *head2;

// Structures
/** \struct PCB
* The PCB represents a process control block, containing all information about a process and pointers to the next/prev PCBs in a queue.
*/
typedef struct PCB 
{
	char name[PROCESS_NAME_LENGTH];         /**<Process Name*/
	int proc_class;						    /**<Process Class*/
	int priority;					        /**<Priority Value (-128 to 127)*/
	int state;						        /**<Process State Flag (Running, Ready, Blocked)*/
	int suspended;					        /**<Process Suspended Flag*/
	unsigned char * stack_base;				/**<Pointer to base of stack*/
	unsigned char * stack_top;				/**<Pointer to top of stack*/
	int mem_size;							/**<Memory size*/
	unsigned char * load_address;			/**<Pointer to loading address*/
	unsigned char * execution_address;		/**<Pointer to execution address*/
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
int free_PCB(struct PCB *);
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
int init_r2() 
{
	return 0;
}

/** Procedure Name: init_r2
* \param none
* \return  an integer error code (0 for now)
* Procedures Called: none
* Globals Used: none
* \brief Description/Purpose: none for now
*/
int cleanup_r2() 
{
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
	
	if (temppcb->state != BLOCKED) 
	{
		removePCB(temppcb);
		temppcb->state = BLOCKED;
		insertPCB(temppcb);
	}

	return err;
}

/**
*/
int unblock() 
{
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
	
	if (temppcb->state == BLOCKED) 
	{
		removePCB(temppcb);
		temppcb->state = READY;
		insertPCB(temppcb);
	}

	return err;
}

/**
*/
int suspend() 
{
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
	
	if (temppcb->state != SUSP) temppcb->suspended = SUSP;
	
	return err;
}

/**
*/
int resume() 
{
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
int set_Priority() 
{
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
	else if (temp >= -128 && temp <= 127) 
	{
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
	while(temppcb->next != NULL) 
	{
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

int isEmpty(int q)
{
	int ret = 0;
	if(q == 1)
	{
		if(head1 == null && tail1 == null) ret = 1;
	}
	else
	{
		if(head2 == null && tail2 == null) ret = 1;
	}
	return ret;
}

int insert(struct PCB *newPCB,int q)
{
	err  = 0;
	PCB tmp;
	if(q == 1){
		if(isEmpty(q))
		{
			tail1 = newPCB;
			head1 = tail1;
		}
		else
		{
			tmp = tail1;
			while((newPCB->priority) > (tmp->priority)){
				(if tmp->next == head)
				{
					(tmp->next) = newPCB;
					(newPCB->prev) = tmp;
					head = newPCB;
				}
				else tmp = (tmp->next);}

			if(head != newPCB)
			{
				((tmp->previous)->next) = newPCB;
				(newPCB->prev) = (tmp->prev);
				(tmp->prev) = newPCB;
				(newPCB->next)= tmp;
			}
		}
	}
	else{
		if(isEmpty(q))
		{
			tail2 = newPCB;
			head2 = tail2;
		}
		else
		{
			tmp = tail1;
			while((newPCB->priority) > (tmp->priority)){
				(if tmp->next == head)
				{
					(tmp->next) = newPCB;
					(newPCB->prev) = tmp;
					head = newPCB;
				}
				else tmp = (tmp->next);}

			if(head != newPCB)
			{
				((tmp->previous)->next) = newPCB;
				(newPCB->prev) = (tmp->prev);
				(tmp->prev) = newPCB;
				(newPCB->next)= tmp;
			}
		}
	}
	return err;
}
//find pcb
int findPCB(int name,PCB *PCBptr)
{
	err = 0;
	PCB tmp = tail1;

	while((tmp != null) && (tmp->name) != name)
		tmp = (tmp->next);
	PCBptr = tmp;
	tmp = tail2;

	while((tmp != null) && (tmp->name) != name)
		tmp = (tmp->next);

	if(tmp == null && PCBptr == null)
		err = 2; //PCB not found
	else if(tmp != null) PCBptr = tmp;

	return err;
}
int qDelete(int name,int q)
{
	err = 0;
	PCB del; // = findPCB(name);
	if(q == 1){
		if(isEmpty(q))
		{
			err = 2; //queue is empty
		}
		else //delete from proper queue
		{
			((del->prev)->next) = (del->next);
			((del->next)->prev) = (del->prev);
			(del->next) = null;
			(del->prev) = null;

			free_PCB(del)

		}
	}
	else{
				if(isEmpty(q))
		{
			err = 2; //queue is empty
		}
		else //delete from proper queue
		{
			((del->prev)->next) = (del->next);
			((del->next)->prev) = (del->prev);
			(del->next) = null;
			(del->prev) = null;

			free_PCB(del)

		}
	}
	return err;
}

			   struct PCB *allocate_PCB()
		  {
			  PCB *newPCPptr;
			  newPCPptr = (PCB*)sys_alloc_mem((sizeof(PCB)));
			  
			  return newPCPptr;
		  }
			   
			   void free_PCB (struct PCB *PCBptr)
		  {
			  err = sys_free_mem(PCBptr->stackBase);
			  err = sys_free_mem(PCBptr);
		  }
			   
			   struct PCB * setup_PCB(char name[]. int class, int priority)
		  {
			  PCB *PCBptr = NULL;
			  PCBptr      = allocate_PCB();
			  
			  unsigned char *stackBase;
			  stackBase = (unsigned char *)sys_alloc_mem((sizeof(STACK_SIZE)));
			  strcpy((PCBptr->name, name));
			  (PCBptr->proc_class)   = class;
			  (PCBptr->priority)     = priority;
			  (PCBptr->state)        = READY;
			  (PCBptr->stack_base)   = stackBase;
			  (PCBptr->stack_top)    = stackBase + STACK_SIZE;
			  (PCBptr->mem_size)     = STACK_SIZE;
			  (PCBptr->load_address) = NULL;
			  (PCBptr->prev)         = NULL;
			  (PCBptr->next)         = NULL;	
			  
			  return PCBptr;
		  }
			   
			   void create_PCB(char process[], int class, int priority)
		  {
			  PCB *PCBptr = NULL;
			  PCBptr      = setup_PCB(process, class, priority);
			  insert_PCB(PCBptr, <I have no idea what 'q' is);
		  }
			   
			   void delete_PCB(char name[], *PCBptr)
		  {
			  PCB *removePCBptr = NULL;
			  removePCBptr      = findPCB(name, PCBptr);
			  
			  if (removePCBptr != NULL)
			  {
				  qDelete(name, <Again, wtf is 'q');
		free(removePCBptr);
	}
}
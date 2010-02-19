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
//#include "mpx_supt.h"
//#include "R2.h"
//#include "COMHAN.c"


// Status and Error Codes
#define ERR_INVCOM (-201) // Invalid command
#define ERR_INVYR (-202)  // Invalid year
#define ERR_INVMON (-203) // Invalid month
#define ERR_INVDAY (-204) // Invalid day
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
#define ERR_PCBNF  (-205)    //PCB Not Found
#define ERR_QUEEMP (-206)    //Queue is Empty
#define ERR_UCPCB  (-207)    //Unable to Create PCB
#define ERR_PRONTL (-208)    //Process Name too Long
#define ERR_NAMEAE (-209)    //Process Name Already Exists
#define ERR_INVCLS (-210)    //Invalid Class

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
struct PCB *tail1=NULL, *tail2=NULL, *head1=NULL, *head2=NULL;
int errx = 0;

// Structures
/** \struct PCB
* The PCB represents a process control block, containing all information about a process and pointers to the next/prev PCBs in a queue.
*/
typedef struct PCB {
	char name[PROCESS_NAME_LENGTH];         //Process Name
	int id;                                 //Process ID#
	int proc_class;						    //Process Class
	int priority;					        //Priority Value (-128 to 127)
	//int state;						        //Process State Flag (Running, Ready, Blocked)
	//int suspended;					        //Process Suspended Flag
	unsigned char stack[STACK_SIZE];        //PCB Stack
	unsigned char* stack_base;				//Pointer to base of stack
	unsigned char* stack_top;				//Pointer to top of stack
	int mem_size;							//Memory size
	unsigned char* load_address;			//Pointer to loading address
	unsigned char* execution_address;		//Pointer to execution address
	struct PCB *prev;				        //Pointer to previous PCB node
	struct PCB *next;				       	//Pointer to next PCB node
	int state;
	int suspended;
} ;

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
int setup_PCB(struct PCB *PCBptr, char name[PROCESS_NAME_LENGTH], int proc_class, int priority);
int free_PCB(struct PCB *PCBptr);
int create_PCB();
int delete_PCB();
int isEmpty(int q);
int insert(struct PCB *newPCB,int q);
struct PCB* findPCB(char *name, struct PCB *PCBptr);
struct PCB* qRemove(char *name, struct PCB *set);
void toLowerCasex(char str[BIGBUFF]);
void trimx(char ary[BIGBUFF]);

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
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter the name of the process to be blocked: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trimx(buff);
	toLowerCasex(buff);
	temppcb = findPCB(buff, temppcb);
	if (errx < OK) return errx;
	if(temppcb->state != BLOCKED) {
	  temppcb = qRemove(buff, temppcb);
	  temppcb->state = BLOCKED;
	  insert(temppcb, BLOCKED);
	}
	return errx;
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
    struct PCB *temppcb = NULL;
    memset(buff, '\0', BIGBUFF);
    
    errx = 0;
    printf("Please enter the name of the process to be unblocked: ");
    errx = sys_req(READ, TERMINAL, buff, &buffsize);
    if (errx < OK) return errx;
    trimx(buff);
    toLowerCasex(buff);
    temppcb = findPCB(buff, temppcb);
    if (errx < OK) return errx;
    if(temppcb->state == BLOCKED) {
	temppcb = qRemove(buff, temppcb);
        temppcb->state = READY;
	insert(temppcb, RUNNING);
    }
    return errx;
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
    struct PCB *temppcb = NULL;
    memset(buff, '\0', BIGBUFF);
    
    errx = 0;
    printf("Please enter the name of the process to be suspended: ");
    errx = sys_req(READ, TERMINAL, buff, &buffsize);
    if (errx < OK) return errx;
    trimx(buff);
    toLowerCasex(buff);
    temppcb = findPCB(buff, temppcb);
    if (errx < OK) return errx;
    if(temppcb->state != SUSP) temppcb->suspended = SUSP;
    return errx;
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
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter the name of the process to be resumed: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trimx(buff);
	toLowerCasex(buff);
	temppcb = findPCB(buff, temppcb);
	if (errx < OK) return errx;
	if(temppcb->state == BLOCKED) temppcb->suspended = NOTSUSP;
	return errx;
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
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter the name of the process to be reprioritized: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trimx(buff);
	toLowerCasex(buff);
	temppcb = findPCB(buff, temppcb);
	if (errx < OK) return errx;
	printf("Please enter the new priority level where 127 is the highest(-128 to 127): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	temp = atoi(buff);
	qRemove(temppcb->name, temppcb);
	temppcb->priority = temp;
	if (temppcb->state == BLOCKED) insert(temppcb, BLOCKED);
	else insert(temppcb, RUNNING);
	printf("Priority for %s successfully set to %d",temppcb->name,temppcb->priority);
	return errx;
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
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter a process name: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trimx(buff);
	toLowerCasex(buff);
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

/**  Procedure Name: show_all
* \param none
* \return an integer error code
* Procedures Called: sys_req
* Globals Used: err
* \brief Description: prints all PCB
*/
int show_All() {
    struct PCB *temppcb = NULL;
    int bufsize = BIGBUFF;
    int i = 2, x = 0;
    char buffer[BIGBUFF] = {0};
	temppcb = tail1;
	
	errx = 0;
	printf("\nPROCESS PROPERTIES------------------------");
	for (x;x<=1;x++) {
	  while(temppcb != NULL) {
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
	      errx = sys_req(READ, TERMINAL, buffer, &bufsize);
	      i = 0;
	    }
      }
      temppcb = tail2;
	}
	return errx;
}

/**  Procedure Name: show_Ready
* \param none
* \return an integer error code
* Procedures Called: sys_req
* Globals Used: err
* \brief Description: prints all PCB in ready queue
*/
int show_Ready() {
	struct PCB* temppcb = NULL;
    int bufsize = BIGBUFF;
    int i = 2;
    char buffer[BIGBUFF] = {0};
    temppcb = tail1; 

    errx = 0;
	printf("\nPROCESS PROPERTIES\n------------------------");
	while(temppcb->next != NULL) {
	  if(temppcb->state == READY) {
	    printf("\n\nName: %s", temppcb->name);
	    printf("\nPriority: %d", temppcb->priority);
	    if(temppcb->suspended == SUSP) printf("\nSuspended?: Yes");
	    else printf("\nSuspended?: No\n");
	  }
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
	struct PCB *tmp = NULL;
	int buffsize = BIGBUFF;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter the name of the PCB to delete: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);	
	if(errx >= OK) {
		trimx(buff);
		tmp = qRemove(buff,tmp);
		free_PCB(tmp);
	}
	return errx;
}

/**  Procedure Name: show_Blocked
* \param none
* \return an integer error code (o for now)
* Procedures Called: sys_req, trim, toLowerCase, findPCB
* Globals Used: err
* \brief Description: moves a PCB from ready to blocked queue 
*/
int show_Blocked() {
	struct PCB* temppcb = NULL;
    int bufsize = BIGBUFF;
    int i = 2;
    char buffer[BIGBUFF] = {0};
    temppcb = tail2;
     
    errx = 0;
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
	errx = sys_req(READ, TERMINAL, buffer, &bufsize);
        i = 0;
      }
	}	
	return errx;
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
	struct PCB *temppcb = NULL;
	struct PCB *newPCBptr = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
    printf("Please enter the name of the process to be created (9 character limit): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trimx(buff);
    if (strlen(buff)>9) return ERR_PRONTL;
    findPCB(buff,temppcb);
	if (temppcb != NULL) return ERR_NAMEAE;
    strncpy(name,buff,PROCESS_NAME_LENGTH);
    
    printf("Please enter the class of the process to be created\n('0' = Application, '1' = System): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trimx(buff);
    if (strncmp(buff,"0\0",2) && strncmp(buff,"1\0",2)) return ERR_INVCLS;
    proc_class = atoi(buff);
    
    printf("Please enter the priority of the process to be created where 127 is high\n(-128 to 127): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trimx(buff);
    priority = atoi(buff);
	newPCBptr = allocate_PCB();
	if (newPCBptr == NULL) errx = ERR_UCPCB;
    else {
	  errx = setup_PCB(newPCBptr, name, proc_class, priority);
	  if (errx < OK) return errx;
	  errx = insert(newPCBptr,RUNNING);

	//printf("Press enter to delete ");
	//errx = sys_req(READ, TERMINAL, buff, &buffsize);
	//free_PCB(newPCBptr);
	  //temppcb = findPCB(newPCBptr->name, temppcb);
	  //printf("\ncreate:%s",temppcb->name);
	}
	return errx;
}

/**  Procedure Name: allocate_PCB 
* \param none
* \return ptr to the new PCB
* Procedures Called: sys_alloc_mem
* Globals Used: err
* \brief Description: allocates memory for a PCB 
*/
struct PCB * allocate_PCB() {
	struct PCB *newPCBptr = NULL;
	errx = 0;
	//newPCBptr = sys_alloc_mem((sizeof(struct PCB)));
	newPCBptr = malloc(sizeof(struct PCB));
	return newPCBptr;
}

/**  Procedure Name: free_PCB
* \param *PCBptr pointer to a PCB struct
* \return an integer error code (o for now)
* Procedures Called: sys_free_mem
* Globals Used: err
* \brief Description: frees PCB from memory
*/
int free_PCB(struct PCB *PCBptr) {
    errx = 0;
	//errx=sys_free_mem(PCBptr -> stack_base);
	//errx=sys_free_mem(PCBptr -> load_address);
	//errx=sys_free_mem(PCBptr);
	free(PCBptr);
	return errx;
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
int setup_PCB(struct PCB *PCBptr, char name[PROCESS_NAME_LENGTH], int proc_class, int priority) {
    errx = 0;
	strncpy((PCBptr->name), name,PROCESS_NAME_LENGTH);
	(PCBptr->proc_class) = proc_class;
	(PCBptr->priority) = priority;
	(PCBptr->state) = READY;
	PCBptr->suspended = NOTSUSP;
	(PCBptr->stack_base) = (PCBptr->stack)[STACK_SIZE] ;
	(PCBptr->stack_top) = (PCBptr->stack)[STACK_SIZE + 1];
	//(PCBptr->mem_size) = ;
	//(PCBptr->load_address) = ;
	//(PCBptr->execution_address) = ;
	(PCBptr->prev) = NULL;
	(PCBptr->next) = NULL;
	return errx;
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
    if(q == 1) if(head1 == NULL && tail1 == NULL) ret = 1;
    else if(head2 == NULL && tail2 == NULL) ret = 1;
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
int insert(struct PCB *newPCB,int q) {
    struct PCB *tmp = NULL;
    errx = 0;
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
	  ((tmp->prev)->next) = newPCB;
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
	  ((tmp->prev)->next) = newPCB;
		  (newPCB->prev) = (tmp->prev);
		  (tmp->prev) = newPCB;
		  (newPCB->next)= tmp;
        }
      }
    }
    //printf("\ntail1:%s",tail1->name);
    //printf("\ntail2:%s",tail2->name);
    return errx;
}

/**  Procedure Name: findPCB
* \param name : string containing name
* \param PCBptr: a pointer to a PCB struct
* \return an integer error code
* Procedures Called: none
* Globals Used: err
* \brief Description: find a PCB pointer given a name
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
    //if(PCBptr != NULL) printf("\nfind:%d",*PCBptr->name);
    return PCBptr;
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
struct PCB* qRemove(char *name,struct PCB *set) {
	struct PCB *del = NULL;
    del = findPCB(name,del);
    if(del->next == NULL && del->prev == NULL) {
      if(del==tail1) tail1 = head1 = NULL;
      else tail2 = head2 = NULL;
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

void toLowerCasex(char str[BIGBUFF]) {
  int i = 0;
  for(i;i<strlen(str);i++) str[i] = tolower(str[i]);
}

 void trimx(char ary[BIGBUFF]) {
   char temp[BIGBUFF] = {0};
   int i,j = 0;
   for(i = 0;i<BIGBUFF;i++) {
	 if(!isspace(ary[i])) {
	   if(ary[i] == 12) j++;  //trims newlines
	   else {
	     temp[j] = ary[i];
	     j++;
	   }
	 }
   }
   for(i = 0;i < BIGBUFF;i++) ary[i] = temp[i];
}

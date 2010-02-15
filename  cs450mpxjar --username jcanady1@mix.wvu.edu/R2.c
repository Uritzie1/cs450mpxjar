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
 *                                 show_PCB, show_All, show_Ready, show_Blocked functions all added
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

// Global Variables

// Structures
/** \struct PCB
  * The PCB represents a process control block, containing all information about a process and pointers to the next/prev PCBs in a queue.
  */
typedef struct PCB {
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


/** Procedure Name: main
 * \param none
 * \return an integer that is 0 if successful (which it always is)
 * Procedures Called: sys_init, init_r1, comhan, cleanup_r1, terminate_mpx
 * Globals Used: 
 * @var err
 * \details Description/Purpose: Main simply initializes the system, calls comhan, and
 *   then cleans up and terminates (though it never actually reaches cleanup or
 *   terminate)
 */
int main() {
  sys_init(MODULE_R2);
  init_r2();
  cleanup_r2();
  return 0;
}

/** Procedure Name: init_r1
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

/** Procedure Name: init_r1
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
int block() { //temp command
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
  //set temppcb to queue root
  //implement paging
  //hit all queues
  printf("\nPROCESS PROPERTIES\n------------------------");
  while(temppcb->next != NULL) {
    printf("\nName: %s", temppcb->name);
    if(temppcb->state == READY) printf("\nState: Ready");
    else if(temppcb->state == RUNNING) printf("\nState: Running"); 
    else printf("\nState: Blocked");
    if(temppcb->suspended == SUSP) printf("\nSuspended?: Yes");
    else printf("\nSuspended?: No\n");
    temppcb = temppcb->next;
  }
  return err;
}

/**
  */
int show_Ready() {
  PCB* temppcb; 
  //set temppcb to queue root
  //implement paging
  printf("\nPROCESS PROPERTIES\n------------------------");
  while(temppcb->next != NULL) {
    if(temppcb->state == READY) {
      printf("\nName: %s", temppcb->name);
      printf("\nPriority: %d", temppcb->priority;
      if(temppcb->suspended == SUSP) printf("\nSuspended?: Yes");
      else printf("\nSuspended?: No\n");
    }
    temppcb = temppcb->next;
  }
  return err;
}

/**
  */
int show_Blocked() {
  PCB* temppcb; 
  //set temppcb to queue root
  //implement paging
  printf("\nPROCESS PROPERTIES\n------------------------");
  while(temppcb->next != NULL) {
    if(temppcb->state == BLOCKED) {
      printf("\nName: %s", temppcb->name);
      if(temppcb->suspended == SUSP) printf("\nSuspended?: Yes");
      else printf("\nSuspended?: No\n");
    }
    temppcb = temppcb->next;
  }
  return err;
}
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
 **					 toLowerCasex, trimx 																					           **
 **																																	   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **																   CHANGE LOG														   **
 **																																	   **
 **		  02/12/2010  JC           template of R2.c created																			   **
 **       02/15/2010  JC           block, unblock, suspend, resume, set_Priority, show_PCB, show_All, show_Ready, show_Blocked		   **
 **								  functions have been added to R2.c																	   **
 **       02/16/2010  AT		  create_PCB, free_PCB, allocate_PCB functions have been added to R2.c								   **
 **       02/18/2010  RW,JC, AT   all functions are complete; documentation	added													   **
 **		  04/24/2010  AT		  code review; additional documentation added 													       **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */

/* Included ANSI C Files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Macros */
#define BIGBUFF 80
#define SMALLBUFF 10
#define TINYBUFF 2
#define OK 0

/* Service Operation Code */
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

// Status & Error Codes
#define ERR_INVCOM (-201)    // Invalid Command
#define ERR_INVYR  (-202)    // Invalid Year
#define ERR_INVMON (-203)    // Invalid Month
#define ERR_INVDAY (-204)    // Invalid Day
#define ERR_PCBNF  (-205)    // PCB Not Found
#define ERR_QUEEMP (-206)    // Queue Is Empty
#define ERR_UCPCB  (-207)    // Unable to Create PCB
#define ERR_PRONTL (-208)    // Process Name Too Long
#define ERR_NAMEAE (-209)    // Process Name Already Exists
#define ERR_INVCLS (-210)    // Invalid Class

/* Macros */
#define PROCESS_NAME_LENGTH 10
#define SYSTEM 1
#define APP 0
#define READY 0
#define RUNNING 1
#define BLOCKED 2
#define NOTSUSP 0
#define SUSP 1
#define STACK_SIZE 1024


/* Global Variables */
struct PCB *tail1 = NULL, *tail2 = NULL, *head1 = NULL, *head2 = NULL;
int errx          = 0;

/* Process Control Block (PCB) Data Structure */
typedef struct PCB 
{
	char name[PROCESS_NAME_LENGTH];         // Process Name
	int id;                                 // Process ID#
	int proc_class;						    // Process Class
	int priority;					        // Priority Value (-128 to 127)
	int state;						        // Process State Flag (Running, Ready, Blocked)
	int suspended;					        // Process Suspended Flag
	unsigned char stack[STACK_SIZE];        // PCB Stack
	unsigned char* stack_base;				// Pointer to Base of Stack
	unsigned char* stack_top;				// Pointer to Top of Stack
	int mem_size;							// Memory Size
	unsigned char* load_address;			// Pointer to Loading Address
	unsigned char* execution_address;		// Pointer to Execution Address
	struct PCB *prev;				        // Pointer to Previous PCB Node
	struct PCB *next;				       	// Pointer to Next PCB Node
};

/* Function Prototypes */
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
int insert(struct PCB *newPCB, int q);
struct PCB* findPCB(char *name, struct PCB *PCBptr);
struct PCB* qRemove(char *name, struct PCB *set);
void toLowerCasex(char str[BIGBUFF]);
void trimx(char ary[BIGBUFF]);
struct PCB* getRHead();

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- init_r2																							       **
 **               Purpose -- The init_r2 function does nothing.																		   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int																									   **
 **     Procedures Called -- N/A																									   **
 **  Global Data Accessed -- N/A																								       **
 **  Summary of Algorithm -- The init_r2 does nothing.																				   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int init_r2() 
{
	return 0;
}

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
int cleanup_r2() 
{
	struct PCB *temppcb = tail1;
    
	while (temppcb != NULL) 
	{
      tail1   = temppcb;
      temppcb = tail1->next;
      free_PCB(tail1);
    }
	
    temppcb = tail2;
    
	while (temppcb != NULL) 
	{
      tail2   = temppcb;
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
 **     Procedures Called -- memset, printf, sys_req, trimx, toLowerCasex, findPCB qRemove, insert									   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The block function prompts the user for the name of the process to be block, processes the input, and	   **
 **						     moves the process from the READY to the BLOCKED queue.													   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int block() 
{
	char buff[BIGBUFF];
	int buffsize        = BIGBUFF;
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);
	errx = 0;
	
	printf("Please Enter the Name of the Process to be Blocked: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	
	if (errx < OK) return errx;
	
	trimx(buff);
	toLowerCasex(buff);
	temppcb = findPCB(buff, temppcb);
	
	if (errx < OK) return errx;
	
	if (temppcb->state != BLOCKED) 
	{
	  temppcb        = qRemove(buff, temppcb);
	  temppcb->state = BLOCKED;
	  insert(temppcb, BLOCKED);
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
 **     Procedures Called -- memset, printf, sys_req, trimx, toLowerCasex, findPCB, qRemove, insert									   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The unblock function prompts the user for the name of the process to be unblocked, processes the input,   ** 
 **							 and moves the process from the BLOCKED to the RUNNING queue.											   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int unblock() 
{
    char buff[BIGBUFF];
    int buffsize        = BIGBUFF;
    struct PCB *temppcb = NULL;
    memset(buff, '\0', BIGBUFF);
    
    errx = 0;
    
	printf("Please Enter the Name of the Process to be Unblocked: ");
    errx = sys_req(READ, TERMINAL, buff, &buffsize);
    
	if (errx < OK) return errx;
    
	trimx(buff);
    toLowerCasex(buff);
    temppcb = findPCB(buff, temppcb);
    
	if (temppcb->state == BLOCKED) 
	{
	  temppcb        = qRemove(buff, temppcb);
      temppcb->state = READY;
	  insert(temppcb, RUNNING);
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
 **     Procedures Called -- memset, printf, sys_req, trimx, toLowerCasex, findPCB, qRemove, insert									   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The unblock function prompts the user for the name of the process to be suspended, processes the input,   ** 
 **							 and moves the process from	the SUSPENDED-READY or SUSPENDED-BLOCKED state, depending on its previous      **
 **							 state.																									   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int suspend() 
{
    char buff[BIGBUFF];
    int buffsize        = BIGBUFF;
    struct PCB *temppcb = NULL;
    memset(buff, '\0', BIGBUFF);
    
    errx = 0;
    printf("Please Enter the Name of the Process to be Suspended: ");
    
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
    
	if (errx < OK) return errx;
    
	trimx(buff);
    toLowerCasex(buff);
    temppcb = findPCB(buff, temppcb);
    
	if (errx < OK) return errx;
    
	if (temppcb->state != SUSP) temppcb->suspended = SUSP;
    
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- resume																									   **
 **               Purpose -- The resume function places a specified process in a non-SUSPENDED state. The state chosen will			   **
 **							 be either READY or BLOCKED, depending on its previous state.											   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- memset, printf, sys_req, trimx, toLowerCasex, findPCB													   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The unblock function prompts the user for the name of the process to be resumed, processes the input,     ** 
 **							 and moves the process from	the READY or BLOCKED state, depending on its previous state.				   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int resume()
{
	char buff[BIGBUFF];
	int buffsize        = BIGBUFF;
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	
	printf("Please Enter the Name of the Process to be Resumed: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	
	if (errx < OK) return errx;
	
	trimx(buff);
	toLowerCasex(buff);
	
	temppcb            = findPCB(buff, temppcb);
	temppcb->suspended = NOTSUSP;
	
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- set_Priority																							   **
 **               Purpose -- The set_Priority function changes the priority of a specified process.									   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- memset, printf, sys_req, trimx, toLowerCasex, findPCB, atoi, qRemove, insert 							   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The set_Priority function prompts the user for the name of the process to be reprioritized, its new	   **
 **							 priority level, removes the process from either the READY or BLOCKED queue, reprioritizes the process,	   **
 **							 and inserts it back in the READY or BLOCKED queue.														   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int set_Priority() 
{
	char buff[BIGBUFF];
	int buffsize        = BIGBUFF, temp;
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	
	printf("Please Enter the Name of the Process to be Reprioritized: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	
	if (errx < OK) return errx;
	
	trimx(buff);
	toLowerCasex(buff);
	temppcb = findPCB(buff, temppcb);
	
	if (errx < OK) return errx;
	
	printf("Please Enter the New Priority Level Where 127 is the Highest (-128 to 127): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	
	if (errx < OK) return errx;
	
	temp    = atoi(buff);
	temppcb = qRemove(temppcb->name, temppcb);	
	
	if (temp<=127 && temp>=-128) temppcb->priority = temp;
	
	else 
	{
		temppcb->priority = 0;
		printf("\nInvalid Priority Leve - Priority Defaulted to 0");
    }
	
	if (temppcb->state == BLOCKED) insert(temppcb, BLOCKED);
	
	else insert(temppcb, RUNNING);
	
	printf("\nPriority for %s Successfully Set to %d",temppcb->name, temppcb->priority);
	
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- show_PCB																								   **
 **               Purpose -- The show_PCB function displays all information contained in a single PCB for a process specified by	   **
 **							 name.																									   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- memset, printf, sys_req, trimx, toLowerCasex, findPCB													   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The show function displays all information contained in a single PCB for a process specified by name      **
 **							 in a table.																							   **				
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int show_PCB() 
{
	char buff[BIGBUFF];
	int buffsize        = BIGBUFF;
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please Enter a Process Name: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	
	if (errx < OK) return errx;
	
	trimx(buff);
	toLowerCasex(buff);
	temppcb = findPCB(buff, temppcb);
	
	if (errx < OK) return errx;
	
	printf("\n     NAME       CLASS PRIORITY   STATE SUSPENDED\n");
	printf("--------------------------------------------------");
	
	printf ("\n%9s", stemppcb->name);
	
	if (temppcb->proc_class == SYSTEM)
	{
		char tempclass[12] = "System";
		printf("%12s", tempclass);
	}
	
	else 
	{
		char tempclass[12] = "Application";
		printf("%12s", tempclass);
	}
	
	printf("%9d", temppcb->priority);
	
	if (temppcb->state == READY)
	{
		char tempstate[8] = "Ready";
		printf("%8s", tempstate);
	}
	
	else if (temppcb->state == RUNNING)
	{
		char tempstate[8] = "Running";
		printf("%8s", tempstate);
	}
	
	else 
	{
		char tempstate[8] = "Blocked";
		printf("%8s", tempstate);
	}
		
	if (temppcb->suspended == SUSP)
	{
		char temp[10] = "Yes";
		printf("%10s", temp);
	}
	
	else 
	{
		char temp[10] = "No";
		printf("%10s", temp);
	}
		
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
int show_All() 
{
    struct PCB *temppcb = NULL;
    int bufsize         = BIGBUFF;
    int i = 2, x = 0;
    char buffer[BIGBUFF] = {0};
	temppcb              = head1;	
	errx                 = 0;
	
	printf("\n     NAME       CLASS PRIORITY   STATE SUSPENDED\n");
	printf("--------------------------------------------------");
	
	for (x; x <= 1; x++) 
	{
		while (temppcb != NULL) 
		{
			printf ("\n%9s", temppcb->name);
			
			if (temppcb->proc_class == SYSTEM)
			{
				char tempclass[12] = "System";
				printf("%12s", tempclass);
			}
			
			else 
			{
				char tempclass[12] = "Application";
				printf("%12s", tempclass);
			}
			
			printf("%9d", temppcb->priority);
			
			if (temppcb->state == READY)
			{
				char tempstate[8] = "Ready";
				printf("%8s", tempstate);
			}
			
			else if (temppcb->state == RUNNING)
			{
				char tempstate[8] = "Running";
				printf("%8s", tempstate);
			}
			
			else 
			{
				char tempstate[8] = "Blocked";
				printf("%8s", tempstate);
			}
			
			if (temppcb->suspended == SUSP)
			{
				char temp[10] = "Yes";
				printf("%10s", temp);
			}
			
			else 
			{
				char temp[10] = "No";
				printf("%10s", temp);
			}
			
			temppcb = temppcb->prev;
			i       = i++;
		
			if (i > 23) 
			{
				printf("Press Any Key to Continue");
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
 **               Purpose -- The show_All function displays information about all processes which are currently in the READY or the	   **
 **							  SUSPENDED-READY state.																				   **
 **							 name.																									   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- printf, sys_req																						   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The show_All function displays information about all processes which are currently in the READY or the	   **
 **							 SUSPENDED-READY state. 																				   **				
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int show_Ready() 
{
	struct PCB *temppcb  = NULL;
    int bufsize		     = BIGBUFF;
    int i				 = 2;
    char buffer[BIGBUFF] = {0};
	temppcb				 = tail1;
	errx = 0;
	
	printf("\n     NAME       CLASS PRIORITY   STATE SUSPENDED\n");
	printf("--------------------------------------------------");
	
	while (temppcb != NULL) 
	{
		printf ("\n%9s", stemppcb->name);
		
		if (temppcb->proc_class == SYSTEM)
		{
			char tempclass[12] = "System";
			printf("%12s", tempclass);
		}
		
		else 
		{
			char tempclass[12] = "Application";
			printf("%12s", tempclass);
		}
		
		printf("%9d", temppcb->priority);
		
		if (temppcb->state == READY)
		{
			char tempstate[8] = "Ready";
			printf("%8s", tempstate);
		}
		
		else if (temppcb->state == RUNNING)
		{
			char tempstate[8] = "Running";
			printf("%8s", tempstate);
		}
		
		else 
		{
			char tempstate[8] = "Blocked";
			printf("%8s", tempstate);
		}
		
		if (temppcb->suspended == SUSP)
		{
			char temp[10] = "Yes";
			printf("%10s", temp);
		}
		
		else 
		{
			char temp[10] = "No";
			printf("%10s", temp);
		}
		
		temppcb = temppcb->next;
		i		= i + 4;
		
		if (i > 20) 
		{
			printf("Press Any Key to Continue");
			errx = sys_req(READ, TERMINAL, buffer, &bufsize);
			i = 0;
		}
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
int show_Blocked() 
{
	struct PCB *temppcb  = NULL;
    int bufsize			 = BIGBUFF;
    int i				 = 2;
    char buffer[BIGBUFF] = {0};
	temppcb				 = tail2;
	errx = 0;
	
	printf("\n     NAME       CLASS PRIORITY   STATE SUSPENDED\n");
	printf("--------------------------------------------------");
	
	while (temppcb != NULL) 
	{
        printf ("\n%9s", stemppcb->name);
		
		if (temppcb->proc_class == SYSTEM)
		{
			char tempclass[12] = "System";
			printf("%12s", tempclass);
		}
		
		else 
		{
			char tempclass[12] = "Application";
			printf("%12s", tempclass);
		}
		
		printf("%9d", temppcb->priority);
		
		if (temppcb->state == READY)
		{
			char tempstate[8] = "Ready";
			printf("%8s", tempstate);
		}
		
		else if (temppcb->state == RUNNING)
		{
			char tempstate[8] = "Running";
			printf("%8s", tempstate);
		}
		
		else 
		{
			char tempstate[8] = "Blocked";
			printf("%8s", tempstate);
		}
		
		if (temppcb->suspended == SUSP)
		{
			char temp[10] = "Yes";
			printf("%10s", temp);
		}
		
		else 
		{
			char temp[10] = "No";
			printf("%10s", temp);
		}
		
		temppcb = temppcb->next;
		i		= i + 4;

		if (i > 20) 
		{       
			printf("Press Any Key to Continue");
			errx = sys_req(READ, TERMINAL, buffer, &bufsize);
			i    = 0;
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
int delete_PCB() 
{
	char buff[BIGBUFF];
	struct PCB *tmp = NULL;
	int buffsize    = BIGBUFF;
	memset(buff, '\0', BIGBUFF);
    errx = 0;
	
	printf("Please Enter the Name of the Process to Delete: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);	
	
	if (errx >= OK) 
	{
	    //if(tmp->proc_class != SYSTEM){
		trimx(buff);
		tmp = qRemove(buff,tmp);
		free_PCB(tmp);
		//}
		//else
		//	return ERR_UTDSC;
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
 **     Procedures Called -- printf, sys_req, findPCB, strncpy, trimx, atoi, allocate_PCB, insert									   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The create_PCB function prompts the user for the process name, the process class, and the process		   **
 **							 priority.  create_PCB processes the user input, allocates a new PCB and inserts it into the RUNNING	   **
 **							 queue.																									   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */ 
int create_PCB() 
{
    char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	char name[PROCESS_NAME_LENGTH];
	int proc_class, priority;
	struct PCB *temppcb   = NULL;
	struct PCB *newPCBptr = NULL;
	memset(buff, '\0', BIGBUFF);
    errx = 0;
    
	printf("Please Enter the Name of the Process to be Created (9 Character Limit): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	
	if (errx < OK) return errx;
	
	trimx(buff);
    
	if (strlen(buff) > 9) return ERR_PRONTL;
    
	temppcb = findPCB(buff, temppcb);
	
	if (temppcb != NULL) return ERR_NAMEAE;
    
	strncpy(name, buff, PROCESS_NAME_LENGTH);
    
    printf("\nPlease Enter the Class of the Process to be Created ('0' = Application, '1' = System): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	
	if (errx < OK) return errx;
	
	trimx(buff);
    
	if (strncmp(buff, "0\0", 2) && strncmp(buff, "1\0", 2)) return ERR_INVCLS;
    proc_class = atoi(buff);
    
    printf("\nPlease Enter the Priority of the Process to be Created Where 127 is the highest (-128 to 127): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	
	if (errx < OK) return errx;
	
	trimx(buff);
    priority  = atoi(buff);
	newPCBptr = allocate_PCB();
	
	if (newPCBptr == NULL) errx = ERR_UCPCB;
    
	else 
	{
		errx = setup_PCB(newPCBptr, name, proc_class, priority);
	  
		if (errx < OK) return errx;
	  
		errx = insert(newPCBptr, RUNNING);
	}
	
	return errx;
	
	//if(temp<=127 && temp>=-128) temppcb->priority = temp;
	//else {
    //  temppcb->priority = 0;
    //  printf("\nInvalid priority level.  Priority defaulted to 0.");
    //}
	//printf("\nPriority for %s successfully set to %d",temppcb->name,temppcb->priority);
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
struct PCB * allocate_PCB() 
{
	struct PCB *newPCBptr = NULL;
	newPCBptr			  = sys_alloc_mem((sizeof(struct PCB)));
	newPCBptr->stack_base = (unsigned char *)sys_alloc_mem(STACK_SIZE * sizeof(unsigned char));
	newPCBptr->stack_top  = newPCBptr->stack_base + STACK_SIZE-sizeof(struct context);
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
int free_PCB(struct PCB *PCBptr) 
{
    errx = 0;
	errx = sys_free_mem(PCBptr->stack_base);
	errx = sys_free_mem(PCBptr->load_address);
	errx = sys_free_mem(PCBptr->execution_address);
	errx = sys_free_mem(PCBptr);
	//free(PCBptr);
	
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
int setup_PCB(struct PCB *PCBptr, char name[PROCESS_NAME_LENGTH], int proc_class, int priority) 
{
	errx = 0;
	strncpy((PCBptr->name), name, PROCESS_NAME_LENGTH);
	(PCBptr->proc_class) = proc_class;
	(PCBptr->priority)   = priority;
	(PCBptr->state)      = READY;
	PCBptr->suspended    = NOTSUSP;
	//(PCBptr->stack_base) = (PCBptr->stack)[STACK_SIZE] ;
	//(PCBptr->stack_top) = (PCBptr->stack)[STACK_SIZE + 1];
	//(PCBptr->mem_size) = ;
	//(PCBptr->load_address) = ;
	//(PCBptr->execution_address) = ;
	(PCBptr->prev) = NULL;
	(PCBptr->next) = NULL;
	
	return errx;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- isEmpty																								   **
 **               Purpose -- The isEmpty checks to see if the queue is empty.														   **
 **            Parameters -- int - represents the queue																				   **
 **			 Return Value -- int - boolean																							   **
 **     Procedures Called -- strncpy																								   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The isEmpty checks to see if the queue is empty.  It returns '1' if it is empty.						   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int isEmpty(int q) 
{
    int ret = 0;
    if (q == 1) 
	{
		if (head1 == NULL && tail1 == NULL) ret = 1;
	}
    
	else if (head2 == NULL && tail2 == NULL) ret = 1;
	
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
int insert(struct PCB *newPCB, int q) 
{
	struct PCB *tmp = NULL;
    errx = 0;
    
	if (q == 1)
	{
		if (isEmpty(q)) 
		{
			tail1 = newPCB;
			head1 = tail1;
		}
		
		else 
		{
			tmp = tail1;
	    
			while ((newPCB->priority) > (tmp->priority)) 
			{
				if (tmp == head1) 
				{
					(tmp->next)    = newPCB;
					(newPCB->prev) = tmp;
					head1          = newPCB;
				}
		  
				else tmp = (tmp->next);
			}
        
			if (tmp == tail1) // If Inserted At Tail
			{
				tail1->prev  = newPCB;
				newPCB->next = tail1;
				tail1		 = newPCB;
			}
        
			else if (head1 != newPCB) 
			{
				((tmp->prev)->next) = newPCB;
				(newPCB->prev)      = (tmp->prev);
				(tmp->prev)         = newPCB;
				(newPCB->next)      = tmp;
			}
		}
    }
	
    else // Blocked Queue 
	{ 
		if (isEmpty(q)) 
		{
			tail2 = newPCB;
			head2 = tail2;
		}
      
		else 
		{
			tmp = tail2;
        
			while ((newPCB->priority) > (tmp->priority)) 
			{
				if (tmp == head2) 
				{
					(tmp->next)    = newPCB;
					(newPCB->prev) = tmp;
					head2		   = newPCB;
				}
		  
				else tmp = (tmp->next);
			}
			
			if (tmp==tail2) // If Inserted At Tail
			{ 
				tail2->prev  = newPCB;
				newPCB->next = tail2;
				tail2		 = newPCB;
			}
				
			else if (head2 != newPCB) 
			{
				((tmp->prev)->next) = newPCB;
				(newPCB->prev)		= (tmp->prev);
				(tmp->prev)			= newPCB;
				(newPCB->next)		= tmp;
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
struct PCB* findPCB(char *name, struct PCB *PCBptr) 
{
	struct PCB *tmp = tail1;
    errx			= 0;
	
    while ((tmp != NULL) && strncmp((tmp->name),name,PROCESS_NAME_LENGTH)) tmp = (tmp->next);
    
	PCBptr = tmp;
    
	if (PCBptr == NULL) //If Not Found Yet, Search Queue 2
	{
		tmp = tail2;
      
		while ((tmp != NULL) && strncmp((tmp->name),name,PROCESS_NAME_LENGTH)) tmp = (tmp->next);
      
		if (tmp == NULL) errx = ERR_PCBNF; //PCB Not Found
      
		else if (tmp != NULL) PCBptr = tmp;
    }
	
    return PCBptr;
}

/**  Procedure Name: qremove
* \param name: string containing PCB name
* \param set: a pointer to a PCB
* \return an integer error code (o for now)
* \return sets set to removed PCB
* Procedures Called: findPCB
* Globals Used: err
* \brief Description: removes a PCB from queue
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

struct PCB* getRHead() {
       return head1;
}

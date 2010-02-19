// Included ANSI C Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
#define STACK_SIZE 1024

// Structures
/** \struct PCB
* The PCB represents a process control block, containing all information about a process and pointers to the next/prev PCBs in a queue.
*/
typedef struct PCB {
	char name[PROCESS_NAME_LENGTH];         //Process Name
	int id;                                 //Process ID#
	int proc_class;						    //Process Class
	int priority;					        //Priority Value (-128 to 127)
	int state;						        //Process State Flag (Running, Ready, Blocked)
	int suspended;					        //Process Suspended Flag
	unsigned char stack[STACK_SIZE];        //PCB Stack
	unsigned char* stack_base;				//Pointer to base of stack
	unsigned char* stack_top;				//Pointer to top of stack
	int mem_size;							//Memory size
	unsigned char* load_address;			//Pointer to loading address
	unsigned char* execution_address;		//Pointer to execution address
	struct PCB *prev;				        //Pointer to previous PCB node
	struct PCB *next;				       	//Pointer to next PCB node
} PCB;

// Global Variables
struct PCB *tail1, *tail2, *head1, *head2;

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
int findPCB(char *name, struct PCB *PCBptr);
int qRemove(char *name, struct PCB *set);

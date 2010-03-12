// Status and Error Codes
#define ERR_INVCOM (-201) // Invalid command
#define ERR_INVYR (-202)  // Invalid year
#define ERR_INVMON (-203) // Invalid month
#define ERR_INVDAY (-204) // Invalid day
#define ERR_PCBNF  (-205)    //PCB Not Found
#define ERR_QUEEMP (-206)    //Queue is Empty
#define ERR_UCPCB  (-207)    //Unable to Create PCB
#define ERR_PRONTL (-208)    //Process Name too Long
#define ERR_NAMEAE (-209)    //Process Name Already Exists
#define ERR_INVCLS (-210)    //Invalid Class
#define NO_DEV		0
#define	TERMINAL	1
#define	PRINTER		2
#define	COM_PORT	3
#define NUM_DEVS	3
#define IDLE	0
#define	READ	1
#define	WRITE	2
#define CLEAR	3
#define GOTOXY	4
#define EXIT	5

// Constants
#define BIGBUFF 80
#define SMALLBUFF 10
#define TINYBUFF 2
#define DATE 0
#define HELP 1
#define VER 2
#define DIR 3
#define QUIT 4
#define LIST 5
#define CREATEPCB 6
#define DELPCB 7
#define BLOCK 8
#define UNBLOCK 9
#define SUSPEND 10
#define RES 11
#define SETPRI 12
#define SHOWPCB 13
#define SHOWALL 14
#define SHOWREADY 15
#define SHOWBLOCKED 16
#define VERSION 2.0
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
int err = 0;  //error code
char * fcns[18] = {"date\0","help\0","ver\0","dir\0","quit\0","list\0","cpcb\0","dpcb\0","block\0","unblock\0","suspend\0","resume\0","setpri\0","shpcb\0","shall\0","shready\0","shblock\0",NULL};  //functions list
char wd[BIGBUFF*2] = {0};  //working directory
struct PCB *tail1=NULL, *tail2=NULL, *head1=NULL, *head2=NULL;
int errx = 0;

//Structures
typedef struct PCB {
	char name[PROCESS_NAME_LENGTH];         //Process Name
	int id;                                 //Process ID#
	int proc_class;						    //Process Class
	int priority;					        //Priority Value (-128 to 127)
	int state;						    //Process State Flag (Running, Ready, Blocked)
	int suspended;					    //Process Suspended Flag
	unsigned char stack[STACK_SIZE];        //PCB Stack
	unsigned char* stack_base;				//Pointer to base of stack
	unsigned char* stack_top;				//Pointer to top of stack
	int mem_size;							//Memory size
	unsigned char* load_address;			//Pointer to loading address
	unsigned char* execution_address;		//Pointer to execution address
	struct PCB *prev;				        //Pointer to previous PCB node
	struct PCB *next;				       	//Pointer to next PCB node
} ;

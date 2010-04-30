// Status and Error Codes
#define ERR_OPEN_NULL_EFLAGP -101
#define ERR_OPEN_INV_BAUD_RATE -102
#define ERR_OPEN_PORT_OPEN -103
#define ERR_CLOSE_PORT_NOT_OPEN -104
#define ERR_READ_PORT_NOT_OPEN -105
#define ERR_READ_INV_BUFF_ADDR -106
#define ERR_READ_INV_COUNT_VAR -107
#define ERR_READ_DEVICE_BUSY -108
#define ERR_WRITE_PORT_NOT_OPEN -109
#define ERR_WRITE_INV_BUFF_ADDR -110
#define ERR_WRITE_INV_COUNT_VAR -111
#define ERR_WRITE_DEVICE_BUSY -112
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
#define ERR_UTDSC  (-211)    //Unable to Delete System Class
#define ERR_UNKN_REQUEST (-212)  //Unknown Request
#define ERR_UNKN_DEVICE  (-213)  //Unknown Device
#define ERR_CNESP (-214)         //Cannot edit system processes
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
#define NUMFCNS	28
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
#define DISPATCH 17
#define LOADPROCS 18
#define LOAD 19
#define TERMINATE 20
#define CHGPROMPT 21
#define RSTPROMPT 22
#define ALIAS 23
#define RSTALIAS 24
#define RDHIST 25
#define CLRHIST 26
#define VERSION 4.0
#define PROCESS_NAME_LENGTH 10
#define SYSTEM 1
#define APP 0
#define READY 0
#define RUNNING 1
#define BLOCKED 2
#define NOTSUSP 0
#define SUSP 1
#define STACK_SIZE 4048
#define SYS_STACK_SIZE 200
#define COMHAN_STACK_SIZE 8192
#define COM1_READ 0x04
#define COM1_WRITE 0x02
#define IDLE 0
#define READING 1
#define WRITING 2
#define CLOSED 0
#define OPEN 1
#define RING_SIZE 128
#define COM1_INT_ID 0x0c
#define COM1_BASE 0x3f8
#define COM1_INT_EN COM1_BASE+1
#define COM1_BRD_LSB COM1_BASE
#define COM1_BRD_MSB COM1_BASE+1
#define COM1_INT_ID_REG COM1_BASE+2
#define COM1_LC COM1_BASE+3
#define COM1_MC COM1_BASE+4
#define COM1_MS COM1_BASE+6
#define PIC_MASK 0x21
#define PIC_CMD 0x20
#define EOI 0x20

// Function Prototypes
//R1
void err_hand(int err_code);
int init_r1();
int disp_dir();
int comhan();
void terminate_mpx();
int help();
void get_Version();
int date();
int valid_date(int yr, int mo, int day);
void toLowerCase(char str[BIGBUFF]);
void trim(char ary[BIGBUFF]);
int changePrompt();
void resetPrompt();
int alias();
void resetAlias();
int openTmp();
int closeTmp();
int writeHistory(char *command);
int readHistory();
int clearHistory();
//R2
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
int setup_PCB(struct PCB * PCBptr, char name[PROCESS_NAME_LENGTH], int proc_class, int priority);
int free_PCB(struct PCB * PCBptr);
int create_PCB();
int delete_PCB();
int isEmpty(int q);
int insert(struct PCB * newPCB,int q);
struct PCB * findPCB(char * name, struct PCB * PCBptr);
struct PCB * qRemove(char * name, struct PCB * set);
void toLowerCasex(char str[BIGBUFF]);
void trimx(char ary[BIGBUFF]);
struct PCB * getRHead();
//R3
int init_r3();
int cleanup_r3();
void test1_R3();
void test2_R3();
void test3_R3();
void test4_R3();
void test5_R3();
void interrupt sys_call();
void interrupt dispatcher();
int load_test();
//R4
int load_prog(char * fname, int pri, int procClass);
int terminate();
int load();
//R5
int com_open (int * eflag_p, int baud_rate);
int com_close();
int com_read(char * buf_p, int * count_p);
int com_write(char * buf_p, int * count_p);
void interrupt com_check();
void readCom();
void writeCom();
void stop_com_request();
//R6
int init_f();
int cleanup_f();
void IOschedule();
int process_com();
int process_trm();
int enqueue(struct IOD * nIOD, struct IOCB * queue);
struct IOD * dequeue(struct IOCB * queue);
struct IOD * createIOD();


//Structures
typedef struct PCB {
	char name[PROCESS_NAME_LENGTH];         //Process Name
	int id;                                 //Process ID#
	int proc_class;						    //Process Class
	int priority;					        //Priority Value (-128 to 127)
	int state;						        //Process State Flag (Running, Ready, Blocked)
	int suspended;					        //Process Suspended Flag
	unsigned char stack[STACK_SIZE];        //PCB Stack
	unsigned char * stack_base;				//Pointer to base of stack
	unsigned char * stack_top;				//Pointer to top of stack
	int mem_size;							//Memory size
	unsigned char * load_address;			//Pointer to loading address
	unsigned char * execution_address;		//Pointer to execution address
	struct PCB * prev;				        //Pointer to previous PCB node
	struct PCB * next;				       	//Pointer to next PCB node
};

typedef struct params {
	int op_code;
	int device_id;
	char * buf_p;
	int * count_p;
} params;

typedef struct context {
	unsigned int BP, DI, SI, DS, ES;
	unsigned int DX, CX, BX, AX;
	unsigned int IP, CS, FLAGS;
};

typedef struct DCB {
	int flagOpen;          //is COM open
	int * eventFlagp;	   //
	int status;		       //COM current status
	char * in_buff;	       //buffer of requestor
	int * in_count;	       //in_buff limit
	int in_done;	       //# chars actually put into in_buff
	char * out_buff;        //
	int * out_count;	       //
	int out_done;	       //
	char ring_buffer[RING_SIZE];    //
	int ring_buffer_in;    //index for next write
	int ring_buffer_out;   //index for next read
	int ring_buffer_count; //
};

typedef struct IOD {
	char name[PROCESS_NAME_LENGTH];
	struct PCB * requestor;
	int request;
	char * tran_buff; //transfer buffer
	int * buff_count;
	struct IOD * next;
};

typedef struct IOCB {	
    int * event_flag;
	int count;
	struct IOD * head;
	struct IOD * tail;
	struct IOD * active;
};


// Global Variables
int err = 0;  //error code
char * fcns[NUMFCNS] = {"date\0","help\0","ver\0","dir\0","quit\0","list\0","cpcb\0","dpcb\0","block\0","unblock\0","suspend\0","resume\0","setpri\0","shpcb\0","shall\0","shready\0","shblock\0","dispat\0","ldprocs\0","load\0","term\0","chgprom\0","rstprom\0","alias\0","rsalias\0","rdhist\0","clrhist\0",NULL};  //functions list
char * alfcns[NUMFCNS] = {"date\0","help\0","ver\0","dir\0","quit\0","list\0","cpcb\0","dpcb\0","block\0","unblock\0","suspend\0","resume\0","setpri\0","shpcb\0","shall\0","shready\0","shblock\0","dispat\0","ldprocs\0","load\0","term\0","chgprom\0","rstprom\0","alias\0","rsalias\0","rdhist\0","clrhist\0",NULL};  //aliased functions list
char prompt[10] = ">>\0";
char alPrompt[10];
FILE * tmpfp;
char wd[BIGBUFF*2] = {0};  //working directory
struct PCB *tail1=NULL, *tail2=NULL, *head1=NULL, *head2=NULL;
int errx = 0;
static unsigned short ss_save;
static unsigned short sp_save;
static unsigned short ss_save_temp;
static unsigned short sp_save_temp;
static unsigned short new_ss;
static unsigned short new_sp;
static unsigned char sys_stack[SYS_STACK_SIZE]={0};
static struct PCB * cop;
static struct PCB * tempnode;
struct context * context_p;
struct params * param_p;
int err3=0;
int err4 = 0;
struct IOCB *comport;
struct IOCB *terminal;
struct IOD *tmpIOD;

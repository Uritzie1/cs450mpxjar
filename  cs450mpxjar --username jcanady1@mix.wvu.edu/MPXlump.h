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
#define DISPATCH 17
#define LOADPROCS 18
#define LOAD 19
#define TERMINATE 20
#define VERSION 4.0
#define PROCESS_NAME_LENGTH 10
#define SYSTEM 1
#define APP 0
#define READY 0
#define RUNNING 1
#define BLOCKED 2
#define NOTSUSP 0
#define SUSP 1
#define STACK_SIZE 2048
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
}context;

typedef struct DCB {
	int flagOpen;          //is COM open
	int * eventFlagp;	   //
	int status;		       //COM current status
	char * in_buff;	       //
	int * in_count;	       //
	int in_done;	       //
	char * out_buff;        //
	int * out_count;	       //
	int out_done;	       //
	char ring_buffer[RING_SIZE];    //
	int ring_buffer_in;    //
	int ring_buffer_out;   //
	int ring_buffer_count; //
};

typedef struct iod
{
	struct PCB *process;	/**<Pointer to process control block. */
	int request_type;	/**<Type of input/output request (i.e. IDLE, READ, WRITE, CLEAR). */
	char *buf_p;		/**<Transfer buffer. */
	int *count_p;		/**<Count of buffer. */
	struct iod *next;	/**<Next IOD in queue. */
} iod;
typedef struct iocb
{
	int event_flag;	/**<Device's event flag (the address of this will be passed to com_open for initialization!). */
	int count;		/**<Number of requests in this devices wait queue. */
	iod* active;	/**<Active request. */
	iod* head;		/**<Pointer to first iod in waiting queue. */
	iod* tail;		/**<Pointer to last iod in waiting queue. */
} iocb;


// Function Prototypes
void err_hand(int err_code);
int init_r1();
int cleanup_r1();
int disp_dir();
int comhan();
void terminate_mpx();
int help(char * cmdName);
void get_Version();
int date();
int valid_date(int yr, int mo, int day);
void toLowerCase(char str[BIGBUFF]);
void trim(char ary[BIGBUFF]);
//
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
//
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
//
int load_prog(char * fname, int pri, int procClass);
int terminate();
int load();
//
int init_f();
int cleanup_f();
void comport_Enqueue(iod* temp_iod);
iod* comport_Dequeue();
void terminal_Enqueue(iod* temp_iod);
iod* terminal_Dequeue();
void removeRequests(struct PCB * tempnode);
iod* setupIOD();
void IOScheduler();
void process_comport();
void process_terminal();
void freeRequestQueues();
//
int com_open (int * eflag_p, int baud_rate);
int com_close();
int com_read(char * buf_p, int * count_p);
int com_write(char * buf_p, int * count_p);
void interrupt com_check();
void readCom();
void writeCom();
void stop_com_request();



// Status and Error Codes
#define ERR_INVCOM (-201) // Invalid command
#define ERR_INVYR (-202)  // Invalid year
#define ERR_INVMON (-203) // Invalid month
#define ERR_INVDAY (-204) // Invalid day

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
#define SHOWBLOCKED 15
#define VERSION 1.72

// Global Variables
int err = 0;  //error code
char * fcns[18] = {"date\0","help\0","ver\0","dir\0","quit\0","list\0","createpcb\0","deletepcb\0","block\0","unblock\0","suspend\0","resume\0","setpriority\0","showpcb\0","showall\0","showready\0","showblocked\0",NULL};  //functions list
char wd[BIGBUFF*2] = {0};  //working directory

// Function Prototypes
void err_hand(int err_code);
int init_r1();
int cleanup_r1();
int disp_dir();
int comhan();
void terminate_mpx();
int help(char *cmdName);
void get_Version();
int date();
int valid_date(int yr, int mo, int day);
void toLowerCase(char str[BIGBUFF]);
void trim(char ary[BIGBUFF]);

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **																FILE INFORMATION													   **
 **																																	   **
 **		FILENAME -- COMHAN.c																										   **	
 **      AUTHORS -- Jonroy Canaday, Adam Trainer, Robert Wayland																	   **
 **      VERSION -- 2.0																												   **
 ** LAST UPDATED -- April 24, 2010																									   **
 **																																	   **
 **   COMPONENTS -- cleanup_r1, comhan, date, disp_dir, err_hand, get_Version, help, init_r1, terminate_mpx, toLowerCase, trim,		   **
 **				    valid_date																										   **
 **																																	   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **																   CHANGE LOG														   **
 **																																	   **
 **		  01/25/2010  JC           nonfunctional, outline of COMHAN.c																   **
 **       01/28/2010  JC, RW       slight function editing of COMHAN.c																   **
 **       02/01/2010  JC, RW       initial errors resolved; successful compilation of COMHAN.c										   **
 **       02/02/2010  JC, RW       version and help functionality added to COMHAN.c													   **
 **       02/03/2010  JC           dir, date, and err_hand functionality added; functional R1 COMHAN.c								   **
 **       02/04/2010  JC, RW, AT   help functionality added; documentation added; completed R1 COMHAN.c								   **
 **       02/08/2010  JC           dir functionality improved; documentation added													   **
 **		  04/23/2010  AT		   code review; additional documentation added														   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */

/* Included ANSI C Files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Included MPX Support Files */
#include "mpx_supt.h"

/* Status & Error Codes */
#define ERR_INVCOM (-201)		// Invalid Command
#define ERR_INVYR  (-202)		// Invalid Year
#define ERR_INVMON (-203)		// Invalid Month
#define ERR_INVDAY (-204)		// Invalid Day
#define ERR_PCBNF  (-205)		// PCB Not Found
#define ERR_QUEEMP (-206)		// Queue Is Empty
#define ERR_UCPCB  (-207)		// Unable to Create PCB
#define ERR_PRONTL (-208)		// Process Name Too Long
#define ERR_NAMEAE (-209)		// Process Name Already Exists
#define ERR_INVCLS (-210)		// Invalid Class

/* Macros */
#define BIGBUFF     80
#define SMALLBUFF   10
#define TINYBUFF     2
#define DATE	     0
#define HELP	     1
#define VER		     2
#define DIR		     3
#define QUIT	     4
#define LIST	     5
#define CREATEPCB    6 
#define DELPCB	     7
#define BLOCK        8
#define UNBLOCK      9
#define SUSPEND     10
#define RES         11
#define SETPRI      12
#define SHOWPCB     13
#define SHOWALL     14
#define SHOWREADY   15
#define SHOWBLOCKED 16
#define DISPATCH    17
#define LOADPROCS   18
#define VERSION    2.0
#define NUMFCNS	   20;

/* Global Variables */
int err            = 0;  // Error Codes
char * fcns[NUMFCNS]    = {"date\0", "help\0", "ver\0", "dir\0", "quit\0", "list\0", "cpcb\0", "dpcb\0", "block\0", "unblock\0", "suspend\0", 
					  "resume\0", "setpri\0", "shpcb\0", "shall\0", "shready\0", "shblock\0", "dispat\0", "ldprocs\0", NULL}; // Available Functions
char * alfcns[NUMFCNS] = 0;
char prompt = ">>";
char * alPrompt = prompt;
char wd[BIGBUFF * 2] = {0}; // Working Directory

/* Function Prototypes */



int changePrompt()
{
	int bufsize = 10;
	char cmd[10] = {0};

	printf("Enter new prompt symbol (max 10 characters): ")
	if (err = sys_req(READ, TERMINAL, cmd, &bufsize) < OK) return err);

	alprompt = cmd;
}
void resetPrompt()
{
	alprompt = prompt;
}

int alias()
{
	int bufsize = BIGBUFF;
	int i = 0;
	char cmd[BIGBUFF] = {0};
	char ncmd[BIGBUFF] = {0};

	printf("Enter the command to be aliased:  ")
	if (err = sys_req(READ, TERMINAL, cmd, &bufsize) < OK) return err);
	for(i<NUMFCNS;i++)
	{
		if(strncmp(cmd,alfcns[i],length(alfcns[i]+1)
		{
			printf("Enter the command's new name:  ")
			if (err = sys_req(READ, TERMINAL, ncmd, &bufsize) < OK) return err);
			alfcns[i] = ncmd;
		}
	}
	if(i = NUMFCNS)
	{
		return ERR_INVCOM;
	}
	printf("change succesful\n"):
	return 0

}
void resetAlias()
{
	int i = 0;
	for(i < NUMFCNS;i++)
	{
		alfcns[i] = fcns[i];
	}
}
//need global FILE *tmpFP
int openTmp()
{
 char tmpname[L_tmpnam];
 char *filename = NULL;

 filename = tmpnam(tmpname);

 printf("Temp file: %s\n", filename);

 tmpfp = tmpfile();
 if(tmpfp)
  printf("Opened a temp file: OK\n");
 else
  perror("tmpfile");

 return 0;
}
int closeTMP()
{
	close(tmpfp);
	return 0;
}
int writeHistory(char *command)
{
	fprintf(tmpfp,command);
}
int readHistory()
{
	char buffer[BIGBUFF] = {0};

	while (fgets(buffer, BIGBUFF, tmpfp)) 
	{
		printf("%s",buffer);
		i++;		
		if (i == 24) // Paging Functionality 
		{
			printf("Press Any Key to Continue");
			err = sys_req(READ, TERMINAL, buffer, &bufsize);
			i = 0;
		}
	}
}
int clearHistory()
{
	closeTMP();
	tmpfp = NULL;
	openTMP();
}

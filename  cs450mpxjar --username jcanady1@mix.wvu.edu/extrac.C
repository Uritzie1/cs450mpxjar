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
int cleanup_r1();
int comhan();
int date();
int disp_dir();
int help(char *cmdName);
int init_r1();
int valid_date(int yr, int mo, int day);
void err_hand(int err_code);
void get_Version();
void terminate_mpx();
void toLowerCase(char str[BIGBUFF]);
void trim(char ary[BIGBUFF]);

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- main																									   **
 **               Purpose -- The main function is where the the program begins execution.											   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- sys_init, init_r1, init_r2, init_r3, comhan, cleanup_r1, cleanup_r2, cleanup_r3, terminate_mpx			   **
 **  Global Data Accessed -- int err																								   **
 **  Summary of Algorithm -- The main function initializes the system, calls comhan, cleans up the OS, and terminates JAROS.  		   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int main() 
{
	sys_init(MODULE_R3);
	err = init_r1();
	err = init_r2();
	err = init_r3();
	err = comhan();
	err = cleanup_r1();
	err = cleanup_r2();
	err = cleanup_r3();
	terminate_mpx();
	
	return 0;
}


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
int readHistory
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





int comhan() 
{
	char cmd[BIGBUFF] = {0};
	int bufsize		  = BIGBUFF;
    printf("\nWelcome to JAROS!\n");
	
	while (1) 
	{
		bufsize = BIGBUFF;
		memset(cmd, '\0', BIGBUFF);                       // Clear Input Buffer
		printf("\n%s",alprompt);                                   // Display Command Prompt
		err = sys_req(READ, TERMINAL, cmd, &bufsize);     // Read User-Inputted Command
		trim(cmd);										  // Trim Extraneous Whitespace from User Input
		toLowerCase(cmd);								  // Convert User Input to Lowercase

/**
 ****************************************************************************************************************************************
 *											    				    R1 Commands											     		    *
 ****************************************************************************************************************************************
 */
		/* Quit */
		if (!strncmp(cmd, fcns[QUIT], strlen(fcns[QUIT]) + 1)) terminate_mpx();
		
		/* Display Version */
		else if (!strncmp(cmd, fcns[VER], strlen(fcns[VER]))) get_Version();
		
		/* Display Help Information */
		else if (!strncmp(cmd, fcns[HELP], strlen(fcns[HELP]) + 1)) 
		{
			err = help(NULL);
      
			if (err < OK) err_hand(err);
		}
		
		/* Display Date */
		else if (!strncmp(cmd, fcns[DATE], strlen(fcns[DATE]) + 1)) 
		{
			err = date();
      
			if (err < OK) err_hand(err);
		}
		
		/* Display Directory */
		else if (!strncmp(cmd, fcns[DIR], strlen(fcns[DIR]) + 1)) 
		{
			err = disp_dir();
			
			if (err < OK) err_hand(err);
		}
/**
 ****************************************************************************************************************************************
 *											    				    R2 Commands											     		    *
 ****************************************************************************************************************************************
 */
		/* Create PCB */	
		else if (!strncmp(cmd, fcns[CREATEPCB], strlen(fcns[CREATEPCB]) + 1)) 
		{
			err = create_PCB();
		  
			if (err < OK) err_hand(err);
		}
		
		/* Delete PCB */
		else if (!strncmp(cmd,fcns[DELPCB], strlen(fcns[DELPCB]) + 1)) 
		{
			err = delete_PCB();
		  
			if (err < OK) err_hand(err);
		}
		
		/* Block */
		else if (!strncmp(cmd, fcns[BLOCK], strlen(fcns[BLOCK]) + 1)) 
		{
			err = block();
		  
			if (err < OK) err_hand(err);
		}

		/* Unblock */
		else if (!strncmp(cmd,fcns[UNBLOCK], strlen(fcns[UNBLOCK]) + 1)) 
		{
			err = unblock();
		  
			if (err < OK) err_hand(err);
		}
		
		/* Suspend */
		else if (!strncmp(cmd, fcns[SUSPEND], strlen(fcns[SUSPEND]) + 1)) 
		{
			err = suspend();
		  
			if (err < OK) err_hand(err);
		}
		
		/* Resume */
		else if (!strncmp(cmd, fcns[RES], strlen(fcns[RES]) + 1)) 
		{
			err = resume();
		  
			if (err < OK) err_hand(err);
		}
		
		/* Set Priority */
		else if (!strncmp(cmd, fcns[SETPRI], strlen(fcns[SETPRI]) + 1)) 
		{
			err = set_Priority();
		  
			if (err < OK) err_hand(err);
		}
		
		/* Show PCB */
		else if (!strncmp(cmd, fcns[SHOWPCB], strlen(fcns[SHOWPCB]) + 1)) 
		{
			err = show_PCB();
		  
			if (err < OK) err_hand(err);
		}
		
		/* Show All */
		else if (!strncmp(cmd, fcns[SHOWALL], strlen(fcns[SHOWALL]) + 1)) 
		{
			err = show_All();
		  
			if (err < OK) err_hand(err);
		}
		
		/* Show Ready */
		else if (!strncmp(cmd, fcns[SHOWREADY], strlen(fcns[SHOWREADY]) + 1)) 
		{
			err = show_Ready();
		  
			if (err < OK) err_hand(err);
		}
		
		/* Show Blocked */
		else if (!strncmp(cmd, fcns[SHOWBLOCKED], strlen(fcns[SHOWBLOCKED]) + 1)) 
		{
			err = show_Blocked();
		  
			if (err < OK) err_hand(err);
		}

/**
 ****************************************************************************************************************************************
 *											    				    R3 Commands											     		    *
 ****************************************************************************************************************************************
 */
		/* Dispatch */
		else if (!strncmp(cmd,fcns[DISPATCH],strlen(fcns[DISPATCH])+1)) dispatcher();
		
		/* Load Processes */
		else if (!strncmp(cmd,fcns[LOADPROCS],strlen(fcns[LOADPROCS])+1)) 
		{
			err = load_test();
			
			if (err < OK) err_hand(err);
		}
		
		/* Error Handling for Invalid Commands */
		else if (!strncmp(cmd, "\n", 1));
		
		else err_hand(ERR_INVCOM);
	}
	
	// terminate_mpx();
	
	return 0;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- disp_dir																								   **
 **               Purpose -- The disp_dir function displays information about all files present in a specific directory which          **
 **							 (apparently) contains executable MPX processes.  These files are recognized by their filename extension:  **
 **							 ".mpx".																								   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- memset, strcat, sys_open_dir, printf, sys_get_entry, sys_close_dir										   **
 **  Global Data Accessed -- int err, char wd[]																						   **
 **  Summary of Algorithm -- The disp_dir function opens the current working directory, display the contents of the directory,		   **
 **							 and closes the directory.																				   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int disp_dir() 
{
	char namebuff[SMALLBUFF];
	long filesize;
	char wdc[BIGBUFF*2] = {0};
	int i;
	memset(namebuff, '\0', SMALLBUFF);
  
	for (i = 0; i < BIGBUFF * 2; i++) wdc[i] = wd[i];
	
	strcat(wdc,"\\MPXFILES\\");
	err = sys_open_dir(wdc);
	
	if (err < OK) return err;
  
	printf ("\nFilename     Size (bytes)");  //print list of MPX files
	
	while ((err = sys_get_entry(namebuff, SMALLBUFF, &filesize)) == OK) 
	{
		printf("\n%-9.9s     %ld", namebuff, filesize);
	}
  
	if (err < OK && err != ERR_SUP_NOENTR) return err;
  
	err = sys_close_dir();
	printf("\n");
	
	return err;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- terminate_mpx																							   **
 **               Purpose -- The terminate_mpx function stops execution of JAROS and returns to the host operating system.			   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- void																									   **
 **     Procedures Called -- memset, printf, sys_req, err_hand, trim, toLowerCase, cleanup_r1, cleanup_r2, cleanup_r3. sys_exit		   **
 **  Global Data Accessed -- int err																								   **
 **  Summary of Algorithm -- The terminate_mpx function prompts the user for confirmation.  If "Y", JAROS terminates execution. 	   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
void terminate_mpx() 
{
	char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	memset(buff, '\0', BIGBUFF);
	
	printf("Are You Sure You Want to Terminate JAROS (Y/N): ");
	err = sys_req(READ, TERMINAL, buff, &buffsize);
	
	if (err < OK) 
	{
		err_hand(err);
		
		return;
	}
	
	trim(buff);
	toLowerCase(buff);
	
	if (buff[0] == 'y') 
	{
		err = cleanup_r1();
		err = cleanup_r2();
		err = cleanup_r3();
		
		if (err < OK) err_hand(err);
		
		sys_exit();
  }
	
  else printf("Termination Cancelled");
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- get_Version																							   **
 **               Purpose -- The get_Version function displays a brief description of the version of JAROS that is currently running.  **
 **            Parameters -- N/A																									   **
 **			 Return Value -- void																									   **
 **     Procedures Called -- printf																									   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The get_Version function prints the value of the VERSION macro.										   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
void get_Version()
{
	printf("JAROS Current Version -- %f", VERSION);
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- trim																									   **
 **               Purpose -- The trim function removes extraneous whitespace and newlines from user input.							   **
 **            Parameters -- char [] - a buffer containing user input																   **
 **			 Return Value -- void																									   **
 **     Procedures Called -- isspace																								   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The trim function loops through a buffer, removing whitespace and newlines from the user input.		   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
 void trim(char ary[BIGBUFF]) 
{
	char temp[BIGBUFF] = {0};
	int i,j = 0;
	
	for (i = 0; i < BIGBUFF; i++) 
	{
		if (!isspace(ary[i])) 
		{
			if (ary[i] == 12) j++;  // Trim Newlines
			
			else 
			{
				temp[j] = ary[i];
				j++;
			}
		}
	}
	
	for (i = 0; i < BIGBUFF; i++) ary[i] = temp[i];
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- help																									   **
 **               Purpose -- The help function displays help information for MPX and for each command.								   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- sys_req, trim, toLowerCase, strlen, strncmp, strcat, fopen, fgets, printf, err_hand, fclose			   **
 **  Global Data Accessed -- int err, char wd[]																						   **
 **  Summary of Algorithm -- The help function processes user input, and displays help information for a specific command.			   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int help() 
{
	FILE *fptr           = 0;
	int i,j              = 0;
	int bufsize          = BIGBUFF;
	char buffer[BIGBUFF] = {0};
	char wdc[BIGBUFF*2]  = {0};
	char tbuffer[9]      = "\\help\\";
	
	for (i = 0; i < BIGBUFF * 2; i++) wdc[i] = wd[i];
	
	printf("Enter a Command or Type \'list\' for a List of Available Commands: ");
	
	if ((err = sys_req(READ, TERMINAL, buffer, &bufsize)) < OK) return err;
	
	trim(buffer);
	toLowerCase(buffer);
	
	for (i = 0; i < strlen(fcns); i++) if (!strncmp(fcns[i], buffer, strlen(fcns[i]))) j++;
	
	if (j > 0) // Build File Path
	{                
		strcat(wdc,tbuffer);
		strcat(wdc,buffer);
		strcat(wdc,".txt");
		//printf("%s",wdc);
		
		if ((fptr = fopen(wdc,"r")) > 0) 
		{
			i = 0;
			
			while (fgets(buffer, BIGBUFF, fptr)) 
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
		
		else 
		{
			printf("Help: ");
			err_hand(ERR_SUP_FILNFD);
		}
	}
	
	else err_hand(ERR_INVCOM);
	
	fclose(fptr);
	
	return err;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- date																									   **
 **               Purpose -- The help function displays the current date and prompts the user as to whether or not he wants to		   **
 **							 change the system date.																				   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- sys_get_date, printf, sys_req, trim, toLowerCase, atoi, err_hand										   **
 **  Global Data Accessed -- int err																								   **
 **  Summary of Algorithm -- The help function processes user input, and displays help information for a specific command.			   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int date() 
{
	char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	int x = 1, temp;
	date_rec *date_p;
  
	sys_get_date(date_p);
	printf("The Current Date Is (MM/DD/YYYY): %d/%d/%d",date_p->month,date_p->day,date_p->year);
	printf("\nWould You Like to Change the Date (Y/N): ");
	err = sys_req(READ, TERMINAL, buff, &buffsize);
  
	if (err < OK) return err;
  
	trim(buff);
	toLowerCase(buff);
  
	if (buff[0] == 'y') 
	{
		/* Change Year */
		while (x) 
		{
			printf("Please Enter the New Year (YYYY): ");
			err = sys_req(READ, TERMINAL, buff, &buffsize);
			
			if (err < OK) return err;
			
			temp = atoi(buff);
			
			if (temp==0) err_hand(ERR_INVYR); // Validate Input
			
			else if (temp >= 0 && temp < 10000) x = 0;
			
			else err_hand(ERR_INVYR);
		}
		
		x			 = 1;
		date_p->year = temp; 
		
		/* Change Month */
		while (x) 
		{
			printf("Please Enter the New Month (MM): ");
			err = sys_req(READ, TERMINAL, buff, &buffsize);
      
			if (err < OK) return err;
      
			temp = atoi(buff);
      
			if (temp==0) err_hand(ERR_INVMON); // Validate Input
      
			else if (temp >= 1 && temp <= 12) x = 0;
      
			else err_hand(ERR_INVMON);
		}
		
		x			  = 1;
		date_p->month = temp;
		
		/* Change Day */
		while (x) 
		{
			printf("Please Enter the New Day (DD): ");
			err = sys_req(READ, TERMINAL, buff, &buffsize);
      
			if (err < OK) return err;
			
			temp = atoi(buff);
      
			printf("You Input %d/%d/%d\n",date_p->month,temp,date_p->year);
      
			if (temp==0) err_hand(ERR_INVDAY);  //validate input
      
			else if (valid_date(date_p->year,date_p->month,temp)) x = 0;
      
			else err_hand(ERR_INVDAY);
		}
		
		date_p->day = temp;
		err = sys_set_date(date_p);
		
		if (err < OK) return err;
    
		printf("The New Date Is (MM/DD/YYYY): %d/%d/%d",date_p->month,date_p->day,date_p->year);
	}
	
	else printf("Aborted Date Change");
  
	return err;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- valid_date																								   **
 **               Purpose -- The valid_date function verifies that the user-inputted date is a valid date.							   **
 **            Parameters -- int - year, int - month, int - day																		   **
 **			 Return Value -- int - a boolean value 																					   **
 **     Procedures Called -- N/A																									   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The valid_date function verifies that the user-inputted date is a valid date.  It checks for leap years   **
 **							 and checks for a valid number of days in a month.														   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int valid_date(int yr, int mo, int day) 
{
	int leap = 0, valid = 1;
  
	if (((yr % 4 == 0) && (yr % 100 != 0)) || (yr % 400 == 0)) leap = 1;  // Check for Leap Year
  
	if ((mo == 1 || mo == 3 || mo == 5 || mo == 7 || mo == 8 || mo == 10 || mo == 12 || mo == 14) && (day >= 1 && day <= 31)) return valid; // Check 31 Days
  
	else if ((mo == 4 || mo == 6 || mo == 9 || mo == 11 || mo == 13) && (day >= 1 && day <= 30)) return valid; //check 30 Days
  
	else if ((mo == 2 && leap && (day >= 1 && day <= 29)) || (mo == 2 && !leap && (day >= 1 && day <= 28))) return valid; // Check February
  
	else return !valid;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- init_r1																							       **
 **               Purpose -- The init_r1 function initializes the current working directory.										   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int																									   **
 **     Procedures Called -- _getdcwd																								   **
 **  Global Data Accessed -- char wd[]																								   **
 **  Summary of Algorithm -- The init_r1 initializes the current working directory.													   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int init_r1() 
{
	_getdcwd(3,wd,sizeof(wd));
  
	return 0;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- cleanup_r1																							       **
 **               Purpose -- The cleanup_r1 function does nothing.																	   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int																									   **
 **     Procedures Called -- N/A																									   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The cleanup_r1 function does nothing.																	   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int cleanup_r1() 
{
	return 0;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- toLowerCase																						       **
 **               Purpose -- The toLowerCase function converts user input to lowercase.												   **
 **            Parameters -- char [] - a buffer containing user input 																   **
 **			 Return Value -- void																									   **
 **     Procedures Called -- strlen, tolower																						   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The toLowerCase function converts user input to lowercase.												   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
void toLowerCase(char str[BIGBUFF]) 
{
	int i = 0;
  
	for (i; i < strlen(str); i++) str[i] = tolower(str[i]);
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- err_hand																								   **
 **               Purpose -- The err_hand function handles errors generated during JAROS execution.									   **
 **            Parameters -- int - an error code																					   **
 **			 Return Value -- void																									   **
 **     Procedures Called -- printf																									   **
 **  Global Data Accessed -- N/A																									   **
 **  Summary of Algorithm -- The err_hand function handles errors generated during JAROS execution.									   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
void err_hand(int err_code) 
{
	if (err_code == ERR_INVCOM) printf("Invalid Command -- Type \'help\' for Additional Information");
  
	else if (err_code == ERR_INVYR) printf("Invalid Year Parameter -- Enter a Year in the Range of 1 to 9999\n");
  
	else if (err_code == ERR_INVMON) printf("Invalid Month Parameter --  Enter a Month in the Range of 1 to 12\n");
  
	else if (err_code == ERR_INVDAY) printf("Invalid Day Parameter -- Enter a Day in the Range of 1 to 31\n");
  
	else if (err_code == ERR_SUP_INVDEV) printf("Invalid Device ID");
  
	else if (err_code == ERR_SUP_INVOPC) printf("Invalid Op Code");
  
	else if (err_code == ERR_SUP_INVPOS) printf("Invalid Character Position");
  
	else if (err_code == ERR_SUP_RDFAIL) printf("Read Failed");
  
	else if (err_code == ERR_SUP_WRFAIL) printf("Write Failed");
  
	else if (err_code == ERR_SUP_INVMEM) printf("Invalid Memory Block Pointer");
  
	else if (err_code == ERR_SUP_FRFAIL) printf("Free Failed");
  
	else if (err_code == ERR_SUP_INVDAT) printf("Invalid Date");
  
	else if (err_code == ERR_SUP_DATNCH) printf("Date Not Changed");
  
	else if (err_code == ERR_SUP_INVDIR) printf("Invalid Directory Name");
  
	else if (err_code == ERR_SUP_DIROPN) printf("Directory Open Error");
  
	else if (err_code == ERR_SUP_DIRNOP) printf("No Directory Is Open");
  
	else if (err_code == ERR_SUP_NOENTR) printf("No More Directory Entries");
  
	else if (err_code == ERR_SUP_NAMLNG) printf("Name Too Long for Buffer");
  
	else if (err_code == ERR_SUP_DIRCLS) printf("Directory Close Error");
  
	else if (err_code == ERR_SUP_LDFAIL) printf("Program Load Failed");
  
	else if (err_code == ERR_SUP_FILNFD) printf("File Not Found");
  
	else if (err_code == ERR_SUP_FILINV) printf("File Invalid");
  
	else if (err_code == ERR_SUP_PROGSZ) printf("Program Size Error");
  
	else if (err_code == ERR_SUP_LDADDR) printf("Invalid Load Address");
  
	else if (err_code == ERR_SUP_NOMEM) printf("Memory Allocation Error");
  
	else if (err_code == ERR_SUP_MFREE) printf("Memory Free Error");
  
	else if (err_code == ERR_SUP_INVHAN) printf("Invalid Handler Address");
  
	else if (err_code == ERR_PCBNF) printf("PCB Not Found");
  
	else if (err_code == ERR_QUEEMP) printf("Queue Is Empty");
  
	else if (err_code == ERR_PRONTL) printf("Process Name Is Too Long");
  
	else if (err_code == ERR_NAMEAE) printf("Process Name Already in Use");
  
	else if (err_code == ERR_INVCLS) printf("Invalid Process Class");
  
	else printf("Invalid Error Code - %d", err_code);
  
	err = 0;
}
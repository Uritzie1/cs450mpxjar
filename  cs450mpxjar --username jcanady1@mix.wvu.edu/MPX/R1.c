/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **																FILE INFORMATION													   **
 **																																	   **
 **		FILENAME -- COMHAN.c																										   **	
 **      AUTHORS -- Jonroy Canaday, Adam Trainer, Robert Wayland																	   **
 **      VERSION -- 6.0																												   **
 ** LAST UPDATED -- April 30, 2010																									   **
 **																																	   **
 **   COMPONENTS -- cleanup_r1, comhan, date, disp_dir, err_hand, get_Version, help, init_r1, terminate_mpx, toLowerCase, trim,		   **
 **				    valid_date, 																										   **
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
 **		  04/23/2010  AT		   code review; additional documentation added	
 **       04/29/2010  JC, RW       upgraded to R6													   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */

// Included ANSI C Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Included Support Files
#include "mpx_supt.h"
#include "trmdrive.h"
#include "MPXlump.h"
#include <dos.h>


/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- main																									   **
 **               Purpose -- The main function is where the program begins execution.											       **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- sys_init, init_r1, init_r2, init_r3, comhan, cleanup_r1, cleanup_r2, cleanup_r3, terminate_mpx			   **
 **  Global Data Accessed -- int err																								   **
 **  Summary of Algorithm -- The main function initializes the system, installs comhan and idle as processes, and calls dispatcher.    **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int main() {
  struct PCB *np;
  struct context *npc;
  
  sys_init(MODULE_R4);
  err = init_r1();
  err = init_r3();
  err = init_f();
  
  np = allocate_PCB();
  if (np == NULL) err = ERR_UCPCB;
  else {
    err = setup_PCB(np, "COMHAN",SYSTEM,127);
    if (err < OK) err_hand(err);
    npc = (struct context*) np->stack_top;
    npc->IP = FP_OFF(&comhan); 
    npc->CS = FP_SEG(&comhan);
    npc->FLAGS = 0x200;
    npc->DS = _DS;
    npc->ES = _ES;
    err = insert(np,RUNNING);
  }

  err = load_prog("idle", -128, SYSTEM); 
  dispatcher();
  sys_exit();
  return OK;
}

/****************************************************************************************************************************************
 ****************************************************************************************************************************************
 **        Procedure Name -- comhan																									   **
 **               Purpose -- The comhan function handles all JAROS commands.        												   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- printf, memset, sys_req, trim, toLowerCase, strncmp, strlen, terminate_mpx, get_Version, help, err_hand,  **
 **							 date, disp_dir, create_PCB, delete_PCB, block, unblock, suspend, resume, set_Priority, show_PCB,		   **
 **						     show_All, show_Ready, show_Blocked, load_test															   **
 **  Global Data Accessed -- int err, char * fcns[] 																				   **
 **  Summary of Algorithm -- The comhan function processes user-inputted commands and receives error codes returned by functions.	   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int comhan() {
  char cmd[BIGBUFF]={0};
  int bufsize = BIGBUFF;
  printf("\nWelcome to JAROS!\n");
  
  while (1) {
    bufsize = BIGBUFF;
    memset(cmd, '\0', BIGBUFF);                       //clear buffer
    printf("\n%s",alPrompt);                          //command prompt
    err = sys_req(READ, TERMINAL, cmd, &bufsize);     //read in command
    trim(cmd);
    toLowerCase(cmd);
    //R1 Commands
    if (!strncmp(cmd,alfcns[QUIT],strlen(alfcns[QUIT])+1)) terminate_mpx();  //call corresponding function
    else if (!strncmp(cmd,alfcns[VER],strlen(alfcns[VER]))) get_Version();
    else if (!strncmp(cmd,alfcns[HELP],strlen(alfcns[HELP])+1)) {
      err = help(NULL);
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[DATE],strlen(alfcns[DATE])+1)) {
      err = date();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[DIR],strlen(alfcns[DIR])+1)) {
      err = disp_dir();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[CHGPROMPT],strlen(alfcns[CHGPROMPT])+1)) {
      err = changePrompt();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[RSTPROMPT],strlen(alfcns[RSTPROMPT])+1)) {
      resetPrompt();
    }
    //R2 commands
    /*else if (!strncmp(cmd,alfcns[CREATEPCB],strlen(alfcns[CREATEPCB])+1)) {
      err = create_PCB();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[DELPCB],strlen(alfcns[DELPCB])+1)) {
      err = delete_PCB();
      if(err < OK) err_hand(err);
    }*/
    /*else if (!strncmp(cmd,alfcns[BLOCK],strlen(alfcns[BLOCK])+1)) {
      err = block();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[UNBLOCK],strlen(alfcns[UNBLOCK])+1)) {
      err = unblock();
      if(err < OK) err_hand(err);
    }*/
    else if (!strncmp(cmd,alfcns[SUSPEND],strlen(alfcns[SUSPEND])+1)) {
      err = suspend();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[RES],strlen(alfcns[RES])+1)) {
      err = resume();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[SETPRI],strlen(alfcns[SETPRI])+1)) {
      err = set_Priority();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[SHOWPCB],strlen(alfcns[SHOWPCB])+1)) {
      err = show_PCB();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[SHOWALL],strlen(alfcns[SHOWALL])+1)) {
      err = show_All();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[SHOWREADY],strlen(alfcns[SHOWREADY])+1)) {
      err = show_Ready();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[SHOWBLOCKED],strlen(alfcns[SHOWBLOCKED])+1)) {
      err = show_Blocked();
      if(err < OK) err_hand(err);
    }
    //R3 commands
    /*else if (!strncmp(cmd,alfcns[DISPATCH],strlen(alfcns[DISPATCH])+1)) dispatcher();
    else if (!strncmp(cmd,alfcns[LOADPROCS],strlen(alfcns[LOADPROCS])+1)) {
      err = load_test();
      if(err < OK) err_hand(err);
    }*/
    //R4 commands
    else if (!strncmp(cmd,alfcns[LOAD],strlen(alfcns[LOAD])+1)) {
      err = load();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[TERMINATE],strlen(alfcns[TERMINATE])+1)) {
      err = terminate();
      if(err < OK) err_hand(err);
    }
    //end new commands
    else if (!strncmp(cmd,"\n",1)) ;
    else err_hand(ERR_INVCOM);
  }
}

// Allows user to change the command prompt.
int changePrompt() {
    int i = 0;
	int bufsize = 10;
	char cmd[10];
	printf("\nEnter new prompt symbol (max 10 characters): ");
	if ((err = sys_req(READ, TERMINAL, cmd, &bufsize)) < OK) return err;
	strncpy(alPrompt, cmd, 10);
	while(i<=10) {
	  if(alPrompt[i] == '\n') alPrompt[i] = '\0';
	  i++;
    }
}

// Allos user to return the command prompt to the default.
void resetPrompt() {
  strncpy(alPrompt, prompt, 10);
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
int disp_dir() {
  char namebuff[SMALLBUFF];
  long filesize;
  char wdc[BIGBUFF*2] = {0};
  int i;
  memset(namebuff, '\0', SMALLBUFF);
  for(i = 0; i < BIGBUFF * 2; i++) wdc[i] = wd[i];
  strcat(wdc,"\\MPXFILES\\");   //build directory from current working directory
  
  err = sys_open_dir(wdc);
  if(err < OK) return err;
  printf("\nFile Name     Size (bytes)");  //print list of MPX files
  while ((err = sys_get_entry(namebuff, SMALLBUFF, &filesize)) == OK) {
    printf("\n%-9.9s     %ld", namebuff, filesize);
  }
  if(err < OK && err != ERR_SUP_NOENTR) return err;
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
 **     Procedures Called -- memset, printf, sys_req, err_hand, trim, toLowerCase, cleanup_r1, cleanup_r2, cleanup_r3, sys_exit		   **
 **  Global Data Accessed -- int err																								   **
 **  Summary of Algorithm -- The terminate_mpx function prompts the user for confirmation.  If "Y", JAROS terminates execution. 	   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
void terminate_mpx() {
  char buff[BIGBUFF];
  int buffsize = BIGBUFF;
  memset(buff, '\0', BIGBUFF);
  
  printf("Are you sure you want to terminate MPX? (Y/N): ");
  err = sys_req(READ, TERMINAL, buff, &buffsize);
  if (err < OK) {
    err_hand(err);
    return;
  }
  trim(buff);
  toLowerCase(buff);
  if (buff[0] == 'y') {
    err = cleanup_r2();
    err = cleanup_r3();
    err = cleanup_f();
    if (err < OK) err_hand(err);
    sys_exit();
    //sys_req(EXIT, NO_DEV, NULL, 0);
  }
  else printf("Termination cancelled.");
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
 printf("JAROS current version: %f",VERSION);
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
 void trim(char ary[BIGBUFF]) {
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
int help() {
  FILE *fptr = 0;
  int i,j = 0;
  int bufsize = BIGBUFF;
  char buffer[BIGBUFF] = {0};
  char wdc[BIGBUFF*2] = {0};
  char tbuffer[9] = "\\help\\";
  for(i = 0; i < BIGBUFF * 2; i++)
  wdc[i] = wd[i];

  printf("Help: enter original command (or 'list' for command list): ");
  if ((err = sys_req(READ, TERMINAL, buffer, &bufsize)) < OK) return err;
  trim(buffer);
  toLowerCase(buffer);
  for(i = 0; i < strlen(fcns);i++) if(!strncmp(fcns[i],buffer, strlen(fcns[i]))) j++;
  if(j > 0){                //build file path
    strcat(wdc,tbuffer);
    strcat(wdc,buffer);
    strcat(wdc,".txt");
    //printf("%s",wdc);
    if ((fptr = fopen(wdc,"r")) > 0) {
      i = 0;
      while(fgets(buffer,BIGBUFF,fptr)) {
	    printf("%s",buffer);
	    i++;
	    if(i == 24) {        //paging
	      printf("Press enter to continue");
	      err = sys_req(READ, TERMINAL, buffer, &bufsize);
	      i = 0;
	    }
      }
    }
    else {
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
 **               Purpose -- The date function displays the current date and prompts the user as to whether or not he wants to		   **
 **							 change the system date.																				   **
 **            Parameters -- N/A																									   **
 **			 Return Value -- int - an error code																					   **
 **     Procedures Called -- sys_get_date, printf, sys_req, trim, toLowerCase, atoi, err_hand										   **
 **  Global Data Accessed -- int err																								   **
 **  Summary of Algorithm -- The date function processes user input, and displays help information for a specific command.			   **
 ****************************************************************************************************************************************
 ****************************************************************************************************************************************
 */
int date() {
  char buff[BIGBUFF];
  int buffsize = BIGBUFF;
  int x = 1, temp;
  date_rec *date_p;
  
  sys_get_date(date_p);      //show current date and prompt for change
  printf("The current date is (MM/DD/YYYY): %d/%d/%d",date_p->month,date_p->day,date_p->year);
  printf("\nWould you like to change the date (Y/N)? ");
  err = sys_req(READ, TERMINAL, buff, &buffsize);
  if (err < OK) return err;
  trim(buff);
  toLowerCase(buff);
  if (buff[0] == 'y') {
    while (x) {  //change year prompt
      printf("Please enter the new year (YYYY): ");
      err = sys_req(READ, TERMINAL, buff, &buffsize);
      if (err < OK) return err;
      temp = atoi(buff);
      if (temp==0) err_hand(ERR_INVYR);  //validate input
      else if (temp >= 0 && temp < 10000) x = 0;
      else err_hand(ERR_INVYR);
    }
    x = 1;
    date_p->year = temp; 

    while(x) {  //change month prompt
      printf("Please enter the new month (MM): ");
      err = sys_req(READ, TERMINAL, buff, &buffsize);
      if (err < OK) return err;
      temp = atoi(buff);
      if (temp==0) err_hand(ERR_INVMON);  //validate input
      else if (temp >= 1 && temp <= 12) x = 0;
      else err_hand(ERR_INVMON);
    }
    x = 1;
    date_p->month = temp;

    while(x) {  //change day prompt
      printf("Please enter the new day (DD): ");
      err = sys_req(READ, TERMINAL, buff, &buffsize);
      if (err < OK) return err;
      temp = atoi(buff);
      printf("You input %d/%d/%d\n",date_p->month,temp,date_p->year);
      if (temp==0) err_hand(ERR_INVDAY);  //validate input
      else if (valid_date(date_p->year,date_p->month,temp)) x = 0;
      else err_hand(ERR_INVDAY);
    }
    date_p->day = temp;

    err = sys_set_date(date_p);
    if (err < OK) return err;
    printf("The new date is (MM/DD/YYYY): %d/%d/%d",date_p->month,date_p->day,date_p->year);
  }
  else printf("Date change aborted.");
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
int valid_date(int yr, int mo, int day) {
  int leap = 0, valid = 1;
  if (((yr % 4 == 0) && (yr % 100 != 0)) || (yr % 400 == 0)) leap = 1;  //determine if leap year
  if ((mo == 1 || mo == 3 || mo == 5 || mo == 7 || mo == 8 || mo == 10 || mo == 12 || mo == 14) && (day >= 1 && day <= 31)) return valid; //check 31-day months
  else if ((mo == 4 || mo == 6 || mo == 9 || mo == 11 || mo == 13) && (day >= 1 && day <= 30)) return valid; //check 30-day months
  else if ((mo == 2 && leap && (day >= 1 && day <= 29)) || (mo == 2 && !leap && (day >= 1 && day <= 28))) return valid; //check February
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
int init_r1() {
  _getdcwd(3,wd,sizeof(wd));
  resetPrompt();
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
void toLowerCase(char str[BIGBUFF]) {
  int i = 0;
  for(i;i<strlen(str);i++) str[i] = tolower(str[i]);
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
void err_hand(int err_code) {
  if(err_code == ERR_INVCOM) printf("Invalid command. Type \"help\" for more info.");
  else if(err_code == ERR_INVYR) printf("Invalid year parameter.  Please enter a year from 1-9999\n");
  else if(err_code == ERR_INVMON) printf("Invalid month parameter.  Please enter a month from 1-12\n");
  else if(err_code == ERR_INVDAY) printf("Invalid day parameter.  Please enter a day from 1-31 depending on the month.\n");
  else if(err_code == ERR_SUP_INVDEV) printf("Invalid device ID.");
  else if(err_code == ERR_SUP_INVOPC) printf("Invalid op code.");
  else if(err_code == ERR_SUP_INVPOS) printf("Invalid character position.");
  else if(err_code == ERR_SUP_RDFAIL) printf("Read failed.");
  else if(err_code == ERR_SUP_WRFAIL) printf("Write failed.");
  else if(err_code == ERR_SUP_INVMEM) printf("Invalid memory block pointer.");
  else if(err_code == ERR_SUP_FRFAIL) printf("Free failed.");
  else if(err_code == ERR_SUP_INVDAT) printf("Invalid date.");
  else if(err_code == ERR_SUP_DATNCH) printf("Date not changed.");
  else if(err_code == ERR_SUP_INVDIR) printf("Invalid directory name.");
  else if(err_code == ERR_SUP_DIROPN) printf("Directory open error.");
  else if(err_code == ERR_SUP_DIRNOP) printf("No directory is open.");
  else if(err_code == ERR_SUP_NOENTR) printf("No more directory entries.");
  else if(err_code == ERR_SUP_NAMLNG) printf("Name too long for buffer.");
  else if(err_code == ERR_SUP_DIRCLS) printf("Directory close error.");
  else if(err_code == ERR_SUP_LDFAIL) printf("Program load failed.");
  else if(err_code == ERR_SUP_FILNFD) printf("File not found.");
  else if(err_code == ERR_SUP_FILINV) printf("File invalid.");
  else if(err_code == ERR_SUP_PROGSZ) printf("Program size error.");
  else if(err_code == ERR_SUP_LDADDR) printf("Invalid load address.");
  else if(err_code == ERR_SUP_NOMEM) printf("Memory allocation error.");
  else if(err_code == ERR_SUP_MFREE) printf("Memory free error.");
  else if(err_code == ERR_SUP_INVHAN) printf("Invalid handler address.");
  else if(err_code == ERR_PCBNF) printf("PCB not found.");
  else if(err_code == ERR_QUEEMP) printf("Queue is empty.");
  else if(err_code == ERR_PRONTL) printf("Process name is too long.");
  else if(err_code == ERR_NAMEAE) printf("Process name already in use.");
  else if(err_code == ERR_UTDSC) printf("Unable to delete system class.");
  else if(err_code == ERR_INVCLS) printf("Invalid process class.");
  else if(err_code == ERR_CNESP) printf("Cannot edit system process.");
  else printf("Invalid error code %d", err_code);
  err = 0; errx=0; err3=0;
}

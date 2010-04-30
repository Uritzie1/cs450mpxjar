/**
 * File Name: comhan.c
 * \title JAROS
 * \author Jonroy Canady
 * \author Adam Trainer
 * \author Rob Wayland   
 * \date 2/04/2010
 * \version: 2.0
 *
 * Components: err_hand, init_r1, cleanup_r1, disp_dir, comhan, terminate_mpx,
 *             help, get_Version, date, valid_date, toLowerCase, trim
 *
 *******************************************************************************
 * Change Log:
 *
 *        1/25/2010  JC           Original version: outline, nonfunctional
 *        1/28/2010  JC, RW       Slight comhan fcn editing
 *        2/01/2010  JC, RW       Solved initial errors such that it compiles
 *        2/02/2010  JC, RW       Added version and partial help functions; improved comhan
 *        2/03/2010  JC           Completed dir, date, and err_hand functions and comhan; R1 operational minus help function
 *        2/04/2010  JC, RW, AT   Completed help; finished commentation; R1 fully operational with many improvements to come
 *        2/08/2010  JC           Edited dir function for operation on any computer and added some comments     
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

// Global Variables
int err = 0;  //error code
char * fcns[NUMFCNS] = {"date\0","help\0","ver\0","dir\0","quit\0","list\0","cpcb\0","dpcb\0","block\0","unblock\0","suspend\0","resume\0","setpri\0","shpcb\0","shall\0","shready\0","shblock\0","dispat\0","ldprocs\0","load\0","term\0","chgprom\0","rstprom\0","alias\0","rsalias\0","rdhist\0","clrhist\0",NULL};  //functions list
char * alfcns[NUMFCNS] = {0};
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
  struct PCB *np;
  struct context *npc;
  
  sys_init(MODULE_R4);
  err = init_r1();
  err = init_r2();
  err = init_r3();
  err = init_f();
  
  np = allocate_PCB();
  if (np == NULL) err = ERR_UCPCB;
  else {
    err = setup_PCB(np, "COMHAN",SYSTEM,127);
    if (err < OK) err_hand(err);
   	//sys_free_mem(np->stack_base);
	//np->stack_base = (unsigned char*)sys_alloc_mem(COMHAN_STACK_SIZE*sizeof(unsigned char));
	//np->stack_top = np->stack_base + COMHAN_STACK_SIZE - sizeof(struct context);
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

/** Procedure Name: comhan
 * \param none
 * \return an integer that is 0 if successful (which it always is)
 * Procedures Called: memset, sys_req, trim, strncmp, help, err_hand, date,
 *   disp_dir, terminate_mpx
 * Globals Used: 
 * @var err
   @var fcns
 * \details Description/Purpose: comhan is the heart of the R1 module. It runs the
 *  central loop of the command handler.  It accepts commands entered by the
 *  user, analyzes them, and calls the proper functions.  It also receives all
 *  error codes returned by the functions, passing them to the error handler.
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
    writeHistory(cmd);
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
    else if (!strncmp(cmd,alfcns[ALIAS],strlen(alfcns[ALIAS])+1)) {
      err = alias();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[RSTALIAS],strlen(alfcns[RSTALIAS])+1)) {
      resetAlias();
    }
    else if (!strncmp(cmd,alfcns[RDHIST],strlen(alfcns[RDHIST])+1)) {
      err = readHistory();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[CLRHIST],strlen(alfcns[CLRHIST])+1)) {
      err = clearHistory();
      if(err < OK) err_hand(err);
    }
    //R2 commands
    else if (!strncmp(cmd,alfcns[CREATEPCB],strlen(alfcns[CREATEPCB])+1)) {
      err = create_PCB();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[DELPCB],strlen(alfcns[DELPCB])+1)) {
      err = delete_PCB();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[BLOCK],strlen(alfcns[BLOCK])+1)) {
      err = block();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,alfcns[UNBLOCK],strlen(alfcns[UNBLOCK])+1)) {
      err = unblock();
      if(err < OK) err_hand(err);
    }
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
    else if (!strncmp(cmd,alfcns[DISPATCH],strlen(alfcns[DISPATCH])+1)) dispatcher();
    else if (!strncmp(cmd,alfcns[LOADPROCS],strlen(alfcns[LOADPROCS])+1)) {
      err = load_test();
      if(err < OK) err_hand(err);
    }
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
void resetPrompt() {
  strncpy(alPrompt, prompt, 10);
}

int alias() {
	int bufsize = BIGBUFF;
	int i = 0;
	char cmd[BIGBUFF] = {0};
	char ncmd[BIGBUFF] = {0};

	printf("\nEnter the command to be aliased:  ");
	if ((err = sys_req(READ, TERMINAL, cmd, &bufsize)) < OK) return err;
    trim(cmd);
    toLowerCase(cmd);
	for(i;i<NUMFCNS-2;i++) {
		if(!strncmp(cmd,alfcns[i],strlen(alfcns[i]+1))) {
			printf("\nEnter the command's new name:  ");
			if ((err = sys_req(READ, TERMINAL, ncmd, &bufsize)) < OK) return err;
			trim(ncmd);
            toLowerCase(ncmd);
			alfcns[i] = ncmd;
		}
	}
	if(i = NUMFCNS) return ERR_INVCOM;
	printf("\nChange successful!");
	return OK;

}
void resetAlias() {
	int i = 0;
	for(i;i < NUMFCNS;i++) alfcns[i] = fcns[i];
}
//need global FILE *tmpFP
int openTmp() {
 char tmpname[L_tmpnam];
 char *filename = NULL;
 filename = tmpnam(tmpname);
 tmpfp = tmpfile();
}
int closeTmp() {
	close(tmpfp);
	return OK;
}
int writeHistory(char *command) {
	fprintf(tmpfp,command);
}
int readHistory() {
    int buffsize = BIGBUFF;
    int i = 0;
	char buffer[BIGBUFF] = {0};
	while (fgets(buffer, BIGBUFF, tmpfp)) {
		printf("\n%s",buffer);
		i++;		
		if (i == 24) {// Paging Functionality 
			printf("\nPress Any Key to Continue");
			err = sys_req(READ, TERMINAL, buffer, &buffsize);
			i = 0;
		}
	}
}
int clearHistory() {
	closeTmp();
	tmpfp = NULL;
	openTmp();
}

/** Procedure Name: disp_dir
 * \param none
 * \return err an integer error code
 * Procedures Called: memset, sys_open_dir, sys_get_entry, sys_close_dir
 * Globals Used: 
 * @var err
 * \details Description/Purpose: disp_dir neatly prints a list of .mpx files found in
 *   the MPXFILES folder as well as their sizes in bytes.
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

/** Procedure Name: terminate_mpx
 * \Param none
 * \return none
 * Procedures Called: memset, sys_req, err_hand, cleanup_r1, sys_exit
 * Globals Used: 
 * @var err
 * \details Description/Purpose: confirms that the user really wishes to terminate MPX.
 *   If yes, it cleans up and exits. If no, it tells the user such and returns.
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
    err = cleanup_r1();
    err = cleanup_r2();
    err = cleanup_r3();
    err = cleanup_f();
    if (err < OK) err_hand(err);
    sys_exit();
    //sys_req(EXIT, NO_DEV, NULL, 0);
  }
  else printf("Termination cancelled.");
}

/** Procedure Name: get_Version
 * \param none
 * \return none
 * Procedures Called: printf
 * Globals Used: none
 * \brief Description/Purpose: simply prints a single line with the version constant
 */
void get_Version()
{
 printf("JAROS current version: %f",VERSION);
}

/** Procedure Name: trim
 * \praram ary is a character array that holds the string to be trimmed
 * \return none
 * Procedures Called: isspace
 * Globals Used: none
 * \details Description/Purpose: trims all white space AND newlines from the entirety of
 *   the string passed in.
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

/** Procedure Name: help
 * \param none
 * \return err an integer error code
 * Procedures Called: sys_req, err_hand, trim, strcat, fopen, fgets, fclose
 * Globals Used: 
 * @var fcns
 * @var err
 * \details Description/Purpose: displays a list of available functions and a short
 *   description of each.  It then asks the user to input a function name if
 *   s/he wants a more detailed description.
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

  printf("Help: enter command (or list for command list): ");
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
	      printf("Press any key to continue");
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

/** Procedure Name: date
 * \param: none
 * \return err an integer error code
 * Procedures Called: sys_get_date, sys_req, atoi, err_hand, valid_date, sys_set_date
 * Globals Used: 
 * @var err
 *\details Description/Purpose: displays the current date in MM/DD/YYYY format. It then
 *   asks the user if s/he wants to set a new date. If so, it asks for a new
 *   year, then a new month, and lastly a new day. Determines date validity.
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

/** Procedure Name: valid_date
 * \param yr int representing the year
 * \param mo int reresenting a month of the year
 * \param day int representing a day of the month
 *   to be validated
 * \return int an integer that's 1 if the date is valid, 0 if not
 * Procedures Called: none
 * Globals Used: none
 * \details Description/Purpose: validates the date passed in, checking that the day is
 *   within the range allowed by the month. Accounts for leap years.
 */
int valid_date(int yr, int mo, int day) {
  int leap = 0, valid = 1;
  if (((yr % 4 == 0) && (yr % 100 != 0)) || (yr % 400 == 0)) leap = 1;  //determine if leap year
  if ((mo == 1 || mo == 3 || mo == 5 || mo == 7 || mo == 8 || mo == 10 || mo == 12 || mo == 14) && (day >= 1 && day <= 31)) return valid; //check 31-day months
  else if ((mo == 4 || mo == 6 || mo == 9 || mo == 11 || mo == 13) && (day >= 1 && day <= 30)) return valid; //check 30-day months
  else if ((mo == 2 && leap && (day >= 1 && day <= 29)) || (mo == 2 && !leap && (day >= 1 && day <= 28))) return valid; //check February
  else return !valid;
}

/** Procedure Name: init_r1
 * \param none
 * \return err an integer error code (0 for now)
 * Procedures Called: _getdcwd
 * Globals Used: 
 * @var wd
 * \brief Description/Purpose: finds the working directory and writes it to the global
 */
int init_r1() {
  _getdcwd(3,wd,sizeof(wd));
  resetAlias();
  resetPrompt();
  openTmp();
  return 0;
}

/** Procedure Name: init_r1
 * \param none
 * \return  an integer error code (0 for now)
 * Procedures Called: none
 * Globals Used: none
 * \brief Description/Purpose: none for now
 */
int cleanup_r1() {
  closeTmp();
  return 0;
}

/** Procedure Name: toLowerCase
 * \param[in] str character array called str that is to be lowered in case
 * \return none
 * Procedures Called: tolower
 * Globals Used: none
 * \brief Description/Purpose: converts the input string to all lowercase
 */
void toLowerCase(char str[BIGBUFF]) {
  int i = 0;
  for(i;i<strlen(str);i++) str[i] = tolower(str[i]);
}

/** Procedure Name: err_hand
 * \param[in] err an integer err_code that corresponds to a textual error
 * \return none
 * Procedures Called: printf
 * Globals Used: none
 * \brief Description/Purpose: prints out an error message based on the error code passed in
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
  else printf("Invalid error code %d", err_code);
  err = 0; errx=0; err3=0;
}

int init_r2() {
    return 0;
}

/** Procedure Name: init_r2
* \param none
* \return  an integer error code (0 for now)
* Procedures Called: none
* Globals Used: none
* \breef Description/Purpose: none for now
*/
int cleanup_r2() {
    struct PCB *temppcb = tail1;
    while (temppcb != NULL) {
      tail1 = temppcb;
      temppcb = tail1->next;
      free_PCB(tail1);
    }
    temppcb = tail2;
    while (temppcb != NULL) {
      tail2 = temppcb;
      temppcb = tail2->next;
      free_PCB(tail2);
    }
    return 0;
}

/**  Procedure Name: block
* \param none
* \return an integer error code 
* Procedures Called: trim, toLowerCase, findPCB, qRemove, insertPCB, sys_req
* Globals Used: err
* \brief Description: moves a PCB from ready to blocked queue 
*/
int block() {  //temp command
	char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter the name of the process to be blocked: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trimx(buff);
	toLowerCasex(buff);
	temppcb = findPCB(buff, temppcb);
	if (errx < OK) return errx;
	if(temppcb->state != BLOCKED) {
	  temppcb = qRemove(buff, temppcb);
	  temppcb->state = BLOCKED;
	  insert(temppcb, BLOCKED);
	  if(temppcb!=NULL) printf("\nPCB successfully blocked!");
	}
	return errx;
}

/**  Procedure Name: unblock
* \param none
* \return an integer error code 
* Procedures Called: findPCB, insertPCB, removePCB, toLowerCase, trim
* Globals Used: err
* \brief Description: moves a PCB from blocked to ready queue 
*/
int unblock() {
    char buff[BIGBUFF];
    int buffsize = BIGBUFF;
    struct PCB *temppcb = NULL;
    memset(buff, '\0', BIGBUFF);
    
    errx = 0;
    printf("Please enter the name of the process to be unblocked: ");
    errx = sys_req(READ, TERMINAL, buff, &buffsize);
    if (errx < OK) return errx;
    trimx(buff);
    toLowerCasex(buff);
    temppcb = findPCB(buff, temppcb);
    if(temppcb->state == BLOCKED) {
	  temppcb = qRemove(buff, temppcb);
      temppcb->state = READY;
	  insert(temppcb, READY+1);
	  if(temppcb!=NULL) printf("\nPCB successfully unblocked!");
    }
    return errx;
}

/**  Procedure Name: suspend
* \param none
* \return an integer error code
* Procedures Called: findPCB, sys_req, trim, toLowerCase
* Globals Used: err
* \brief Description: suspends a PCB
*/
int suspend() {
    char buff[BIGBUFF];
    int buffsize = BIGBUFF;
    struct PCB *temppcb = NULL;
    memset(buff, '\0', BIGBUFF);
    
    errx = 0;
    printf("Please enter the name of the process to be suspended: ");
    errx = sys_req(READ, TERMINAL, buff, &buffsize);
    if (errx < OK) return errx;
    trimx(buff);
    toLowerCasex(buff);
    temppcb = findPCB(buff, temppcb);
    if (errx < OK) return errx;
    if(temppcb->state != SUSP) temppcb->suspended = SUSP;
    if(temppcb!=NULL) printf("\nPCB successfully suspended!");
    return errx;
}

/**  Procedure Name: resume
* \param none
* \return an integer error code
* Procedures Called: sys_req, trim, toLowerCase, findPCB
* Globals Used: err
* \brief Description: sets PCB to not suspended 
*/
int resume() {
	char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter the name of the process to be resumed: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trimx(buff);
	toLowerCasex(buff);
	temppcb = findPCB(buff, temppcb);
	temppcb->suspended = NOTSUSP;
	if(temppcb!=NULL) printf("\nPCB successfully resumed!");
	return errx;
}

/**  Procedure Name: set_Priority
* \param none
* \return an integer error code
* Procedures Called: sys_req, trim, toLowerCase, findPCB, inset, qremove
* Globals Used: err
* \brief Description: sets the priority level for a specified PCB 
*/
int set_Priority() {
	char buff[BIGBUFF];
	int buffsize = BIGBUFF, temp;
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter the name of the process to be reprioritized: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trimx(buff);
	toLowerCasex(buff);
	temppcb = findPCB(buff, temppcb);
	if (errx < OK) return errx;
	printf("Please enter the new priority level where 127 is the highest(-128 to 127): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	temp = atoi(buff);
	temppcb = qRemove(temppcb->name, temppcb);	
	if(temp<=127 && temp>=-128) temppcb->priority = temp;
	else {
      temppcb->priority = 0;
      printf("\nInvalid priority level.  Priority defaulted to 0.");
    }
	if (temppcb->state == BLOCKED) insert(temppcb, BLOCKED);
	else insert(temppcb, READY+1);
	printf("\nPriority for %s successfully set to %d",temppcb->name,temppcb->priority);
	return errx;
}

/**  Procedure Name: show_PCB
* \param none
* \return an integer error code
* Procedures Called: sys_req, trim, toLowerCase, findPCB
* Globals Used: err
* \brief Description: prints a specific PCB 
*/
int show_PCB() {
	char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter a process name: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trimx(buff);
	toLowerCasex(buff);
	temppcb = findPCB(buff,temppcb);
	if (errx < OK) return errx;
	printf("\nPROCESS PROPERTIES\n------------------------");
	printf("\nName: %s", temppcb->name);
	if(temppcb->proc_class == SYSTEM) printf("\nClass: System");
	else printf("\nClass: Application");
	printf("\nPriority: %d", temppcb->priority);
	if(temppcb->state == READY) printf("\nState: Ready");
	else if(temppcb->state == RUNNING) printf("\nState: Running"); 
	else printf("\nState: Blocked");
	if(temppcb->suspended == SUSP) printf("\nSuspended?: Yes");
	else printf("\nSuspended?: No");
	return errx;
}

/**  Procedure Name: show_all
* \param none
* \return an integer error code
* Procedures Called: sys_req
* Globals Used: err
* \brief Description: prints all PCB
*/
int show_All() {
    struct PCB *temppcb = NULL;
    int bufsize = BIGBUFF;
    int i = 2, x = 0;
    char buffer[BIGBUFF] = {0};
	temppcb = head1;	
	errx = 0;
	printf("\nPROCESS PROPERTIES------------------------");
	if(cop != NULL) printf("\nCOP: %-8s\n",cop->name);
	else printf("\nNo COP\n");
	for (x;x<=1;x++) {
	  while(temppcb != NULL) {
	printf("\nName: %-8s", temppcb->name);
	printf("    Priority: %-3d",temppcb->priority);
	    if(temppcb->state == READY) printf("    State: %-7s","Ready");
	    else if(temppcb->state == RUNNING) printf("    State: %-7s","Running");
		else printf("    State: %-7s","Blocked");
		if(temppcb->suspended == SUSP) printf("    Suspended?: Yes\n");
		else printf("    Suspended?: No\n");
		temppcb = temppcb->prev;
		i=i++;
		if(i > 23) {        //paging
	      printf("Press any key to continue");
	      errx = sys_req(READ, TERMINAL, buffer, &bufsize);
	      i = 0;
	    }
      }
      temppcb = tail2;
	}
	return errx;
}

/**  Procedure Name: show_Ready
* \param none
* \return an integer error code
* Procedures Called: sys_req
* Globals Used: err
* \brief Description: prints all PCB in ready queue
*/
int show_Ready() {
	struct PCB *temppcb = NULL;
    int bufsize = BIGBUFF;
    int i = 2;
    char buffer[BIGBUFF] = {0};
	temppcb = tail1;
	
	errx = 0;
	printf("\nPROCESS PROPERTIES------------------------");
	  while(temppcb != NULL) {
        printf("\n\nName: %s", temppcb->name);
	    printf("\nPriority: %d",temppcb->priority);
		if(temppcb->suspended == SUSP) printf("\nSuspended?: Yes");
		else printf("\nSuspended?: No\n");
		temppcb = temppcb->next;
		i=i+4;
		if(i > 20) {        //paging
	      printf("Press any key to continue");
	      errx = sys_req(READ, TERMINAL, buffer, &bufsize);
	      i = 0;
	    }
      }
	return errx;
}

/**  Procedure Name: delete
* \param none
* \return an integer error code
* Procedures Called: sys_req, trim, remove, free_PCB
* Globals Used: err
* \brief Description: moves a PCB from ready to blocked queue 
*/
int delete_PCB() { //temp function	
	char buff[BIGBUFF];
	struct PCB *tmp = NULL;
	int buffsize = BIGBUFF;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
	printf("Please enter the name of the Process to delete: ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);	
	if(errx >= OK) {
	//	if(tmp->proc_class != SYSTEM){
		trimx(buff);
		tmp = qRemove(buff,tmp);
		free_PCB(tmp);
		printf("PCB successfully deleted!");
	//	}
		//else
		//	return ERR_UTDSC;
	}
	return errx;
}

/**  Procedure Name: show_Blocked
* \param none
* \return an integer error code (o for now)
* Procedures Called: sys_req, trim, toLowerCase, findPCB
* Globals Used: err
* \brief Description: moves a PCB from ready to blocked queue 
*/
int show_Blocked() {
	struct PCB *temppcb = NULL;
    int bufsize = BIGBUFF;
    int i = 2;
    char buffer[BIGBUFF] = {0};
	temppcb = tail2;
	
	errx = 0;
	printf("\nPROCESS PROPERTIES------------------------");
	  while(temppcb != NULL) {
        printf("\n\nName: %s", temppcb->name);
		if(temppcb->suspended == SUSP) printf("\nSuspended?: Yes");
		else printf("\nSuspended?: No\n");
		temppcb = temppcb->next;
		i=i+4;
		if(i > 20) {        //paging
	      printf("Press any key to continue");
	      errx = sys_req(READ, TERMINAL, buffer, &bufsize);
	      i = 0;
	    }
      }
	return errx;
}

/** Procedure Name: create_PCB
* \param none
* \return an integer error code
* Procedures Called: sys_req, trim, toLowerCase, setup_PCB, allocate_PCB
* Globals Used: err
* \brief Description: moves a PCB from ready to blocked queue 
*/   
int create_PCB() { //temp fcn
    char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	char name[PROCESS_NAME_LENGTH];
	int proc_class, priority;
	struct PCB *temppcb = NULL;
	struct PCB *newPCBptr = NULL;
	memset(buff, '\0', BIGBUFF);

    errx = 0;
    printf("Please enter the name of the process to be created (9 character limit): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trimx(buff);
	toLowerCase(buff);
    if (strlen(buff)>9) return ERR_PRONTL;
    temppcb = findPCB(buff,temppcb);
	if (temppcb != NULL) return ERR_NAMEAE;
    strncpy(name,buff,PROCESS_NAME_LENGTH);
    
    printf("Please enter the class of the process to be created\n('0' = Application, '1' = System): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trimx(buff);
    if (strncmp(buff,"0\0",2) && strncmp(buff,"1\0",2)) return ERR_INVCLS;
    proc_class = atoi(buff);
    
    printf("Please enter the priority of the process to be created where 127 is high\n(-128 to 127): ");
	errx = sys_req(READ, TERMINAL, buff, &buffsize);
	if (errx < OK) return errx;
	trimx(buff);
    priority = atoi(buff);
	newPCBptr = allocate_PCB();
	if (newPCBptr == NULL) errx = ERR_UCPCB;
    else {
	  errx = setup_PCB(newPCBptr, name, proc_class, priority);
	  if (errx < OK) return errx;
	  errx = insert(newPCBptr,READY+1);
	  printf("\nPCB successfully created!");
	}
	return errx;
	
    	
	//if(temp<=127 && temp>=-128) temppcb->priority = temp;
	//else {
    //  temppcb->priority = 0;
    //  printf("\nInvalid priority level.  Priority defaulted to 0.");
    //}
	//printf("\nPriority for %s successfully set to %d",temppcb->name,temppcb->priority);
}

/**  Procedure Name: allocate_PCB 
* \param none
* \return ptr to the new PCB
* Procedures Called: sys_alloc_mem
* Globals Used: err
* \brief Description: allocates memory for a PCB 
*/
struct PCB * allocate_PCB() {
	struct PCB *newPCBptr = NULL;
	newPCBptr = sys_alloc_mem((sizeof(struct PCB)));
	newPCBptr->stack_base = (unsigned char *)sys_alloc_mem(STACK_SIZE * sizeof(unsigned char));
	newPCBptr->stack_top = newPCBptr->stack_base + STACK_SIZE-sizeof(struct context);
	//newPCBptr = malloc(sizeof(struct PCB));
	return newPCBptr;
}

/**  Procedure Name: free_PCB
* \param *PCBptr pointer to a PCB struct
* \return an integer error code (o for now)
* Procedures Called: sys_free_mem
* Globals Used: err
* \brief Description: frees PCB from memory
*/
int free_PCB(struct PCB *PCBptr) {
    errx = 0;
	errx=sys_free_mem(PCBptr -> stack_base);
	errx=sys_free_mem(PCBptr -> load_address);
	errx=sys_free_mem(PCBptr -> execution_address);
	errx=sys_free_mem(PCBptr);
	//free(PCBptr);
	return errx;
}

/**  Procedure Name: setup_PCB
* \param *PCBptr pointer to PCB
* \param name : char array with PCB name
* \param proc_class : class of the PCB
* \param priority : int that set priority
* \return an integer error code (0 for now)
* Procedures Called: sys_req, trim, toLowerCase, findPCB
* Globals Used: err
* \brief Description: sets the contents of a PCB
*/
int setup_PCB(struct PCB *PCBptr, char name[PROCESS_NAME_LENGTH], int proc_class, int priority) {
    errx = 0;
	strncpy((PCBptr->name), name,PROCESS_NAME_LENGTH);
	(PCBptr->proc_class) = proc_class;
	(PCBptr->priority) = priority;
	(PCBptr->state) = READY;
	PCBptr->suspended = NOTSUSP;
	//(PCBptr->stack_base) = (PCBptr->stack)[STACK_SIZE] ;
	//(PCBptr->stack_top) = (PCBptr->stack)[STACK_SIZE + 1];
	//(PCBptr->mem_size) = ;
	//(PCBptr->load_address) = ;
	//(PCBptr->execution_address) = ;
	(PCBptr->prev) = NULL;
	(PCBptr->next) = NULL;
	return errx;
}

/**  Procedure Name: isEmpty
* \param q :int representing the queue
* \return an integer error code (o for now)
* Procedures Called: none
* Globals Used: err
* \brief Description: checks if a queue is empty 
*/
int isEmpty(int q) {
    int ret = 0;
    if(q == 1) {if(head1 == NULL && tail1 == NULL) ret = 1;}
    else if(head2 == NULL && tail2 == NULL) ret = 1;
    return ret;
}

/**  Procedure Name: insert
* \param newPCB: pointer to a PCB
* \param q : int the specifies a queue
* \return an integer error code (o for now)
* Procedures Called: isEmpty
* Globals Used: err
* \brief Description: inserts a PCB into the queue 
*/
int insert(struct PCB *newPCB,int q) {
    struct PCB *tmp = NULL;
    errx = 0;
    if(q == READY+1) {  //For Ready Queue
      if(isEmpty(q)) {
        tail1 = newPCB;
        head1 = tail1;
      }
      else {
  	    tmp = tail1;
	    while((newPCB->priority) > (tmp->priority)) {
          if (tmp == head1) {
		    (tmp->next) = newPCB;
		    (newPCB->prev) = tmp;
		    head1 = newPCB;
	      }
		  else tmp = (tmp->next);
        }
        if(tmp==tail1){ //if inserted at tail
          tail1->prev = newPCB;
          newPCB->next = tail1;
          tail1 = newPCB;
        }
        else if(head1 != newPCB) {
          ((tmp->prev)->next) = newPCB;
          (newPCB->prev) = (tmp->prev);
		  (tmp->prev) = newPCB;
		  (newPCB->next)= tmp;
        }
      }
    }
    else {  //For Blocked Queue
      if(isEmpty(q)) {
        tail2 = newPCB;
        head2 = tail2;
      }
      else {
        tmp = tail2;
        while((newPCB->priority) > (tmp->priority)) {
          if (tmp == head2) {
		    (tmp->next) = newPCB;
		    (newPCB->prev) = tmp;
		    head2 = newPCB;
		  }
		  else tmp = (tmp->next);
        }
        if(tmp==tail2){ //if inserted at tail
          tail2->prev = newPCB;
          newPCB->next = tail2;
          tail2 = newPCB;
        }
        else if(head2 != newPCB) {
	      ((tmp->prev)->next) = newPCB;
		  (newPCB->prev) = (tmp->prev);
		  (tmp->prev) = newPCB;
		  (newPCB->next)= tmp;
        }
      }
    }
    return errx;
}

/**  Procedure Name: findPCB
* \param name : string containing name
* \param PCBptr: a pointer to a PCB struct
* \return an integer error code
* Procedures Called: none
* Globals Used: err
* \brief Description: find a PCB pointer given a name
*/
struct PCB* findPCB(char *name,struct PCB *PCBptr) {
    struct PCB *tmp = tail1;
    errx = 0;
    while((tmp != NULL) && strncmp((tmp->name),name,PROCESS_NAME_LENGTH)) tmp = (tmp->next);
    PCBptr = tmp;
    if (PCBptr == NULL) { //if not found yet, search queue2
      tmp = tail2;
      while((tmp != NULL) && strncmp((tmp->name),name,PROCESS_NAME_LENGTH)) tmp = (tmp->next);
      if(tmp == NULL) errx = ERR_PCBNF; //PCB not found
      else if(tmp != NULL) PCBptr = tmp;
    }
    return PCBptr;
}

/**  Procedure Name: qremove
* \param name: string containing PCB name
* \param set: a pointer to a PCB
* \return an integer error code (0 for now)
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

/** Procedure Name: init_r3
*/
int init_r3() {
	ss_save = NULL;
	sp_save = NULL;
	new_ss = NULL;
	new_sp = NULL;
	cop = NULL;
	tempnode = NULL;
	err3 = sys_set_vec(sys_call);
	if(err3<OK) err_hand(err3);
    return 0;
}

/** Procedure Name: init_r3
*/
int cleanup_r3() {
    return 0;
}

/**
  * \brief Assigns the next ready, non-suspended process to be the current operating process.
  * Searches ready queue for the next non-suspended process. When found, removes the process from the queue.
  * <BR>Then, assigns to the COP and changes state to running. Finally, performs a context switch and completes.
  */
void interrupt dispatcher() {
	if(sp_save == NULL) {
		ss_save = _SS;
		sp_save = _SP;
	}
	tempnode = getRHead();	
	while(tempnode != NULL) { //look for a non-suspended process
		if(tempnode->suspended == NOTSUSP) break;
		tempnode = tempnode -> prev;
	}
	if(tempnode != NULL) { //found a ready, non-suspended process
		cop = qRemove(tempnode->name,cop);
		//reset tempnode so it doesn't interfere with other interrupt/function calls
		tempnode = NULL;
		cop->state = RUNNING;
		new_ss = FP_SEG(cop -> stack_base);
		new_sp = FP_OFF(cop -> stack_top);
		_SS = new_ss;
		_SP = new_sp;
	}
	else { //no ready, unsuspended processes; restore state
		cop = NULL;
		tempnode = NULL;
		_SS = ss_save;
		_SP = sp_save;
		ss_save = NULL;
		sp_save = NULL;
	}
}

/**
 */
void interrupt sys_call() {	
	ss_save_temp = _SS;
	sp_save_temp = _SP;
    
    cop->stack_top = (unsigned char *)MK_FP(ss_save_temp, sp_save_temp);
    /*new_ss = FP_SEG(sys_stack);
	new_sp = FP_OFF(sys_stack)+SYS_STACK_SIZE;
    _SS = new_ss;
	_SP = new_sp;*/
	param_p = (params *)(cop -> stack_top + sizeof(struct context));
	
	
    trm_getc();
    //check for comport request completion
	if(*(comport->event_flag) == 1 && comport->active != NULL) {
        *(comport->event_flag) = 0;
        tmpIOD = dequeue(comport);
        tempnode = qRemove((tmpIOD->requestor)->name, tempnode);
        tempnode->state = READY;
	    insert(tempnode, READY+1);	    
        //process nxt IO req for this dev
        if(comport->count > 0) process_com();
	}
	//check for terminal request completion
	if(*(terminal->event_flag) == 1) {
		*(terminal->event_flag) = 0;
		tmpIOD = dequeue(terminal);
        tempnode = qRemove((tmpIOD->requestor)->name, tempnode);
        tempnode->state = READY;
	    insert(tempnode, READY+1);	    
        //process nxt IO req for this dev
        if(terminal->count > 0) process_trm();
	}
	
	
	if(param_p->op_code == IDLE) {
        cop->state = READY;
		insert(cop,READY+1);
		cop = NULL;
	}
	else if(param_p->op_code == EXIT) {
		free_pcb(cop);
		cop = NULL;
	}
	else if(param_p->op_code == READ || param_p->op_code == WRITE || param_p->op_code == CLEAR || param_p->op_code == GOTOXY) {
      IOschedule();
    } 
	else context_p->AX = param_p->op_code;

	dispatcher();
}

/**
 */
int load_test() {	
	struct PCB *np;
	struct context *npc;

	findPCB("test1",np);
	if(errx == ERR_PCBNF){
		np = allocate_PCB();
		if (np == NULL) err3 = ERR_UCPCB;
		else {
			err3 = setup_PCB(np, "test1",APP,0);
			if (err3 < OK) return err3;
			npc = (struct context*) np->stack_top;
			npc->IP = FP_OFF(&test1_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test1_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			err3 = insert(np,READY+1);
		}
	}
	else printf("\nProcess with name 'test1' already exists.");

	findPCB("test2",np);
	if(errx == ERR_PCBNF){
		np = allocate_PCB();
		if (np == NULL) err3 = ERR_UCPCB;
		else {
			err3 = setup_PCB(np, "test2",APP,0);
			if (err3 < OK) return err3;
			npc = (struct context*) np->stack_top;
			npc->IP = FP_OFF(&test2_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test2_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			err3 = insert(np,READY+1);
		}
	}
	else printf("\nProcess with name 'test2' already exists.");

	findPCB("test3",np);
	if(errx == ERR_PCBNF){
		np = allocate_PCB();
		if (np == NULL) err3 = ERR_UCPCB;
		else {
			err3 = setup_PCB(np, "test3",APP,0);
			if (err3 < OK) return err3;
			npc = (struct context*) np->stack_top;
			npc->IP = FP_OFF(&test3_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test3_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			err3 = insert(np,READY+1);
		}
	}
	else printf("\nProcess with name 'test3' already exists.");

	findPCB("test4",np);
	if(errx == ERR_PCBNF){
		np = allocate_PCB();
		if (np == NULL) err3 = ERR_UCPCB;
		else {
			err3 = setup_PCB(np, "test4",APP,0);
			if (err3 < OK) return err3;
			npc = (struct context*) np->stack_top;
			npc->IP = FP_OFF(&test4_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test4_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			err3 = insert(np,READY+1);
		}
	}
    else printf("\nProcess with name 'test4' already exists.");

	findPCB("test5",np);
	if(errx == ERR_PCBNF){
		np = allocate_PCB();
		if (np == NULL) err3 = ERR_UCPCB;
		else {
			err3 = setup_PCB(np, "test5",APP,0);
			if (err3 < OK) return err3;
			npc = (struct context*) np->stack_top;
			npc->IP = FP_OFF(&test5_R3); //test1_R3 is a func name in procs-r3.c
			npc->CS = FP_SEG(&test5_R3);
			npc->FLAGS = 0x200;
			npc->DS = _DS;
			npc->ES = _ES;
			err3 = insert(np,READY+1);
		}
	}
	else printf("\nProcess with name 'test5' already exists.");
	if(err3==OK) printf("\nTest processes loaded successfully!");
	return err3;
}

/*
 */
int load_prog(char * fname, int pri, int procClass) {
	int offset_p;
	int progLength;
	struct PCB *newNode;
	struct context *cp;
	err4 = 0;

	if(pri < -128 || pri > 127) {
		pri = 0;
        printf("\nInvalid priority level.  Priority defaulted to 0.");
    }
    newNode = findPCB(fname, newNode);
    if(newNode != NULL) return ERR_NAMEAE;

	err4 = sys_check_program("MPXFILES",fname,&progLength,&offset_p);
	if(err4 < OK) return err4;
	if(NULL == (newNode = allocate_PCB())) return ERR_UCPCB;

	setup_PCB(newNode,fname,procClass,pri);
	if(!strncmp(fname, "idle", 5)) newNode->suspended = NOTSUSP;
	else newNode->suspended = SUSP;

    newNode->mem_size = progLength;
	newNode->load_address = (unsigned char*)sys_alloc_mem(progLength);
	newNode->execution_address = newNode->load_address + offset_p;

	cp = (struct context *)newNode->stack_top;
	cp->IP = FP_OFF(newNode->execution_address);
	cp->CS = FP_SEG(newNode->execution_address);
	cp->DS = _DS;
	cp->ES = _ES;
	cp->FLAGS = 0x200;
	
	err4 = sys_load_program(newNode->load_address, newNode->mem_size, "MPXFILES",fname);
	if(err4>=OK) err4 = insert(newNode,READY+1);
	if(strncmp(fname, "idle", 5)) {if(err4>=OK) printf("Program successfully loaded!");}
	return err4;
}

/**
 */
int load() {
    char buff[BIGBUFF];
	int buffsize = BIGBUFF;
	char name[PROCESS_NAME_LENGTH];
	struct PCB *temppcb = NULL;
	memset(buff, '\0', BIGBUFF);
	
	err4 = 0;
    printf("\nPlease enter the name of the process to be created (9 char limit; no extension): ");
	err4 = sys_req(READ, TERMINAL, buff, &buffsize);
	if (err4 < OK) return err4;
	trim(buff);
	toLowerCase(buff);
    if (strlen(buff)>9) return ERR_PRONTL;
    temppcb = findPCB(buff,temppcb);
	if (temppcb != NULL) return ERR_NAMEAE;
    strncpy(name,buff,PROCESS_NAME_LENGTH);
    load_prog(name,0,APP);
    return err4;
}

/**
 */	
int terminate() {
    char buff[BIGBUFF];
	struct PCB *tmp = NULL;
	int buffsize = BIGBUFF;
	memset(buff, '\0', BIGBUFF);

    err4 = 0;
	printf("Please enter the name of the process to delete: ");
	err4 = sys_req(READ, TERMINAL, buff, &buffsize);	
	if(err4 >= OK) {
		if(tmp->proc_class != SYSTEM) {
		  trimx(buff);
		  tmp = qRemove(buff,tmp);
		  free_PCB(tmp);
		  printf("\nTermination successful!");
		}
		else return ERR_UTDSC;
	}
	return err4;
}

/**
 */
int init_f() {
  tmpIOD=NULL;  
  *(terminal->event_flag) = 1;
  terminal->count = 0;
  terminal->head = NULL;
  terminal->tail = NULL;
	
  *(comport->event_flag) = 1;
  comport->count = 0;
  comport->head = NULL;
  comport->tail = NULL;
  
  trm_open(terminal->event_flag);
  com_open(comport->event_flag, 1200);
  return 0;
}

/**
 */
int cleanup_f() {
  trm_close();
  com_close();

  //free IODs
  tmpIOD = comport->head;
  while (tmpIOD != NULL) {
    comport->head = (comport->head)->next;
    sys_free_mem(tmpIOD);
    tmpIOD = comport->head;   
  }
  tmpIOD = terminal->head;
  while (tmpIOD != NULL) {
    terminal->head = (terminal->head)->next;
    sys_free_mem(tmpIOD);
    tmpIOD = terminal->head;
  }
  
  return 0;
}

/*
 */
void IOschedule() {
	int retq = 0;
	int dev_id = param_p->device_id;
	struct IOD * newIOD = createIOD();

    cop->state = BLOCKED;
    insert(cop, BLOCKED);

	if(dev_id == COM_PORT) {
		retq = enqueue(newIOD,comport);
		if(retq == 1) process_com();
	}
	else if(dev_id == TERMINAL) {
	    retq = enqueue(newIOD,terminal);
		if(retq == 1) process_trm();
    }
}

/*
 */
int process_com() {
  *(comport->event_flag) = 0;
  switch((comport->head)->request) {
  case READ: {
		com_read((comport->head)->tran_buff, (comport->head)->buff_count);
		break;}
  case WRITE: {
		com_write((comport->head)->tran_buff, (comport->head)->buff_count);
		break;}
  default: {
		return ERR_UNKN_REQUEST;}
  }
  return OK;
}

/*
 */
int process_trm() {
  *(terminal->event_flag) = 0;
  switch((terminal->head)->request) {
  case READ: {
		trm_read((terminal->head)->tran_buff, (terminal->head)->buff_count);
		break;}
  case WRITE: {
		trm_write((terminal->head)->tran_buff, (terminal->head)->buff_count);
		break;}
  case CLEAR: {
        trm_clear();
		break;}
  case GOTOXY: {
        trm_gotoxy(0,0);  //NOT DONE!
		break;}
  default: {
		return ERR_UNKN_REQUEST;}
  }
  return OK;
}

/*
 */
int enqueue(struct IOD * nIOD, struct IOCB * queue) {
	int retv = 0;

	if(queue->count == 0) {
		queue->head = nIOD;
		queue->tail = nIOD;
		retv = 1;
	}
	else {
		(queue->tail)->next = nIOD;
		queue->tail = nIOD;
	}
	queue->count++;
	return retv;
}

/*
 */
struct IOD * dequeue(struct IOCB * queue) {
	struct IOD * tempIOD;
	tempIOD = queue->head;

    if(queue->count == 0) return NULL;
	if(queue->count == 1) {
	 queue->head = NULL;
	 queue->tail = NULL;
	}
	else queue->head = (queue->head)->next;
	queue->count--;

	return tempIOD;
}

/*
 */
struct IOD * createIOD() {
	struct IOD *newIOD = NULL;
	newIOD = (struct IOD *)sys_alloc_mem((sizeof(struct IOD)));
	//newIOD->name = cop->name;
	strncpy(newIOD->name, cop->name, PROCESS_NAME_LENGTH);
	newIOD->requestor = cop;
	newIOD->tran_buff = param_p->buf_p;
	newIOD->buff_count = param_p->count_p;
	newIOD->request = param_p->op_code;
	newIOD->next = NULL;
	return newIOD;
}

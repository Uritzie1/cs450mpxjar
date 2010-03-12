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
#include "R12.h"

// Global Variables
int err = 0;  //error code
char * fcns[20] = {"date\0","help\0","ver\0","dir\0","quit\0","list\0","cpcb\0","dpcb\0","block\0","unblock\0","suspend\0","resume\0","setpri\0","shpcb\0","shall\0","shready\0","shblock\0","dispat\0","ldprocs\0",NULL};  //functions list
char wd[BIGBUFF*2] = {0};  //working directory
struct PCB *tail1=NULL, *tail2=NULL, *head1=NULL, *head2=NULL;
int errx = 0;


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
int setup_PCB(struct PCB *PCBptr, char name[PROCESS_NAME_LENGTH], int proc_class, int priority);
int free_PCB(struct PCB *PCBptr);
int create_PCB();
int delete_PCB();
int isEmpty(int q);
int insert(struct PCB *newPCB,int q);
struct PCB* findPCB(char *name, struct PCB *PCBptr);
struct PCB* qRemove(char *name, struct PCB *set);
void toLowerCasex(char str[BIGBUFF]);
void trimx(char ary[BIGBUFF]);
struct PCB* getRHead();

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
  sys_init(MODULE_R3);
  err = init_r1();
  err = init_r2();
  err = comhan();
  err = cleanup_r1();
  err = cleanup_r2();
  terminate_mpx();
  return 0;
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
    printf("\n>>");                                   //command prompt
    err = sys_req(READ, TERMINAL, cmd, &bufsize);     //read in command
    trim(cmd);
    toLowerCase(cmd);
    if (!strncmp(cmd,fcns[QUIT],strlen(fcns[QUIT])+1)) terminate_mpx();  //call corresponding function
    else if (!strncmp(cmd,fcns[VER],strlen(fcns[VER]))) get_Version();
    else if (!strncmp(cmd,fcns[HELP],strlen(fcns[HELP])+1)) {
      err = help(NULL);
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[DATE],strlen(fcns[DATE])+1)) {
      err = date();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[DIR],strlen(fcns[DIR])+1)) {
      err = disp_dir();
      if(err < OK) err_hand(err);
    }
    //R2 commands
    else if (!strncmp(cmd,fcns[CREATEPCB],strlen(fcns[CREATEPCB])+1)) {
      err = create_PCB();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[DELPCB],strlen(fcns[DELPCB])+1)) {
      err = delete_PCB();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[BLOCK],strlen(fcns[BLOCK])+1)) {
      err = block();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[UNBLOCK],strlen(fcns[UNBLOCK])+1)) {
      err = unblock();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[SUSPEND],strlen(fcns[SUSPEND])+1)) {
      err = suspend();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[RES],strlen(fcns[RES])+1)) {
      err = resume();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[SETPRI],strlen(fcns[SETPRI])+1)) {
      err = set_Priority();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[SHOWPCB],strlen(fcns[SHOWPCB])+1)) {
      err = show_PCB();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[SHOWALL],strlen(fcns[SHOWALL])+1)) {
      err = show_All();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[SHOWREADY],strlen(fcns[SHOWREADY])+1)) {
      err = show_Ready();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[SHOWBLOCKED],strlen(fcns[SHOWBLOCKED])+1)) {
      err = show_Blocked();
      if(err < OK) err_hand(err);
    }
    //R3 commands
    else if (!strncmp(cmd,fcns[DISPATCH],strlen(fcns[DISPATCH])+1)) {
      err = dispatcher();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[LOADPROCS],strlen(fcns[LOADPROCS])+1)) {
      err = load_test();
      if(err < OK) err_hand(err);
    }
    //end new commands
    else if (!strncmp(cmd,"\n",1)) ;
    else err_hand(ERR_INVCOM);
  }
  //terminate_mpx();
  //return 0;
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
    if (err < OK) err_hand(err);
    sys_exit();
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
  else if(err_code == ERR_INVCLS) printf("Invalid process class.");
  else printf("Invalid error code %d", err_code);
  err = 0;
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
	  insert(temppcb, RUNNING);
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
	else insert(temppcb, RUNNING);
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
	temppcb = tail1;	
	errx = 0;
	printf("\nPROCESS PROPERTIES------------------------");
	for (x;x<=1;x++) {
	  while(temppcb != NULL) {
	printf("\n\nName: %s", temppcb->name);
	    if(temppcb->state == READY) printf("\nState: Ready");
	    else if(temppcb->state == RUNNING) printf("\nState: Running");
		else printf("\nState: Blocked");
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
	  errx = insert(newPCBptr,RUNNING);
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
	newPCBptr->stack_top = newPCBptr->stack_base + STACK_SIZE;
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
* \return an integer error code (o for now)
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
	(PCBptr->stack_base) = (PCBptr->stack)[STACK_SIZE] ;
	(PCBptr->stack_top) = (PCBptr->stack)[STACK_SIZE + 1];
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
    if(q == 1) {  //For Ready Queue
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

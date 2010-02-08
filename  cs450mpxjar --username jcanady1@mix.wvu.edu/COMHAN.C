/**
 * File Name: comhan.c
 * \title JAROS
 * \author Jonroy Canady
 * \author Adam Trainer
 * \author Rob Wayland   
 * \date 2/04/2010
 * \version: 1.72
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
#define VERSION 1.72

// Global Variables
int err = 0;  //error code
char * fcns[7] = {"date\0","help\0","ver\0","dir\0","quit\0","list\0",NULL};  //functions list
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
  sys_init(MODULE_R1);
  err = init_r1();
  err = comhan();
  err = cleanup_r1();
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
    if (!strncmp(cmd,fcns[QUIT],5)) terminate_mpx();  //call corresponding function
    else if (!strncmp(cmd,fcns[VER],4)) get_Version();
    else if (!strncmp(cmd,fcns[HELP],5)) {
      err = help(NULL);
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[DATE],5)) {
      err = date();
      if(err < OK) err_hand(err);
    }
    else if (!strncmp(cmd,fcns[DIR],4)) {
      err = disp_dir();
      if(err < OK) err_hand(err);
    }
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
  int *bufsize = BIGBUFF;
  char buffer[BIGBUFF] = {0};
  char wdc[BIGBUFF*2] = {0};
  char tbuffer[9] = "\\help\\";
  for(i = 0; i < BIGBUFF * 2; i++)
  wdc[i] = wd[i];

  printf("Help: enter command (or list for command list): ");
  if ((err = sys_req(READ, TERMINAL, buffer, &bufsize)) < OK) return err;
  trim(buffer);
  toLowerCase(buffer);
  for(i = 0; i < 6;i++) if(!strncmp(fcns[i],buffer, strlen(fcns[i]))) j++;
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
  else printf("Invalid error code %d", err_code);
  err = 0;
}

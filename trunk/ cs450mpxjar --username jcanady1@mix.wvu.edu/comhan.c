/*******************************************************************************
 * File Name: comhan.c
 *
 * Author(s): Jonroy Canady,
 * Version: 1.0
 * Date Last Modified: 1/25/2010
 *
 * Components:
 *
 *******************************************************************************
 * Change Log:
 *
 *        1/25/2010  JDC       Original version: outline, nonfunctional
 *        1/28/2010  JDC, RW   Slight comhan fcn editing
 *        2/01/2010  JDC, RW   Solved initial errors such that it compiles
 *        2/02/2010  JDC, RW   Added version and partial help functions; improved comhan
 *        2/03/2010  JDC       Completed dir, date, and err_hand functions and comhan; R1 operational minus help function
 *
 ******************************************************************************/

// Included ANSI C Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <direct.h>

// Included Support Files
#include "mpx_supt.h"

// Status and Error Codes
#define ERR_INVCOM (-201) // Invalid command
#define ERR_INVYR (-202) // Invalid year
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
#define VERSION 1.0

// Global Variables
int err = 0;
char * fcns[6] = {"date\0","help\0","ver\0","dir\0","quit\0","list\0",NULL};
char * fcns[6] = {"date\n","help\n","ver\n","dir\n","quit\n",NULL};
char wd[160] = {0};

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


/* Procedure Name: main
 * Params: none
/*
 *
 * Returns: an integer that is 0 if successful (which it always is)
 * Procedures Called: sys_init, init_r1, comhan, cleanup_r1, terminate_mpx
 * Globals Used: err
 * Description/Purpose: Main simply initializes the system, calls comhan, and
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

/* Procedure Name: comhan
 * Params: none
 * Returns: an integer that is 0 if successful (which it always is)
 * Procedures Called: memset, sys_req, trim, strncmp, help, err_hand, date,
 *   disp_dir, terminate_mpx
 * Globals Used: err, fcns
 * Description/Purpose: comhan is the heart of the R1 module. It runs the
 *  central loop of the command handler.  It accepts commands entered by the
 *  user, analyzes them, and calls the proper functions.  It also receives all
 *  error codes returned by the functions, passing them to the error handler.
/*
 *
 */
int comhan() {
  char cmd[BIGBUFF]={0};
  int bufsize = BIGBUFF, x = 0;
  printf("\nWelcome to JAROS!\n");
  while (1) {
    x = 0;
    bufsize = BIGBUFF;
    printf("\n>>");  //command prompt
    err = sys_req(READ, TERMINAL, cmd, &bufsize);  //read in command
    if (!strncmp(cmd,fcns[QUIT],5) && !x) {
      terminate_mpx();
      x = 1;
    }
    else if (!strncmp(cmd,fcns[VER],4) && !x) {
      get_Version();
      x = 1;
    }
    else if (!strncmp(cmd,fcns[HELP],5) && !x) {
      err = help(NULL);
      if(err < OK) err_hand(err);
      x = 1;
    }
    else if (!strncmp(cmd,fcns[DATE],5) && !x) {
      err = date();
      if(err < OK) err_hand(err);
      x = 1;
    }
    else if (!strncmp(cmd,fcns[DIR],4) && !x) {
      err = disp_dir();
      if(err < OK) err_hand(err);
      x = 1;
    }
    else if (!strncmp(cmd,"\n",1) && !x) x = 1;
    else if (!x) err_hand(ERR_INVCOM);
  }
 //terminate_mpx();
  //return 0;
}

/* Procedure Name: disp_dir
 * Params: none
 * Returns: an integer error code
 * Procedures Called: memset, sys_open_dir, sys_get_entry, sys_close_dir
 * Globals Used: err
 * Description/Purpose: disp_dir neatly prints a list of .mpx files found in
 *   the MPXFILES folder as well as their sizes in bytes.
/*
 *
 */
int disp_dir() {
  char namebuff[SMALLBUFF];
  long filesize;
  err = sys_open_dir("C:\\Docume~1\\XPMUser\\Desktop\\SVN\\MPXFILES");
  if(err < OK) return err;
  printf("\nFile Name     Size (bytes)");
  while ((err = sys_get_entry(namebuff, SMALLBUFF, &filesize)) == OK) {
    printf("\n%-9.9s     %dl", namebuff, filesize);
  }
  if(err < OK && err != ERR_SUP_NOENTR) return err;
  err = sys_close_dir();
  printf("\n");
  return err;
}

/* Procedure Name: terminate_mpx
 * Params: none
 * Returns: none
 * Procedures Called: memset, sys_req, err_hand, cleanup_r1, sys_exit
 * Globals Used: err
 * Description/Purpose: confirms that the user really wishes to terminate MPX.
 *   If yes, it cleans up and exits. If no, it tells the user such and returns.
/*
 *
 */
void terminate_mpx() {
  char buff[TINYBUFF];
  int buffsize = TINYBUFF;
  printf("Are you sure you want to terminate MPX? (Y/N): ");
  err = sys_req(READ, TERMINAL, buff, &buffsize);
  if (err < OK) {
    err_hand(err);
    return;
  }
  if (buff[0] == 'Y' || buff[0] == 'y') {
    err = cleanup_r1();
    if (err < OK) err_hand(err);
    sys_exit();
  }
  else printf("Termination cancelled.");
}

/* Procedure Name: get_Version
 * Params: none
 * Returns: none
 * Procedures Called: printf
 * Globals Used: none
 * Description/Purpose: simply prints a single line with the version constant
/*
 *
 */
void get_Version()
{
 printf("JAROS current version: %f",VERSION);
}

/* Procedure Name: trim
 * Params: ary is a character array that holds the string to be trimmed
 * Returns: none
 * Procedures Called: isspace
 * Globals Used: none
 * Description/Purpose: trims all white space AND newlines from the entirety of
 *   the string passed in.
/*
 *
 */
 void trim(char ary[BIGBUFF])
{
      char temp[BIGBUFF] = {0};
      int i,j = 0;

      for(i = 0;i<BIGBUFF;i++)
      {
	if(!isspace(ary[i]))
	{
	 if(ary[i] == 12)   //trims newlines
	   j++;
	 else{
	  temp[j] = ary[i];
	  j++;
	 }
	}
      }
      for(i = 0; i < BIGBUFF;i++)
      {
	ary[i] = temp[i];
      }

}

/* Procedure Name: help
 * Params: none
 * Returns: an integer error code
 * Procedures Called: sys_req, err_hand, trim, strcat, fopen, fgets, fclose
 * Globals Used: err, fcns
 * Description/Purpose: displays a list of available functions and a short
 *   description of each.  It then asks the user to input a function name if 
 *   s/he wants a more detailed description.
 */

int help()
{
    FILE *fptr;
    int i,j = 0;
    int *bufsize = BIGBUFF;
    char buffer[BIGBUFF] = {0};
    char tbuffer[9] = "\\help\\";

    printf("Help: enter command (or list for command list): ");
    if ((err = sys_req(READ, TERMINAL, buffer, &bufsize)) < OK)
    {
     printf("%i",err);
     err_hand(err);
    }
    else
    {
      trim(buffer);
      for(i = 0; i < 6;i++)
      {
	if(!strncmp(fcns[i],buffer, strlen(fcns[i])))
	{
	 j++;
	// printf("%s %d",fcns[i],j);
	}
      }
      if(j > 0){
      strcat(wd,tbuffer);
      strcat(wd,buffer);
      strcat(wd,".txt");
      //printf("%s",wd);
       if ((fptr = fopen(wd,"r")) > 0){
	  i = 0;
	  while(fgets(buffer,BIGBUFF,fptr))
	  {
	      printf("%s",buffer);
	       i++;
	       if(i == 24)
	       {
	       printf("Press any key to continue");
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
	else
	{
	 err_hand(ERR_INVCOM);
	}
       }
    fclose(fptr);
    return err;
}

/* Procedure Name: date
 * Params: none
 * Returns: an integer error code
 * Procedures Called: sys_get_date, sys_req, atoi, err_hand, valid_date, sys_set_date
 * Globals Used: err
 * Description/Purpose: displays the current date in MM/DD/YYYY format. It then
 *   asks the user if s/he wants to set a new date. If so, it asks for a new
 *   year, then a new month, and lastly a new day. Determines date validity.
/*
 *
 */
int date() {
  char buff[SMALLBUFF];
  int buffsize = SMALLBUFF;
  int x = 1, temp;
  date_rec *date_p;
  sys_get_date(date_p);
  printf("The current date is (MM/DD/YYYY): %d/%d/%d",date_p->month,date_p->day,date_p->year);
  printf("\nWould you like to change the date (Y/N)? ");
  err = sys_req(READ, TERMINAL, buff, &buffsize);
  if (err < OK) return err;
  if (buff[0] == 'Y' || buff[0] == 'y') {
    while (x) {
      printf("Please enter the new year (YYYY): ");
      err = sys_req(READ, TERMINAL, buff, &buffsize);
      if (err < OK) return err;
      temp = atoi(buff);
      if (temp >= 0 && temp < 10000) x = 0;
      else err_hand(ERR_INVYR);
    }
    x = 1;
    date_p->year = temp;

    while(x) {
      printf("Please enter the new month (MM): ");
      err = sys_req(READ, TERMINAL, buff, &buffsize);
      if (err < OK) return err;
      temp = atoi(buff);
      if (temp >= 1 && temp <= 12) x = 0;
      else err_hand(ERR_INVMON);
    }
    x = 1;
    date_p->month = temp;

    while(x) {
      printf("Please enter the new day (DD): ");
      err = sys_req(READ, TERMINAL, buff, &buffsize);
      if (err < OK) return err;
      temp = atoi(buff);
      printf("You input %d/%d/%d",date_p->month,temp,date_p->year);
      if (valid_date(date_p->year,date_p->month,temp)) x = 0;
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

/* Procedure Name: valid_date
 * Params: accepts three integers, yr, mo, and day, which represent the date
 *   to be validated
 * Returns: an integer that's 1 if the date is valid, 0 if not
 * Procedures Called: none
 * Globals Used: none
 * Description/Purpose: validates the date passed in, checking that the day is
 *   within the range allowed by the month. Accounts for leap years.
/*
 *
 */
int valid_date(int yr, int mo, int day) {
  int leap = 0, valid = 1;
  if (((yr % 4 == 0) && (yr % 100 != 0)) || (yr % 400 == 0)) leap = 1;  //determine if leap year
  if ((mo == 1 || mo == 3 || mo == 5 || mo == 7 || mo == 8 || mo == 10 || mo == 12 || mo == 14) && (day >= 1 && day <= 31)) return valid; //check 31-day months
  else if ((mo == 4 || mo == 6 || mo == 9 || mo == 11 || mo == 13) && (day >= 1 && day <= 30)) return valid; //check 30-day months
  else if ((mo == 2 && leap && (day >= 1 && day <= 29)) || (mo == 2 && !leap && (day >= 1 && day <= 28))) return valid; //check February
  else return !valid;
}

/* Procedure Name: init_r1
 * Params: none
 * Returns: an integer error code (0 for now)
 * Procedures Called: none
 * Globals Used: none
 * Description/Purpose: none for now
/*
 *
 */
int init_r1() {
_getdcwd(3,wd,sizeof(wd));


return 0;
}

/* Procedure Name: init_r1
 * Params: none
 * Returns: an integer error code (0 for now)
 * Procedures Called: none
/*
 *
 * Globals Used: none
 * Description/Purpose: none for now
 */
int cleanup_r1() {
return 0;
}

/* Procedure Name: toLowerCase
 * Params: 
 * Returns: 
 * Procedures Called:
 * Globals Used:
 * Description/Purpose:
/*
 *
 */
void toLowerCase(char str[BIGBUFF*2]) {
  int i = 0;
  for(i;i<strlen(str);i++) str[i] = tolower(str[i]);
}

/* Procedure Name: err_hand
 * Params: an integer err_code that corresponds to a textual error
 * Returns: none
 * Procedures Called: printf
 * Globals Used: none
 * Description/Purpose: prints out an error message based on the error code passed in
 */
void err_hand(int err_code) {
  if(err_code == ERR_INVCOM) printf("Invalid command. Type \"help\" for more info.");
  if(err_code == ERR_INVCOM) printf("Invalid command.  All JAROS commands are lower case.  Type ""help"" for more info.");
  else if(err_code == ERR_INVYR) printf("Invalid year parameter.  Please enter a year from 0-9999\n");
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

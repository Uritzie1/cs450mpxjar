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
char * fcns[6] = {"date\n","help\n","ver\n","dir\n","quit\n",NULL};

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


/*
 *
 */
int main() {
  sys_init(MODULE_R1);
  err = init_r1();
  err = comhan();
  terminate_mpx();
  return 0;
}

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
  terminate_mpx();
  return 0;
}

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

/*
 *
 */
void get_Version()
{
 printf("JAROS current version: %f",VERSION);
}

/*
 *
 */
int help(char *cmdName)
{
    FILE *fptr;
    int i = 0;
    char file_name[11] = {0,0,0,0,0,0,'.','t','x','t','\0'};
    char test[80] = {0};

    printf("Help: %s\n", cmdName);
    for (i;i<6;i++)
    {
	file_name[i] = cmdName[i];
    }


//    printf("file name: %s\n",file_name);
    fptr = fopen(file_name,"r");

    i = 0;
    while(fgets(test,80,fptr))
    {
     if(i == 24)
     {
     i = 0;
     }
    printf("%s",test);
    i++;
    }
}

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

/*
 *
 */
int init_r1() {
return 0;
}

/*
 *
 */
int cleanup_r1() {
return 0;
}

/*
 *
 */
void err_hand(int err_code) {
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
  else printf("Invalid error code.");
  err = 0;
}

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
 *        2/02/2010  JDC, RW   Added help and version functions; improved comhan
 *        2/03/2010  
 ******************************************************************************/

// Included ANSI C Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Included Support Files
#include "mpx_supt.h"

// Status and Error Codes
#define OK	0
#define ERR_INVCOM (-201) // Invalid command

#define ERR_SUP_INVDEV (-101) // invalid device id
#define ERR_SUP_INVOPC (-102) // invalid op code
#define ERR_SUP_INVPOS (-103) // invalid character position
#define ERR_SUP_RDFAIL (-104) // read failed
#define ERR_SUP_WRFAIL (-105) // write failed
#define ERR_SUP_INVMEM (-106) // invalid memory block pointer
#define ERR_SUP_FRFAIL (-107) // free failed
#define ERR_SUP_INVDAT (-108) // invalid date
#define ERR_SUP_DATNCH (-109) // date not changed
#define ERR_SUP_INVDIR (-110) // invalid directory name
#define ERR_SUP_DIROPN (-111) // directory open error
#define ERR_SUP_DIRNOP (-112) // no directory is open
#define ERR_SUP_NOENTR (-113) // no more directory entries
#define ERR_SUP_NAMLNG (-114) // name too long for buffer
#define ERR_SUP_DIRCLS (-115) // directory close error
#define ERR_SUP_LDFAIL (-116) // program load failed
#define ERR_SUP_FILNFD (-117) // file not found
#define ERR_SUP_FILINV (-118) // file invalid
#define ERR_SUP_PROGSZ (-119) // program size error
#define ERR_SUP_LDADDR (-120) // invalid load address
#define ERR_SUP_NOMEM  (-121) // memory allocation error
#define ERR_SUP_MFREE  (-122) // memory free error
#define ERR_SUP_INVHAN (-123) // invalid handler address

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
int prompt = 1;
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
  int bufsize = BIGBUFF;
  printf("\nWelcome to JAROS!\n");
  err = 0;
  while (1) {
    if (prompt==1) printf("\n>>");  //command prompt
    prompt = 1;
    err = sys_req(READ, TERMINAL, cmd, &bufsize);  //read in command
    if (!strncmp(cmd,fcns[QUIT],5)) terminate_mpx();
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
    //analyze command
    //execute command
  }
  //closing message
  terminate_mpx();
  return 0;
}

/*
 *
 */
int disp_dir() {
  char dir_name[SMALLBUFF] = {'M','P','X','F','I','L','E','S','\0'};
  char buff[SMALLBUFF];
  int bufsize = SMALLBUFF, err;
  int filesize;
  err = sys_open_dir(dir_name);
  if(err < OK) {printf("error");return err;}
  printf("\nFile Name     Size (bytes)");
  while ((err = sys_get_entry(buff, bufsize, &filesize)) != ERR_NOENTR) {
    if(err < OK) {printf("error");return err;}
    printf("in loop");
    printf("\n%s-9.9     %dl", buff, filesize);
  }
  if((err = sys_close_dir()) != OK) {printf("error");return err;}
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
    err_hand(err);
    sys_exit();
  }
  else printf("Termination cancelled.");
  prompt = 0;
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
  printf("Not yet implemented.");
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
  else if(err_code == ERR_SUP_NOENTR) printf("No more directory entires.");
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
}

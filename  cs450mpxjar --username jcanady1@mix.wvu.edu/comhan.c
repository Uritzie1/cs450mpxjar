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
#define ERR_INVCOM (-101) // Invalid command
#define ERR_NOENTR (-113) // No more directory entries

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
    printf("\n>>");  //command prompt
    err = sys_req(READ, TERMINAL, cmd, &bufsize);  //read in command
    if (!strncmp(cmd,fcns[QUIT],5)) terminate_mpx();
    else if (!strncmp(cmd,fcns[VER],4)) get_Version();
    else if (!strncmp(cmd,fcns[HELP],5)) err = help(NULL);
    else if (!strncmp(cmd,fcns[DATE],5)) err = date();
    else if (!strncmp(cmd,fcns[DIR],4)) err = disp_dir();
    else if (!strncmp(cmd,"/n",1)) ;
    else printf("Invalid function.");
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
  char dir_name[SMALLBUFF] = {'M','P','X','F','I','L','E','S','/0'};
  char buff[SMALLBUFF];
  int bufsize = SMALLBUFF;
  int filesize;
  sys_open_dir(dir_name);
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
void err_hand(int err_code) {
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
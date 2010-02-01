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

// Global Variables
int err = 0;

// Function Prototypes
void err_hand(int err_code);
int init_r1();
int cleanup_r1();
int disp_dir();
int comhan();


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
  printf("\nWelcome to JAROS!\n");
  char cmd[BIGBUFF]={0};
  int bufsize = BIGBUFF, x = 1;
  err = 0;
  while (x) {
    printf("\n>>");
    //accept command
    err = sys_req(READ, TERMINAL, cmd, &bufsize);
    if (strcmp(cmd,"quit")==0) x = 0;
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
  sys_open_dir(dir_name);
  char buff[SMALLBUFF];
  int bufsize = SMALLBUFF;
  int filesize;
  printf("\nFile Name  Size (bytes)");
  while ((err = sys_get_entry(buff, bufsize, &filesize)) != ERR_NOENTR) {
    if(err < OK) return err;
    printf("\n%s-9.9  %dl", buff, filesize);
  }
  if((err = sys_close_dir(void)) != OK) err_hand(err);
}

/*
 *
 */
void terminate_mpx() {
  printf("\nAre you sure you want to terminate MPX? (Y/N): ");
  char buff[TINYBUFF];
  int buffsize = TINYBUFF;
  err = sys_req(READ, TERMINAL, buff, &buffsize);
  if (err < OK) {
    err_hand(err);
    return;
  }
  if (buff[0] == 'Y' || buff[0] == 'y') {
    err = cleanup_r1();
    sys_exit(void);
  }
  else {
    printf("Termination cancelled.");
    return;
  }
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

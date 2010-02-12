/**
 * File Name: 
 * \title JAROS
 * \author Jonroy Canady
 * \author Adam Trainer
 * \author Rob Wayland   
 * \date 2/12/2010
 * \version: 1.0
 *
 * Components: 
 *
 *******************************************************************************
 * Change Log:
 *
 *        2/12/2010  JC           File created with placeholders
 */

// Included ANSI C Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Included Support Files
#include "mpx_supt.h"

// Status and Error Codes


// Constants


// Global Variables

// Function Prototypes
int init_r2();
int cleanup_r2();


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
  sys_init(MODULE_R2);
  init_r2();
  cleanup_r2();
  return 0;
}

/** Procedure Name: init_r1
 * \param none
 * \return err an integer error code (0 for now)
 * Procedures Called: _getdcwd
 * Globals Used: 
 * @var wd
 * \brief Description/Purpose: finds the working directory and writes it to the global
 */
int init_r2() {
  return 0;
}

/** Procedure Name: init_r1
 * \param none
 * \return  an integer error code (0 for now)
 * Procedures Called: none
 * Globals Used: none
 * \brief Description/Purpose: none for now
 */
int cleanup_r2() {
  return 0;
}


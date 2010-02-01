/**
   Author:  Rob Wayland
*/



#include <stdio.h>

float ver = 1;


void getVersion()
{
 printf("JAROS current version: %f",ver);
}

int Help(char *cmdName)
{
    char fname[10];
    int buf_size = 10;
    int sys_open_dir ("help");
    int sys_get_entry (fname, /* file name buffer */
			   int buf_size, /* name buffer size */
			   long *file_size_p /* file size variable */
			   );
    
}

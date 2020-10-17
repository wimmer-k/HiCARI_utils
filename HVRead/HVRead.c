#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "HVFunctions.h"


HV System[MAX_HVPS];
int main(void){
  int ret;
  for(ret = 0; ret < MAX_HVPS; ret++)
    System[ret].ID = -1;

  (*HVLogin)();
  (*HVBdTemps)();
	
  //(*HVChNames)();
  (*HVReadCh)("VMon");
  (*HVLogout)();

  return 0;
}

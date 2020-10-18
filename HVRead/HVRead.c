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
#include <ncurses.h>
#include "HVFunctions.h"

HV System[MAX_HVPS];
bool signal_received = false;
void signalhandler(int sig){
  if (sig == SIGINT){
    signal_received = true;
  }
}



int main(void){

  signal(SIGINT, signalhandler);
  
  int ret;
  for(ret = 0; ret < MAX_HVPS; ret++)
    System[ret].ID = -1;

  HVLogin();
  while(!signal_received){
    outfile = fopen("HVStatus.dat", "w");
    HVBdTemps(); 
    HVRead();
    fclose(outfile);
    sleep(5);
  } 
  //HVRead();
  //HVTestCh(6, 0, "Pw");
  //HVTestCh(0, 0);
  //HVChNames();
  //HVReadCh("VMon");
  //HVReadValues();
  HVLogout();

  return 0;
}

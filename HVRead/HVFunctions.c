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
#include <errno.h>
#include "HVFunctions.h"
#include "CAENHVWrapper.h"


#define MAX_CH_NUM 10
#define MAX_BD_NUM 5
// from example
static int noHVPS(void){	
  int i = 0;
  
  while( System[i].ID == -1 && i != (MAX_HVPS - 1)) i++;
  return ( ( i == MAX_HVPS - 1 ) ? 1 : 0 );
}
static int OneHVPS(void){	
  int i, j, k;
  
  for( i = 0, k = 0 ; i < (MAX_HVPS - 1) ; i++ )
    if( System[i].ID != -1 ){
      j = i;
      k++;
    }
  
  return ( ( k != 1 ) ? -1 : j );
}



void HVLogin(){
  int i, link;
  char ipAddress[30], userName[30], passwd[30];
  CAENHVRESULT ret;
  int sysHndl=-1;
  int sysType= 3; // SY5527
  i = 0;
  while( System[i].ID != -1 && i != (MAX_HVPS - 1))
    i++;
  if(i == MAX_HVPS - 1){
      printf("Too many connections");
      return;
  }
  link = LINKTYPE_TCPIP;
  strcpy(ipAddress, "10.32.4.181");
  strcpy(userName, "admin");
  strcpy(passwd, "admin");
  ret = CAENHV_InitSystem((CAENHV_SYSTEM_TYPE_t)sysType, link, ipAddress, userName, passwd, &sysHndl);
  if( ret == CAENHV_OK ){
    printf("login successful\n");
    
    i = 0;
    while( System[i].ID != -1 ) i++;
    System[i].ID = ret;
    System[i].Handle = sysHndl;
  }
}
void HVLogout(){
  int handle = -1;
  int i;
  CAENHVRESULT ret;

  if( noHVPS() )
    return;

  if(( i = OneHVPS() ) >= 0)
    handle = System[i].Handle;


  ret = CAENHV_DeinitSystem(handle);
  
  if(ret == CAENHV_OK){
    printf("logout successful\n");
    i = 0;
    while(System[i].Handle,handle)
      i++;
    for( ; System[i].ID != -1; i++){
	System[i].ID = System[i+1].ID;
	System[i].Handle = System[i+1].Handle;
    }
  }
  else{
    printf("logout failure: %s (num. %d)\n\n", CAENHV_GetError(handle), ret);
  }
return;
}
void HVChNames(){
  int handle = -1;
  CAENHVRESULT ret;
  int i;
  unsigned short slot;
  unsigned short chList[MAX_CH_NUM];
  for(int j=0;j<MAX_CH_NUM;j++){
    chList[j] = j;
  }
  char (*chNameList)[MAX_CH_NAME];
  if( noHVPS() )
    return;
  
  if(( i = OneHVPS() ) >= 0 )
    handle = System[i].Handle;

  // we use every other slot
  for(slot =0; slot<10; slot+=2){
    int maxch_thisslot = 8;
    if(slot ==4){
      maxch_thisslot = 10;
    }
    
    chNameList = malloc(maxch_thisslot*MAX_CH_NAME);

    ret = CAENHV_GetChName(handle, slot, maxch_thisslot, chList, chNameList);
    if(ret != CAENHV_OK){
      free(chNameList);
      printf("error reading ch names of slot %d: %s (num. %d)\n\n", slot, CAENHV_GetError(handle), ret);
      free(chNameList);
      return;
    }
    printf("SLOT %d\n",slot);    
    for(int j=0;j<maxch_thisslot;j++){
      printf("ch %d: %s\n", chList[j], chNameList[j]);
    }
    free(chNameList);
  }
}
void HVBdTemps(){
  int handle = -1;
  CAENHVRESULT ret;
  int i;
  char parName[30];
  unsigned short *bdList;
  float	*parValues = NULL;
  unsigned long	parType;
  
  
  if( noHVPS() )
    return;
  
  if(( i = OneHVPS() ) >= 0 )
    handle = System[i].Handle;
  
  strcpy(parName, "Temp");
  bdList = malloc(MAX_BD_NUM * sizeof(unsigned short));
  int nBD = 0;
  for(int slot =0; slot<10; slot+=2){
    bdList[nBD] = slot;
    nBD = nBD + 1;
  }
  printf("number of boards %d\n",nBD);
  ret = CAENHV_GetBdParamProp(handle, bdList[0], parName, "Type", &parType);
  if(ret != CAENHV_OK || parType != PARAM_TYPE_NUMERIC){
    printf("error in getting board temperatures");
    if(parValues != NULL)
      free(parValues);
    free(bdList);
    return;
  }
  parValues = malloc(nBD*sizeof(float));
  ret = CAENHV_GetBdParam(handle, nBD, bdList, parName, parValues);
  if(ret != CAENHV_OK){
    printf("error in getting board temperatures");
    if(parValues != NULL)
      free(parValues);
    free(bdList);
    return;
  }
  for(i = 0; i<nBD; i++){
    printf("Board %d Temperature: %.2f\n",bdList[i],parValues[i]);
  }
  if(parValues != NULL)
    free(parValues);
  free(bdList);

}
void HVReadCh(char *pn){
  int handle = -1;
  CAENHVRESULT ret;
  int i;
  char parName[30];
  float	*parValues = NULL;
  unsigned long	parType;
  
  
  if( noHVPS() )
    return;
  
  if(( i = OneHVPS() ) >= 0 )
    handle = System[i].Handle;

  unsigned short chList[MAX_CH_NUM];
  for(int j=0;j<MAX_CH_NUM;j++){
    chList[j] = j;
  }

  strcpy(parName, pn);

  int slot = 0;
  // we use every other slot
  for(slot =0; slot<10; slot+=2){
    int maxch_thisslot = 8;
    if(slot ==4){
      maxch_thisslot = 10;
    }

    // print the parameters
    //int channel = 0;
    //char *pars, *p;
    //int npars;
    //ret = CAENHV_GetChParamInfo(handle, slot, channel, &pars, &npars);
    //for(i = 0, p = pars; i < npars; i++, p+=10) {
    //  printf("par %d: %s\n",i,p);
    //}
  


    // get type
    ret = CAENHV_GetChParamProp(handle, slot, chList[0], parName, "Type", &parType);
    if( ret != CAENHV_OK || parType != PARAM_TYPE_NUMERIC){
      printf("error reading channel property %s of slot %d: %s (num. %d)\n\n", parName, slot, CAENHV_GetError(handle), ret);
      return;
    }

    parValues = malloc(maxch_thisslot*sizeof(float));
    ret = CAENHV_GetChParam(handle, slot, parName, maxch_thisslot, chList, parValues);
    if(ret != CAENHV_OK){
      printf("error in getting board temperatures");
      if(parValues != NULL)
	free(parValues);
      return;
    }
    printf("SLOT %d\n",slot);    
    for(int j=0;j<maxch_thisslot;j++){
      printf("ch %d: %.3f\n", chList[j], parValues[j]);
    }   
  }// slots
  if(parValues != NULL)
    free(parValues);
  return;
}

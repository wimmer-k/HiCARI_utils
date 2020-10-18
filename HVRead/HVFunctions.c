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
  else{
    printf("cannot login\n");
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

//void OLD_HVBdTemps(){
//  int handle = -1;
//  CAENHVRESULT ret;
//  int i;
//  char parName[30];
//  unsigned short *bdList;
//  float	*parValues = NULL;
//  unsigned long	parType;
//  
//  
//  if( noHVPS() )
//    return;
//  
//  if(( i = OneHVPS() ) >= 0 )
//    handle = System[i].Handle;
//  
//  strcpy(parName, "Temp");
//  bdList = malloc(MAX_BD_NUM * sizeof(unsigned short));
//  int nBD = 0;
//  for(int slot =0; slot<10; slot+=2){
//    bdList[nBD] = slot;
//    nBD = nBD + 1;
//  }
//  printf("number of boards %d\n",nBD);
//  ret = CAENHV_GetBdParamProp(handle, bdList[0], parName, "Type", &parType);
//  if(ret != CAENHV_OK || parType != PARAM_TYPE_NUMERIC){
//    printf("error in getting board temperatures");
//    if(parValues != NULL)
//      free(parValues);
//    free(bdList);
//    return;
//  }
//  parValues = malloc(nBD*sizeof(float));
//  ret = CAENHV_GetBdParam(handle, nBD, bdList, parName, parValues);
//  if(ret != CAENHV_OK){
//    printf("error in getting board temperatures");
//    if(parValues != NULL)
//      free(parValues);
//    free(bdList);
//    return;
//  }
//  for(i = 0; i<nBD; i++){
//    printf("Board %d Temperature: %.2f\n",bdList[i],parValues[i]);
//  }
//  if(parValues != NULL)
//    free(parValues);
//  free(bdList);
//
//}


void HVBdTemps(){
  int handle = -1;
  CAENHVRESULT ret;
  int i;
  char parName[30];

  unsigned char parValues[512];
  unsigned int parType;
  float Temp = 0;
  
  if( noHVPS() )
    return;
  
  if(( i = OneHVPS() ) >= 0 )
    handle = System[i].Handle;
  
  strcpy(parName, "Temp");

  unsigned short slot, ch;
  // we use every other slot
  for(slot =0; slot<10; slot+=2){
    ret = CAENHV_GetBdParamProp(handle, slot, parName, "Type", &parType);
    if(ret != CAENHV_OK || parType != PARAM_TYPE_NUMERIC){
      printf("error in getting board temperatures");
      return;
    }
    ret = CAENHV_GetBdParam(handle, 1, &slot, parName, (void*)parValues);
    if(ret != CAENHV_OK){
      printf("error in getting board temperatures");
      return;
    }
    Temp = *((float *)parValues);
    printf("Board %d Temperature: %.2f\n",slot,Temp);
    fprintf(outfile,"Board %d Temperature: %.2f\n",slot,Temp);
  }//slots
  
}

//Main function to read all important values from all our boards
void HVRead(){
 int handle = -1;
  CAENHVRESULT ret;
  int i;
  char parName[30];
  unsigned char parValues[512];
  unsigned int parType;

  char chName[MAX_CH_NAME] = "ND";
  char onoff[5] = "ND";
  float VMon = 0;
  float IMon = 0;
  unsigned int status = 0;
  
  char* pnames[4] = {"Pw","VMon","IMon", "Status"};
  
  if( noHVPS() )
    return;
  
  if(( i = OneHVPS() ) >= 0 )
    handle = System[i].Handle;

  unsigned short slot, ch;
  // we use every other slot
  for(slot =0; slot<10; slot+=2){
    int maxch_thisslot = 8;
    if(slot ==4){
      maxch_thisslot = 10;
    }

    for(ch=0;ch<maxch_thisslot;ch++){
      
      //channel names
      ret = CAENHV_GetChName(handle, slot, 1, &ch, (void *)parValues);
      if(ret != CAENHV_OK){
	printf("error reading ch names of slot %d: %s (num. %d)\n\n", slot, CAENHV_GetError(handle), ret);
	return;
      }
      strcpy(chName, (char *)parValues );

      // parameters
      for(i =0; i<4; i++){
	strcpy(parName, pnames[i]);
	ret = CAENHV_GetChParamProp(handle, slot, ch, parName, "Type", &parType);
	if( ret != CAENHV_OK){
	  printf("error reading channel property %s of slot %d: %s (num. %d)\n\n", parName, slot, CAENHV_GetError(handle), ret);
	  return;
	}
	if((int)parType==PARAM_TYPE_ONOFF){
	  ret = CAENHV_GetChParam(handle, slot, parName, 1, &ch, (void *)parValues);
	  strcpy(onoff, (*((char *)parValues)) ? "ON" : "OFF");
	}
	if((int)parType==PARAM_TYPE_NUMERIC){
	  ret = CAENHV_GetChParam(handle, slot, parName, 1, &ch, (void *)parValues);
	  if(i==1)
	    VMon = *((float *)parValues);
	  if(i==2)
	    IMon = *((float *)parValues);
	}
	if((int)parType==PARAM_TYPE_CHSTATUS){
	  ret = CAENHV_GetChParam(handle, slot, parName, 1, &ch, (void *)parValues);
	  status = *((int *)parValues);
	}
      }//parameters to read

      
      printf("%2d %2d %s:\t\t%s\t%.2f\t%.5f\t",slot,ch,chName,onoff,VMon,IMon);
      fprintf(outfile,"%2d %2d %s:\t\t%s\t%.2f\t%.5f\t",slot,ch,chName,onoff,VMon,IMon);
      
      char *stats[] = {"On", "Ramp Up", "Ramp Down", "Over Current",
		       "Over Voltage", "UnderVoltage", "Ext. Trip",
		       "max V", "ext. Disable", "Int. Trip", "Cal. Err",
		       "Unplugged","Empty","Over Voltage Protection",
		       "Power Fail", "Temp. Err", NULL };
      for(int i=0; stats[i];i++){
	if(! (status & (1 <<i))) // bit not set
	  continue;
	printf("%s, ", stats[i]);
      }
      
      printf("\t%d\n",status);
      fprintf(outfile,"\t%d\n",status);
    }// channles
  }// slots
}


void HVTestCh(unsigned short slot, unsigned short ch){
 int handle = -1;
  CAENHVRESULT ret;
  int i;
  char parName[30];
  unsigned char parValues[512];
  unsigned int parType;

  char chName[MAX_CH_NAME] = "ND";
  char onoff[5] = "ND";
  float VMon = 0;
  float IMon = 0;

  char* pnames[4] = {"Pw","VMon","IMon", "Status"};
  
  if( noHVPS() )
    return;
  
  if(( i = OneHVPS() ) >= 0 )
    handle = System[i].Handle;

  //name 
  //channel names
  ret = CAENHV_GetChName(handle, slot, 1, &ch, (void *)parValues);
  if(ret != CAENHV_OK){
    printf("error reading ch names of slot %d: %s (num. %d)\n\n", slot, CAENHV_GetError(handle), ret);
    return;
  }
  strcpy(chName, (char *)parValues );
  
  for(i =0; i<4; i++){
    strcpy(parName, pnames[i]);
    ret = CAENHV_GetChParamProp(handle, slot, ch, parName, "Type", &parType);
    if( ret != CAENHV_OK){
      printf("error reading channel property %s of slot %d: %s (num. %d)\n\n", parName, slot, CAENHV_GetError(handle), ret);
      return;
    }
    if((int)parType==PARAM_TYPE_ONOFF){
      ret = CAENHV_GetChParam(handle, slot, parName, 1, &ch, (void *)parValues);
      strcpy(onoff, (*((char *)parValues)) ? "ON" : "OFF");
    }
    if((int)parType==PARAM_TYPE_NUMERIC){
      ret = CAENHV_GetChParam(handle, slot, parName, 1, &ch, (void *)parValues);
      if(i==1)
	VMon = *((float *)parValues);
      if(i==2)
	IMon = *((float *)parValues);
    }


    
    if((int)parType==PARAM_TYPE_CHSTATUS){
      ret = CAENHV_GetChParam(handle, slot, parName, 1, &ch, (void *)parValues);
      
      char *stats[] = {"On", "Ramp Up", "Ramp Down", "Over Current", "Over Voltage", "UnderVoltage", "Ext. Trip", "max V", "ext. Disable", "Int. Trip", "Cal. Err", "Unplugged","Empty","Over Voltage Protection", "Power Fail", "Temp. Err", NULL };

      unsigned int num = *((int *)parValues);
      printf("%X\t\t",*((int *)parValues));
      
      for(int i=0; stats[i];i++){
	if(! (num & (1 <<i))) // bit not set
	  continue;
	printf("%s, ", stats[i]);
      }
    }
  }//parameters to read
  printf("\n%s:\t%s\t%.2f\t%.5f\n",chName,onoff,VMon,IMon);
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
  //// print the parameters
  //int channel = 0;
  //char *pars, *p;
  //int npars;
  //ret = CAENHV_GetChParamInfo(handle, slot, channel, &pars, &npars);
  //for(i = 0, p = pars; i < npars; i++, p+=10) {
  //  printf("par %d: %s\n",i,p);
  //}

  // we use every other slot
  for(slot =0; slot<10; slot+=2){
    int maxch_thisslot = 8;
    if(slot ==4){
      maxch_thisslot = 10;
    }

  


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
void HVReadValues(){
  int handle = -1;
  CAENHVRESULT ret;
  int i;
  unsigned short slot;
  unsigned short chList[MAX_CH_NUM];
  char parName[30];
  float	*VMonValues = NULL;
  float	*IMonValues = NULL;
  //unsigned long *OnOffValues = NULL;
  //unsigned long *StatusValues = NULL;
  unsigned long	parType;

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

    //channel names
    ret = CAENHV_GetChName(handle, slot, maxch_thisslot, chList, chNameList);
    if(ret != CAENHV_OK){
      printf("error reading ch names of slot %d: %s (num. %d)\n\n", slot, CAENHV_GetError(handle), ret);
      free(chNameList);
      return;
    }

    // read the voltage
    strcpy(parName, "VMon");
    // get type
    ret = CAENHV_GetChParamProp(handle, slot, chList[0], parName, "Type", &parType);
    if( ret != CAENHV_OK || parType != PARAM_TYPE_NUMERIC){
      printf("error reading channel property %s of slot %d: %s (num. %d)\n\n", parName, slot, CAENHV_GetError(handle), ret);
      return;
    }
    VMonValues = malloc(maxch_thisslot*sizeof(float));
    ret = CAENHV_GetChParam(handle, slot, parName, maxch_thisslot, chList, VMonValues);
    if(ret != CAENHV_OK){
      printf("error in getting channel voltage");
      if(VMonValues != NULL)
	free(VMonValues);
      return;
    }
    // read the current 
    strcpy(parName, "IMon");
    // get type
    ret = CAENHV_GetChParamProp(handle, slot, chList[0], parName, "Type", &parType);
    if( ret != CAENHV_OK || parType != PARAM_TYPE_NUMERIC){
      printf("error reading channel property %s of slot %d: %s (num. %d)\n\n", parName, slot, CAENHV_GetError(handle), ret);
      return;
    }
    IMonValues = malloc(maxch_thisslot*sizeof(float));
    ret = CAENHV_GetChParam(handle, slot, parName, maxch_thisslot, chList, IMonValues);
    if(ret != CAENHV_OK){
      printf("error in getting channel current");
      if(IMonValues != NULL)
	free(IMonValues);
      return;
    }

    //// read the status
    //strcpy(parName, "Pw");
    //// get type
    //ret = CAENHV_GetChParamProp(handle, slot, chList[0], parName, "Type", &parType);
    //if( ret != CAENHV_OK || parType != PARAM_TYPE_ONOFF){
    //  printf("error reading channel property %s of slot %d: %s (num. %d)\n\n", parName, slot, CAENHV_GetError(handle), ret);
    //  return;
    //}
    //OnOffValues = malloc(maxch_thisslot*sizeof(long));
    //ret = CAENHV_GetChParam(handle, slot, parName, maxch_thisslot, chList, OnOffValues);
    //if(ret != CAENHV_OK){
    //  printf("error in getting channel on/off status");
    //  if(OnOffValues != NULL)
    //	free(OnOffValues);
    //  return;
    //}

    //// read the status
    //strcpy(parName, "Status");
    //// get type
    //ret = CAENHV_GetChParamProp(handle, slot, chList[0], parName, "Type", &parType);
    //if( ret != CAENHV_OK || parType == PARAM_TYPE_NUMERIC){
    //  printf("error reading channel property %s of slot %d: %s (num. %d)\n\n", parName, slot, CAENHV_GetError(handle), ret);
    //  return;
    //}
    //StatusValues = malloc(maxch_thisslot*sizeof(long));
    //ret = CAENHV_GetChParam(handle, slot, parName, maxch_thisslot, chList, StatusValues);
    //if(ret != CAENHV_OK){
    //  printf("error in getting channel status");
    //  if(StatusValues != NULL)
    //	free(StatusValues);
    //  return;
    //}

    
    printf("SLOT %d\n",slot);    
    for(int j=0;j<maxch_thisslot;j++){
      //char OnOffStatus[30];
      //strcpy(OnOffStatus, (*((char *)OnOffValues[j])) ? "ON" : "OFF");
      //printf("ch %d: %s\t%.2f\t%.4f\t%X\n", chList[j], chNameList[j],VMonValues[j],IMonValues[j],(int)StatusValues[j]);
      //printf("ch %d: %s\t%.2f\t%.4f\t%s\n", chList[j], chNameList[j],VMonValues[j],IMonValues[j],OnOffStatus);
      printf("ch %d: %s\t%.2f\t%.4f\n", chList[j], chNameList[j],VMonValues[j],IMonValues[j]);
    }
    free(chNameList);
    if(VMonValues != NULL)
      free(VMonValues);
    if(IMonValues != NULL)
      free(IMonValues);
    //if(OnOffValues != NULL)
    //  free(OnOffValues);
    //if(StatusValues != NULL)
    //  free(StatusValues);
  }//slots


 return;
}

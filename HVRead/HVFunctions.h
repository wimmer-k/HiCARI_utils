#define  MAX_HVPS          (5)

/* struttura che contiene gli indici di sistema */
typedef struct sys{
  int Handle;
  int ID;
} HV;

void HVLogin(void);
void HVLogout(void);
void HVBdTemps(void);
void HVChNames(void);
void HVReadCh(char*);
void HVReadValues();
void HVTestCh(unsigned short, unsigned short, char *);

extern HV System[];

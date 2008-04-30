
#define HERM1BLOCK 0x80000
#define HERM2BLOCK 0x100000
#define RBLOCK 0x10000
#define RADIOSIZE 0xd80000
//#define BUFSIZE 40000
#define BUFSIZE 600000

extern int fd;
extern char buf[BUFSIZE];
extern int SHELL;
extern int DEBUG;
extern int VERBOSE;
extern char *MODEL;
extern char *device;

typedef struct model_t {
	char *name;
	int (*flashNBH)(char *flashfile);
	int (*flashRadio)(char *flashfile);
	int (*dumpRadio)(char *dumpfile);
	void (*enterRadioBL)();
	void (*exitRadioBL)();
	void (*progressbar)(int pc);
} model_t;

#define CALL(x,y) if(model->x) model->x(y)

extern struct model_t *model;

// crc32.c
unsigned long Crc32 (const unsigned char *pData, unsigned long nSize, unsigned long nPrev);

// decode.c
void DecodeSecurityBuffer (unsigned char *szBuffer);
void DecodeRadioSecurityBuffer (unsigned char *pBuffer);

// io.c
void send (const char *command);
void rsend (const char *command);
void fsend (const char *command);
void get (const char *command);
void opendev();
void init ();

// utils.c
int control_c(int sig);
void spl_progressbar(int pc);
void spl2_progressbar(int pc);
void progressbar(int pc);
void strip (char *string);

// hermes1.c
int hermes1_flashNBH (char *flashfile);
int hermes1_flashRadio (char *flashfile);
int hermes1_dumpRadio (char *dumpfile);
void enterRadioBL();
void exitRadioBL();
void hermes1_authBL();
int hermes1_do_rbmc (char *dumpfile, int i_rbmcstart, int i_rbmcend);
void authRadioBL();

// hermes2.c
int hermes2_flashNBH (char *flashfile);
void hermes2_authBL();
int hermes2_do_rbmc (char *dumpfile, int i_rbmcstart, int i_rbmcend);

// models.c
model_t *resolve_model(char *name);

//flasher.c
void getCID (char *cid);
void getSecLevel (char *seclevel);
void getSPLversion (char *splversion);
void getRadioBLver (char *htcboot);
void getDeviceName (char *devname);
void getQualcommBLver (char *qcboot);
void getCommonBase (char *commonbase);
unsigned long getChecksum(char *addr, char *len, int f);

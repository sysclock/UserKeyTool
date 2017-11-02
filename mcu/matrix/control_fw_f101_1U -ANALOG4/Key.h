#ifndef KEY_H
#define KEY_H

//key val
#define SAVE    10
#define RECALL  11
#define MENU 	13
#define ENTER   14
#define CANCER  15
#define A  12
#define V 	16
#define VGA   17
#define AV  18

typedef enum
{
	DispPage,
	SwitchPage,
	RecallPage,
	SavePage,
	MenuPage,
	AddrPage,
	BaudPage,
    SystemPage,
	LangPage,
	BeepPage,
        CyclePage1,
        CyclePage2,
        CyclePage3
}Page_t;


typedef struct
{
  u8 Input;
	u8 InputFlag;	
	u8 Output;
	u8 OutputFlag;	
	u8 Mode;		
	Page_t CurPage;
}OperateInfo_t;
extern OperateInfo_t OperateInfo;		

typedef struct
{
  u32 CycleTiming;
  u8  CycleNum;
}CycleInfo_t;
extern CycleInfo_t CycleInfo;
extern u32 CycleDelay;
extern u32 CycleTimingTmp;
extern u8 CycleNumTmp;
extern u8 CycleIndex;
typedef enum
{
	Null,		
	Input,
	Output,
	Switch,
	Addr,
	Baud,
	Connect,
	Save,
	Load,
	Beep,
        CycleTim,
        CycleNumber,
        CycleOn,
        CycleOff
}Mode_t;
typedef struct
{
	u8 Enable;
	Mode_t TipType;
}TipInfo_t;
extern TipInfo_t TipInfo;
typedef struct
{
	u8 Enable;
	u8 CurPage;
	u8 DlyTime;
}GoInfo_T;
extern GoInfo_T GoInfo;

void DisposeKey(void);
u8 ReadKey(void);
void ChangeDisp(void);
void DisposalTick(void);
void DrawLogo(void);
void InitSwitch(void);
void LoadEnv(u16 Index);
void SaveEnv(u16 Index);
void DisposalIR(void);
void InitFm(void);
#endif

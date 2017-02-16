#include <gpib.h>
#include <formatio.h>
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>

#include "util.h"
#include "list.h"
#include "channel.h"
#include "changen.h"
#include "chanfnc.h"
#include "chanops.h"
#include "acqchan.h"
#include "acqchanu.h"
#include "curve.h"
#include "acqcrv.h"
#include "graph.h"
#include "graphu.h"
#include "curveop.h"
#include "acquire.h"
#include "acquireu.h"
#include "gpibio.h"
#include "gpibiou.h"
#include "source.h"
#include "sourceu.h"
#include "HP3245A.h"
#include "HP3245Au.h"

#define TRUE 1
#define FALSE 0
#define HP3245A_ID "hewlett-packard"	// faked ID since has no *IDN? response
/*
Specifications:
Amp: 1uV - 10V

Sin: 0 - 1MHz
Squ: 0 - 100kHz
Tri: 0 - 100KHz
resolution 0.001 Hz

Duty:
100uHz - 1MHz: 5 - 95%


*/
typedef enum{SIN, SQU, TRI}waveforms;

typedef enum{FREQ, AMPL} hp3245aCh;
typedef struct{
	sourcePtr sources[2];
	int id;
	double duty;
	char wave[256];
}hp3245aType;

typedef hp3245aType *hp3245aPtr;

/****************************************************************/

void SetHP3245aFreqLvl (sourcePtr src);
void GetHP3245aFreqLvl (acqchanPtr acqchan);
void SetHP3245aAmplLvl (sourcePtr src);
void GetHP3245aAmplLvl (acqchanPtr acqchan);

void hp3245a_UpdateControls(int p, gpibioPtr dev);
int hp3245aControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int hp3245aPanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2);
void hp3245aMenuCallback (int menuBar, int menuItem, void *callbackData, int panel);

void *hp3245a_Create(gpibioPtr dev);
int  hp3245a_InitGPIB(gpibioPtr dev);
void Operatehp3245a (int menubar, int menuItem, void *callbackData, int panel);
void hp3245a_UpdateReadings(int panel, void *ptr);
void hp3245a_Save(gpibioPtr dev);
void hp3245a_Load(gpibioPtr dev);
void hp3245a_Remove(void *ptr);
void hp3245a_Init(void);





/****************************************************************/
void SetHP3245aFreqLvl (sourcePtr src)
{
	gpibioPtr dev = src->acqchan->dev;
	hp3245aPtr hp = dev->device;
	char cmd[256];
	
	Fmt(cmd, "APPLY:%s %f, %f\n", hp->wave, src->biaslevel, hp->sources[AMPL]->acqchan->reading);
	gpibio_Out (dev, cmd);
    util_Delay (src->segments[src->seg]->delay);
}								 

void GetHP3245aFreqLvl (acqchanPtr acqchan)
{
	acqchan->reading = gpib_GetDoubleVal(acqchan->dev, "FREQ?");
}

void SetHP3245aAmplLvl (sourcePtr src)
{
	gpibioPtr dev = src->acqchan->dev;
	hp3245aPtr hp = dev->device;
	char cmd[256];
	
	Fmt(cmd, "APPLY %s,%f\n", hp->wave,  src->biaslevel);
	gpibio_Out (dev, cmd);
    util_Delay (src->segments[src->seg]->delay);
}

void GetHP3245aAmplLvl (acqchanPtr acqchan)
{
	acqchan->reading = gpib_GetDoubleVal(acqchan->dev, "OUTPUT?");
}

/****************************************************************/
void hp3245a_UpdateControls(int p, gpibioPtr dev)
{
	hp3245aPtr hp = dev->device;
	
	//gpib_GetCharVal(dev, "SOURCE:FUNC:SHAPE?", hp->wave);
//	Fmt("ACV", "%s[w3]", hp->wave);
	//hp->wave="ACV"
	
//	SetCtrlVal(p, HP3245A_WAVE, hp->wave);
//	hp->sources[FREQ]->min = .001;
//	if(!strcmp(hp->wave, "SIN") || !strcmp(hp->wave, "SQU"))
//		hp->sources[FREQ]->max = 1000000;
//	else
//		hp->sources[FREQ]->max = 1000000;
//	hp->sources[FREQ]->freq = 1;
//	hp->sources[AMPL]->freq = 1;
	
	//hp->sources[FREQ]->acqchan->reading = gpib_GetDoubleVal(dev, "FREQ?");
	//SetCtrlVal(p, HP3245A_FREQ, hp->sources[FREQ]->acqchan->reading);
	
//	hp->sources[AMPL]->acqchan->reading = gpib_GetDoubleVal(dev, "OUTPUT?");
//	SetCtrlVal(p, HP3245A_AMPL, hp->sources[AMPL]->acqchan->reading);
	
//	hp->duty = gpib_GetDoubleVal(dev, "SOURCE:PULSE:DCYCLE?");
//	SetCtrlVal(p, HP3245A_DUTY, hp->duty);
}

int hp3245aControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	gpibioPtr dev = callbackData;
	hp3245aPtr hp = dev->device;
	char msg[60];
	switch (control)
	{
		/*case HP3245A_DUTY:
		if(event == EVENT_COMMIT)
		{
			GetCtrlVal(panel, control, &hp->duty);
			Fmt(msg, "SOUR:PULS:DCYC %f\n", hp->duty);
			gpibio_Out(dev, msg);
		}
		break; */
		case HP3245A_WAVE:
		if(event == EVENT_COMMIT)
		{
			GetCtrlVal(panel, control, hp->wave);
			Fmt(msg, "APPLY %s\n", hp->wave);
			gpibio_Out(dev, msg);
		}
		break;
		
	}
	return 0;
}
		  
int hp3245aPanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	gpibioPtr dev = callbackData;
	if ((event == EVENT_KEYPRESS && eventData1 == VAL_ESC_VKEY) || (event == EVENT_RIGHT_DOUBLE_CLICK))
	{
		devPanel_Remove (panel);
        DiscardPanel (panel);
		dev->iPanel = 0;
		SetMenuBarAttribute (acquire_GetMenuBar(), dev->menuitem_id, ATTR_DIMMED, FALSE);
	}
	return 0;
}

void hp3245aMenuCallback (int menuBar, int menuItem, void *callbackData, int panel)
{
	switch(menuItem)
	{
		case HP3245MENU_SOURCES_FREQ:
		case HP3245MENU_SOURCES_AMPL:
			{
				sourcePtr src = callbackData;
				switch(utilG.exp)
				{
					case EXP_SOURCE: source_InitPanel(src); break;
					case EXP_FLOAT : gensrc_InitPanel(src); break;
				}
			}
			break;
	}
}

/****************************************************************/
void *hp3245a_Create(gpibioPtr dev)
{
	hp3245aPtr hp;
	hp = malloc(sizeof(hp3245aType));
	if(dev) hp->id = dev->id;
	
	hp->sources[FREQ] = source_Create("hp3245a frequency", dev, SetHP3245aFreqLvl, GetHP3245aFreqLvl);
	hp->sources[AMPL] = source_Create("hp3245a amplitude", dev, SetHP3245aAmplLvl, GetHP3245aAmplLvl);
	if(dev) dev->device = hp;
	return hp;
}

int  hp3245a_InitGPIB(gpibioPtr dev)
{
	gpibio_Remote(dev);
//	if(gpibio_DeviceMatch(dev, "*IDN?", HP3245A_ID))
		return TRUE;
//	return FALSE;
}

void Operatehp3245a (int menubar, int menuItem, void *callbackData, int panel)
{
	gpibioPtr dev = callbackData;
	hp3245aPtr hp = dev->device;
	char label[256];
	int p, m;
	
	hp->sources[FREQ]->min = .0;
	hp->sources[AMPL]->min = .00001;
	hp->sources[AMPL]->max = 10;
	
	SetMenuBarAttribute(menubar, menuItem, ATTR_DIMMED, 1);
	
	p = dev->iPanel? dev->iPanel: LoadPanel(utilG.p, "HP3245Au.uir", HP3245A);
    dev->iPanel = p;
	
	SetPanelAttribute(p, ATTR_TITLE, dev->label);
	
	m = LoadMenuBar(p, "HP3245Au.uir", HP3245MENU);
	SetPanelMenuBar(p, m);
	
	SetMenuBarAttribute(m, HP3245MENU_SOURCES_FREQ, ATTR_CALLBACK_DATA, hp->sources[FREQ]);
	SetMenuBarAttribute(m, HP3245MENU_SOURCES_AMPL, ATTR_CALLBACK_DATA, hp->sources[AMPL]);
	
	SetPanelAttribute(p, ATTR_CALLBACK_DATA, dev);
	
	SetCtrlAttribute(p, HP3245A_FREQ, ATTR_CALLBACK_DATA, dev);
	SetCtrlAttribute(p, HP3245A_AMPL, ATTR_CALLBACK_DATA, dev);
//	SetCtrlAttribute(p, HP3245A_DUTY, ATTR_CALLBACK_DATA, dev);
	SetCtrlAttribute(p, HP3245A_WAVE, ATTR_CALLBACK_DATA, dev);
	SetCtrlVal(p, HP3245A_GPIB, dev->paddr);
	
	hp3245a_UpdateControls(p, dev);
	devPanel_Add(p, dev, hp3245a_UpdateReadings);
	
	DisplayPanel(p);
}

void hp3245a_UpdateReadings(int panel, void *ptr)
{
	gpibioPtr dev = ptr;
	hp3245aPtr hp = dev->device;
	
	SetCtrlVal(panel, HP3245A_FREQ, hp->sources[FREQ]->acqchan->reading);
	SetCtrlVal(panel, HP3245A_AMPL, hp->sources[AMPL]->acqchan->reading);
}

void hp3245a_Save(gpibioPtr dev)
{
	hp3245aPtr hp;
	hp = dev->device;
	
	FmtFile (fileHandle.analysis, "Wave : %s\n", hp->wave);
	FmtFile (fileHandle.analysis, "Duty : %f\n", hp->duty);
	FmtFile (fileHandle.analysis, "Freq : %f\n", hp->sources[FREQ]->acqchan->reading);
	FmtFile (fileHandle.analysis, "Ampl : %f\n", hp->sources[AMPL]->acqchan->reading);
	source_Save(hp->sources[FREQ]);
	source_Save(hp->sources[AMPL]);
}

void hp3245a_Load(gpibioPtr dev)
{
	hp3245aPtr hp;
	char msg[256];
	double r;
	
	if(dev)
	{
		hp = dev->device;
		ScanFile(fileHandle.analysis, "Wave : %s", hp->wave);
		ScanFile(fileHandle.analysis, "Duty : %f", &r); hp->duty = r;
		ScanFile(fileHandle.analysis, "Freq : %f", &r); hp->sources[FREQ]->acqchan->reading = r;
		ScanFile(fileHandle.analysis, "Ampl : %f", &r); hp->sources[AMPL]->acqchan->reading = r;
		Fmt(msg, "APPLY:%s %f, %f\n",hp->wave, 
			hp->sources[FREQ]->acqchan->reading,
			hp->sources[AMPL]->acqchan->reading);
		gpibio_Out(dev, msg);
		Fmt(msg, "SOUR:PULS:DCYC %f\n",hp->duty); gpibio_Out(dev, msg);
		source_Load(dev, hp->sources[FREQ]);
		source_Load(dev, hp->sources[AMPL]);
		
		if(!strcmp(hp->wave, "SIN") || !strcmp(hp->wave, "SQU"))
			hp->sources[FREQ]->max = 15000000;
		else
			hp->sources[FREQ]->max = 100000;
		hp->sources[FREQ]->min = .001;
		hp->sources[AMPL]->min = .05;
		hp->sources[AMPL]->max = 10;
		hp->sources[FREQ]->freq = 1;
		hp->sources[AMPL]->freq = 1;
	}
}
	  
void hp3245a_Remove(void *ptr)
{
	hp3245aPtr hp = ptr;
	int i;
	for(i = 0; i < 2; i++)
		source_Remove(hp->sources[i]);
	free(hp);
}


void hp3245a_Init(void)
{
	devTypePtr devType;
	if(utilG.acq.status != ACQ_NONE)
	{
		util_ChangeInitMessage("hp3245a control utilities...");
		devType = malloc(sizeof(devTypeItem));
		if (devType)
		{
			Fmt(devType->label, "hewlett packard 3245a");
			Fmt(devType->id, HP3245A_ID);
			devType->CreateDevice = hp3245a_Create;
			devType->InitDevice = hp3245a_InitGPIB;
			devType->OperateDevice = Operatehp3245a;
			devType->UpdateReadings = hp3245a_UpdateReadings;
			devType->SaveDevice = hp3245a_Save;
			devType->LoadDevice = hp3245a_Load;
			devType->RemoveDevice = hp3245a_Remove;
			devTypeList_AddItem(devType);
		}
	}
}

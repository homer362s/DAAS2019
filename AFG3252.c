 //MM 12/15/17: to understand what is going on in this file, start with the function afg3252_Init
#include <userint.h>
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
#include "AFG3252.h"
#include "AFG3252init.h"


#define TRUE 1
#define FALSE 0
#define AFG3252_ID "TEKTRONIX"
/*
Specifications:
Amp: 100mV - 10V

Sin: 100uHz - 240MHz
Squ: 100uHz - 120MHz
Tri: 100uHz - 120MHz

*/
typedef enum{SIN, SQU, TRI}waveforms;

typedef enum{FREQ1, AMPL1, FREQ2, AMPL2} afg3252Ch;
typedef struct{
	sourcePtr sources[4];
	int id;
	char wave1[256];
	char wave2[256];
	double off1,off2;
}afg3252Type;

typedef afg3252Type *afg3252Ptr;

/****************************************************************/

void Setafg3252Freq1Lvl (sourcePtr src);
void Getafg3252Freq1Lvl (acqchanPtr acqchan);
void Setafg3252Ampl1Lvl (sourcePtr src);
void Getafg3252Ampl1Lvl (acqchanPtr acqchan);
void Setafg3252Freq2Lvl (sourcePtr src);
void Getafg3252Freq2Lvl (acqchanPtr acqchan);
void Setafg3252Ampl2Lvl (sourcePtr src);
void Getafg3252Ampl2Lvl (acqchanPtr acqchan);

void afg3252_UpdateControls(int p, gpibioPtr dev);
int afg3252ControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int afg3252PanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2);
void afg3252MenuCallback (int menuBar, int menuItem, void *callbackData, int panel);

void *afg3252_Create(gpibioPtr dev);
int  afg3252_InitGPIB(gpibioPtr dev);
void Operateafg3252 (int menubar, int menuItem, void *callbackData, int panel);
void afg3252_UpdateReadings(int panel, void *ptr);
void afg3252_Save(gpibioPtr dev);
void afg3252_Load(gpibioPtr dev);
void afg3252_Remove(void *ptr);
void afg3252_Init(void); 


/****************************************************************/
void SetAFG3252Freq1Lvl (sourcePtr src)
{
	gpibioPtr dev = src->acqchan->dev;
	afg3252Ptr hp = dev->device;
	char cmd[256];
	
	Fmt(cmd, "SOUR1:FREQ:FIX %f\n", src->biaslevel);
	gpibio_Out (dev, cmd);
    util_Delay (src->segments[src->seg]->delay);
}

void SetAFG3252Freq2Lvl (sourcePtr src)
{
	gpibioPtr dev = src->acqchan->dev;
	afg3252Ptr hp = dev->device;
	char cmd[256];
	
	Fmt(cmd, "SOUR2:FREQ:FIX %f\n", src->biaslevel);
	gpibio_Out (dev, cmd);
    util_Delay (src->segments[src->seg]->delay);
}

void GetAFG3252Freq1Lvl (acqchanPtr acqchan)
{
	acqchan->reading = gpib_GetDoubleVal(acqchan->dev, "SOUR1:FREQ:FIX?");
}

void GetAFG3252Freq2Lvl (acqchanPtr acqchan)
{
	acqchan->reading = gpib_GetDoubleVal(acqchan->dev, "SOUR2:FREQ:FIX?");
}

void SetAFG3252Ampl1Lvl (sourcePtr src)
{
	gpibioPtr dev = src->acqchan->dev;
	afg3252Ptr hp = dev->device;
	char cmd[256];
	
	Fmt(cmd, "SOUR1:VOLT %f\n", src->biaslevel);
	gpibio_Out (dev, cmd);
    util_Delay (src->segments[src->seg]->delay);
}

void SetAFG3252Ampl2Lvl (sourcePtr src)
{
	gpibioPtr dev = src->acqchan->dev;
	afg3252Ptr hp = dev->device;
	char cmd[256];
	
	Fmt(cmd, "SOUR2:VOLT %f\n", src->biaslevel);
	gpibio_Out (dev, cmd);
    util_Delay (src->segments[src->seg]->delay);
}

void GetAFG3252Ampl1Lvl (acqchanPtr acqchan)
{
	acqchan->reading = gpib_GetDoubleVal(acqchan->dev, "SOUR1:VOLT?");
}

void GetAFG3252Ampl2Lvl (acqchanPtr acqchan)
{
	acqchan->reading = gpib_GetDoubleVal(acqchan->dev, "SOUR2:VOLT?");
}

/****************************************************************/
void afg3252_UpdateControls(int p, gpibioPtr dev)
{
	afg3252Ptr hp = dev->device;
	
	//Update control value for WAVE1
	gpib_GetCharVal(dev, "SOUR1:FUNC:SHAP?", hp->wave1);
	Fmt(hp->wave1, "%s[w3]", hp->wave1);
	SetCtrlVal(p, AFG3252_WAVE1, hp->wave1);
	
	//Update control value for WAVE2
	gpib_GetCharVal(dev, "SOUR2:FUNC:SHAP?", hp->wave2);
	Fmt(hp->wave2, "%s[w3]", hp->wave2);
	SetCtrlVal(p, AFG3252_WAVE2, hp->wave2);
	
	//Update control value for AFG3252_OFFSET1
	hp->off1 = gpib_GetDoubleVal(dev, "SOUR1:VOLT:OFFS?");
	SetCtrlVal(p,AFG3252_OFFSET1,hp->off1);
	
	//Update control value for AFG3252_OFFSET2
	hp->off2 = gpib_GetDoubleVal(dev, "SOUR2:VOLT:OFFS?");
	SetCtrlVal(p,AFG3252_OFFSET2,hp->off2);	
	
	hp->sources[FREQ1]->min = .001;
	hp->sources[FREQ2]->min = .001;
	if(!strcmp(hp->wave1, "SIN"))
		hp->sources[FREQ1]->max = 120000000;
	else
		hp->sources[FREQ1]->max = 240000000;
	if(!strcmp(hp->wave2, "SIN"))
		hp->sources[FREQ2]->max = 120000000;
	else
		hp->sources[FREQ2]->max = 240000000;
	hp->sources[FREQ1]->freq = 1;
	hp->sources[AMPL1]->freq = 1;
	hp->sources[FREQ2]->freq = 1;
	hp->sources[AMPL2]->freq = 1;
	
	hp->sources[FREQ1]->acqchan->reading = gpib_GetDoubleVal(dev, "SOUR1:FREQ:FIX?");
	SetCtrlVal(p, AFG3252_FREQ1, hp->sources[FREQ1]->acqchan->reading);
	
	hp->sources[FREQ2]->acqchan->reading = gpib_GetDoubleVal(dev, "SOUR2:FREQ:FIX?");
	SetCtrlVal(p, AFG3252_FREQ2, hp->sources[FREQ2]->acqchan->reading);
	
	hp->sources[AMPL1]->acqchan->reading = gpib_GetDoubleVal(dev, "SOUR1:VOLT?");
	SetCtrlVal(p, AFG3252_AMPL1, hp->sources[AMPL1]->acqchan->reading);
	
	hp->sources[AMPL2]->acqchan->reading = gpib_GetDoubleVal(dev, "SOUR2:VOLT?");
	SetCtrlVal(p, AFG3252_AMPL2, hp->sources[AMPL2]->acqchan->reading);
	
}

int afg3252ControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	gpibioPtr dev = callbackData;
	afg3252Ptr hp = dev->device;
	char msg[60];
	switch (control)
	{
		case AFG3252_WAVE2:
			if (event == EVENT_COMMIT)
			{
				GetCtrlVal(panel,control,hp->wave2);
				Fmt(msg, "SOUR2:FUNC:SHAP %s\n", hp->wave2);
				gpibio_Out(dev, msg);
			}
			break;
		case AFG3252_WAVE1:
			if (event == EVENT_COMMIT)
			{
				GetCtrlVal(panel,control,hp->wave1);
				Fmt(msg, "SOUR1:FUNC:SHAP %s\n", hp->wave1);
				gpibio_Out(dev, msg);
			}
			break;
		case AFG3252_OFFSET2:
			if (event == EVENT_COMMIT)
			{
				GetCtrlVal(panel,control,&hp->off2);
				Fmt(msg, "SOUR2:VOLT:OFFS %fV\n", hp->off2);
				gpibio_Out(dev, msg);
			}
			break;
		case AFG3252_OFFSET1:
			if (event == EVENT_COMMIT)
			{
				GetCtrlVal(panel,control,&hp->off1);
				Fmt(msg, "SOUR1:VOLT:OFFS %fV\n", hp->off1);
				gpibio_Out(dev, msg);
			}
			break;
		
	}
	return 0;
}
		  
int afg3252PanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2)
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

void afg3252MenuCallback (int menuBar, int menuItem, void *callbackData, int panel)
{
	switch(menuItem)
	{
		case AFG325MENU_SOURCES_CHAN1_AMPL1:
		case AFG325MENU_SOURCES_CHAN1_FREQ1: 
		case AFG325MENU_SOURCES_CHAN2_AMPL2:   
		case AFG325MENU_SOURCES_CHAN2_FREQ2:
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
void *afg3252_Create(gpibioPtr dev)
{
	afg3252Ptr hp;
	hp = malloc(sizeof(afg3252Type));
	if(dev) hp->id = dev->id;
	
	hp->sources[FREQ1] = source_Create("afg3252 ch1 freq", dev, SetAFG3252Freq1Lvl, GetAFG3252Freq1Lvl);
	hp->sources[FREQ2] = source_Create("afg3252 ch2 freq", dev, SetAFG3252Freq2Lvl, GetAFG3252Freq2Lvl);
	hp->sources[AMPL1] = source_Create("afg3252 ch1 ampl", dev, SetAFG3252Ampl1Lvl, GetAFG3252Ampl1Lvl);
	hp->sources[AMPL2] = source_Create("afg3252 ch2 ampl", dev, SetAFG3252Ampl2Lvl, GetAFG3252Ampl2Lvl);
	if(dev) dev->device = hp;
	return hp;
}

int  afg3252_InitGPIB(gpibioPtr dev)
{
	gpibio_Remote(dev);
	if(gpibio_DeviceMatch(dev, "*IDN?", AFG3252_ID))
		return TRUE;
	return FALSE;
}

void Operateafg3252 (int menubar, int menuItem, void *callbackData, int panel)
{
	gpibioPtr dev = callbackData;
	afg3252Ptr hp = dev->device;
	char label[256];
	int p, m;
	
	hp->sources[FREQ1]->min = .001;
	hp->sources[FREQ2]->min = .001;
	hp->sources[AMPL1]->min = .05;
	hp->sources[AMPL1]->max = 10;
	hp->sources[AMPL2]->min = .05;
	hp->sources[AMPL2]->max = 10;
	
	SetMenuBarAttribute(menubar, menuItem, ATTR_DIMMED, 1);
	
	p = dev->iPanel? dev->iPanel: LoadPanel(utilG.p, "AFG3252.uir", AFG3252);
    dev->iPanel = p;
	
	SetPanelAttribute(p, ATTR_TITLE, dev->label);
	
	m = LoadMenuBar(p, "AFG3252.uir", AFG325MENU);
	SetPanelMenuBar(p, m);
	
	SetMenuBarAttribute(m, AFG325MENU_SOURCES_CHAN1_AMPL1, ATTR_CALLBACK_DATA, hp->sources[AMPL1]);
	SetMenuBarAttribute(m, AFG325MENU_SOURCES_CHAN1_FREQ1, ATTR_CALLBACK_DATA, hp->sources[FREQ1]);
	SetMenuBarAttribute(m, AFG325MENU_SOURCES_CHAN2_AMPL2, ATTR_CALLBACK_DATA, hp->sources[AMPL2]);
	SetMenuBarAttribute(m, AFG325MENU_SOURCES_CHAN2_FREQ2, ATTR_CALLBACK_DATA, hp->sources[FREQ2]);
	
	SetPanelAttribute(p, ATTR_CALLBACK_DATA, dev);
	
	SetCtrlAttribute(p, AFG3252_FREQ2, ATTR_CALLBACK_DATA, dev);
	SetCtrlAttribute(p, AFG3252_FREQ1, ATTR_CALLBACK_DATA, dev);
	SetCtrlAttribute(p, AFG3252_AMPL2, ATTR_CALLBACK_DATA, dev);
	SetCtrlAttribute(p, AFG3252_AMPL1, ATTR_CALLBACK_DATA, dev);
	SetCtrlAttribute(p, AFG3252_WAVE2, ATTR_CALLBACK_DATA, dev);
	SetCtrlAttribute(p, AFG3252_WAVE1, ATTR_CALLBACK_DATA, dev);
	SetCtrlAttribute(p, AFG3252_OFFSET2, ATTR_CALLBACK_DATA, dev);
	SetCtrlAttribute(p, AFG3252_OFFSET1, ATTR_CALLBACK_DATA, dev); 
	SetCtrlVal(p, AFG3252_GPIB, dev->paddr);
	
	afg3252_UpdateControls(p, dev);
	devPanel_Add(p, dev, afg3252_UpdateReadings);
	
	DisplayPanel(p);
}

void afg3252_UpdateReadings(int panel, void *ptr)
{
	gpibioPtr dev = ptr;
	afg3252Ptr hp = dev->device;
	
	SetCtrlVal(panel, AFG3252_FREQ1, hp->sources[FREQ1]->acqchan->reading);
	SetCtrlVal(panel, AFG3252_AMPL1, hp->sources[AMPL1]->acqchan->reading);
	SetCtrlVal(panel, AFG3252_FREQ2, hp->sources[FREQ2]->acqchan->reading);
	SetCtrlVal(panel, AFG3252_AMPL2, hp->sources[AMPL2]->acqchan->reading);
}

void afg3252_Save(gpibioPtr dev)
{
	afg3252Ptr hp;
	hp = dev->device;
	
	FmtFile (fileHandle.analysis, "Wave1 : %s\n", hp->wave1);
	FmtFile (fileHandle.analysis, "Wave2 : %s\n", hp->wave2);
	FmtFile (fileHandle.analysis, "Freq1 : %f\n", hp->sources[FREQ1]->acqchan->reading);
	FmtFile (fileHandle.analysis, "Freq2 : %f\n", hp->sources[FREQ2]->acqchan->reading);
	FmtFile (fileHandle.analysis, "Ampl1 : %f\n", hp->sources[AMPL1]->acqchan->reading);
	FmtFile (fileHandle.analysis, "Ampl2 : %f\n", hp->sources[AMPL2]->acqchan->reading);
	FmtFile (fileHandle.analysis, "Offs1 : %f\n", hp->off1);
	FmtFile (fileHandle.analysis, "Offs2 : %f\n", hp->off2);
	source_Save(hp->sources[FREQ1]);
	source_Save(hp->sources[AMPL1]);
	source_Save(hp->sources[FREQ2]);
	source_Save(hp->sources[AMPL2]);
}

void afg3252_Load(gpibioPtr dev)
{
	afg3252Ptr hp;
	char msg[256];
	double r;
	
	if(dev)
	{
		hp = dev->device;
		ScanFile(fileHandle.analysis, "Wave1 : %s", hp->wave1);
		ScanFile(fileHandle.analysis, "Wave2 : %s", hp->wave2);
		ScanFile(fileHandle.analysis, "Freq1 : %f", &r); hp->sources[FREQ1]->acqchan->reading = r;
		ScanFile(fileHandle.analysis, "Ampl1 : %f", &r); hp->sources[AMPL1]->acqchan->reading = r;
		ScanFile(fileHandle.analysis, "Freq2 : %f", &r); hp->sources[FREQ2]->acqchan->reading = r;
		ScanFile(fileHandle.analysis, "Ampl2 : %f", &r); hp->sources[AMPL2]->acqchan->reading = r;
		SetAFG3252Freq1Lvl(hp->sources[FREQ1]);
		SetAFG3252Freq2Lvl(hp->sources[FREQ2]);
		SetAFG3252Ampl1Lvl(hp->sources[AMPL1]);
		SetAFG3252Ampl2Lvl(hp->sources[AMPL2]);
		source_Load(dev, hp->sources[FREQ1]);
		source_Load(dev, hp->sources[AMPL1]);
		source_Load(dev, hp->sources[FREQ2]);
		source_Load(dev, hp->sources[AMPL2]);
		
		if(!strcmp(hp->wave1, "SIN"))
			hp->sources[FREQ1]->max = 120000000;
		else
			hp->sources[FREQ1]->max = 240000000;
		if(!strcmp(hp->wave2, "SIN"))
			hp->sources[FREQ2]->max = 120000000;
		else
			hp->sources[FREQ2]->max = 240000000;
		
		hp->sources[FREQ1]->min = .001;
		hp->sources[FREQ2]->min = .001;
		hp->sources[AMPL1]->min = .05;
		hp->sources[AMPL1]->max = 10;
		hp->sources[AMPL2]->min = .05;
		hp->sources[AMPL2]->max = 10;
		hp->sources[FREQ1]->freq = 1;
		hp->sources[AMPL1]->freq = 1;
		hp->sources[FREQ2]->freq = 1;
		hp->sources[AMPL2]->freq = 1;
	}
}
	  
void afg3252_Remove(void *ptr)
{
	afg3252Ptr hp = ptr;
	int i;
	for(i = 0; i < 2; i++)
		source_Remove(hp->sources[i]);
	free(hp);
}


//MM 12/15/17: for future users, this is where you can start to make sense of this whole file
//Each gpio device is defined by a devTypePtr to a devTypeItem
//The devTypeItem has the fields and pointers listed below for the instance devType
//The function pointer OperateDevice points to a function repeatedly executed to update the UI controls and the actual device
void afg3252_Init(void)
{
	devTypePtr devType;
	if(utilG.acq.status != ACQ_NONE)
	{
		util_ChangeInitMessage("AFG3252 control utilities...");
		devType = malloc(sizeof(devTypeItem));
		if (devType)
		{
			Fmt(devType->label, "Tektoronix AFG3252");
			Fmt(devType->id, AFG3252_ID);
			devType->CreateDevice = afg3252_Create;
			devType->InitDevice = afg3252_InitGPIB;
			devType->OperateDevice = Operateafg3252;
			devType->UpdateReadings = afg3252_UpdateReadings;
			devType->SaveDevice = afg3252_Save;
			devType->LoadDevice = afg3252_Load;
			devType->RemoveDevice = afg3252_Remove;
			devTypeList_AddItem(devType);
		}
	}
}

#include <utility.h>
#include <formatio.h>
#include <ansi_c.h>
#include <userint.h>   
#include <time.h>

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

#include "k2400.h"
#include "k2400u.h"

#define K2400_ID "KEITHLEY INSTRUMENTS INC.,MODEL 2400"
//"0679385"

#define K2400_SRE_SRQDISABLED 0
#define K2400_SRE_WARNING 1
#define K2400_SRE_SWEEPDONE 2
#define K2400_SRE_TRIGGEROUT 4
#define K2400_SRE_READINGDONE 8
#define K2400_SRE_READYFORTRIGGER 16
#define K2400_SRE_ERROR 32
#define K2400_SRE_COMPLIANCE 128

#define TRUE 1
#define FALSE 0

typedef struct {
    acqchanPtr measure;
    sourcePtr source;
    int id, overLimit, sourceOn;
    int operate, sense;
    struct {int delay, current, rangeV, rangeC;} src;
    struct {int range, filter; double inttime;} meas;
}   k2400Type;

typedef k2400Type *k2400Ptr;

int flag = 0;
time_t past, present;
double delay = 1;

static void k2400_GetErrorStatus (gpibioPtr dev);
static void k2400_GetWarningStatus (gpibioPtr dev);

static void k2400_In (gpibioPtr dev, char *msg);
static void k2400_Out (gpibioPtr dev, char *cmd, double delay);
static void k2400_CheckforProblems (gpibioPtr dev);
static void k2400_GetLevels (gpibioPtr dev);
static void k2400_GetReadings (acqchanPtr acqchan);
static void k2400_SetLevel (sourcePtr src);

static void k2400_GetMachineStatus (gpibioPtr dev);
static void k2400_GetMeasurementParameters (gpibioPtr dev);
static double k2400_GetCompLevel (gpibioPtr dev);
static void k2400_Save (gpibioPtr dev);
static void k2400_Load (gpibioPtr dev);
static void k2400_Remove (void *ptr);

static void k2400_UpdateControls (int panel, gpibioPtr dev);

static void k2400_Meas_UpdateReadings (int panel, void *smu);

static int  k2400_InitGPIB (gpibioPtr dev);
static void *k2400_Create (gpibioPtr dev);
static void OperateK2400 (int menubar, int menuItem, void *callbackData, int panel);
static void k2400_UpdateReadings (int panel, void *dev);

int k2400_GetMode(gpibioPtr);

void k2400_Init (void)
{
    devTypePtr devType;
    if (utilG.acq.status != ACQ_NONE) {
        util_ChangeInitMessage ("Keithley 2400 Control Utilities...");
        devType = malloc (sizeof (devTypeItem));
        if (devType) {
            Fmt (devType->label, "Keithley 2400 SMU");
            Fmt (devType->id, K2400_ID);
            devType->InitDevice = k2400_InitGPIB;
            devType->CreateDevice = k2400_Create;
            devType->OperateDevice = OperateK2400;
            devType->UpdateReadings = k2400_UpdateReadings;
            devType->SaveDevice = k2400_Save;
            devType->LoadDevice = k2400_Load;
            devType->RemoveDevice = k2400_Remove;
            devTypeList_AddItem (devType);
        }
    }
	time(&past);
}

static void k2400_UpdateReadings (int panel, void *dev)
{
    gpibioPtr my_dev = dev;
    k2400Ptr smu = my_dev->device;
	acqchanPtr acqchan = smu->source->acqchan;
    short statusbyte;
    char rsp[256];
    int control, dim, bg, mode, m;

	if (smu->operate) {
        if (utilG.acq.status == ACQ_BUSY)
			Delay(.05);
		if (!util_TakingData() || !(smu->source->acqchan->acquire || smu->measure->acquire)) {
            gpibio_GetStatusByte (dev, &statusbyte);
            if (statusbyte & K2400_SRE_READINGDONE) 
			{
				//k2400_In (dev, rsp);
                //Scan (rsp, "%s>%f,%f", &smu->source->acqchan->reading,
                //      &smu->measure->reading); 
            }
            //if (statusbyte & K2400_SRE_READYFORTRIGGER) 
				//k2400_Out (dev, "H0X", .02);
        }
		
		time(&present);
		if(difftime(present, past) >= delay)
		{
			k2400_GetReadings(acqchan);
			time(&past);
		}
        //if (expG.acqstatus != utilG.acq.status) 
		{
            m = GetPanelMenuBar (panel);
            dim = (util_TakingData() && smu->source->inlist &&
                   (utilG.exp == EXP_SOURCE));

            if (dim) { mode = VAL_INDICATOR; bg = VAL_PANEL_GRAY;}
                else { mode = VAL_HOT; bg = VAL_WHITE;}

            SetCtrlAttribute (panel, K2400_SOURCE, ATTR_CTRL_MODE, mode);
            SetCtrlAttribute (panel, K2400_SOURCE, ATTR_TEXT_BGCOLOR, bg);

            SetInputMode (panel, K2400_OPERATE, !dim);
            SetInputMode (panel, K2400_SELECT, !dim);
            SetInputMode (panel, K2400_SENSE, !dim);
            SetMenuBarAttribute (m, K2400MENUS_FILE_LOAD, ATTR_DIMMED, dim);
        }

        k2400_CheckforProblems (my_dev);
         SetCtrlVal (panel, K2400_OVERLIMIT, smu->overLimit);

        control = GetActiveCtrl (panel);
        if (util_TakingData() || (control != K2400_SOURCE))
            SetCtrlVal (panel, K2400_SOURCE, smu->source->acqchan->reading);
        SetCtrlVal (panel, K2400_MEASURE, smu->measure->reading);
    }
}

static void *k2400_Create (gpibioPtr dev)
{
	char msg[256];
    k2400Ptr smu;

    smu = malloc (sizeof(k2400Type));
    smu->overLimit = FALSE;
    if (dev) smu->id = dev->id;
    smu->src.rangeC = 0; 
	smu->src.rangeV = 0;/* AUTO */
    smu->source = source_Create ("K2400 Source", dev, k2400_SetLevel,
                                 k2400_GetReadings);
    smu->measure = acqchan_Create ("K2400 Measure", dev, k2400_GetReadings);
	k2400_Out(dev, ":OUTPut:STATe?", 0);
	k2400_In(dev, msg);
	Scan(msg, "%i", &smu->source->sourceOn);
    if (dev) dev->device = smu;
    return smu;
}

static int k2400_InitGPIB (gpibioPtr dev)
{
    char cmd[256], rsp[256], buffer[256];
    int result;

    gpibio_Remote (dev); 
    
    if (gpibio_DeviceMatch (dev, "*IDN?", K2400_ID))
	{
        return TRUE;
	}
    return FALSE;
}

void LoadK2400Callback(int menubar, int menuItem, void *callbackData, int panel)
{
    int fileselect, id;
    char path[512], info[512];
    gpibioPtr dev = callbackData;
    k2400Ptr smu = dev->device;

    fileselect = FileSelectPopup ("", "*.dev", "*.dev", "Load Keithley 2400 SMU Setup",
                                  VAL_LOAD_BUTTON, 0, 1, 1, 0, path);
    if (fileselect == VAL_EXISTING_FILE_SELECTED) {
        fileHandle.analysis = util_OpenFile (path, FILE_READ, FALSE);
        ScanFile (fileHandle.analysis, "%s>#INSTRSETUP %i", &id);
        if (smu->id == id) {
            k2400_Load (dev);
            ReadLine (fileHandle.analysis, info, 255);
            k2400_UpdateControls(panel, dev);
        }
        else MessagePopup ("Keithley Load Message", "Different instrument types--process aborted");
        util_CloseFile();
    }
}

void SaveK2400Callback(int menubar, int menuItem, void *callbackData, int panel)
{
    int fileselect;
    char path[512];
    gpibioPtr dev = callbackData;
    k2400Ptr smu = dev->device;
    fileselect = FileSelectPopup ("", "*.dev", "*.dev", "Save K2400 Setup",
                                  VAL_SAVE_BUTTON, 0, 1, 1, 1, path);
    if (fileselect == VAL_NEW_FILE_SELECTED) {
        fileHandle.analysis = util_OpenFile (path, FILE_WRITE, FALSE);
        FmtFile (fileHandle.analysis, "%s<#INSTRSETUP %i\0", smu->id);
        k2400_Save(dev);
        FmtFile (fileHandle.analysis, "#ENDSETUP\0");
        util_CloseFile();
    }
}

int  K2400MeasControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    acqchanPtr acqchan = callbackData;
    switch (control) {
        case K2400_MEAS_ACQ:
            if (event == EVENT_VAL_CHANGED) {
                GetCtrlVal (panel, control, &acqchan->acquire);
                if (acqchan->acquire) acqchanlist_AddChannel (acqchan);
                    else acqchanlist_RemoveChannel (acqchan);
            }
            break;
        case K2400_MEAS_COEFF:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &acqchan->coeff);
                if (acqchan->p) SetCtrlVal (acqchan->p, ACQDATA_COEFF, acqchan->coeff);
            }
            break;
        case K2400_MEAS_LABEL:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, acqchan->channel->label);
                acqchanlist_ReplaceChannel (acqchan);
                if (acqchan->p) SetPanelAttribute (acqchan->p, ATTR_TITLE, acqchan->channel->label);
            }
            break;
        case K2400_MEAS_CLOSE:
            if (event == EVENT_COMMIT) {
                devPanel_Remove(panel);
                
                DiscardPanel (panel);
            }
            break;
    }
    return 0;
}

static void k2400_Meas_UpdateReadings (int panel, void *measchan)
{
    acqchanPtr measure = measchan;

    SetCtrlVal (panel, K2400_MEAS_MEAS, measure->reading);
}

void K2400MeasureSetupCallback(int menubar, int menuItem, void *callbackData, int panel)
{
    int p;
    acqchanPtr measure = callbackData;

    p = LoadPanel (0, "k2400u.uir", K2400_MEAS);
    SetPanelPos (p, VAL_AUTO_CENTER, VAL_AUTO_CENTER);
    util_InitClose (p, K2400_MEAS_CLOSE, FALSE);

    SetCtrlVal (p, K2400_MEAS_LABEL, measure->channel->label);
    SetCtrlVal (p, K2400_MEAS_COEFF, measure->coeff);
    SetCtrlVal (p, K2400_MEAS_ACQ, measure->acquire);
    SetCtrlVal (p, K2400_MEAS_NOTE, measure->note);

    SetCtrlAttribute(p, K2400_MEAS_LABEL, ATTR_CALLBACK_DATA, measure);
    SetCtrlAttribute(p, K2400_MEAS_COEFF, ATTR_CALLBACK_DATA, measure);
    SetCtrlAttribute(p, K2400_MEAS_ACQ, ATTR_CALLBACK_DATA, measure);
    SetCtrlAttribute(p, K2400_MEAS_NOTE, ATTR_CALLBACK_DATA, measure);

    SetInputMode (p, K2400_MEAS_ACQ, !util_TakingData());

    devPanel_Add (p, measure, k2400_Meas_UpdateReadings);
    InstallPopup (p);
}

void K2400SourceSetupCallback(int menubar, int menuItem, void *callbackData, int panel)
{
	sourcePtr src = callbackData; 
	if(src->sourceOn)
	{
		int p;
		gpibioPtr dev = src->acqchan->dev;
		switch (utilG.exp) {
        case EXP_SOURCE: source_InitPanel (src); break;
        case EXP_FLOAT: gensrc_InitPanel (src); break;
    	} 
		src->sourceOn = 1;
	}
}

int  K2400SourceCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    sourcePtr src = callbackData;

    if (event == EVENT_COMMIT) {
        GetCtrlVal (panel, control, &src->biaslevel );
        k2400_SetLevel (src);
    }
    return 0;
}

int  K2400MeasureCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	return 0;
}

int  K2400ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    gpibioPtr dev = callbackData;
    k2400Ptr smu = dev->device;
	sourcePtr src = smu->source;
    char cmd[256];
    double r, level;
    int id;

    switch (control) {
        case K2400_INTTIME:		  
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &smu->meas.inttime);
                Fmt (cmd, "SENSe:CURRent:NPLCycles %f", smu->meas.inttime);
				//sends same value to VOLTage
            }
            break;
        case K2400_FILTER:		 
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &smu->meas.filter);
                Fmt (cmd, "SENSe:AVERage:COUNt %i", smu->meas.filter);
                k2400_Out (dev, cmd, .02);
            }
            break;
        case K2400_MEAS_V_RANGE:	//TODO
        case K2400_MEAS_I_RANGE:
            /*if (event == EVENT_COMMIT) {
                GetCtrlIndex (panel, control, &smu->meas.range);
                GetCtrlVal (panel, control, &r);
                level = k2400_GetCompLevel(dev);
                if (r < level) level = r;
				SetMeasRange();
            } */ 
            break;
        case K2400_LEVEL:	  
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &r);
				if(smu->src.current)
					Fmt(cmd,":SENSe:VOLTage:PROTection:LEVel %f",r);
				else
                	Fmt(cmd,":SENSe:CURRent:PROTection:LEVel %f",r); /* trigger level change */
                k2400_Out (dev, cmd, .04);
            }
            break;
        case K2400_SENSE: // 0 = disable remote sense, 1 = enable remote sense
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &smu->sense);
				Fmt (cmd, ":SYSTem:RSENse %i", smu->sense);
                k2400_Out (dev, cmd, .02);
            }
            break;
        case K2400_SELECT:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &smu->src.current);
				if(smu->src.current)
				{
					k2400_Out(dev, ":SOURce:FUNCtion:MODE CURRent", .07); 
					k2400_Out(dev, ":SENSe:FUNCtion \"VOLTage\"", .07); 
				}
				else
				{
					k2400_Out(dev, ":SOURce:FUNCtion:MODE VOLTage", .07); 
					k2400_Out(dev, ":SENSe:FUNCtion \"CURRent\"", .07); 
				}
				smu->source->sourceOn = 0;
            }
            break;
        case K2400_OPERATE:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &smu->operate);
                Fmt (cmd, ":OUTPut:STATe %i", smu->operate);
                k2400_Out (dev, cmd, .02);
				smu->source->sourceOn = !smu->source->sourceOn;
            }
            break;
        case K2400_IDENTIFY:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &id);
				if(id)
				{
					k2400_Out(dev, ":DISPlay:WINDow:TEXT:STATe 1", .015);
					Fmt(cmd, ":DISPlay:WINDow:TEXT:DATA \"%i\"", dev->paddr);
					k2400_Out(dev, cmd, .5);
				}
				else
					k2400_Out(dev, ":DISPlay:WINDow:TEXT:STATe 0", .015);
            }
            break;
        case K2400_DELAY:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &smu->src.delay);
                Fmt (cmd, ":SOURce:DELay:AUTO %i", smu->src.delay);
                k2400_Out (dev, cmd, .02);
            }
            break;
        case K2400_SOURCE_V_RANGE:
        case K2400_SOURCE_I_RANGE:
            if (event == EVENT_COMMIT) {
                GetCtrlVal(panel, K2400_SOURCE, &level);
				GetCtrlVal(panel, control, &r);
                if (r < level) level = r;
				if(smu->src.current)
				{
					GetCtrlIndex(panel, control, &smu->src.rangeC);
					Fmt(cmd, ":SOURce:CURRent:RANGe %f", r);
				}
				else
				{
					GetCtrlIndex(panel, control, &smu->src.rangeV);
					Fmt(cmd, ":SOURce:VOLTage:RANGe %f", r);
				}
				k2400_Out (dev, cmd, .02); 
				src->biaslevel = level;
				k2400_SetLevel(src); 
                k2400_Out (dev, cmd, .02);
                smu->source->min = -r;
                smu->source->max = r;
				if(src->panel)
					source_UpdatePanel(src->panel, src);
            }
            break;
    }

    if (event == EVENT_COMMIT) {
        k2400_UpdateControls(panel, dev);
    } //*/
    return 0;
}


int  K2400ControlPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2)
{
    gpibioPtr dev = callbackData;
    k2400Ptr smu = dev->device;
    int menubar;

    if ((event == EVENT_KEYPRESS && eventData1 == VAL_ESC_VKEY) || event == EVENT_RIGHT_DOUBLE_CLICK) {
        devPanel_Remove (panel);
		DiscardPanel (panel);
		dev->iPanel = 0;
		SetMenuBarAttribute (acquire_GetMenuBar(), dev->menuitem_id, ATTR_DIMMED, FALSE);
    }

    if (event == EVENT_GOT_FOCUS) {
        menubar = GetPanelMenuBar (panel);
        SetPanelAttribute (panel, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, K2400MENUS_FILE_SAVE, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, K2400MENUS_FILE_LOAD, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, K2400MENUS_SOURCE, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, K2400MENUS_MEASURE, ATTR_DIMMED, (dev->status != DEV_REMOTE));

        if (!util_TakingData()) k2400_UpdateControls (panel, dev);
    }
    return 0;
}

static void OperateK2400 (int menubar, int menuItem, void *callbackData, int panel)
{
    int p, m;
    gpibioPtr dev = callbackData;
    k2400Ptr smu = dev->device;
    char label[256];

    SetMenuBarAttribute (menubar, menuItem, ATTR_DIMMED, TRUE);

    p = dev->iPanel? dev->iPanel:LoadPanel (utilG.p, "k2400u.uir", K2400);
	dev->iPanel = p;
    
    SetPanelPos (p, VAL_AUTO_CENTER, VAL_AUTO_CENTER);

    Fmt (label, "Keithley 2400 Source Measure Unit: %s", dev->label);
    SetPanelAttribute (p, ATTR_TITLE, label);

    m = LoadMenuBar (p, "k2400u.uir", K2400MENUS);
    
    SetPanelMenuBar (p, m);

    SetMenuBarAttribute (m, K2400MENUS_FILE_SAVE, ATTR_CALLBACK_DATA, dev);
    SetMenuBarAttribute (m, K2400MENUS_FILE_LOAD, ATTR_CALLBACK_DATA, dev);
    SetMenuBarAttribute (m, K2400MENUS_FILE_GPIB, ATTR_CALLBACK_DATA, dev);
    SetMenuBarAttribute (m, K2400MENUS_SOURCE, ATTR_CALLBACK_DATA, smu->source);
    SetMenuBarAttribute (m, K2400MENUS_MEASURE, ATTR_CALLBACK_DATA, smu->measure);

    SetPanelAttribute (p, ATTR_CALLBACK_DATA, dev);

    SetCtrlAttribute (p, K2400_SOURCE, ATTR_CALLBACK_DATA, smu->source);

    SetCtrlAttribute (p, K2400_SOURCE_I_RANGE, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K2400_SOURCE_V_RANGE, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K2400_DELAY, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K2400_IDENTIFY, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K2400_OPERATE, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K2400_SELECT, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K2400_SENSE, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K2400_MEAS_I_RANGE, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K2400_MEAS_V_RANGE, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K2400_LEVEL, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K2400_FILTER, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K2400_INTTIME, ATTR_CALLBACK_DATA, dev);

    k2400_UpdateControls (p, dev);

    devPanel_Add (p, dev, k2400_UpdateReadings);
    DisplayPanel (p);
}

static void k2400_UpdateControls (int panel, gpibioPtr dev)
{
    char s_text[20], m_text[20];
    k2400Ptr smu = dev->device;
    double absmax;
	
    k2400_GetMachineStatus (dev);
    k2400_GetMeasurementParameters (dev);
	//k2400_GetReadings(smu->source->acqchan);

    SetCtrlVal (panel, K2400_SOURCE, smu->source->acqchan->reading);
    SetCtrlVal (panel, K2400_MEASURE, smu->measure->reading);

    SetCtrlVal (panel, K2400_OPERATE, smu->operate);
    SetInputMode (panel, K2400_SOURCE, smu->operate);
    SetInputMode (panel, K2400_MEASURE, smu->operate);

    SetCtrlVal (panel, K2400_SELECT, smu->src.current);
    SetCtrlVal (panel, K2400_SENSE, smu->sense);

    if (smu->src.current) {
        Fmt (s_text, "A"); Fmt (m_text, "V");
    } else {
        Fmt (s_text, "V"); Fmt (m_text, "A");
    }

    SetCtrlVal (panel, K2400_SOURCETEXT, s_text);
    SetCtrlVal (panel, K2400_MEASURETEXT, m_text);

    SetCtrlAttribute (panel, K2400_SOURCE_I_RANGE, ATTR_VISIBLE, smu->src.current);
    SetCtrlAttribute (panel, K2400_SOURCE_V_RANGE, ATTR_VISIBLE, !smu->src.current);

    if (smu->src.current) {
        SetCtrlIndex (panel, K2400_SOURCE_I_RANGE, smu->src.rangeC);
        GetCtrlVal (panel, K2400_SOURCE_I_RANGE, &smu->source->max);
    } else {
        SetCtrlIndex (panel, K2400_SOURCE_V_RANGE, smu->src.rangeV);
        GetCtrlVal (panel, K2400_SOURCE_V_RANGE, &smu->source->max);
    }

    smu->source->min = -smu->source->max;
    SetCtrlAttribute (panel, K2400_SOURCE, ATTR_MIN_VALUE, smu->source->min);
    SetCtrlAttribute (panel, K2400_SOURCE, ATTR_MAX_VALUE, smu->source->max);
    SetCtrlVal (panel, K2400_DELAY, smu->src.delay);

    SetCtrlAttribute (panel, K2400_MEAS_I_RANGE, ATTR_VISIBLE, !smu->src.current);
    SetCtrlAttribute (panel, K2400_MEAS_V_RANGE, ATTR_VISIBLE, smu->src.current);

    SetCtrlVal (panel, K2400_LEVEL, k2400_GetCompLevel (dev));

    if (smu->src.current) {
        SetCtrlIndex (panel, K2400_MEAS_V_RANGE, smu->meas.range);
        GetCtrlVal (panel, K2400_MEAS_V_RANGE, &absmax);
    } else {
        SetCtrlIndex (panel, K2400_MEAS_I_RANGE, smu->meas.range);
        GetCtrlVal (panel, K2400_MEAS_I_RANGE, &absmax);
    }

    SetCtrlAttribute (panel, K2400_LEVEL, ATTR_MIN_VALUE, -absmax);
    SetCtrlAttribute (panel, K2400_LEVEL, ATTR_MAX_VALUE, absmax);

    SetCtrlVal (panel, K2400_FILTER, smu->meas.filter);
    SetCtrlVal (panel, K2400_INTTIME, smu->meas.inttime);

}

static void k2400_Remove (void *ptr)
{
    k2400Ptr smu = ptr;
    acqchan_Remove(smu->measure);
    source_Remove (smu->source);
    free (smu);
}

static void k2400_Load (gpibioPtr dev)
{
    char cmd[256];
    double r;
    int i;
    k2400Ptr smu;
	sourcePtr src;

    if (dev){
		smu = dev->device;
		src = smu->source;
	}
    else smu = k2400_Create (NULL);

/*  if (dev) k2400_Out (dev, "N0X");*/

    ScanFile (fileHandle.analysis, "%s>Sense   : %i%s[dw1]", &smu->sense);
    if (dev) {Fmt (cmd, ":SYSTem:RSENse %i", smu->sense); k2400_Out (dev, cmd, 0);}

    ScanFile (fileHandle.analysis, "%s>Source  : %i, %i%s[dw1]",
              &smu->src.current, &smu->src.delay);
    if (dev) {
		if(smu->src.current)
		{
			k2400_Out(dev, ":SOURce:FUNCtion:MODE CURRent", .07); 
			k2400_Out(dev, ":SENSe:FUNCtion \"VOLTage\"", .07); 
		}
		else
		{
			k2400_Out(dev, ":SOURce:FUNCtion:MODE VOLTage", .07); 
			k2400_Out(dev, ":SENSe:FUNCtion \"CURRent\"", .07); 
		}
		smu->source->sourceOn = 0;
        Fmt (cmd, ":SOURce:DELay:AUTO %i", smu->src.delay);
        k2400_Out (dev, cmd, 0);
    }
    smu->src.rangeV = 0;
	smu->src.rangeC = 0;/*AUTO*/
    smu->source->min = -110.0;
    smu->source->max = 110.0;
	k2400_Out(dev,":OUTPut:STATe?",0);
	k2400_In(dev,cmd);
	Scan(cmd, "%i", &smu->source->sourceOn);
    source_Load (dev, smu->source);

    ScanFile (fileHandle.analysis, "%s>Measure : %f, %i, %i, %f\0",
             &r, &smu->meas.range, &smu->meas.filter, &smu->meas.inttime);
    if (dev) {
		Fmt (cmd, "SENSe:CURRent:NPLCycles %f", smu->meas.inttime);
		k2400_Out (dev, cmd, 0);
		Fmt (cmd, "SENSe:AVERage:COUNt %i", smu->meas.filter);
        k2400_Out (dev, cmd, 0);
    }

    acqchan_Load (dev, smu->measure);

    if (!dev) k2400_Remove (smu);
}

static void k2400_Save (gpibioPtr dev)
{
    k2400Ptr smu = dev->device;

    k2400_GetMachineStatus(dev);
    k2400_GetMeasurementParameters(dev);

    FmtFile (fileHandle.analysis, "Sense   : %i\n", smu->sense);
    FmtFile (fileHandle.analysis, "Source  : %i, %i\n", smu->src.current, smu->src.delay);
    source_Save (smu->source);
    FmtFile (fileHandle.analysis, "Measure : %f[e2p5], %i, %i, %f\n",
             k2400_GetCompLevel (dev), smu->meas.range,
             smu->meas.filter, smu->meas.inttime);
    acqchan_Save (smu->measure);
}


static double k2400_GetCompLevel (gpibioPtr dev)
{
    char msg[256];
    double level;
	
	k2400Ptr smu = dev->device;
	
	if (smu->src.current)
		k2400_Out (dev, ":SENSe:VOLTage:PROTection:LEVel?", .008); 
	else
    	k2400_Out (dev, ":SENSe:CURRent:PROTection:LEVel?", .008);
    k2400_In (dev, msg);

    Scan (msg, "%s>%f", &level);
    return level;
}

static void k2400_GetMeasurementParameters (gpibioPtr dev)
{
    char msg[256];
    int srctype;
    k2400Ptr smu = dev->device;
	/*Check Source mode*/
    k2400_Out(dev, ":SOURce:FUNCtion:MODE?", .008);
    k2400_In(dev, msg);
	if(msg[0] == 'V')
		smu->src.current = 0;
	else
		smu->src.current = 1;
	
	/*Check for remote/local sense*/
	k2400_Out(dev, ":SYStem:RSENse?", .008);
	k2400_In(dev, msg);
	Scan(msg, "%i", &smu->sense);
	
	/*Check Filter steps*/
	k2400_Out(dev, ":SENSe:AVERage:COUNt?", .008);
	k2400_In(dev, msg);
	Scan(msg, "%i", &smu->meas.filter);
	
	/*Check Integration time*/
	k2400_Out(dev, ":SENSe:CURRent:NPLCycles?", .008);
	k2400_In(dev, msg);
	Scan(msg, "%f", &smu->meas.inttime);
	
	/*Check Delay time*/
	k2400_Out(dev, ":SOURce:DELay:AUTO?", .008);
	k2400_In(dev, msg);
	Scan(msg, "%i", &smu->src.delay);
										  
    //Scan (msg, "%s[i3]>L,%i", &smu->meas.range);
}

static void k2400_GetMachineStatus (gpibioPtr dev)
{
    char msg[256];
    k2400Ptr smu = dev->device;

    k2400_Out (dev, ":OUTPut:STATe?", .008);
    k2400_In (dev, msg);

    Scan (msg, "%i", &smu->operate);
}

static void k2400_SetLevel (sourcePtr src)
{
    char cmd[256];
	
	gpibioPtr dev = src->acqchan->dev;
	
	if(k2400_GetMode(dev))
   		Fmt(cmd, ":SOURce:CURRent:LEVel %f", src->biaslevel); /* outputs new current value */
	else
		Fmt(cmd, ":SOURce:VOLTage:LEVel %f", src->biaslevel);
    k2400_Out (dev, cmd, .02);
    util_Delay (src->segments[src->seg]->delay);
}

static void k2400_GetReadings (acqchanPtr acqchan)
{
	char msg[256];
    gpibioPtr dev = acqchan->dev;
	k2400Ptr smu = dev->device;
	k2400_Out(dev, ":OUTPut:STATe?", 0.02);
	k2400_In(dev, msg);
	Scan(msg, "%i", &smu->source->sourceOn);
	if (smu->source->sourceOn)
		k2400_GetLevels (dev);
}

static void k2400_GetLevels (gpibioPtr dev)
{
    char msg[256];
    short statusbyte;
    k2400Ptr smu = dev->device;
	k2400_Out(dev, ":FORMat:ELEMents VOLTage,CURRent", .02);
	k2400_Out(dev, ":INIT:IMM", .2);
	k2400_Out (dev, ":READ?", .02);
	k2400_In (dev, msg);
	if(k2400_GetMode(dev))
		Scan (msg, "%f,%f", &smu->measure->reading, &smu->source->acqchan->reading); 
	else
    	Scan (msg, "%f,%f", &smu->source->acqchan->reading, &smu->measure->reading);
    smu->source->acqchan->newreading = TRUE;
    smu->measure->newreading = TRUE;
}

static void k2400_CheckforProblems (gpibioPtr dev)
{
	char msg[256];
    short statusbyte;
    k2400Ptr smu = dev->device;

    gpibio_GetStatusByte (dev, &statusbyte);

    if (statusbyte & K2400_SRE_WARNING) k2400_GetWarningStatus (dev);
    if (statusbyte & K2400_SRE_ERROR) k2400_GetErrorStatus (dev);

    if (gpibio_SRQ(dev)) gpibio_GetStatusByte (dev, &statusbyte);
    smu->overLimit = statusbyte & K2400_SRE_COMPLIANCE;
	//*/
}

static void k2400_Out (gpibioPtr dev, char *cmd, double delay)
{
	gpibio_Out (dev, cmd);
	//Delay(delay);
}

static void k2400_In (gpibioPtr dev, char *msg)
{
	gpibio_In (dev, msg);
	//Delay(.02);
}

static void k2400_GetWarningStatus (gpibioPtr dev)
{
	int i, byte;
    char msg[256], rsp[256];

    k2400_Out (dev, "U9X", .008);
    k2400_In (dev, rsp);

    Scan (rsp, "%s>WRS%s", msg);
    for (i = 0; i<9; i++) {
        byte = msg[i];
        if (byte == 49) {
            switch (i) {
                case 0:
                    util_MessagePopup ("Keithley 2400 Message", "WARNING: Uncalibrated");
                    break;
                case 1:
                    util_MessagePopup ("Keithley 2400 Message", "WARNING: Temporary cal");
                    break;
                case 2:
                    util_MessagePopup ("Keithley 2400 Message", "WARNING: Value out of range");
                    break;
                case 3:
                    util_MessagePopup ("Keithley 2400 Message", "WARNING: Sweep buffer filled");
                    break;
                case 4:
                    util_MessagePopup ("Keithley 2400 Message", "WARNING: No sweep points, must create");
                    break;
                case 5:
                    util_MessagePopup ("Keithley 2400 Message", "WARNING: Pulse times not met");
                    break;
                case 6:
                    util_MessagePopup ("Keithley 2400 Message", "WARNING: Not in remote");
                    break;
                case 7:
                    util_MessagePopup ("Keithley 2400 Message", "WARNING: Measure range changed");
                    break;
                case 8:
                    util_MessagePopup ("Keithley 2400 Message", "WARNING: Measurement overflow (OFLO)/Sweep aborted");
                    break;
                case 9:
                    util_MessagePopup ("Keithley 2400 Message", "WARNING: Pending trigger");
                    break;
            }
        }
    } //*/
}

static void k2400_GetErrorStatus (gpibioPtr dev)
{
    int i, byte;
    char msg[256], rsp[256];

    k2400_Out (dev, "U1X", .008);
    k2400_In (dev, rsp);

    Scan (rsp, "%s>ERS%s", msg);
    for (i = 0; i<26; i++) {
        byte = msg[i];
        if (byte == 49) {
            switch (i) {
                case 0:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Trigger Overrun");
					break;
                case 1:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Illegal device dependent command");
                    break;
                case 2:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Illegal device dependent command option");
                    break;
                case 3:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Interlock present");
                    break;
                case 4:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Illegal measure range");
                    break;
                case 5:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Illegal source range");
                    break;
                case 6:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Invalid sweep mix");
                    break;
                case 7:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Log cannot cross zero");
                    break;
                case 8:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Autoranging source w/ pulse sweep");
                    break;
                case 9:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: In calibration");
                    break;
                case 10:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: In standby");
                    break;
                case 11:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Unit is a 2400");
                    break;
                case 12:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: IOU DPRAM failed");
                    break;
                case 13:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: IOU EEROM failed");
                    break;
                case 14:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: IOU Cal checksum error");
                    break;
                case 15:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: DPRAM lockup");
                    break;
                case 16:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: DPRAM link error");
                    break;
                case 17:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Cal ADC zero error");
                    break;
                case 18:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Cal ADC gain error");
                    break;
                case 19:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Cal SRC zero error");
                    break;
                case 20:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Cal SRC gain error");
                    break;
                case 21:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Cal common mode error");
                    break;
                case 22:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Cal compsmunce error");
                    break;
                case 23:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Cal value error");
                    break;
                case 24:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Cal constants error");
                    break;
                case 25:
                    util_MessagePopup ("Keithley 2400 Message", "ERROR: Cal invalid error");
                    break;
            }
        }
    } //*/
}

//Inputs: gpibioPtr
//Outputs: int
//Fucntion: Polls the machine to find out if it is sourcing voltage or current.
//Voltage returns 0, Current returns 1 (mirrors the SELECT button on the GUI)
int k2400_GetMode (gpibioPtr dev)
{
	char msg[256];
	k2400_Out(dev, ":SOURce:FUNCtion:MODE?", 0.008);
	k2400_In(dev, msg);
	if(msg[0] == 'V')
		return 0;
	return 1;
}


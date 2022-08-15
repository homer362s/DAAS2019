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

#include "BK9201.h"
#include "BK9201u.h"

#define BK9201_ID "B&K Precision, 9201"
//"0679385"

#define BK9201_SRE_SRQDISABLED 0
#define BK9201_SRE_WARNING 1
#define BK9201_SRE_SWEEPDONE 2
#define BK9201_SRE_TRIGGEROUT 4
#define BK9201_SRE_READINGDONE 8
#define BK9201_SRE_READYFORTRIGGER 16
#define BK9201_SRE_ERROR 32
#define BK9201_SRE_COMPLIANCE 128

#define TRUE 1
#define FALSE 0

typedef struct {
    acqchanPtr measure;
    sourcePtr source;
    int id, overLimit, sourceOn;
    int operate, sense;
    struct {int delay, current;} src;
    struct {} meas;
}   BK9201Type;

typedef BK9201Type *BK9201Ptr;

int BK_flag = 0;
time_t past, present;
double BK_delay = 1;

static void BK9201_GetErrorStatus (gpibioPtr dev);
static void BK9201_GetWarningStatus (gpibioPtr dev);

static void BK9201_In (gpibioPtr dev, char *msg);
static void BK9201_Out (gpibioPtr dev, char *cmd, double delay);
static void BK9201_CheckforProblems (gpibioPtr dev);
static void BK9201_GetLevels (gpibioPtr dev);
static void BK9201_GetReadings (acqchanPtr acqchan);
static void BK9201_SetLevel (sourcePtr src);

static void BK9201_GetOnOffStatus (gpibioPtr dev);
static void BK9201_GetMeasurementParameters (gpibioPtr dev);
static double BK9201_GetICompLevel (gpibioPtr dev);
static double BK9201_GetVCompLevel (gpibioPtr dev);   
static void BK9201_Save (gpibioPtr dev);
static void BK9201_Load (gpibioPtr dev);
static void BK9201_Remove (void *ptr);

static void BK9201_UpdateControls (int panel, gpibioPtr dev);

static void BK9201_Meas_UpdateReadings (int panel, void *smu);

static int  BK9201_InitGPIB (gpibioPtr dev);
static void *BK9201_Create (gpibioPtr dev);
static void OperateBK9201 (int menubar, int menuItem, void *callbackData, int panel);
static void BK9201_UpdateReadings (int panel, void *dev);

int BK9201_GetMode(gpibioPtr);

void BK9201_Init (void)
{
    devTypePtr devType;
    if (utilG.acq.status != ACQ_NONE) {
        util_ChangeInitMessage ("BK9201 Control Utilities...");
        devType = malloc (sizeof (devTypeItem));
        if (devType) {
            Fmt (devType->label, "BK9201 SMU");
            Fmt (devType->id, BK9201_ID);
            devType->InitDevice = BK9201_InitGPIB;
            devType->CreateDevice = BK9201_Create;
            devType->OperateDevice = OperateBK9201;
            devType->UpdateReadings = BK9201_UpdateReadings;
            devType->SaveDevice = BK9201_Save;
            devType->LoadDevice = BK9201_Load;
            devType->RemoveDevice = BK9201_Remove;
            devTypeList_AddItem (devType);
        }
    }
	//time(&past);
}

static void BK9201_UpdateReadings (int panel, void *dev)
{
    gpibioPtr my_dev = dev;
    BK9201Ptr smu = my_dev->device;
	//acqchanPtr acqchan = smu->source->acqchan;
    short statusbyte;
    char rsp[256];
    int control, dim, bg, mode, m;

	if (smu->operate) {
        if (utilG.acq.status == ACQ_BUSY)
			Delay(.05);
		if (!util_TakingData() || !(smu->source->acqchan->acquire || smu->measure->acquire)) {
            gpibio_GetStatusByte (dev, &statusbyte);
            if (statusbyte & BK9201_SRE_READINGDONE) 
			{
				
				BK9201_Out(dev, "MEASure:SCALar:VOLTage?", .001);   // request to read applied current  
	            BK9201_In (dev, rsp);
                Scan (rsp, "%f",  &smu->source->acqchan->reading); 
	
	            BK9201_Out(dev, "MEASure:SCALar:CURRent?", .002); // request to read applied voltage 
	            BK9201_In (dev, rsp);
                Scan (rsp, "%f", &smu->measure->reading); 
				//BK9201_In (dev, rsp);
                //Scan (rsp, "%s>%f,%f", &smu->source->acqchan->reading,
                //      &smu->measure->reading); 
            }
            //if (statusbyte & BK9201_SRE_READYFORTRIGGER) 
				//BK9201_Out (dev, "H0X", .02);
        }
		
		time(&present);
		if(difftime(present, past) >= BK_delay)
		{
			BK9201_GetReadings(smu->source->acqchan);
			time(&past);
		}
        //if (expG.acqstatus != utilG.acq.status) 
		{
            m = GetPanelMenuBar (panel);
            dim = (util_TakingData() && smu->source->inlist &&
                   (utilG.exp == EXP_SOURCE));

            if (dim) { mode = VAL_INDICATOR; bg = VAL_PANEL_GRAY;}
                else { mode = VAL_HOT; bg = VAL_WHITE;}

            SetCtrlAttribute (panel, BK9201_SOURCE, ATTR_CTRL_MODE, mode);
            SetCtrlAttribute (panel, BK9201_SOURCE, ATTR_TEXT_BGCOLOR, bg);

            SetInputMode (panel, BK9201_OPERA, !dim);
            //SetInputMode (panel, BK9201_SELECT, !dim);
            //SetInputMode (panel, BK9201_SENSE, !dim);
            SetMenuBarAttribute (m, BK9201MENU_FILE_LOAD, ATTR_DIMMED, dim);
        }

        BK9201_CheckforProblems (my_dev);
         SetCtrlVal (panel, BK9201_OVERLIMIT, smu->overLimit);

        control = GetActiveCtrl (panel);
        if (util_TakingData() || (control != BK9201_SOURCE))
        SetCtrlVal (panel, BK9201_SOURCE, smu->source->acqchan->reading);
        SetCtrlVal (panel, BK9201_MEASURE, smu->measure->reading);
    }
}

static void *BK9201_Create (gpibioPtr dev)
{
	char msg[256];
    BK9201Ptr smu;

    smu = malloc (sizeof(BK9201Type));
    smu->overLimit = FALSE;
    if (dev) smu->id = dev->id;
   // smu->src.range = 0; 
	//smu->src.rangeV = 0;/* AUTO */
    smu->source = source_Create ("BK9201 Source", dev, BK9201_SetLevel,
                                 BK9201_GetReadings);
    smu->measure = acqchan_Create ("BK9201 Measure", dev, BK9201_GetReadings);
	BK9201_Out(dev, "SOUR:OUTPut:STATe?", 0);  // needs to be changed here!!!!
	BK9201_In(dev, msg);
	smu->sourceOn = 0;
	//BK9201_Out (dev, "SOURc:OUTPut:STATe?", .008); // different from Keithley checks if it is on or off
    //BK9201_In (dev, msg);
	//Scan(msg, "%i", &smu->source->sourceOn);
    if (dev) dev->device = smu;
    return smu;
}

static int BK9201_InitGPIB (gpibioPtr dev)
{
    char cmd[512], rsp[512], buffer[512];
    int result;

    gpibio_Remote (dev); 
    
    if (gpibio_DeviceMatch (dev, "*IDN?", BK9201_ID))
	{
        return TRUE;
	}
    return FALSE;
}

void LoadBK9201Callback(int menubar, int menuItem, void *callbackData, int panel)
{
    int fileselect, id;
    char path[512], info[512];
    gpibioPtr dev = callbackData;
    BK9201Ptr smu = dev->device;

    fileselect = FileSelectPopup ("", "*.dev", "*.dev", "Load BK9201 SMU Setup",
                                  VAL_LOAD_BUTTON, 0, 1, 1, 0, path);
    if (fileselect == VAL_EXISTING_FILE_SELECTED) {
        fileHandle.analysis = util_OpenFile (path, FILE_READ, FALSE);
        ScanFile (fileHandle.analysis, "%s>#INSTRSETUP %i", &id);
        if (smu->id == id) {
            BK9201_Load (dev);
            ReadLine (fileHandle.analysis, info, 255);
            BK9201_UpdateControls(panel, dev);
        }
        else MessagePopup ("BK9201 Load Message", "Different instrument types--process aborted");
        util_CloseFile();
    }
}

void SaveBK9201Callback(int menubar, int menuItem, void *callbackData, int panel)
{
    int fileselect;
    char path[512];
    gpibioPtr dev = callbackData;
    BK9201Ptr smu = dev->device;
    fileselect = FileSelectPopup ("", "*.dev", "*.dev", "Save BK9201 Setup",
                                  VAL_SAVE_BUTTON, 0, 1, 1, 1, path);
    if (fileselect == VAL_NEW_FILE_SELECTED) {
        fileHandle.analysis = util_OpenFile (path, FILE_WRITE, FALSE);
        FmtFile (fileHandle.analysis, "%s<#INSTRSETUP %i\0", smu->id);
        BK9201_Save(dev);
        FmtFile (fileHandle.analysis, "#ENDSETUP\0");
        util_CloseFile();
    }
}

int  BK9201MeasControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    acqchanPtr acqchan = callbackData;
    switch (control) {
        case BK9201_MEA_ACQ:
            if (event == EVENT_VAL_CHANGED) {
                GetCtrlVal (panel, control, &acqchan->acquire);
                if (acqchan->acquire) acqchanlist_AddChannel (acqchan);
                    else acqchanlist_RemoveChannel (acqchan);
            }
            break;
        case BK9201_MEA_COEFF:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &acqchan->coeff);
                if (acqchan->p) SetCtrlVal (acqchan->p, ACQDATA_COEFF, acqchan->coeff);
            }
            break;
        case BK9201_MEA_LABEL:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, acqchan->channel->label);
                acqchanlist_ReplaceChannel (acqchan);
                if (acqchan->p) SetPanelAttribute (acqchan->p, ATTR_TITLE, acqchan->channel->label);
            }
            break;
        case BK9201_MEA_CLOSE:
            if (event == EVENT_COMMIT) {
                devPanel_Remove(panel);
                
                DiscardPanel (panel);
            }
            break;
    }
    return 0;
}

static void BK9201_Meas_UpdateReadings (int panel, void *measchan)
{
    acqchanPtr measure = measchan;

    SetCtrlVal (panel, BK9201_MEA_MEAS, measure->reading);
}

void BK9201MeasureSetupCallback(int menubar, int menuItem, void *callbackData, int panel)
{
    int p;
    acqchanPtr measure = callbackData;

    p = LoadPanel (0, "BK9201u.uir", BK9201_MEA);
    SetPanelPos (p, VAL_AUTO_CENTER, VAL_AUTO_CENTER);
    util_InitClose (p, BK9201_MEA_CLOSE, FALSE);

    SetCtrlVal (p, BK9201_MEA_LABEL, measure->channel->label);
    SetCtrlVal (p, BK9201_MEA_COEFF, measure->coeff);
    SetCtrlVal (p, BK9201_MEA_ACQ, measure->acquire);
    SetCtrlVal (p, BK9201_MEA_NOTE,measure->note);

    SetCtrlAttribute(p, BK9201_MEA_LABEL, ATTR_CALLBACK_DATA,measure);
    SetCtrlAttribute(p, BK9201_MEA_COEFF, ATTR_CALLBACK_DATA, measure);
    SetCtrlAttribute(p, BK9201_MEA_ACQ, ATTR_CALLBACK_DATA, measure);
    SetCtrlAttribute(p, BK9201_MEA_NOTE, ATTR_CALLBACK_DATA, measure);

    SetInputMode (p, BK9201_MEA_ACQ, !util_TakingData());

    devPanel_Add (p, measure, BK9201_Meas_UpdateReadings);
    InstallPopup (p);
}

void BK9201SourceSetupCallback(int menubar, int menuItem, void *callbackData, int panel)
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

int  BK9201SourceCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    sourcePtr src = callbackData;

    if (event == EVENT_COMMIT) {
        GetCtrlVal (panel, control, &src->biaslevel );
        BK9201_SetLevel (src);
    }
    return 0;
}

/*int  BK9201MeasureCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	return 0;
}*/

int  BK9201ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    gpibioPtr dev = callbackData;	//callback data comes as null	 
	BK9201Ptr smu = dev->device;	  // this line is giving trouble
	//sourcePtr src = smu->source;
    char cmd[256];
    double r, level;
    int id;

    switch (control) {
        /*case BK9201_INTTIME:		  
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &smu->meas.inttime);
                Fmt (cmd, "SENSe:CURRent:NPLCycles %f", smu->meas.inttime);																								   
				//sends same value to VOLTage
            }
            break;
       /*case BK9201_FILTER:		 
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &smu->meas.filter);
                Fmt (cmd, "SENSe:AVERage:COUNt %i", smu->meas.filter);
                BK9201_Out (dev, cmd, .02);
            }
            break;
       // case BK9201_MEAS_V_RANGE:
       // case BK9201_MEAS_I_RANGE:
            /*if (event == EVENT_COMMIT) {
                GetCtrlIndex (panel, control, &smu->meas.range);
                GetCtrlVal (panel, control, &r);
                level = BK9201_GetCompLevel(dev);
                if (r < level) level = r;
				SetMeasRange();
            }  
            break;*/
        case BK9201_I_LIMIT:	  
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &r);
				BK9201_Out (dev, "SOUR:CURR:PROT:STAT 1",0.01);
			//	if(smu->src.current)
				;//	Fmt(cmd,":SENSe:VOLTage:PROTection:LEVel %f",r);
				//else
                Fmt(cmd,"SOURce:CURRent:PROTection:LEVel %f",r); /* trigger level change */
                BK9201_Out (dev, cmd, .04);
            }
            break;
       /* case BK9201_SENSE: // 0 = disable remote sense, 1 = enable remote sense
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &smu->sense);
				Fmt (cmd, ":SYSTem:RSENse %i", smu->sense);
                BK9201_Out (dev, cmd, .02);
            }
            break;  */
        case BK9201_V_LIMIT:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &r);
				
				Fmt(cmd, "SOURce:VOLTage:LIMIt:LEVel %f", r);
				BK9201_Out (dev, cmd, .04);	
				
				smu->source->sourceOn = 0;   //what is it?
            }
            break;
        case BK9201_OPERA:
            if (event == EVENT_COMMIT) 
			{
                //GetCtrlVal (panel, control, &smu->operate);
				//BK9201_Out (dev, "SOURce:OUTPut:STATe?", .008); //  checks if it is on or off
                //BK9201_In (dev, cmd);

                //Scan (cmd, "%i", &smu->operate);
				
                //Fmt (cmd, "SOURce:OUTPut:STATe %i", smu->operate);
                 //if (smu->operate==1) BK9201_Out (dev,   "SOURce:OUTPut:STATe 0" , .02); 
				 // else   BK9201_Out (dev,   "SOURce:OUTPut:STATe 1" , .02);
				//BK9201_Out (dev, cmd, .02);
				
				GetCtrlVal (panel, control, &smu->operate);
                Fmt (cmd, "SOURce:OUTPut:STATe %i", smu->operate);
                BK9201_Out (dev, cmd, .02);
				//smu->source->sourceOn = !smu->source->sourceOn;
            }
            break;
		 case BK9201_CLEARBRAINS:
           if (event == EVENT_COMMIT) 
		   {
           //  GetCtrlVal (panel, control, &smu->operate);
			BK9201_Out (dev, "*CLS", .008); //  clears out event registers
              //  BK9201_In (dev, cmd);

                //Scan (cmd, "%i", &smu->operate);
				
                //Fmt (cmd, "SOURce:OUTPut:STATe %i", smu->operate);
                 //if (smu->operate==1) BK9201_Out (dev,   "SOURce:OUTPut:STATe 0" , .02); 
				  //else   BK9201_Out (dev,   "SOURce:OUTPut:STATe 1" , .02);
				//BK9201_Out (dev, cmd, .02);
				
		//		
		//	smu->source->sourceOn = !smu->source->sourceOn;
          }
          break;
        
		 
		 case BK9201_IDENTIFY:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &id);
				if(id)
				{
					BK9201_Out(dev, ":*IDN?", .015);
					Fmt(cmd, "*IDN?", dev->paddr);
					BK9201_Out(dev, cmd, .5);
				}
				//else
					//BK9201_Out(dev, ":DISPlay:WINDow:TEXT:STATe 0", .015);

            }
            break;
			

        /*case BK9201_DELAY:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &smu->src.delay);
                Fmt (cmd, ":SOURce:DELay:AUTO %i", smu->src.delay);
                BK9201_Out (dev, cmd, .02);
            }
            break; 
        case BK9201_SOURCE_V_RANGE:
        case BK9201_SOURCE_I_RANGE:
            if (event == EVENT_COMMIT) {
                GetCtrlVal(panel, BK9201_SOURCE, &level);
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
				BK9201_Out (dev, cmd, .02); 
				src->biaslevel = level;
				BK9201_SetLevel(src); 
                BK9201_Out (dev, cmd, .02);
                smu->source->min = -r;
                smu->source->max = r;
				if(src->panel)
					source_UpdatePanel(src->panel, src);
            }
            break;		*/
    }

    if (event == EVENT_COMMIT) {
        BK9201_UpdateControls(panel, dev);
    } //*/
    return 0;
}


int  BK9201MainPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2)
{
    gpibioPtr dev = callbackData;
    BK9201Ptr smu = dev->device;
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
        SetMenuBarAttribute (menubar, BK9201MENU_FILE_SAVE, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, BK9201MENU_FILE_LOAD, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, BK9201MENU_SOURCE, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, BK9201MENU_MEASURE, ATTR_DIMMED, (dev->status != DEV_REMOTE));

        if (!util_TakingData()) BK9201_UpdateControls (panel, dev);
    }
    return 0;
}

static void OperateBK9201 (int menubar, int menuItem, void *callbackData, int panel)
{
    int p, m;
    gpibioPtr dev = callbackData;
    BK9201Ptr smu = dev->device;
    char label[256];

    SetMenuBarAttribute (menubar, menuItem, ATTR_DIMMED, TRUE);

    p = dev->iPanel? dev->iPanel:LoadPanel (utilG.p, "BK9201u.uir", BK9201);
	dev->iPanel = p;
    
    SetPanelPos (p, VAL_AUTO_CENTER, VAL_AUTO_CENTER);

    Fmt (label, "BK9201 Source Measure Unit: %s", dev->label);
    SetPanelAttribute (p, ATTR_TITLE, label);

    m = LoadMenuBar (p, "BK9201u.uir", BK9201MENU);
    
    SetPanelMenuBar (p, m);

    SetMenuBarAttribute (m, BK9201MENU_FILE_SAVE, ATTR_CALLBACK_DATA, dev);
    SetMenuBarAttribute (m, BK9201MENU_FILE_LOAD, ATTR_CALLBACK_DATA, dev);
    SetMenuBarAttribute (m, BK9201MENU_FILE_GPIB, ATTR_CALLBACK_DATA, dev);
    SetMenuBarAttribute (m, BK9201MENU_SOURCE, ATTR_CALLBACK_DATA, smu->source);
    SetMenuBarAttribute (m, BK9201MENU_MEASURE, ATTR_CALLBACK_DATA, smu->measure);

    SetPanelAttribute (p, ATTR_CALLBACK_DATA, dev);

    SetCtrlAttribute (p, BK9201_SOURCE, ATTR_CALLBACK_DATA, smu->source);

    //SetCtrlAttribute (p, BK9201_SOURCE_I_RANGE, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, BK9201_SOURCE_V_RANGE, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, BK9201_DELAY, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, BK9201_IDENTIFY, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, BK9201_OPERA, ATTR_CALLBACK_DATA, dev);
	SetCtrlAttribute (p, BK9201_CLEARBRAINS, ATTR_CALLBACK_DATA, dev);   
    //SetCtrlAttribute (p, BK9201_SELECT, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, BK9201_SENSE, ATTR_CALLBACK_DATA, dev);
    
    
    SetCtrlAttribute (p, BK9201_I_LIMIT, ATTR_CALLBACK_DATA, dev);
	SetCtrlAttribute (p, BK9201_V_LIMIT, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, BK9201_FILTER, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, BK9201_INTTIME, ATTR_CALLBACK_DATA, dev);
	
	// insert current protection here
    BK9201_UpdateControls (p, dev);

    devPanel_Add (p, dev, BK9201_UpdateReadings);
    DisplayPanel (p);
}
/****************************/
static void BK9201_UpdateControls (int panel, gpibioPtr dev)
{
    char s_text[20], m_text[20], cmd[256];
    BK9201Ptr smu = dev->device;
    double absmax,r;
	
    BK9201_GetOnOffStatus (dev);
    BK9201_GetMeasurementParameters (dev);
	BK9201_GetReadings(smu->source->acqchan);

    SetCtrlVal (panel, BK9201_SOURCE, smu->source->acqchan->reading);
    SetCtrlVal (panel, BK9201_MEASURE, smu->measure->reading);

    SetCtrlVal (panel, BK9201_OPERA, smu->operate);		  // controls dimming =gray color enables /disables
    SetInputMode (panel, BK9201_SOURCE, smu->operate);
    SetInputMode (panel, BK9201_MEASURE, smu->operate);

    //SetCtrlVal (panel, BK9201_SELECT, smu->src.current);
    //SetCtrlVal (panel, BK9201_SENSE, smu->sense);

    /*if (smu->src.current) {
        Fmt (s_text, "A"); Fmt (m_text, "V");   ///needs a fix!!
    } else {
        Fmt (s_text, "V"); Fmt (m_text, "A");
    } 

    SetCtrlVal (panel, BK9201_SOURCETEXT, s_text);
    SetCtrlVal (panel, BK9201_MEASURETEXT, m_text);

    //SetCtrlAttribute (panel, BK9201_SOURCE_I_RANGE, ATTR_VISIBLE, smu->src.current);
    //SetCtrlAttribute (panel, BK9201_SOURCE_V_RANGE, ATTR_VISIBLE, !smu->src.current);

    /* if (smu->src.current) {
        SetCtrlIndex (panel, BK9201_SOURCE_I_RANGE, smu->src.rangeC);
        GetCtrlVal (panel, BK9201_SOURCE_I_RANGE, &smu->source->max);
    } else {
        SetCtrlIndex (panel, BK9201_SOURCE_V_RANGE, smu->src.rangeV);
        GetCtrlVal (panel, BK9201_SOURCE_V_RANGE, &smu->source->max);
    }  */

    smu->source->min = 0.0;
	GetCtrlVal (panel, BK9201_V_LIMIT, &r);
	
	smu->source->max=r;
    SetCtrlAttribute (panel, BK9201_SOURCE, ATTR_MIN_VALUE, smu->source->min);
    SetCtrlAttribute (panel, BK9201_SOURCE, ATTR_MAX_VALUE, smu->source->max);
    // SetCtrlVal (panel, BK9201_DELAY, smu->src.delay);

   // SetCtrlAttribute (panel, BK9201_MEAS_I_RANGE, ATTR_VISIBLE, !smu->src.current);
    //SetCtrlAttribute (panel, BK9201_MEAS_V_RANGE, ATTR_VISIBLE, smu->src.current);

	//GetCtrlVal (panel, BK9201_I_LIMIT, &smu->meas.range); 
	//Fmt(cmd, "SOURce:CURRent:LIMit %f", smu->meas.range);
	
	SetCtrlVal (panel, BK9201_I_LIMIT, BK9201_GetICompLevel (dev)); 
	SetCtrlVal (panel, BK9201_V_LIMIT, BK9201_GetVCompLevel (dev));       
	
	//BK9201_Out (dev, ":SOUR:CURRent:PROTection:LEVel?", .008);   
	
    //SetCtrlVal (panel, BK9201_I_LIMIT, BK9201_GetCompLevel (dev));
	
    //BK9201_In (dev, msg);

    //Scan (msg, "%s>%f", &level)
	//SetCtrlVal (panel, BK9201_V_LIMIT, BK9201_GetCompLevel (dev)); // need to fix function

    /*if (smu->src.current) {
        SetCtrlIndex (panel, BK9201_MEAS_V_RANGE, smu->meas.range);
        GetCtrlVal (panel, BK9201_MEAS_V_RANGE, &absmax);
    } else {
        SetCtrlIndex (panel, BK9201_MEAS_I_RANGE, smu->meas.range);
        GetCtrlVal (panel, BK9201_MEAS_I_RANGE, &absmax);
    }  */

    SetCtrlAttribute (panel, BK9201_I_LIMIT, ATTR_MIN_VALUE, 0.0);
    SetCtrlAttribute (panel, BK9201_I_LIMIT, ATTR_MAX_VALUE, 1.0);
	
	SetCtrlAttribute (panel, BK9201_V_LIMIT, ATTR_MIN_VALUE, 0.0);
    SetCtrlAttribute (panel, BK9201_V_LIMIT, ATTR_MAX_VALUE, 65.0);


    

}

static void BK9201_Remove (void *ptr)
{
    BK9201Ptr smu = ptr;
    acqchan_Remove(smu->measure);
    source_Remove (smu->source);
    free (smu);
}

static void BK9201_Load (gpibioPtr dev)
{
    char cmd[256];
    double r;
    int i;
    BK9201Ptr smu;
	sourcePtr src;

    if (dev){
		smu = dev->device;
		src = smu->source;
	}
    else smu = BK9201_Create (NULL);

/*  if (dev) BK9201_Out (dev, "N0X");*/

    ScanFile (fileHandle.analysis, "%s>Sense   : %i%s[dw1]", &smu->sense);
    if (dev) {Fmt (cmd, ":SYSTem:RSENse %i", smu->sense); BK9201_Out (dev, cmd, 0);}

    ScanFile (fileHandle.analysis, "%s>Source  : %i, %i%s[dw1]",
              &smu->src.current, &smu->src.delay);
    if (dev) {
		if(smu->src.current)
		{
			BK9201_Out(dev, ":SOURce:FUNCtion:MODE CURRent", .07); 
			BK9201_Out(dev, ":SENSe:FUNCtion \"VOLTage\"", .07); 
		}
		else
		{
			BK9201_Out(dev, ":SOURce:FUNCtion:MODE VOLTage", .07); 
			BK9201_Out(dev, ":SENSe:FUNCtion \"CURRent\"", .07); 
		}
		smu->source->sourceOn = 0;
        Fmt (cmd, ":SOURce:DELay:AUTO %i", smu->src.delay);
        BK9201_Out (dev, cmd, 0);
    }
   // smu->src.rangeV = 0;
	//smu->src.rangeC = 0;/*AUTO*/
    smu->source->min = 0;
    smu->source->max = 60.0;
	BK9201_Out(dev,":OUTPut:STATe?",0);
	BK9201_In(dev,cmd);
	Scan(cmd, "%i", &smu->source->sourceOn);
    source_Load (dev, smu->source);

    //ScanFile (fileHandle.analysis, "%s>Measure : %f, %i, %i, %f\0",
     //        &r, &smu->meas.range, &smu->meas.filter, &smu->meas.inttime); needs a fix!!
    if (dev) {
	//	Fmt (cmd, "SENSe:CURRent:NPLCycles %f", smu->meas.inttime);
	//	BK9201_Out (dev, cmd, 0);
	//	Fmt (cmd, "SENSe:AVERage:COUNt %i", smu->meas.filter);
     //   BK9201_Out (dev, cmd, 0);
    }

    acqchan_Load (dev, smu->measure);

    if (!dev) BK9201_Remove (smu);
}

static void BK9201_Save (gpibioPtr dev)
{
    BK9201Ptr smu = dev->device;

    BK9201_GetOnOffStatus(dev);
    BK9201_GetMeasurementParameters(dev);

    FmtFile (fileHandle.analysis, "Sense   : %i\n", smu->sense);
    FmtFile (fileHandle.analysis, "Source  : %i, %i\n", smu->src.current, smu->src.delay);
    source_Save (smu->source);
   // FmtFile (fileHandle.analysis, "Measure : %f[e2p5], %i, %i, %f\n",
             //BK9201_GetICompLevel (dev), smu->meas.range,
    //         smu->meas.filter, smu->meas.inttime);	   needs fix!
    acqchan_Save (smu->measure);
}


static double BK9201_GetICompLevel (gpibioPtr dev)
{
    char msg[256];
    double level;
	
	BK9201Ptr smu = dev->device;
	
//	if (smu->src.current)
	//	BK9201_Out (dev, "SOUR:VOLTage:LEVel?", .008)	   /// needs a fix!!!
	; 
//	else
    	BK9201_Out (dev, ":SOUR:CURRent:PROTection:LEVel?", .008);
    BK9201_In (dev, msg);

    Scan (msg, "%s>%f", &level);
    return level;
}

static double BK9201_GetVCompLevel (gpibioPtr dev)
{
    char msg[256];
    double level;
	
	BK9201Ptr smu = dev->device;
	
//	if (smu->src.current)
	//	BK9201_Out (dev, "SOUR:VOLTage:LEVel?", .008)	   /// needs a fix!!!
	; 
//	else
    	BK9201_Out (dev, ":SOUR:VOLTage:LIMI:LEVel?", .008);
    BK9201_In (dev, msg);

    Scan (msg, "%s>%f", &level);
    return level;
}

static void BK9201_GetMeasurementParameters (gpibioPtr dev)
{
    char msg[256];
    int srctype;
    BK9201Ptr smu = dev->device;
	
	
	/*Check Source mode*
    BK9201_Out(dev, ":SOURce:FUNCtion:MODE?", .008);  //no such command!!
    BK9201_In(dev, msg);
	if(msg[0] == 'V')
		smu->src.current = 0;
	else
		smu->src.current = 1;
	
	/*Check for remote/local sense*
	BK9201_Out(dev, ":SYStem:RSENse?", .008);	   // not plan to use
	BK9201_In(dev, msg);
	Scan(msg, "%i", &smu->sense);
	
	/*Check Filter steps*
	BK9201_Out(dev, ":SENSe:AVERage:COUNt?", .008);		//no such command!!
	BK9201_In(dev, msg);
	Scan(msg, "%i", &smu->meas.filter);
	
	/*Check Integration time*
	BK9201_Out(dev, ":SENSe:CURRent:NPLCycles?", .008);
	BK9201_In(dev, msg);
	Scan(msg, "%f", &smu->meas.inttime);
	
	/*Check Delay time*
	BK9201_Out(dev, ":SOURce:DELay:AUTO?", .008);
	BK9201_In(dev, msg);
	Scan(msg, "%i", &smu->src.delay);
										  
    //Scan (msg, "%s[i3]>L,%i", &smu->meas.range);
	*/
}

static void BK9201_GetOnOffStatus (gpibioPtr dev)
{
    char msg[256];
    BK9201Ptr smu = dev->device;

    BK9201_Out (dev, "SOURce:OUTPut:STATe?", .008); // different from Keithley checks if it is on or off
    BK9201_In (dev, msg);

    Scan (msg, "%i", &smu->operate);
}

static void BK9201_SetLevel (sourcePtr src)
{
    char cmd[256];
	
	gpibioPtr dev = src->acqchan->dev;
	
	//if(BK9201_GetMode(dev))
   	//	Fmt(cmd, ":SOURce:CURRent:LEVel %f", src->biaslevel); /* trigger new source value */
//	else
		Fmt(cmd, "SOURce:VOLTage:LEVel %f", src->biaslevel);
    BK9201_Out (dev, cmd, .02);
    util_Delay (src->segments[src->seg]->delay);
}

static void BK9201_GetReadings (acqchanPtr acqchan)
{
	char msg[256];
    gpibioPtr dev = acqchan->dev;
	BK9201Ptr smu = dev->device;
	BK9201_Out(dev, ":OUTPut:STATe?", 0.02);
	BK9201_In(dev, msg);
	Scan(msg, "%i", &smu->source->sourceOn);
	if (smu->source->sourceOn)
		BK9201_GetLevels (dev);
}
/****************** needs work*************/
static void BK9201_GetLevels (gpibioPtr dev)
{
    char msg[256];
    short statusbyte;
    BK9201Ptr smu = dev->device;
	
	BK9201_Out(dev, "MEASure:SCALar:CURRent?", .002);   // request to read applied current  
	BK9201_In (dev, msg);
    Scan (msg, "%f",  &smu->measure->reading); 
	
	BK9201_Out(dev, "MEASure:SCALar:VOLTage?", .002); // request to read applied voltage 
	BK9201_In (dev, msg);
    Scan (msg, "%f", &smu->source->acqchan->reading); 
	
    smu->source->acqchan->newreading = TRUE;
    smu->measure->newreading = TRUE;
}

static void BK9201_CheckforProblems (gpibioPtr dev)
{
	char msg[256];
    short statusbyte;
    BK9201Ptr smu = dev->device;

    gpibio_GetStatusByte (dev, &statusbyte);

    if (statusbyte & BK9201_SRE_WARNING) BK9201_GetWarningStatus (dev);
    if (statusbyte & BK9201_SRE_ERROR) BK9201_GetErrorStatus (dev);

    if (gpibio_SRQ(dev)) gpibio_GetStatusByte (dev, &statusbyte);
    smu->overLimit = statusbyte & BK9201_SRE_COMPLIANCE;
	//*/
}

static void BK9201_Out (gpibioPtr dev, char *cmd, double delay)
{
	gpibio_Out (dev, cmd);
	//Delay(delay);
}

static void BK9201_In (gpibioPtr dev, char *msg)
{
	gpibio_In (dev, msg);
	//Delay(.02);
}

static void BK9201_GetWarningStatus (gpibioPtr dev)
{
	int i, byte;
    char msg[256], rsp[256];

    BK9201_Out (dev, "U9X", .008);
    BK9201_In (dev, rsp);

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

static void BK9201_GetErrorStatus (gpibioPtr dev)
{
    int i, byte;
    char msg[256], rsp[256];

    BK9201_Out (dev, "U1X", .008);
    BK9201_In (dev, rsp);

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
int BK9201_GetMode (gpibioPtr dev)
{
	char msg[256];
	BK9201_Out(dev, ":SOURce:FUNCtion:MODE?", 0.008);
	BK9201_In(dev, msg);
	if(msg[0] == 'V')
		return 0;
	return 1;
}


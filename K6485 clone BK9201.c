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

#include "K6485.h"
#include "K6485u.h"

#define K6485_ID "KEITHLEY INSTRUMENTS INC.,MODEL 6485"
//"0679385"

#define K6485_SRE_SRQDISABLED 0
#define K6485_SRE_WARNING 1
#define K6485_SRE_SWEEPDONE 2
#define K6485_SRE_TRIGGEROUT 4
#define K6485_SRE_READINGDONE 8
#define K6485_SRE_READYFORTRIGGER 16
#define K6485_SRE_ERROR 32
#define K6485_SRE_COMPLIANCE 128

#define TRUE 1
#define FALSE 0

typedef struct {
    acqchanPtr measure;
    sourcePtr source;
    int id, overLimit, sourceOn;
    int operate, sense;
    struct {int delay, current;} src;
    struct {} meas;
}   K6485Type;

typedef K6485Type *K6485Ptr;

int BK_flag = 0;
time_t past, present;
double BK_delay = 1;

static void K6485_GetErrorStatus (gpibioPtr dev);
static void K6485_GetWarningStatus (gpibioPtr dev);

static void K6485_In (gpibioPtr dev, char *msg);
static void K6485_Out (gpibioPtr dev, char *cmd, double delay);
static void K6485_CheckforProblems (gpibioPtr dev);
static void K6485_GetLevels (gpibioPtr dev);
static void K6485_GetReadings (acqchanPtr acqchan);
static void K6485_SetLevel (sourcePtr src);

static void K6485_GetOnOffStatus (gpibioPtr dev);
static void K6485_GetMeasurementParameters (gpibioPtr dev);
static double K6485_GetICompLevel (gpibioPtr dev);
static double K6485_GetVCompLevel (gpibioPtr dev);   
static void K6485_Save (gpibioPtr dev);
static void K6485_Load (gpibioPtr dev);
static void K6485_Remove (void *ptr);

static void K6485_UpdateControls (int panel, gpibioPtr dev);

static void K6485_Meas_UpdateReadings (int panel, void *smu);

static int  K6485_InitGPIB (gpibioPtr dev);
static void *K6485_Create (gpibioPtr dev);
static void OperateK6485 (int menubar, int menuItem, void *callbackData, int panel);
static void K6485_UpdateReadings (int panel, void *dev);

int K6485_GetMode(gpibioPtr);

void K6485_Init (void)
{
    devTypePtr devType;
    if (utilG.acq.status != ACQ_NONE) {
        util_ChangeInitMessage ("K6485 Control Utilities...");
        devType = malloc (sizeof (devTypeItem));
        if (devType) {
            Fmt (devType->label, "K6485 Picoammeter");
            Fmt (devType->id, K6485_ID);
            devType->InitDevice = K6485_InitGPIB;
            devType->CreateDevice = K6485_Create;
            devType->OperateDevice = OperateK6485;
            devType->UpdateReadings = K6485_UpdateReadings;
            devType->SaveDevice = K6485_Save;
            devType->LoadDevice = K6485_Load;
            devType->RemoveDevice = K6485_Remove;
            devTypeList_AddItem (devType);
        }
    }
	//time(&past);
}

static void K6485_UpdateReadings (int panel, void *dev)
{
    gpibioPtr my_dev = dev;
    K6485Ptr smu = my_dev->device;
	//acqchanPtr acqchan = smu->source->acqchan;
    short statusbyte;
    char rsp[256];
    int control, dim, bg, mode, m;

	if (smu->operate) {
        if (utilG.acq.status == ACQ_BUSY)
			Delay(.05);
		if (!util_TakingData() || !(smu->source->acqchan->acquire || smu->measure->acquire)) {
            gpibio_GetStatusByte (dev, &statusbyte);
            if (statusbyte & K6485_SRE_READINGDONE) 
			{
				
				K6485_Out(dev, "MEASure:SCALar:VOLTage?", .001);   // request to read applied current  
	            K6485_In (dev, rsp);
                Scan (rsp, "%f",  &smu->source->acqchan->reading); 
	
	            K6485_Out(dev, "MEASure:SCALar:CURRent?", .002); // request to read applied voltage 
	            K6485_In (dev, rsp);
                Scan (rsp, "%f", &smu->measure->reading); 
				//K6485_In (dev, rsp);
                //Scan (rsp, "%s>%f,%f", &smu->source->acqchan->reading,
                //      &smu->measure->reading); 
            }
            //if (statusbyte & K6485_SRE_READYFORTRIGGER) 
				//K6485_Out (dev, "H0X", .02);
        }
		
		time(&present);
		if(difftime(present, past) >= BK_delay)
		{
			K6485_GetReadings(smu->source->acqchan);
			time(&past);
		}
        //if (expG.acqstatus != utilG.acq.status) 
		{
            m = GetPanelMenuBar (panel);
            dim = (util_TakingData() && smu->source->inlist &&
                   (utilG.exp == EXP_SOURCE));

            if (dim) { mode = VAL_INDICATOR; bg = VAL_PANEL_GRAY;}
                else { mode = VAL_HOT; bg = VAL_WHITE;}

            

            SetInputMode (panel, K6485_OPERA, !dim);
            //SetInputMode (panel, K6485_SELECT, !dim);
            //SetInputMode (panel, K6485_SENSE, !dim);
            SetMenuBarAttribute (m, K6485MENU_FILE_LOAD, ATTR_DIMMED, dim);
        }

        K6485_CheckforProblems (my_dev);
         SetCtrlVal (panel, K6485_OVERLIMIT, smu->overLimit);

        control = GetActiveCtrl (panel);
        if (util_TakingData() )
        
        SetCtrlVal (panel, K6485_MEASURE, smu->measure->reading);
    }
}

static void *K6485_Create (gpibioPtr dev)
{
	char msg[256];
    K6485Ptr smu;

    smu = malloc (sizeof(K6485Type));
    smu->overLimit = FALSE;
    if (dev) smu->id = dev->id;
   // smu->src.range = 0; 
	//smu->src.rangeV = 0;/* AUTO */
   
    smu->measure = acqchan_Create ("K6485 Measure", dev, K6485_GetReadings);
	K6485_Out(dev, "SOUR:OUTPut:STATe?", 0);  // needs to be changed here!!!!
	K6485_In(dev, msg);
	
	//K6485_Out (dev, "SOURc:OUTPut:STATe?", .008); // different from Keithley checks if it is on or off
    //K6485_In (dev, msg);
	//Scan(msg, "%i", &smu->source->sourceOn);
    if (dev) dev->device = smu;
    return smu;
}

static int K6485_InitGPIB (gpibioPtr dev)
{
    char cmd[512], rsp[512], buffer[512];
    int result;

    gpibio_Remote (dev); 
    
    if (gpibio_DeviceMatch (dev, "*IDN?", K6485_ID))
	{
        return TRUE;
	}
    return FALSE;
}

void LoadK6485Callback(int menubar, int menuItem, void *callbackData, int panel)
{
    int fileselect, id;
    char path[512], info[512];
    gpibioPtr dev = callbackData;
    K6485Ptr smu = dev->device;

    fileselect = FileSelectPopup ("", "*.dev", "*.dev", "Load K6485 SMU Setup",
                                  VAL_LOAD_BUTTON, 0, 1, 1, 0, path);
    if (fileselect == VAL_EXISTING_FILE_SELECTED) {
        fileHandle.analysis = util_OpenFile (path, FILE_READ, FALSE);
        ScanFile (fileHandle.analysis, "%s>#INSTRSETUP %i", &id);
        if (smu->id == id) {
            K6485_Load (dev);
            ReadLine (fileHandle.analysis, info, 255);
            K6485_UpdateControls(panel, dev);
        }
        else MessagePopup ("K6485 Load Message", "Different instrument types--process aborted");
        util_CloseFile();
    }
}

void SaveK6485Callback(int menubar, int menuItem, void *callbackData, int panel)
{
    int fileselect;
    char path[512];
    gpibioPtr dev = callbackData;
    K6485Ptr smu = dev->device;
    fileselect = FileSelectPopup ("", "*.dev", "*.dev", "Save K6485 Setup",
                                  VAL_SAVE_BUTTON, 0, 1, 1, 1, path);
    if (fileselect == VAL_NEW_FILE_SELECTED) {
        fileHandle.analysis = util_OpenFile (path, FILE_WRITE, FALSE);
        FmtFile (fileHandle.analysis, "%s<#INSTRSETUP %i\0", smu->id);
        K6485_Save(dev);
        FmtFile (fileHandle.analysis, "#ENDSETUP\0");
        util_CloseFile();
    }
}

int  K6485MeasControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    acqchanPtr acqchan = callbackData;
    switch (control) {
        case K6485_MEA_ACQ:
            if (event == EVENT_VAL_CHANGED) {
                GetCtrlVal (panel, control, &acqchan->acquire);
                if (acqchan->acquire) acqchanlist_AddChannel (acqchan);
                    else acqchanlist_RemoveChannel (acqchan);
            }
            break;
        case K6485_MEA_COEFF:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &acqchan->coeff);
                if (acqchan->p) SetCtrlVal (acqchan->p, ACQDATA_COEFF, acqchan->coeff);
            }
            break;
        case K6485_MEA_LABEL:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, acqchan->channel->label);
                acqchanlist_ReplaceChannel (acqchan);
                if (acqchan->p) SetPanelAttribute (acqchan->p, ATTR_TITLE, acqchan->channel->label);
            }
            break;
        case K6485_MEA_CLOSE:
            if (event == EVENT_COMMIT) {
                devPanel_Remove(panel);
                
                DiscardPanel (panel);
            }
            break;
    }
    return 0;
}

static void K6485_Meas_UpdateReadings (int panel, void *measchan)
{
    acqchanPtr measure = measchan;

    SetCtrlVal (panel, K6485_MEA_MEAS, measure->reading);
}

void K6485MeasureSetupCallback(int menubar, int menuItem, void *callbackData, int panel)
{
    int p;
    acqchanPtr measure = callbackData;

    p = LoadPanel (0, "K6485u.uir", K6485_MEA);
    SetPanelPos (p, VAL_AUTO_CENTER, VAL_AUTO_CENTER);
    util_InitClose (p, K6485_MEA_CLOSE, FALSE);

    SetCtrlVal (p, K6485_MEA_LABEL, measure->channel->label);
    SetCtrlVal (p, K6485_MEA_COEFF, measure->coeff);
    SetCtrlVal (p, K6485_MEA_ACQ, measure->acquire);
    SetCtrlVal (p, K6485_MEA_NOTE,measure->note);

    SetCtrlAttribute(p, K6485_MEA_LABEL, ATTR_CALLBACK_DATA,measure);
    SetCtrlAttribute(p, K6485_MEA_COEFF, ATTR_CALLBACK_DATA, measure);
    SetCtrlAttribute(p, K6485_MEA_ACQ, ATTR_CALLBACK_DATA, measure);
    SetCtrlAttribute(p, K6485_MEA_NOTE, ATTR_CALLBACK_DATA, measure);

    SetInputMode (p, K6485_MEA_ACQ, !util_TakingData());

    devPanel_Add (p, measure, K6485_Meas_UpdateReadings);
    InstallPopup (p);
}





/*int  K6485MeasureCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	return 0;
}*/

int  K6485ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    gpibioPtr dev = callbackData;	//callback data comes as null	 
	K6485Ptr smu = dev->device;	  // this line is giving trouble
	//sourcePtr src = smu->source;
    char cmd[256];
    double r, level;
    int id;

    switch (control) {
        /*case K6485_INTTIME:		  
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &smu->meas.inttime);
                Fmt (cmd, "SENSe:CURRent:NPLCycles %f", smu->meas.inttime);																								   
				//sends same value to VOLTage
            }
            break;
       /*case K6485_FILTER:		 
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &smu->meas.filter);
                Fmt (cmd, "SENSe:AVERage:COUNt %i", smu->meas.filter);
                K6485_Out (dev, cmd, .02);
            }
            break;
       // case K6485_MEAS_V_RANGE:
       // case K6485_MEAS_I_RANGE:
            /*if (event == EVENT_COMMIT) {
                GetCtrlIndex (panel, control, &smu->meas.range);
                GetCtrlVal (panel, control, &r);
                level = K6485_GetCompLevel(dev);
                if (r < level) level = r;
				SetMeasRange();
            }  
            break;*/
        //case K6485_I_LIMIT:	  
           // if (event == EVENT_COMMIT) {
               // GetCtrlVal (panel, control, &r);
				//K6485_Out (dev, "SOUR:CURR:PROT:STAT 1",0.01);
			//	if(smu->src.current)
				;//	Fmt(cmd,":SENSe:VOLTage:PROTection:LEVel %f",r);
				//else
               // Fmt(cmd,"SOURce:CURRent:PROTection:LEVel %f",r); /* trigger level change */
               // K6485_Out (dev, cmd, .04);
           // }
           // break;
       /* case K6485_SENSE: // 0 = disable remote sense, 1 = enable remote sense
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &smu->sense);
				Fmt (cmd, ":SYSTem:RSENse %i", smu->sense);
                K6485_Out (dev, cmd, .02);
            }
            break;  */
        /*case K6485_V_LIMIT:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &r);
				
				Fmt(cmd, "SOURce:VOLTage:LIMIt:LEVel %f", r);
				K6485_Out (dev, cmd, .04);	
				
				smu->source->sourceOn = 0;   //what is it?
            }
            break;*/
        case K6485_OPERA:
            if (event == EVENT_COMMIT) 
			{
                //GetCtrlVal (panel, control, &smu->operate);
				//K6485_Out (dev, "SOURce:OUTPut:STATe?", .008); //  checks if it is on or off
                //K6485_In (dev, cmd);

                //Scan (cmd, "%i", &smu->operate);
				
                //Fmt (cmd, "SOURce:OUTPut:STATe %i", smu->operate);
                 //if (smu->operate==1) K6485_Out (dev,   "SOURce:OUTPut:STATe 0" , .02); 
				 // else   K6485_Out (dev,   "SOURce:OUTPut:STATe 1" , .02);
				//K6485_Out (dev, cmd, .02);
				
				GetCtrlVal (panel, control, &smu->operate);
                Fmt (cmd, "SOURce:OUTPut:STATe %i", smu->operate);
                K6485_Out (dev, cmd, .02);
				//smu->source->sourceOn = !smu->source->sourceOn;
            }
            break;
		 /*case K6485_CLEARBRAINS:
           if (event == EVENT_COMMIT) 
		   {
           //  GetCtrlVal (panel, control, &smu->operate);
			K6485_Out (dev, "*CLS", .008); //  clears out event registers
              //  K6485_In (dev, cmd);

                //Scan (cmd, "%i", &smu->operate);
				
                //Fmt (cmd, "SOURce:OUTPut:STATe %i", smu->operate);
                 //if (smu->operate==1) K6485_Out (dev,   "SOURce:OUTPut:STATe 0" , .02); 
				  //else   K6485_Out (dev,   "SOURce:OUTPut:STATe 1" , .02);
				//K6485_Out (dev, cmd, .02);
				
		//		
		//	smu->source->sourceOn = !smu->source->sourceOn;
          }
          break;
        
		 
		 case K6485_IDENTIFY:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &id);
				if(id)
				{
					K6485_Out(dev, ":*IDN?", .015);
					Fmt(cmd, "*IDN?", dev->paddr);
					K6485_Out(dev, cmd, .5);
				}
				//else
					//K6485_Out(dev, ":DISPlay:WINDow:TEXT:STATe 0", .015);

            }
            break;
			

        /*case K6485_DELAY:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &smu->src.delay);
                Fmt (cmd, ":SOURce:DELay:AUTO %i", smu->src.delay);
                K6485_Out (dev, cmd, .02);
            }
            break; 
        case K6485_SOURCE_V_RANGE:
        case K6485_SOURCE_I_RANGE:
            if (event == EVENT_COMMIT) {
                GetCtrlVal(panel, K6485_SOURCE, &level);
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
				K6485_Out (dev, cmd, .02); 
				src->biaslevel = level;
				K6485_SetLevel(src); 
                K6485_Out (dev, cmd, .02);
                smu->source->min = -r;
                smu->source->max = r;
				if(src->panel)
					source_UpdatePanel(src->panel, src);
            }
            break;		*/
    }

    if (event == EVENT_COMMIT) {
        K6485_UpdateControls(panel, dev);
    } //*/
    return 0;
}


int  K6485MainPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2)
{
    gpibioPtr dev = callbackData;
    K6485Ptr smu = dev->device;
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
        SetMenuBarAttribute (menubar, K6485MENU_FILE_SAVE, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, K6485MENU_FILE_LOAD, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        
        SetMenuBarAttribute (menubar, K6485MENU_MEASURE, ATTR_DIMMED, (dev->status != DEV_REMOTE));

        if (!util_TakingData()) K6485_UpdateControls (panel, dev);
    }
    return 0;
}

static void OperateK6485 (int menubar, int menuItem, void *callbackData, int panel)
{
    int p, m;
    gpibioPtr dev = callbackData;
    K6485Ptr smu = dev->device;
    char label[256];

    SetMenuBarAttribute (menubar, menuItem, ATTR_DIMMED, TRUE);

    p = dev->iPanel? dev->iPanel:LoadPanel (utilG.p, "K6485u.uir", K6485);
	dev->iPanel = p;
    
    SetPanelPos (p, VAL_AUTO_CENTER, VAL_AUTO_CENTER);

    Fmt (label, "K6485 Picoammeter: %s", dev->label);
    SetPanelAttribute (p, ATTR_TITLE, label);

    m = LoadMenuBar (p, "K6485u.uir", K6485MENU);
    
    SetPanelMenuBar (p, m);

    SetMenuBarAttribute (m, K6485MENU_FILE_SAVE, ATTR_CALLBACK_DATA, dev);
    SetMenuBarAttribute (m, K6485MENU_FILE_LOAD, ATTR_CALLBACK_DATA, dev);
    SetMenuBarAttribute (m, K6485MENU_FILE_GPIB, ATTR_CALLBACK_DATA, dev);
    
    SetMenuBarAttribute (m, K6485MENU_MEASURE, ATTR_CALLBACK_DATA, smu->measure);

    SetPanelAttribute (p, ATTR_CALLBACK_DATA, dev);

    //SetCtrlAttribute (p, K6485_SOURCE, ATTR_CALLBACK_DATA, smu->source);

    //SetCtrlAttribute (p, K6485_SOURCE_I_RANGE, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, K6485_SOURCE_V_RANGE, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, K6485_DELAY, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, K6485_IDENTIFY, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K6485_OPERA, ATTR_CALLBACK_DATA, dev);
	//SetCtrlAttribute (p, K6485_CLEARBRAINS, ATTR_CALLBACK_DATA, dev);   
    //SetCtrlAttribute (p, K6485_SELECT, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, K6485_SENSE, ATTR_CALLBACK_DATA, dev);
    
    
    //SetCtrlAttribute (p, K6485_I_LIMIT, ATTR_CALLBACK_DATA, dev);
	//SetCtrlAttribute (p, K6485_V_LIMIT, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, K6485_FILTER, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, K6485_INTTIME, ATTR_CALLBACK_DATA, dev);
	
	// insert current protection here
    K6485_UpdateControls (p, dev);

    devPanel_Add (p, dev, K6485_UpdateReadings);
    DisplayPanel (p);
}
/****************************/
static void K6485_UpdateControls (int panel, gpibioPtr dev)
{
    char s_text[20], m_text[20], cmd[256];
    K6485Ptr smu = dev->device;
    double absmax,r;
	
    K6485_GetOnOffStatus (dev);
    K6485_GetMeasurementParameters (dev);
	K6485_GetReadings(smu->source->acqchan);

   // SetCtrlVal (panel, K6485_SOURCE, smu->source->acqchan->reading);
    SetCtrlVal (panel, K6485_MEASURE, smu->measure->reading);

    SetCtrlVal (panel, K6485_OPERA, smu->operate);		  // controls dimming =gray color enables /disables
  //  SetInputMode (panel, K6485_SOURCE, smu->operate);
    SetInputMode (panel, K6485_MEASURE, smu->operate);

    //SetCtrlVal (panel, K6485_SELECT, smu->src.current);
    //SetCtrlVal (panel, K6485_SENSE, smu->sense);

    /*if (smu->src.current) {
        Fmt (s_text, "A"); Fmt (m_text, "V");   ///needs a fix!!
    } else {
        Fmt (s_text, "V"); Fmt (m_text, "A");
    } 

    SetCtrlVal (panel, K6485_SOURCETEXT, s_text);
    SetCtrlVal (panel, K6485_MEASURETEXT, m_text);

    //SetCtrlAttribute (panel, K6485_SOURCE_I_RANGE, ATTR_VISIBLE, smu->src.current);
    //SetCtrlAttribute (panel, K6485_SOURCE_V_RANGE, ATTR_VISIBLE, !smu->src.current);

    /* if (smu->src.current) {
        SetCtrlIndex (panel, K6485_SOURCE_I_RANGE, smu->src.rangeC);
        GetCtrlVal (panel, K6485_SOURCE_I_RANGE, &smu->source->max);
    } else {
        SetCtrlIndex (panel, K6485_SOURCE_V_RANGE, smu->src.rangeV);
        GetCtrlVal (panel, K6485_SOURCE_V_RANGE, &smu->source->max);
    }  */

   
    // SetCtrlVal (panel, K6485_DELAY, smu->src.delay);

   // SetCtrlAttribute (panel, K6485_MEAS_I_RANGE, ATTR_VISIBLE, !smu->src.current);
    //SetCtrlAttribute (panel, K6485_MEAS_V_RANGE, ATTR_VISIBLE, smu->src.current);

	//GetCtrlVal (panel, K6485_I_LIMIT, &smu->meas.range); 
	//Fmt(cmd, "SOURce:CURRent:LIMit %f", smu->meas.range);
	
	
	
	//K6485_Out (dev, ":SOUR:CURRent:PROTection:LEVel?", .008);   
	
    //SetCtrlVal (panel, K6485_I_LIMIT, K6485_GetCompLevel (dev));
	
    //K6485_In (dev, msg);

    //Scan (msg, "%s>%f", &level)
	//SetCtrlVal (panel, K6485_V_LIMIT, K6485_GetCompLevel (dev)); // need to fix function

    /*if (smu->src.current) {
        SetCtrlIndex (panel, K6485_MEAS_V_RANGE, smu->meas.range);
        GetCtrlVal (panel, K6485_MEAS_V_RANGE, &absmax);
    } else {
        SetCtrlIndex (panel, K6485_MEAS_I_RANGE, smu->meas.range);
        GetCtrlVal (panel, K6485_MEAS_I_RANGE, &absmax);
    }  */

   // SetCtrlAttribute (panel, K6485_I_LIMIT, ATTR_MIN_VALUE, 0.0);
   // SetCtrlAttribute (panel, K6485_I_LIMIT, ATTR_MAX_VALUE, 1.0);
	
	//SetCtrlAttribute (panel, K6485_V_LIMIT, ATTR_MIN_VALUE, 0.0);
   // SetCtrlAttribute (panel, K6485_V_LIMIT, ATTR_MAX_VALUE, 65.0);


    

}

static void K6485_Remove (void *ptr)
{
    K6485Ptr smu = ptr;
    acqchan_Remove(smu->measure);
   
    free (smu);
}

/*static void K6485_Load (gpibioPtr dev)
{
    char cmd[256];
    double r;
    int i;
    K6485Ptr smu;
	//sourcePtr src;

    if (dev){
		smu = dev->device;
		
    else smu = K6485_Create (NULL);}

/*  if (dev) K6485_Out (dev, "N0X");*/

   // ScanFile (fileHandle.analysis, "%s>Sense   : %i%s[dw1]", &smu->sense);
   // if (dev) {Fmt (cmd, ":SYSTem:RSENse %i", smu->sense); K6485_Out (dev, cmd, 0);}

    //ScanFile (fileHandle.analysis, "%s>Source  : %i, %i%s[dw1]",
     //         &smu->src.current, &smu->src.delay);
    //if (dev) {
	//	if(smu->src.current)
	//	{
	//		K6485_Out(dev, ":SOURce:FUNCtion:MODE CURRent", .07); 
	//		K6485_Out(dev, ":SENSe:FUNCtion \"VOLTage\"", .07); 
	//	}
	//	else
	//	{
	//		K6485_Out(dev, ":SOURce:FUNCtion:MODE VOLTage", .07); 
			//K6485_Out(dev, ":SENSe:FUNCtion \"CURRent\"", .07); 
		//}
	//	smu->source->sourceOn = 0;
     //   Fmt (cmd, ":SOURce:DELay:AUTO %i", smu->src.delay);
    //    K6485_Out (dev, cmd, 0);
   // }
   // smu->src.rangeV = 0;
	//smu->src.rangeC = 0;/*AUTO*/
    //smu->source->min = 0;
   // smu->source->max = 60.0;
	//K6485_Out(dev,":OUTPut:STATe?",0);
	//K6485_In(dev,cmd);
	//Scan(cmd, "%i", &smu->source->sourceOn);
    //source_Load (dev, smu->source);

    //ScanFile (fileHandle.analysis, "%s>Measure : %f, %i, %i, %f\0",
     //        &r, &smu->meas.range, &smu->meas.filter, &smu->meas.inttime); needs a fix!!
   /* if (dev) {
	//	Fmt (cmd, "SENSe:CURRent:NPLCycles %f", smu->meas.inttime);
	//	K6485_Out (dev, cmd, 0);
	//	Fmt (cmd, "SENSe:AVERage:COUNt %i", smu->meas.filter);
     //   K6485_Out (dev, cmd, 0);
   // }

    acqchan_Load (dev, smu->measure);}

   // if (!dev) K6485_Remove (smu);
}

static void K6485_Save (gpibioPtr dev)
{
    K6485Ptr smu = dev->device;

    K6485_GetOnOffStatus(dev);
    K6485_GetMeasurementParameters(dev);

    FmtFile (fileHandle.analysis, "Sense   : %i\n", smu->sense);
    FmtFile (fileHandle.analysis, "Source  : %i, %i\n", smu->src.current, smu->src.delay);
    source_Save (smu->source);
   // FmtFile (fileHandle.analysis, "Measure : %f[e2p5], %i, %i, %f\n",
             //K6485_GetICompLevel (dev), smu->meas.range,
    //         smu->meas.filter, smu->meas.inttime);	   needs fix!
    acqchan_Save (smu->measure);
}*/
static void k6485_Load (gpibioPtr dev)
{
    char cmd[256];
    double r;
    int i, scanCard;
    K6485Ptr dmm;

    if (dev) dmm = dev->device;
    else dmm = k6485_Create (NULL);

    ScanFile (fileHandle.analysis, "%s>Scan Card: %i%s[dw1]", &scanCard);

    k6485_LoadChannel (0, dev);
    

    if (!dev) k6485_Remove (dmm);
}

static void k6485_Save (gpibioPtr dev)
{
    k6485Ptr dmm = dev->device;
    int i;

    FmtFile (fileHandle.analysis, "Scan Card: %i\n", dmm->scanCard);
    k6485_SaveChannel (dmm->chan[0]);
    
    }
}


static double K6485_GetICompLevel (gpibioPtr dev)
{
    char msg[256];
    double level;
	
	K6485Ptr smu = dev->device;
	
//	if (smu->src.current)
	//	K6485_Out (dev, "SOUR:VOLTage:LEVel?", .008)	   /// needs a fix!!!
	; 
//	else
    	K6485_Out (dev, ":SOUR:CURRent:PROTection:LEVel?", .008);
    K6485_In (dev, msg);

    Scan (msg, "%s>%f", &level);
    return level;
}

static double K6485_GetVCompLevel (gpibioPtr dev)
{
    char msg[256];
    double level;
	
	K6485Ptr smu = dev->device;
	
//	if (smu->src.current)
	//	K6485_Out (dev, "SOUR:VOLTage:LEVel?", .008)	   /// needs a fix!!!
	; 
//	else
    	K6485_Out (dev, ":SOUR:VOLTage:LIMI:LEVel?", .008);
    K6485_In (dev, msg);

    Scan (msg, "%s>%f", &level);
    return level;
}

static void K6485_GetMeasurementParameters (gpibioPtr dev)
{
    char msg[256];
    int srctype;
    K6485Ptr smu = dev->device;
	
	
	/*Check Source mode*
    K6485_Out(dev, ":SOURce:FUNCtion:MODE?", .008);  //no such command!!
    K6485_In(dev, msg);
	if(msg[0] == 'V')
		smu->src.current = 0;
	else
		smu->src.current = 1;
	
	/*Check for remote/local sense*
	K6485_Out(dev, ":SYStem:RSENse?", .008);	   // not plan to use
	K6485_In(dev, msg);
	Scan(msg, "%i", &smu->sense);
	
	/*Check Filter steps*
	K6485_Out(dev, ":SENSe:AVERage:COUNt?", .008);		//no such command!!
	K6485_In(dev, msg);
	Scan(msg, "%i", &smu->meas.filter);
	
	/*Check Integration time*
	K6485_Out(dev, ":SENSe:CURRent:NPLCycles?", .008);
	K6485_In(dev, msg);
	Scan(msg, "%f", &smu->meas.inttime);
	
	/*Check Delay time*
	K6485_Out(dev, ":SOURce:DELay:AUTO?", .008);
	K6485_In(dev, msg);
	Scan(msg, "%i", &smu->src.delay);
										  
    //Scan (msg, "%s[i3]>L,%i", &smu->meas.range);
	*/
}

static void K6485_GetOnOffStatus (gpibioPtr dev)
{
    char msg[256];
    K6485Ptr smu = dev->device;

    K6485_Out (dev, "SOURce:OUTPut:STATe?", .008); // different from Keithley checks if it is on or off
    K6485_In (dev, msg);

    Scan (msg, "%i", &smu->operate);
}

static void K6485_SetLevel (sourcePtr src)
{
    char cmd[256];
	
	gpibioPtr dev = src->acqchan->dev;
	
	//if(K6485_GetMode(dev))
   	//	Fmt(cmd, ":SOURce:CURRent:LEVel %f", src->biaslevel); /* trigger new source value */
//	else
		Fmt(cmd, "SOURce:VOLTage:LEVel %f", src->biaslevel);
    K6485_Out (dev, cmd, .02);
    util_Delay (src->segments[src->seg]->delay);
}

static void K6485_GetReadings (acqchanPtr acqchan)
{
	char msg[256];
    gpibioPtr dev = acqchan->dev;
	//K6485Ptr smu = dev->device;
	//K6485_Out(dev, ":OUTPut:STATe?", 0.02);
	K6485_In(dev, msg);
	Scan(msg, "%i", &smu->source->sourceOn);
	if (smu->source->sourceOn)
		K6485_GetLevels (dev);
}
/****************** needs work*************/
static void K6485_GetLevels (gpibioPtr dev)
{
    char msg[256];
    short statusbyte;
    K6485Ptr smu = dev->device;
	
	K6485_Out(dev, "MEASure:SCALar:CURRent?", .002);   // request to read applied current  
	K6485_In (dev, msg);
    Scan (msg, "%f",  &smu->measure->reading); 
	
	K6485_Out(dev, "MEASure:SCALar:VOLTage?", .002); // request to read applied voltage 
	K6485_In (dev, msg);
    Scan (msg, "%f", &smu->source->acqchan->reading); 
	
    smu->source->acqchan->newreading = TRUE;
    smu->measure->newreading = TRUE;
}

static void K6485_CheckforProblems (gpibioPtr dev)
{
	char msg[256];
    short statusbyte;
    K6485Ptr smu = dev->device;

    gpibio_GetStatusByte (dev, &statusbyte);

    if (statusbyte & K6485_SRE_WARNING) K6485_GetWarningStatus (dev);
    if (statusbyte & K6485_SRE_ERROR) K6485_GetErrorStatus (dev);

    if (gpibio_SRQ(dev)) gpibio_GetStatusByte (dev, &statusbyte);
    smu->overLimit = statusbyte & K6485_SRE_COMPLIANCE;
	//*/
}

static void K6485_Out (gpibioPtr dev, char *cmd, double delay)
{
	gpibio_Out (dev, cmd);
	//Delay(delay);
}

static void K6485_In (gpibioPtr dev, char *msg)
{
	gpibio_In (dev, msg);
	//Delay(.02);
}

static void K6485_GetWarningStatus (gpibioPtr dev)
{
	int i, byte;
    char msg[256], rsp[256];

    K6485_Out (dev, "U9X", .008);
    K6485_In (dev, rsp);

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

static void K6485_GetErrorStatus (gpibioPtr dev)
{
    int i, byte;
    char msg[256], rsp[256];

    K6485_Out (dev, "U1X", .008);
    K6485_In (dev, rsp);

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
/*int K6485_GetMode (gpibioPtr dev)
{
	char msg[256];
	K6485_Out(dev, ":SOURce:FUNCtion:MODE?", 0.008);
	K6485_In(dev, msg);
	if(msg[0] == 'V')
		return 0;
	return 1;
}*/


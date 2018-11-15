
//modified 3/10/2017
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
#include "LS340.h"
#include "LS340u.h"

#define TRUE 1
#define FALSE 0
#define LS340_ID "LSCI"

typedef enum {SEN_A, SEN_B, SEN_C, SEN_D} LS340_inputs;
typedef enum {KELVIN, CELSIUS, SEN_UNITS} units;
typedef struct{
    acqchanPtr channels[4];   // 4 sensors
    sourcePtr source;
    int id;
    struct {int loop, units, power, maxpower, powerup, on;
        char *input;
        double setplimit, pchange, nchange, current, setpoint, rampspeed;} heater;
    struct {double p, i, d; int pon, ion, don;} pid;
    //struct {int on; double level;} alarm;
    struct {char *serial, *format; int source, target, file;} curveload;
} LS340Type;
    
typedef LS340Type *LS340Ptr;

/*******************************index*********************************/
void GetHeaterLvl(acqchanPtr acqchan);
void SetHeaterLvl(sourcePtr src);
void LS340_GetABCD (gpibioPtr dev);
void LS340_GetReadings (acqchanPtr acqchan);    
void GetSensor(acqchanPtr acqchan);
      
void LS340_UpdateSensorReadings (int panel, void *ptr);
void LS340_UpdateHeaterSettings(int panel, gpibioPtr dev);
//void LS340_UpdateControls(int p, gpibioPtr dev);
//int  LS340PanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2);
int  LS340ControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  LS340HeatControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  LS340SensorControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  LS340SendCurve (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void LS340menuCallack (int menuBar, int menuItem, void *callbackData, int panel);

void   LS340_GetReadings (acqchanPtr acqchan); 

void *LS340_Create(gpibioPtr dev);
int  LS340_InitGPIB(gpibioPtr dev);
void OperateLS340(int menubar, int menuItem, void *callbackData, int panel);
void LS340_UpdateReadings(int panel, void *ptr);
void LS340_Save(gpibioPtr dev);
void LS340_Load(gpibioPtr dev);
void LS340_Remove(void *ptr);
void LS340_Init(void);
void LS340_init_menus_and_controls (int menubar, int menuItem, void *callbackData, int panel);


/*******************************Communication functions*********************************/

void GetHeaterLvl(acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    LS340Ptr ls = dev->device;
    char msg[10];
    
    Fmt(msg, "SETP? %i\n", ls->heater.loop);
    acqchan->reading = gpib_GetDoubleVal(dev, msg);
    acqchan->newreading = TRUE;
}

void SetHeaterLvl(sourcePtr src)
{
    gpibioPtr dev = src->acqchan->dev;
    LS340Ptr ls = dev->device;
    char msg[30];
    gpibPrint(dev, "SETP %i, %f", &ls->heater.loop, &src->biaslevel);
    util_Delay(src->segments[src->seg]->delay);
}
void LS340_GetReadings (acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    LS340Ptr ls = dev->device;

    LS340_GetABCD (dev);
}
void GetSensor(acqchanPtr acqchan)
{     
    gpibioPtr dev = acqchan->dev;
    LS340Ptr ls = dev->device;
  //  int alarm = 0;
    char *sens_name="", msg[10];
    if(!strcmp(acqchan->channel->label, "sorb")) sens_name = "A";
    if(!strcmp(acqchan->channel->label, "1 k pot")) {sens_name = "B"; //alarm = 1;
	}
    if(!strcmp(acqchan->channel->label, "He 3 pot")) sens_name = "C";
	if(!strcmp(acqchan->channel->label, "Sensor D")) sens_name = "D";      
    Fmt(msg, "KRDG? %s", sens_name);
    acqchan->reading = gpib_GetDoubleVal(dev, msg);
    acqchan->newreading = TRUE;
   /* if(alarm && ls->alarm.on)
    {
        if(acqchan->reading > ls->alarm.level)
        {
            Beep();
            SetCtrlVal(dev->iPanel, LS340_CTRL_ALARMLED, 1);
        }
        else
            SetCtrlVal(dev->iPanel, LS340_CTRL_ALARMLED, 0);
    }  */
}

/*void newGetSensor(acqchanPtr acqchan)
{     
    gpibioPtr dev = acqchan->dev;
    LS340Ptr ls = dev->device;
  //  int alarm = 0;
    char *sens_name="", msg[10];
    if(!strcmp(acqchan->channel->label, "sorb")) sens_name = "A";
    if(!strcmp(acqchan->channel->label, "1 k pot")) {sens_name = "B"; //alarm = 1;
	}
    if(!strcmp(acqchan->channel->label, "He 3 pot")) sens_name = "C";
	if(!strcmp(acqchan->channel->label, "Sensor D")) sens_name = "D";      
    
	for (i=0; i<4; i++)
	  {Fmt(msg, "KRDG? %s", i);
      acqchan->reading = gpib_GetDoubleVal(dev, msg);
      acqchan->newreading = TRUE;
   
} */
void LS340_GetABCD(gpibioPtr dev)
{     
    char msg[256]; // int i;
    LS340Ptr ls = dev->device;
	LS340_inputs chan;
    for (chan=0; chan<4; chan++) 
	 {   //i=chan;
		 Fmt(msg, "KRDG? %i", chan);
         ls->channels[chan]->reading=gpib_GetDoubleVal(dev, msg);
	 }   
     for (chan = 0; chan < 4; chan++) ls->channels[chan]->newreading = TRUE;
} 

/*void LS340_UpdateReadings(int panel, void *ptr)
{   int m;
    gpibioPtr dev = ptr;
    LS340Ptr ls = dev->device;
    GetSensor(ls->channels[SEN_A]);
    GetSensor(ls->channels[SEN_B]);
    GetSensor(ls->channels[SEN_C]);
	GetSensor(ls->channels[SEN_D]); 
	
    SetCtrlVal(panel, LS340_CTRL_SORBREAD, ls->channels[SEN_A]->reading);
    SetCtrlVal(panel, LS340_CTRL_KPOTREAD, ls->channels[SEN_B]->reading); //error with panel ID here was due to Discard vs Hide
    SetCtrlVal(panel, LS340_CTRL_HE3PREAD, ls->channels[SEN_C]->reading);
	SetCtrlVal(panel, LS340_CTRL_SEN_D_READ,ls->channels[SEN_D]->reading);
	if(utilG.acq.status == ACQ_BUSY)
        HidePanel(panel);
}
*/
void LS340_UpdateReadings (int panel, void *dev)
{
    gpibioPtr my_dev = dev;
    LS340Ptr ls = my_dev->device; 
    int m;
    if (!util_TakingData()             ||
        !(
		  ls->channels[SEN_A]->acquire ||
          ls->channels[SEN_B]->acquire ||
          ls->channels[SEN_C]->acquire ||
          ls->channels[SEN_D]->acquire 
		 )
	   ) 
		LS340_GetABCD (my_dev);

     if (expG.acqstatus != utilG.acq.status) {
        m = GetPanelMenuBar (panel);
        SetMenuBarAttribute (m, LS340MENU_MEASURE, ATTR_DIMMED, util_TakingData());
        //SetMenuBarAttribute (m, LS340MENU_FILE_LOAD, ATTR_DIMMED, util_TakingData());
    }

    SetCtrlVal(panel, LS340_CTRL_SORBREAD, ls->channels[SEN_A]->reading);
    SetCtrlVal(panel, LS340_CTRL_KPOTREAD, ls->channels[SEN_B]->reading); //error with panel ID here was due to Discard vs Hide ???
    SetCtrlVal(panel, LS340_CTRL_HE3PREAD, ls->channels[SEN_C]->reading);
	SetCtrlVal(panel, LS340_CTRL_SEN_D_READ,ls->channels[SEN_D]->reading);
	//if(utilG.acq.status == ACQ_BUSY)  HidePanel(panel);
    
}   
/******************************Callback Functions**********************************/
void LS340_UpdateSensorTabReadings (int panel, void *ptr) // only works when sensor panel is open
{
    LS340Ptr ls = ptr;
    SetCtrlVal(panel, LS340_SENS_SORBMEAS, ls->channels[SEN_A]->reading);
    SetCtrlVal(panel, LS340_SENS_KPOTMEAS, ls->channels[SEN_B]->reading);
    SetCtrlVal(panel, LS340_SENS_HE3PMEAS, ls->channels[SEN_C]->reading);
	SetCtrlVal(panel, LS340_SENS_SEN_D_MEAS, ls->channels[SEN_D]->reading);
if(utilG.acq.status == ACQ_BUSY)   HidePanel(panel);
}

void old_LS340_UpdateReadings(int panel, void *ptr)
{   int m;
    gpibioPtr dev = ptr;
    LS340Ptr ls = dev->device;
    GetSensor(ls->channels[SEN_A]);
    GetSensor(ls->channels[SEN_B]);
    GetSensor(ls->channels[SEN_C]);
	GetSensor(ls->channels[SEN_D]); 
	
    SetCtrlVal(panel, LS340_CTRL_SORBREAD, ls->channels[SEN_A]->reading);
    SetCtrlVal(panel, LS340_CTRL_KPOTREAD, ls->channels[SEN_B]->reading); //error with panel ID here was due to Discard vs Hide
    SetCtrlVal(panel, LS340_CTRL_HE3PREAD, ls->channels[SEN_C]->reading);
	SetCtrlVal(panel, LS340_CTRL_SEN_D_READ, ls->channels[SEN_D]->reading);
}

void LS340_UpdateHeaterSettings(int panel, gpibioPtr dev)
{
    char msg[260];
    LS340Ptr ls = dev->device;
    gpibPrint(dev, "CSET? %i\n", &ls->heater.loop);
    gpibio_In(dev, msg);
    Scan(msg, "%s[w1],%i,%i,%i", ls->heater.input, &ls->heater.units, &ls->heater.on, &ls->heater.powerup);
    gpibPrint(dev, "CLIMIT? %i\n", &ls->heater.loop);
    gpibio_In(dev, msg);
    Scan(msg, "%f,%f,%f,%f,%i", &ls->heater.setplimit, &ls->heater.pchange, &ls->heater.nchange, &ls->heater.current, &ls->heater.maxpower);
    gpibPrint(dev, "PID? %i\n", &ls->heater.loop);
    gpibio_In(dev, msg);
    Scan(msg, "%f,%f,%f", &ls->pid.p, &ls->pid.i, &ls->pid.d);
}

void LS340_InitControls(int p, gpibioPtr dev)
{
    LS340Ptr ls = dev->device;
    char msg[260]; int status;
    
    ls->heater.power = gpib_GetIntVal(dev, "RANGE?");
	if  (ls->heater.power)  SetCtrlVal(p, LS340_CTRL_HEATER, 1); else SetCtrlVal(p, LS340_CTRL_HEATER, 0);   
    Fmt(msg, "SETP? %i", ls->heater.loop);
    ls->heater.setpoint = gpib_GetDoubleVal(dev, msg);
    Fmt(msg, "RAMP? %i", ls->heater.loop);
    gpib_GetCharVal(dev, msg, msg);
    Scan(msg, "%i,%f", &status, &ls->heater.rampspeed);  //needs to be instead of status &ls->heater.ramp_status
    //gpibPrint(dev, "RANGE 0\n");// this  shuts heater off
	
    SetCtrlVal(p, LS340_CTRL_POWER, ls->heater.power);
    //SetCtrlVal(p, LS340_CTRL_HEATER, ls->heater.on);
	
	//ls->heater.on=0;
    SetCtrlVal(p, LS340_CTRL_SORBTSET, ls->heater.setpoint);
    SetCtrlAttribute(p, LS340_CTRL_SORBTSET, ATTR_MAX_VALUE, ls->heater.setplimit);
    SetCtrlVal(p, LS340_CTRL_RAMPSPEED, ls->heater.rampspeed);
    //SetCtrlVal(p, LS340_CTRL_ALARM, ls->alarm.on);
    //SetCtrlVal(p, LS340_CTRL_ALARMLVL, ls->alarm.level);
}

int LS340ControlPanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2)
{  
    gpibioPtr dev;
    LS340Ptr ls; 
    int menubar;
    if ((event == EVENT_KEYPRESS && eventData1 == VAL_ESC_VKEY) || (event == EVENT_RIGHT_DOUBLE_CLICK))
    {   
		gpibioPtr dev = callbackData; // needs to be here or else hidden panel is constantly updated    
		LS340Ptr ls = dev->device;// no need for it, but it does not crash the code
        devPanel_Remove (panel);
        DiscardPanel (panel);  //HidePanel (panel); 
        dev->iPanel = 0;
        SetMenuBarAttribute (acquire_GetMenuBar(), dev->menuitem_id, ATTR_DIMMED, FALSE);

    }
	if (event == EVENT_GOT_FOCUS) {
        dev = callbackData;
        menubar = GetPanelMenuBar (panel);
        SetPanelAttribute (panel, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, LS340MENU_CURVES, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, LS340MENU_SOURCE, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, LS340MENU_MEASURE, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        //SetMenuBarAttribute (menubar, SR830MENU_FILE_LOAD, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        if (!util_TakingData()) LS340_UpdateHeaterSettings (panel, dev);
    }
    
    return 0;
}
/*int  SR830ControlPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2)
{
    gpibioPtr dev;
    sr830Ptr lia;
    int menubar;
    if ((event == EVENT_KEYPRESS) && (eventData1 == VAL_ESC_VKEY) || event == EVENT_RIGHT_DOUBLE_CLICK) {
        dev = callbackData;
        lia = dev->device;
        devPanel_Remove (panel);
        DiscardPanel (panel);   
		dev->iPanel = 0;
		SetMenuBarAttribute (acquire_GetMenuBar(), dev->menuitem_id, ATTR_DIMMED, FALSE);
    }

    if (event == EVENT_GOT_FOCUS) {
        dev = callbackData;
        menubar = GetPanelMenuBar (panel);
        SetPanelAttribute (panel, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, SR830MENU_SOURCES, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, SR830MENU_MEASURE, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, SR830MENU_FILE_SAVE, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, SR830MENU_FILE_LOAD, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        if (!util_TakingData()) sr830_UpdateControls (panel, dev);
    }
    return 0;
}  */
int LS340ControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    gpibioPtr dev = callbackData;
    LS340Ptr ls = dev->device;
    char msg[260];
    switch(control)
    {
		 
        case LS340_CTRL_HEATER:
            if (event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->heater.on);
                if(ls->heater.on)
                {
                    GetCtrlVal(panel, LS340_CTRL_POWER, &ls->heater.power);
                    GetCtrlVal(panel, LS340_CTRL_SORBTSET, &ls->heater.setpoint);
                    GetCtrlVal(panel, LS340_CTRL_RAMPSPEED, &ls->heater.rampspeed);
                    gpibPrint(dev, "SETP %i, %f\n", &ls->heater.loop, &ls->heater.setpoint);
                    gpibPrint(dev, "RAMP %i, 1, %f\n", &ls->heater.loop, &ls->heater.rampspeed);
                    gpibPrint(dev, "RANGE %i\n", &ls->heater.power);
                }
                else
                    gpibPrint(dev, "RANGE 0\n");
            }
            break;
        case LS340_CTRL_HEAT_PROP:
            if (event == EVENT_COMMIT)
            {
                int heater = LoadPanel(utilG.p, "LS340u.uir", LS340_HEAT);
                SetCtrlAttribute(heater, LS340_HEAT_DON, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, LS340_HEAT_ION, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, LS340_HEAT_PON, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, LS340_HEAT_ACCEPT, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, LS340_HEAT_RESET, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, LS340_HEAT_P, ATTR_DIMMED, !ls->pid.pon);
                SetCtrlAttribute(heater, LS340_HEAT_I, ATTR_DIMMED, !ls->pid.ion);
                SetCtrlAttribute(heater, LS340_HEAT_D, ATTR_DIMMED, !ls->pid.don);
				//GetCtrlVal(panel, LS340_HEAT_POWERUP, &ls->heater.powerup);   what is this for?
                
                LS340_UpdateHeaterSettings(heater, dev);
                SetCtrlVal(heater, LS340_HEAT_LOOPNUM,  ls->heater.loop);
                SetCtrlVal(heater, LS340_HEAT_SETPLIM,  ls->heater.setplimit);
                SetCtrlVal(heater, LS340_HEAT_PCHANGE,  ls->heater.pchange);
                SetCtrlVal(heater, LS340_HEAT_NCHANGE,  ls->heater.nchange);
                SetCtrlVal(heater, LS340_HEAT_CURRENT,  ls->heater.current);
                SetCtrlVal(heater, LS340_HEAT_POWERUP,  ls->heater.powerup);
                SetCtrlVal(heater, LS340_HEAT_MXPOWER,  ls->heater.maxpower);
                SetCtrlVal(heater, LS340_HEAT_INPUTNM,  ls->heater.input);
                SetCtrlVal(heater, LS340_HEAT_UNITS,    ls->heater.units);
                SetCtrlVal(heater, LS340_HEAT_P,        ls->pid.p);
                SetCtrlVal(heater, LS340_HEAT_PON,      ls->pid.pon);
                SetCtrlVal(heater, LS340_HEAT_I,        ls->pid.i);
                SetCtrlVal(heater, LS340_HEAT_ION,      ls->pid.ion);
                SetCtrlVal(heater, LS340_HEAT_D,        ls->pid.d);
                SetCtrlVal(heater, LS340_HEAT_DON,      ls->pid.don);
                DisplayPanel(heater);
            }
            break;
        case LS340_CTRL_SORBTSET:
            if (event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->heater.setpoint);
                gpibPrint(dev, "SETP %i, %f\n", &ls->heater.loop, &ls->heater.setpoint);
            }
            break;
        case LS340_CTRL_RAMPSPEED:
            if (event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->heater.rampspeed);
                gpibPrint(dev, "RAMP %i, %i, %f\n", &ls->heater.loop, &ls->heater.on, &ls->heater.rampspeed);
            }
            break;
        case LS340_CTRL_POWER:
            if (event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->heater.power);
                gpibPrint(dev, "RANGE %i\n", &ls->heater.power);
            }
            break;
       
        
    }
    return 0;
}

int LS340HeatControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    gpibioPtr dev = callbackData;
    LS340Ptr ls = dev->device;
    switch (control)
    {
        case LS340_HEAT_PON:
            if(event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->pid.pon);
                SetCtrlAttribute(panel, LS340_HEAT_P, ATTR_DIMMED, !ls->pid.pon);
            }
            break;
        case LS340_HEAT_ION:
            if(event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->pid.ion);
                SetCtrlAttribute(panel, LS340_HEAT_I, ATTR_DIMMED, !ls->pid.ion);
            }
            break;
        case LS340_HEAT_DON:
            if(event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->pid.don);
                SetCtrlAttribute(panel, LS340_HEAT_D, ATTR_DIMMED, !ls->pid.don);
            }
            break;
        case LS340_HEAT_ACCEPT:
            if(event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, LS340_HEAT_LOOPNUM, &ls->heater.loop);
                GetCtrlVal(panel, LS340_HEAT_SETPLIM, &ls->heater.setplimit);
                GetCtrlVal(panel, LS340_HEAT_PCHANGE, &ls->heater.pchange);
                GetCtrlVal(panel, LS340_HEAT_NCHANGE, &ls->heater.nchange);
                GetCtrlVal(panel, LS340_HEAT_CURRENT, &ls->heater.current);
                GetCtrlVal(panel, LS340_HEAT_MXPOWER, &ls->heater.maxpower);
                GetCtrlVal(panel, LS340_HEAT_POWERUP, &ls->heater.powerup);
                GetCtrlVal(panel, LS340_HEAT_INPUTNM, ls->heater.input);   
                GetCtrlVal(panel, LS340_HEAT_UNITS,   &ls->heater.units);  //*
                SetCtrlAttribute(dev->iPanel, LS340_CTRL_SORBTSET, ATTR_MAX_VALUE, ls->heater.setplimit);
                gpibPrint(dev, "CSET %i, %s, %i, %i, %i\n", &ls->heater.loop,
                                                            ls->heater.input,
                                                            &ls->heater.units,
                                                            &ls->heater.on,
                                                            &ls->heater.powerup);
                gpibPrint(dev, "CLIMIT %i, %f, %f, %f, %f, %i\n",   &ls->heater.loop,
                                                                    &ls->heater.setplimit,
                                                                    &ls->heater.pchange,
                                                                    &ls->heater.nchange,
                                                                    &ls->heater.current,
                                                                    &ls->heater.maxpower);//*/
                GetCtrlVal(panel, LS340_HEAT_P, &ls->pid.p);
                GetCtrlVal(panel, LS340_HEAT_I, &ls->pid.i);
                GetCtrlVal(panel, LS340_HEAT_D, &ls->pid.d);
                if(ls->pid.pon)
                {
                    gpibPrint(dev, "PID %i, %f[, %f][, %f]\n",  &ls->heater.loop,
                                                            &ls->pid.p,
                                                            ls->pid.ion,
                                                            &ls->pid.i,
                                                            (ls->pid.ion && ls->pid.don),
                                                            &ls->pid.d);
                }           
            }
            break;
        case LS340_HEAT_RESET:
            if(event == EVENT_COMMIT)
            {
                SetCtrlVal(panel, LS340_HEAT_LOOPNUM, ls->heater.loop);
                SetCtrlVal(panel, LS340_HEAT_SETPLIM, ls->heater.setplimit);
                SetCtrlVal(panel, LS340_HEAT_PCHANGE, ls->heater.pchange);
                SetCtrlVal(panel, LS340_HEAT_NCHANGE, ls->heater.nchange);
                SetCtrlVal(panel, LS340_HEAT_CURRENT, ls->heater.current);
                SetCtrlVal(panel, LS340_HEAT_MXPOWER, ls->heater.maxpower);
                SetCtrlVal(panel, LS340_HEAT_POWERUP, ls->heater.powerup);
                SetCtrlVal(panel, LS340_HEAT_INPUTNM, ls->heater.input);   
                SetCtrlVal(panel, LS340_HEAT_UNITS,   ls->heater.units);  //*
                if(ls->pid.pon)
                    SetCtrlVal(panel, LS340_HEAT_P, ls->pid.p);
                if(ls->pid.ion)
                    SetCtrlVal(panel, LS340_HEAT_I, ls->pid.i);
                if(ls->pid.don)
                    SetCtrlVal(panel, LS340_HEAT_D, ls->pid.d);
                
            }
            break;
        
    }
    return 0;
}

int  LS340SensorControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)  // panel where coefficents are set and acq channels chosen
{
    acqchanPtr acqchan;
    acqchan = callbackData;
	 
    switch (control) {
        case LS340_SENS_NOTE_1:
        case LS340_SENS_NOTE_2:
        case LS340_SENS_NOTE_3:
		case LS340_SENS_NOTE_4:	
            AcqDataNoteCallback (panel, control, event, callbackData, eventData1, eventData2);
            break;
        case LS340_SENS_SORBACQ:
        case LS340_SENS_KPOTACQ:
        case LS340_SENS_HE3PACQ:
		case LS340_SENS_SEN_D_ACQ:	
            if (event == EVENT_VAL_CHANGED) {
                GetCtrlVal (panel, control, &acqchan->acquire);
                if (acqchan->acquire) acqchanlist_AddChannel (acqchan);
                    else acqchanlist_RemoveChannel (acqchan);
            }
            break;
        
        case LS340_SENS_SORBLABEL:
        case LS340_SENS_KPOTLABEL:
        case LS340_SENS_HE3PLABEL:
		case LS340_SENS_SEN_D_LABEL:	 	
			
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, acqchan->channel->label);
                acqchanlist_ReplaceChannel (acqchan);
                if (acqchan->p) SetPanelAttribute (acqchan->p, ATTR_TITLE, acqchan->channel->label);
            }
            break;
        /*case LS340_SENS_CLOSE:
            if (event == EVENT_COMMIT) 
				
				
                HidePanel (panel);
            break;  */
		case LS340_SENS_CLOSE:
            if (event == EVENT_COMMIT) {
                //lia = callbackData;
                devPanel_Remove(panel);
                DiscardPanel (panel);
            }
    }
   //updateGraphSource();  // what is this for?
    return 0;
}

int LS340SendCurve (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    if (event == EVENT_COMMIT)
    {
        char *curve="", *buffer, vars[30];
        gpibioPtr dev = callbackData;
        LS340Ptr ls = dev->device;
        
        GetCtrlVal(panel, LS340CURVE_CURVESRC, &ls->curveload.source);
        GetCtrlVal(panel, LS340CURVE_CURVENUM, &ls->curveload.target);
        GetCtrlVal(panel, LS340CURVE_SERIAL,   ls->curveload.serial);
        
        do{
            ScanFile(ls->curveload.file, "%s", vars);
            buffer = malloc(sizeof(char) * StringLength(curve) + sizeof(vars));
            Fmt(buffer, "%s%s", curve, vars);
            curve = buffer;
        }while(*vars);
        gpibPrint(dev, "SCAL %i, %i, %s, %s\n", &ls->curveload.source,
                                                &ls->curveload.target,
                                                ls->curveload.serial,
                                                curve);
    }
    return 0;
}

void LS340menuCallback (int menuBar, int menuItem, void *callbackData, int panel)
{
    switch(menuItem)
    {
        case LS340MENU_CURVES_LOAD:
        {
            gpibioPtr dev = callbackData;
            LS340Ptr ls = dev->device;
            char pathname[260];
            int cPanel, i = FileSelectPopup ("", "*.dat", "*.dat", "Custom Curve", VAL_LOAD_BUTTON,
                                 0, 0, 1, 0, pathname);
            if(i)
            {
                ls->curveload.file = OpenFile (pathname, VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_ASCII);
                cPanel = LoadPanel(utilG.p, "LS340u.uir", LS340CURVE);
	/*			while (!feof(file_handle) && i < 200) 
	 	{	
	 		fscanf (file_handle,"%f %f", &x, &y );
	 		In_Array[0][i]=x;
	 		In_Array[1][i]=y;
	 		i++;
		}
		fclose (file_handle);CurveLength = i; 
	}*/
                
                SetCtrlAttribute(cPanel, LS340CURVE_ACCEPT, ATTR_CALLBACK_DATA, dev);
                
                SetCtrlVal(cPanel, LS340CURVE_SERIAL,    ls->curveload.serial);
                SetCtrlVal(cPanel, LS340CURVE_CURVESRC,  ls->curveload.source);
                SetCtrlVal(cPanel, LS340CURVE_CURVENUM,  ls->curveload.target);
                
                DisplayPanel(cPanel);
            }
        }
        break;
        case LS340MENU_SOURCE_HEATER:
        {
            sourcePtr src = callbackData;
            switch(utilG.exp)
            {
                case EXP_SOURCE: source_InitPanel(src); break;
                case EXP_FLOAT : gensrc_InitPanel(src); break;
            }
        }
        break;
        case LS340MENU_MEASURE_SENSOR_SETTINGS:
        {
            
            int p = LoadPanel (utilG.p, "LS340u.uir", LS340_SENS);
			SetPanelPos (p, VAL_AUTO_CENTER, VAL_AUTO_CENTER);
            util_InitClose (p, LS340_SENS_CLOSE, FALSE); //to use a popup
			LS340Ptr ls = callbackData;   
			
            SetCtrlVal (p, LS340_SENS_SORBLABEL, ls->channels[SEN_A]->channel->label);
            
            SetCtrlVal (p, LS340_SENS_SORBACQ,   ls->channels[SEN_A]->acquire);
            SetCtrlVal (p, LS340_SENS_NOTE_1,    ls->channels[SEN_A]->note);
                                  
            SetCtrlVal (p, LS340_SENS_KPOTLABEL, ls->channels[SEN_B]->channel->label);
            
            SetCtrlVal (p, LS340_SENS_KPOTACQ,   ls->channels[SEN_B]->acquire);
            SetCtrlVal (p, LS340_SENS_NOTE_2,    ls->channels[SEN_B]->note);

            SetCtrlVal (p, LS340_SENS_HE3PLABEL, ls->channels[SEN_C]->channel->label);
            
            SetCtrlVal (p, LS340_SENS_HE3PACQ,   ls->channels[SEN_C]->acquire);
            SetCtrlVal (p, LS340_SENS_NOTE_3,    ls->channels[SEN_C]->note);
			
			SetCtrlVal (p, LS340_SENS_SEN_D_LABEL, ls->channels[SEN_D]->channel->label);
            
            SetCtrlVal (p, LS340_SENS_SEN_D_ACQ,   ls->channels[SEN_D]->acquire);
            SetCtrlVal (p, LS340_SENS_NOTE_4,    ls->channels[SEN_D]->note);

            SetCtrlAttribute(p, LS340_SENS_SORBLABEL,   ATTR_CALLBACK_DATA, ls->channels[SEN_A]);
            
            SetCtrlAttribute(p, LS340_SENS_SORBACQ,     ATTR_CALLBACK_DATA, ls->channels[SEN_A]);
            SetCtrlAttribute(p, LS340_SENS_NOTE_1,      ATTR_CALLBACK_DATA, ls->channels[SEN_A]);
    
            SetCtrlAttribute(p, LS340_SENS_KPOTLABEL,   ATTR_CALLBACK_DATA, ls->channels[SEN_B]);
            
            SetCtrlAttribute(p, LS340_SENS_KPOTACQ,     ATTR_CALLBACK_DATA, ls->channels[SEN_B]);
            SetCtrlAttribute(p, LS340_SENS_NOTE_2,      ATTR_CALLBACK_DATA, ls->channels[SEN_B]);
        
            SetCtrlAttribute(p, LS340_SENS_HE3PLABEL,   ATTR_CALLBACK_DATA, ls->channels[SEN_C]);
            
            SetCtrlAttribute(p, LS340_SENS_HE3PACQ,     ATTR_CALLBACK_DATA, ls->channels[SEN_C]);
            SetCtrlAttribute(p, LS340_SENS_NOTE_3,      ATTR_CALLBACK_DATA, ls->channels[SEN_C]);
			
			SetCtrlAttribute(p, LS340_SENS_SEN_D_LABEL,   ATTR_CALLBACK_DATA, ls->channels[SEN_D]);
            
            SetCtrlAttribute(p, LS340_SENS_SEN_D_ACQ,     ATTR_CALLBACK_DATA, ls->channels[SEN_D]);
            SetCtrlAttribute(p, LS340_SENS_NOTE_4,      ATTR_CALLBACK_DATA, ls->channels[SEN_D]);
    
            devPanel_Add (p, ls, LS340_UpdateSensorTabReadings);
            InstallPopup (p); //if popup is reqired or DisplayPanel (p);
        }
        break; 
    }
}

/****************************** Initialization **********************************/
void *LS340_Create(gpibioPtr dev)
{
    LS340Ptr ls;
    ls = malloc(sizeof(LS340Type));
    if (dev){ dev->device = ls; ls->id = dev->id;}
    
    
    ls->channels[SEN_A] = acqchan_Create("sens_A", dev, LS340_GetReadings );
    ls->channels[SEN_B] = acqchan_Create("sens_B", dev, LS340_GetReadings );
    ls->channels[SEN_C] = acqchan_Create("sens_C", dev, LS340_GetReadings );
	ls->channels[SEN_D] = acqchan_Create("sens_D", dev, LS340_GetReadings );
	
	ls->source = source_Create("temperature", dev, SetHeaterLvl, GetHeaterLvl); 
    //ls->alarm.on = 0;
    //ls->alarm.level = 0;
    ls->curveload.format = "%s, %s,> %s";
    ls->curveload.serial = "no_number";
    ls->curveload.source = 1;
    ls->curveload.target = 21;
    ls->heater.current = 3;
    ls->heater.input = "A";
    ls->heater.loop = 1;
    ls->heater.maxpower = 5;
    ls->heater.nchange = 0;
    ls->heater.on = 0;
    ls->heater.pchange = 10;
    ls->heater.power = 5;
    ls->heater.powerup = 0;
    ls->heater.rampspeed = 150;
    ls->heater.setplimit = 350;
    ls->heater.setpoint = 0;
    ls->heater.units = KELVIN;
    ls->pid.d = 0;
    ls->pid.don = 0;
    ls->pid.i =0;
    ls->pid.ion = 0;
    ls->pid.p= 0;
    ls->pid.pon = 0;
    ls->source->min = 0;
    return ls;
}
void *oldLS340_Create(gpibioPtr dev)
{
    LS340Ptr ls;
    ls = malloc(sizeof(LS340Type));
    if (dev){ dev->device = ls; ls->id = dev->id;}
    
    
    ls->channels[SEN_A] = acqchan_Create("sorb", dev, GetSensor);
    ls->channels[SEN_B] = acqchan_Create("1 k pot", dev, GetSensor);
    ls->channels[SEN_C] = acqchan_Create("He 3 pot", dev, GetSensor);
	ls->channels[SEN_D] = acqchan_Create("Sensor D", dev, GetSensor);
	
	ls->source = source_Create("temperature", dev, SetHeaterLvl, GetHeaterLvl); 
    //ls->alarm.on = 0;
    //ls->alarm.level = 0;
    ls->curveload.format = "%s, %s,> %s";
    ls->curveload.serial = "no_number";
    ls->curveload.source = 1;
    ls->curveload.target = 21;
    ls->heater.current = 3;
    ls->heater.input = "A";
    ls->heater.loop = 1;
    ls->heater.maxpower = 5;
    ls->heater.nchange = 0;
    ls->heater.on = 0;
    ls->heater.pchange = 10;
    ls->heater.power = 5;
    ls->heater.powerup = 0;
    ls->heater.rampspeed = 150;
    ls->heater.setplimit = 350;
    ls->heater.setpoint = 0;
    ls->heater.units = KELVIN;
    ls->pid.d = 0;
    ls->pid.don = 0;
    ls->pid.i =0;
    ls->pid.ion = 0;
    ls->pid.p= 0;
    ls->pid.pon = 0;
    ls->source->min = 0;
    return ls;
}
int  LS340_InitGPIB(gpibioPtr dev)
{
    gpibio_Remote(dev);
    if(gpibio_DeviceMatch(dev, "*IDN?", LS340_ID))
        return TRUE;
    return FALSE;
}




void LS340_Save(gpibioPtr dev)
{
    int i;
    LS340Ptr ls = dev->device;
    FmtFile (fileHandle.analysis, "%s<Heater Properties  : %i, %i, %i, %i, %i, %s, %f, %f, %f, %f, %f, %f\n",
                                                                            ls->heater.loop,
                                                                            ls->heater.units,
                                                                            ls->heater.power,
                                                                            ls->heater.maxpower,
                                                                            ls->heater.powerup,
                                                                            ls->heater.input,
                                                                            ls->heater.setplimit,
                                                                            ls->heater.pchange,
                                                                            ls->heater.nchange,
                                                                            ls->heater.current,
                                                                            ls->heater.setpoint,
                                                                            ls->heater.rampspeed);
    FmtFile(fileHandle.analysis, "%s<PID properties      : %i, %i, %i, %f, %f, %f\n", 
                                                                            ls->pid.pon,
                                                                            ls->pid.ion,
                                                                            ls->pid.don,
                                                                            ls->pid.p,
                                                                            ls->pid.i,
                                                                            ls->pid.d);
   // FmtFile(fileHandle.analysis, "%s<Alarm properties    : %i, %f\n", ls->alarm.on, ls->alarm.level);
    
    for (i = 0; i < 4; i++) acqchan_Save (ls->channels[i]);  // 4 sensors
    source_Save (ls->source);
}

void LS340_Load(gpibioPtr dev)
{
    int i;
    LS340Ptr ls = dev? dev->device:NULL;
    if(dev)
    {
        ScanFile (fileHandle.analysis, "%s>Heater Properties  : %i, %i, %i, %i, %i, %s[w1], %f, %f, %f, %f, %f, %f",
                                                                            &ls->heater.loop,
                                                                            &ls->heater.units,
                                                                            &ls->heater.power,
                                                                            &ls->heater.maxpower,
                                                                            &ls->heater.powerup,
                                                                            ls->heater.input,
                                                                            &ls->heater.setplimit,
                                                                            &ls->heater.pchange,
                                                                            &ls->heater.nchange,
                                                                            &ls->heater.current,
                                                                            &ls->heater.setpoint,
                                                                            &ls->heater.rampspeed);
        ScanFile(fileHandle.analysis, "%s>PID properties      : %i, %i, %i, %f, %f, %f", 
                                                                            &ls->pid.pon,
                                                                            &ls->pid.ion,
                                                                            &ls->pid.don,
                                                                            &ls->pid.p,
                                                                            &ls->pid.i,
                                                                            &ls->pid.d);
        //ScanFile(fileHandle.analysis, "%s>Alarm properties    : %i, %f",        &ls->alarm.on,&ls->alarm.level);
    
        for (i = 0; i < 4; i++) acqchan_Load (dev, ls->channels[i]); // 4 sensors
        source_Load (dev, ls->source);
        ls->source->max = ls->heater.setplimit;
        ls->source->min = 0;
    }
}

void LS340_Remove(void *dev)
{
    LS340Ptr ls = dev;
    acqchan_Remove(ls->channels[SEN_A]);
    acqchan_Remove(ls->channels[SEN_B]);
    acqchan_Remove(ls->channels[SEN_C]);
	acqchan_Remove(ls->channels[SEN_D]);  
    source_Remove(ls->source);
    free(ls);
}

void LS340_Init(void)
{
    devTypePtr devType;
    if(utilG.acq.status != ACQ_NONE)
    {
        util_ChangeInitMessage("LS340 control utilities...");
        devType = malloc(sizeof(devTypeItem));
        if(devType)
        {
            Fmt(devType->label, "LS340 temperature controller");
            Fmt(devType->id, LS340_ID);
            devType->CreateDevice = LS340_Create;
            devType->InitDevice = LS340_InitGPIB;
            devType->OperateDevice = LS340_init_menus_and_controls;//OperateLS340;
            devType->UpdateReadings = LS340_UpdateReadings;
            devType->SaveDevice = LS340_Save;
            devType->LoadDevice = LS340_Load;
            devType->RemoveDevice = LS340_Remove;
            devTypeList_AddItem(devType);
			
        }
    }
}
/*void sr830_Init (void)
{
    devTypePtr devType;
    if (utilG.acq.status != ACQ_NONE) {
        util_ChangeInitMessage ("Stanford SR830 Control Utilities...");
        devType = malloc (sizeof (devTypeItem));
        if (devType) {
            Fmt (devType->label, "Stanford Research SR830");
            Fmt (devType->id, SR830_ID);
            devType->CreateDevice = sr830_Create;
            devType->InitDevice = sr830_InitGPIB;
            devType->OperateDevice = sr830_init_menus_and_controls;
            devType->UpdateReadings = sr830_UpdateReadings;
            devType->SaveDevice = sr830_Save;
            devType->LoadDevice = sr830_Load;
            devType->RemoveDevice = sr830_Remove;
            devTypeList_AddItem (devType);
        }
    }
} */
/*void OperateLS340(int menubar, int menuItem, void *callbackData, int panel)
{
    gpibioPtr dev = callbackData;
    LS340Ptr ls = dev->device;
    int p, m;
    //SetMenuBarAttribute (menubar, menuItem, ATTR_DIMMED, TRUE);  //
    ls->source->max = ls->heater.setplimit;
    p = dev->iPanel? dev->iPanel: LoadPanel(utilG.p, "LS340u.uir", LS340_CTRL);
    dev->iPanel = p;
    SetPanelAttribute(p, ATTR_TITLE, dev->label);
    
    m = LoadMenuBar(p, "LS340u.uir", LS340MENU);
    SetPanelMenuBar(p, m);
    
    SetMenuBarAttribute(m, LS340MENU_CURVES_LOAD, ATTR_CALLBACK_DATA, dev);
    SetMenuBarAttribute(m, LS340MENU_SOURCE_HEATER, ATTR_CALLBACK_DATA, ls->source);
    SetMenuBarAttribute(m, LS340MENU_MEASURE_SENSOR_SETTINGS, ATTR_CALLBACK_DATA, ls);
    
    SetCtrlAttribute(p, LS340_CTRL_HEATER, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute(p, LS340_CTRL_HEAT_PROP, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute(p, LS340_CTRL_SORBTSET, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute(p, LS340_CTRL_RAMPSPEED, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute(p, LS340_CTRL_POWER, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute(p, LS340_CTRL_ALARM, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute(p, LS340_CTRL_ALARMLVL, ATTR_CALLBACK_DATA, dev);
    
    SetCtrlAttribute(p, LS340_CTRL_SORBTSET, ATTR_MAX_VALUE, ls->heater.setplimit);
    SetCtrlAttribute(p, LS340_CTRL_SORBTSET, ATTR_MIN_VALUE, 0.);
    SetPanelAttribute(p, ATTR_CALLBACK_DATA, dev);
    
    LS340_InitControls(p, dev);
    devPanel_Add(p, dev, LS340_UpdateReadings);
    
    DisplayPanel(p);
} */
void LS340_init_menus_and_controls (int menubar, int menuItem, void *callbackData, int panel)
{
    int p, m,ramp_speed;//status;
    gpibioPtr dev = callbackData;
    LS340Ptr ls =  dev->device;
    char label[256];
	char msg[260];

    SetMenuBarAttribute (menubar, menuItem, ATTR_DIMMED, TRUE);

//    p = dev->iPanel? dev->iPanel: LoadPanel (utilG.p, "sr830u.uir", SR830_CTRL);
//    dev->iPanel = p;

	p = dev->iPanel? dev->iPanel: LoadPanel(utilG.p, "LS340u.uir", LS340_CTRL);
    dev->iPanel = p;
    SetPanelAttribute(p, ATTR_TITLE, dev->label);
    SetPanelPos (p, VAL_AUTO_CENTER, VAL_AUTO_CENTER);

    Fmt (label, "Lakeshore  %s", dev->label);
    SetPanelAttribute (p, ATTR_TITLE, label);

    //m = LoadMenuBar (p, "sr830u.uir", SR830MENU);
	m = LoadMenuBar(p, "LS340u.uir", LS340MENU);   
    SetPanelMenuBar (p, m);
	SetMenuBarAttribute(m, LS340MENU_CURVES_LOAD, ATTR_CALLBACK_DATA, dev);
    SetMenuBarAttribute(m, LS340MENU_SOURCE_HEATER, ATTR_CALLBACK_DATA, ls->source);
    SetMenuBarAttribute(m, LS340MENU_MEASURE_SENSOR_SETTINGS, ATTR_CALLBACK_DATA, ls);
    
    SetCtrlAttribute(p, LS340_CTRL_HEATER, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute(p, LS340_CTRL_HEAT_PROP, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute(p, LS340_CTRL_SORBTSET, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute(p, LS340_CTRL_RAMPSPEED, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute(p, LS340_CTRL_POWER, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute(p, LS340_CTRL_SORBTSET, ATTR_MAX_VALUE, ls->heater.setplimit);
    SetCtrlAttribute(p, LS340_CTRL_SORBTSET, ATTR_MIN_VALUE, 0.);
    SetPanelAttribute(p, ATTR_CALLBACK_DATA, dev);
   

    
	//LS340_InitControls(p, dev);
	//LS340Ptr ls = dev->device;
    //char msg[260]; int status;
    
    ls->heater.power = gpib_GetIntVal(dev, "RANGE?");
	if  (ls->heater.power)  SetCtrlVal(p, LS340_CTRL_HEATER, 1); else SetCtrlVal(p, LS340_CTRL_HEATER, 0);   
    Fmt(msg, "SETP? %i", ls->heater.loop);
    ls->heater.setpoint = gpib_GetDoubleVal(dev, msg);
    Fmt(msg, "RAMP? %i", ls->heater.loop);
    gpib_GetCharVal(dev, msg, msg);
    Scan(msg, "%i,%f", &ramp_speed, &ls->heater.rampspeed);  //needs to be instead of status &ls->heater.ramp_status
    //gpibPrint(dev, "RANGE 0\n");// this  shuts heater off
	
    SetCtrlVal(p, LS340_CTRL_POWER, ls->heater.power);
    //SetCtrlVal(p, LS340_CTRL_HEATER, ls->heater.on);
	
	//ls->heater.on=0;
    SetCtrlVal(p, LS340_CTRL_SORBTSET, ls->heater.setpoint);
    SetCtrlAttribute(p, LS340_CTRL_SORBTSET, ATTR_MAX_VALUE, ls->heater.setplimit);
    SetCtrlVal(p, LS340_CTRL_RAMPSPEED, ls->heater.rampspeed);
	
    
	
	LS340_UpdateHeaterSettings(p,  dev); 
    devPanel_Add(p, dev, LS340_UpdateReadings);  
    DisplayPanel(p);
}	
/*void sr830_UpdateControls (int panel, gpibioPtr dev)
{
    char msg[260];
    int i = 0, ktest;
    double r = 0;
    sr830Ptr lia;

    lia = dev->device;
   // printf(" Time constant is %d\n", sr830_GetTimeConstant(dev));
    SetCtrlIndex (panel, SR830_CTRL_TC, sr830_GetTimeConstant(dev));

    SetCtrlIndex (panel, SR830_CTRL_SENS, sr830_GetSensitivity(dev));
    
	//printf(" Slope is %d\n", sr830_GetFilterSlope (dev)); 
    SetCtrlIndex (panel, SR830_CTRL_FILTSLOPE, sr830_GetFilterSlope (dev));

    SetCtrlVal (panel, SR830_CTRL_SYNC, sr830_GetSync(dev));

    SetCtrlIndex (panel, SR830_CTRL_DYNRES, sr830_GetDynReserve (dev));

    SetCtrlIndex (panel, SR830_CTRL_REJECT, sr830_GetLineReject(dev));
    SetCtrlVal (panel, SR830_CTRL_AUTOSENSE, lia->autosens); 
}
*/

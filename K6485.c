// clone of LS335
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
#include "K6485.h"
#include "K6485u.h"

#define TRUE 1
#define FALSE 0
#define K6485_ID "KEITHLEY INSTRUMENTS INC.,MODEL 6485"

//double dl = 0;


//typedef enum {KELVIN, CELSIUS, SEN_UNITS} units;
typedef struct{
    acqchanPtr channels;
    //sourcePtr source;
    int id;
    /*struct {int loop, units, power, maxpower, powerup, on;
        char *input;
        double setplimit, pchange, nchange, current, setpoint, rampspeed;} heater;
	*/
    //struct {double p, i, d; int pon, ion, don;} pid;
    //struct {char *serial, *format; int source, target, file;} curveload;
} K6485Type;
    
typedef K6485Type *K6485Ptr;

/*******************************index*********************************/
//void GetHeaterLvl(acqchanPtr acqchan);
//void SetHeaterLvl(sourcePtr src);
void GetSensor(acqchanPtr acqchan);

void K6485_UpdateSensorReadings (int panel, void *ptr);
//void K6485_UpdateHeaterSettings(int panel, gpibioPtr dev);
void K6485_UpdateControls(int p, gpibioPtr dev);
int  K6485PanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2);
int  K6485ControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
//int  K6485HeatControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  K6485SensorControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
//int  K6485SendCurve (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void K6485menuCallack (int menuBar, int menuItem, void *callbackData, int panel);

void *K6485_Create(gpibioPtr dev);
int  K6485_InitGPIB(gpibioPtr dev);
void OperateK6485(int menubar, int menuItem, void *callbackData, int panel);
void K6485_UpdateReadings(int panel, void *ptr);
void K6485_Save(gpibioPtr dev);
void K6485_Load(gpibioPtr dev);
void K6485_Remove(void *ptr);
void K6485_Init(void);
void K6485_UpdateSettings(int panel, gpibioPtr dev);

/*******************************Communication functions*********************************/



void K6485_GetSensor(acqchanPtr acqchan)
{     
    gpibioPtr dev = acqchan->dev;
    K6485Ptr ls = dev->device;
    char msg[100];
    //msg = ; 
   // if(!strcmp(acqchan->channel->label, ls->channels[SENSOR_B]->channel->label)) sens_name = "B"; 
    //Fmt(msg, "KRDG? %s", sens_name);
    acqchan->reading = gpib_GetDoubleVal(dev, "MEAS:CURR:DC?");
    acqchan->newreading = TRUE;
}

/******************************Callback Functions**********************************/
void K6485_UpdateSensorReadings (int panel, void *ptr)
{
    K6485Ptr ls = ptr;
    SetCtrlVal(panel, K6485_SENS_SENSOR_A_MEAS, ls->channels->reading);
   // SetCtrlVal(panel, K6485_SENS_SENSOR_B_MEAS, ls->channels[SENSOR_B]->reading);
    if(utilG.acq.status == ACQ_BUSY)
        HidePanel(panel);
}


void K6485_InitControls(int p, gpibioPtr dev)
{
    K6485Ptr ls = dev->device;
    char msg[260]; int status;
    
   // ls->heater.power = gpib_GetIntVal(dev, "RANGE?2");//status of the heater in second loop
	//if  (ls->heater.power)  SetCtrlVal(p, K6485_CTRL_HEATER, 1); else SetCtrlVal(p, K6485_CTRL_HEATER, 0);   
   // Fmt(msg, "SETP?2 %i", ls->heater.loop);   //status of the setpoint in second loop  
    //ls->heater.setpoint = gpib_GetDoubleVal(dev, msg);
   // Fmt(msg, "RAMP?2 %i", ls->heater.loop);  // status of the Ramp in second loop  
   // gpib_GetCharVal(dev, msg, msg);
   // Scan(msg, "%i,%f", &status, &ls->heater.rampspeed);  
    //gpibPrint(dev, "RANGE0, loop#");// this  shuts heater off for loop#
    //SetCtrlVal(p, K6485_CTRL_POWER, ls->heater.power);
    //SetCtrlVal(p, K6485_CTRL_SENSOR_A_TSET, ls->heater.setpoint);
    //SetCtrlAttribute(p, K6485_CTRL_SENSOR_A_TSET, ATTR_MAX_VALUE, ls->heater.setplimit);
    //SetCtrlVal(p, K6485_CTRL_RAMPSPEED, ls->heater.rampspeed);
    
}
/*void K6485_UpdateControls(int p, gpibioPtr dev) // not in use anymore
{
    K6485Ptr ls = dev->device;
    char msg[260];
    
    ls->heater.power = gpib_GetIntVal(dev, "RANGE?\0");
    Fmt(msg, "SETP? %i\0", ls->heater.loop);
    ls->heater.setpoint = gpib_GetDoubleVal(dev, msg);
    Fmt(msg, "RAMP? %i\0", ls->heater.loop);
    gpib_GetCharVal(dev, msg, msg);
    Scan(msg, "%i,%f", &ls->heater.on, &ls->heater.rampspeed);
    
    SetCtrlVal(p, K6485_CTRL_POWER, ls->heater.power);
    SetCtrlVal(p, K6485_CTRL_HEATER, ls->heater.on);
    SetCtrlVal(p, K6485_CTRL_SENSOR_A_TSET, ls->heater.setpoint);
    SetCtrlAttribute(p, K6485_CTRL_SENSOR_A_TSET, ATTR_MAX_VALUE, ls->heater.setplimit);
    SetCtrlVal(p, K6485_CTRL_RAMPSPEED, ls->heater.rampspeed);
}  */

int K6485PanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2)
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

int K6485ControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    gpibioPtr dev = callbackData;
    K6485Ptr ls = dev->device;
    char msg[260];
    switch(control)
    {
        /*case K6485_CTRL_HEATER:
            if (event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->heater.on);
                if(ls->heater.on)
                {
                    GetCtrlVal(panel, K6485_CTRL_POWER, &ls->heater.power);
                    GetCtrlVal(panel, K6485_CTRL_SENSOR_A_TSET, &ls->heater.setpoint);
                    GetCtrlVal(panel, K6485_CTRL_RAMPSPEED, &ls->heater.rampspeed);
                    gpibPrint(dev, "SETP %i, %f\0", &ls->heater.loop, &ls->heater.setpoint);
                    gpibPrint(dev, "RAMP %i, 1, %f\0", &ls->heater.loop, &ls->heater.rampspeed);
                    gpibPrint(dev, "RANGE %i, %i\0", &ls->heater.loop, &ls->heater.power);
                }
                else
                    gpibPrint(dev, "RANGE %i,0\0", &ls->heater.loop);
            }
            break;
        case K6485_CTRL_HEATER_PROP:
            if (event == EVENT_COMMIT)
            {
                int heater = LoadPanel(utilG.p, "K6485u.uir", K6485_HEAT);
                SetCtrlAttribute(heater, K6485_HEAT_DON, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, K6485_HEAT_ION, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, K6485_HEAT_PON, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, K6485_HEAT_ACCEPT, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, K6485_HEAT_RESET, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, K6485_HEAT_P, ATTR_DIMMED, !ls->pid.pon);
                SetCtrlAttribute(heater, K6485_HEAT_I, ATTR_DIMMED, !ls->pid.ion);
                SetCtrlAttribute(heater, K6485_HEAT_D, ATTR_DIMMED, !ls->pid.don);
                
                K6485_UpdateHeaterSettings(heater, dev);
                SetCtrlVal(heater, K6485_HEAT_LOOPNUM,  ls->heater.loop);
                SetCtrlVal(heater, K6485_HEAT_SETPLIM,  ls->heater.setplimit);
                SetCtrlVal(heater, K6485_HEAT_PCHANGE,  ls->heater.pchange);
                SetCtrlVal(heater, K6485_HEAT_NCHANGE,  ls->heater.nchange);
                SetCtrlVal(heater, K6485_HEAT_CURRENT,  ls->heater.current);
                SetCtrlVal(heater, K6485_HEAT_POWERUP,  ls->heater.powerup);
                SetCtrlVal(heater, K6485_HEAT_MXPOWER,  ls->heater.maxpower);
                SetCtrlVal(heater, K6485_HEAT_INPUTNM,  ls->heater.input);
                SetCtrlVal(heater, K6485_HEAT_UNITS,    ls->heater.units);
                SetCtrlVal(heater, K6485_HEAT_P,        ls->pid.p);
                SetCtrlVal(heater, K6485_HEAT_PON,      ls->pid.pon);
                SetCtrlVal(heater, K6485_HEAT_I,        ls->pid.i);
                SetCtrlVal(heater, K6485_HEAT_ION,      ls->pid.ion);
                SetCtrlVal(heater, K6485_HEAT_D,        ls->pid.d);
                SetCtrlVal(heater, K6485_HEAT_DON,      ls->pid.don);
                DisplayPanel(heater);
            }
            break;
       case K6485_CTRL_SENSOR_A_TSET:
            if (event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->heater.setpoint);
                gpibPrint(dev, "SETP %i, %f\n", &ls->heater.loop, &ls->heater.setpoint);
            }
            break; 
        case K6485_CTRL_RAMPSPEED:
            if (event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->heater.rampspeed);
                gpibPrint(dev, "RAMP %i, %i, %f\0", &ls->heater.loop, &ls->heater.on, &ls->heater.rampspeed);
            }
            break;
        case K6485_CTRL_POWER:
            if (event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->heater.power);
                gpibPrint(dev, "RANGE %i, %i\0",&ls->heater.loop, &ls->heater.power);
            }
            break;*/
    }
    return 0;
}



int  K6485SensorControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    acqchanPtr acqchan;
    acqchan = callbackData;

    switch (control) {
        case K6485_SENS_NOTE_1:
        
            AcqDataNoteCallback (panel, control, event, callbackData, eventData1, eventData2);
            break;
        case K6485_SENS_SENSOR_A_ACQ:
        
            if (event == EVENT_VAL_CHANGED) {
                GetCtrlVal (panel, control, &acqchan->acquire);
                if (acqchan->acquire) acqchanlist_AddChannel (acqchan);
                    else acqchanlist_RemoveChannel (acqchan);
            }
            break;
        case K6485_SENS_SENSOR_A_COEFF:
        
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &acqchan->coeff);
                if (acqchan->p) SetCtrlVal (acqchan->p, ACQDATA_COEFF, acqchan->coeff);
            }
            break;
        case K6485_SENS_SENSOR_A_LABEL:
        
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, acqchan->channel->label);
                acqchanlist_ReplaceChannel (acqchan);
                if (acqchan->p) SetPanelAttribute (acqchan->p, ATTR_TITLE, acqchan->channel->label);
            }
            break;
        case K6485_SENS_CLOSE:
            if (event == EVENT_COMMIT) 
                devPanel_Remove(panel);
                DiscardPanel (panel);//
				//HidePanel (panel); if popup is not in use
            break;
    }
    updateGraphSource();
    return 0;
}



void K6485menuCallback (int menuBar, int menuItem, void *callbackData, int panel)
{
    switch(menuItem)
    {
        case K6485_SETUP_LOAD:
        {
            gpibioPtr dev = callbackData;
            K6485Ptr ls = dev->device;
            char pathname[260];
            //int cPanel, i = FileSelectPopup ("", "*.crv", "*.crv", "Custom Curve", VAL_LOAD_BUTTON,
            //                     0, 0, 1, 0, pathname);
           // if(i)
            {
                //ls->curveload.file = OpenFile (pathname, VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_ASCII);
                //cPanel = LoadPanel(utilG.p, "K6485u.uir", K6485CURVE);
                
                //SetCtrlAttribute(cPanel, K6485CURVE_ACCEPT, ATTR_CALLBACK_DATA, dev);
                
                //SetCtrlVal(cPanel, K6485CURVE_SERIAL,    ls->curveload.serial);
                //SetCtrlVal(cPanel, K6485CURVE_CURVESRC,  ls->curveload.source);
                //SetCtrlVal(cPanel, K6485CURVE_CURVENUM,  ls->curveload.target);
                
                //DisplayPanel(cPanel);
            }
        }
        break;
        
        case K6485_MEASURE_MEAS:
        {
            K6485Ptr ls = callbackData;
            int p = LoadPanel (utilG.p, "K6485u.uir", K6485_SENS);
            
            util_InitClose (p, K6485_SENS_CLOSE, FALSE); //needed for popup panel, empty otherwise
			
            SetCtrlVal (p, K6485_SENS_SENSOR_A_LABEL, ls->channels->channel->label);
            SetCtrlVal (p, K6485_SENS_SENSOR_A_COEFF, ls->channels->coeff);
            SetCtrlVal (p, K6485_SENS_SENSOR_A_ACQ,   ls->channels->acquire);
            SetCtrlVal (p, K6485_SENS_NOTE_1,    ls->channels->note);
                                  
            

            SetCtrlAttribute(p, K6485_SENS_SENSOR_A_LABEL,   ATTR_CALLBACK_DATA, ls->channels);
            SetCtrlAttribute(p, K6485_SENS_SENSOR_A_COEFF,   ATTR_CALLBACK_DATA, ls->channels);
            SetCtrlAttribute(p, K6485_SENS_SENSOR_A_ACQ,     ATTR_CALLBACK_DATA, ls->channels);
            SetCtrlAttribute(p, K6485_SENS_NOTE_1,      ATTR_CALLBACK_DATA, ls->channels);
    
            
    
            devPanel_Add (p, ls, K6485_UpdateSensorReadings);
            //DisplayPanel (p);
			InstallPopup (p);  // if popup is used
        }
        break;
    }
}


/******************************Initialization **********************************/
void *K6485_Create(gpibioPtr dev)
{
    K6485Ptr ls;
    ls = malloc(sizeof(K6485Type));
    if (dev){ dev->device = ls; ls->id = dev->id;}
    
   // ls->source = source_Create("temperature", dev, K6485_SetHeaterLvl, K6485_GetHeaterLvl);
    ls->channels = acqchan_Create("Current", dev, K6485_GetSensor);
   // ls->channels[SENSOR_B] = acqchan_Create("Sensor B", dev, K6485_GetSensor);
   
    return ls;
}

int  K6485_InitGPIB(gpibioPtr dev)
{
    gpibio_Remote(dev);
    if(gpibio_DeviceMatch(dev, "*IDN?", K6485_ID))
        return TRUE;
    return FALSE;
}

void OperateK6485(int menubar, int menuItem, void *callbackData, int panel)
{
    gpibioPtr dev = callbackData;
    K6485Ptr ls = dev->device;
    int p, m;
    
    //ls->source->max = ls->heater.setplimit;
    p = dev->iPanel? dev->iPanel: LoadPanel(utilG.p, "K6485u.uir", K6485_CTRL);
    dev->iPanel = p;
    
    SetMenuBarAttribute(menubar, menuItem, ATTR_DIMMED, 1);
    SetPanelAttribute(p, ATTR_TITLE, dev->label);
    
    m = LoadMenuBar(p, "K6485u.uir", K6485);
    SetPanelMenuBar(p, m);
    
    
    SetMenuBarAttribute(m, K6485_MEASURE_MEAS, ATTR_CALLBACK_DATA, ls);
    
    
    
    
    SetPanelAttribute(p, ATTR_CALLBACK_DATA, dev);
    
    
	K6485_InitControls(p, dev); 
	K6485_UpdateSettings(p, dev);
    devPanel_Add(p, dev, K6485_UpdateReadings);
    
    DisplayPanel(p);
	
}

void K6485_UpdateReadings(int panel, void *ptr)
{
    gpibioPtr dev = ptr;
    K6485Ptr ls = dev->device;
    K6485_GetSensor(ls->channels);
    
    SetCtrlVal(panel, K6485_CTRL_SENSOR_A_READ, ls->channels->reading);
   
   
    
}

void K6485_Save(gpibioPtr dev)
{
    int i;
    K6485Ptr ls = dev->device;
    /*FmtFile (fileHandle.analysis, "%s<Heater Properties  : %i, %i, %i, %i, %i, %s, %f, %f, %f, %f, %f, %f\n",
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
    
    for (i = 0; i < 2; i++) acqchan_Save (ls->channels[i]);*/
    //source_Save (ls->source);
}

void K6485_Load(gpibioPtr dev)
{
    int i;
    K6485Ptr ls = dev? dev->device:NULL;
    if(dev)
    {
        
    }
}

void K6485_Remove(void *dev)
{
    K6485Ptr ls = dev;
    acqchan_Remove(ls->channels);
    //acqchan_Remove(ls->channels[SENSOR_B]);
   // source_Remove(ls->source);
    free(ls);
}

void K6485_Init(void)
{
    devTypePtr devType;
    if(utilG.acq.status != ACQ_NONE)
    {
        util_ChangeInitMessage("K6485 control utilities...");
        devType = malloc(sizeof(devTypeItem));
        if(devType)
        {
            Fmt(devType->label, "K6485 Picoammeter");
            Fmt(devType->id, K6485_ID);
            devType->CreateDevice = K6485_Create;
            devType->InitDevice = K6485_InitGPIB;
            devType->OperateDevice = OperateK6485;
            devType->UpdateReadings = K6485_UpdateReadings;
            devType->SaveDevice = K6485_Save;
            devType->LoadDevice = K6485_Load;
            devType->RemoveDevice = K6485_Remove;
            devTypeList_AddItem(devType);
        }
    }
}


void K6485_UpdateSettings(int panel, gpibioPtr dev)
{
    char msg[260];
    K6485Ptr ls = dev->device;
    gpibio_Out(dev, "CONF:CURR");
   // gpibio_In(dev, msg);
   // Scan(msg, "%s[w1],%i,%i,%i", ls->heater.input, &ls->heater.units, &ls->heater.on, &ls->heater.powerup);
    //gpibPrint(dev, "CLIMIT? %i\0", &ls->heater.loop);
    //gpibio_In(dev, msg);
    //Scan(msg, "%f,%f,%f,%f,%i", &ls->heater.setplimit, &ls->heater.pchange, &ls->heater.nchange, &ls->heater.current, &ls->heater.maxpower);
   // gpibPrint(dev, "PID? %i\0", &ls->heater.loop);
   // gpibio_In(dev, msg);
   // Scan(msg, "%f,%f,%f", &ls->pid.p, &ls->pid.i, &ls->pid.d);
}

/*void K6485_SingleShot (void)
{    
	gpibioPtr dev = src->acqchan->dev;
    char cmd[256];
    double dly;
    Fmt (cmd, "PHAS%f", src->biaslevel);
    gpibio_Out (dev, cmd);
    util_Delay (src->segments[src->seg]->delay);
 }
void K6485_Out (gpibioPtr dev, char *cmd )
{
    gpibio_Out (dev, cmd);
}*/
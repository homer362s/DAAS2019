// modified December 28 2012
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
#include "LS335.h"
#include "LS335u.h"

#define TRUE 1
#define FALSE 0
#define LS335_ID "335A0AL"

double dl = 0;

typedef enum {SORB, ONEK, HE3P} inputs;
typedef enum {KELVIN, CELSIUS, SEN_UNITS} units;
typedef struct{
    acqchanPtr channels[2];
    sourcePtr source;
    int id;
    struct {int loop, units, power, maxpower, powerup, on;
        char *input;
        double setplimit, pchange, nchange, current, setpoint, rampspeed;} heater;
    struct {double p, i, d; int pon, ion, don;} pid;
    struct {char *serial, *format; int source, target, file;} curveload;
} LS335Type;
    
typedef LS335Type *LS335Ptr;

/*******************************index*********************************/
void GetHeaterLvl(acqchanPtr acqchan);
void SetHeaterLvl(sourcePtr src);
void GetSensor(acqchanPtr acqchan);

void LS335_UpdateSensorReadings (int panel, void *ptr);
void LS335_UpdateHeaterSettings(int panel, gpibioPtr dev);
void LS335_UpdateControls(int p, gpibioPtr dev);
int  LS335PanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2);
int  LS335ControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  LS335HeatControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  LS335SensorControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  LS335SendCurve (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void LS335menuCallack (int menuBar, int menuItem, void *callbackData, int panel);

void *LS335_Create(gpibioPtr dev);
int  LS335_InitGPIB(gpibioPtr dev);
void OperateLS335(int menubar, int menuItem, void *callbackData, int panel);
void LS335_UpdateReadings(int panel, void *ptr);
void LS335_Save(gpibioPtr dev);
void LS335_Load(gpibioPtr dev);
void LS335_Remove(void *ptr);
void LS335_Init(void);


/*******************************Communication functions*********************************/

void LS335_GetHeaterLvl(acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    LS335Ptr ls = dev->device;
    char msg[10];
    
    Fmt(msg, "SETP? %i\0", ls->heater.loop);
    acqchan->reading = gpib_GetDoubleVal(dev, msg);
    acqchan->newreading = TRUE;
}

void LS335_SetHeaterLvl(sourcePtr src)
{
    gpibioPtr dev = src->acqchan->dev;
    LS335Ptr ls = dev->device;
    char msg[30];
    gpibPrint(dev, "SETP %i, %f", &ls->heater.loop, &src->biaslevel);
    util_Delay(src->segments[src->seg]->delay);
}

void LS335_GetSensor(acqchanPtr acqchan)
{     
    gpibioPtr dev = acqchan->dev;
    LS335Ptr ls = dev->device;
    char *sens_name="", msg[10];
    if(!strcmp(acqchan->channel->label, ls->channels[SORB]->channel->label)) sens_name = "A"; 
    if(!strcmp(acqchan->channel->label, ls->channels[ONEK]->channel->label)) sens_name = "B"; 
    Fmt(msg, "KRDG? %s", sens_name);
    acqchan->reading = gpib_GetDoubleVal(dev, msg);
    acqchan->newreading = TRUE;
}

/******************************Callback Functions**********************************/
void LS335_UpdateSensorReadings (int panel, void *ptr)
{
    LS335Ptr ls = ptr;
    SetCtrlVal(panel, LS335_SENS_SORBMEAS, ls->channels[SORB]->reading);
    SetCtrlVal(panel, LS335_SENS_KPOTMEAS, ls->channels[ONEK]->reading);
    if(utilG.acq.status == ACQ_BUSY)
        HidePanel(panel);
}

void LS335_UpdateHeaterSettings(int panel, gpibioPtr dev)
{
    char msg[260];
    LS335Ptr ls = dev->device;
    gpibPrint(dev, "CSET? %i\0", &ls->heater.loop);
    gpibio_In(dev, msg);
    Scan(msg, "%s[w1],%i,%i,%i", ls->heater.input, &ls->heater.units, &ls->heater.on, &ls->heater.powerup);
    gpibPrint(dev, "CLIMIT? %i\0", &ls->heater.loop);
    gpibio_In(dev, msg);
    Scan(msg, "%f,%f,%f,%f,%i", &ls->heater.setplimit, &ls->heater.pchange, &ls->heater.nchange, &ls->heater.current, &ls->heater.maxpower);
    gpibPrint(dev, "PID? %i\0", &ls->heater.loop);
    gpibio_In(dev, msg);
    Scan(msg, "%f,%f,%f", &ls->pid.p, &ls->pid.i, &ls->pid.d);
}

void LS335_UpdateControls(int p, gpibioPtr dev)
{
    LS335Ptr ls = dev->device;
    char msg[260];
    
    ls->heater.power = gpib_GetIntVal(dev, "RANGE?\0");
    Fmt(msg, "SETP? %i\0", ls->heater.loop);
    ls->heater.setpoint = gpib_GetDoubleVal(dev, msg);
    Fmt(msg, "RAMP? %i\0", ls->heater.loop);
    gpib_GetCharVal(dev, msg, msg);
    Scan(msg, "%i,%f", &ls->heater.on, &ls->heater.rampspeed);
    
    SetCtrlVal(p, LS335_CTRL_POWER, ls->heater.power);
    SetCtrlVal(p, LS335_CTRL_HEATER, ls->heater.on);
    SetCtrlVal(p, LS335_CTRL_SORBTSET, ls->heater.setpoint);
    SetCtrlAttribute(p, LS335_CTRL_SORBTSET, ATTR_MAX_VALUE, ls->heater.setplimit);
    SetCtrlVal(p, LS335_CTRL_RAMPSPEED, ls->heater.rampspeed);
}

int LS335PanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2)
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

int LS335ControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    gpibioPtr dev = callbackData;
    LS335Ptr ls = dev->device;
    char msg[260];
    switch(control)
    {
        case LS335_CTRL_HEATER:
            if (event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->heater.on);
                if(ls->heater.on)
                {
                    GetCtrlVal(panel, LS335_CTRL_POWER, &ls->heater.power);
                    GetCtrlVal(panel, LS335_CTRL_SORBTSET, &ls->heater.setpoint);
                    GetCtrlVal(panel, LS335_CTRL_RAMPSPEED, &ls->heater.rampspeed);
                    gpibPrint(dev, "SETP %i, %f\0", &ls->heater.loop, &ls->heater.setpoint);
                    gpibPrint(dev, "RAMP %i, 1, %f\0", &ls->heater.loop, &ls->heater.rampspeed);
                    gpibPrint(dev, "RANGE %i, %i\0", &ls->heater.loop, &ls->heater.power);
                }
                else
                    gpibPrint(dev, "RANGE %i,0\0", &ls->heater.loop);
            }
            break;
        case LS335_CTRL_HEATER_PROP:
            if (event == EVENT_COMMIT)
            {
                int heater = LoadPanel(utilG.p, "LS335u.uir", LS335_HEAT);
                SetCtrlAttribute(heater, LS335_HEAT_DON, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, LS335_HEAT_ION, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, LS335_HEAT_PON, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, LS335_HEAT_ACCEPT, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, LS335_HEAT_RESET, ATTR_CALLBACK_DATA, dev);
                SetCtrlAttribute(heater, LS335_HEAT_P, ATTR_DIMMED, !ls->pid.pon);
                SetCtrlAttribute(heater, LS335_HEAT_I, ATTR_DIMMED, !ls->pid.ion);
                SetCtrlAttribute(heater, LS335_HEAT_D, ATTR_DIMMED, !ls->pid.don);
                
                LS335_UpdateHeaterSettings(heater, dev);
                SetCtrlVal(heater, LS335_HEAT_LOOPNUM,  ls->heater.loop);
                SetCtrlVal(heater, LS335_HEAT_SETPLIM,  ls->heater.setplimit);
                SetCtrlVal(heater, LS335_HEAT_PCHANGE,  ls->heater.pchange);
                SetCtrlVal(heater, LS335_HEAT_NCHANGE,  ls->heater.nchange);
                SetCtrlVal(heater, LS335_HEAT_CURRENT,  ls->heater.current);
                SetCtrlVal(heater, LS335_HEAT_POWERUP,  ls->heater.powerup);
                SetCtrlVal(heater, LS335_HEAT_MXPOWER,  ls->heater.maxpower);
                SetCtrlVal(heater, LS335_HEAT_INPUTNM,  ls->heater.input);
                SetCtrlVal(heater, LS335_HEAT_UNITS,    ls->heater.units);
                SetCtrlVal(heater, LS335_HEAT_P,        ls->pid.p);
                SetCtrlVal(heater, LS335_HEAT_PON,      ls->pid.pon);
                SetCtrlVal(heater, LS335_HEAT_I,        ls->pid.i);
                SetCtrlVal(heater, LS335_HEAT_ION,      ls->pid.ion);
                SetCtrlVal(heater, LS335_HEAT_D,        ls->pid.d);
                SetCtrlVal(heater, LS335_HEAT_DON,      ls->pid.don);
                DisplayPanel(heater);
            }
            break;
/*      case LS335_CTRL_SORBTSET:
            if (event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->heater.setpoint);
                gpibPrint(dev, "SETP %i, %f\n", &ls->heater.loop, &ls->heater.setpoint);
            }
            break; */
        case LS335_CTRL_RAMPSPEED:
            if (event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->heater.rampspeed);
                gpibPrint(dev, "RAMP %i, %i, %f\0", &ls->heater.loop, &ls->heater.on, &ls->heater.rampspeed);
            }
            break;
        case LS335_CTRL_POWER:
            if (event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->heater.power);
                gpibPrint(dev, "RANGE %i, %i\0",&ls->heater.loop, &ls->heater.power);
            }
            break;
    }
    return 0;
}

int LS335HeatControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    gpibioPtr dev = callbackData;
    LS335Ptr ls = dev->device;
    switch (control)
    {
        case LS335_HEAT_PON:
            if(event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->pid.pon);
                SetCtrlAttribute(panel, LS335_HEAT_P, ATTR_DIMMED, !ls->pid.pon);
            }
            break;
        case LS335_HEAT_ION:
            if(event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->pid.ion);
                SetCtrlAttribute(panel, LS335_HEAT_I, ATTR_DIMMED, !ls->pid.ion);
            }
            break;
        case LS335_HEAT_DON:
            if(event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, control, &ls->pid.don);
                SetCtrlAttribute(panel, LS335_HEAT_D, ATTR_DIMMED, !ls->pid.don);
            }
            break;
        case LS335_HEAT_ACCEPT:
            if(event == EVENT_COMMIT)
            {
                GetCtrlVal(panel, LS335_HEAT_LOOPNUM, &ls->heater.loop);
                GetCtrlVal(panel, LS335_HEAT_SETPLIM, &ls->heater.setplimit);
                GetCtrlVal(panel, LS335_HEAT_PCHANGE, &ls->heater.pchange);
                GetCtrlVal(panel, LS335_HEAT_NCHANGE, &ls->heater.nchange);
                GetCtrlVal(panel, LS335_HEAT_CURRENT, &ls->heater.current);
                GetCtrlVal(panel, LS335_HEAT_MXPOWER, &ls->heater.maxpower);
                GetCtrlVal(panel, LS335_HEAT_POWERUP, &ls->heater.powerup);
                GetCtrlVal(panel, LS335_HEAT_INPUTNM, ls->heater.input);   
                GetCtrlVal(panel, LS335_HEAT_UNITS,   &ls->heater.units);  //*
                //SetCtrlAttribute(dev->iPanel, LS335_CTRL_SORBTSET, ATTR_MAX_VALUE, ls->heater.setplimit);
                gpibPrint(dev, "CSET %i, %s, %i, %i, %i\0", &ls->heater.loop,
                                                            ls->heater.input,
                                                            &ls->heater.units,
                                                            &ls->heater.on,
                                                            &ls->heater.powerup);
                gpibPrint(dev, "CLIMIT %i, %f, %f, %f, %f, %i\0",   &ls->heater.loop,
                                                                    &ls->heater.setplimit,
                                                                    &ls->heater.pchange,
                                                                    &ls->heater.nchange,
                                                                    &ls->heater.current,
                                                                    &ls->heater.maxpower);//*/
                GetCtrlVal(panel, LS335_HEAT_P, &ls->pid.p);
                GetCtrlVal(panel, LS335_HEAT_I, &ls->pid.i);
                GetCtrlVal(panel, LS335_HEAT_D, &ls->pid.d);
                if(ls->pid.pon)
                {
                    gpibPrint(dev, "PID %i, %f[, %f][, %f]\0",  &ls->heater.loop,
                                                            &ls->pid.p,
                                                            ls->pid.ion,
                                                            &ls->pid.i,
                                                            (ls->pid.ion && ls->pid.don),
                                                            &ls->pid.d);
                }           
            }
            break;
        case LS335_HEAT_RESET:
            if(event == EVENT_COMMIT)
            {
                SetCtrlVal(panel, LS335_HEAT_LOOPNUM, ls->heater.loop);
                SetCtrlVal(panel, LS335_HEAT_SETPLIM, ls->heater.setplimit);
                SetCtrlVal(panel, LS335_HEAT_PCHANGE, ls->heater.pchange);
                SetCtrlVal(panel, LS335_HEAT_NCHANGE, ls->heater.nchange);
                SetCtrlVal(panel, LS335_HEAT_CURRENT, ls->heater.current);
                SetCtrlVal(panel, LS335_HEAT_MXPOWER, ls->heater.maxpower);
                SetCtrlVal(panel, LS335_HEAT_POWERUP, ls->heater.powerup);
                SetCtrlVal(panel, LS335_HEAT_INPUTNM, ls->heater.input);   
                SetCtrlVal(panel, LS335_HEAT_UNITS,   ls->heater.units);  //*
                if(ls->pid.pon)
                    SetCtrlVal(panel, LS335_HEAT_P, ls->pid.p);
                if(ls->pid.ion)
                    SetCtrlVal(panel, LS335_HEAT_I, ls->pid.i);
                if(ls->pid.don)
                    SetCtrlVal(panel, LS335_HEAT_D, ls->pid.d);
                
            }
            break;
        
    }
    return 0;
}

int  LS335SensorControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    acqchanPtr acqchan;
    acqchan = callbackData;

    switch (control) {
        case LS335_SENS_NOTE_1:
        case LS335_SENS_NOTE_2:
            AcqDataNoteCallback (panel, control, event, callbackData, eventData1, eventData2);
            break;
        case LS335_SENS_SORBACQ:
        case LS335_SENS_KPOTACQ:
            if (event == EVENT_VAL_CHANGED) {
                GetCtrlVal (panel, control, &acqchan->acquire);
                if (acqchan->acquire) acqchanlist_AddChannel (acqchan);
                    else acqchanlist_RemoveChannel (acqchan);
            }
            break;
        case LS335_SENS_SORBCOEFF:
        case LS335_SENS_KPOTCOEFF:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &acqchan->coeff);
                if (acqchan->p) SetCtrlVal (acqchan->p, ACQDATA_COEFF, acqchan->coeff);
            }
            break;
        case LS335_SENS_SORBLABEL:
        case LS335_SENS_KPOTLABEL:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, acqchan->channel->label);
                acqchanlist_ReplaceChannel (acqchan);
                if (acqchan->p) SetPanelAttribute (acqchan->p, ATTR_TITLE, acqchan->channel->label);
            }
            break;
        case LS335_SENS_CLOSE:
            if (event == EVENT_COMMIT) 
                HidePanel (panel);
            break;
    }
    updateGraphSource();
    return 0;
}

int LS335SendCurve (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    if (event == EVENT_COMMIT)
    {
        char *curve="", *buffer, vars[30];
        gpibioPtr dev = callbackData;
        LS335Ptr ls = dev->device;
        
        GetCtrlVal(panel, LS335CURVE_CURVESRC, &ls->curveload.source);
        GetCtrlVal(panel, LS335CURVE_CURVENUM, &ls->curveload.target);
        GetCtrlVal(panel, LS335CURVE_SERIAL,   ls->curveload.serial);
        
        do{
            ScanFile(ls->curveload.file, "%s", vars);
            buffer = malloc(sizeof(char) * StringLength(curve) + sizeof(vars));
            Fmt(buffer, "%s%s", curve, vars);
            curve = buffer;
        }while(*vars);
        gpibPrint(dev, "SCAL %i, %i, %s, %s\0", &ls->curveload.source,
                                                &ls->curveload.target,
                                                ls->curveload.serial,
                                                curve);
    }
    return 0;
}

void LS335menuCallack (int menuBar, int menuItem, void *callbackData, int panel)
{
    switch(menuItem)
    {
        case LS335_CURVES_LOAD:
        {
            gpibioPtr dev = callbackData;
            LS335Ptr ls = dev->device;
            char pathname[260];
            int cPanel, i = FileSelectPopup ("", "*.crv", "*.crv", "Custom Curve", VAL_LOAD_BUTTON,
                                 0, 0, 1, 0, pathname);
            if(i)
            {
                ls->curveload.file = OpenFile (pathname, VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_ASCII);
                cPanel = LoadPanel(utilG.p, "LS335u.uir", LS335CURVE);
                
                SetCtrlAttribute(cPanel, LS335CURVE_ACCEPT, ATTR_CALLBACK_DATA, dev);
                
                SetCtrlVal(cPanel, LS335CURVE_SERIAL,    ls->curveload.serial);
                SetCtrlVal(cPanel, LS335CURVE_CURVESRC,  ls->curveload.source);
                SetCtrlVal(cPanel, LS335CURVE_CURVENUM,  ls->curveload.target);
                
                DisplayPanel(cPanel);
            }
        }
        break;
        case LS335_SOURCE_HEATER:
        {
            sourcePtr src = callbackData;
            switch(utilG.exp)
            {
                case EXP_SOURCE: source_InitPanel(src); break;
                case EXP_FLOAT : gensrc_InitPanel(src); break;
            }
        }
        break;
        case LS335_MEASURE_MEAS:
        {
            LS335Ptr ls = callbackData;
            int p = LoadPanel (utilG.p, "LS335u.uir", LS335_SENS);
            
            SetCtrlVal (p, LS335_SENS_SORBLABEL, ls->channels[SORB]->channel->label);
            SetCtrlVal (p, LS335_SENS_SORBCOEFF, ls->channels[SORB]->coeff);
            SetCtrlVal (p, LS335_SENS_SORBACQ,   ls->channels[SORB]->acquire);
            SetCtrlVal (p, LS335_SENS_NOTE_1,    ls->channels[SORB]->note);
                                  
            SetCtrlVal (p, LS335_SENS_KPOTLABEL, ls->channels[ONEK]->channel->label);
            SetCtrlVal (p, LS335_SENS_KPOTCOEFF, ls->channels[ONEK]->coeff);
            SetCtrlVal (p, LS335_SENS_KPOTACQ,   ls->channels[ONEK]->acquire);
            SetCtrlVal (p, LS335_SENS_NOTE_2,    ls->channels[ONEK]->note);

            SetCtrlAttribute(p, LS335_SENS_SORBLABEL,   ATTR_CALLBACK_DATA, ls->channels[SORB]);
            SetCtrlAttribute(p, LS335_SENS_SORBCOEFF,   ATTR_CALLBACK_DATA, ls->channels[SORB]);
            SetCtrlAttribute(p, LS335_SENS_SORBACQ,     ATTR_CALLBACK_DATA, ls->channels[SORB]);
            SetCtrlAttribute(p, LS335_SENS_NOTE_1,      ATTR_CALLBACK_DATA, ls->channels[SORB]);
    
            SetCtrlAttribute(p, LS335_SENS_KPOTLABEL,   ATTR_CALLBACK_DATA, ls->channels[ONEK]);
            SetCtrlAttribute(p, LS335_SENS_KPOTCOEFF,   ATTR_CALLBACK_DATA, ls->channels[ONEK]);
            SetCtrlAttribute(p, LS335_SENS_KPOTACQ,     ATTR_CALLBACK_DATA, ls->channels[ONEK]);
            SetCtrlAttribute(p, LS335_SENS_NOTE_2,      ATTR_CALLBACK_DATA, ls->channels[ONEK]);
    
            devPanel_Add (p, ls, LS335_UpdateSensorReadings);
            DisplayPanel (p);
        }
        break;
    }
}


/******************************Init and operation functions**********************************/
void *LS335_Create(gpibioPtr dev)
{
    LS335Ptr ls;
    ls = malloc(sizeof(LS335Type));
    if (dev){ dev->device = ls; ls->id = dev->id;}
    
    ls->source = source_Create("temperature", dev, LS335_SetHeaterLvl, LS335_GetHeaterLvl);
    ls->channels[SORB] = acqchan_Create("Sensor A", dev, LS335_GetSensor);
    ls->channels[ONEK] = acqchan_Create("Sensor B", dev, LS335_GetSensor);
    ls->curveload.format = "%s, %s,> %s";
    ls->curveload.serial = "no_number";
    ls->curveload.source = 1;
    ls->curveload.target = 21;
    ls->heater.current = 3;
    ls->heater.input = "A";
    ls->heater.loop = 2;
    ls->heater.maxpower = 5;
    ls->heater.nchange = 0;
    ls->heater.on = 0;
    ls->heater.pchange = 10;
    ls->heater.power = 3;
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
    ls->source->freq = 1;
    return ls;
}

int  LS335_InitGPIB(gpibioPtr dev)
{
    gpibio_Remote(dev);
    if(gpibio_DeviceMatch(dev, "*IDN?", LS335_ID))
        return TRUE;
    return FALSE;
}

void OperateLS335(int menubar, int menuItem, void *callbackData, int panel)
{
    gpibioPtr dev = callbackData;
    LS335Ptr ls = dev->device;
    int p, m;
    
    ls->source->max = ls->heater.setplimit;
    p = dev->iPanel? dev->iPanel: LoadPanel(utilG.p, "LS335u.uir", LS335_CTRL);
    dev->iPanel = p;
    
    SetMenuBarAttribute(menubar, menuItem, ATTR_DIMMED, 1);
    SetPanelAttribute(p, ATTR_TITLE, dev->label);
    
    m = LoadMenuBar(p, "LS335u.uir", LS335);
    SetPanelMenuBar(p, m);
    
    SetMenuBarAttribute(m, LS335_CURVES_LOAD, ATTR_CALLBACK_DATA, dev);
    SetMenuBarAttribute(m, LS335_SOURCE_HEATER, ATTR_CALLBACK_DATA, ls->source);
    SetMenuBarAttribute(m, LS335_MEASURE_MEAS, ATTR_CALLBACK_DATA, ls);
    
    SetCtrlAttribute(p, LS335_CTRL_HEATER, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute(p, LS335_CTRL_HEATER_PROP, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute(p, LS335_CTRL_SORBTSET, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute(p, LS335_CTRL_RAMPSPEED, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute(p, LS335_CTRL_POWER, ATTR_CALLBACK_DATA, dev);
    
    SetCtrlAttribute(p, LS335_CTRL_SORBTSET, ATTR_MAX_VALUE, ls->heater.setplimit);
    SetCtrlAttribute(p, LS335_CTRL_SORBTSET, ATTR_MIN_VALUE, 0.);
    SetPanelAttribute(p, ATTR_CALLBACK_DATA, dev);
    
    LS335_UpdateControls(p, dev);
    devPanel_Add(p, dev, LS335_UpdateReadings);
    
    DisplayPanel(p);
}

void LS335_UpdateReadings(int panel, void *ptr)
{
    gpibioPtr dev = ptr;
    LS335Ptr ls = dev->device;
    LS335_GetSensor(ls->channels[SORB]);
    LS335_GetSensor(ls->channels[ONEK]);
    SetCtrlVal(panel, LS335_CTRL_SORBREAD, ls->channels[SORB]->reading);
    SetCtrlVal(panel, LS335_CTRL_KPOTREAD, ls->channels[ONEK]->reading);
    LS335_GetHeaterLvl(ls->source->acqchan);
    SetCtrlVal(panel, LS335_CTRL_SORBTSET, ls->source->acqchan->reading);
}

void LS335_Save(gpibioPtr dev)
{
    int i;
    LS335Ptr ls = dev->device;
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
    
    for (i = 0; i < 2; i++) acqchan_Save (ls->channels[i]);
    source_Save (ls->source);
}

void LS335_Load(gpibioPtr dev)
{
    int i;
    LS335Ptr ls = dev? dev->device:NULL;
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

    
        for (i = 0; i < 2; i++) acqchan_Load (dev, ls->channels[i]);
        source_Load (dev, ls->source);
        ls->source->max = ls->heater.setplimit;
        ls->source->min = 0;
    }
}

void LS335_Remove(void *dev)
{
    LS335Ptr ls = dev;
    acqchan_Remove(ls->channels[SORB]);
    acqchan_Remove(ls->channels[ONEK]);
    source_Remove(ls->source);
    free(ls);
}

void LS335_Init(void)
{
    devTypePtr devType;
    if(utilG.acq.status != ACQ_NONE)
    {
        util_ChangeInitMessage("ls335 control utilities...");
        devType = malloc(sizeof(devTypeItem));
        if(devType)
        {
            Fmt(devType->label, "ls 335 tmperature controller");
            Fmt(devType->id, LS335_ID);
            devType->CreateDevice = LS335_Create;
            devType->InitDevice = LS335_InitGPIB;
            devType->OperateDevice = OperateLS335;
            devType->UpdateReadings = LS335_UpdateReadings;
            devType->SaveDevice = LS335_Save;
            devType->LoadDevice = LS335_Load;
            devType->RemoveDevice = LS335_Remove;
            devTypeList_AddItem(devType);
        }
    }
}

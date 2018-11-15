// AUXPORT needs to be used instead of FIRSTPORTA 
#include <userint.h>
#include <ansi_c.h>
#include <cbw.h>
#include <formatio.h>
#include <utility.h>

#include "util.h"
#include "list.h"
#include "channel.h"
#include "changen.h"
#include "chanfnc.h"
#include "chanops.h"
#include "acqchan.h"
#include "curve.h"
#include "acqcrv.h"
#include "graph.h"
#include "gpibio.h"
#include "curveop.h"
#include "acquire.h"
#include "acquireu.h"
#include "source.h"
#include "MCCdevices.h"
#include "USB-234.h"
#include "USB-234u.h"     
#include "digitalSource.h"

#define TRUE 1
#define FALSE 0
#define USB_234_ID "USB-234"

typedef struct{
    portPtr Achannels[10], Dchannels[1];
    int panel, menu;    short HiRes ;

}USB234Type;
typedef USB234Type *USB234Ptr;
/***********************************************************************************/
int  USB234_PanelCallback          (int panel, int event, void *callbackData, int eventData1, int eventData2);
int  USB234_ControlCallback        (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void USB234_MenuCallback           (int menbar, int menuItem, void *callbackData, int panel);
int  USB234_MeasureControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  USB234_IndexFromRange         (int val);

void USB234_UpdateMeasurePanel(portPtr port);

void USB234_Create     (MCCdevPtr dev);
void USB234_Remove         (void* ptr);
int  USB234_InitIO         (MCCdevPtr dev);
void USB234Operate         (int menubar, int menuItem, void *callbackData, int panel);
void USB234_UpdateReadings (int panel, void *ptr);
void USB234_Save           (MCCdevPtr dev);
void USB234_Load           (MCCdevPtr dev);
void USB234_Init           (void);





/***********************************************************************************/
int USB234_PanelCallback (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
    USB234Ptr das = callbackData;
    if((event == EVENT_KEYPRESS && eventData1 == VAL_ESC_VKEY) || event == EVENT_RIGHT_DOUBLE_CLICK)
    {
        MCCdevPtr dev = das->Achannels[0]->port.analogueIOport.IO.acqchan->dev;
        int i;
        for (i = 0; i < 8; i++)
            if(das->Achannels[i]->measPanel)
                HidePanel( das->Achannels[i]->measPanel);
        HidePanel(panel);
        devPanel_Remove(panel);
        SetMenuBarAttribute (acquire_GetMenuBar(), dev->menuitem_id, ATTR_DIMMED, FALSE);
    }
    return 0;
}

int USB234_ControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    portPtr port = callbackData;
    switch (control)
    {   
        case DAS_CTRL_RANGE:
            if(event == EVENT_COMMIT && port)
                GetCtrlVal(panel, control, &port->port.analogueIOport.range);
            break;
        case DAS_CTRL_INPUT_NUM:
            if(event == EVENT_COMMIT && port)
            {
                int i;
                MCCdevPtr dev = port->port.analogueIOport.IO.acqchan->dev;
                USB234Ptr das = dev->device;
                GetCtrlVal(panel, control, &i);
                port->control = 0;
                if(i != -1)
                {
                    das->Achannels[i]->control = control;
                    SetCtrlAttribute (panel, control, ATTR_CALLBACK_DATA, das->Achannels[i]);
                    SetCtrlAttribute (panel, DAS_CTRL_RANGE, ATTR_CALLBACK_DATA, das->Achannels[i]);
                    //SetCtrlAttribute (panel, DAS_CTRL_ACQ, ATTR_CALLBACK_DATA, das->Achannels[i]);
                    SetCtrlIndex (panel, DAS_CTRL_RANGE, USB234_IndexFromRange(das->Achannels[i]->port.analogueIOport.range));
                   // SetCtrlVal (panel, DAS_CTRL_ACQ, das->Achannels[i]->port.analogueIOport.IO.acqchan->acquire);
			    }   
				
                SetCtrlAttribute (panel, DAS_CTRL_RANGE, ATTR_DIMMED, (i == -1));
                //SetCtrlAttribute (panel, DAS_CTRL_ACQ, ATTR_DIMMED, (i == -1));
                //if(i == -1)SetCtrlVal (panel, DAS_CTRL_ACQ, 0);
            }
            break;
       /* case DAS_CTRL_ACQ:
            if(event == EVENT_COMMIT && port && utilG.acq.status != ACQ_BUSY)
            {
                int i;
                MCCdevPtr dev = port->port.analogueIOport.IO.acqchan->dev;
                USB234Ptr das = dev->device;
                GetCtrlIndex (panel, DAS_CTRL_INPUT_NUM, &i);
                if(i)
                {
                    GetCtrlVal (panel, control, &port->port.analogueIOport.IO.acqchan->acquire);
                    if (port->port.analogueIOport.IO.acqchan->acquire)
                        acqchanlist_AddChannel(port->port.analogueIOport.IO.acqchan);
                    else
                        acqchanlist_RemoveChannel(port->port.analogueIOport.IO.acqchan);
                    SetMenuBarAttribute (das->menu, port->menuitem_id, ATTR_CHECKED, port->port.analogueIOport.IO.acqchan->acquire);
                }
            else if(event == EVENT_COMMIT && utilG.acq.status == ACQ_BUSY)
                SetCtrlVal (panel, control, port->port.analogueIOport.IO.acqchan->acquire);
            }
            break;  */
    }
    if (event == EVENT_COMMIT  && port)
        USB234_UpdateMeasurePanel(port);
    return 0;
}

void USB234_MenuCallback(int menbar, int menuItem, void *callbackData, int panel)
{
    switch (menuItem)
    {
        case DASMENU_MEAS_IN_0:
        case DASMENU_MEAS_IN_1:
        case DASMENU_MEAS_IN_2:
        case DASMENU_MEAS_IN_3:
        case DASMENU_MEAS_IN_4:
        case DASMENU_MEAS_IN_5:
        case DASMENU_MEAS_IN_6:
        case DASMENU_MEAS_IN_7:
            if(utilG.acq.status != ACQ_BUSY){
                portPtr port = callbackData;
                MCCdevPtr dev = port->port.analogueIOport.IO.acqchan->dev;
                USB234Ptr das = dev->device;
                port->measPanel = port->measPanel? port->measPanel : LoadPanel(utilG.p, "USB-234u.uir", MEASURE);
                SetCtrlAttribute (port->measPanel, MEASURE_RANGE, ATTR_CALLBACK_DATA, port->port.analogueIOport.IO.acqchan);
                SetCtrlAttribute (port->measPanel, MEASURE_LABEL, ATTR_CALLBACK_DATA, port->port.analogueIOport.IO.acqchan);
                SetCtrlAttribute (port->measPanel, MEASURE_COEFF, ATTR_CALLBACK_DATA, port->port.analogueIOport.IO.acqchan);
                SetCtrlAttribute (port->measPanel, MEASURE_NOTE, ATTR_CALLBACK_DATA, port->port.analogueIOport.IO.acqchan);
                SetCtrlAttribute (port->measPanel, MEASURE_ACQ, ATTR_CALLBACK_DATA, port->port.analogueIOport.IO.acqchan);
			
                SetCtrlAttribute (port->measPanel, MEASURE_AVERAGE, ATTR_CALLBACK_DATA, port->port.analogueIOport.IO.acqchan); 
				SetCtrlAttribute (port->measPanel, MEASURE_RATE,ATTR_CALLBACK_DATA, port->port.analogueIOport.IO.acqchan);  
				
                SetPanelAttribute (port->measPanel, ATTR_TITLE, port->port.analogueIOport.IO.acqchan->channel->label);
                
                SetCtrlVal (port->measPanel, MEASURE_LABEL, port->port.analogueIOport.IO.acqchan->channel->label);
                SetCtrlVal (port->measPanel, MEASURE_RANGE, port->port.analogueIOport.range);
                SetCtrlVal (port->measPanel, MEASURE_COEFF, port->port.analogueIOport.IO.acqchan->coeff);
                SetCtrlVal (port->measPanel, MEASURE_NOTE, port->port.analogueIOport.IO.acqchan->note);
                SetCtrlVal (port->measPanel, MEASURE_ACQ, port->port.analogueIOport.IO.acqchan->acquire);
				SetCtrlVal (port->measPanel, MEASURE_AVERAGE, port->averaging); 
				port->sample_rate=25000; //default sampling  rate
				SetCtrlVal (port->measPanel, MEASURE_RATE, port->sample_rate); //? what does it do?
                DisplayPanel (port->measPanel);
            }
            break;
        case DASMENU_SOURCE_DAC1:
        case DASMENU_SOURCE_DAC2:
            {
                sourcePtr src = callbackData;
                switch(utilG.exp)
                {
                    case EXP_SOURCE: source_InitPanel(src); break;
                    case EXP_FLOAT : gensrc_InitPanel(src); break;
                }
            }
            break;
        case DASMENU_SOURCE_DAC3:
            {
                portPtr port = callbackData;
                boards_DigitalSourceInit(port);
            }                                   
            break;
        case DASMENU_FILE_SAVE:
            boards_Save(callbackData, USB234_Save);
            break;
        case DASMENU_FILE_LOAD:
            boards_Load(callbackData, USB234_Load);
            break;
    }
}

int  USB234_MeasureControlCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    acqchanPtr acqchan = callbackData;
    switch (control)
    {
        case MEASURE_RANGE:
            if (event == EVENT_COMMIT)
            {
                portPtr port = acqchan->upLvl;
                GetCtrlVal (panel, control, &port->port.analogueIOport.range);
            }
            break;
        case MEASURE_LABEL:
            if (event == EVENT_COMMIT)
            {
                portPtr port = acqchan->upLvl;
                MCCdevPtr dev = acqchan->dev;
                USB234Ptr das = dev->device;
                GetCtrlVal(panel, control, acqchan->channel->label);
                acqchanlist_ReplaceChannel(acqchan);
                SetPanelAttribute(panel, ATTR_TITLE, acqchan->channel->label);
                SetMenuBarAttribute (das->menu, port->menuitem_id, ATTR_ITEM_NAME,
                                     acqchan->channel->label);
            }
            break;
        case MEASURE_COEFF:
            if (event == EVENT_COMMIT)
                GetCtrlVal(panel, control, &acqchan->coeff);
            break;
			
		case MEASURE_RATE:
            if (event == EVENT_COMMIT)
			{portPtr port = acqchan->upLvl;  
                GetCtrlVal(panel, control, &port->sample_rate);}
            break;	
		case MEASURE_AVERAGE:
            if (event == EVENT_COMMIT)
                {portPtr port = acqchan->upLvl; 
				GetCtrlVal(panel, control, &port->averaging);
				if ((port->averaging)>(port->sample_rate/2)) port->averaging=1; SetCtrlVal(panel, control,port->averaging);}
            break;		
        case MEASURE_ACQ:
            if (event == EVENT_COMMIT && utilG.acq.status != ACQ_BUSY)
            {
                portPtr port = acqchan->upLvl;
                MCCdevPtr dev = acqchan->dev;
                USB234Ptr das = dev->device;
                GetCtrlVal(panel, control, &acqchan->acquire);
                if (acqchan->acquire) acqchanlist_AddChannel(acqchan);
                else acqchanlist_RemoveChannel(acqchan);
                SetMenuBarAttribute (das->menu, port->menuitem_id, ATTR_CHECKED, acqchan->acquire);
            }
            else if(event == EVENT_COMMIT && utilG.acq.status == ACQ_BUSY)
                SetCtrlVal (panel, control, acqchan->acquire);
            break;
		
            
    }
    return 0;
}

int  USB234_IndexFromRange (int val)
{
    int range=0;
    switch(val)
    {
        case BIP10VOLTS:    range = 0;break;
        
    }
    return range;
}

void USB234_UpdateMeasurePanel(portPtr port)
{
    if(port->measPanel)
    {
        SetCtrlVal (port->measPanel, MEASURE_RANGE, port->port.analogueIOport.range);
        SetCtrlVal (port->measPanel, MEASURE_LABEL, port->port.analogueIOport.IO.acqchan->channel->label);
        SetCtrlVal (port->measPanel, MEASURE_COEFF, port->port.analogueIOport.IO.acqchan->coeff);
        SetCtrlVal (port->measPanel, MEASURE_ACQ, port->port.analogueIOport.IO.acqchan->acquire);
    }
}

/***********************************************************************************/
void USB234_Create (MCCdevPtr dev)
{
    int i;
    char name[50];
    USB234Ptr das = malloc(sizeof(USB234Type));
    dev->resolution=0;  //16 bit board
    dev->device = das;
    das->panel = 0;
    for(i = 0; i < 8; i++)
    {
        //portPtr create_Port(*dev, *name, type, direction, GetReading, channel, range)
        Fmt (name, "USB-234 AI %i", i);
        das->Achannels[i] = create_Port(dev, name, ANALOGUE, IN_PORT, ReadAnalogue, i, BIP10VOLTS);
        das->Achannels[i]->control = 0;
    }
    for(i = 0; i < 2; i++)
    {
        //portPtr create_Port(*dev, *name, type, direction, GetReading, SetLevel, channel, range)
        Fmt (name, "USB-234 AO %i", i);
        das->Achannels[i+8] = create_Port(dev, name, ANALOGUE, OUT_PORT, ReadAnalogueOut, SetAnalogue, i, BIP10VOLTS);
        das->Achannels[i+8]->port.analogueIOport.IO.source->max = 10;
        das->Achannels[i+8]->port.analogueIOport.IO.source->min = -10;
        das->Achannels[i+8]->port.analogueIOport.IO.source->ranges.temprange[0] = range_Create(10, -10, 0.000305);
    }
    das->Dchannels[0] = create_Port (dev, "USB-234 digital out", DIGITAL, OUT_PORT, ReadDigitalOut, SetDigital, AUXPORT, 0, 8);  //instead of FIRSTPORTA 8 bis 
    das->Achannels[0]->menuitem_id = DASMENU_MEAS_IN_0;
    das->Achannels[1]->menuitem_id = DASMENU_MEAS_IN_1;
    das->Achannels[2]->menuitem_id = DASMENU_MEAS_IN_2;
    das->Achannels[3]->menuitem_id = DASMENU_MEAS_IN_3;
    das->Achannels[4]->menuitem_id = DASMENU_MEAS_IN_4;
    das->Achannels[5]->menuitem_id = DASMENU_MEAS_IN_5;
    das->Achannels[6]->menuitem_id = DASMENU_MEAS_IN_6;
    das->Achannels[7]->menuitem_id = DASMENU_MEAS_IN_7;
}

void USB234_Remove (void* ptr)
{
    MCCdevPtr dev = ptr;
    USB234Ptr das = dev->device;
    int i;
    if(das->panel)
    {
        devPanel_Remove(das->panel);
        DiscardPanel(das->panel);
    }
    for(i = 0; i < 8; i++)
        acqchan_Remove(das->Achannels[i]->port.analogueIOport.IO.acqchan);
    for(i = 8; i < 10; i++)
        source_Remove (das->Achannels[i]->port.analogueIOport.IO.source);
    free(das);
}

void USB234Operate (int menubar, int menuItem, void *callbackData, int panel)
{
    MCCdevPtr dev = callbackData;
    USB234Ptr das = dev->device;
    acqchanPtr acqchan;
    int i, x = 0, m;
	
    if(!das->panel)
    {
        das->panel = LoadPanel(utilG.p, "USB-234u.uir", DAS_CTRL);
        das->menu = LoadMenuBar(das->panel, "USB-234u.uir", DASMENU);
        SetPanelMenuBar(das->panel, das->menu);
        for(i = 0; i < 8; i ++)
        {
            SetMenuBarAttribute (das->menu, das->Achannels[i]->menuitem_id, ATTR_CALLBACK_DATA, das->Achannels[i]);
            SetMenuBarAttribute (das->menu, das->Achannels[i]->menuitem_id, ATTR_ITEM_NAME, das->Achannels[i]->port.analogueIOport.IO.acqchan->channel->label);
            SetMenuBarAttribute (das->menu, das->Achannels[i]->menuitem_id, ATTR_CHECKED, das->Achannels[i]->port.analogueIOport.IO.acqchan->acquire);
        }
        SetMenuBarAttribute (das->menu, DASMENU_SOURCE_DAC1, ATTR_CALLBACK_DATA, das->Achannels[8]->port.analogueIOport.IO.source);
        SetMenuBarAttribute (das->menu, DASMENU_SOURCE_DAC2, ATTR_CALLBACK_DATA, das->Achannels[9]->port.analogueIOport.IO.source);
        SetMenuBarAttribute (das->menu, DASMENU_SOURCE_DAC3, ATTR_CALLBACK_DATA, das->Dchannels[0]);
        SetMenuBarAttribute (das->menu, DASMENU_SOURCE_DAC1, ATTR_ITEM_NAME, das->Achannels[8]->port.analogueIOport.IO.source->acqchan->channel->label);
        SetMenuBarAttribute (das->menu, DASMENU_SOURCE_DAC2, ATTR_ITEM_NAME, das->Achannels[9]->port.analogueIOport.IO.source->acqchan->channel->label);
        SetMenuBarAttribute (das->menu, DASMENU_SOURCE_DAC3, ATTR_ITEM_NAME, das->Dchannels[0]->port.digitalIOport.IO.source->acqchan->channel->label);
        SetMenuBarAttribute (das->menu, DASMENU_FILE_SAVE, ATTR_CALLBACK_DATA, dev);
        SetMenuBarAttribute (das->menu, DASMENU_FILE_LOAD, ATTR_CALLBACK_DATA, dev);
        
        SetCtrlAttribute (das->panel, DAS_CTRL_INPUT_NUM, ATTR_CALLBACK_DATA, das->Achannels[0]);
        SetPanelAttribute (das->panel, ATTR_CALLBACK_DATA, das);
    }
    SetMenuBarAttribute(menubar, menuItem, ATTR_DIMMED, 1);
    devPanel_Add (das->panel, dev, USB234_UpdateReadings);
    DisplayPanel(das->panel);
}

void USB234_UpdateReadings (int panel, void *ptr)
{
    int i;
    MCCdevPtr dev = ptr;
    USB234Ptr das = dev->device;
    acqchanPtr acqchan;
    for(i = 0; i < 8; i++)
    {
        acqchan = das->Achannels[i]->port.analogueIOport.IO.acqchan;
        ReadAnalogue(acqchan);
	// if dev	        ReadAnalogue(acqchan);

        if(das->Achannels[i]->control)  
        {
            SetCtrlVal (das->panel, DAS_CTRL_ANALOGUE_IN, acqchan->reading);
            SetCtrlVal (das->panel, DAS_CTRL_RANGE, das->Achannels[i]->port.analogueIOport.range);
            //SetCtrlVal (das->panel, DAS_CTRL_ACQ, acqchan->acquire);
        }
    }
}

void USB234_Save (MCCdevPtr dev)
{
    int i;
    USB234Ptr das = dev->device;
    for(i = 0; i < 10; i++)
        port_Save(das->Achannels[i]);
    port_Save(das->Dchannels[0]);
}

void USB234_Load (MCCdevPtr dev)
{
    int i;
    USB234Ptr das = dev->device;
    for(i = 0; i < 10; i++)
        port_Load(dev, das->Achannels[i]);
    for(i = 8; i < 10; i++)
    {
        das->Achannels[i]->port.analogueIOport.IO.source->max = 10;
        das->Achannels[i]->port.analogueIOport.IO.source->min = -10;
        das->Achannels[i]->port.analogueIOport.IO.source->ranges.range[0] = range_Create(10, -10, 0.000305);
    }
    port_Load(dev, das->Dchannels[0]);
}

void USB234_Init(void)
{
    MCCdevTypePtr devType = malloc(sizeof(MCCdevTypeItem));
    if(utilG.acq.status != ACQ_NONE)
    {
        util_ChangeInitMessage ("USB-234...");
        if(devType)
        {
            Fmt(devType->id, USB_234_ID);
            Fmt(devType->label, "USB-234");
            devType->CreateDevice   = USB234_Create;
            devType->RemoveDevice   = USB234_Remove;
            devType->OperateDevice  = USB234Operate;
            devType->UpdateReadings = USB234_UpdateReadings;
            devType->SaveDevice     = USB234_Save;
            devType->LoadDevice     = USB234_Load;
            boards_DevTypeList_AddDev(devType);
			//set all bits to  zero
			//cbDConfigPort(dev->BoardNum, AUXPORT, DIGITALOUT);
			//cbDOut(dev->BoardNum, AUXPORT,0); }// all bits to 0 
        }
    }
}


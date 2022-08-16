#include <utility.h>
#include <formatio.h>
#include <ansi_c.h>
#include <userint.h>

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


#include "k6485.h"
#include "k6485u.h"


#define K6485_ID "KEITHLEY INSTRUMENTS INC.,MODEL 6485"

/* ESE - event status enable */

#define K6485_ESE_OPC 1     /* Operation complete */
#define K6485_ESE_QYE 4     /* Query error */
#define K6485_ESE_DDE 8     /* Device dependent error */
#define K6485_ESE_EXE 16    /* Execution error */
#define K6485_ESE_CME 32    /* Command error */
#define K6485_ESE_URQ 64    /* User request */
#define K6485_ESE_PON 128   /* Power cycled on / off */

#define K6485_ESE K6485_ESE_OPC+K6485_ESE_QYE+K6485_ESE_DDE+K6485_ESE_EXE+K6485_ESE_CME+K6485_ESE_URQ+K6485_ESE_PON

/* MSE - measurement status enable */

#define K6485_MSE_ROF 1     /* Reading overflow */
#define K6485_MSE_LL  2     /* Low limit */
#define K6485_MSE_HL  4     /* High limit */
#define K6485_MSE_RAV 32    /* Reading available */
#define K6485_MSE_BAV 128   /* Buffer available */
#define K6485_MSE_BHF 256   /* Buffer half full */
#define K6485_MSE_BFL 512   /* Buffer full */

#define K6485_MSE K6485_MSE_ROF+K6485_MSE_LL+K6485_MSE_HL+K6485_MSE_RAV+K6485_MSE_BAV+K6485_MSE_BHF+K6485_MSE_BFL

/* QSE - questionable status enable */

#define K6485_QSE_TEMP 16       /* bogus reference junction measurement */
#define K6485_QSE_CAL  256      /* invalid cal constant detected during power up */
#define K6485_QSE_WARN 16384    /* signal oriented measurement command parmeter ignored */

#define K6485_QSE K6485_QSE_TEMP+K6485_QSE_CAL+K6485_QSE_WARN

/* OSE - operation status enable */

#define K6485_OSE_MEAS 16       /* performing measurement */
#define K6485_OSE_TRIG 32       /* triggering */
#define K6485_OSE_IDLE 1024     /* idle */

#define K6485_OSE K6485_OSE_MEAS+K6485_OSE_TRIG+K6485_OSE_IDLE

/* SRE - service request enable */

#define K6485_SRE_CLEAR 0
#define K6485_SRE_MSB 1     /* Measurement summary */
#define K6485_SRE_EAV 4     /* Error available */
#define K6485_SRE_QSB 8     /* Questionable summary */
#define K6485_SRE_MAV 16    /* Message available */
#define K6485_SRE_ESB 32    /* Event summary */
#define K6485_SRE_OSB 128   /* Operation summary */

#define K6485_SRE K6485_SRE_EAV+K6485_SRE_QSB+K6485_SRE_ESB+K6485_SRE_MAV+K6485_SRE_MSB+K6485_SRE_OSB

#define TRUE 1
#define FALSE 0

typedef struct {
    acqchanPtr acqchan;
    struct {int enable, readings, moving;} filter;
    int overLimit;
    double rate, range;
}   k6485chanType;

typedef k6485chanType *k6485chanPtr;

typedef struct {
    //k6485chanPtr chan[11];
	//k6485chanPtr chan;
    int id;
    int operation, func, scanCard;
}   k6485Type;

typedef k6485Type *k6485Ptr;

static void k6485_GetErrorMessage (gpibioPtr dev);
static void k6485_GetEventSummary (gpibioPtr dev);
static void k6485_GetQuestionableSummary (gpibioPtr dev);
static void k6485_GetOperationSummary (int panel, gpibioPtr dev);
static void k6485_CheckforProblems (gpibioPtr dev);

static void k6485_Out (gpibioPtr dev, char *cmd);
static void k6485_In (gpibioPtr dev, char *msg);

static char     *k6485_MeasFunction (gpibioPtr dev);
static int      k6485_GetChannel (gpibioPtr dev);
static int      k6485_GetFunction (gpibioPtr dev);
static double   k6485_GetRate (gpibioPtr dev);
static int      k6485_GetFilterStatus (gpibioPtr dev);
static int      k6485_GetMovingFilterType (gpibioPtr dev);
static int      k6485_GetFilterReadings (gpibioPtr dev);
static int      k6485_GetAutoRange (gpibioPtr dev);
static double   k6485_GetRange (gpibioPtr dev);
static int      k6485_GetMeasurementStatus (gpibioPtr dev);
static double   k6485_GetMeasurement (int chan, gpibioPtr dev);

static void     k6485_GetReadings (acqchanPtr acqchan);
/*static void     k6485_GetCh0Readings (acqchanPtr acqchan);
static void     k6485_GetCh1Readings (acqchanPtr acqchan);
static void     k6485_GetCh2Readings (acqchanPtr acqchan);
static void     k6485_GetCh3Readings (acqchanPtr acqchan);
static void     k6485_GetCh4Readings (acqchanPtr acqchan);
static void     k6485_GetCh5Readings (acqchanPtr acqchan);
static void     k6485_GetCh6Readings (acqchanPtr acqchan);
static void     k6485_GetCh7Readings (acqchanPtr acqchan);
static void     k6485_GetCh8Readings (acqchanPtr acqchan);
static void     k6485_GetCh9Readings (acqchanPtr acqchan);
static void     k6485_GetCh10Readings (acqchanPtr acqchan);*/

static void     k6485_UpdateControls (int panel, gpibioPtr dev);
static void     k6485_SaveChannel (k6485chanPtr chan);
static void     k6485_LoadChannel (int chan, gpibioPtr dev);

static int k6485_InitGPIB (gpibioPtr dev);
static void *k6485_Create (gpibioPtr dev);
static void k6485_UpdateReadings (int panel, void *dev);
static void k6485_Operate (int menubar, int menuItem, void *callbackData, int panel);
static void k6485_Save (gpibioPtr dev);
static void k6485_Load (gpibioPtr dev);
static void k6485_Remove (void *ptr);


/***************************************************************************/

void k6485_Init (void)
{
    devTypePtr devType;
    if (utilG.acq.status != ACQ_NONE) {
        util_ChangeInitMessage ("Keithley 6485 Control Utilities...");
        devType = malloc (sizeof (devTypeItem));
        if (devType) {
            Fmt (devType->label, "Keithley 6485 Picoammeter");
            Fmt (devType->id, K6485_ID);
            devType->InitDevice = k6485_InitGPIB;
            devType->CreateDevice = k6485_Create;
            devType->UpdateReadings = k6485_UpdateReadings;
            devType->OperateDevice = k6485_Operate;
            devType->SaveDevice = k6485_Save;
            devType->LoadDevice = k6485_Load;
            devType->RemoveDevice = k6485_Remove;
            devTypeList_AddItem (devType);
        }
    }
}

static void k6485_Remove (void *ptr)
{
    k6485Ptr dmm = ptr;
    int i;
    //acqchan_Remove(dmm->chan[0]->acqchan);
	acqchan_Remove(dmm->chan->acqchan);
   // if (dmm->scanCard)
    //    for (i = 1; i < 11; i++) acqchan_Remove(dmm->chan[i]->acqchan);
    free (dmm);
}

static void k6485_Load (gpibioPtr dev)
{
    char cmd[256];
    double r;
    int i, scanCard;
    k6485Ptr dmm;

    if (dev) dmm = dev->device;
    else dmm = k6485_Create (NULL);

    ScanFile (fileHandle.analysis, "%s>Scan Card: %i%s[dw1]", &scanCard);

    k6485_LoadChannel (0, dev);
    if (scanCard) {
        for (i = 1; i < 11; i++)
            k6485_LoadChannel (i, dev);
    }

    if (!dev) k6485_Remove (dmm);
}

static void k6485_Save (gpibioPtr dev)
{
    k6485Ptr dmm = dev->device;
    int i;

    FmtFile (fileHandle.analysis, "Scan Card: %i\n", dmm->scanCard);
    k6485_SaveChannel (dmm->chan);
    /*if (dmm->scanCard) {
        for (i = 1; i < 11; i++)
            k6485_SaveChannel (dmm->chan[i]);
    }*/
}

static void k6485_Operate (int menubar, int menuItem, void *callbackData, int panel)
{
    int p, m;
    gpibioPtr dev = callbackData;
    k6485Ptr dmm = dev->device;
    char label[256];

    SetMenuBarAttribute (menubar, menuItem, ATTR_DIMMED, TRUE);

    p = dev->iPanel? dev->iPanel: LoadPanel (utilG.p, "k6485u.uir", K6485);
    dev->iPanel = p;
	
	SetPanelPos (p, VAL_AUTO_CENTER, VAL_AUTO_CENTER);

    Fmt (label, "Keithley 6485 Multimeter: %s", dev->label);
    SetPanelAttribute (p, ATTR_TITLE, label);

    m = LoadMenuBar (p, "k6485u.uir", K6485MENUS);
    
    SetPanelMenuBar (p, m);

    SetMenuBarAttribute (m, K6485MENUS_FILE_SAVE, ATTR_CALLBACK_DATA, dev);
    SetMenuBarAttribute (m, K6485MENUS_FILE_LOAD, ATTR_CALLBACK_DATA, dev);
    SetMenuBarAttribute (m, K6485MENUS_FILE_GPIB, ATTR_CALLBACK_DATA, dev);

   // SetMenuBarAttribute (m, K6485MENUS_MEASURE_DMM, ATTR_CALLBACK_DATA, dmm->chan->acqchan);

   /* SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH1, ATTR_CALLBACK_DATA, dmm->chan[1]->acqchan);
    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH1, ATTR_DIMMED,!dmm->scanCard);

    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH2, ATTR_CALLBACK_DATA, dmm->chan[2]->acqchan);
    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH2, ATTR_DIMMED,!dmm->scanCard);

    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH3, ATTR_CALLBACK_DATA, dmm->chan[3]->acqchan);
    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH3, ATTR_DIMMED,!dmm->scanCard);

    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH4, ATTR_CALLBACK_DATA, dmm->chan[4]->acqchan);
    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH4, ATTR_DIMMED,!dmm->scanCard);

    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH5, ATTR_CALLBACK_DATA, dmm->chan[5]->acqchan);
    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH5, ATTR_DIMMED,!dmm->scanCard);

    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH6, ATTR_CALLBACK_DATA, dmm->chan[6]->acqchan);
    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH6, ATTR_DIMMED,!dmm->scanCard);

    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH7, ATTR_CALLBACK_DATA, dmm->chan[7]->acqchan);
    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH7, ATTR_DIMMED,!dmm->scanCard);

    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH8, ATTR_CALLBACK_DATA, dmm->chan[8]->acqchan);
    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH8, ATTR_DIMMED,!dmm->scanCard);

    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH9, ATTR_CALLBACK_DATA, dmm->chan[9]->acqchan);
    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH9, ATTR_DIMMED,!dmm->scanCard);

    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH10, ATTR_CALLBACK_DATA, dmm->chan[10]->acqchan);
    SetMenuBarAttribute (m, K6485MENUS_MEASURE_CH10, ATTR_DIMMED,!dmm->scanCard);*/

    SetPanelAttribute (p, ATTR_CALLBACK_DATA, dev);

    //SetCtrlAttribute (p, K6485_MONITOR, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, K6485_DCV, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, K6485_ACV, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K6485_DCI, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, K6485_ACI, ATTR_CALLBACK_DATA, dev);
    //SetCtrlAttribute (p, K6485_OHMS2, ATTR_CALLBACK_DATA, dev);
   // SetCtrlAttribute (p, K6485_OHMS4, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K6485_RATE, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K6485_FILTER_ENABLE, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K6485_FILTER_READINGS, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K6485_FILTER_TYPE, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K6485_RANGE, ATTR_CALLBACK_DATA, dev);
    SetCtrlAttribute (p, K6485_RANGE_AUTO, ATTR_CALLBACK_DATA, dev);

    k6485_UpdateControls (p, dev);

    devPanel_Add (p, dev, k6485_UpdateReadings);

    DisplayPanel (p);
}

static void k6485_UpdateReadings (int panel, void *dev)
{
    gpibioPtr my_dev = dev;
    k6485Ptr dmm = my_dev->device;
    char rsp[256];
    int chan, status = 0;
    short statusbyte;

    if (gpibio_SRQ (my_dev)) {
        gpibio_GetStatusByte (my_dev, &statusbyte);
        if (statusbyte & K6485_SRE_EAV) k6485_GetErrorMessage (my_dev);
        if (statusbyte & K6485_SRE_ESB) k6485_GetEventSummary (my_dev);
        if (statusbyte & K6485_SRE_QSB) k6485_GetQuestionableSummary (my_dev);
        if (statusbyte & K6485_SRE_MSB) status = k6485_GetMeasurementStatus (my_dev);
        if (statusbyte & K6485_SRE_OSB) k6485_GetOperationSummary(panel, my_dev);
    }

    chan = k6485_GetChannel (my_dev);
    if ((!util_TakingData() || !dmm->chan->acqchan->acquire)) {
        k6485_Out (dev, "fetch?");
        k6485_In (dev, rsp);
        Scan (rsp, "%s>%f", &dmm->chan->acqchan->reading);
        dmm->chan->overLimit =  status & K6485_MSE_ROF;
    }

    SetCtrlVal (panel, K6485_OVER, dmm->chan->overLimit);
    SetCtrlVal (panel, K6485_MEASURE, dmm->chan->acqchan->reading);
}

static void *k6485_Create (gpibioPtr dev)
{
    k6485Ptr dmm = NULL;
    char msg[256];
    int i;

    dmm = malloc (sizeof(k6485Type));
   /* for (i = 0; i < 11; i++) {
        dmm->chan[i] = malloc (sizeof(k6485chanType));
        if (dmm->chan[i]) {
            dmm->chan[i]->overLimit = FALSE;
            dmm->chan[i]->rate = 2;
            dmm->chan[i]->filter.enable = FALSE;
            dmm->chan[i]->filter.readings = 10;
            dmm->chan[i]->filter.moving = TRUE;
            dmm->chan[i]->range = 0;
        }*/
		
        dmm->chan= malloc (sizeof(k6485chanType));
        if (dmm->chan) {
            dmm->chan->overLimit = FALSE;
            dmm->chan->rate = 2;
            dmm->chan->filter.enable = FALSE;
            dmm->chan->filter.readings = 10;
            dmm->chan->filter.moving = TRUE;
            dmm->chan->range = 0;
        }
    

      dmm->chan->acqchan = acqchan_Create ("K6485 Picoammeter", dev, k6485_GetReadings);
	   dmm->scanCard = FALSE;
	/*if (dmm->chan[0]) dmm->chan[0]->acqchan = acqchan_Create ("K6485 Multimeter", dev, k6485_GetCh0Readings);
    if (dmm->chan[1]) dmm->chan[1]->acqchan = acqchan_Create ("K6485 CH1", dev, k6485_GetCh1Readings);
    if (dmm->chan[2]) dmm->chan[2]->acqchan = acqchan_Create ("K6485 CH2", dev, k6485_GetCh2Readings);
    if (dmm->chan[3]) dmm->chan[3]->acqchan = acqchan_Create ("K6485 CH3", dev, k6485_GetCh3Readings);
    if (dmm->chan[4]) dmm->chan[4]->acqchan = acqchan_Create ("K6485 CH4", dev, k6485_GetCh4Readings);
    if (dmm->chan[5]) dmm->chan[5]->acqchan = acqchan_Create ("K6485 CH5", dev, k6485_GetCh5Readings);
    if (dmm->chan[6]) dmm->chan[6]->acqchan = acqchan_Create ("K6485 CH6", dev, k6485_GetCh6Readings);
    if (dmm->chan[7]) dmm->chan[7]->acqchan = acqchan_Create ("K6485 CH7", dev, k6485_GetCh7Readings);
    if (dmm->chan[8]) dmm->chan[8]->acqchan = acqchan_Create ("K6485 CH8", dev, k6485_GetCh8Readings);
    if (dmm->chan[9]) dmm->chan[9]->acqchan = acqchan_Create ("K6485 CH9", dev, k6485_GetCh9Readings);
    if (dmm->chan[10]) dmm->chan[10]->acqchan = acqchan_Create ("K6485 CH10", dev, k6485_GetCh10Readings);*/

  /*  if (dmm && dev) {
        dmm->id = dev->id;
        dev->device = dmm;
        gpibio_Out (dev, "*OPT?");
        gpibio_In (dev, msg);
        if (CompareBytes ("0,0", 0, msg, 0, 3, 0) == 0) dmm->scanCard = FALSE;
            else dmm->scanCard = TRUE;
    }*/
    return dmm;
}

static int k6485_InitGPIB (gpibioPtr dev)
{
    char cmd[256], rsp[256], buffer[256];
    int result;

    gpibio_Remote (dev);

    if (gpibio_DeviceMatch (dev, "*IDN?", K6485_ID)) {
        gpibio_Out (dev, "*CLS");

        Fmt (cmd, "%s<*SRE %i", K6485_SRE);
        gpibio_Out (dev, cmd);

        Fmt (cmd, "%s<*ESE %i", K6485_ESE);
        gpibio_Out (dev, cmd);

        gpibio_Out (dev, "format:data ascii");
        gpibio_Out (dev, "initiate:continuous 1");
        gpibio_Out (dev, "trigger:source imm");

        Fmt (cmd, "status:measurement:enable %i", K6485_MSE);
        gpibio_Out (dev, cmd);

        Fmt (cmd, "status:operation:enable %i", K6485_OSE);
        gpibio_Out (dev, cmd);

        Fmt (cmd, "status:questionable:enable %i", K6485_QSE);
        gpibio_Out (dev, cmd);

        return TRUE;
    }
    return FALSE;
}

/***************************************************************************/

void k6485_LoadCallback(int menubar, int menuItem, void *callbackData, int panel)
{
    int fileselect, id;
    char path[256], info[256];
    gpibioPtr dev = callbackData;
    k6485Ptr dmm = dev->device;

    fileselect = FileSelectPopup ("", "*.dev", "*.dev", "Load Keithley 6485 Setup",
                                  VAL_LOAD_BUTTON, 0, 1, 1, 0, path);
    if (fileselect == VAL_EXISTING_FILE_SELECTED) {
        fileHandle.analysis = util_OpenFile (path, FILE_READ, FALSE);
        ScanFile (fileHandle.analysis, "%s>#INSTRSETUP %i", &id);
        if (dmm->id == id) {
            k6485_Load (dev);
            ReadLine (fileHandle.analysis, info, 255);
            k6485_UpdateControls(panel, dev);
        }
        else MessagePopup ("Keithley Load Message", "Different instrument types--process aborted");
        util_CloseFile();
    }
}

static void k6485_LoadChannel (int chan, gpibioPtr dev)
{
    k6485Ptr dmm = NULL;
    char info[256];
    if (dev) {
        dmm = dev->device;
        ScanFile (fileHandle.analysis, "%s>Rate  : %f%s[dw1]", &dmm->chan->rate);
        ScanFile (fileHandle.analysis, "%s>Range : %f%s[dw1]", &dmm->chan->range);
        ScanFile (fileHandle.analysis, "%s>Filter: %i, %i, %i%s[dw1]",
              &dmm->chan->filter.enable, &dmm->chan->filter.moving,
              &dmm->chan->filter.readings);
        acqchan_Load (dev, dmm->chan->acqchan);
    } else {
        ReadLine (fileHandle.analysis, info, 255);
        ReadLine (fileHandle.analysis, info, 255);
        ReadLine (fileHandle.analysis, info, 255);
        acqchan_Load (dev, NULL);
    }
}

void k6485_SaveCallback(int menubar, int menuItem, void *callbackData, int panel)
{
    int fileselect;
    char path[256];
    gpibioPtr dev = callbackData;
    k6485Ptr dmm = dev->device;
    fileselect = FileSelectPopup ("", "*.dev", "*.dev", "Save K6485 Setup",
                                  VAL_SAVE_BUTTON, 0, 1, 1, 1, path);
    if (fileselect == VAL_NEW_FILE_SELECTED) {
        fileHandle.analysis = util_OpenFile (path, FILE_WRITE, FALSE);
        FmtFile (fileHandle.analysis, "%s<#INSTRSETUP %i\n", dmm->id);
        k6485_Save(dev);
        FmtFile (fileHandle.analysis, "#ENDSETUP\n");
        util_CloseFile();
    }
}

static void k6485_SaveChannel (k6485chanPtr chan)
{
    FmtFile (fileHandle.analysis, "Rate  : %f[p2]\n", chan->rate);
    FmtFile (fileHandle.analysis, "Range : %f[e2p2]\n", chan->range);
    FmtFile (fileHandle.analysis, "Filter: %i, %i, %i\n",
             chan->filter.enable, chan->filter.moving, chan->filter.readings);
    acqchan_Save (chan->acqchan);
}

int  k6485_ChannelControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    acqchanPtr acqchan = callbackData;
    char label[256];
    switch (control) {
        case K6485_CHAN_ACQ:
            if (event == EVENT_VAL_CHANGED) {
                GetCtrlVal (panel, control, &acqchan->acquire);
                if (acqchan->acquire) acqchanlist_AddChannel (acqchan);
                    else acqchanlist_RemoveChannel (acqchan);
            }
            break;
        case K6485_CHAN_COEFF:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &acqchan->coeff);
                if (acqchan->p) SetCtrlVal (acqchan->p, ACQDATA_COEFF, acqchan->coeff);
            }
            break;
        case K6485_CHAN_LABEL:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, acqchan->channel->label);
                acqchanlist_ReplaceChannel (acqchan);
                if (acqchan->p) SetPanelAttribute (acqchan->p, ATTR_TITLE, acqchan->channel->label);
                Fmt (label, "K6485 Channel Setup: %s", acqchan->channel->label);
                SetPanelAttribute (panel, ATTR_TITLE, label);
            }
            break;
        case K6485_CHAN_CLOSE:
            if (event == EVENT_COMMIT) {
                devPanel_Remove(panel);
                
                DiscardPanel (panel);
            }
            break;
    }
    return 0;
}

void k6485_ChannelSetupCallback(int menubar, int menuItem, void *callbackData, int panel)
{
    int p;
    acqchanPtr chan = callbackData;
    char label[256];

    p = LoadPanel (0, "k6485u.uir", K6485_CHAN);
    
    SetPanelPos (p, VAL_AUTO_CENTER, VAL_AUTO_CENTER);
    
    util_InitClose (p, K6485_CHAN_CLOSE, FALSE);

    SetCtrlVal (p, K6485_CHAN_LABEL, chan->channel->label);
    SetCtrlVal (p, K6485_CHAN_COEFF, chan->coeff);
    SetCtrlVal (p, K6485_CHAN_ACQ, chan->acquire);
    SetCtrlVal (p, K6485_CHAN_NOTE, chan->note);

    SetCtrlAttribute(p, K6485_CHAN_LABEL, ATTR_CALLBACK_DATA, chan);
    SetCtrlAttribute(p, K6485_CHAN_COEFF, ATTR_CALLBACK_DATA, chan);
    SetCtrlAttribute(p, K6485_CHAN_ACQ, ATTR_CALLBACK_DATA, chan);
    SetCtrlAttribute(p, K6485_CHAN_NOTE, ATTR_CALLBACK_DATA, chan);

    Fmt (label, "K6485 Channel Setup: %s", chan->channel->label);
    SetPanelAttribute (p, ATTR_TITLE, label);

    SetInputMode (p, K6485_CHAN_ACQ, !util_TakingData());
    InstallPopup (p);
}

int  k6485_ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    gpibioPtr dev = callbackData;
    k6485Ptr dmm = dev->device;
    char cmd[256];
    int chan, autorange;

    switch (control) {
        case K6485_FILTER_TYPE:
            if (event == EVENT_COMMIT) {
                chan = k6485_GetChannel (dev);
                GetCtrlVal (panel, control, &dmm->chan->filter.moving);
                if (dmm->chan->filter.moving)
                    Fmt (cmd, "%s:average:tcontrol mov", k6485_MeasFunction(dev));
                else
                    Fmt (cmd, "%s:average:tcontrol rep", k6485_MeasFunction(dev));
                k6485_Out (dev, cmd);
            }
            break;
        case K6485_FILTER_READINGS:
            if (event == EVENT_COMMIT) {
                chan = k6485_GetChannel (dev);
                GetCtrlVal (panel, control, &dmm->chan->filter.readings);
                Fmt (cmd, "%s:average:count %i", k6485_MeasFunction(dev), dmm->chan->filter.readings);
                k6485_Out (dev, cmd);
            }
            break;
        case K6485_FILTER_ENABLE:
            if (event == EVENT_COMMIT) {
                chan = k6485_GetChannel (dev);
                GetCtrlVal (panel, control, &dmm->chan->filter.enable);
                Fmt (cmd, "%s:average:state %i", k6485_MeasFunction(dev), dmm->chan->filter.enable);
                k6485_Out (dev, cmd);
                SetInputMode (panel, K6485_FILTER_READINGS, dmm->chan->filter.enable);
                SetInputMode (panel, K6485_FILTER_TYPE, dmm->chan->filter.enable);
                if (dmm->chan->filter.enable) {
                    Fmt (cmd, "%s:average:count %i", k6485_MeasFunction(dev), dmm->chan->filter.readings);
                    k6485_Out (dev, cmd);
                    if (dmm->chan->filter.moving)
                        Fmt (cmd, "%s:average:tcontrol mov", k6485_MeasFunction(dev));
                    else
                        Fmt (cmd, "%s:average:tcontrol rep", k6485_MeasFunction(dev));
                    k6485_Out (dev, cmd);
                }
            }
            break;
        case K6485_RANGE_AUTO:
            if (event == EVENT_COMMIT) {
                chan = k6485_GetChannel (dev);
                GetCtrlVal (panel, control, &autorange);
                Fmt (cmd, "%s:range:auto %i", k6485_MeasFunction(dev), autorange);
                k6485_Out (dev, cmd);
                SetInputMode (panel, K6485_RANGE, !autorange);
                if (autorange) dmm->chan->range = 0.0;
                else {
                    dmm->chan->range = k6485_GetRange (dev);
                    SetCtrlVal (panel, K6485_RANGE, dmm->chan->range);
                }
            }
            break;
        case K6485_RANGE:
            if (event == EVENT_COMMIT) {
                chan = k6485_GetChannel (dev);
                GetCtrlVal (panel, control, &dmm->chan->range);
                Fmt (cmd, "%s:range:upper %f[p2]", k6485_MeasFunction (dev), dmm->chan->range);
                k6485_Out (dev, cmd);
            }
            break;
        case K6485_RATE:
            if (event == EVENT_COMMIT) {
                chan = k6485_GetChannel (dev);
                GetCtrlVal (panel, control, &dmm->chan->rate);
                Fmt (cmd, "%s:nplc %f[p2]", k6485_MeasFunction(dev), dmm->chan->rate*0.06);
                k6485_Out (dev, cmd);
            }
            break;
        //case K6485_DCV:
        //case K6485_ACV:
        case K6485_DCI:
        //case K6485_ACI:
        //case K6485_OHMS2:
        //case K6485_OHMS4:
            if (event == EVENT_COMMIT) {
                if (dmm->func != control) {
                    SetCtrlAttribute (panel, dmm->func, ATTR_CMD_BUTTON_COLOR,
                                      VAL_PANEL_GRAY);
                    SetCtrlAttribute (panel, control, ATTR_CMD_BUTTON_COLOR, VAL_RED);
                    dmm->func = control;
                    Fmt (cmd, "function '%s'", k6485_MeasFunction(dev));
                    k6485_Out (dev, cmd);
                }
            }
            break;
        /**case K6485_MONITOR:
            if (event == EVENT_COMMIT) {
                GetCtrlVal (panel, control, &chan);
                k6485_GetMeasurement (chan, dev);
            }
            break;*/
    }

    if (event == EVENT_COMMIT) k6485_UpdateControls(panel, dev);
    return 0;
}

static void k6485_GetReadings (acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    k6485_GetMeasurement (0, dev);
}
/*static void k6485_GetCh0Readings (acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    k6485_GetMeasurement (0, dev);
}

static void k6485_GetCh1Readings (acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    k6485_GetMeasurement (1, dev);
}

static void k6485_GetCh2Readings (acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    k6485_GetMeasurement (2, dev);
}

static void k6485_GetCh3Readings (acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    k6485_GetMeasurement (3, dev);
}

static void k6485_GetCh4Readings (acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    k6485_GetMeasurement (4, dev);
}

static void k6485_GetCh5Readings (acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    k6485_GetMeasurement (5, dev);
}

static void k6485_GetCh6Readings (acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    k6485_GetMeasurement (6, dev);
}

static void k6485_GetCh7Readings (acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    k6485_GetMeasurement (7, dev);
}

static void k6485_GetCh8Readings (acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    k6485_GetMeasurement (8, dev);
}

static void k6485_GetCh9Readings (acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    k6485_GetMeasurement (9, dev);
}

static void k6485_GetCh10Readings (acqchanPtr acqchan)
{
    gpibioPtr dev = acqchan->dev;
    k6485_GetMeasurement (10, dev);
}
*/
static double k6485_GetMeasurement (int chan, gpibioPtr dev)
{
    char cmd[256], rsp[256];
    k6485Ptr dmm = dev->device;
    double reading;
    int status = 0;
    short statusbyte;

    if (chan != k6485_GetChannel (dev)) {
        if (chan == 0) {
            k6485_Out (dev, "route:open:all");
            k6485_CheckforProblems (dev);
        } else {
            Fmt (cmd, "route:close (@%i)", chan);
            k6485_Out (dev, cmd);
        }

        Fmt (cmd, "%s:nplc %f[p2]", k6485_MeasFunction (dev), dmm->chan->rate*0.06);
        k6485_Out (dev, cmd);
        k6485_CheckforProblems(dev);

        if (dmm->chan->range == 0.0) {
            Fmt (cmd, "%s:range:auto 1", k6485_MeasFunction (dev));
            k6485_Out (dev, cmd);
        } else {
            Fmt (cmd, "%s:range:auto 0", k6485_MeasFunction (dev));
            k6485_Out (dev, cmd);
            Fmt (cmd, "%s:range:upper %f", k6485_MeasFunction (dev), dmm->chan->range);
            k6485_Out (dev, cmd);
        }
        k6485_CheckforProblems (dev);

        Fmt (cmd, "%s:average:state %i", k6485_MeasFunction (dev), dmm->chan->filter.enable);
        k6485_Out (dev, cmd);
        k6485_CheckforProblems (dev);
        if (dmm->chan->filter.enable) {
            if (dmm->chan->filter.moving) Fmt (rsp, "MOV"); else Fmt (rsp, "REP");
            Fmt (cmd, "%s:average:tcontrol %s", k6485_MeasFunction (dev), rsp);
            k6485_Out (dev, cmd);
            k6485_CheckforProblems (dev);

            Fmt (cmd, "%s:average:count %i", k6485_MeasFunction (dev), dmm->chan->filter.readings);
            k6485_Out (dev, cmd);
            k6485_CheckforProblems (dev);
        }
    }

    gpibio_Out (dev, "abort");

    k6485_GetMeasurementStatus (dev); /* clear */
    while (!(status & K6485_MSE_RAV)) {
        if (gpibio_SRQ (dev)) {
            gpibio_GetStatusByte (dev, &statusbyte);
            if (statusbyte & K6485_SRE_MSB)
                status = k6485_GetMeasurementStatus (dev);
        }
        ProcessSystemEvents();
    }
    dmm->chan->overLimit = status & K6485_MSE_ROF;
    k6485_Out (dev, "fetch?");
    k6485_In (dev, rsp);
    Scan (rsp, "%s>%f", &reading);
    dmm->chan->acqchan->reading = reading;
    dmm->chan->acqchan->newreading = TRUE;
    return reading;
}

static void k6485_UpdateControls (int panel, gpibioPtr dev)
{
    char msg[256], rsp[256];
    k6485Ptr dmm = dev->device;
    int chan, autorange, m;
    double r;

    if (expG.acqstatus != utilG.acq.status) {
        m = GetPanelMenuBar (panel);
        SetMenuBarAttribute (m, K6485MENUS_FILE_LOAD, ATTR_DIMMED, util_TakingData());
        //SetInputMode (panel, K6485_DCV, !util_TakingData());
        //SetInputMode (panel, K6485_ACV, !util_TakingData());
        SetInputMode (panel, K6485_DCI, !util_TakingData());
       // SetInputMode (panel, K6485_ACI, !util_TakingData());
       // SetInputMode (panel, K6485_OHMS2, !util_TakingData());
       // SetInputMode (panel, K6485_OHMS4, !util_TakingData());
    }

    k6485_Out (dev, "CONF:CURR:DC");
    k6485_CheckforProblems (dev);

    chan = k6485_GetChannel (dev);
   // SetCtrlVal (panel, K6485_MONITOR, chan);
   // SetInputMode (panel, K6485_MONITOR, dmm->scanCard);

    SetCtrlAttribute (panel, k6485_GetFunction (dev), ATTR_CMD_BUTTON_COLOR, VAL_RED);

    dmm->chan->rate = k6485_GetRate (dev);
    SetCtrlVal (panel, K6485_RATE, dmm->chan->rate);

    dmm->chan->filter.enable = k6485_GetFilterStatus (dev);
    SetCtrlVal (panel, K6485_FILTER_ENABLE, dmm->chan->filter.enable);
    SetInputMode (panel, K6485_FILTER_READINGS, dmm->chan->filter.enable);
    SetInputMode (panel, K6485_FILTER_TYPE, dmm->chan->filter.enable);

    if (dmm->chan->filter.enable) {
        dmm->chan->filter.moving = k6485_GetMovingFilterType (dev);
        SetCtrlVal (panel, K6485_FILTER_TYPE, dmm->chan->filter.moving);

        dmm->chan->filter.readings = k6485_GetFilterReadings (dev);
        SetCtrlVal (panel, K6485_FILTER_READINGS, dmm->chan->filter.readings);
    }

    autorange = k6485_GetAutoRange (dev);
    SetCtrlVal (panel, K6485_RANGE_AUTO, autorange);
    SetInputMode (panel, K6485_RANGE, !autorange);
    if (!autorange) {
        dmm->chan->range = k6485_GetRange(dev);
        SetCtrlVal (panel, K6485_RANGE, dmm->chan->range);
    } else dmm->chan->range = 0.0;
}

int  k6485_ControlPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2)
{
    gpibioPtr dev = callbackData;
    k6485Ptr dmm = dev->device;
    int menubar;

    if ((event == EVENT_KEYPRESS && eventData1 == VAL_ESC_VKEY) || event == EVENT_RIGHT_DOUBLE_CLICK) 
	{
    	devPanel_Remove(panel);   
		DiscardPanel (panel);
		dev->iPanel = 0;
		SetMenuBarAttribute (acquire_GetMenuBar(), dev->menuitem_id, ATTR_DIMMED, FALSE);
    }

    if (event == EVENT_GOT_FOCUS) {
        menubar = GetPanelMenuBar (panel);
        SetPanelAttribute (panel, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, K6485MENUS_FILE_SAVE, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, K6485MENUS_FILE_LOAD, ATTR_DIMMED, (dev->status != DEV_REMOTE));
        SetMenuBarAttribute (menubar, K6485MENUS_MEASURE, ATTR_DIMMED, (dev->status != DEV_REMOTE));

        if (!util_TakingData()) k6485_UpdateControls (panel, dev);
    }
    return 0;
}

static int k6485_GetMeasurementStatus (gpibioPtr dev)
{
    char rsp[256];
    int status;
    k6485_Out (dev, "status:measurement:event?");
    k6485_In (dev, rsp);
    Scan (rsp, "%s>%i", &status);
    return status;
}

static double k6485_GetRange (gpibioPtr dev)
{
    char msg[256], rsp[256];
    double range;
    Fmt (msg, "%s:range:upper?", k6485_MeasFunction (dev));
    k6485_Out (dev, msg);
    k6485_In (dev, rsp);
    k6485_CheckforProblems (dev);
    Scan (rsp, "%s>%f", &range);
    return range;
}

static int k6485_GetAutoRange (gpibioPtr dev)
{
    char msg[256], rsp[256];
    int autorange;
    Fmt (msg, "%s:range:auto?", k6485_MeasFunction (dev));
    k6485_Out (dev, msg);
    k6485_In (dev, rsp);
    k6485_CheckforProblems (dev);
    Scan (rsp, "%s>%i", &autorange);
    return autorange;
}

static int k6485_GetFilterReadings (gpibioPtr dev)
{
    char msg[256], rsp[256];
    int readings = 0;

    Fmt (msg, "%s:average:count?", k6485_MeasFunction (dev));
    k6485_Out (dev, msg);
    k6485_In (dev, rsp);
    k6485_CheckforProblems (dev);
    Scan (rsp, "%s>%i", &readings);
    return readings;
}

static int k6485_GetMovingFilterType (gpibioPtr dev)
{
    char msg[256], rsp[256];
    int moving = FALSE;
    Fmt (msg, "%s:average:tcontrol?", k6485_MeasFunction (dev));
    k6485_Out (dev, msg);
    k6485_In (dev, rsp);
    k6485_CheckforProblems (dev);
    if (CompareBytes (rsp, 0, "MOV", 0, 3, 0) == 0) moving = TRUE;
    return moving;
}

static int k6485_GetFilterStatus (gpibioPtr dev)
{
    char msg[256], rsp[256];
    int status;
    Fmt (msg, "%s:average:state?", k6485_MeasFunction (dev));
    k6485_Out (dev, msg);
    k6485_In (dev, rsp);
    k6485_CheckforProblems (dev);
    Scan (rsp, "%s>%i", &status);
    return status;
}

static double k6485_GetRate (gpibioPtr dev)
{
    char msg[256], rsp[256];
    double rate;
    Fmt (msg, "%s:nplc?", k6485_MeasFunction (dev));
    k6485_Out (dev, msg);
    k6485_In (dev, rsp);
    k6485_CheckforProblems (dev);
    Scan (rsp, "%s>%f", &rate);
    rate/=0.06;     /* convert to msec */
    return rate;
}

static int k6485_GetFunction (gpibioPtr dev)
{
    char rsp[256];
    k6485Ptr dmm = dev->device;

    k6485_Out (dev, "function?");
    k6485_In (dev, rsp);
    k6485_CheckforProblems (dev);
    dmm->func = K6485_DCI;
    //if (CompareBytes (rsp, 1, "VOLT:DC", 0, 7, 0) == 0) dmm->func = K6485_DCV;
    //else if (CompareBytes (rsp, 1, "VOLT:AC", 0, 7, 0) == 0) dmm->func = K6485_ACV;
   // if (CompareBytes (rsp, 1, "CURR:DC", 0, 7, 0) == 0) dmm->func = K6485_DCI;
    //else if (CompareBytes (rsp, 1, "CURR:AC", 0, 7, 0) == 0) dmm->func = K6485_ACI;
   // else if (CompareBytes (rsp, 1, "RES", 0, 3, 0) == 0) dmm->func = K6485_OHMS2;
   // else if (CompareBytes (rsp, 1, "FRES", 0, 4, 0) == 0) dmm->func = K6485_OHMS4;
    return dmm->func;
}

static int k6485_GetChannel (gpibioPtr dev)
{
    char rsp[256];
    int chan;
    k6485Ptr dmm = dev->device;
    /*if (dmm->scanCard) {
        k6485_Out (dev, "route:close:state?");
        k6485_In (dev, rsp);
        if (Scan (rsp, "%s>(@%i)", &chan) != 1) chan = 0;
    } else */
		chan = 0;
    return chan;
}

static char *k6485_MeasFunction (gpibioPtr dev)
{
    char *msg;
    k6485Ptr dmm = dev->device;

    msg = "                          ";
    switch (dmm->func) {
        //case K6485_DCV: Fmt (msg, "voltage:dc"); break;
        //case K6485_ACV: Fmt (msg, "voltage:ac"); break;
        case K6485_DCI: Fmt (msg, "CURR:DC"); break;
        //case K6485_ACI: Fmt (msg, "current:ac"); break;
       // case K6485_OHMS2: Fmt (msg, "resistance"); break;
       // case K6485_OHMS4: Fmt (msg, "fresistance"); break;
    }
    return msg;
}

static void k6485_Out (gpibioPtr dev, char *cmd)
{
    gpibio_Out (dev, cmd);
}

static void k6485_In (gpibioPtr dev, char *msg)
{
    gpibio_In (dev, msg);
}

static void k6485_CheckforProblems (gpibioPtr dev)
{
    char msg[256];
    short statusbyte;

    if (gpibio_SRQ(dev)) {
        gpibio_GetStatusByte (dev, &statusbyte);
        if (statusbyte & K6485_SRE_EAV) k6485_GetErrorMessage (dev);
        if (statusbyte & K6485_SRE_ESB) k6485_GetEventSummary (dev);
        if (statusbyte & K6485_SRE_QSB) k6485_GetQuestionableSummary (dev);
    }
}

static void k6485_GetOperationSummary (int panel, gpibioPtr dev)
{
    char msg[256];
    int statusbyte;
    k6485Ptr dmm = dev->device;

    statusbyte = 0;
    gpibio_Out (dev, "status:operation:event?");
    gpibio_In (dev, msg);
    Scan (msg, "%s>%i", &statusbyte);
    if (statusbyte != dmm->operation) {
        SetCtrlVal (panel, K6485_MEAS, statusbyte & K6485_OSE_MEAS);
        SetCtrlVal (panel, K6485_TRIG, statusbyte & K6485_OSE_TRIG);
        SetCtrlVal (panel, K6485_IDLE, statusbyte & K6485_OSE_IDLE);
    }
    dmm->operation = statusbyte;
}

static void k6485_GetQuestionableSummary (gpibioPtr dev)
{
    char msg[256];
    int statusbyte;

    statusbyte = 0;
    gpibio_Out (dev, "status:questionable:event?");
    gpibio_In (dev, msg);
    Scan (msg, "%s>%i", &statusbyte);
    if (statusbyte & K6485_QSE_TEMP)
        MessagePopup ("Keithley 6485 Questionable Event Message", "Invalid reference junction measurement");
    if (statusbyte & K6485_QSE_CAL)
        MessagePopup ("Keithley 6485 Questionable Event Message", "Invalid calibration constant detected during power up");
    if (statusbyte & K6485_QSE_WARN)
        MessagePopup ("Keithley 6485 Questionable Event Message", "Signal oriented measurement command ignored");
}

static void k6485_GetEventSummary (gpibioPtr dev)
{
    char msg[256];
    int statusbyte;

    statusbyte = 0;
    gpibio_Out (dev, "*ESR?");
    gpibio_In (dev, msg);
    Scan (msg, "%s>%i", &statusbyte);
    if (statusbyte & K6485_ESE_QYE)
        MessagePopup ("Keithley 6485 Event Message", "No data in output queue");
    if (statusbyte & K6485_ESE_DDE)
        MessagePopup ("Keithley 6485 Event Message", "Instrument operation error due to some internal condition");
    if (statusbyte & K6485_ESE_EXE)
        MessagePopup ("Keithley 6485 Event Message", "Command execution error");
    if (statusbyte & K6485_ESE_CME)
        MessagePopup ("Keithley 6485 Event Message", "Illegal command received");
    if (statusbyte & K6485_ESE_URQ)
        MessagePopup ("Keithley 6485 Event Message", "LOCAL key on front panel pressed");
    if (statusbyte & K6485_ESE_PON)
        MessagePopup ("Keithley 6485 Event Message", "Power cycled off and on");
}

static void k6485_GetErrorMessage (gpibioPtr dev)
{
    char msg[256];
    int statusbyte;

    gpibio_Out (dev, "system:error?");
    gpibio_In (dev, msg);
	MessagePopup ("Keithley 6485 Error Message", msg);
}

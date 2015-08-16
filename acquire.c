#include <ansi_c.h>
#include <userint.h>
#include <utility.h>
#include <formatio.h>
#include "toolbox.h"

#include "Acquireu.h"
#include "util.h"
#include "utilu.h"
#include "list.h"
#include "channel.h"
#include "changen.h"
#include "chanfnc.h"
#include "chanops.h"
#include "acqchan.h"
#include "curve.h"
#include "curveu.h"
#include "acqcrv.h"
#include "graph.h"
#include "graphu.h"
#include "curveop.h"
#include "acquire.h"
#include "source.h"

#define TRUE 1
#define FALSE 0

#define DFLT_FILE_SUFFIX "NanoSilicon"
#define DFLT_DATA_DIR "C:\\Data"

struct acquireStruct acqG;
struct expStruct expG = {NULL, NULL, NULL, 0, ACQ_DONE};

static struct datafileStruct {
    char filePath[260];
    char dataDir[MAX_PATHNAME_LEN];
    char fileSuffix[MAX_PATHNAME_LEN];
}   dataFile;

static int acqinfoP = 0;

void acquire_Init (void);
void acquire_Exit(void);
int  acquire_GetMenuBar (void);
void acquire_UpdatePanel(void);
void acquire_UpdatePanelExp (void);

int  exp_Begin (void);

void exp_UpdateGeneralPanel (void);
void exp_InitGeneralExp (void);
void exp_DoGeneralExp (void);

void acquire_DataFileMakePath (void);


int  AcqSetupCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void exp_StartDaq();



void exp_DoGeneralExp (void)
{
    acqchanlist_GetandSaveReadings();
    graphlist_PlotReadings ();
    util_Delay (expG.delay);
}

void exp_InitGeneralExp (void)
{
    int i;
    graphPtr graph;
    for (i = 0; i < graphG.graphs.nItems; i++)
    {
        graph = graphlist_GetItem (i);
        graph->acqcurve.marker.offset = 0;
        graph->acqcurve.marker.pts = utilG.acq.nPts;
    }
}

// lock/unlock floating exp n of points
void exp_UpdateGeneralPanel (void)
{
    SetInputMode (acqG.p.setup, ACQSETUP_GEN_POINTS, !util_TakingData());
}

// set experiment to floating; called from the menu experiment->floating
void GenExpCallback(int menubar, int menuItem, void *callbackData, int panel)
{
    if (utilG.exp == EXP_SOURCE) {
        utilG.exp = EXP_FLOAT;
        // shows/hides controls relevant to current experiment (float or source) 
        acquire_UpdatePanelExp();
    }

    expG.InitExp = exp_InitGeneralExp;
    expG.UpdatePanel = exp_UpdateGeneralPanel;
    expG.DoExp = exp_DoGeneralExp;
    expG.delay = 0.0;

    GetCtrlVal (panel, ACQSETUP_GEN_POINTS, &utilG.acq.nPts);
    exp_UpdateGeneralPanel(); // lock/unlock floating exp n of points 
}

// Called from the menu experiment->begin
int exp_Begin (void)
{
    int size;

    if (acqchanG.channels.nItems < 1)
    {
        MessagePopup ("Begin Experiment Failed:",
                      "Please select at least one channel for acquisition");
        return FALSE;
    }
    // Check datafile dir exists (in case it was removed after it was set
    int dirNameLen;
    if( ! FileExists(dataFile.dataDir, &dirNameLen) ) {
        MessagePopup ("Begin Experiment Failed:",
                      "Data dir does not exist! ");
        return FALSE;
    }
    if (!acqchanlist_AllocMemory()) return FALSE;
        
    // set filepath from the data dir, timestamp, suffix and extension
    double dt;
    GetCurrentDateTime(&dt); // sec from 1900, local time
    char fname[MAX_PATHNAME_LEN];
    FormatDateTimeString(dt, "%Y-%m-%d_%H%M%S", fname, MAX_PATHNAME_LEN-1);
    sprintf(dataFile.filePath,"%s\\%s_%s.daas", dataFile.dataDir, fname, dataFile.fileSuffix);
    DebugPrintf("File path: %s\n", dataFile.filePath);
    SetCtrlVal (acqG.p.setup, ACQSETUP_FILEPATH, dataFile.filePath);                      
    //updateGraphSource();

    utilG.acq.pt = 0;
    expG.InitExp();
    acqchanlist_InitDataFile(dataFile.filePath);
    exp_StartDaq();

    return TRUE;
}

void acquire_UpdateDataInfoPanel (void)
{
    double pasttime, timeperpt;
    int pts2go;

    if (acqinfoP) {
        pasttime = Timer()-acqchanG.time;
        pts2go = utilG.acq.nPts-1-utilG.acq.pt;
        timeperpt = pasttime/(utilG.acq.pt+1);
        SetCtrlVal (acqinfoP, ACQINFO_PTS2GO, pts2go);
        SetCtrlVal (acqinfoP, ACQINFO_TIMEPERPT, timeperpt);
        SetCtrlVal (acqinfoP, ACQINFO_ELAPSEDTIME, pasttime/60);
        SetCtrlVal (acqinfoP, ACQINFO_TIME2GO, (double)pts2go*timeperpt/60);
    }
}

// shows/hides controls relevant to current experiment (float or source) 
void acquire_UpdatePanelExp (void)
{
    int bTop, cTop=0, height=0, bottom;

/* Floating experiment */

    SetCtrlAttribute (acqG.p.setup, ACQSETUP_GEN_POINTS, ATTR_VISIBLE, utilG.exp == EXP_FLOAT);
    SetCtrlAttribute (acqG.p.setup, ACQSETUP_GEN_DELAY, ATTR_VISIBLE, utilG.exp == EXP_FLOAT);
    SetCtrlAttribute (acqG.p.setup, ACQSETUP_GEN_TIME, ATTR_VISIBLE, utilG.exp == EXP_FLOAT);
    SetMenuBarAttribute (acquire_GetMenuBar(), ACQMENUS_EXP_GENERAL, ATTR_CHECKED, utilG.exp == EXP_FLOAT);

/* Source experiment */

    SetCtrlAttribute (acqG.p.setup, ACQSETUP_SRC_LIST, ATTR_VISIBLE, utilG.exp == EXP_SOURCE);
    SetCtrlAttribute (acqG.p.setup, ACQSETUP_SRC_MOVEUP, ATTR_VISIBLE, utilG.exp == EXP_SOURCE);
    SetCtrlAttribute (acqG.p.setup, ACQSETUP_SRC_MOVEDOWN, ATTR_VISIBLE, utilG.exp == EXP_SOURCE);
    SetCtrlAttribute (acqG.p.setup, ACQSETUP_SRC_REMOVE, ATTR_VISIBLE, utilG.exp == EXP_SOURCE);
    SetCtrlAttribute (acqG.p.setup, ACQSETUP_SRC_POINTS, ATTR_VISIBLE, utilG.exp == EXP_SOURCE);
    SetCtrlAttribute (acqG.p.setup, ACQSETUP_SRC_TIME, ATTR_VISIBLE, utilG.exp == EXP_SOURCE);
    SetMenuBarAttribute (acquire_GetMenuBar(), ACQMENUS_EXP_SOURCE, ATTR_CHECKED, utilG.exp == EXP_SOURCE);


    switch (utilG.exp) {
        case EXP_FLOAT:
            GetCtrlAttribute (acqG.p.setup, ACQSETUP_GEN_DELAY, ATTR_TOP, &cTop);
            GetCtrlAttribute (acqG.p.setup, ACQSETUP_GEN_DELAY, ATTR_HEIGHT, &height);
            SetCtrlVal (acqG.p.setup, ACQSETUP_EXPTITLE, "Floating Experiment");
            break;
        case EXP_SOURCE:
            GetCtrlAttribute (acqG.p.setup, ACQSETUP_SRC_POINTS, ATTR_TOP, &cTop);
            GetCtrlAttribute (acqG.p.setup, ACQSETUP_SRC_POINTS, ATTR_HEIGHT, &height);
            SetCtrlVal (acqG.p.setup, ACQSETUP_EXPTITLE, "source control experiment");
            break;
    }

    GetCtrlAttribute (acqG.p.setup, ACQSETUP_BOX_1, ATTR_TOP, &bTop);
    SetCtrlAttribute (acqG.p.setup, ACQSETUP_BOX_1, ATTR_HEIGHT, cTop+height+10-bTop);
    SetPanelAttribute (acqG.p.setup, ATTR_HEIGHT, cTop+height+36);
}

void acquire_UpdatePanel(void)
{
    int menuBar;

    if (expG.UpdatePanel) expG.UpdatePanel();

    
    menuBar  = GetPanelMenuBar (acqG.p.setup);
    SetMenuBarAttribute (menuBar, ACQMENUS_EXP_GENERAL, ATTR_DIMMED, util_TakingData());
    SetMenuBarAttribute (menuBar, ACQMENUS_EXP_SOURCE, ATTR_DIMMED, util_TakingData());

    SetMenuBarAttribute (menuBar, ACQMENUS_EXP_BEGIN, ATTR_DIMMED, util_TakingData());
    SetMenuBarAttribute (menuBar, ACQMENUS_EXP_PAUSE, ATTR_DIMMED, !(utilG.acq.status == ACQ_BUSY));
    SetMenuBarAttribute (menuBar, ACQMENUS_EXP_CONTINUE, ATTR_DIMMED, !(utilG.acq.status == ACQ_PAUSED));
    SetMenuBarAttribute (menuBar, ACQMENUS_EXP_END, ATTR_DIMMED, !util_TakingData());

//    SetMenuBarAttribute (menuBar, ACQMENUS_EXP_INFO, ATTR_DIMMED, !util_TakingData());

    acqchanlist_Dimmed (util_TakingData());

    switch (utilG.acq.status) {
        case ACQ_BUSY:
            SetCtrlVal (acqG.p.setup, ACQSETUP_STATUS, "BUSY");
            break;
        case ACQ_STOPPED:
            SetCtrlVal (acqG.p.setup, ACQSETUP_STATUS, "STOPPED");
            break;
        case ACQ_DONE:
            SetCtrlVal (acqG.p.setup, ACQSETUP_STATUS, "DONE");
            break;
        case ACQ_PAUSED:
            SetCtrlVal (acqG.p.setup, ACQSETUP_STATUS, "PAUSED");
            break;
    }
}

int acquire_GetMenuBar (void)
{
    return GetPanelMenuBar (acqG.p.setup);
}

void acquire_Init (void)
{
    if (utilG.acq.status != ACQ_NONE) {
        util_ChangeInitMessage ("Acquisition Utilities...");
        util_WriteLog("acquire_Init()\nStarting...\n");
        acqG.p.setup = LoadPanel (utilG.p, "acquireu.uir", ACQSETUP);
        
        //SetPanelPos (acqG.p.setup, VAL_AUTO_CENTER, VAL_AUTO_CENTER);
        // Set default data dir and suffix
        int dirNameLen;
        if(FileExists(DFLT_DATA_DIR, &dirNameLen) ) {
            strcpy(dataFile.dataDir, DFLT_DATA_DIR);
        } else {
            char msg[MAX_PATHNAME_LEN*2];
            sprintf(msg, "Default Data Does not exist:\n%s\nUsing application dir.", DFLT_DATA_DIR);
            MessagePopup ("Acquisition Setup", msg );

            GetProjectDir (dataFile.dataDir);
        }
        SetCtrlVal (acqG.p.setup, ACQSETUP_DATADIR, dataFile.dataDir);
        strcpy(dataFile.fileSuffix, DFLT_FILE_SUFFIX);
        SetCtrlVal(acqG.p.setup, ACQSETUP_FILESUFFIX, dataFile.fileSuffix); 

        // This control is a button on startup panel, "control. acquisition, and analysis"
        InstallCtrlCallback (utilG.p, BG_ACQSETUP, AcqSetupCallback, 0);

    } else SetCtrlAttribute (utilG.p, BG_ACQSETUP, ATTR_VISIBLE, FALSE);
}

void acquire_Exit(void)
{
    if (utilG.acq.status != ACQ_NONE) {
        
        DiscardPanel (acqG.p.setup);

        if (acqinfoP) {
            
            DiscardPanel (acqinfoP);
        }
    }

    graphG_Exit();
}

void ExpStatusCallback(int menubar, int menuItem, void *callbackData, int panel)
{
    switch (menuItem)
    {
        case ACQMENUS_EXP_BEGIN:
            utilG.acq.status = ACQ_BEGIN;
            break;
        case ACQMENUS_EXP_PAUSE:
            utilG.acq.status = ACQ_PAUSED;
            break;
        case ACQMENUS_EXP_CONTINUE:
            exp_StartDaq();
            break;
        case ACQMENUS_EXP_END:
            utilG.acq.status = ACQ_TERMINATE;
            break;
    }
    acquire_UpdatePanel();
}

int  GenExpControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    if (event == EVENT_VAL_CHANGED){
        GetCtrlVal (panel, ACQSETUP_GEN_POINTS, &utilG.acq.nPts);
        GetCtrlVal (panel, ACQSETUP_GEN_DELAY, &expG.delay);
        SetCtrlVal (panel, ACQSETUP_GEN_TIME, (double)utilG.acq.nPts*expG.delay/60);
    }
    return 0;
}

void AcqInfoCallback(int menubar, int menuItem, void *callbackData, int panel)
{
    if (!acqinfoP) {
        acqinfoP = LoadPanel (utilG.p, "acquireu.uir", ACQINFO);
        if (acqinfoP < 0) acqinfoP = 0;
        else {
            SetPanelPos (acqinfoP, VAL_AUTO_CENTER, VAL_AUTO_CENTER);
            
            DisplayPanel (acqinfoP);
        }
    }
}

int  RemoveAcqInfoCallback(int panel, int event, void *callbackData, int eventData1, int eventData2)
{
    if (((event == EVENT_KEYPRESS) && (eventData1 == VAL_ESC_VKEY)) || (event == EVENT_RIGHT_DOUBLE_CLICK))
    {
        DiscardPanel (panel);
        acqinfoP = 0;
    }
    return 0;
}

int  AcqSetupCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    if (event == EVENT_COMMIT) {
        acquire_UpdatePanel();
        DisplayPanel(acqG.p.setup);
    }
    return 0;
}


int  DirSelCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    char strDir[MAX_PATHNAME_LEN];
    
    if (event != EVENT_COMMIT) return 0;

    int ret = DirSelectPopupEx("", "Select Data Directory", strDir);
    
    if( ret == VAL_DIRECTORY_SELECTED) {
        SetCtrlVal (acqG.p.setup, ACQSETUP_DATADIR, strDir);
        strcpy(dataFile.dataDir,strDir);
    }
    
    return 0;
}


int CVICALLBACK BeepCallback (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    int b;
    switch (event)
    {
        case EVENT_COMMIT:
            GetCtrlVal(panel,control, &b);
            utilG.beep = b;
        break;
    }
    return 0;
}


//void acq_TimerInit(){
//  ACQSETUP, ACQSETUP_ACQ_TIMER    
//}

//void acq_TimerStart(){
//    
//}

double acqTimerGetInterval(){
    double delay;
    int err = GetCtrlAttribute (acqG.p.setup, ACQSETUP_ACQ_TIMER, ATTR_INTERVAL, &delay);
    return delay;
}

void acqTimerReset( ){
    
    // If the timer has already started, setting ATTR_INTERVAL resets the timer.    
    int err = ResetTimer(acqG.p.setup, ACQSETUP_ACQ_TIMER);    
}


void acqTimerSetInterval( double delay_sec ){
    double curr_delay = acqTimerGetInterval();
    
    // we will set the interval only if it differs by the timer resolution ~1ms
    if( fabs(curr_delay - delay_sec) < 5e-4) return;
    
    // If the timer has already started, setting ATTR_INTERVAL resets the timer.    
    int err = SetCtrlAttribute(acqG.p.setup, ACQSETUP_ACQ_TIMER, ATTR_INTERVAL, delay_sec);    
}

extern void exp_StartDaq(){
    // first point will be collected in 10ms
    acqTimerReset();
    acqTimerSetInterval(0.01); 
    utilG.acq.status = ACQ_BUSY;
}


int acquire_Point(){
    expG.DoExp();
    acquire_UpdateDataInfoPanel();
    util_IncAcqPt();
    return 0;
}


int CVICALLBACK acq_timerCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2){
    
    if( utilG.acq.status == ACQ_BUSY ){
      acquire_Point();   
    }
    if (utilG.acq.pt == utilG.acq.nPts)
        utilG.acq.status = ACQ_TERMINATE;
   
    
    return 0;
}

int CVICALLBACK DataFileSuffixControlCallback (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        char strSuff[MAX_PATHNAME_LEN];
        case EVENT_COMMIT:
            GetCtrlVal(acqG.p.setup, ACQSETUP_FILESUFFIX, strSuff);
            // validate the filename for valid chars
            char invChars[] = {'<', '>', ':', '"', '/', '\\', '|', '?', '*'};  
            int nChars = 9;
            void * chP;
            char ch;
            
            for( int i = 0; i<nChars; i++){
                ch = invChars[i];
                chP = strchr(strSuff, ch);

                if ( chP ) {  
                    MessagePopup("Invalid filename character in the file suffix",
                                      "Do not to use <>:\"/\\\|?* characters");
                    // reverse to previous val
                    SetCtrlVal(acqG.p.setup, ACQSETUP_FILESUFFIX, dataFile.fileSuffix);
                    return 0;
                }
            }
            // store validated suffix
            strcpy(dataFile.fileSuffix, strSuff);

            break;
    }
    return 0;
}

/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  BK9201                           1       /* callback function: BK9201MainPanelCallback */
#define  BK9201_OPERA                     2       /* control type: toggle, callback function: BK9201ControlCallback */
#define  BK9201_SOURCE                    3       /* control type: numeric, callback function: BK9201SourceCallback */
#define  BK9201_V_LIMIT                   4       /* control type: numeric, callback function: BK9201ControlCallback */
#define  BK9201_I_LIMIT                   5       /* control type: numeric, callback function: BK9201ControlCallback */
#define  BK9201_OVERLIMIT                 6       /* control type: LED, callback function: (none) */
#define  BK9201_MEASURE                   7       /* control type: numeric, callback function: (none) */
#define  BK9201_SOURCETEXT_2              8       /* control type: textMsg, callback function: (none) */
#define  BK9201_SOURCETEXT                9       /* control type: textMsg, callback function: (none) */
#define  BK9201_MEASURETEXT               10      /* control type: textMsg, callback function: (none) */
#define  BK9201_TEXT_1                    11      /* control type: textMsg, callback function: (none) */
#define  BK9201_DECORATION                12      /* control type: deco, callback function: (none) */
#define  BK9201_IDENTIFY                  13      /* control type: command, callback function: BK9201ControlCallback */
#define  BK9201_CLEARBRAINS               14      /* control type: command, callback function: BK9201ControlCallback */

#define  BK9201_MEA                       2
#define  BK9201_MEA_LABEL                 2       /* control type: string, callback function: BK9201MeasControlCallback */
#define  BK9201_MEA_COEFF                 3       /* control type: numeric, callback function: BK9201MeasControlCallback */
#define  BK9201_MEA_MEAS                  4       /* control type: numeric, callback function: (none) */
#define  BK9201_MEA_ACQ                   5       /* control type: toggle, callback function: BK9201MeasControlCallback */
#define  BK9201_MEA_NOTE                  6       /* control type: toggle, callback function: AcqDataNoteCallback */
#define  BK9201_MEA_CLOSE                 7       /* control type: toggle, callback function: K2400MeasControlCallback */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  BK9201MENU                       1
#define  BK9201MENU_FILE                  2
#define  BK9201MENU_FILE_LOAD             3       /* callback function: LoadBK9201Callback */
#define  BK9201MENU_FILE_SAVE             4       /* callback function: SaveBK9201Callback */
#define  BK9201MENU_FILE_SEP_1            5
#define  BK9201MENU_FILE_GPIB             6       /* callback function: OperateDevCallback */
#define  BK9201MENU_SOURCE                7       /* callback function: BK9201SourceSetupCallback */
#define  BK9201MENU_MEASURE               8       /* callback function: BK9201MeasureSetupCallback */


     /* Callback Prototypes: */

int  CVICALLBACK AcqDataNoteCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK BK9201ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK BK9201MainPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK BK9201MeasControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK BK9201MeasureSetupCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK BK9201SourceCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK BK9201SourceSetupCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK K2400MeasControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK LoadBK9201Callback(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK OperateDevCallback(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SaveBK9201Callback(int menubar, int menuItem, void *callbackData, int panel);


#ifdef __cplusplus
    }
#endif

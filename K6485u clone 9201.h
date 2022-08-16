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

#define  K6485                            1       /* callback function: K6485MainPanelCallback */
#define  K6485_OPERA                      2       /* control type: toggle, callback function: BK9201ControlCallback */
#define  K6485_OVERLIMIT                  3       /* control type: LED, callback function: (none) */
#define  K6485_MEASURE                    4       /* control type: numeric, callback function: (none) */
#define  K6485_MEASURETEXT                5       /* control type: textMsg, callback function: (none) */

#define  K6485_MEA                        2
#define  K6485_MEA_LABEL                  2       /* control type: string, callback function: BK9201MeasControlCallback */
#define  K6485_MEA_COEFF                  3       /* control type: numeric, callback function: BK9201MeasControlCallback */
#define  K6485_MEA_MEAS                   4       /* control type: numeric, callback function: (none) */
#define  K6485_MEA_ACQ                    5       /* control type: toggle, callback function: BK9201MeasControlCallback */
#define  K6485_MEA_NOTE                   6       /* control type: toggle, callback function: AcqDataNoteCallback */
#define  K6485_MEA_CLOSE                  7       /* control type: toggle, callback function: K2400MeasControlCallback */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  K6485MENU                        1
#define  K6485MENU_FILE                   2
#define  K6485MENU_FILE_LOAD              3       /* callback function: LoadBK9201Callback */
#define  K6485MENU_FILE_SAVE              4       /* callback function: SaveBK9201Callback */
#define  K6485MENU_FILE_SEP_1             5
#define  K6485MENU_FILE_GPIB              6       /* callback function: OperateDevCallback */
#define  K6485MENU_MEASURE                7       /* callback function: BK9201MeasureSetupCallback */


     /* Callback Prototypes: */

int  CVICALLBACK AcqDataNoteCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK BK9201ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK BK9201MeasControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK BK9201MeasureSetupCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK K2400MeasControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK K6485MainPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK LoadBK9201Callback(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK OperateDevCallback(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SaveBK9201Callback(int menubar, int menuItem, void *callbackData, int panel);


#ifdef __cplusplus
    }
#endif
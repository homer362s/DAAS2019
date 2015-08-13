/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2013. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  K2400                            1       /* callback function: K2400ControlPanelCallback */
#define  K2400_IDENTIFY                   2       /* control type: toggle, callback function: K2400ControlCallback */
#define  K2400_OPERATE                    3       /* control type: toggle, callback function: K2400ControlCallback */
#define  K2400_SELECT                     4       /* control type: binary, callback function: K2400ControlCallback */
#define  K2400_SENSE                      5       /* control type: binary, callback function: K2400ControlCallback */
#define  K2400_SOURCE                     6       /* control type: numeric, callback function: K2400SourceCallback */
#define  K2400_SOURCE_I_RANGE             7       /* control type: ring, callback function: K2400ControlCallback */
#define  K2400_SOURCE_V_RANGE             8       /* control type: ring, callback function: K2400ControlCallback */
#define  K2400_DELAY                      9       /* control type: toggle, callback function: K2400ControlCallback */
#define  K2400_INTTIME                    10      /* control type: numeric, callback function: K2400ControlCallback */
#define  K2400_FILTER                     11      /* control type: numeric, callback function: K2400ControlCallback */
#define  K2400_LEVEL                      12      /* control type: numeric, callback function: K2400ControlCallback */
#define  K2400_MEAS_I_RANGE               13      /* control type: ring, callback function: K2400ControlCallback */
#define  K2400_MEAS_V_RANGE               14      /* control type: ring, callback function: K2400ControlCallback */
#define  K2400_OVERLIMIT                  15      /* control type: LED, callback function: (none) */
#define  K2400_MEASURE                    16      /* control type: numeric, callback function: K2400MeasureCallback */
#define  K2400_SOURCETEXT                 17      /* control type: textMsg, callback function: (none) */
#define  K2400_MEASURETEXT                18      /* control type: textMsg, callback function: (none) */
#define  K2400_TEXT_1                     19      /* control type: textMsg, callback function: (none) */
#define  K2400_DECORATION                 20      /* control type: deco, callback function: (none) */

#define  K2400_MEAS                       2
#define  K2400_MEAS_LABEL                 2       /* control type: string, callback function: K2400MeasControlCallback */
#define  K2400_MEAS_COEFF                 3       /* control type: numeric, callback function: K2400MeasControlCallback */
#define  K2400_MEAS_MEAS                  4       /* control type: numeric, callback function: (none) */
#define  K2400_MEAS_ACQ                   5       /* control type: toggle, callback function: K2400MeasControlCallback */
#define  K2400_MEAS_NOTE                  6       /* control type: toggle, callback function: AcqDataNoteCallback */
#define  K2400_MEAS_CLOSE                 7       /* control type: toggle, callback function: K2400MeasControlCallback */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  K2400MENUS                       1
#define  K2400MENUS_FILE                  2
#define  K2400MENUS_FILE_LOAD             3       /* callback function: LoadK2400Callback */
#define  K2400MENUS_FILE_SAVE             4       /* callback function: SaveK2400Callback */
#define  K2400MENUS_FILE_SEP_1            5
#define  K2400MENUS_FILE_GPIB             6       /* callback function: OperateDevCallback */
#define  K2400MENUS_SOURCE                7       /* callback function: K2400SourceSetupCallback */
#define  K2400MENUS_MEASURE               8       /* callback function: K2400MeasureSetupCallback */


     /* Callback Prototypes: */

int  CVICALLBACK AcqDataNoteCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK K2400ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK K2400ControlPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK K2400MeasControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK K2400MeasureCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK K2400MeasureSetupCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK K2400SourceCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK K2400SourceSetupCallback(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK LoadK2400Callback(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK OperateDevCallback(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SaveK2400Callback(int menubar, int menuItem, void *callbackData, int panel);


#ifdef __cplusplus
    }
#endif

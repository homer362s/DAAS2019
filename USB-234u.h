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

#define  DAS_CTRL                         1       /* callback function: USB234_PanelCallback */
#define  DAS_CTRL_ANALOGUE_IN             2       /* control type: numeric, callback function: (none) */
#define  DAS_CTRL_INPUT_NUM               3       /* control type: ring, callback function: USB234_ControlCallback */
#define  DAS_CTRL_RANGE                   4       /* control type: ring, callback function: USB234_ControlCallback */

#define  MEASURE                          2       /* callback function: util_HidePanelCallback */
#define  MEASURE_LABEL                    2       /* control type: string, callback function: USB234_MeasureControlCallback */
#define  MEASURE_AVERAGE                  3       /* control type: numeric, callback function: USB234_MeasureControlCallback */
#define  MEASURE_RATE                     4       /* control type: numeric, callback function: USB234_MeasureControlCallback */
#define  MEASURE_COEFF                    5       /* control type: numeric, callback function: USB234_MeasureControlCallback */
#define  MEASURE_RANGE                    6       /* control type: ring, callback function: USB234_MeasureControlCallback */
#define  MEASURE_NOTE                     7       /* control type: toggle, callback function: AcqDataNoteCallback */
#define  MEASURE_ACQ                      8       /* control type: toggle, callback function: USB234_MeasureControlCallback */

#define  PANEL_HELL                       3
#define  PANEL_HELL_RANGE                 2       /* control type: ring, callback function: das6036_ControlCallback */
#define  PANEL_HELL_ACQ                   3       /* control type: toggle, callback function: USB234_ControlCallback */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  DASMENU                          1
#define  DASMENU_FILE                     2
#define  DASMENU_FILE_SAVE                3       /* callback function: USB234_MenuCallback */
#define  DASMENU_FILE_LOAD                4       /* callback function: USB234_MenuCallback */
#define  DASMENU_SOURCE                   5
#define  DASMENU_SOURCE_DAC1              6       /* callback function: USB234_MenuCallback */
#define  DASMENU_SOURCE_DAC2              7       /* callback function: USB234_MenuCallback */
#define  DASMENU_SOURCE_DAC3              8       /* callback function: USB234_MenuCallback */
#define  DASMENU_MEAS                     9
#define  DASMENU_MEAS_IN_0                10      /* callback function: USB234_MenuCallback */
#define  DASMENU_MEAS_IN_1                11      /* callback function: USB234_MenuCallback */
#define  DASMENU_MEAS_IN_2                12      /* callback function: USB234_MenuCallback */
#define  DASMENU_MEAS_IN_3                13      /* callback function: USB234_MenuCallback */
#define  DASMENU_MEAS_IN_4                14      /* callback function: USB234_MenuCallback */
#define  DASMENU_MEAS_IN_5                15      /* callback function: USB234_MenuCallback */
#define  DASMENU_MEAS_IN_6                16      /* callback function: USB234_MenuCallback */
#define  DASMENU_MEAS_IN_7                17      /* callback function: USB234_MenuCallback */


     /* Callback Prototypes: */

int  CVICALLBACK AcqDataNoteCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK das6036_ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK USB234_ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK USB234_MeasureControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK USB234_MenuCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK USB234_PanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK util_HidePanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

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

#define  K6485                            1       /* callback function: k6485_ControlPanelCallback */
#define  K6485_MEASURE                    2       /* control type: numeric, callback function: (none) */
#define  K6485_RATE                       3       /* control type: numeric, callback function: k6485_ControlCallback */
#define  K6485_RANGE                      4       /* control type: numeric, callback function: k6485_ControlCallback */
#define  K6485_RANGE_AUTO                 5       /* control type: radioButton, callback function: k6485_ControlCallback */
#define  K6485_FILTER_ENABLE              6       /* control type: radioButton, callback function: k2000_ControlCallback */
#define  K6485_FILTER_TYPE                7       /* control type: binary, callback function: k2000_ControlCallback */
#define  K6485_FILTER_READINGS            8       /* control type: numeric, callback function: k6485_ControlCallback */
#define  K6485_MEAS                       9       /* control type: LED, callback function: (none) */
#define  K6485_TRIG                       10      /* control type: LED, callback function: (none) */
#define  K6485_IDLE                       11      /* control type: LED, callback function: (none) */
#define  K6485_OVER                       12      /* control type: LED, callback function: (none) */
#define  K6485_RANGE_TEXT                 13      /* control type: textMsg, callback function: (none) */
#define  K6485_INT_TEXT                   14      /* control type: textMsg, callback function: (none) */
#define  K6485_DECORATION                 15      /* control type: deco, callback function: (none) */
#define  K6485_DCI                        16      /* control type: command, callback function: k6485_ControlCallback */
#define  K6485_DECORATION_2               17      /* control type: deco, callback function: (none) */

#define  K6485_CHAN                       2
#define  K6485_CHAN_LABEL                 2       /* control type: string, callback function: k6485_ChannelControlCallback */
#define  K6485_CHAN_COEFF                 3       /* control type: numeric, callback function: k6485_ChannelControlCallback */
#define  K6485_CHAN_NOTE                  4       /* control type: toggle, callback function: AcqDataNoteCallback */
#define  K6485_CHAN_ACQ                   5       /* control type: toggle, callback function: k6485_ChannelControlCallback */
#define  K6485_CHAN_CLOSE                 6       /* control type: toggle, callback function: k6485_ChannelControlCallback */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  K6485MENUS                       1
#define  K6485MENUS_FILE                  2
#define  K6485MENUS_FILE_LOAD             3       /* callback function: k2000_LoadCallback */
#define  K6485MENUS_FILE_SAVE             4       /* callback function: k2000_SaveCallback */
#define  K6485MENUS_FILE_SEP_1            5
#define  K6485MENUS_FILE_GPIB             6       /* callback function: OperateDevCallback */
#define  K6485MENUS_MEASURE               7
#define  K6485MENUS_MEASURE_DMM           8       /* callback function: k6485_ChannelSetupCallback */


     /* Callback Prototypes: */

int  CVICALLBACK AcqDataNoteCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK k2000_ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK k2000_LoadCallback(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK k2000_SaveCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK k6485_ChannelControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK k6485_ChannelSetupCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK k6485_ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK k6485_ControlPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK OperateDevCallback(int menubar, int menuItem, void *callbackData, int panel);


#ifdef __cplusplus
    }
#endif
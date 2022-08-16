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

#define  K6485_CTRL                       1       /* callback function: K6485PanelCallback */
#define  K6485_CTRL_SENSOR_A_READ         2       /* control type: numeric, callback function: (none) */
#define  K6485_CTRL_DECORATION_3          3       /* control type: deco, callback function: (none) */

#define  K6485_SENS                       2
#define  K6485_SENS_NOTE_1                2       /* control type: toggle, callback function: K6485SensorControlCallback */
#define  K6485_SENS_SENSOR_A_LABEL        3       /* control type: string, callback function: K6485SensorControlCallback */
#define  K6485_SENS_SENSOR_A_COEFF        4       /* control type: numeric, callback function: K6485SensorControlCallback */
#define  K6485_SENS_SENSOR_A_MEAS         5       /* control type: numeric, callback function: (none) */
#define  K6485_SENS_SENSOR_A_ACQ          6       /* control type: toggle, callback function: K6485SensorControlCallback */
#define  K6485_SENS_CLOSE                 7       /* control type: toggle, callback function: K6485SensorControlCallback */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  K6485                            1
#define  K6485_SETUP                      2
#define  K6485_SETUP_LOAD                 3       /* callback function: K6485menuCallback */
#define  K6485_MEASURE                    4
#define  K6485_MEASURE_MEAS               5       /* callback function: K6485menuCallback */


     /* Callback Prototypes: */

void CVICALLBACK K6485menuCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK K6485PanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK K6485SensorControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
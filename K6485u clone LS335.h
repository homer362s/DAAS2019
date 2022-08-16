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

#define  LS335_CTRL                       1       /* callback function: LS335PanelCallback */
#define  LS335_CTRL_SENSOR_B_READ         2       /* control type: numeric, callback function: (none) */
#define  LS335_CTRL_SENSOR_A_READ         3       /* control type: numeric, callback function: (none) */
#define  LS335_CTRL_HEATER_PROP           4       /* control type: command, callback function: LS335ControlCallback */
#define  LS335_CTRL_RAMPSPEED             5       /* control type: numeric, callback function: LS335ControlCallback */
#define  LS335_CTRL_SENSOR_A_TSET         6       /* control type: numeric, callback function: LS335ControlCallback */
#define  LS335_CTRL_POWER                 7       /* control type: ring, callback function: LS335ControlCallback */
#define  LS335_CTRL_HEATER                8       /* control type: binary, callback function: LS335ControlCallback */
#define  LS335_CTRL_DECORATION_3          9       /* control type: deco, callback function: (none) */
#define  LS335_CTRL_TEXTMSG_3             10      /* control type: textMsg, callback function: (none) */
#define  LS335_CTRL_DECORATION            11      /* control type: deco, callback function: (none) */
#define  LS335_CTRL_TEXTMSG               12      /* control type: textMsg, callback function: (none) */

#define  LS335_HEAT                       2       /* callback function: util_HidePanelCallback */
#define  LS335_HEAT_D                     2       /* control type: numeric, callback function: (none) */
#define  LS335_HEAT_I                     3       /* control type: numeric, callback function: (none) */
#define  LS335_HEAT_P                     4       /* control type: numeric, callback function: (none) */
#define  LS335_HEAT_NCHANGE               5       /* control type: numeric, callback function: (none) */
#define  LS335_HEAT_PCHANGE               6       /* control type: numeric, callback function: (none) */
#define  LS335_HEAT_SETPLIM               7       /* control type: numeric, callback function: (none) */
#define  LS335_HEAT_LOOPNUM               8       /* control type: numeric, callback function: (none) */
#define  LS335_HEAT_RESET                 9       /* control type: command, callback function: LS335HeatControlCallback */
#define  LS335_HEAT_ACCEPT                10      /* control type: command, callback function: LS335HeatControlCallback */
#define  LS335_HEAT_INPUTNM               11      /* control type: ring, callback function: (none) */
#define  LS335_HEAT_POWERUP               12      /* control type: ring, callback function: (none) */
#define  LS335_HEAT_UNITS                 13      /* control type: ring, callback function: (none) */
#define  LS335_HEAT_CURRENT               14      /* control type: ring, callback function: (none) */
#define  LS335_HEAT_MXPOWER               15      /* control type: ring, callback function: (none) */
#define  LS335_HEAT_DON                   16      /* control type: radioButton, callback function: LS335HeatControlCallback */
#define  LS335_HEAT_ION                   17      /* control type: radioButton, callback function: LS335HeatControlCallback */
#define  LS335_HEAT_PON                   18      /* control type: radioButton, callback function: LS335HeatControlCallback */

#define  LS335_SENS                       3
#define  LS335_SENS_SENSOR_B_LABEL        2       /* control type: string, callback function: LS335SensorControlCallback */
#define  LS335_SENS_NOTE_2                3       /* control type: toggle, callback function: LS335SensorControlCallback */
#define  LS335_SENS_SENSOR_B_COEFF        4       /* control type: numeric, callback function: LS335SensorControlCallback */
#define  LS335_SENS_SENSOR_B_MEAS         5       /* control type: numeric, callback function: (none) */
#define  LS335_SENS_SENSOR_B_ACQ          6       /* control type: toggle, callback function: LS335SensorControlCallback */
#define  LS335_SENS_NOTE_1                7       /* control type: toggle, callback function: LS335SensorControlCallback */
#define  LS335_SENS_SENSOR_A_LABEL        8       /* control type: string, callback function: LS335SensorControlCallback */
#define  LS335_SENS_SENSOR_A_COEFF        9       /* control type: numeric, callback function: LS335SensorControlCallback */
#define  LS335_SENS_SENSOR_A_MEAS         10      /* control type: numeric, callback function: (none) */
#define  LS335_SENS_SENSOR_A_ACQ          11      /* control type: toggle, callback function: LS335SensorControlCallback */
#define  LS335_SENS_CLOSE                 12      /* control type: toggle, callback function: LS335SensorControlCallback */
#define  LS335_SENS_XTEXT                 13      /* control type: textMsg, callback function: (none) */
#define  LS335_SENS_YTEXT                 14      /* control type: textMsg, callback function: (none) */

#define  LS335CURVE                       4       /* callback function: util_HidePanelCallback */
#define  LS335CURVE_SERIAL                2       /* control type: string, callback function: (none) */
#define  LS335CURVE_CURVESRC              3       /* control type: numeric, callback function: (none) */
#define  LS335CURVE_CURVENUM              4       /* control type: numeric, callback function: (none) */
#define  LS335CURVE_ACCEPT                5       /* control type: command, callback function: LS335SendCurve */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  LS335                            1
#define  LS335_CURVES                     2
#define  LS335_CURVES_LOAD                3       /* callback function: LS335menuCallback */
#define  LS335_SOURCE                     4
#define  LS335_SOURCE_HEATER              5       /* callback function: LS335menuCallback */
#define  LS335_MEASURE                    6
#define  LS335_MEASURE_MEAS               7       /* callback function: LS335menuCallback */


     /* Callback Prototypes: */

int  CVICALLBACK LS335ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LS335HeatControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK LS335menuCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK LS335PanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LS335SendCurve(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LS335SensorControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK util_HidePanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
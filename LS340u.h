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

#define  LS340_CTRL                       1       /* callback function: LS340ControlPanelCallback */
#define  LS340_CTRL_SEN_D_READ            2       /* control type: numeric, callback function: (none) */
#define  LS340_CTRL_HE3PREAD              3       /* control type: numeric, callback function: (none) */
#define  LS340_CTRL_KPOTREAD              4       /* control type: numeric, callback function: (none) */
#define  LS340_CTRL_SORBREAD              5       /* control type: numeric, callback function: (none) */
#define  LS340_CTRL_HEAT_PROP             6       /* control type: command, callback function: LS340ControlCallback */
#define  LS340_CTRL_RAMPSPEED             7       /* control type: numeric, callback function: LS340ControlCallback */
#define  LS340_CTRL_SORBTSET              8       /* control type: numeric, callback function: LS340ControlCallback */
#define  LS340_CTRL_POWER                 9       /* control type: ring, callback function: LS340ControlCallback */
#define  LS340_CTRL_HEATER                10      /* control type: binary, callback function: LS340ControlCallback */
#define  LS340_CTRL_DECORATION_3          11      /* control type: deco, callback function: (none) */
#define  LS340_CTRL_TEXTMSG_3             12      /* control type: textMsg, callback function: (none) */
#define  LS340_CTRL_DECORATION            13      /* control type: deco, callback function: (none) */
#define  LS340_CTRL_TEXTMSG               14      /* control type: textMsg, callback function: (none) */

#define  LS340_HEAT                       2       /* callback function: util_HidePanelCallback */
#define  LS340_HEAT_D                     2       /* control type: numeric, callback function: (none) */
#define  LS340_HEAT_I                     3       /* control type: numeric, callback function: (none) */
#define  LS340_HEAT_P                     4       /* control type: numeric, callback function: (none) */
#define  LS340_HEAT_NCHANGE               5       /* control type: numeric, callback function: (none) */
#define  LS340_HEAT_PCHANGE               6       /* control type: numeric, callback function: (none) */
#define  LS340_HEAT_SETPLIM               7       /* control type: numeric, callback function: (none) */
#define  LS340_HEAT_LOOPNUM               8       /* control type: numeric, callback function: (none) */
#define  LS340_HEAT_RESET                 9       /* control type: command, callback function: LS340HeatControlCallback */
#define  LS340_HEAT_ACCEPT                10      /* control type: command, callback function: LS340HeatControlCallback */
#define  LS340_HEAT_INPUTNM               11      /* control type: ring, callback function: (none) */
#define  LS340_HEAT_POWERUP               12      /* control type: ring, callback function: (none) */
#define  LS340_HEAT_UNITS                 13      /* control type: ring, callback function: (none) */
#define  LS340_HEAT_CURRENT               14      /* control type: ring, callback function: (none) */
#define  LS340_HEAT_MXPOWER               15      /* control type: ring, callback function: (none) */
#define  LS340_HEAT_DON                   16      /* control type: radioButton, callback function: LS340HeatControlCallback */
#define  LS340_HEAT_ION                   17      /* control type: radioButton, callback function: LS340HeatControlCallback */
#define  LS340_HEAT_PON                   18      /* control type: radioButton, callback function: LS340HeatControlCallback */

#define  LS340_SENS                       3
#define  LS340_SENS_SEN_D_LABEL           2       /* control type: string, callback function: LS340SensorControlCallback */
#define  LS340_SENS_HE3PLABEL             3       /* control type: string, callback function: LS340SensorControlCallback */
#define  LS340_SENS_NOTE_4                4       /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_NOTE_3                5       /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_SEN_D_ACQ             6       /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_HE3PACQ               7       /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_KPOTLABEL             8       /* control type: string, callback function: LS340SensorControlCallback */
#define  LS340_SENS_NOTE_2                9       /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_KPOTACQ               10      /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_NOTE_1                11      /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_SORBLABEL             12      /* control type: string, callback function: LS340SensorControlCallback */
#define  LS340_SENS_SORBACQ               13      /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_CLOSE                 14      /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_SEN_D_MEAS            15      /* control type: numeric, callback function: (none) */
#define  LS340_SENS_HE3PMEAS              16      /* control type: numeric, callback function: (none) */
#define  LS340_SENS_KPOTMEAS              17      /* control type: numeric, callback function: (none) */
#define  LS340_SENS_SORBMEAS              18      /* control type: numeric, callback function: (none) */
#define  LS340_SENS_XTEXT                 19      /* control type: textMsg, callback function: (none) */
#define  LS340_SENS_YTEXT                 20      /* control type: textMsg, callback function: (none) */
#define  LS340_SENS_RTEXT_2               21      /* control type: textMsg, callback function: (none) */
#define  LS340_SENS_RTEXT                 22      /* control type: textMsg, callback function: (none) */

#define  LS340CURVE                       4       /* callback function: util_HidePanelCallback */
#define  LS340CURVE_SERIAL                2       /* control type: string, callback function: (none) */
#define  LS340CURVE_CURVESRC              3       /* control type: numeric, callback function: (none) */
#define  LS340CURVE_CURVENUM              4       /* control type: numeric, callback function: (none) */
#define  LS340CURVE_ACCEPT                5       /* control type: command, callback function: LS340SendCurve */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  LS340MENU                        1
#define  LS340MENU_CURVES                 2
#define  LS340MENU_CURVES_LOAD            3       /* callback function: LS340menuCallback */
#define  LS340MENU_SOURCE                 4
#define  LS340MENU_SOURCE_HEATER          5       /* callback function: LS340menuCallback */
#define  LS340MENU_MEASURE                6
#define  LS340MENU_MEASURE_SENSOR_SETTINGS 7      /* callback function: LS340menuCallback */


     /* Callback Prototypes: */

int  CVICALLBACK LS340ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LS340ControlPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LS340HeatControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK LS340menuCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK LS340SendCurve(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LS340SensorControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK util_HidePanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

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

#define  LS340_CTRL                       1       /* callback function: LS340PanelCallback */
#define  LS340_CTRL_HE3PREAD              2       /* control type: numeric, callback function: (none) */
#define  LS340_CTRL_KPOTREAD              3       /* control type: numeric, callback function: (none) */
#define  LS340_CTRL_SORBREAD              4       /* control type: numeric, callback function: (none) */
#define  LS340_CTRL_HEAT_PROP             5       /* control type: command, callback function: LS340ControlCallback */
#define  LS340_CTRL_ALARMLVL              6       /* control type: numeric, callback function: LS340ControlCallback */
#define  LS340_CTRL_RAMPSPEED             7       /* control type: numeric, callback function: LS340ControlCallback */
#define  LS340_CTRL_SORBTSET              8       /* control type: numeric, callback function: LS340ControlCallback */
#define  LS340_CTRL_POWER                 9       /* control type: ring, callback function: LS340ControlCallback */
#define  LS340_CTRL_ALARM                 10      /* control type: binary, callback function: LS340ControlCallback */
#define  LS340_CTRL_HEATER                11      /* control type: binary, callback function: LS340ControlCallback */
#define  LS340_CTRL_ALARMLED              12      /* control type: LED, callback function: (none) */
#define  LS340_CTRL_DECORATION_2          13      /* control type: deco, callback function: (none) */
#define  LS340_CTRL_DECORATION_3          14      /* control type: deco, callback function: (none) */
#define  LS340_CTRL_TEXTMSG_3             15      /* control type: textMsg, callback function: (none) */
#define  LS340_CTRL_DECORATION            16      /* control type: deco, callback function: (none) */
#define  LS340_CTRL_TEXTMSG               17      /* control type: textMsg, callback function: (none) */
#define  LS340_CTRL_TEXTMSG_2             18      /* control type: textMsg, callback function: (none) */

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

#define  LS340_SENS                       3       /* callback function: util_HidePanelCallback */
#define  LS340_SENS_HE3PLABEL             2       /* control type: string, callback function: LS340SensorControlCallback */
#define  LS340_SENS_HE3PCOEFF             3       /* control type: numeric, callback function: LS340SensorControlCallback */
#define  LS340_SENS_NOTE_3                4       /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_HE3PMEAS              5       /* control type: numeric, callback function: (none) */
#define  LS340_SENS_HE3PACQ               6       /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_KPOTLABEL             7       /* control type: string, callback function: LS340SensorControlCallback */
#define  LS340_SENS_NOTE_2                8       /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_KPOTCOEFF             9       /* control type: numeric, callback function: LS340SensorControlCallback */
#define  LS340_SENS_KPOTMEAS              10      /* control type: numeric, callback function: (none) */
#define  LS340_SENS_KPOTACQ               11      /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_NOTE_1                12      /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_SORBLABEL             13      /* control type: string, callback function: LS340SensorControlCallback */
#define  LS340_SENS_SORBCOEFF             14      /* control type: numeric, callback function: LS340SensorControlCallback */
#define  LS340_SENS_SORBMEAS              15      /* control type: numeric, callback function: (none) */
#define  LS340_SENS_SORBACQ               16      /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_CLOSE                 17      /* control type: toggle, callback function: LS340SensorControlCallback */
#define  LS340_SENS_XTEXT                 18      /* control type: textMsg, callback function: (none) */
#define  LS340_SENS_YTEXT                 19      /* control type: textMsg, callback function: (none) */
#define  LS340_SENS_RTEXT                 20      /* control type: textMsg, callback function: (none) */

#define  LS340CURVE                       4       /* callback function: util_HidePanelCallback */
#define  LS340CURVE_SERIAL                2       /* control type: string, callback function: (none) */
#define  LS340CURVE_CURVESRC              3       /* control type: numeric, callback function: (none) */
#define  LS340CURVE_CURVENUM              4       /* control type: numeric, callback function: (none) */
#define  LS340CURVE_ACCEPT                5       /* control type: command, callback function: LS340SendCurve */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  LS340                            1
#define  LS340_CURVES                     2
#define  LS340_CURVES_LOAD                3       /* callback function: LS340menuCallack */
#define  LS340_SOURCE                     4
#define  LS340_SOURCE_HEATER              5       /* callback function: LS340menuCallack */
#define  LS340_MEASURE                    6
#define  LS340_MEASURE_MEAS               7       /* callback function: LS340menuCallack */


     /* Callback Prototypes: */

int  CVICALLBACK LS340ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LS340HeatControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK LS340menuCallack(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK LS340PanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LS340SendCurve(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LS340SensorControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK util_HidePanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

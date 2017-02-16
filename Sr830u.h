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

#define  SR830_ADC                        1
#define  SR830_ADC_LABEL_4                2       /* control type: string, callback function: ADCControlCallback */
#define  SR830_ADC_COEFF_4                3       /* control type: numeric, callback function: ADCControlCallback */
#define  SR830_ADC_MEAS_4                 4       /* control type: numeric, callback function: (none) */
#define  SR830_ADC_NOTE_4                 5       /* control type: toggle, callback function: ADCControlCallback */
#define  SR830_ADC_ACQ_4                  6       /* control type: toggle, callback function: ADCControlCallback */
#define  SR830_ADC_LABEL_3                7       /* control type: string, callback function: ADCControlCallback */
#define  SR830_ADC_COEFF_3                8       /* control type: numeric, callback function: ADCControlCallback */
#define  SR830_ADC_NOTE_3                 9       /* control type: toggle, callback function: ADCControlCallback */
#define  SR830_ADC_MEAS_3                 10      /* control type: numeric, callback function: (none) */
#define  SR830_ADC_ACQ_3                  11      /* control type: toggle, callback function: ADCControlCallback */
#define  SR830_ADC_LABEL_2                12      /* control type: string, callback function: ADCControlCallback */
#define  SR830_ADC_NOTE_2                 13      /* control type: toggle, callback function: ADCControlCallback */
#define  SR830_ADC_COEFF_2                14      /* control type: numeric, callback function: ADCControlCallback */
#define  SR830_ADC_MEAS_2                 15      /* control type: numeric, callback function: (none) */
#define  SR830_ADC_ACQ_2                  16      /* control type: toggle, callback function: ADCControlCallback */
#define  SR830_ADC_NOTE_1                 17      /* control type: toggle, callback function: ADCControlCallback */
#define  SR830_ADC_LABEL_1                18      /* control type: string, callback function: ADCControlCallback */
#define  SR830_ADC_COEFF_1                19      /* control type: numeric, callback function: ADCControlCallback */
#define  SR830_ADC_MEAS_1                 20      /* control type: numeric, callback function: (none) */
#define  SR830_ADC_ACQ_1                  21      /* control type: toggle, callback function: ADCControlCallback */
#define  SR830_ADC_CLOSE                  22      /* control type: toggle, callback function: ADCControlCallback */
#define  SR830_ADC_CONVERSION_4           23      /* control type: ring, callback function: ADCControlCallback */
#define  SR830_ADC_CONVERSION_3           24      /* control type: ring, callback function: ADCControlCallback */
#define  SR830_ADC_CONVERSION_2           25      /* control type: ring, callback function: ADCControlCallback */
#define  SR830_ADC_CONVERSION_1           26      /* control type: ring, callback function: ADCControlCallback */
#define  SR830_ADC_TEXT_1                 27      /* control type: textMsg, callback function: (none) */
#define  SR830_ADC_TEXT_2                 28      /* control type: textMsg, callback function: (none) */
#define  SR830_ADC_TEXT_3                 29      /* control type: textMsg, callback function: (none) */
#define  SR830_ADC_TEXT_4                 30      /* control type: textMsg, callback function: (none) */

#define  SR830_CTRL                       2       /* callback function: SR830ControlPanelCallback */
#define  SR830_CTRL_REF                   2       /* control type: command, callback function: OperateReferenceCallback */
#define  SR830_CTRL_TC                    3       /* control type: ring, callback function: SR830ControlCallback */
#define  SR830_CTRL_AUTOSENSE             4       /* control type: toggle, callback function: SR830ControlCallback */
#define  SR830_CTRL_SENS                  5       /* control type: ring, callback function: SR830ControlCallback */
#define  SR830_CTRL_XDISP                 6       /* control type: numeric, callback function: (none) */
#define  SR830_CTRL_YDISP                 7       /* control type: numeric, callback function: (none) */
#define  SR830_CTRL_FILTSLOPE             8       /* control type: ring, callback function: SR830ControlCallback */
#define  SR830_CTRL_SYNC                  9       /* control type: toggle, callback function: SR830ControlCallback */
#define  SR830_CTRL_DYNRES                10      /* control type: ring, callback function: SR830ControlCallback */
#define  SR830_CTRL_REJECT                11      /* control type: ring, callback function: SR830ControlCallback */
#define  SR830_CTRL_INPUTOVERLOAD         12      /* control type: LED, callback function: (none) */
#define  SR830_CTRL_FILTEROVERLOAD        13      /* control type: LED, callback function: (none) */
#define  SR830_CTRL_OUTPUTOVERLOAD        14      /* control type: LED, callback function: (none) */
#define  SR830_CTRL_MAG                   15      /* control type: numeric, callback function: (none) */
#define  SR830_CTRL_PHASE                 16      /* control type: scale, callback function: (none) */
#define  SR830_CTRL_HARMON                17      /* control type: toggle, callback function: SR830ControlCallback */
#define  SR830_CTRL_HARMVAL               18      /* control type: numeric, callback function: SR830ControlCallback */

#define  SR830_DAC                        3
#define  SR830_DAC_LABEL_4                2       /* control type: string, callback function: DACControlCallback */
#define  SR830_DAC_COEFF_4                3       /* control type: numeric, callback function: DACControlCallback */
#define  SR830_DAC_MEAS_4                 4       /* control type: numeric, callback function: (none) */
#define  SR830_DAC_NOTE_4                 5       /* control type: toggle, callback function: DACControlCallback */
#define  SR830_DAC_ACQ_4                  6       /* control type: toggle, callback function: DACControlCallback */
#define  SR830_DAC_LABEL_3                7       /* control type: string, callback function: DACControlCallback */
#define  SR830_DAC_COEFF_3                8       /* control type: numeric, callback function: DACControlCallback */
#define  SR830_DAC_NOTE_3                 9       /* control type: toggle, callback function: DACControlCallback */
#define  SR830_DAC_MEAS_3                 10      /* control type: numeric, callback function: (none) */
#define  SR830_DAC_ACQ_3                  11      /* control type: toggle, callback function: DACControlCallback */
#define  SR830_DAC_LABEL_2                12      /* control type: string, callback function: DACControlCallback */
#define  SR830_DAC_NOTE_2                 13      /* control type: toggle, callback function: DACControlCallback */
#define  SR830_DAC_COEFF_2                14      /* control type: numeric, callback function: DACControlCallback */
#define  SR830_DAC_MEAS_2                 15      /* control type: numeric, callback function: (none) */
#define  SR830_DAC_ACQ_2                  16      /* control type: toggle, callback function: DACControlCallback */
#define  SR830_DAC_NOTE_1                 17      /* control type: toggle, callback function: DACControlCallback */
#define  SR830_DAC_LABEL_1                18      /* control type: string, callback function: DACControlCallback */
#define  SR830_DAC_COEFF_1                19      /* control type: numeric, callback function: DACControlCallback */
#define  SR830_DAC_MEAS_1                 20      /* control type: numeric, callback function: (none) */
#define  SR830_DAC_ACQ_1                  21      /* control type: toggle, callback function: DACControlCallback */
#define  SR830_DAC_CLOSE                  22      /* control type: toggle, callback function: DACControlCallback */
#define  SR830_DAC_TEXT_1                 23      /* control type: textMsg, callback function: (none) */
#define  SR830_DAC_TEXT_2                 24      /* control type: textMsg, callback function: (none) */
#define  SR830_DAC_TEXT_3                 25      /* control type: textMsg, callback function: (none) */
#define  SR830_DAC_TEXT_4                 26      /* control type: textMsg, callback function: (none) */

#define  SR830_REF                        4
#define  SR830_REF_PHASE                  2       /* control type: scale, callback function: SR830RefCallback */
#define  SR830_REF_FREQ                   3       /* control type: scale, callback function: SR830RefCallback */
#define  SR830_REF_AMPLITUDE              4       /* control type: scale, callback function: SR830RefCallback */
#define  SR830_REF_CLOSE                  5       /* control type: toggle, callback function: util_DiscardCallback */

#define  SR830_XYMP                       5
#define  SR830_XYMP_YNLABEL               2       /* control type: string, callback function: XYMPControlCallback */
#define  SR830_XYMP_XNLABEL               3       /* control type: string, callback function: XYMPControlCallback */
#define  SR830_XYMP_PLABEL                4       /* control type: string, callback function: XYMPControlCallback */
#define  SR830_XYMP_YNCOEFF               5       /* control type: numeric, callback function: XYMPControlCallback */
#define  SR830_XYMP_YNMEAS                6       /* control type: numeric, callback function: (none) */
#define  SR830_XYMP_PCOEFF                7       /* control type: numeric, callback function: XYMPControlCallback */
#define  SR830_XYMP_PMEAS                 8       /* control type: numeric, callback function: (none) */
#define  SR830_XYMP_YN_NOTE               9       /* control type: toggle, callback function: XYMPControlCallback */
#define  SR830_XYMP_NOTE_4                10      /* control type: toggle, callback function: XYMPControlCallback */
#define  SR830_XYMP_YNACQ                 11      /* control type: toggle, callback function: XYMPControlCallback */
#define  SR830_XYMP_XNCOEFF               12      /* control type: numeric, callback function: XYMPControlCallback */
#define  SR830_XYMP_PACQ                  13      /* control type: toggle, callback function: XYMPControlCallback */
#define  SR830_XYMP_XNMEAS                14      /* control type: numeric, callback function: (none) */
#define  SR830_XYMP_XN_NOTE               15      /* control type: toggle, callback function: XYMPControlCallback */
#define  SR830_XYMP_MLABEL                16      /* control type: string, callback function: XYMPControlCallback */
#define  SR830_XYMP_XNACQ                 17      /* control type: toggle, callback function: XYMPControlCallback */
#define  SR830_XYMP_MCOEFF                18      /* control type: numeric, callback function: XYMPControlCallback */
#define  SR830_XYMP_NOTE_3                19      /* control type: toggle, callback function: XYMPControlCallback */
#define  SR830_XYMP_MMEAS                 20      /* control type: numeric, callback function: (none) */
#define  SR830_XYMP_MACQ                  21      /* control type: toggle, callback function: XYMPControlCallback */
#define  SR830_XYMP_YLABEL                22      /* control type: string, callback function: XYMPControlCallback */
#define  SR830_XYMP_NOTE_2                23      /* control type: toggle, callback function: XYMPControlCallback */
#define  SR830_XYMP_YCOEFF                24      /* control type: numeric, callback function: XYMPControlCallback */
#define  SR830_XYMP_YMEAS                 25      /* control type: numeric, callback function: (none) */
#define  SR830_XYMP_YACQ                  26      /* control type: toggle, callback function: XYMPControlCallback */
#define  SR830_XYMP_NOTE_1                27      /* control type: toggle, callback function: XYMPControlCallback */
#define  SR830_XYMP_XLABEL                28      /* control type: string, callback function: XYMPControlCallback */
#define  SR830_XYMP_XCOEFF                29      /* control type: numeric, callback function: XYMPControlCallback */
#define  SR830_XYMP_XMEAS                 30      /* control type: numeric, callback function: (none) */
#define  SR830_XYMP_XACQ                  31      /* control type: toggle, callback function: XYMPControlCallback */
#define  SR830_XYMP_CLOSE                 32      /* control type: toggle, callback function: XYMPControlCallback */
#define  SR830_XYMP_YTEXT                 33      /* control type: textMsg, callback function: (none) */
#define  SR830_XYMP_MTEXT                 34      /* control type: textMsg, callback function: (none) */
#define  SR830_XYMP_XNTEXT                35      /* control type: textMsg, callback function: (none) */
#define  SR830_XYMP_PTEXT                 36      /* control type: textMsg, callback function: (none) */
#define  SR830_XYMP_XTEXT                 37      /* control type: textMsg, callback function: (none) */
#define  SR830_XYMP_YNTEXT                38      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  SR830MENU                        1
#define  SR830MENU_FILE                   2
#define  SR830MENU_FILE_SAVE              3       /* callback function: SaveSR830SetupCallback */
#define  SR830MENU_FILE_LOAD              4       /* callback function: LoadSR830SetupCallback */
#define  SR830MENU_FILE_SEP_1             5
#define  SR830MENU_FILE_GPIB              6       /* callback function: OperateDevCallback */
#define  SR830MENU_SOURCES                7
#define  SR830MENU_SOURCES_DAC1           8       /* callback function: sr830_MenuCallback */
#define  SR830MENU_SOURCES_DAC2           9       /* callback function: sr830_MenuCallback */
#define  SR830MENU_SOURCES_DAC3           10      /* callback function: sr830_MenuCallback */
#define  SR830MENU_SOURCES_DAC4           11      /* callback function: sr830_MenuCallback */
#define  SR830MENU_SOURCES_FREQ           12      /* callback function: sr830_MenuCallback */
#define  SR830MENU_SOURCES_AMPL           13      /* callback function: sr830_MenuCallback */
#define  SR830MENU_SOURCES_PHAS           14      /* callback function: sr830_MenuCallback */
#define  SR830MENU_MEASURE                15
#define  SR830MENU_MEASURE_LIA            16      /* callback function: MeasXYMPCallback */
#define  SR830MENU_MEASURE_DACS           17      /* callback function: MeasDACCallback */
#define  SR830MENU_MEASURE_ADCS           18      /* callback function: MeasADCCallback */


     /* Callback Prototypes: */

int  CVICALLBACK ADCControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DACControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK LoadSR830SetupCallback(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MeasADCCallback(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MeasDACCallback(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MeasXYMPCallback(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK OperateDevCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK OperateReferenceCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK SaveSR830SetupCallback(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK sr830_MenuCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK SR830ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SR830ControlPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SR830RefCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK util_DiscardCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK XYMPControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

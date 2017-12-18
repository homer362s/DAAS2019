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

#define  AFG3252                          1       /* callback function: afg3252PanelCallback */
#define  AFG3252_FREQ2                    2       /* control type: numeric, callback function: (none) */
#define  AFG3252_FREQ1                    3       /* control type: numeric, callback function: (none) */
#define  AFG3252_AMPL2                    4       /* control type: numeric, callback function: (none) */
#define  AFG3252_AMPL1                    5       /* control type: numeric, callback function: (none) */
#define  AFG3252_WAVE2                    6       /* control type: ring, callback function: afg3252ControlCallback */
#define  AFG3252_WAVE1                    7       /* control type: ring, callback function: afg3252ControlCallback */
#define  AFG3252_OFFSET2                  8       /* control type: numeric, callback function: afg3252ControlCallback */
#define  AFG3252_OFFSET1                  9       /* control type: numeric, callback function: afg3252ControlCallback */
#define  AFG3252_GPIB                     10      /* control type: numeric, callback function: (none) */
#define  AFG3252_CHAN1                    11      /* control type: textMsg, callback function: (none) */
#define  AFG3252_SPLITTER                 12      /* control type: splitter, callback function: (none) */
#define  AFG3252_SPLITTER_2               13      /* control type: splitter, callback function: (none) */
#define  AFG3252_CHAN2                    14      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  AFG325MENU                       1
#define  AFG325MENU_SOURCES               2
#define  AFG325MENU_SOURCES_CHAN1         3
#define  AFG325MENU_SOURCES_CHAN1_SUBMENU 4
#define  AFG325MENU_SOURCES_CHAN1_AMPL1   5       /* callback function: afg3252MenuCallback */
#define  AFG325MENU_SOURCES_CHAN1_FREQ1   6       /* callback function: afg3252MenuCallback */
#define  AFG325MENU_SOURCES_CHAN2         7
#define  AFG325MENU_SOURCES_CHAN2_SUBMENU 8
#define  AFG325MENU_SOURCES_CHAN2_AMPL2   9       /* callback function: afg3252MenuCallback */
#define  AFG325MENU_SOURCES_CHAN2_FREQ2   10      /* callback function: afg3252MenuCallback */


     /* Callback Prototypes: */

int  CVICALLBACK afg3252ControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK afg3252MenuCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK afg3252PanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

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

#define  HP3245A                           1       /* callback function: hp3245aPanelCallback */
#define  HP3245A_FREQ                      2       /* control type: numeric, callback function: (none) */
#define  HP3245A_AMPL                      3       /* control type: numeric, callback function: (none) */
#define  HP3245A_WAVE                      4       /* control type: ring, callback function: hp3245aControlCallback */
#define  HP3245A_GPIB                      5       /* control type: numeric, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  HP3245MENU                       1
#define  HP3245MENU_SOURCES               2
#define  HP3245MENU_SOURCES_AMPL          3       /* callback function: hp3245aMenuCallback */
#define  HP3245MENU_SOURCES_FREQ          4       /* callback function: hp3245aMenuCallback */


     /* Callback Prototypes: */

int  CVICALLBACK hp3245aControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK hp3245aMenuCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK hp3245aPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

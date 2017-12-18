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

#define  HP33120A                         1       /* callback function: hp33120aPanelCallback */
#define  HP33120A_FREQ                    2       /* control type: numeric, callback function: (none) */
#define  HP33120A_AMPL                    3       /* control type: numeric, callback function: (none) */
#define  HP33120A_WAVE                    4       /* control type: ring, callback function: hp33120aControlCallback */
#define  HP33120A_DUTY                    5       /* control type: numeric, callback function: hp33120aControlCallback */
#define  HP33120A_GPIB                    6       /* control type: numeric, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  HP3312MENU                       1
#define  HP3312MENU_SOURCES               2
#define  HP3312MENU_SOURCES_AMPL          3       /* callback function: hp33120aMenuCallback */
#define  HP3312MENU_SOURCES_FREQ          4       /* callback function: hp33120aMenuCallback */


     /* Callback Prototypes: */

int  CVICALLBACK hp33120aControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK hp33120aMenuCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK hp33120aPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

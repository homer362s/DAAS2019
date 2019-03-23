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

#define  HP8657B                          1       /* callback function: hp8657bPanelCallback */
#define  HP8657B_FREQ                     2       /* control type: numeric, callback function: (none) */
#define  HP8657B_AMPL                     3       /* control type: numeric, callback function: (none) */
#define  HP8657B_GPIB                     4       /* control type: numeric, callback function: (none) */
#define  HP8657B_FREQUNITRING             5       /* control type: ring, callback function: (none) */
#define  HP8657B_AMPUNITRING              6       /* control type: ring, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  HP3245MENU                       1
#define  HP3245MENU_SOURCES               2
#define  HP3245MENU_SOURCES_AMPL          3       /* callback function: hp3245aMenuCallback */
#define  HP3245MENU_SOURCES_FREQ          4       /* callback function: hp3245aMenuCallback */


     /* Callback Prototypes: */

void CVICALLBACK hp3245aMenuCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK hp8657bPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

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

#define  ADDDEV                           1
#define  ADDDEV_ADDRS                     2       /* control type: ring, callback function: (none) */
#define  ADDDEV_CLOSE                     3       /* control type: toggle, callback function: util_DiscardCallback */
#define  ADDDEV_SCAN                      4       /* control type: command, callback function: AddDeviceControlCallback */
#define  ADDDEV_CONNECT                   5       /* control type: command, callback function: AddDeviceControlCallback */
#define  ADDDEV_TEXT_1                    6       /* control type: textMsg, callback function: (none) */
#define  ADDDEV_LIST                      7       /* control type: ring, callback function: AddDeviceControlCallback */
#define  ADDDEV_LABEL                     8       /* control type: string, callback function: (none) */

#define  DEV                              2
#define  DEV_LABEL                        2       /* control type: string, callback function: DevControlCallback */
#define  DEV_STATUS                       3       /* control type: string, callback function: (none) */
#define  DEV_PADDR                        4       /* control type: numeric, callback function: (none) */
#define  DEV_SADDR                        5       /* control type: numeric, callback function: (none) */
#define  DEV_COMMAND                      6       /* control type: string, callback function: DevControlCallback */
#define  DEV_LOGIO                        7       /* control type: toggle, callback function: DevControlCallback */
#define  DEV_DISABLE                      8       /* control type: toggle, callback function: DevControlCallback */
#define  DEV_CLEAR                        9       /* control type: command, callback function: DevControlCallback */
#define  DEV_CLOSE                        10      /* control type: toggle, callback function: DevControlCallback */
#define  DEV_RESPONSES                    11      /* control type: textBox, callback function: (none) */
#define  DEV_COMMANDS                     12      /* control type: textBox, callback function: (none) */
#define  DEV_TEXT1                        13      /* control type: textMsg, callback function: (none) */
#define  DEV_DECORATION                   14      /* control type: deco, callback function: (none) */

#define  IFC                              3
#define  IFC_TIMEOUT                      2       /* control type: ring, callback function: ifcControlCallback */
#define  IFC_STATUS                       3       /* control type: string, callback function: (none) */
#define  IFC_DISABLE                      4       /* control type: toggle, callback function: ifcControlCallback */
#define  IFC_LOGIO                        5       /* control type: toggle, callback function: ifcControlCallback */
#define  IFC_PADDR                        6       /* control type: ring, callback function: (none) */
#define  IFC_SADDR                        7       /* control type: ring, callback function: (none) */
#define  IFC_ENABLESADDR                  8       /* control type: toggle, callback function: ifcControlCallback */
#define  IFC_COMMAND                      9       /* control type: string, callback function: ifcControlCallback */
#define  IFC_CLOSE                        10      /* control type: toggle, callback function: ifcControlCallback */
#define  IFC_RESPONSES                    11      /* control type: textBox, callback function: (none) */
#define  IFC_COMMANDS                     12      /* control type: textBox, callback function: (none) */
#define  IFC_TEXT1                        13      /* control type: textMsg, callback function: (none) */
#define  IFC_DECORATION                   14      /* control type: deco, callback function: (none) */

#define  SETUP                            4       /* callback function: SetupPanelCallback */
#define  SETUP_ADD                        2       /* control type: command, callback function: AddDeviceCallback */
#define  SETUP_SAVE                       3       /* control type: command, callback function: SaveDeviceSetupCallback */
#define  SETUP_LOAD                       4       /* control type: command, callback function: LoadDeviceSetupCallback */
#define  SETUP_LIST                       5       /* control type: listBox, callback function: DevListCallback */
#define  SETUP_TEXT                       6       /* control type: textMsg, callback function: (none) */
#define  SETUP_TEXT_4                     7       /* control type: textMsg, callback function: (none) */
#define  SETUP_TEXT_5                     8       /* control type: textMsg, callback function: (none) */
#define  SETUP_TEXT_3                     9       /* control type: textMsg, callback function: (none) */
#define  SETUP_TEXT_2                     10      /* control type: textMsg, callback function: (none) */
#define  SETUP_TEXT4                      11      /* control type: textMsg, callback function: (none) */
#define  SETUP_DECORATION                 12      /* control type: deco, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK AddDeviceCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK AddDeviceControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DevControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DevListCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ifcControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LoadDeviceSetupCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SaveDeviceSetupCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetupPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK util_DiscardCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

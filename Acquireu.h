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

#define  ACQINFO                          1       /* callback function: RemoveAcqInfoCallback */
#define  ACQINFO_PTS2GO                   2       /* control type: numeric, callback function: (none) */
#define  ACQINFO_TIMEPERPT                3       /* control type: numeric, callback function: (none) */
#define  ACQINFO_TIME2GO                  4       /* control type: numeric, callback function: (none) */
#define  ACQINFO_ELAPSEDTIME              5       /* control type: numeric, callback function: (none) */

#define  ACQSETUP                         2       /* callback function: util_HidePanelCallback */
#define  ACQSETUP_STATUS                  2       /* control type: string, callback function: (none) */
#define  ACQSETUP_FILESUFFIX              3       /* control type: string, callback function: DataFileSuffixControlCallback */
#define  ACQSETUP_FILEPATH                4       /* control type: string, callback function: (none) */
#define  ACQSETUP_DATADIR                 5       /* control type: string, callback function: (none) */
#define  ACQSETUP_GEN_POINTS              6       /* control type: numeric, callback function: GenExpControlCallback */
#define  ACQSETUP_GEN_DELAY               7       /* control type: numeric, callback function: GenExpControlCallback */
#define  ACQSETUP_GEN_TIME                8       /* control type: numeric, callback function: (none) */
#define  ACQSETUP_SRC_LIST                9       /* control type: listBox, callback function: (none) */
#define  ACQSETUP_SRC_POINTS              10      /* control type: numeric, callback function: (none) */
#define  ACQSETUP_SRC_TIME                11      /* control type: numeric, callback function: (none) */
#define  ACQSETUP_BEEP                    12      /* control type: textButton, callback function: BeepCallback */
#define  ACQSETUP_SRC_MOVEUP              13      /* control type: command, callback function: (none) */
#define  ACQSETUP_SRC_MOVEDOWN            14      /* control type: command, callback function: (none) */
#define  ACQSETUP_SRC_REMOVE              15      /* control type: command, callback function: (none) */
#define  ACQSETUP_DIRSEL                  16      /* control type: command, callback function: DirSelCallback */
#define  ACQSETUP_DECORATION              17      /* control type: deco, callback function: (none) */
#define  ACQSETUP_ACQ_TIMER               18      /* control type: timer, callback function: acq_timerCallback */
#define  ACQSETUP_EXPTITLE                19      /* control type: textMsg, callback function: (none) */
#define  ACQSETUP_BOX_1                   20      /* control type: deco, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  ACQMENUS                         1
#define  ACQMENUS_EXP                     2
#define  ACQMENUS_EXP_SOURCE              3
#define  ACQMENUS_EXP_GENERAL             4       /* callback function: GenExpCallback */
#define  ACQMENUS_EXP_SEP_1               5
#define  ACQMENUS_EXP_BEGIN               6       /* callback function: ExpStatusCallback */
#define  ACQMENUS_EXP_PAUSE               7       /* callback function: ExpStatusCallback */
#define  ACQMENUS_EXP_CONTINUE            8       /* callback function: ExpStatusCallback */
#define  ACQMENUS_EXP_END                 9       /* callback function: ExpStatusCallback */
#define  ACQMENUS_EXP_SEP_2               10
#define  ACQMENUS_EXP_INFO                11      /* callback function: AcqInfoCallback */
#define  ACQMENUS_GPIB                    12
#define  ACQMENUS_GPIB_SEP_1              13
#define  ACQMENUS_GPIB_SETUP              14
#define  ACQMENUS_GPIB_SETUP_SUBMENU      15
#define  ACQMENUS_GPIB_SETUP_SETUP        16
#define  ACQMENUS_GPIB_SETUP_STATUS       17
#define  ACQMENUS_GPIB_SEPARATOR          18
#define  ACQMENUS_GPIB_RS232SETUP         19
#define  ACQMENUS_GPIB_SEP_2              20
#define  ACQMENUS_GPIB_PCIDEV             21
#define  ACQMENUS_GPIB_PCIDEV_SUBMENU     22
#define  ACQMENUS_GPIB_PCIDEV_PCI_SEP     23
#define  ACQMENUS_GPIB_SEP_3              24


     /* Callback Prototypes: */

int  CVICALLBACK acq_timerCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK AcqInfoCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK BeepCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DataFileSuffixControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DirSelCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK ExpStatusCallback(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK GenExpCallback(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK GenExpControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RemoveAcqInfoCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK util_HidePanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

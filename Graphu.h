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

#define  AXIS                             1       /* callback function: util_HidePanelCallback */
#define  AXIS_LABEL                       2       /* control type: string, callback function: AxisControlCallback */
#define  AXIS_MIN                         3       /* control type: numeric, callback function: AxisControlCallback */
#define  AXIS_MAX                         4       /* control type: numeric, callback function: AxisControlCallback */
#define  AXIS_GRID                        5       /* control type: toggle, callback function: AxisControlCallback */
#define  AXIS_DIVISIONS                   6       /* control type: numeric, callback function: AxisControlCallback */
#define  AXIS_AUTOSCALE                   7       /* control type: toggle, callback function: AxisControlCallback */
#define  AXIS_AUTODIV                     8       /* control type: toggle, callback function: AxisControlCallback */
#define  AXIS_SELECTION                   9       /* control type: binary, callback function: AxisControlCallback */
#define  AXIS_CLOSE                       10      /* control type: toggle, callback function: util_DiscardCallback */
#define  AXIS_SCALE                       11      /* control type: binary, callback function: AxisControlCallback */
#define  AXIS_CONV                        12      /* control type: ring, callback function: AxisControlCallback */
#define  AXIS_DECORATION                  13      /* control type: deco, callback function: (none) */

#define  GRAPH                            2       /* callback function: GraphPanelCallback */
#define  GRAPH_GRAPH                      2       /* control type: graph, callback function: GraphCallback */
#define  GRAPH_ZOOMOUT                    3       /* control type: command, callback function: ZoomOutGraphCallback */
#define  GRAPH_ZOOMIN                     4       /* control type: command, callback function: ZoomInGraphCallback */
#define  GRAPH_ACQCURVE                   5       /* control type: command, callback function: AcquireCurveCallback */
#define  GRAPH_HOME                       6       /* control type: command, callback function: HomeGraphCallback */
#define  GRAPH_DOWN                       7       /* control type: command, callback function: MoveGraphDownCallback */
#define  GRAPH_UP                         8       /* control type: command, callback function: MoveGraphUpCallback */
#define  GRAPH_RIGHT                      9       /* control type: command, callback function: MoveGraphRightCallback */
#define  GRAPH_LEFT                       10      /* control type: command, callback function: MoveGraphLeftCallback */
#define  GRAPH_CURSOR                     11      /* control type: ring, callback function: SelectCursorCallback */
#define  GRAPH_CLOSE                      12      /* control type: toggle, callback function: PanelClose */

#define  GRAPHS                           3       /* callback function: util_HidePanelCallback */
#define  GRAPHS_LIST                      2       /* control type: listBox, callback function: SelectGraphCallback */
#define  GRAPHS_PRINT                     3       /* control type: command, callback function: PrintGraphCallback */
#define  GRAPHS_SAVE                      4       /* control type: command, callback function: SaveGraphCallback */
#define  GRAPHS_LOAD                      5       /* control type: command, callback function: LoadGraphCallback */
#define  GRAPHS_REMOVE                    6       /* control type: command, callback function: RemoveGraphCallback */
#define  GRAPHS_CREATE                    7       /* control type: command, callback function: CreateGraphCallback */
#define  GRAPHS_CLOSE                     8       /* control type: toggle, callback function: PanelClose */

#define  REN_GRAPH                        4       /* callback function: RenGraphEntCallback */
#define  REN_GRAPH_CMD_OK                 2       /* control type: command, callback function: RenGraphCallBackOk */
#define  REN_GRAPH_CMD_CANCEL             3       /* control type: command, callback function: RenGraphCallBackCancel */
#define  REN_GRAPH_STRING                 4       /* control type: string, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK AcquireCurveCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK AxisControlCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CreateGraphCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK GraphCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK GraphPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK HomeGraphCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LoadGraphCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MoveGraphDownCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MoveGraphLeftCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MoveGraphRightCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MoveGraphUpCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PanelClose(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PrintGraphCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RemoveGraphCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RenGraphCallBackCancel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RenGraphCallBackOk(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RenGraphEntCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SaveGraphCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SelectCursorCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SelectGraphCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK util_DiscardCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK util_HidePanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ZoomInGraphCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ZoomOutGraphCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif

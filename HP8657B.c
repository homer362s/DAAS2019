/*

#include <ansi_c.h>
#include <formatio.h>
#include "hp4156.h"

#include "util.h"
#include "list.h"
#include "channel.h"
#include "changen.h"
#include "chanfnc.h"
#include "chanops.h"
#include "acqchan.h"
#include "acqchanu.h"
#include "curve.h"
#include "acqcrv.h"
#include "graph.h"
#include "graphu.h"
#include "curveop.h"
#include "acquire.h"
#include "acquireu.h"
#include "gpibio.h"
#include "gpibiou.h"
#include "source.h"
#include "sourceu.h"

void hp8657b_Init(void)
{
    devTypePtr devType;
    if(utilG.acq.status != ACQ_NONE)
    {
        util_ChangeInitMessage("HP8657B control utilities...");
        devType = malloc(sizeof(devTypeItem));
        if(devType)
        {
            Fmt(devType->label, "HP 8657B RF Signal Generator");
            Fmt(devType->id, HP8657B_ID);
            devType->CreateDevice = HP8657B_Create;
            devType->InitDevice = HP8657B_InitGPIB;
            devType->OperateDevice = HP8657B_OperateDevice;
            devType->UpdateReadings = HP8657B_UpdateReadings;
            devType->SaveDevice = HP8657B_Save;
            devType->LoadDevice = HP8657B_Load;
            devType->RemoveDevice = HP8657B_Remove;
            devTypeList_AddItem(devType);
        }
    }
}


void *HP8657B_Create(gpibioPtr dev)
{
    HP8657BPtr hp;
    hp = malloc(sizeof(HP8657BType));
    if (dev){ dev->device = hp; hp->id = dev->id;}
    
    hp->sources[0] = source_Create("HP 8657B Freq", dev, HP8657B_SetFreq, HP8657B_GetFreq);
	hp->sources[1] = source_Create("HP 8657B Ampl", dev, HP8657B_SetAmp, HP8657B_GetAmp);
	
    return hp;
}

*/

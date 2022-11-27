#ifndef __DIVERMODE_H__
#define __DIVERMODE_H__

#include "main.h"
#include "DiverUIState.h"

class DiverUI;

struct DiverMode
{
    virtual void OnInit(){};
    virtual void OnActivate(DiverUIState& state) = 0;
    virtual void OnOddField(DiverUIState& state) = 0;
    virtual void OnInterruptFrameStart(DiverUIState& state) = 0;
    virtual void OnInterruptHSync(DiverUIState& state) = 0;
};

#endif
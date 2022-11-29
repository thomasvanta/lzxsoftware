#ifndef __DIVERBANKBASE_H__
#define __DIVERBANKBASE_H__

#include "DiverUIState.h"

struct DiverBankBase
{
    virtual void Init(){};
    virtual void OnActivate(DiverUIState& state) = 0;
    virtual void OnOddField(DiverUIState& state) = 0;
    virtual void OnInterruptFrameStart(DiverUIState& state) = 0;
    virtual void OnInterruptHSync(DiverUIState& state) = 0;
};

#endif
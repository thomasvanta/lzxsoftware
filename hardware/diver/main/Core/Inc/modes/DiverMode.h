#include "main.h"

void GenerateLUT(uint8_t waveshape);

class DiverUI;

struct DiverMode
{
    void Init(DiverUI* _ui) 
    { 
        ui = _ui;
    };

    virtual void OnActivate() = 0;
    virtual void OnOddField() = 0;
    virtual void OnInterruptFrameStart() = 0;
    virtual void OnInterruptHSync() = 0;

protected:
    DiverUI* ui;
};


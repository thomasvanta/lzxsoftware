#include "stm32f4xx_hal.h"

#ifndef __bank_H
#define __bank_H
#ifdef __cplusplus
extern "C"
{
#endif

    class bank
    {
    private:
        /* data */
    public:
        bank(/* args */);
        ~bank();
        uint32_t generateStaticLUT();
        uint32_t generateRealTimeLUT();
    };

#ifdef __cplusplus
}
#endif
#endif /*__ perlin_H */
#include "bank.h"

//idea for an OOP approach to Diver
//a Bank is an object
//a NUM_BANKS (main.h) number of banks get initialized at region SETUP of main.cpp
//in the same region banks are initializated
//bank objects have functions with a 'State' struct as parameter, containing all info needed (buttons, sliders....)
//each bank implements generateStaticLUT and optionally generateRealTimeLUT functions
//
// main would be something like this:

// main{
//  SETUP
//  Buttons_Poll()
//  switch bank
//   case 1: banks[0].generateStaticLUT
//   ...
//   default:
//  DISPLAYFEEDBACK
//  STATES FOR FREEZE/SCROLL/INVERT...
//  pass states to Struct
//  H Loop (for...hres){
//      switch bank
//          case 1: banks[0].generateRealTimeLUT
//      ...
//      default:
//  V Loop (for...hres){
//      switch bank
//          case 1: banks[0].generateRealTimeLUT
//      ...
//      default:
//
// display_refresh()
//}


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

bank::bank(/* args */)
{
}

bank::~bank()
{
}

uint32_t bank::generateStaticLUT(/* state Struct */)
{
}

uint32_t bank::generateRealTimeLUT(/* state Struct */)
{
}

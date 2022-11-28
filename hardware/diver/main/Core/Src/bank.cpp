#include "bank.h"

//idea for an OOP approach to Diver
//a Bank is an object
//different people can create files with collections of objects inherited from bank
//for example: lzx_banks.h (7 original banks), dewb_banks.h (some extra banks)
//a NUM_BANKS (main.h) number of banks get initialized at region SETUP of main.cpp
//in the same region banks are initializated and assigned to a ... n array maybe?
//bank objects have functions with a 'State' struct as parameter, containing all info needed (buttons, sliders....)
//each bank implements generateStaticLUT and optionally generateRealTimeLUT functions
//
// main would be something like this:

// main{
//  SETUP 
//    banks[0] = lzx::bankRamp();
//    banks[1] = dewb::bankNoise()
//    .....
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

bank::bank(/* args */)
{
    //args for example initial SEED for randomness...
}

bank::~bank()
{
}

uint32_t bank::generateStaticLUT(/* state Struct */)
{
    return 1;
}

uint32_t bank::generateRealTimeLUT(/* state Struct */)
{
    return 0;
}

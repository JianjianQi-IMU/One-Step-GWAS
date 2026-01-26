#include "BlinkRaw.hpp"

#include <cstring>

namespace FARMCPU
{

BlinkRaw::BlinkRaw()
{
    maxLoop = 10;
    binSelection = 1e9;
    functionIndex = 3;
    priorIndex = 0;
    cutoffBon = 1;
    memset(lengthBinP, 0, 30 * sizeof(int));
    memset(prior, 0, 100 * sizeof(int));
}

BlinkRaw::~BlinkRaw()
{}

};

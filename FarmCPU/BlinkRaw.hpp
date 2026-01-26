#ifndef BLINKRAW_HPP
#define BLINKRAW_HPP

#include "FileDeal/FileIterator.hpp"
#include "DataManager/DataManager.hpp"
#include "FarmCPUAlgo.hpp"

#include <string>

namespace FARMCPU
{

class BlinkParam
{
public:
    double threMAF;
    uint32_t maxLoop;
    double threPVal;
    double threConverge;
    bool isLDRemove;

    BlinkParam()
    {
        threMAF = 0.0;
        maxLoop = 10;
        threPVal = -1;
        threConverge = 1.0;
        isLDRemove = false;
    }

    ~BlinkParam()
    {

    }
};

class BlinkRaw
{
private:
    FarmCPUUtil farmCpuUtil;
    FarmCPULM farmCpuLm;
public:
    int maxLoop;
    int binSelection;
    int lengthBinP[30];
    int nMarker;
    int functionIndex;
    int priorIndex;
    double cutoffBon;
    int prior[100];

    std::string logStr;

    BlinkRaw();
    ~BlinkRaw();

    

};

};

#endif // BLINKRAW2_HPP

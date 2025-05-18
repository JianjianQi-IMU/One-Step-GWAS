#ifndef BSATHRESHOLDCALCULATORF2_HPP
#define BSATHRESHOLDCALCULATORF2_HPP

#include "DataManager/DataManager.hpp"
#include <algorithm>

class BSAThresholdCalculatorF2
{
private:
    std::default_random_engine gen;
    size_t indN;
    size_t depN;
    double pv;
    size_t ploid;
    size_t testn;

    inline double getSNPIndex(int iDep,double ratio);
    inline double getDeltaIndex(int iDep);

public:
    BSAThresholdCalculatorF2();
    BSAThresholdCalculatorF2(size_t inIndN,size_t inDepN,double inPv);
    void setPara(size_t inIndN,size_t inDepN,double inPv);
    bool calcuThreshold(std::vector<double>& out); //  0 <= depth <= inDepN
};

#endif // BSATHRESHOLDCALCULATORF2_HPP

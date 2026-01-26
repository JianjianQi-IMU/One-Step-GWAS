#ifndef BSATHRESHOLDCALCULATORF2_HPP
#define BSATHRESHOLDCALCULATORF2_HPP

#include "DataManager/DataManager.hpp"
#include <algorithm>

class BSAThresholdCalculatorF2
{
private:
    std::default_random_engine gen;
    uint64_t indN;
    uint64_t depN;
    double pv;
    uint64_t ploid;
    uint64_t testn;

    inline double getSNPIndex(int iDep, double ratio);
    inline double getDeltaIndex(int iDep);

public:
    BSAThresholdCalculatorF2();
    BSAThresholdCalculatorF2(uint64_t inIndN, uint64_t inDepN, double inPv);
    void setPara(uint64_t inIndN, uint64_t inDepN, double inPv);
    bool calcuThreshold(std::vector<double>& out); //  0 <= depth <= inDepN
};

#endif // BSATHRESHOLDCALCULATORF2_HPP

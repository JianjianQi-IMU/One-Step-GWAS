#include "BSAThreshold.hpp"

double BSAThresholdCalculatorF2::getSNPIndex(int iDep, double ratio)
{
    std::binomial_distribution<int> index(iDep,ratio);
    return double(index(gen))/iDep;
}

double BSAThresholdCalculatorF2::getDeltaIndex(int iDep)
{
    std::binomial_distribution<int> dis(ploid * indN, 0.5);
    size_t i = 0;
    std::vector<double> testList;
    double tmp = 0;
    size_t pindex = size_t((1 - pv / 2) * testn);
    if (pindex == testn) {
        --pindex;
    }
    for (i = 0; i < testn; ++i) {
        tmp = getSNPIndex(iDep, dis(gen) / double(ploid * indN))
            - getSNPIndex(iDep, dis(gen) / double(ploid * indN));
        testList.push_back(tmp);
    }
    std::sort(testList.begin(), testList.end());
    return testList[pindex];
}

BSAThresholdCalculatorF2::BSAThresholdCalculatorF2()
{
    ploid = 2;
    testn = 10000;
}

BSAThresholdCalculatorF2::BSAThresholdCalculatorF2(size_t inIndN, size_t inDepN, double inPv)
{
    setPara(inIndN, inDepN, inPv);
}

void BSAThresholdCalculatorF2::setPara(size_t inIndN, size_t inDepN, double inPv)
{
    indN = inIndN;
    depN = inDepN;
    pv = inPv;
}

bool BSAThresholdCalculatorF2::calcuThreshold(std::vector<double> &out)
{
    out.clear();
    if (!indN || !depN || pv < 0 || pv > 1) {
        return false;
    }
    size_t i;
    out.push_back(0);
    for (i = 1; i <= depN; ++i) {
        out.push_back(getDeltaIndex(i));
    }
    return true;
}

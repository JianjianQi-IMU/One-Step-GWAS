
#ifndef FARMCPU_FARMCPUALGO_HPP
#define FARMCPU_FARMCPUALGO_HPP

#include "DataManager/MatrixLib.hpp"
#include "DataManager/BaseAlgoTools.hpp"
#include "FarmCPUFlag.hpp"
#include <vector>

namespace FARMCPU
{

class FarmCPUUtil
{
private:
    void HeapSortIdxTopToDown(const MML::Mat &valVec, std::vector<uint64_t> &idxVec, uint64_t lim, uint64_t idx);
    void HeapSortIdxInner(const MML::Mat &valVec, std::vector<uint64_t> &idx);

protected:
    double BICGetLikelihood(const MML::Mat &xMat, const MML::Mat &y);
    void SortIdx(const MML::Mat &valVec, std::vector<uint64_t> &outIdx);

public:
    FarmCPUUtil();

    ~FarmCPUUtil();

    FARMCPU_ERROR_FLAG CheckDataOne(const MML::SIMat &valVec, int8_t *outLabel);

    FARMCPU_ERROR_FLAG NormDataOne(const MML::SIMat &valVec, double denom, MML::Mat &out);

    FARMCPU_ERROR_FLAG FilterLessIdx(const MML::Mat &valVec, double threshold, std::vector<uint64_t> &outIdx);

    FARMCPU_ERROR_FLAG RemoveLD(const MML::Mat &xMat, double threshold, uint64_t lim, std::vector<uint64_t> &outIdx);

    /* xMat (nSample, nMarker); y (nSample, 1) */
    FARMCPU_ERROR_FLAG BICSelectionIdx(const MML::Mat &xMat, const MML::Mat &y, uint64_t *outNum);

    FARMCPU_ERROR_FLAG UpdateIdx(const std::vector<uint64_t> &newIdx, std::vector<uint64_t> &inOutIdx);
};

class FarmCPULM
{
private:
    bool isInit;
    MML::Mat my;
    MML::Mat mW;
    MML::Mat miWtW;
    MML::Mat mytW;
    double myty;
protected:

public:
    FarmCPULM();

    FarmCPULM(const MML::Mat &y, const MML::Mat &W);

    ~FarmCPULM();

    FARMCPU_ERROR_FLAG LMInit(const MML::Mat &y, const MML::Mat &W);

    FARMCPU_ERROR_FLAG LMTestOne(const MML::Mat &y, const MML::Mat &W, const MML::Mat &x,
        double &betax, double &tval);
    
    FARMCPU_ERROR_FLAG LMTestOne(const MML::Mat &x, double &betax, double &tval);

    FARMCPU_ERROR_FLAG LMTestOne(const MML::Mat &x, MML::Mat &betaVec, MML::Mat &tvalVec);

};

};

#endif

#include "FarmCPUAlgo.hpp"
#include "DataManager/DataDefine.hpp"
#include <cstdint>
#include <cmath>
#include <algorithm>

namespace FARMCPU
{

void FarmCPUUtil::HeapSortIdxTopToDown(const MML::Mat &valVec, std::vector<uint64_t> &idxVec, uint64_t lim, uint64_t idx)
{
    uint64_t tIdx = idxVec[idx];
    while (idx <= lim) {
        uint64_t lc = idx * 2 + 1;
        uint64_t rc = idx * 2 + 2;
        uint64_t tc = lc;
        if (tc > lim) {
            break;
        }
        if (rc <= lim && valVec(idxVec[rc]) > valVec(idxVec[tc])) {
            tc = rc;
        }
        if (valVec(idxVec[tc]) > valVec(tIdx)) {
            idxVec[idx] = idxVec[tc];
            idx = tc;
        } else {
            break;
        }
    }
    idxVec[idx] = tIdx;
}

void FarmCPUUtil::HeapSortIdxInner(const MML::Mat &valVec, std::vector<uint64_t> &idx)
{
    uint64_t num = idx.size();
    for (uint64_t i = (num >> 1); i > 0; --i) {
        HeapSortIdxTopToDown(valVec, idx, num - 1, i);
    }
    for (uint64_t i = num - 1; i > 0; --i) {
        HeapSortIdxTopToDown(valVec, idx, i, 0);
        uint64_t tmp = idx[i];
        idx[i] = idx[0];
        idx[0] = tmp;
    }
}

double FarmCPUUtil::BICGetLikelihood(const MML::Mat &xMat, const MML::Mat &y)
{
    MML::Mat xx;
    MML::Mat ixx;
    MML::Mat xy;
    MML::Mat beta;
    MML::Mat xbeta;
    uint64_t n = xMat.getNRow();
    double sse = 0;
    MML::Mat::XtYmul(xMat, xMat, xx);
    xx.toSym('L');
    xx.symInv(ixx);
    MML::Mat::VtXmul(y, xMat, xy);
    xy.t();
    MML::Mat::XVmul(ixx, xy, beta);
    MML::Mat::XVmul(xMat, beta, xbeta);
    for (uint64_t i = 0; i < n; ++i) {
        sse += (y(i) - xbeta(i)) * (y(i) - xbeta(i));
    }
    return n + n * std::log(2 * MML::PI) + n * log(sse / n);
}

void FarmCPUUtil::SortIdx(const MML::Mat &valVec, std::vector<uint64_t> &outIdx)
{
    HeapSortIdxInner(valVec, outIdx);
}

FarmCPUUtil::FarmCPUUtil()
{

}

FarmCPUUtil::~FarmCPUUtil()
{

}

FARMCPU_ERROR_FLAG FarmCPUUtil::CheckDataOne(const MML::SIMat &valVec, int8_t *outLabel)
{
    uint64_t num = valVec.getNCol() * valVec.getNRow();
    if (num == 0) {
        *outLabel = 0;
        return FARMCPUPRE_ERROR_SUCCESS;
    }
    uint64_t validNum = 0;
    int8_t isDiff = 0;
    int16_t lastVal = valVec(0);
    for (uint64_t i = 0; i < num; ++i) {
        if (valVec(i) != MML::UNASSIGNED) {
            ++validNum;
            if (lastVal != MML::UNASSIGNED && lastVal != valVec(i)) {
                isDiff = 1;
            }
            lastVal = valVec(i);
        }
    }
    if (validNum > 0 && isDiff != 0) {
        *outLabel = 1;
    } else {
        *outLabel = 0;
    }
    return FARMCPUPRE_ERROR_SUCCESS;
}

FARMCPU_ERROR_FLAG FarmCPUUtil::NormDataOne(const MML::SIMat &valVec, double denom, MML::Mat &out)
{
    uint64_t num = valVec.getNCol() * valVec.getNRow();
    uint64_t validNum = 0;
    double validSum = 0;
    out.setData(valVec.getNRow(), valVec.getNCol(), (double)0.0, MML::_colvec);
    for (uint64_t i = 0; i < num; ++i) {
        if (valVec(i) != MML::UNASSIGNED) {
            ++validNum;
            validSum += valVec(i);
        }
    }
    for (uint64_t i = 0; i < num; ++i) {
        if (valVec(i) == MML::UNASSIGNED) {
            out(i) = validSum / validNum;
        } else {
            out(i) = valVec(i);
        }
        out(i) /= denom;
    }
    return FARMCPUPRE_ERROR_SUCCESS;
}

FARMCPU_ERROR_FLAG FarmCPUUtil::FilterLessIdx(const MML::Mat &valVec, double threshold, std::vector<uint64_t> &outIdx)
{
    uint64_t num = valVec.getNCol() * valVec.getNRow();
    outIdx.clear();
    for (uint64_t i = 0; i < num; ++i) {
        if (valVec(i) < threshold && valVec(i) != MML::DATA_NA) {
            outIdx.push_back(i);
        }
    }
    if (outIdx.size() == 0) {
        return FARMCPUPRE_ERROR_SUCCESS;
    }
    SortIdx(valVec, outIdx);
    return FARMCPUPRE_ERROR_SUCCESS;
}

FARMCPU_ERROR_FLAG FarmCPUUtil::RemoveLD(const MML::Mat &xMat, double threshold, uint64_t lim, std::vector<uint64_t> &outIdx)
{
    uint64_t n = xMat.getNRow();
    uint64_t len = xMat.getNCol();
    MML::Mat sumx(1, len, (double)0.0);
    MML::Mat sumx2(1, len, (double)0.0);
    outIdx.clear();
    lim = lim < len ? lim : len;
    for (uint64_t i = 0; i < len; ++i) {
        double sumi = 0;
        double sumi2 = 0;
        for (uint64_t k = 0; k < n; ++k) {
            sumi += xMat(k, i);
            sumi2 += xMat(k, i) * xMat(k, i);
        }
        sumx(i) = sumi;
        sumx2(i) = sumi2;
        outIdx.push_back(i);
    }
    uint64_t l = 0;
    uint64_t lenj = len;
    for (uint64_t i = 0; i < lim; ++i) {
        uint64_t idx = outIdx[i];
        l = i + 1;
        for (uint64_t j = i + 1; j < lenj; ++j) {
            uint64_t idxj = outIdx[j];
            double sumij = 0;
            for (uint64_t k = 0; k < n; ++k) {
                sumij += xMat(k, idx) * xMat(k, idxj);
            }
            double nume = n * sumij - sumx(idx) * sumx(idxj);
            double deno = std::sqrt((n * sumx2(idx) - sumx(idx) * sumx(idx)) * (n * sumx2(idxj) - sumx(idxj) * sumx(idxj)));
            if (nume / (deno + MML::EPS) < threshold) {
                outIdx[l] = outIdx[j];
                ++l;
            }
        }
        lim = l;
        lenj = lim;
    }
    outIdx.erase(outIdx.begin() + l, outIdx.end());
    return FARMCPUPRE_ERROR_SUCCESS;
}

FARMCPU_ERROR_FLAG FarmCPUUtil::BICSelectionIdx(const MML::Mat &xMat, const MML::Mat &y, uint64_t *outNum)
{
    uint64_t n = xMat.getNRow();
    uint64_t len = xMat.getNCol();
    double minll = 0;
    uint64_t minIdx = UINT64_MAX;
    MML::Mat tmpx;
    MML::Mat tmpy;
    tmpx.setMatClass(MML::_general);
    tmpy.resize(n, 1);
    tmpy.setMatClass(MML::_colvec);
    for (uint64_t j = 0; j < n; ++j) {
        tmpy(j) = y(j);
    }
    for (uint64_t i = 1; i < len; ++i) {
        tmpx.resize(n, i + 1);
        for (uint64_t k = 0; k < i + 1; ++k) {
            for (uint64_t j = 0; j < n; ++j) {
                tmpx(j, k) = xMat(j, k);
            }
        }
        double tmpll = BICGetLikelihood(tmpx, tmpy);
        if (minIdx == UINT64_MAX || minll > tmpll) {
            minll = tmpll;
            minIdx = i;
        }
    }
    *outNum = minIdx;
    return FARMCPUPRE_ERROR_SUCCESS;
}

FARMCPU_ERROR_FLAG FarmCPUUtil::UpdateIdx(const std::vector<uint64_t> &newIdx, std::vector<uint64_t> &inOutIdx)
{
    uint64_t nNew = newIdx.size();
    uint64_t nIn = inOutIdx.size();
    for (uint64_t i = 0; i < nNew; ++i) {
        int8_t diff = 1;
        for (uint64_t j = 0; j < nIn; ++j) {
            if (newIdx[i] == inOutIdx[j]) {
                diff = 0;
            }
        }
        if (diff != 0) {
            inOutIdx.push_back(newIdx[i]);
        }
    }
    return FARMCPUPRE_ERROR_SUCCESS;
}

FarmCPULM::FarmCPULM()
{
    isInit = false;
}

FarmCPULM::FarmCPULM(const MML::Mat &y, const MML::Mat &W)
    : FarmCPULM()
{
    LMInit(y, W);
}

FarmCPULM::~FarmCPULM()
{

}

FARMCPU_ERROR_FLAG FarmCPULM::LMInit(const MML::Mat &y, const MML::Mat &W)
{
    int64_t m = y.getNRow();
    int64_t q = W.getNCol();
    if (m == 0) {
        return FARMCPUPRE_ERROR_LM_Y_VEC_EMPTY;
    }
    if (q == 0 || W.getNRow() == (uint64_t)0) {
        return FARMCPUPRE_ERROR_LM_W_MAT_EMPTY;
    }
    if (y.getMatClass() != MML::_colvec) {
        return FARMCPUPRE_ERROR_LM_Y_VEC_NO_COLVEC;
    }
    if ((uint64_t)m != W.getNRow()) {
        return FARMCPUPRE_ERROR_LM_M_NUM_UNEQUAL;
    }
    MML::Mat WtW;
    MML::Mat::XtXmul(W, WtW);
    WtW.symInv(miWtW);

    MML::Mat::VtXmul(y, W, mytW);

    MML::Mat::VtUmul(y, y, myty);

    my = y;
    mW = W;

    isInit = true;

    return FARMCPUPRE_ERROR_SUCCESS;
}

FARMCPU_ERROR_FLAG FarmCPULM::LMTestOne(const MML::Mat &y, const MML::Mat &W, const MML::Mat &x,
    double &betax, double &tval)
{
    int64_t m = y.getNRow();
    int64_t q = W.getNCol();
    if (y.getMatClass() != MML::_colvec) {
        return FARMCPUPRE_ERROR_LM_Y_VEC_NO_COLVEC;
    }
    if (x.getMatClass() != MML::_colvec) {
        return FARMCPUPRE_ERROR_LM_X_VEC_NO_COLVEC;
    }
    if (m == 0) {
        return FARMCPUPRE_ERROR_LM_Y_VEC_EMPTY;
    }
    if (q == 0 || W.getNRow() == (uint64_t)0) {
        return FARMCPUPRE_ERROR_LM_W_MAT_EMPTY;
    }
    if (x.getNRow() == (uint64_t)0) {
        return FARMCPUPRE_ERROR_LM_X_VEC_EMPTY;
    }
    if ((uint64_t)m != W.getNRow() || (uint64_t)m != x.getNRow()) {
        return FARMCPUPRE_ERROR_LM_M_NUM_UNEQUAL;
    }
    int32_t i, j;
    MML::Mat Mt;
    MML::Mat MtM;
    MML::Mat iMtM;
    MML::Mat MiMtM;
    MML::Mat K;
    MML::Mat ytM, Mty;
    MML::Mat beta;

    double ytKy;
    double yty;
    double sigma2;
    double sqrtCovx;
    double df = double(m - q - 2);

    MML::Mat M(m, q + 1, (double)0);
    for (i = 0; i < m; ++i) {
        for (j = 0; j < q; ++j) {
            M(i, j) = W(i, j);
        }
    }
    for (i = 0; i < m; ++i) {
        M(i, q) = x(i);
    }
    M.t(Mt);
    MML::Mat::VtXmul(y, M, ytM);
    ytM.t(Mty);
    MML::Mat::XtXmul(M, MtM);
    MtM.symInv(iMtM);
    MML::Mat::XYmul(M, iMtM, MiMtM);
    MML::Mat::XYmul(MiMtM, Mt, K);
    MML::Mat::VtXUmul(y, K, y, ytKy);
    MML::Mat::VtUmul(y, y, yty);
    MML::Mat::XYmul(iMtM, Mty, beta);

    sigma2 = (yty - ytKy) / df;
    
    betax = beta(q);
    sqrtCovx = std::sqrt(sigma2 * iMtM(q, q));
    if (std::fabs(sqrtCovx) < MML::EPS12) {
        return FARMCPUPRE_ERROR_LM_SQRTBETAX_ZERO;
    }
    tval = betax / sqrtCovx;

    return FARMCPUPRE_ERROR_SUCCESS;
}

FARMCPU_ERROR_FLAG FarmCPULM::LMTestOne(const MML::Mat &x, double &betax, double &tval)
{
    if (isInit == false) {
        return FARMCPUPRE_ERROR_LM_UNINIT;
    }
    if (x.getMatClass() != MML::_colvec) {
        return FARMCPUPRE_ERROR_LM_X_VEC_NO_COLVEC;
    }
    if (x.getNRow() == (uint64_t)0) {
        return FARMCPUPRE_ERROR_LM_X_VEC_EMPTY;
    }
    int64_t m = my.getNRow();
    int64_t q = mW.getNCol();
    if ((uint64_t)m != mW.getNRow() || (uint64_t)m != x.getNRow()) {
        return FARMCPUPRE_ERROR_LM_M_NUM_UNEQUAL;
    }

    int32_t i, j;
    bool ret;
    MML::Mat H;
    MML::Mat Wtx, xtW;
    MML::Mat A, B;
    MML::Mat iMM;
    MML::Mat Mty;
    MML::Mat beta;
    double ytMiMMMty;
    double HWtx;
    double D;
    double sigma2;
    double sqrtCovx;
    double xtx;
    double ytx;
    double df = double(m - q - 2);

    ret = MML::Mat::VtXmul(x, mW, xtW);
    xtW.t(Wtx);
    ret = MML::Mat::VtXmul(Wtx, miWtW, H);
    ret = MML::Mat::VtXUmul(Wtx, miWtW, Wtx, HWtx);
    ret = MML::Mat::VtUmul(x, x, xtx);
    D = xtx - HWtx;
    D = 1.0 / D;
    B = H;
    B.mul(-D);

    iMM.resize(q + 1, q + 1);
    for (i = 0; i < q; ++i) {
        for (j = 0; j < q; ++j) {
            iMM(i, j) = miWtW(i, j) + D * H(i) * H(j);
        }
    }

    for (i = 0; i < q; ++i) {
        iMM(i, q) = B(i);
        iMM(q, i) = B(i);
    }

    iMM(q, q) = D;

    MML::Mat::VtUmul(my, x, ytx);
    Mty.resize(q + 1, 1);
    Mty.setMatClass(MML::_colvec);
    for (i = 0; i < q; ++i) {
        Mty(i) = mytW(i);
    }
    Mty(q) = ytx;

    MML::Mat::VtXUmul(Mty, iMM, Mty, ytMiMMMty);
    sigma2 = (myty - ytMiMMMty) / df;

    MML::Mat::XYmul(iMM, Mty, beta);
    betax = beta(q);
    sqrtCovx = std::sqrt(sigma2 * iMM(q, q));
    if (std::fabs(sqrtCovx) < MML::EPS12) {
        return FARMCPUPRE_ERROR_LM_SQRTBETAX_ZERO;
    }
    tval = betax / sqrtCovx;

    return FARMCPUPRE_ERROR_SUCCESS;
}

FARMCPU_ERROR_FLAG FarmCPULM::LMTestOne(const MML::Mat &x, MML::Mat &betaVec, MML::Mat &tvalVec)
{
    if (isInit == false) {
        return FARMCPUPRE_ERROR_LM_UNINIT;
    }
    if (x.getMatClass() != MML::_colvec) {
        return FARMCPUPRE_ERROR_LM_X_VEC_NO_COLVEC;
    }
    if (x.getNRow() == (uint64_t)0) {
        return FARMCPUPRE_ERROR_LM_X_VEC_EMPTY;
    }
    int64_t m = my.getNRow();
    int64_t q = mW.getNCol();
    if ((uint64_t)m != mW.getNRow() || (uint64_t)m != x.getNRow()) {
        return FARMCPUPRE_ERROR_LM_M_NUM_UNEQUAL;
    }

    int32_t i, j;
    bool ret;
    MML::Mat H;
    MML::Mat Wtx, xtW;
    MML::Mat A, B;
    MML::Mat iMM;
    MML::Mat Mty;
    MML::Mat beta;
    MML::Mat sqrtCov;
    double ytMiMMMty;
    double HWtx;
    double D;
    double sigma2;
    double xtx;
    double ytx;
    double df = double(m - q - 2);

    ret = MML::Mat::VtXmul(x, mW, xtW);
    xtW.t(Wtx);
    ret = MML::Mat::VtXmul(Wtx, miWtW, H);
    ret = MML::Mat::VtXUmul(Wtx, miWtW, Wtx, HWtx);
    ret = MML::Mat::VtUmul(x, x, xtx);
    D = xtx - HWtx;
    D = 1.0 / D;
    B = H;
    B.mul(-D);

    iMM.resize(q + 1, q + 1);
    for (i = 0; i < q; ++i) {
        for (j = 0; j < q; ++j) {
            iMM(i, j) = miWtW(i, j) + D * H(i) * H(j);
        }
    }

    for (i = 0; i < q; ++i) {
        iMM(i, q) = B(i);
        iMM(q, i) = B(i);
    }

    iMM(q, q) = D;

    MML::Mat::VtUmul(my, x, ytx);
    Mty.resize(q + 1, 1);
    Mty.setMatClass(MML::_colvec);
    for (i = 0; i < q; ++i) {
        Mty(i) = mytW(i);
    }
    Mty(q) = ytx;

    MML::Mat::VtXUmul(Mty, iMM, Mty, ytMiMMMty);
    sigma2 = (myty - ytMiMMMty) / df;

    MML::Mat::XYmul(iMM, Mty, beta);
    betaVec = beta;
    sqrtCov.resize(q + 1, 1);
    sqrtCov.setMatClass(MML::_colvec);
    tvalVec.resize(q + 1, 1);
    tvalVec.setMatClass(MML::_colvec);
    for (i = 0; i < q + 1; ++i) {
        sqrtCov(i) = std::sqrt(sigma2 * iMM(i, i));
        if (std::fabs(sqrtCov(i)) < MML::EPS12) {
            tvalVec(i) = MML::DATA_NA;
        } else {
            tvalVec(i) = betaVec(i) / sqrtCov(i);
        }
    }

    return FARMCPUPRE_ERROR_SUCCESS;
}

};

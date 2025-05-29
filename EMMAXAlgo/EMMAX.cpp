#include "EMMAX.hpp"

EMMAX::REML::REML()
{
    eps = MML::EPS;
    pi = MML::PI;
    isValid = false;
    filterN = 0;
    factorN = 0;
}

EMMAX::REML::REML(const MML::Kinship& inKin, const MML::Covariates& inCov)
    : REML()
{
    read(inKin,inCov);
}

EMMAX::REML::~REML()
{

}

bool EMMAX::REML::sortEigen(size_t* outIndex,size_t inNum){
    size_t i = 0;
    for (i = 0; i < inNum; ++i) {
        outIndex[i]=i;
    }
    qSortEigen(0, inNum - 1, outIndex);
    return true;
}

void EMMAX::REML::qSortEigen(size_t a, size_t b, size_t *outIndex)
{
    size_t i = 0, j = 0, tmp = 0;
    if (a < b) {
        i = a, j = b;
        tmp = outIndex[i];
        while (i < j) {
            while (i < j && rawEigenVal(outIndex[j]) >= rawEigenVal(tmp)) {
                --j;
            }
            outIndex[i] = outIndex[j];
            while (i < j && rawEigenVal(outIndex[i]) < rawEigenVal(tmp)) {
                ++i;
            }
            outIndex[j] = outIndex[i];
        }
        outIndex[i] = tmp;
        if (i >= 1) qSortEigen(a, i - 1, outIndex);
        qSortEigen(i + 1, b, outIndex);
    }
}

bool EMMAX::REML::read(const MML::Kinship& inKin, const MML::Covariates& inCov){
    if (!(inKin.isValid) || !(inCov.isValid) ||
       inKin.getFilterNum() != inCov.getFilterNum()) {
        isValid = false;
        return false;
    }
    filterN = inKin.getFilterNum();
    factorN = inCov.getFactorNum();
    MML::Mat K, KtK, KtKi, KKtKi;
    MML::Mat S, V, SV, SVS;
    size_t i = 0;
    size_t *indexList = nullptr;
    K = inCov.getFilterMat();
    V = inKin.getFilterMat();
    for (i = 0; i < filterN; ++i) {
        V(i, i) += 1.0;
    }
    MML::Mat::XtXmul(K, KtK);
    KtK.toSym('L');
    if (!(KtK.symInv(KtKi))) {
        isValid = false;
        return false;
    }
    MML::Mat::XYmul(K, KtKi, KKtKi);
    K.t();
    MML::Mat::XYmul(KKtKi, K, S);
    S.mul(-1);
    for (i = 0; i < filterN; ++i) {
        S(i, i) += 1.0;
    }
    MML::Mat::XYmul(S, V, SV);
    MML::Mat::XYmul(SV, S, SVS);
    SVS.toSym('L');
    if (!(SVS.symEig(rawEigenVec, rawEigenVal))) {
        isValid = false;
        return false;
    }
    rawEigenVal.add(-1.0);
    indexList = new size_t [filterN];
    sortEigen(indexList, filterN);
    rawEigenVec.sortCols(filterN, indexList);
    rawEigenVal.sortRows(filterN, indexList);
    delete [] indexList;
    rawEigenVec.copyToCols(factorN, filterN - 1, filterEigenVec);
    rawEigenVal.copyToRows(factorN, filterN - 1, filterEigenVal);
    isValid = true;
    return true;
}

bool EMMAX::REML::getDelta(const MML::Phenotype& inPhe, double& outDelta, double& outLLD)
{
    if (!(inPhe.isValid) || inPhe.filterN != filterN || !isValid) return false;
    MML::Mat etas, EVect;
    filterEigenVec.t(EVect);
    MML::Mat::XVmul(EVect, inPhe.getFilterVec(), etas);
    size_t rangeNum = 100, i = 0;
    double *expRange = new double [rangeNum + 1];
    double *dLL = new double [rangeNum + 1];
    double upVal = 0.0, downVal = 0.0, midVal = 0.0;
    double halfDiff = 0.0, dLLMid = 0.0;
    for (i = 0; i <= rangeNum; ++i) {
        expRange[i] = std::exp(i * 0.2 - 10);
    }
    if (dLikelihood(etas, expRange[0], dLL[0])) {
        if (dLL[0] < 0) {
            delete [] expRange;
            delete [] dLL;
            outDelta = expRange[0];
            return true;
        }
    } else {
        delete [] expRange;
        delete [] dLL;
        return false;
    }
    if (dLikelihood(etas, expRange[rangeNum], dLL[rangeNum])) {
        if (dLL[rangeNum] > 0) {
            delete [] expRange;
            delete [] dLL;
            outDelta = expRange[rangeNum];
            return true;
        }
    } else {
        delete [] expRange;
        delete [] dLL;
        return false;
    }
    for (i = 1; i <= rangeNum; ++i) {
        if (dLikelihood(etas, expRange[i], dLL[i])) {
            if (dLL[i] < 0) {
                upVal = expRange[i], downVal = expRange[i - 1];
                break;
            }
        } else {
            delete [] expRange;
            delete [] dLL;
            return false;
        }
    }
    delete [] expRange;
    delete [] dLL;
    halfDiff = (upVal - downVal) / 2;
    midVal = halfDiff + downVal;
    while (std::abs(upVal - downVal) > eps) {
        if (dLikelihood(etas, midVal, dLLMid)) {
            if (dLLMid > 0) downVal = midVal;
            else upVal = midVal;
        }
        else return false;
        halfDiff = (upVal - downVal) / 2;
        midVal = halfDiff + downVal;
    }
    outDelta = midVal;
    if (!(likelihood(etas, midVal, outLLD))) return false;
    return true;
}

bool EMMAX::REML::dLikelihood(const MML::Mat& etas, double delta, double& outVal)
{
    if (etas.nrow != filterN - factorN) return false;
    double sum1 = 0, sum2 = 0, sum3 = 0;
    for (size_t i = 0; i < filterN - factorN; i++) {
        sum1 += (etas(i) * etas(i) / ((filterEigenVal(i) + delta) * (filterEigenVal(i) + delta)));
        sum2 += (etas(i) * etas(i) / (filterEigenVal(i) + delta));
        sum3 += (1 / (filterEigenVal(i) + delta));
    }
    outVal = 0.5 * ((filterN - factorN) * (sum1 / sum2) - sum3);
    return true;
}

bool EMMAX::REML::likelihood(const MML::Mat& etas, double delta, double& outVal)
{
    if (etas.nrow != filterN - factorN) return false;
    double sum1 = 0, sum2 = 0, ffn = filterN - factorN;
    for (int i = 0; i < filterN - factorN; i++) {
        sum1 += (etas(i) * etas(i) / (filterEigenVal(i) + delta));
        sum2 += std::log(filterEigenVal(i) + delta);
    }
    outVal = 0.5 * ((ffn) * (std::log((ffn) / (2 * pi)) - 1 - std::log(sum1)) - sum2);
    return true;
}

bool EMMAX::REML::getLL(const MML::Phenotype &inPhe, double inDelta, double &outLL)
{
    if (!(inPhe.isValid) || inPhe.filterN != filterN || !isValid) return false;
    MML::Mat etas, EVect;
    filterEigenVec.t(EVect);
    MML::Mat::XVmul(EVect, inPhe.getFilterVec(), etas);
    if (likelihood(etas, inDelta, outLL)) return true;
    return false;
}

EMMAX::Betas::Betas()
{
    isValid = false;
    eps = MML::EPS;
}

EMMAX::Betas::Betas(const MML::Phenotype& inPhe, const MML::Kinship& inKin,
    const MML::Covariates& inCov, double inDelta) :Betas()
{
    read(inPhe, inKin, inCov, inDelta);
}

EMMAX::Betas::~Betas()
{

}

bool EMMAX::Betas::read(const MML::Phenotype& inPhe, const MML::Kinship& inKin, const MML::Covariates& inCov, double inDelta)
{
    if (inPhe.filterN!=inKin.filterN || inPhe.filterN!=inCov.filterN || !(inPhe.isValid) ||
       !(inKin.isValid) || !(inCov.isValid) || !(inKin.isEigen)) {
        isValid = false;
        delta = 0.0;
        return false;
    }
    filterN = inCov.filterN;
    factorN = inCov.factorN;
    Ut = inKin.getEigenVec();
    Ut.t();
    D = inKin.getEigenVal();
    if (!(MML::Mat::XVmul(Ut, inPhe.getFilterVec(), Uty))) {
        isValid = false;
        return false;
    }
    if (!(MML::Mat::XYmul(Ut, inCov.getFilterMat(), UtKx))) {
        isValid = false;
        return false;
    }
    UtKx.appendCols(1);
    delta = inDelta;
    D.add(delta);
    if (!(MML::Mat::XVmul(Ut, inPhe.getFilterVec(), Uty))) {
        isValid = false;
        return false;
    }
    if (!(MML::Mat::VtDiUmul(Uty, D, Uty, UtytDiUty))) {
        isValid = false;
        return false;
    }
    isValid = true;
    return true;
}

bool EMMAX::Betas::calculateTstat(const MML::Mat& inMarkers, double& outBetas0, double& outTstat)
{
    if (inMarkers.info == MML::_null || !isValid || inMarkers.info != MML::_colvec) return false;
    MML::Mat Utx, UtKxtDiUtKx, UtKxtDiUtKxi, UtKxtDiUty, B;

    double denoTstat = 0.0, VtHiV = 0.0;
    if (!(MML::Mat::XVmul(Ut, inMarkers, Utx))) return false;
    for (size_t i = 0; i < filterN; ++i) {
        UtKx(i, factorN) = Utx(i);
    }

    if (!(MML::Mat::XtDiXmul(UtKx, D, UtKxtDiUtKx))) return false;
    UtKxtDiUtKx.toSym('L');
    if (!(UtKxtDiUtKx.symInv(UtKxtDiUtKxi))) return false;
    if (!(MML::Mat::XtDiVmul(UtKx, D, Uty, UtKxtDiUty))) return false;
    if (!(MML::Mat::XVmul(UtKxtDiUtKxi, UtKxtDiUty, B))) return false;
    if (!(MML::Mat::VtXUmul(UtKxtDiUty, UtKxtDiUtKxi, UtKxtDiUty, VtHiV))) return false;

    denoTstat = std::sqrt(UtKxtDiUtKxi(factorN, factorN) * (UtytDiUty - VtHiV) / (filterN - factorN - 1));
    outBetas0 = B(factorN);
    if (std::fabs(denoTstat) < eps) return false;
    outTstat = outBetas0 / denoTstat;
    return true;
}

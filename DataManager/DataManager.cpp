#include "DataManager.hpp"
#include <cmath>

MML::Phenotype::Phenotype()
{
    dataNA = DATA_NA;
    eps = EPS;
    rawData = nullptr;
    filterData = nullptr;
    filterIndex = nullptr;
    rawN = 0;
    filterN = 0;
    isValid = false;
}

MML::Phenotype::Phenotype(const Phenotype &object) :Phenotype()
{
    copy(object);
}

MML::Phenotype::Phenotype(size_t inSampleNumber, const double* inData)
    : Phenotype()
{
    if (!inData || inSampleNumber == 0) return;
    rawData = new double [inSampleNumber];
    rawN = inSampleNumber;
    filterN = 0;
    size_t j = 0;
    for (size_t i = 0; i < rawN; ++i) {
        rawData[i] = inData[i];
        if (rawData[i] != dataNA) ++filterN;
    }
    if (filterN == 0) {
        filterData = nullptr;
        filterIndex = nullptr;
        isValid = false;
        return ;
    }
    filterData = new double [filterN];
    filterIndex = new size_t [filterN];
    for (size_t i = 0; i < rawN; ++i) {
        if (rawData[i] != dataNA) {
            filterData[j] = rawData[i];
            filterIndex[j] = i;
            ++j;
        }
    }
    filterVec.setData(filterN, 1, filterData, _colvec);
    isValid = true;
}

MML::Phenotype::~Phenotype()
{
    if (rawData) delete [] rawData;
    if (filterData) delete [] filterData;
    if (filterIndex) delete [] filterIndex;
}

bool MML::Phenotype::read(size_t inSampleNumber, const double* inData)
{
    if (!inData || inSampleNumber == 0) {
        rawData = nullptr;
        filterData = nullptr;
        filterIndex = nullptr;
        rawN = 0;
        filterN = 0;
        isValid = false;
        return false;
    }
    if (rawN != inSampleNumber) {
        if (rawData) delete [] rawData;
        rawData = new double [inSampleNumber];
        rawN = inSampleNumber;
    }
    filterN = 0;
    size_t j = 0;
    for (size_t i = 0; i < rawN; ++i) {
        rawData[i] = inData[i];
        if (rawData[i] != dataNA) ++filterN;
    }
    if (filterN == 0) {
        filterData = nullptr;
        filterIndex = nullptr;
        isValid = false;
        return false;
    }
    if (filterData) delete [] filterData;
    if (filterIndex) delete [] filterIndex;
    filterData = new double [filterN];
    filterIndex = new size_t [filterN];
    for (size_t i = 0; i < rawN; ++i) {
        if (rawData[i] != dataNA) {
            filterData[j] = rawData[i];
            filterIndex[j] = i;
            ++j;
        }
    }
    filterVec.setData(filterN, 1, filterData, _colvec);
    isValid = true;
    return true;
}

bool MML::Phenotype::filterMarkers(const Mat& inVec, Mat& outVec){
    if (!isValid) return false;
    size_t i, validN = 0;
    double S = 0, E;
    outVec.resize(filterN, 1);
    outVec.info = _colvec;
    bool good = false;
    double last = inVec(filterIndex[0]);
    for (i = 0; i < filterN; ++i) {
        outVec(i) = inVec(filterIndex[i]);
        if (outVec(i) != dataNA) {
            if (last != dataNA && std::fabs(last - outVec(i)) > eps) {
                good = true;
            }
            last = outVec(i);
            S += outVec(i);
            ++validN;
        }
    }
    if (!good) return false;
    E = S / validN;
    for (i = 0; i < filterN; ++i) {
        if (outVec(i) == dataNA) {
            outVec(i) = E;
        }
    }
    return true;
}

const MML::Mat& MML::Phenotype::getFilterVec() const
{
    return filterVec;
}

const size_t* MML::Phenotype::getFilterIndex() const
{
    return filterIndex;
}

size_t MML::Phenotype::getFilterNum() const
{
    return filterN;
}

void MML::Phenotype::copy(const Phenotype &object)
{
    dataNA = object.dataNA;
    eps = object.eps;
    if (object.rawN != rawN) {
        if (rawData) delete [] rawData;
        rawN = object.rawN;
        if (!rawN) rawData = nullptr;
        else rawData = new double [rawN];
    }
    if (filterN != object.filterN) {
        if (filterData) delete [] filterData;
        if (filterIndex) delete [] filterIndex;
        filterN = object.filterN;
        if (!filterN) {
            filterData = nullptr;
            filterIndex = nullptr;
        } else {
            filterData = new double [filterN];
            filterIndex = new size_t [filterN];
        }
    }
    isValid = object.isValid;
    for (size_t i = 0; i < rawN; ++i) {
        rawData[i] = (object.rawData)[i];
    }
    for (size_t i = 0; i < filterN; ++i) {
        filterData[i] = (object.filterData)[i];
        filterIndex[i] = (object.filterIndex)[i];
    }
    filterVec = object.filterVec;
}

void MML::Phenotype::operator=(const Phenotype &object)
{
    copy(object);
}

MML::Kinship::Kinship(size_t inSampleNumber, const double* inData, const size_t* inFilterIndex,
                      size_t inFilterNum)
    : Kinship()
{
    read(inSampleNumber, inData, inFilterIndex, inFilterNum);
}

MML::Kinship::Kinship()
{
    isValid = false;
    isEigen = false;
    rawN = 0;
    filterN = 0;
    filterIndex = nullptr;
}

MML::Kinship::Kinship(const Kinship &object)
    : Kinship()
{
    copy(object);
}

MML::Kinship::~Kinship()
{
    if (filterIndex) delete [] filterIndex;
}

bool MML::Kinship::read(size_t inSampleNumber, const double* inData, const size_t* inFilterIndex,
                        size_t inFilterNum)
{
    if (!inData || inSampleNumber == 0 || inFilterNum == 0) {
        isValid = false;
        isEigen = false;
        rawN = 0;
        filterN = 0;
        filterIndex = nullptr;
        return false;
    }
    if (filterIndex) delete [] filterIndex;
    rawN = inSampleNumber;
    rawMat.setData(rawN, rawN, inData);
    filterMat = rawMat;
    filterMat.setCols(inFilterNum, inFilterIndex);
    filterMat.setRows(inFilterNum, inFilterIndex);
    filterMat.toSym('L');
    filterN = inFilterNum;
    filterIndex = new size_t [inFilterNum];
    for (size_t i = 0; i < inFilterNum; ++i) {
        filterIndex[i] = inFilterIndex[i];
    }
    isValid = true;
    isEigen = false;
    return true;
}

bool MML::Kinship::eigen()
{
    if (!isEigen) {
        isEigen = true;
        if (!(filterMat.symEig(eigenVec, eigenVal))) {
            isEigen = false;
        }
    }
    return isEigen;
}

const MML::Mat& MML::Kinship::getFilterMat() const
{
    return filterMat;
}

size_t MML::Kinship::getFilterNum() const
{
    return filterN;
}

const MML::Mat& MML::Kinship::getEigenVec() const
{
    return eigenVec;
}

const MML::Mat& MML::Kinship::getEigenVal() const
{
    return eigenVal;
}

const size_t* MML::Kinship::getFilterIndex() const
{
    return filterIndex;
}

void MML::Kinship::copy(const Kinship &object)
{
    if (filterN != object.filterN) {
        if (filterIndex) delete [] filterIndex;
        filterN = object.filterN;
        if (!filterN) filterIndex = nullptr;
        else filterIndex = new size_t [filterN];
    }
    for (size_t i = 0; i < filterN; ++i) {
        filterIndex[i] = (object.filterIndex)[i];
    }
    rawMat = object.rawMat;
    filterMat = object.filterMat;
    rawN = object.rawN;
    eigenVec = object.eigenVec;
    eigenVal = object.eigenVal;
    isValid = object.isValid;
    isEigen = object.isEigen;
}

void MML::Kinship::operator=(const Kinship &object)
{
    copy(object);
}

MML::Covariates::Covariates(size_t inSampleNumber, size_t inFactorNumber, const double* inData,
                            const size_t* inFilterIndex, size_t inFilterNum)
    : Covariates()
{
    read(inSampleNumber, inFactorNumber, inData, inFilterIndex, inFilterNum);
}

MML::Covariates::Covariates(size_t inSampleNumber, const size_t* inFilterIndex, size_t inFilterNum)
    : Covariates()
{
    read(inSampleNumber, inFilterIndex, inFilterNum);
}

MML::Covariates::Covariates()
{
    rawN = 0;
    filterN = 0;
    factorN = 0;
    filterIndex = nullptr;
    isValid = false;
}

MML::Covariates::Covariates(const Covariates &object)
    : Covariates()
{
    copy(object);
}

MML::Covariates::~Covariates()
{
    if (filterIndex) delete [] filterIndex;
}

bool MML::Covariates::read(size_t inSampleNumber, size_t inFactorNumber, const double* inData,
                           const size_t* inFilterIndex, size_t inFilterNum)
{
    if (!inData || inSampleNumber == 0 || inFactorNumber == 0 || inFilterNum == 0) {
        rawN = 0;
        filterN = 0;
        factorN = 0;
        filterIndex = nullptr;
        isValid = false;
        return false;
    }
    if (filterIndex) delete [] filterIndex;
    rawMat.setData(inSampleNumber, inFactorNumber, inData);
    filterMat = rawMat;
    filterMat.setRows(inFilterNum, inFilterIndex);
    rawN = inSampleNumber;
    factorN = inFactorNumber;
    filterN = inFilterNum;
    filterIndex = new size_t [inFilterNum];
    for (size_t i = 0; i < inFilterNum; ++i) {
        filterIndex[i] = inFilterIndex[i];
    }
    isValid = true;
    return true;
}

bool MML::Covariates::read(size_t inSampleNumber, const size_t* inFilterIndex, size_t inFilterNum)
{
    if (inSampleNumber == 0 || inFilterNum == 0) {
        isValid = false;
        return false;
    }
    if (filterIndex) delete [] filterIndex;
    for (size_t i = 0; i < inFilterNum; ++i) {
        filterIndex[i] = inFilterIndex[i];
    }
    rawN = inSampleNumber;
    filterN = inFilterNum;
    factorN = 1;
    rawMat.setData(inSampleNumber, 1, 1);
    filterMat.setData(inFilterNum, 1, 1);
    isValid = true;
    return true;
}

const MML::Mat& MML::Covariates::getFilterMat() const
{
    return filterMat;
}

size_t MML::Covariates::getFilterNum() const
{
    return filterN;
}

size_t MML::Covariates::getFactorNum() const
{
    return factorN;
}

const size_t* MML::Covariates::getFilterIndex() const
{
    return filterIndex;
}

void MML::Covariates::copy(const Covariates &object)
{
    if (filterN != object.filterN) {
        if (filterIndex) delete [] filterIndex;
        filterN = object.filterN;
        if (!filterN) filterIndex = nullptr;
        else filterIndex = new size_t [filterN];
    }
    for (size_t i = 0; i < filterN; ++i) {
        filterIndex[i] = (object.filterIndex)[i];
    }
    rawMat = object.rawMat;
    filterMat = object.filterMat;
    rawN = object.rawN ;
    factorN = object.factorN;
    isValid = object.isValid;
}

void MML::Covariates::operator=(const Covariates &object)
{
    copy(object);
}

MML::Distribution::Distribution(size_t inBetaIterN)
    : betaIterN(inBetaIterN)
{
    dataNA = DATA_NA;
    eps = EPS;
    tiny = 1e-200;
    pi = PI;
    betaError = 1e-100;
    maxIterNum = 1000;
}

MML::Distribution::Distribution()
    : Distribution(10000)
{
}

MML::Distribution::Distribution(const Distribution &object)
    : Distribution()
{
    copy(object);
}

MML::Distribution::~Distribution()
{

}

double MML::Distribution::gamma(double x)
{
    // return std::exp(std::lgamma(x));
    return gammaMod.gamma(x);
}

double MML::Distribution::lgamma(double x)
{
    // return std::lgamma(x);
    return gammaMod.lgamma(x);
}

double MML::Distribution::digamma(double x)
{
    return gammaMod.digamma(x);
}

double MML::Distribution::polygamma(size_t k, double x)
{
    return gammaMod.polygamma(k, x);
}

double MML::Distribution::igammap(double a, double x)
{
    return gammaMod.impGammaP(a, x);
}

double MML::Distribution::igammaq(double a, double x)
{
    return gammaMod.impGammaQ(a, x);
}

double MML::Distribution::beta(double a, double b)
{
    return betaMod.beta(a, b);
}

double MML::Distribution::ibeta(double a, double b, double x)
{
    return betaMod.impBeta(a, b, x);
}

double MML::Distribution::lbeta(double a, double b)
{
    return betaMod.lBeta(a, b);
}

double MML::Distribution::tcdf(double t, double df)
{
    double x = df /(t * t + df); //x ~ Ix(0.5*df,0.5)
    return 1 - 0.5 * ibeta(0.5 * df, 0.5, x);
}

double MML::Distribution::tcdfTri(double t, double df)
{
    double x = df / (t * t + df); //x ~ Ix(0.5*df,0.5)
    return 0.5 * ibeta(0.5 * df, 0.5, x);
}

double MML::Distribution::tPvalue(double t, double df)
{
    double x = df / (t * t + df); //x ~ Ix(0.5*df,0.5)
    return 0.5 * ibeta(0.5 * df, 0.5, x);
}

double MML::Distribution::tPvalue2(double t, double df)
{
    double x = df / (t * t + df);
    return ibeta(0.5 * df, 0.5, x);
}

double MML::Distribution::fcdf(double f, double df1, double df2)
{
    double x = df1 * f / (df1 * f + df2); //x ~ Ix(0.5*df1,0.5*df2)
    return ibeta(0.5 * df1, 0.5 * df2, x);
}

double MML::Distribution::fcdfTri(double f, double df1, double df2)
{
    double x = df1 * f / (df1 * f + df2);
    return ibeta(0.5 * df2, 0.5 * df1, 1.0 - x);
}

bool MML::Distribution::genGammaRandom(double alpha, double beta, double *out, size_t n)
{
    if (n == 0) return false;
    std::default_random_engine generator(std::chrono::steady_clock::now().time_since_epoch().count());
    std::gamma_distribution<double> distribution(alpha, beta);
    size_t i = 0;
    for (i = 0; i < n; ++i) {
        out[i] = distribution(generator);
    }
    return true;
}

double MML::Distribution::genGammaRandom(double alpha, double beta)
{
    std::gamma_distribution<double> distribution(alpha, beta);
    std::default_random_engine generator(std::chrono::steady_clock::now().time_since_epoch().count());
    return distribution(generator);
}

bool MML::Distribution::genDirichletRandom(double *para, double *out, size_t n)
{
    if (n == 0) return false;
    size_t i = 0;
    double sum = 0.0;
    for (i = 0; i < n; ++i) {
        out[i] = genGammaRandom(para[i], 1);
        sum += out[i];
    }
    for (i = 0; i < n; ++i) {
        out[i] /= sum;
    }
    return true;
}

bool MML::Distribution::genPPoints(long long n, double *out)
{
    long long i = 1;
    if (n <= 0) {
        return false;
    } else if (n <= 10) {
        for(i = 1; i <= n; ++i) {
            out[i - 1] = (i - 0.375) / (n + 0.25);
        }
        return true;
    } else {
        for(i = 1; i <= n; ++i) {
            out[i - 1] = (i - 0.5) / (n);
        }
        return true;
    }
    return true;
}

bool MML::Distribution::genNegLog10PPoints(long long n, double *out)
{
    long long i = 1;
    if (n <= 0) {
        return false;
    } else if (n <= 10) {
        for(i = 1; i <= n; ++i) {
            out[i - 1] = -std::log10((i - 0.375) / (n + 0.25));
        }
        return true;
    } else {
        for (i = 1; i <= n; ++i) {
            out[i - 1] = -std::log10((i - 0.5) / (n));
        }
        return true;
    }
    return true;
}

void MML::Distribution::copy(const Distribution &object)
{
    dataNA = object.dataNA;
    eps = object.eps;
    pi = object.pi;
    maxIterNum = object.maxIterNum;

    betaIterN = object.betaIterN;
    betaError = object.betaError;

    gammaMod = object.gammaMod;
    betaMod = object.betaMod;
}

const MML::Distribution& MML::Distribution::operator=(const Distribution &object)
{
    copy(object);
    return *this;
}

bool MML::CopyDataIMatToMat(const IMat &inMat, Mat &outMat)
{
    if (inMat.getMatClass() == MML::_null) {
        return false;
    }
    outMat.clear();
    size_t nCol = inMat.getNCol();
    size_t nRow = inMat.getNRow();
    outMat.setData(nRow, nCol, (double)0, inMat.getMatClass());
    for (size_t i = 0; i < nRow; ++i) {
        for (size_t j = 0; j < nCol; ++j) {
            if (inMat(i, j) == UNASSIGNED) {
                outMat(i, j) = DATA_NA;
            } else {
                outMat(i, j) = (double)inMat(i, j);
            }
        }
    }
    return true;
}

bool MML::MulDataMat(Mat &inMat, double val)
{
    if (inMat.getMatClass() == MML::_null) {
        return false;
    }
    size_t nCol = inMat.getNCol();
    size_t nRow = inMat.getNRow();
    for (size_t i = 0; i < nRow; ++i) {
        for (size_t j = 0; j < nCol; ++j) {
            if (inMat(i, j) != DATA_NA) {
                inMat(i, j) = inMat(i, j) * val;
            }
        }
    }
    return true;
}

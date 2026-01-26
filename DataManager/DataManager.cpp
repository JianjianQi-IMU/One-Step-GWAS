#include "DataManager.hpp"
#include <cmath>

MML::Phenotype::Phenotype()
{
    dataNA = DATA_NA;
    eps = EPS;
    isValid = false;
}

MML::Phenotype::Phenotype(const Phenotype &object) :Phenotype()
{
    copy(object);
}

MML::Phenotype::Phenotype(uint64_t inSampleNumber, const double* inData)
    : Phenotype()
{
    read(inSampleNumber, inData);
}

MML::Phenotype::~Phenotype()
{

}

bool MML::Phenotype::read(uint64_t inSampleNumber, const double* inData)
{
    if (!inData || inSampleNumber == 0) {
        return false;
    }
    rawData.resize(inSampleNumber, 1);
    rawData.setMatClass(MML::_colvec);
    uint64_t filterN = 0;
    for (uint64_t i = 0; i < inSampleNumber; ++i) {
        rawData(i) = inData[i];
        if (rawData(i) != dataNA) {
            ++filterN;
        }
    }
    if (filterN == 0) {
        isValid = false;
        return false;
    }
    uint64_t *filterIndex = new uint64_t [filterN];
    uint64_t j = 0;
    for (uint64_t i = 0; i < inSampleNumber; ++i) {
        if (rawData(i) != dataNA) {
            filterIndex[j] = i;
            ++j;
        }
    }
    filterList.SetData(filterIndex, filterN);
    filterData = rawData;
    filterData.setRows(filterN, filterList.GetFilterIdx());
    isValid = true;
    delete [] filterIndex;
    filterIndex = nullptr;
    return true;
}

bool MML::Phenotype::filterMarkers(const Mat& inVec, Mat& outVec)
{
    if (!isValid) {
        return false;
    }
    uint64_t i, validN = 0;
    uint64_t filterN = filterList.Length();
    double S = 0, E;
    outVec.resize(filterN, 1);
    outVec.info = _colvec;
    bool good = false;
    filterList.FilterVec(inVec, outVec);
    double last = outVec(0);
    for (i = 0; i < filterN; ++i) {
        if (outVec(i) != dataNA) {
            if (last != dataNA && std::fabs(last - outVec(i)) > eps) {
                good = true;
            }
            last = outVec(i);
            S += outVec(i);
            ++validN;
        }
    }
    if (!good) {
        return false;
    }
    E = S / validN;
    for (i = 0; i < filterN; ++i) {
        if (outVec(i) == dataNA) {
            outVec(i) = E;
        }
    }
    return true;
}

const MML::Mat &MML::Phenotype::getFilterVec() const
{
    return filterData;
}

const MML::DataFilter<uint64_t> &MML::Phenotype::getFilterList() const
{
    return filterList;
}

uint64_t MML::Phenotype::getRawNum() const
{
    return rawData.getNRow();
}

uint64_t MML::Phenotype::getFilterNum() const
{
    return filterList.Length();
}

void MML::Phenotype::copy(const Phenotype &object)
{
    dataNA = object.dataNA;
    eps = object.eps;
    rawData = object.rawData;
    filterList = object.filterList;
    isValid = object.isValid;
    filterData = object.filterData;
}

void MML::Phenotype::operator=(const Phenotype &object)
{
    copy(object);
}

MML::Kinship::Kinship(uint64_t inSampleNumber, const double* inData, const DataFilter<uint64_t> &inFilterList)
    : Kinship()
{
    read(inSampleNumber, inData, inFilterList);
}

MML::Kinship::Kinship()
{
    isValid = false;
    isEigen = false;
}

MML::Kinship::Kinship(const Kinship &object)
    : Kinship()
{
    copy(object);
}

MML::Kinship::~Kinship()
{

}

bool MML::Kinship::read(uint64_t inSampleNumber, const double* inData, const DataFilter<uint64_t> &inFilterList)
{
    if (!inData || inSampleNumber == 0 || inFilterList.Length() == 0) {
        isValid = false;
        isEigen = false;
        return false;
    }

    uint64_t rawN = inSampleNumber;
    filterList = inFilterList;
    rawMat.setData(rawN, rawN, inData);
    filterMat = rawMat;
    filterMat.setCols(filterList.Length(), filterList.GetFilterIdx());
    filterMat.setRows(filterList.Length(), filterList.GetFilterIdx());
    filterMat.toSym('L');
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

uint64_t MML::Kinship::getFilterNum() const
{
    return filterList.Length();
}

const MML::Mat& MML::Kinship::getEigenVec() const
{
    return eigenVec;
}

const MML::Mat& MML::Kinship::getEigenVal() const
{
    return eigenVal;
}

const MML::DataFilter<uint64_t> &MML::Kinship::getFilterList() const
{
    return filterList;
}

void MML::Kinship::copy(const Kinship &object)
{
    rawMat = object.rawMat;
    filterMat = object.filterMat;
    eigenVec = object.eigenVec;
    eigenVal = object.eigenVal;
    isValid = object.isValid;
    isEigen = object.isEigen;
    filterList = object.filterList;
}

void MML::Kinship::operator=(const Kinship &object)
{
    copy(object);
}

MML::Covariates::Covariates(uint64_t inSampleNumber, uint64_t inFactorNumber, const double* inData, const DataFilter<uint64_t> &inFilterList)
    : Covariates()
{
    read(inSampleNumber, inFactorNumber, inData, inFilterList);
}

MML::Covariates::Covariates(uint64_t inSampleNumber, const DataFilter<uint64_t> &inFilterList)
    : Covariates()
{
    read(inSampleNumber, inFilterList);
}

MML::Covariates::Covariates()
{
    isValid = false;
}

MML::Covariates::Covariates(const Covariates &object)
    : Covariates()
{
    copy(object);
}

MML::Covariates::~Covariates()
{

}

bool MML::Covariates::read(uint64_t inSampleNumber, uint64_t inFactorNumber, const double* inData, const DataFilter<uint64_t> &inFilterList)
{
    if (!inData || inSampleNumber == 0 || inFactorNumber == 0 || inFilterList.Length() == 0) {
        isValid = false;
        return false;
    }
    rawMat.setData(inSampleNumber, inFactorNumber, inData);
    filterMat = rawMat;
    filterList = inFilterList;
    filterMat.setRows(filterList.Length(), filterList.GetFilterIdx());
    isValid = true;
    return true;
}

bool MML::Covariates::read(uint64_t inSampleNumber, const DataFilter<uint64_t> &inFilterList)
{
    if (inSampleNumber == 0 || inFilterList.Length() == 0) {
        isValid = false;
        return false;
    }
    filterList = inFilterList;
    rawMat.setData(inSampleNumber, 1, 1);
    filterMat.setData(filterList.Length(), 1, 1);
    isValid = true;
    return true;
}

const MML::Mat& MML::Covariates::getFilterMat() const
{
    return filterMat;
}

uint64_t MML::Covariates::getFilterNum() const
{
    return filterList.Length();
}

uint64_t MML::Covariates::getFactorNum() const
{
    return rawMat.getNCol();
}

const MML::DataFilter<uint64_t> &MML::Covariates::getFilterList() const
{
    return filterList;
}

void MML::Covariates::copy(const Covariates &object)
{
    rawMat = object.rawMat;
    filterMat = object.filterMat;
    isValid = object.isValid;
    filterList = object.filterList;
}

void MML::Covariates::operator=(const Covariates &object)
{
    copy(object);
}

MML::Distribution::Distribution(uint64_t inBetaIterN)
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

double MML::Distribution::polygamma(uint64_t k, double x)
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

bool MML::Distribution::genGammaRandom(double alpha, double beta, double *out, uint64_t n)
{
    if (n == 0) return false;
    std::default_random_engine generator(std::chrono::steady_clock::now().time_since_epoch().count());
    std::gamma_distribution<double> distribution(alpha, beta);
    uint64_t i = 0;
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

bool MML::Distribution::genDirichletRandom(double *para, double *out, uint64_t n)
{
    if (n == 0) return false;
    uint64_t i = 0;
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

bool MML::CopyDataIMatToMat(const SIMat &inMat, Mat &outMat)
{
    if (inMat.getMatClass() == MML::_null) {
        return false;
    }
    outMat.clear();
    uint64_t nCol = inMat.getNCol();
    uint64_t nRow = inMat.getNRow();
    outMat.setData(nRow, nCol, (double)0, inMat.getMatClass());
    for (uint64_t i = 0; i < nRow; ++i) {
        for (uint64_t j = 0; j < nCol; ++j) {
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
    uint64_t nCol = inMat.getNCol();
    uint64_t nRow = inMat.getNRow();
    for (uint64_t i = 0; i < nRow; ++i) {
        for (uint64_t j = 0; j < nCol; ++j) {
            if (inMat(i, j) != DATA_NA) {
                inMat(i, j) = inMat(i, j) * val;
            }
        }
    }
    return true;
}

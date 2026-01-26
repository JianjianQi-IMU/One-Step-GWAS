#ifndef DATAMANAGER_DATA_MANAGER_HPP
#define DATAMANAGER_DATA_MANAGER_HPP

#include "DataManager/MatrixLib.hpp"
#include "DataManager/MathSpecialFunc.hpp"
#include <random>
#include <chrono>

//#define DATA_NA -1e300
//#define EPS     1e-6
//#define PI      3.1415926535898

namespace MML
{

template<typename IDX_CLASS>
class DataFilter
{
private:
    IDX_CLASS *filterIdx;
    IDX_CLASS n;
protected:
    void copy(const DataFilter& object)
    {
        if (filterIdx != nullptr && n != 0) {
            delete [] filterIdx;
        }
        filterIdx = nullptr;
        n = 0;
        if (object.filterIdx != nullptr && object.n != 0) {
            n = object.n;
            filterIdx = new IDX_CLASS [n];
            for (IDX_CLASS i = 0; i < n; ++i) {
                filterIdx[i] = (object.filterIdx)[i];
            }
        }
    }
public:
    DataFilter()
    {
        filterIdx = nullptr;
        n = 0;
    }
    DataFilter(const DataFilter& object) : DataFilter()
    {
        if (object.filterIdx != nullptr && object.n != 0) {
            n = object.n;
            filterIdx = new IDX_CLASS [n];
            for (IDX_CLASS i = 0; i < n; ++i) {
                filterIdx[i] = (object.filterIdx)[i];
            }
        } else {
            if (filterIdx != nullptr && n != 0) {
                delete [] filterIdx;
            }
            filterIdx = nullptr;
            n = 0;
        }
    }
    DataFilter(DataFilter&& object) : DataFilter()
    {
        if (filterIdx != nullptr && n != 0) {
            delete [] filterIdx;
        }
        filterIdx = nullptr;
        n = 0;
        if (object.filterIdx != nullptr && object.n != 0) {
            n = object.n;
            filterIdx = object.filterIdx;
            object.n = 0;
            object.filterIdx = nullptr;
        }
    }
    ~DataFilter()
    {
        if (filterIdx != nullptr && n != 0) {
            delete [] filterIdx;
        }
        filterIdx = nullptr;
        n = 0;
    }
    void operator=(const DataFilter& object)
    {
        copy(object);
    }
    bool SetData(const IDX_CLASS *inIdx, IDX_CLASS num)
    {
        if (inIdx == nullptr || num == 0) {
            return false;
        }
        if (filterIdx != nullptr && n != 0) {
            delete [] filterIdx;
        }
        n = num;
        filterIdx = new IDX_CLASS [n];
        for (IDX_CLASS i = 0; i < n; ++i) {
            filterIdx[i] = inIdx[i];
        }
        return true;
    }
    IDX_CLASS Length() const
    {
        return n;
    }
    const IDX_CLASS *GetFilterIdx() const
    {
        return filterIdx;
    }
    bool FilterVec(const SIMat& inVec, SIMat& outVec) const
    {
        
        MML::matClass info = inVec.getMatClass();
        if (info != MML::_colvec && info != MML::_rowvec) {
            return false;
        }
        if (info == MML::_colvec) {
            outVec.resize(n, 1);
        } else {
            outVec.resize(1, n);
        }
        outVec.setMatClass(info);
        for (IDX_CLASS i = 0; i < n; ++i) {
            outVec(i) = inVec(filterIdx[i]);
        }
        return true;
    }
    bool FilterVec(const Mat& inVec, Mat& outVec) const
    {
        
        MML::matClass info = inVec.getMatClass();
        if (info != MML::_colvec && info != MML::_rowvec) {
            return false;
        }
        if (info == MML::_colvec) {
            outVec.resize(n, 1);
        } else {
            outVec.resize(1, n);
        }
        outVec.setMatClass(info);
        for (IDX_CLASS i = 0; i < n; ++i) {
            outVec(i) = inVec(filterIdx[i]);
        }
        return true;
    }
    bool FilterMatRow(const Mat& inMat, Mat& outMat) const
    {
        MML::matClass info = inMat.getMatClass();
        if (info == MML::_null) {
            return false;
        }
        IDX_CLASS ncol = inMat.getNCol();
        outMat.resize(n, ncol);
        for (IDX_CLASS i = 0; i < n; ++i) {
            for (IDX_CLASS j = 0; j < ncol; ++j) {
                outMat(i, j) = inMat(filterIdx[i], j);
            }
        }
        return true;
    }
    bool FilterMatCol(const Mat& inMat, Mat& outMat) const
    {
        MML::matClass info = inMat.getMatClass();
        if (info == MML::_null) {
            return false;
        }
        IDX_CLASS nrow = inMat.getNRow();
        outMat.resize(nrow, n);
        for (IDX_CLASS i = 0; i < n; ++i) {
            for (IDX_CLASS j = 0; j < nrow; ++j) {
                outMat(j, i) = inMat(j, filterIdx[i]);
            }
        }
        return true;
    }
};

class Phenotype
{
public:
    double dataNA;
    double eps;
    Mat rawData;
    DataFilter<uint64_t> filterList;
    Mat filterData;
    bool isValid;
    Phenotype();
    Phenotype(const Phenotype& object);
    Phenotype(uint64_t inSampleNumber, const double* inData);
    ~Phenotype();
    bool read(uint64_t inSampleNumber, const double* inData);
    bool filterMarkers(const Mat& inVec, Mat& outVec);
    const Mat &getFilterVec() const;
    const DataFilter<uint64_t> &getFilterList() const;
    uint64_t getRawNum() const;
    uint64_t getFilterNum() const;
    void copy(const Phenotype& object);
    void operator=(const Phenotype& object);
};

class Kinship
{
public:
    Mat rawMat;
    Mat filterMat;
    DataFilter<uint64_t> filterList;
    Mat eigenVec;
    Mat eigenVal;
    bool isValid;
    bool isEigen;
    Kinship(uint64_t inSampleNumber, const double* inData, const DataFilter<uint64_t> &inFilterList);
    Kinship();
    Kinship(const Kinship& object);
    ~Kinship();
    bool read(uint64_t inSampleNumber, const double* inData, const DataFilter<uint64_t> &inFilterList);

    bool eigen();
    const Mat& getFilterMat() const;
    uint64_t getFilterNum() const;
    const Mat& getEigenVec() const;
    const Mat& getEigenVal() const;
    const DataFilter<uint64_t> &getFilterList() const;
    void copy(const Kinship& object);
    void operator=(const Kinship& object);
};

class Covariates
{
public:
    Mat rawMat;
    Mat filterMat;
    DataFilter<uint64_t> filterList;
    bool isValid;
    Covariates(uint64_t inSampleNumber, uint64_t inFactorNumber, const double* inData, const DataFilter<uint64_t> &inFilterList);
    Covariates();
    Covariates(const Covariates& object);
    Covariates(uint64_t inSampleNumber, const DataFilter<uint64_t> &inFilterList);
    ~Covariates();
    bool read(uint64_t inSampleNumber, uint64_t inFactorNumber, const double* inData, const DataFilter<uint64_t> &inFilterList);
    bool read(uint64_t inSampleNumber, const DataFilter<uint64_t> &inFilterList);
    const Mat& getFilterMat() const;
    uint64_t getFilterNum() const;
    uint64_t getFactorNum() const;
    const DataFilter<uint64_t> &getFilterList() const;
    void copy(const Covariates& object);
    void operator=(const Covariates& object);
};

class Distribution
{
private:
    GammaFuncMod gammaMod;
    BetaFuncMod  betaMod;
public:
    double dataNA;
    double eps;
    double tiny;
    double pi;
    uint64_t maxIterNum;
    uint64_t betaIterN;
    double betaError;
    Distribution(uint64_t inBetaIterN);
    Distribution();
    Distribution(const Distribution& object);
    ~Distribution();
    double gamma(double x);
    double lgamma(double x);
    double digamma(double x);
    double polygamma(uint64_t k, double x);
    double igammap(double a, double x);
    double igammaq(double a, double x);
    double beta(double a, double b);
    double ibeta(double a, double b, double x);
    double lbeta(double a, double b);
    double tcdf(double t, double df);
    double tcdfTri(double t, double df);
    double tPvalue(double t, double df);
    double tPvalue2(double t, double df);
    double fcdf(double f, double df1, double df2);
    double fcdfTri(double f, double df1, double df2); // = 1.0 - fcdf()
    bool genGammaRandom(double alpha, double beta, double* out, uint64_t n = 1);
    double genGammaRandom(double alpha, double beta);
    bool genDirichletRandom(double* para, double* out, uint64_t n);
    bool genPPoints(long long n, double* out); // like ppoints function in stats module in R
    bool genNegLog10PPoints(long long n, double* out); // = -log10( genPPoints(n, out) )
    void copy(const Distribution& object);
    const Distribution& operator=(const Distribution& object);
};

bool CopyDataIMatToMat(const SIMat &inMat, Mat &outMat);
bool MulDataMat(Mat &inMat, double val);

};

#endif

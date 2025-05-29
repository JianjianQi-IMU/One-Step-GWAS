#ifndef DATA_MANAGER_HPP
#define DATA_MANAGER_HPP

#include "DataManager/MatrixLib.hpp"
#include "DataManager/MathSpecialFunc.hpp"
#include <random>
#include <chrono>

//#define DATA_NA -1e300
//#define EPS     1e-6
//#define PI      3.1415926535898

namespace MML
{

class Phenotype
{
public:
    double dataNA;
    double eps;
    double* rawData;
    double* filterData;
    size_t* filterIndex;
    Mat filterVec;
    size_t rawN;
    size_t filterN;
    bool isValid;
    Phenotype();
    Phenotype(const Phenotype& object);
    Phenotype(size_t inSampleNumber, const double* inData);
    ~Phenotype();
    bool read(size_t inSampleNumber, const double* inData);
    bool filterMarkers(const Mat& inVec, Mat& outVec);
    const Mat& getFilterVec() const;
    const size_t* getFilterIndex() const;
    size_t getFilterNum() const;
    void copy(const Phenotype& object);
    void operator=(const Phenotype& object);
};

class Kinship{
public:
    Mat rawMat;
    Mat filterMat;
    size_t* filterIndex;
    size_t rawN;
    size_t filterN;
    Mat eigenVec;
    Mat eigenVal;
    bool isValid;
    bool isEigen;
    Kinship(size_t inSampleNumber, const double* inData, const size_t* inFilterIndex, size_t inFilterNum);
    Kinship();
    Kinship(const Kinship& object);
    ~Kinship();
    bool read(size_t inSampleNumber, const double* inData, const size_t* inFilterIndex, size_t inFilterNum);

    bool eigen();
    const Mat& getFilterMat() const;
    size_t getFilterNum() const;
    const Mat& getEigenVec() const;
    const Mat& getEigenVal() const;
    const size_t* getFilterIndex() const;
    void copy(const Kinship& object);
    void operator=(const Kinship& object);
};

class Covariates{
public:
    Mat rawMat;
    Mat filterMat;
    size_t* filterIndex;
    size_t rawN;
    size_t factorN;
    size_t filterN;
    bool isValid;
    Covariates(size_t inSampleNumber, size_t inFactorNumber, const double* inData, const size_t* inFilterIndex, size_t inFilterNum);
    Covariates();
    Covariates(const Covariates& object);
    Covariates(size_t inSampleNumber, const size_t* inFilterIndex, size_t inFilterNum);
    ~Covariates();
    bool read(size_t inSampleNumber, size_t inFactorNumber, const double* inData,
              const size_t* inFilterIndex, size_t inFilterNum);
    bool read(size_t inSampleNumber, const size_t* inFilterIndex, size_t inFilterNum);
    const Mat& getFilterMat() const;
    size_t getFilterNum() const;
    size_t getFactorNum() const;
    const size_t* getFilterIndex() const;
    void copy(const Covariates& object);
    void operator=(const Covariates& object);
};

class Distribution{
private:
    GammaFuncMod gammaMod;
    BetaFuncMod  betaMod;
public:
    double dataNA;
    double eps;
    double tiny;
    double pi;
    size_t maxIterNum;
    size_t betaIterN;
    double betaError;
    Distribution(size_t inBetaIterN);
    Distribution();
    Distribution(const Distribution& object);
    ~Distribution();
    double gamma(double x);
    double lgamma(double x);
    double digamma(double x);
    double polygamma(size_t k, double x);
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
    bool genGammaRandom(double alpha, double beta, double* out, size_t n = 1);
    double genGammaRandom(double alpha, double beta);
    bool genDirichletRandom(double* para, double* out, size_t n);
    bool genPPoints(long long n, double* out); // like ppoints function in stats module in R
    bool genNegLog10PPoints(long long n, double* out); // = -log10( genPPoints(n, out) )
    void copy(const Distribution& object);
    const Distribution& operator=(const Distribution& object);
};

bool CopyDataIMatToMat(const IMat &inMat, Mat &outMat);
bool MulDataMat(Mat &inMat, double val);

};

#endif

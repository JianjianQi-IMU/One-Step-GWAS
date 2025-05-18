#ifndef GEMMA_HPP
#define GEMMA_HPP

#include "DataManager/DataManager.hpp"
#include <string>
#include <QDebug>

namespace GEMMA
{

class NewtonPara
{
public:
    double min;
    double max;
    size_t n;
    size_t maxTimes;
    double* limList;
    double* dll1List;
    double* uList;
    double* dList;
    NewtonPara(double iMin, double iMax, size_t iN, size_t iMaxTimes);
    NewtonPara();
    NewtonPara(const NewtonPara& inPara);
    ~NewtonPara();
    void copy(const NewtonPara& object);
    const NewtonPara& operator=(const NewtonPara& inPara);
};

class VecList
{
public:
    double** data;
    bool isValid;
    size_t vecN;
    size_t vecLen;
    VecList();
    VecList(const VecList& inVecList);
    VecList(size_t inVecN, size_t inVecLen);
    ~VecList();
    bool resize(size_t inVecN, size_t inVecLen);
    size_t getVecN();
    size_t getVecLen();
    void copy(const VecList& object);
    double* operator()(size_t Index);
    double& operator()(size_t Index1, size_t Index2);
    const double& operator()(size_t Index1, size_t Index2) const;
    const VecList& operator=(const VecList& inVecList);
};

class MLM
{
public:
    double eps;
    double pi;
    size_t factorN;
    size_t filterN;
    MML::Mat Ut;
    MML::Mat UtK;
    MML::Mat Uty;
    MML::Mat delta;
    VecList UaUb;
    NewtonPara Lpara;
    bool isValid;
    MLM(const MML::Phenotype& inPhe, const MML::Kinship& inKin, const MML::Covariates& inCov);
    MLM();
    ~MLM();
    bool read(const MML::Phenotype& inPhe, const MML::Kinship& inKin, const MML::Covariates& inCov);
    bool initUaUb();
    bool analyze(const MML::Mat& Utx, double& outBetax, double& outFstat);
    bool analyze(const double* Utx, double& outBetax, double& outFstat);
    bool updateUaUb(const MML::Mat& Utx);
    bool updateUaUb(const double* Utx);
    bool calcuPatb(const MML::Mat& D,MML::Mat& atPb);
    bool calcuPPatb(const MML::Mat& D, const MML::Mat& atPb, MML::Mat& atPPb);
    bool calcuPPPatb(const MML::Mat& D, const MML::Mat& atPb, const MML::Mat& atPPb, MML::Mat& atPPPb);
    bool calcuIatb(MML::Mat& atIb);
    bool calcuTrP(double lambda, const MML::Mat& atPb, const MML::Mat& atPPb, double& outTrP);
    bool calcuTrPP(double lambda, const MML::Mat& atPb, const MML::Mat& atPPb, const MML::Mat& atPPPb, double& outTrPP);
    bool calcuREMLLogL(double lambda, const MML::Mat& atPb, const MML::Mat& atIb, double& outLogL);
    bool calcuREMLDLogL1(double lambda, double trP, const MML::Mat& atPb, const MML::Mat& atPPb, double& outDLogL1);
    bool calcuREMLDLogL2(double lambda, double trP, double trPP, const MML::Mat& atPb,
                         const MML::Mat& atPPb, const MML::Mat& atPPPb,
                         double& outDLogL2);
    bool calcuLambda(double& outLambda, double& outLL);
    bool calcuFWald(double lambda, double& outBetax, double& outFWald);
    static bool centerKin(const MML::Mat& inMat, MML::Mat& outMat);
};

class GLM
{
public:
    double eps;
    size_t factorN;
    size_t filterN;
    bool isValid;
    MML::Mat K;
    MML::Mat Kt;
    MML::Mat y;
    MML::Mat KtK;
    MML::Mat KtKi;
    MML::Mat Kty;
    MML::Mat KtKiKty;
    double yPKy;
    GLM(const MML::Phenotype& inPhe,
        const MML::Covariates& inCov);
    GLM();
    ~GLM();
    bool read(const MML::Phenotype& inPhe,
              const MML::Covariates& inCov);
    bool analyze(const MML::Mat& x, double& outBetax, double& outFstat);
};

};
#endif

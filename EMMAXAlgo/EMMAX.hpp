#ifndef EMMAX_HPP
#define EMMAX_HPP

#include "DataManager/DataManager.hpp"
#include <cmath>
//#include "FastPCA.hpp"

namespace EMMAX
{

class REML
{
public:
    double eps;
    double pi;
    MML::Mat rawEigenVec;
    MML::Mat rawEigenVal;
    MML::Mat filterEigenVec;
    MML::Mat filterEigenVal;
    uint64_t filterN;
    uint64_t factorN;
    bool isValid;
    REML();
    REML(const MML::Kinship& inKin, const MML::Covariates& inCov);
    ~REML();
    bool sortEigen(uint64_t* outIndex, uint64_t inNum);
    void qSortEigen(uint64_t a, uint64_t b, uint64_t* outIndex);
    bool read(const MML::Kinship& inKin, const MML::Covariates& inCov);
    bool getDelta(const MML::Phenotype& inPhe, double& outDelta, double& outLLD);
    bool dLikelihood(const MML::Mat& etas, double delta, double& outVal);
    bool likelihood(const MML::Mat& etas, double delta, double& outVal);
    bool getLL(const MML::Phenotype& inPhe, double inDelta, double& outLL);
};

class Betas{
public:
    double eps;
    MML::Mat Ut; // the eigen vector of kinship matrix 
    MML::Mat UtKx; // = Ut %*% [Cov : markers] = Ut[K:x]
    MML::Mat Uty; // = Ut %*% Phe = Uty
    MML::Mat D; // the eigen values of kinship matrix
    double UtytDiUty; // = t(Uty) %*% Di %*% Uty
    uint64_t filterN;
    uint64_t factorN;
    double delta;
    bool isValid; 
    Betas();
    Betas(const MML::Phenotype& inPhe, const MML::Kinship& inKin, const MML::Covariates& inCov, double inDelta);
    ~Betas();
    bool read(const MML::Phenotype& inPhe, const MML::Kinship& inKin,
        const MML::Covariates& inCov, double inDelta);
    bool calculateTstat(const MML::Mat& inMarkers, double& outBetas0, double& outTstat);
};

};


#endif

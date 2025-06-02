#ifndef KINSHIPCALCULATOR_HPP
#define KINSHIPCALCULATOR_HPP

#include "DataManager/DataManager.hpp"

namespace MML {

enum KinshipMode{
    REL_TYPE = 0,
    COV_TYPE = 1,
    IBS_TYPE = 2,
};

class KinshipParam
{
public:
    size_t nSamples;
    size_t nMarkers;
    size_t nPloid;
    KinshipMode mode;
    explicit KinshipParam();
};

class KinshipCalculator
{
protected:
    Mat kin;
    size_t nPloid;
    size_t nSamples;
    size_t nMarkers;
    size_t nValidMarkers;
    bool isPolyploid;
    KinshipMode mode;
    bool isValid;
public:
    KinshipCalculator();
    virtual ~KinshipCalculator();
    bool standardizeCol(Mat& inMat, double factor = 1.0);
    bool zeroCenterCol(Mat& inMat, double factor = 1.0);
    virtual bool addMat(const IMat& inX);
    virtual bool getKinship(Mat& outMat);
    size_t getNValidMarkers();
    KinshipMode getMode();
    bool setNPloid(size_t inNPloid);
    bool setIsPolyploid(bool inIsPolyploid);
    bool setNSamples(size_t inNSamples);
    bool setMode(KinshipMode inMode);
    void reset();
    void clear();
    void printKin(){
        kin.print();
    }
};

class KinshipIBSMatCalculator : public KinshipCalculator
{
protected:
    bool fillCol(Mat &inMat);
    bool fillColBi(const IMat &inMat, Mat &outMat);

public:
    KinshipIBSMatCalculator();
    ~KinshipIBSMatCalculator();
    bool addMat(const IMat& inX);
    bool getKinship(Mat& outMat);
};




}

#endif // KINSHIPCALCULATOR_HPP

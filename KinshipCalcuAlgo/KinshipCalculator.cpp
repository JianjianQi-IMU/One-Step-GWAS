#include "KinshipCalculator.hpp"

#include <QDebug>

namespace MML {

KinshipParam::KinshipParam()
{
    nMarkers = 0;
    nSamples = 0;
    nPloid = 0;
    mode = REL_TYPE;
}

KinshipCalculator::KinshipCalculator()
{
    mode = REL_TYPE;
    nSamples = 0;
    nMarkers = 0;
    nValidMarkers = 0;
}

KinshipCalculator::~KinshipCalculator()
{

}

bool KinshipCalculator::standardizeCol(Mat &inMat, double factor)
{
    if (inMat.info == _null) {
        return false;
    }
    size_t nRow = inMat.getNRow(), nCol = inMat.getNCol();
    double sum = 0.0, mean = 0.0, d2 = 0.0, d = 0.0;
    size_t i = 0, j = 0, n = 0;
    for (i = 0; i < nCol; ++i) {
        sum = 0.0;
        n = 0;
        for (j = 0; j < nRow; ++j){
            if (inMat(j, i) != DATA_NA) {
                sum += inMat(j, i);
                ++n;
            }
        }
        if (n == 0) {
            for (j = 0; j < nRow; ++j) {
                inMat(j, i) = 0.0;
            }
            continue;
        }
        mean = sum / n;
        d2 = 0.0;
        for (j = 0; j < nRow; ++j) {
            d2 += (inMat(j, i) - mean) * (inMat(j, i) - mean);
        }
        d2 /= n;
        if (d2 < MML::EPS) {
            for(j = 0; j < nRow; ++j) {
                inMat(j, i) = 0.0;
            }
        } else {
            d = std::sqrt(d2);
            for (j = 0; j < nRow; ++j) {
                if (inMat(j, i) != DATA_NA) {
                    inMat(j, i) = (inMat(j, i) - mean) * factor / d;
                } else {
                    inMat(j, i) = 0.0;
                }
            }
        }
        ++nValidMarkers;
    }
    return true;
}

bool KinshipCalculator::zeroCenterCol(Mat &inMat, double factor)
{
    if (inMat.info == _null) {
        return false;
    }
    size_t nRow = inMat.getNRow(), nCol = inMat.getNCol();
    double sum = 0.0, mean = 0.0;
    size_t i, j, n = 0;
    for (i = 0; i < nCol; ++i) {
        sum = 0.0;
        n = 0;
        for (j = 0; j < nRow; ++j) {
            if (inMat(j, i) != DATA_NA) {
                sum += inMat(j, i);
                ++n;
            }
        }
        if (n == 0) {
            for (j = 0; j < nRow; ++j) {
                inMat(j, i) = 0.0;
            }
            continue;
        }
        mean = sum / n;
        for (j = 0; j < nRow; ++j) {
            if (inMat(j, i) != DATA_NA) {
                inMat(j, i) = (inMat(j, i) - mean) * factor;
            } else {
                inMat(j, i) = 0.0;
            }
        }
        ++nValidMarkers;
    }
    return true;
}

bool KinshipCalculator::addMat(const IMat &inX)
{
    if (inX.getNRow() != nSamples || !isValid) {
        return false;
    }

    int K = inX.getNCol();
    Mat tX;
    MML::CopyDataIMatToMat(inX, tX);
    MML::MulDataMat(tX, 1.0 / nPloid);
    switch (mode) {
        case COV_TYPE:
            zeroCenterCol(tX);
            break;
        case REL_TYPE:
        default:
            standardizeCol(tX);
            break;
    }
    MML::Mat::XXtmul(tX, kin, true);

    nMarkers += K;
    return true;
}

bool KinshipCalculator::getKinship(Mat &outMat)
{
    if (isValid && nMarkers > 0) {
        outMat.resize(nSamples, nSamples);
        for (size_t i = 0; i < nSamples; ++i) {
            for (size_t j = 0 ; j <= i; ++j) {
                outMat(i, j) = kin(i, j) / nValidMarkers;
                outMat(j, i) = outMat(i, j);
            }
        }
        return true;
    }
    return false;
}

size_t KinshipCalculator::getNValidMarkers()
{
    return nValidMarkers;
}

KinshipMode KinshipCalculator::getMode()
{
    return mode;
}

bool KinshipCalculator::setNPloid(size_t inNPloid)
{
    nPloid = inNPloid;
    return true;
}

bool KinshipCalculator::setIsPolyploid(bool inIsPolyploid)
{
    isPolyploid = inIsPolyploid;
    return true;
}

bool KinshipCalculator::setNSamples(size_t inNSamples)
{
    if (inNSamples == 0) {
        return false;
    }
    nSamples = inNSamples;
    isValid = true;
    return kin.setData(nSamples, nSamples, 0.0);
}

bool KinshipCalculator::setMode(KinshipMode inMode)
{
    mode = inMode;
    return true;
}

void KinshipCalculator::reset()
{
    kin.setData(nSamples, nSamples, 0.0);
    nMarkers = 0;
}

void KinshipCalculator::clear()
{
    kin.clear();
    nMarkers = 0;
    nSamples = 0;
    isValid = false;
}

bool KinshipIBSMatCalculator::fillCol(Mat &inMat)
{
    if (inMat.info == _null) {
        return false;
    }
    size_t nRow = inMat.getNRow(), nCol = inMat.getNCol();
    double sum = 0.0, mean = 0.0;
    size_t i = 0, j = 0, n = 0;
    for (i = 0; i < nCol; ++i) {
        sum = 0.0;
        n = 0;
        for (j = 0; j < nRow; ++j){
            if (inMat(j, i) != DATA_NA) {
                sum += inMat(j, i);
                ++n;
            }
        }
        if (n == 0) {
            for (j = 0; j < nRow; ++j) {
                inMat(j, i) = 0.0;
            }
            continue;
        }
        mean = sum / n;
        for (j = 0; j < nRow; ++j) {
            if (inMat(j, i) == DATA_NA) {
                inMat(j, i) = mean;
            }
        }
        ++nValidMarkers;
    }
    return true;
}

bool KinshipIBSMatCalculator::fillColBi(const IMat &inMat, Mat &outMat)
{
    if (inMat.getMatClass() == MML::_null) {
        return false;
    }
    size_t nRow = inMat.getNRow(), nCol = inMat.getNCol();
    double fac = 0.0;
    size_t i = 0, j = 0, n = 0;
    outMat.resize(nRow, 2 * nCol);
    for (i = 0; i < nCol; ++i) {
        fac = 0.0;
        n = 0;
        for (j = 0; j < nRow; ++j) {
            if (inMat(j, i) != MML::UNASSIGNED) {
                ++n;
            }
            if (inMat(j, i) == 0) {
                outMat(j, 2 * i) = -1.0;
                outMat(j, 2 * i + 1) = -1.0;
            } else if (inMat(j, i) == 1) {
                outMat(j, 2 * i) = 1.0;
                outMat(j, 2 * i + 1) = -1.0;
                fac += 1.0;
            }  else if (inMat(j, i) == 2) {
                outMat(j, 2 * i) = 1.0;
                outMat(j, 2 * i + 1) = 1.0;
                fac += 2.0;
            }
        }
        if (n == 0) {
            for (j = 0; j < nRow; ++j) {
                outMat(j, 2 * i) = 0.0;
                outMat(j, 2 * i + 1) = 0.0;
            }
            continue;
        }
        fac /= (2 * n);
        for (j = 0; j < nRow; ++j) {
            if (inMat(j, i) == MML::UNASSIGNED) {
                outMat(j, 2 * i) = 2.0 * fac - 1.0;
                outMat(j, 2 * i + 1) = 2.0 * fac - 1.0;
            }
        }
        ++nValidMarkers;
    }
    return true;
}

KinshipIBSMatCalculator::KinshipIBSMatCalculator()
    : KinshipCalculator()
{
    mode = IBS_TYPE;
}

KinshipIBSMatCalculator::~KinshipIBSMatCalculator()
{

}

bool KinshipIBSMatCalculator::addMat(const IMat &inX)
{
    if (inX.info == _null || inX.getNRow() != nSamples || !isValid) {
        return false;
    }

    int K = inX.ncol;

    if (isPolyploid) {

    } else {

        Mat tX;

        fillColBi(inX, tX);

        MML::Mat::XXtmul(tX, kin, true);
    }
    nMarkers += K;
    return true;
}

bool KinshipIBSMatCalculator::getKinship(Mat &outMat)
{
    if (isValid && nMarkers > 0) {
        outMat.resize(nSamples, nSamples);
        if (isPolyploid) {
            for (size_t i = 0; i < nSamples; ++i) {
                for (size_t j = 0; j <= i; ++j) {
                    outMat(i, j) = 0.0;
                    outMat(j, i) = outMat(i, j);
                }
            }
        } else {
            for (size_t i = 0; i < nSamples; ++i) {
                for (size_t j = 0 ;j <= i; ++j) {
                    outMat(i, j) = 0.5 * kin(i, j) / (2 * nValidMarkers) + 0.5;
                    outMat(j, i) = outMat(i, j);
                }
            }
        }
        return true;
    }
    return false;
}

}

#include "AxisValueCalculator.h"

#include <limits>

bool AxisValueCalculator::pri_calcuAxisData(double inMin, double inMax, double &outStart, double &outStride, double a, double b)
{
    double tmax = inMax, tmin = inMin, range;
    double tstride = 1, tprobe;
    int flag = 0;
    tprobe = tmax;
    range = tmax - tmin;
    if (range / tstride > b) {
        while (range / tstride > b) {
            if (flag % 2 == 0) {
                tstride *= 2;
                flag = (flag + 1) % 2;
            } else {
                tstride *= 5;
                flag = (flag + 1) % 2;
            }
        }
    } else {
        while (range / tstride < a) {
            if (flag % 2 == 0) {
                tstride /= 5;
                flag = (flag + 1) % 2;
            } else {
                tstride /= 2;
                flag = (flag + 1) % 2;
            }
        }
    }
    tprobe = tmin;
    while (tprobe - tstride > tmin) {
        tprobe -= tstride;
    }
    while (tprobe < tmin) {
        tprobe += tstride;
    }
    outStart = tprobe;
    outStride = tstride;
    return true;
}

bool AxisValueCalculator::pri_calcuAxisData0X(double inMax, double &outStride, double a, double b)
{
    double tmax = inMax, tmin = 0,range;
    double tstride = 1;
    int flag = 0;
    range = tmax - tmin;
    if (range / tstride > b) {
        while (range / tstride > b) {
            if (flag % 2 == 0) {
                tstride *= 2;
                flag = (flag + 1) % 2;
            } else {
                tstride *= 5;
                flag = (flag + 1) % 2;
            }
        }
    } else {
        while (range / tstride < a) {
            if (flag % 2 == 0) {
                tstride /= 5;
                flag = (flag + 1) % 2;
            } else {
                tstride /= 2;
                flag = (flag + 1) % 2;
            }
        }
    }
    outStride = tstride;
    return true;
}

AxisValueCalculator::AxisValueCalculator()
    : alphaLimits(4), betaimits(6)
{
}

bool AxisValueCalculator::calcuAxisData(double inMin, double inMax, double &outStart, double &outStride)
{
    return pri_calcuAxisData(inMin, inMax, outStart, outStride, alphaLimits, betaimits);
}

bool AxisValueCalculator::calcuAxisData(double inMin, double inMax, QVector<double>& outValVec)
{
    double tstride,tprobe,val;
    if (!pri_calcuAxisData(inMin, inMax, tprobe, tstride, alphaLimits, betaimits)) {
        return false;
    }
    outValVec.clear();
    for (val = tprobe; val < inMax; val += tstride) {
        outValVec.append(val);
    }
    outValVec.append(val);
    return true;
}

bool AxisValueCalculator::calcuAxisData0X(double inMax, double &outStride)
{
    return pri_calcuAxisData0X(inMax, outStride, alphaLimits, betaimits);
}

bool AxisValueCalculator::calcuAxisData0X(double inMax, QVector<double> &outValVec)
{
    double tstride, val;
    if (!pri_calcuAxisData0X(inMax, tstride, alphaLimits, betaimits)) {
        return false;
    }
    outValVec.clear();
    for (val = 0; val < inMax; val += tstride) {
        outValVec.append(val);
    }
    outValVec.append(val);
    return true;
}

bool AxisValueCalculator::calcuAxisDataStatic(double inMin, double inMax, double &outStart, double &outStride)
{
    return pri_calcuAxisData(inMin, inMax, outStart, outStride);
}

bool AxisValueCalculator::calcuAxisDataStatic(double inMin, double inMax, QVector<double>& outValVec)
{
    double tstride, tprobe, val;
    double eps = std::numeric_limits<double>::epsilon();
    if (!pri_calcuAxisData(inMin, inMax, tprobe, tstride)) {
        return false;
    }
    outValVec.clear();
    for (val = tprobe; val < inMax * (1.0 + eps); val += tstride) {
        outValVec.append(val);
    }
    return true;
}

bool AxisValueCalculator::calcuAxisData0XStatic(double inMax, QVector<double> &outValVec)
{
    double tstride,val;
    double eps = std::numeric_limits<double>::epsilon();
    if (!pri_calcuAxisData0X(inMax, tstride)) {
        return false;
    }
    outValVec.clear();
    for (val = 0; val < inMax * (1.0 + eps); val += tstride) {
        outValVec.append(val);
    }
    return true;
}

bool AxisValueCalculator::calcuAxisData0XStatic(double inMax, double &outStride)
{
    return pri_calcuAxisData0X(inMax, outStride);
}

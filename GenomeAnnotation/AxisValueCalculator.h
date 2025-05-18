#ifndef AXISVALUECALCULATOR_H
#define AXISVALUECALCULATOR_H

#include <QVector>

class AxisValueCalculator
{
private:
    int alphaLimits; // the number of down limitation
    int betaimits; // the number of up limitation
    static bool pri_calcuAxisData(double inMin, double inMax,
                                  double& outStart, double& outStride,
                                  double a = 4, double b = 6);
    static bool pri_calcuAxisData0X(double inMax,
                                    double& outStride,
                                    double a = 4, double b = 6);
public:
    AxisValueCalculator();

    bool calcuAxisData(double inMin, double inMax, double& outStart, double& outStride);
    bool calcuAxisData(double inMin, double inMax, QVector<double>& outValVec);
    bool calcuAxisData0X(double inMax, double& outStride);
    bool calcuAxisData0X(double inMax, QVector<double>& outValVec);

    static bool calcuAxisDataStatic(double inMin, double inMax, double& outStart, double& outStride);
    static bool calcuAxisDataStatic(double inMin, double inMax, QVector<double>& outValVec);
    static bool calcuAxisData0XStatic(double inMax, QVector<double>& outValVec);
    static bool calcuAxisData0XStatic(double inMax, double& outStride);
};

#endif // AXISVALUECALCULATOR_H

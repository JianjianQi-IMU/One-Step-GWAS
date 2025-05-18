#ifndef MATH_SPECIAL_FUNC_H
#define MATH_SPECIAL_FUNC_H

#include <cmath>
#include <limits>
#include "DataManager/DataDefine.hpp"

#include <QDebug>

namespace MML{

// FuncIterator() x0 --> next --> x1 --> next --> x2 ...
template<typename DataType>
class FuncIteratorS {
protected:
    DataType tmpValue;
public:
    FuncIteratorS() {};
    DataType getValue() {
        return tmpValue;
    }
    virtual void next() = 0;
    virtual ~FuncIteratorS() {};
};

// FuncIterator() x0 --> next --> x1 --> next --> x2 ...
template<typename DataType>
class FuncIteratorD {
protected:
    DataType tmpValueA;
    DataType tmpValueB;
public:
    FuncIteratorD() {};
    DataType getValueA() {
        return tmpValueA;
    }
    DataType getValueB() {
        return tmpValueB;
    }
    virtual void next() = 0;
    virtual ~FuncIteratorD() {};
};

template<typename DataType>
class ContinuedFraction1 {
private:
    long long maxIter;
    long long k;
    DataType ender;
    DataType s;
    FuncIteratorD<DataType>* abn;
public:
    ContinuedFraction1() {
        k = 0;
        maxIter = 50;
        ender = std::numeric_limits<DataType>::epsilon();
        abn = nullptr;
    }
    ContinuedFraction1(FuncIteratorD<DataType>* ab) :
        ContinuedFraction1()
    {
        s = 0;
        abn = ab;
    }
    DataType getValue() {
        DataType a;
        DataType b;
        DataType C;
        DataType D;
        DataType delta;
        DataType tiny = 16 * std::numeric_limits<DataType>::min();
        abn->next();
        DataType a1 = abn->getValueA();
        s = abn->getValueB();
        C = s;
        D = 0.0;
        do {
            ++k;
            abn->next();
            a = abn->getValueA(), b = abn->getValueB();
            D = b + a * D;
            if (D == 0.0) {
                D = tiny;
            }
            C = b + a / C;
            if (C == 0.0) {
                C = tiny;
            }
            D = 1.0 / D;
            delta = C * D;
            s = s * delta;
        } while (k < maxIter && std::fabs(delta - 1.0) > ender);
        return a1 / s;
    }
};

template<typename DataType>
class ContinuedFraction2 {
private:
    long long maxIter;
    long long k;
    DataType ender;
    DataType s;
    FuncIteratorD<DataType>* abn;
public:
    ContinuedFraction2() {
        k = 0;
        maxIter = 50;
        ender = std::numeric_limits<DataType>::epsilon();
        abn = nullptr;
    }
    ContinuedFraction2(FuncIteratorD<DataType>* ab) :
        ContinuedFraction2()
    {
        s = 0;
        abn = ab;
    }
    DataType getValue() {
        DataType a;
        DataType b;
        DataType C;
        DataType D;
        DataType delta;
        DataType tiny = 16 * std::numeric_limits<DataType>::min();
        abn->next();
        s = abn->getValueB();
        C = s;
        D = 0.0;
        do {
            ++k;
            abn->next();
            a = abn->getValueA(), b = abn->getValueB();
            D = b + a * D;
            if (D == 0.0) {
                D = tiny;
            }
            C = b + a / C;
            if (C == 0.0) {
                C = tiny;
            }
            D = 1.0 / D;
            delta = C * D;
            s = s * delta;
        } while (k < maxIter && std::fabs(delta - 1.0) > ender);
        qDebug() << k;
        return s;
    }
};

class upperImpGammaFuncIter : public FuncIteratorD<double> {
private:
    double a;
    double z;
    long long ak;
    long long bk;

public:
    upperImpGammaFuncIter() {}
    void setValue(double in_a, double in_x) {
        ak = 0;
        bk = 0;
        a = in_a;
        z = in_x - in_a + 1.0;
    }
    virtual void next() {
        ++ak;
        bk += 2;
        tmpValueA = ak * (a - ak);
        tmpValueB = z + bk;
    }
};

class ImpBetaGreatFuncIter : public FuncIteratorD<double> {
private:
    double a;
    double b;
    double x;
    double x2;
    double a_b;
    long long k;

public:
    ImpBetaGreatFuncIter() {}
    void setValue(double in_a, double in_b, double in_x) {
        k = 0;
        a = in_a;
        b = in_b;
        x = in_x;
        x2 = in_x * in_x;
        a_b = in_a + in_b;
    }
    virtual void next() {
        ++k;

        double kbkx = (k - 1) * (b - k + 1.0) * x;
        double a2k3 = a + k + k - 3.0;

        if(k == 1) {
            tmpValueA = 1;
            tmpValueB = a * (a - a_b * x + 1.0) / (a + 1.0);
        } else {
            tmpValueA = (a + k - 2.0) * (a_b + k - 2.0) * kbkx * x /
                (a2k3 * a2k3);
            tmpValueB = k - 1.0 + kbkx / a2k3 +
                (a + k - 1.0) * (a - a_b * x + 1.0 + (k - 1) * (2.0 - x)) / (a + 2 * k - 1.0);
        }
    }
};

class GammaFuncMod {
private:
    int maxIterNum;
    double dataNA;
    double tiny;
    double eps;
    double pi;
    size_t gammaIterN;
    double* gammaCk;

    size_t lanczosG;
    size_t lanczosN;
    double* lanczosP;

    bool initGammaCk();
    bool initLanczosParam();
    double lowerImpGammaGeneral(double a, double x);
    double upperImpGammaGeneral(double a, double x);
protected:

public:
    GammaFuncMod();
    double gamma(double x);
    double lgamma(double x);
    double digamma(double x);
    double polygamma(size_t k, double x);
    double impGammaP(double a, double x);
    double impGammaQ(double a, double x);
    double smallXUpperImpGamma(double a, double x);
    double lowerImpGamma(double a, double x);
    double upperImpGamma(double a, double x);
    void copy(const GammaFuncMod& object);
    const GammaFuncMod& operator=(const GammaFuncMod& object);
};

class BetaFuncMod {
private:
    int    maxIterNum;
    double dataNA;
    double tiny;
    double eps;
    double pi;

    int n_betaLargeASmallB_pn;

    GammaFuncMod gammaFunc;

    double *accuSum;

    static const int lenAccuSum = 171;
protected:
    void initAccuSum();
public:
    BetaFuncMod();
    ~BetaFuncMod();
    double beta(double a, double b);
    double lBeta(double a, double b);
    double impBetaLargeASmallB(double a, double b, double x);
    double impBetaSmallishBX(double a, double b, double x);
    double impBetaGreat(double a, double b, double x);
    double impBeta(double a, double b, double x);
    void copy(const BetaFuncMod& object);
    const BetaFuncMod& operator=(const BetaFuncMod& object);
};

};


#endif

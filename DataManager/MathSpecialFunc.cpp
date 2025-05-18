#include "MathSpecialFunc.hpp"
#include <QDebug>

namespace MML
{

bool GammaFuncMod::initGammaCk()
{
    /*Spouge's approximation*/
    double factorialK = 1.0;
    if (gammaCk) delete [] gammaCk;
    gammaCk = new double [gammaIterN];
    gammaCk[0] = std::sqrt(2.0 * pi);
    for (size_t i = 1; i < gammaIterN; ++i) {
        gammaCk[i] = std::exp(gammaIterN - i) * std::pow(gammaIterN - i, i - 0.5) / factorialK;
        factorialK *= (-double(i));
    }
    return true;
}

bool GammaFuncMod::initLanczosParam()
{
    /*Lanczos approximation*/
    if (lanczosP) delete [] lanczosP;
    lanczosP = new double [9] {
            0.99999999999980993,
            676.5203681218851,
            -1259.1392167224028,
            771.32342877765313,
            -176.61502916214059,
            12.507343278686905,
            -0.13857109526572012,
            9.9843695780195716e-6,
            1.5056327351493116e-7
        };
    lanczosN = 9;
    lanczosG = 7;
    return true;
}

double GammaFuncMod::lowerImpGammaGeneral(double a, double x)
{
    double prefix;
    double lnPrefix;
    lnPrefix = a * std::log(x) - x;
    prefix = std::exp(lnPrefix);

    double ret = 0.0;
    int nIter = 0;
    double aIter = a;
    double subfrac = 1.0 / aIter;
    double val = prefix * subfrac;
    while (nIter < maxIterNum && std::fabs(val) > tiny) {
        ret += val;
        aIter += 1.0;
        subfrac *= (x / aIter);
        val = prefix * subfrac;
        ++nIter;
    }
    return ret;
}

double GammaFuncMod::upperImpGammaGeneral(double a, double x)
{
    double prefix;
    double lnPrefix;
    lnPrefix = a * std::log(x) - x;
    prefix = std::exp(lnPrefix);

    upperImpGammaFuncIter iter;
    iter.setValue(a, x);
    ContinuedFraction1<double> fracVal(&iter);

    return prefix / (x - a + 1.0 + fracVal.getValue());
}

GammaFuncMod::GammaFuncMod()
{
    dataNA = DATA_NA;
    eps = EPS;
    tiny = 1e-200;
    pi = PI;
    gammaIterN = 10;
    gammaCk = nullptr;
    maxIterNum = 50;
    initGammaCk();
    lanczosP = nullptr;
    initLanczosParam();
}

double GammaFuncMod::gamma(double x)
{
    /*Spouge's approximation*/
    double out = gammaCk[0];
    for (size_t i = 1; i < gammaIterN; ++i) {
        out += gammaCk[i] / (x+i);
    }
    out *= std::exp(-(x + gammaIterN)) * std::pow(x + gammaIterN, x + 0.5);
    return out / x;
}

double GammaFuncMod::lgamma(double x)
{
    /*Spouge's approximation*/
    double out = gammaCk[0];
    for (size_t i = 1; i < gammaIterN; ++i) {
        out += gammaCk[i] / (x + i);
    }
    out = std::log(out) - (x + gammaIterN) + (x + 0.5) * std::log(x + gammaIterN) - std::log(x);
    return out;
}

double GammaFuncMod::digamma(double x)
{
    /*Lanczos approximation*/
    double Ag = lanczosP[0], dAg = 0, out = 0;
    x -= 1.0;
    for (size_t i = 1; i < lanczosN; ++i) {
        Ag += lanczosP[i] / (x + i);
        dAg += lanczosP[i] / ((x + i) * (x + i));
    }
    out = std::log(x + lanczosG + 0.5) - lanczosG / (x + lanczosG + 0.5) - dAg / Ag;
    return out;
}

double GammaFuncMod::polygamma(size_t k, double x)
{
    double	brn[] = {1.6666666666666666e-01, 3.3333333333333333e-02,
     2.3809523809523809e-02, 3.3333333333333333e-02, 7.5757575757575757e-02,
     2.5311355311355311e-01, 1.1666666666666667e+00, 7.0921568627450980e+00,
     5.4971177944862155e+01, 5.2912424242424242e+02};
    int nBrn = 9;
    double	s;	/* return value */
    double	y;	/* minimum value more than `slv', adding `x' to integers */
    double	x2;	/* x * x */
    double	*bp;	/* the pointer to `brn[]` */
    double	pk;	/* k! */
    double	pxk;	/* pxk = pow(x, k+1)	*/
    double	slv;	/* sufficient large value applied for asymptotic expansion */
    double	f;
    int	n;	/* [slv -x] */
    int	i, j;
    int	i2, isgn;
    if (k < 0 || x < 0) { 	/* k < 0 or x < 0 or x is +-INF */
        return dataNA;
    } else if (k > 3) {
        /*
         * calculation of `slv'
         */
        f = 1.0;
        for (i = k + 19; i > 20; i--) {
            f *= (double) i;
        }
        for (i = k + 1; i > 2; i--) {
            f /= (double) i;
        }
        f *= (174611.0 / 55.0);	/* B_{20} / B_{2} */
        slv = 6.812921 * std::pow(f, 1.0 / 18.0);
        if (slv < 13.06) slv = 13.06;
    } else {	/* 1 <= k <= 3 */
        slv = 13.06;
    }
    pk = 1.0;
    for (i = 0; i < k; i++) {
        pk *= (double) (i + 1);	/* pk = k! */
    }
    if (x < eps) {
        /*
         * SING error: polygamma(x) return infinity
         */
        return 1.0 / 0.0;
    } else if (x >= slv) {
        /* Adopted `x' to the asymptotic expansion. */
        s = 0.0;
        x2 = x * x;
        bp = &brn[nBrn];
        isgn = k % 2 ? -1 : 1;
        if (k == 0) {
            /* digamma function */
            for (i = nBrn; i >= 0; i--) {
                i2 = 2 * (i + 1);
                s += *bp-- / (double) i2 * isgn;
                s /= x2;
                isgn *= -1;
            }
            s += std::log(x) - 0.5 / x;
        } else {
            /* k >= 1; trigamm, tetragamma, ... */
            for (i = nBrn; i >= 0; i--) {
                f = 1.0;
                i2 = 2 * (i + 1);
                j = i2 + k - 1;
                while (j > i2) {
                    f *= (double) j--;
                }
                s += *bp-- * f * isgn;
                s /= x2;
                isgn *= -1;
            }
            for (i = 0; i < k; i++) {
                s /= x;
            }
            pxk = 1.0;
            for (i = 0; i < k; i++) {
                pxk *= x;	/* pxk = pow(x, k) */
            }
            s -= pk * 0.5 / pxk / x * isgn;
            f = pk / (double) k;
            s -= f / pxk * isgn;
        }
    } else {
        /*
         * x < slv;
         * Adopted `y' instead of `x' to the asymptotic expansion,
         * we calculation the value.
         */
        n = (int) (slv - x);
        y = (double) n + x + 1.0;
        s = polygamma(k, y);
        isgn = k % 2 ? 1 : -1;
        for (i = 0; i <= n; i++) {
            y -= 1.0;
            if (std::fabs(y) < 1.e-3) {
                if (x > 0) y = x - (double)((int) (x + 0.5));
                else y = x - (double)((int) (x - 0.5));
            }
            pxk = 1.0;
            for (j = 0; j < k; j++) {
                pxk *= y;	/* pxk = pow(y, k) */
            }
            s += isgn *  pk / pxk / y;
        }
    }
    return s;
}

double GammaFuncMod::impGammaP(double a, double x)
{
    return lowerImpGamma(a, x) / gamma(a);
}

double GammaFuncMod::impGammaQ(double a, double x)
{
    return upperImpGamma(a, x) / gamma(a);
}

double GammaFuncMod::smallXUpperImpGamma(double a, double x)
{
    double prefix = 0.0;
    double gammaA_1 = gamma(a + 1);
    double powxa = std::pow(x, a);
    prefix = (gammaA_1 - powxa) / a;

    double enderFactor = std::numeric_limits<double>::epsilon();

    int k = 1;
    double delta = DOUBLE_MAX;
    double subfrac = 1;
    double result = prefix;
    double tmpfrac;
    do {
        subfrac *= ((-x) / k);
        tmpfrac = powxa * (subfrac / (a + k));
        result -= tmpfrac;
        delta = std::fabs(tmpfrac);
        ++k;
    } while (k < maxIterNum && delta > std::fabs(enderFactor * result));
    return result;
}

double GammaFuncMod::lowerImpGamma(double a, double x)
{
    double tmpGammaVal;
    double gammaA;
    if (x > 1.1) {
        if (a * 4 < x) {
            tmpGammaVal = upperImpGammaGeneral(a, x);
            gammaA = gamma(a);
            return gammaA - tmpGammaVal;
        } else {
            tmpGammaVal = lowerImpGammaGeneral(a, x);
            return tmpGammaVal;
        }
    } else {
        if (x < 0.5) {
            if (-0.4 / std::log(x) < a) {
                tmpGammaVal = lowerImpGammaGeneral(a, x);
                return tmpGammaVal;
            } else {
                tmpGammaVal = smallXUpperImpGamma(a, x);
                gammaA = gamma(a);
                return gammaA - tmpGammaVal;
            }
        } else {
            if (x * 0.75f < a) {
                tmpGammaVal = lowerImpGammaGeneral(a, x);
                return tmpGammaVal;
            } else {
                tmpGammaVal = smallXUpperImpGamma(a, x);
                gammaA = gamma(a);
                return gammaA - tmpGammaVal;
            }
        }
    }

    return lowerImpGammaGeneral(a, x);
}

double GammaFuncMod::upperImpGamma(double a, double x)
{
    double tmpGammaVal;
    double gammaA;
    if (x > 1.1) {
        if (a * 4 < x) {
            tmpGammaVal = upperImpGammaGeneral(a, x);
            return tmpGammaVal;
        } else {
            tmpGammaVal = lowerImpGammaGeneral(a, x);
            gammaA = gamma(a);
            return gammaA - tmpGammaVal;
        }
    } else {
        if (x < 0.5) {
            if (-0.4 / std::log(x) < a) {
                tmpGammaVal = lowerImpGammaGeneral(a, x);
                gammaA = gamma(a);
                return gammaA - tmpGammaVal;
            } else {
                tmpGammaVal = smallXUpperImpGamma(a, x);
                return tmpGammaVal;
            }
        } else {
            if (x * 0.75f < a) {
                tmpGammaVal = lowerImpGammaGeneral(a, x);
                gammaA = gamma(a);
                return gammaA - tmpGammaVal;
            } else {
                tmpGammaVal = smallXUpperImpGamma(a, x);
                return tmpGammaVal;
            }
        }
    }

    return upperImpGammaGeneral(a, x);
}

void GammaFuncMod::copy(const GammaFuncMod& object)
{
    maxIterNum = object.maxIterNum;
    tiny = object.tiny;
    dataNA = object.dataNA;
    eps = object.eps;
    pi = object.pi;
    if (gammaIterN != object.gammaIterN) {
        if (gammaCk) delete [] gammaCk;
        gammaIterN = object.gammaIterN;
        if (!gammaIterN) gammaCk = nullptr;
        else gammaCk = new double [gammaIterN];
    }
    if (lanczosN != object.lanczosN) {
        if (lanczosP) delete [] lanczosP;
        lanczosN = object.lanczosN;
        if (!lanczosN) lanczosP=nullptr;
        else lanczosP = new double [lanczosN];
    }
    for (size_t i = 0; i < gammaIterN; ++i) {
        gammaCk[i] = (object.gammaCk)[i];
    }
    for (size_t i = 0; i < lanczosN; ++i) {
        lanczosP[i] = (object.lanczosP)[i];
    }
    lanczosG = object.lanczosG;
}

const GammaFuncMod &GammaFuncMod::operator=(const GammaFuncMod &object)
{
    copy(object);
    return *this;
}

void BetaFuncMod::initAccuSum()
{
    int i;
    if (accuSum) {
        return;
    }
    accuSum = new double[lenAccuSum + 1];
    for (i = 0; i < lenAccuSum; ++i) {
        if (i == 1) {
            accuSum[i] = 1;
        } else {
            accuSum[i] = double(i) * accuSum[i - 1];
        }
    }
}

BetaFuncMod::BetaFuncMod()
{
    n_betaLargeASmallB_pn = 20;
    maxIterNum = 150;

    accuSum = nullptr;
    initAccuSum();
}

BetaFuncMod::~BetaFuncMod()
{
    if (accuSum) {
        delete [] accuSum;
        accuSum = nullptr;
    }
}

double BetaFuncMod::beta(double a, double b)
{
    return std::exp(std::lgamma(a) + std::lgamma(b) - std::lgamma(a+b));
}

double BetaFuncMod::lBeta(double a, double b)
{
    return std::lgamma(a) + std::lgamma(b) - std::lgamma(a+b);
}

double BetaFuncMod::impBetaLargeASmallB(double a, double b, double x)
{
    // Ix(a,b) = 1 - Iy(b,a)
    // and y = 1 - x
    if (a < b) {
        return 1.0 - impBetaLargeASmallB(b, a, 1.0 - x);
    }

    double ret = 0.0;

    // calculate M = H(b) * gamma(a+b) / ( gamma(a) * T^b )
    // T = a + (b-1)/2
    // u = -T * ln(x)
    // H(b) = exp(-u) * u^b / gamma(b)
    double T = a + .5 * (b - 1.0);
    double lnT = std::log(T);
    double lnx = std::log(x);
    double lnx_2 = 0.5 * lnx;
    double u = - T * lnx;
    double lnu = std::log(u);
    double lgamA = lgamma(a);
    double lgamB = lgamma(b);
    double lgamApB = lgamma(a + b);
    double lnHb = -u + b * lnu - lgamB;
    double lnM = lnHb + lgamApB - lgamA - b * lnT;
    double Hb = std::exp(lnHb);
    double M = std::exp(lnM);

    double *pn = new double[n_betaLargeASmallB_pn];
    double jbu = gammaFunc.impGammaQ(b, u) / Hb;
    double s_pni;
    double jn;
    double lnx_n;
    double delta = std::numeric_limits<double>::max();
    double enderFactor = std::numeric_limits<double>::epsilon();
    int ip;
    int jp;
    double mb_n;
    int m2_1;
    int n2_1;
    double b_2n;

    bool isBreak = false;

    ret += M * jbu;
    pn[0] = 0;
    ip = 1, n2_1 = 3, b_2n = b, jn = jbu, lnx_n = 1;
    lnx_2 *= lnx_2;
    while (ip < n_betaLargeASmallB_pn && std::fabs(delta) > std::fabs(enderFactor * ret)) {
        pn[ip] = 0;
        s_pni = 0;
        for (jp = 1, m2_1 = 3; jp < ip; ++jp, m2_1 += 2) {
            mb_n = jp * b - ip;
            if (m2_1 >= lenAccuSum) {
                isBreak = true;
                break;
            }
            s_pni += mb_n * pn[ip - jp] / accuSum[m2_1];
        }
        if (n2_1 >= lenAccuSum || isBreak) {
            isBreak = true;
            break;
        }
        s_pni /= ip;
        pn[ip] = (b - 1.0)/accuSum[n2_1] + s_pni;
        jn = (b_2n * (b_2n + 1) * jn + (u + b_2n + 1) * lnx_n) / (4 * T * T);
        lnx_n *= lnx_2;
        ret += M * pn[ip] * jn;
        ++ip, n2_1 += 2, b_2n += 2;
    }
    delete [] pn;
    return ret;
}

double BetaFuncMod::impBetaSmallishBX(double a, double b, double x)
{
    double ret = 0.0;
    double lnx = std::log(x);
    double prefix = 0.0;
    double lnPrefix = 0.0;
    lnPrefix = a * lnx - (std::lgamma(a) + std::lgamma(b) - std::lgamma(a+b));
    prefix = std::exp(lnPrefix);

    int k = 1;
    double subFrac = 1.0;
    double delta = DOUBLE_MAX;
    double enderFactor = std::numeric_limits<double>::epsilon();
    ret += prefix / a;
    while (k < maxIterNum && std::fabs(delta) > std::fabs(enderFactor * ret)) {
        subFrac *= ((k - b) * x / k);
        delta = prefix * subFrac / (a + k);
        ret += delta;
        ++k;
    }

    return ret;
}

double BetaFuncMod::impBetaGreat(double a, double b, double x)
{
    double lnx = std::log(x);
    double lny = std::log(1.0 - x);
    double prefix = 0.0;
    double lnPrefix = 0.0;
    lnPrefix = a * lnx + b * lny - (std::lgamma(a) + std::lgamma(b) - std::lgamma(a+b));
    prefix = std::exp(lnPrefix);
    ImpBetaGreatFuncIter iter;
    iter.setValue(a, b, x);
    ContinuedFraction1<double> frac(&iter);

    return prefix * frac.getValue();
}

double BetaFuncMod::impBeta(double a, double b, double x)
{
    if (a < b) {
        return 1.0 - impBeta(b, a, 1.0 - x);
    }

    if (a > 1.0 && b > 1.0) {
        if (b > 40) {
            return impBetaGreat(a, b, x);
        } else {
            if (b * x <= 0.7) {
                return impBetaSmallishBX(a, b ,x);
            } else if (a > 15) {
                return impBetaLargeASmallB(a, b, x);
            } else {
                return impBetaGreat(a, b, x);
            }
        }
    } else {
        if (x < 0.7) {
            return impBetaSmallishBX(a, b ,x);  
        } else {
            return impBetaLargeASmallB(a, b, x);
        }
    }

    return impBetaGreat(a, b, x);
}

void BetaFuncMod::copy(const BetaFuncMod &object)
{
    maxIterNum = object.maxIterNum;
    dataNA     = object.dataNA;
    tiny       = object.tiny;
    eps        = object.eps;
    pi         = object.pi;

    n_betaLargeASmallB_pn = object.n_betaLargeASmallB_pn;
    gammaFunc = object.gammaFunc;

    initAccuSum();
}

const BetaFuncMod &BetaFuncMod::operator=(const BetaFuncMod &object)
{
    copy(object);
    return *this;
}

};

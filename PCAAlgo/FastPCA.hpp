#ifndef MML_FASTPCA_HPP
#define MML_FASTPCA_HPP

#include "DataManager/DataManager.hpp"
#include <fstream>

//#include <QDebug>
//#include <QString>

namespace MML {

void printMat(const Mat& inX,const char *out);

class FastPCA
{
public:
    size_t ranNCol;
    Mat ranMat;
    FastPCA();
    bool calcuPC(const Mat& inQ,const Mat& inS,Mat& outPC,size_t d);
    bool IterativeRun(const Mat& covMat,Mat& Y);
    bool addCovMat(const Mat& inX,Mat& outMat); // outMat += X %*% Xt
    bool addIterMat(const Mat& inX,const Mat& inRan,Mat& outMat); // outMat += X %*% R
    bool addIterMat(const Mat& inX,Mat& outMat);
    bool addSmallMat(const Mat& inQ,const Mat& inX,Mat& outS); //S += Qt %*% X %*% Xt %*% Q

    bool standardizeCol(Mat& inMat,double factor = 1.0);
    bool normalizeCol(Mat& inMat);
    static int MAXITERTIMES;
};

} // namespace MML

#endif // MML_FASTPCA_HPP

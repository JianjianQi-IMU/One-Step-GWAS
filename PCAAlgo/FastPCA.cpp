#include "FastPCA.hpp"

namespace MML {

//void printMat(const Mat &inX,const char *out)
//{
//    qDebug() << out;
//    if(inX.info==_null||
//       inX.getNCol()==0||
//       inX.getNRow()==0){
//        qDebug() << "NA";
//    }

//    QString OS;
//    for(size_t i=0;i<inX.nrow;++i){
//        OS.clear();
//        for(size_t j=0;j<inX.ncol;++j){
//            OS+=QString::number(inX.data[i*inX.ncol+j])+'\t';
//        }
//        qDebug()<< OS;
//    }
//}

int FastPCA::MAXITERTIMES = 1000;


FastPCA::FastPCA()
    :ranNCol(100)
{

}

bool FastPCA::calcuPC(const Mat &inQ, const Mat &inS, Mat &outPC, size_t d)
{
    if(inQ.info==_null||
       inS.info!=_sym||
       inQ.getNCol()!=inS.getNRow()) return false;
    size_t n=inQ.getNRow();
    Mat EVec,EVal,Ud;
    double lambdad=0.0;
    if(!inS.symEig(EVec,EVal)) return false;
    if(!quickSortEigen(EVec,EVal,false)) return false;
//    MML::printMat(EVal,"val:");
    if(!Mat::XYmul(inQ,EVec(0,0,ranNCol-1,d-1),Ud)) return false;
    outPC.resize(n,d);
    for(size_t i=0;i<d;++i){
        lambdad=std::sqrt(EVal(i)/(n-1));
        for(size_t j=0;j<n;++j){
            outPC(j,i)=EVec(j,i)*lambdad;
        }
    }
    outPC.info=_general;
    return true;
}

bool FastPCA::IterativeRun(const Mat &covMat, Mat &Y)
{
    Mat CY;
    if(covMat.info==_null||
       Y.info==_null     ||
       covMat.getNCol()!=Y.getNRow()) return false;
    for(size_t i=0;i<MAXITERTIMES;++i){
        Mat::XYmul(covMat,Y,CY);
        normalizeCol(CY);
        Y=CY;
    }
    return true;
}

bool FastPCA::addCovMat(const Mat &inX, Mat &outMat)
{
//    if(inX.info==MML::Mat::_null) return false;
//    int M=inX.nrow,N=inX.nrow,K=inX.ncol;
//    double alpha=1.0,beta=1.0;
//    cblas_dgemm(CblasRowMajor,
//                CblasNoTrans,
//                CblasTrans,
//                M,
//                N,
//                K,
//                alpha,
//                inX.data,
//                K,
//                inX.data,
//                K,
//                beta,
//                outMat.data,
//                M);

    return MML::Mat::XXtmul(inX,outMat,true);
}

bool FastPCA::addIterMat(const Mat &inX, const Mat &inRan, Mat &outMat)
{
//    if(inX.ncol!=inRan.nrow      ||
//       inX.info==MML::Mat::_null ||
//       inRan.info==MML::Mat::_null   ) return false;
//    int M=inX.nrow,N=inRan.ncol,K=inX.ncol;
//    double alpha=1.0,beta=1.0;
//    cblas_dgemm(CblasRowMajor,
//                CblasNoTrans,
//                CblasNoTrans,
//                M,
//                N,
//                K,
//                alpha,
//                inX.data,
//                K,
//                inRan.data,
//                N,
//                beta,
//                outMat.data,
//                N);


    return MML::Mat::XYmul(inX,inRan,outMat,true);
}

bool FastPCA::addIterMat(const Mat &inX, Mat &outMat)
{
    std::default_random_engine generator(std::chrono::steady_clock::now().time_since_epoch().count());
    if(inX.info==_null) return false;
    size_t XNCol=inX.getNCol();
    ranMat.resize(XNCol,ranNCol);
    std::uniform_real_distribution<double> distribution(-.5,.5);
    for(size_t i=0;i<XNCol;++i){
        for(size_t j=0;j<ranNCol;++j) ranMat(i,j)=distribution(generator);
    }
    return addIterMat(inX, ranMat, outMat);
}

bool FastPCA::addSmallMat(const Mat &inQ, const Mat &inX, Mat& outS)
{
    if(inQ.info==_null||
       inX.info==_null||
       inQ.getNRow()!=inX.getNRow()) return false;
    Mat B;
    if(!Mat::XtYmul(inQ,inX,B)) return false;

//    int M=B.nrow,N=B.nrow,K=B.ncol;
//    double alpha=1.0,beta=1.0;
//    cblas_dgemm(CblasRowMajor,
//                CblasNoTrans,
//                CblasTrans,
//                M,
//                N,
//                K,
//                alpha,
//                B.data,
//                K,
//                B.data,
//                K,
//                beta,
//                outS.data,
//                outS.getNCol());

    return MML::Mat::XXtmul(B,outS,true);
}

bool FastPCA::standardizeCol(Mat &inMat,double factor)
{
    if(inMat.info==_null) return false;
    size_t nRow=inMat.getNRow(),nCol=inMat.getNCol();
    double sum=0.0,mean=0.0,d2=0.0,d=0.0;
    size_t i,j,n=0;
    for(i=0;i<nCol;++i){
        sum=0.0;
        n=0;
        for(j=0;j<nRow;++j){
            if(inMat(j,i)!=DATA_NA){
                sum+=inMat(j,i);
                ++n;
            }
        }
        mean=sum/n;
        d2=0.0;
        for(j=0;j<nRow;++j) d2+=(inMat(j,i)-mean)*(inMat(j,i)-mean);
        d2/=n;
        if(d2<MML::EPS){
            for(j=0;j<nRow;++j) inMat(j,i)=0.0;
        }
        else{
            d=std::sqrt(d2);
            for(j=0;j<nRow;++j){
                if(inMat(j,i)!=DATA_NA){
                    inMat(j,i)=(inMat(j,i)-mean)*factor/d;
                }
                else{
                    inMat(j,i)=0.0;
                }
            }
        }
    }
    return true;
}

bool FastPCA::normalizeCol(Mat &inMat)
{
    if(inMat.info==_null) return false;
    size_t nRow=inMat.getNRow(),nCol=inMat.getNCol();
    double sum2=0.0,stdev=0.0;
    size_t i,j;
    for(i=0;i<nCol;++i){
        sum2=0.0;
        for(j=0;j<nRow;++j){
            sum2+=inMat(j,i)*inMat(j,i);
        }
        stdev=std::sqrt(sum2);
        if(stdev<MML::EPS){
            for(j=0;j<nRow;++j) inMat(j,i)=0.0;
        }
        else{
            for(j=0;j<nRow;++j){
                inMat(j,i)=inMat(j,i)/stdev;
            }
        }
    }
    return true;
}


} // namespace MML

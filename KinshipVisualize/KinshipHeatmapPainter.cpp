#include "KinshipHeatmapPainter.hpp"

KinDis::KinDis()
{

}

KinDis::KinDis(const MML::Mat &inKin)
{
    setData(inKin);
}

KinDis::~KinDis()
{

}

void KinDis::setData(const MML::Mat &inKin)
{
    size_t n=inKin.getNRow();
    double tmp=0.0;
    dis.resize(n,n);
    val.resize(n,1);
    for(size_t i=0;i<n-1;++i){
        for(size_t j=i+1;j<n;++j){
            tmp=0.0;
            for(size_t k=0;k<n;++k){
                tmp+=inKin(i,k)-inKin(j,k);
            }
            tmp=std::abs(tmp);
            tmp/=n;
            dis(i,j)=tmp;
            dis(j,i)=tmp;
        }
    }
    for(size_t i=0;i<n-1;++i){
        tmp=0.0;
        for(size_t j=i+1;j<n;++j){
            tmp+=inKin(i,j);
        }
        val(i)=tmp;
    }
}

KinshipHeatmapPainter::KinshipHeatmapPainter()
{
    lowColor=QColor(0,0,255);
    highColor=QColor(255,0,0);
    meanColor=QColor(255,255,255);
    maxVal = 1.0;
    minVal = 0.0;
    meanVal = 0.5;
    rowSelected = -1;
    colSelected = -1;
    isDisplaySelectedText = false;
    lineStyleParamSelectedFrame.lineColor = Qt::red;
    lineStyleParamSelectedFrame.lineSize = 2;
    lineStyleParamSelectedFrame.lineStyle = Qt::DashLine;
    colorSelectedText=QColor(0,0,0);
    fontSelectedText.setFamily("Times New Roman");
    fontSelectedText.setBold(true);
}

KinshipHeatmapPainter::KinshipHeatmapPainter(const MML::Mat &inKin, char **inName)
    : KinshipHeatmapPainter()
{
    loadKinshipData(inKin, inName);
}

void KinshipHeatmapPainter::loadKinshipData(const MML::Mat &inKin, char **inName)
{
    individualsName.clear();
    paintSeq.clear();
    minVal=1e300;
    maxVal=-1e300;
    kin = inKin;
    double tmpLen = 0.0;
    long long nInd = inKin.getNRow();
    for(long long iInd = 0;iInd < nInd;++iInd){
        individualsName.append(inName[iInd]);
        paintSeq.append(iInd);
        for(long long jInd=0;jInd<nInd;++jInd){
            maxVal=std::max(maxVal,kin(iInd,jInd));
            minVal=std::min(minVal,kin(iInd,jInd));
        }
    }
    tmpLen=maxVal-minVal;
    maxVal+=0.1*tmpLen;
    minVal-=0.1*tmpLen;
    meanVal=.5*(maxVal+minVal);
}

void KinshipHeatmapPainter::setClusterIndex()
{
    KinDis dis;
    dis.setData(kin);
    long long nInd = kin.getNRow();
    HierarchicalCluster<long long,KinDis> cluster(&dis);
    long long* tIndex = new long long[nInd];
    for(long long i=0;i<nInd;++i) tIndex[i]=i;
    cluster.meanCluster(tIndex,nInd);
    cluster.getSeq(tIndex);
    for(long long i=0;i<nInd;++i){
        paintSeq[i]=tIndex[i];
    }
    delete [] tIndex;
}

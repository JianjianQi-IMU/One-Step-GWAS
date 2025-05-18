#include "GEMMA.hpp"
#include <cmath>
#include <iostream>
#include <cstdio>

#include <QDebug>

GEMMA::NewtonPara::NewtonPara(double iMin,double iMax,size_t iN,size_t iMaxTimes):
min(iMin),max(iMax),n(iN),maxTimes(iMaxTimes){
    limList=new double [n+1];
    dll1List=new double [n+1];
    double lenRegion=(max-min)/n;
    for(size_t i=0;i<n+1;++i){
        limList[i]=min+i*lenRegion;
    }
    uList=new double [n];
    dList=new double [n];
}

GEMMA::NewtonPara::NewtonPara():NewtonPara(-5,5,10,100){}

GEMMA::NewtonPara::NewtonPara(const NewtonPara &inPara) : NewtonPara()
{
    copy(inPara);
}

GEMMA::NewtonPara::~NewtonPara(){
    if(limList)  delete [] limList;
    if(dll1List) delete [] dll1List;
    if(uList)    delete [] uList;
    if(dList)    delete [] dList;
    limList=nullptr;
    dll1List=nullptr;
    uList=nullptr;
    dList=nullptr;
}

void GEMMA::NewtonPara::copy(const NewtonPara &object)
{
    min=object.min;
    max=object.max;
    n=object.n;
    maxTimes=object.maxTimes;
    if(limList) {
        delete [] limList;
    }
    if(dll1List) {
        delete [] dll1List;
    }
    if(uList) {
        delete [] uList;
    }
    if(dList) {
        delete [] dList;
    }
    limList = new double [n + 1];
    dll1List = new double [n + 1];
    for(size_t i = 0; i < n + 1; ++i){
        limList[i] = (object.limList)[i];
        dll1List[i] = (object.dll1List)[i];
    }
    uList = new double [n];
    dList = new double [n];
    for(size_t i=0;i<n;++i){
        uList[i] = (object.uList)[i];
        dList[i] = (object.dList)[i];
    }
}

const GEMMA::NewtonPara& GEMMA::NewtonPara::operator=(const NewtonPara& inPara){
    copy(inPara);
    return *this;
}

GEMMA::VecList::VecList(){
    data=nullptr;
    isValid=false;
    vecN=0;
    vecLen=0;
}

GEMMA::VecList::VecList(const VecList &inVecList)
    :VecList()
{
    copy(inVecList);
}

GEMMA::VecList::VecList(size_t inVecN,size_t inVecLen){
    if(inVecLen==0||inVecN==0){
        isValid=false;
        data=nullptr;
        vecN=0;
        vecLen=0;
    }
    else{
        vecN=inVecN,vecLen=inVecLen;
        data=new double* [vecN];
        for(size_t i=0;i<vecN;++i){
            data[i]=new double [vecLen];
        }
        isValid=true;
    }
}

GEMMA::VecList::~VecList(){
    if(data){
        for(size_t i=0;i<vecN;++i){
            delete [] data[i];
        }
        delete [] data;
        data=nullptr;
        isValid=false;
    }
}

bool GEMMA::VecList::resize(size_t inVecN,size_t inVecLen){
    if(inVecLen==0||inVecN==0){
        isValid=false;
        data=nullptr;
        vecN=0;
        vecLen=0;
    }
    else if(vecN==inVecN&&vecLen==inVecLen){
        isValid=true;
    }
    else{
        if(data){
            for(size_t i=0;i<vecN;++i){
                delete [] data[i];
            }
            delete [] data;
        }
        vecN=inVecN,vecLen=inVecLen;
        data=new double* [vecN];
        for(size_t i=0;i<vecN;++i){
            data[i]=new double [vecLen];
        }
        isValid=true;
    }
    return isValid;
}

size_t GEMMA::VecList::getVecN(){return vecN;}

size_t GEMMA::VecList::getVecLen(){return vecLen;}

void GEMMA::VecList::copy(const VecList &object)
{
    if(data){
        for(size_t i=0;i<vecN;++i){
            delete [] data[i];
        }
        delete [] data;
    }
    isValid = object.isValid;
    vecN = object.vecN;
    vecLen = object.vecLen;
    if(!object.data||!isValid||!vecN||!vecLen){
        data = nullptr;
    }
    else{
        data=new double* [vecN];
        for(size_t i=0;i<vecN;++i){
            data[i]=new double [vecLen];
            for(size_t j=0;j<vecLen;++j){
                data[i][j]=(object.data)[i][j];
            }
        }
    }
}

double* GEMMA::VecList::operator()(size_t Index){return data[Index];}

double& GEMMA::VecList::operator()(size_t Index1,size_t Index2){return data[Index1][Index2];}

const double& GEMMA::VecList::operator()(size_t Index1,size_t Index2) const{return data[Index1][Index2];}

const GEMMA::VecList& GEMMA::VecList::operator=(const VecList& inVecList){
    copy(inVecList);
    return *this;
}

GEMMA::MLM::MLM(const MML::Phenotype&  inPhe,
                const MML::Kinship&    inKin,
                const MML::Covariates& inCov)
    :MLM(){
    read(inPhe,inKin,inCov);
}

GEMMA::MLM::MLM(){
    isValid=false;
    eps=MML::EPS;
    pi=MML::PI;
}

GEMMA::MLM::~MLM(){}

bool GEMMA::MLM::read(const MML::Phenotype&  inPhe,
                      const MML::Kinship&    inKin,
                      const MML::Covariates& inCov){
    if(!(inPhe.isValid)||
       !(inKin.isValid)||
       !(inCov.isValid)||
       inPhe.filterN!=inKin.filterN||
       inPhe.filterN!=inCov.filterN||
       inCov.factorN==0) isValid=false;
    else{
        MML::Mat G;
        if(!centerKin(inKin.filterMat,G)) return false;
        if(!G.symEig(Ut,delta)) return false;
        Ut.t();

        if(!MML::Mat::XVmul(Ut,inPhe.getFilterVec(),Uty)){
            isValid=false;
            return false;
        }
        if(!MML::Mat::XYmul(Ut,inCov.getFilterMat(),UtK)){
            isValid=false;
            return false;
        }
        factorN=inCov.factorN;
        filterN=inPhe.filterN;
        if(!initUaUb()){
            isValid=false;
            return false;
        }
        Lpara=NewtonPara();
        isValid=true;
    }

    return isValid;
}

bool GEMMA::MLM::initUaUb(){
    if(filterN==0||factorN==0) return false;
    size_t i=0,j=0,k=0;
    double *pUab=nullptr;
    UaUb.resize((factorN+2)*(factorN+3)/2,filterN);
    for(i=1;i<=factorN;++i){
        for(j=1;j<=i;++j){
            pUab=UaUb((i-1)*i/2+j-1);
            for(k=0;k<filterN;++k){
                pUab[k]=UtK(k,i-1)*UtK(k,j-1);
            }
        }
    }
    for(j=1;j<=factorN;++j){
        pUab=UaUb((factorN+1)*(factorN+2)/2+j-1);
        for(k=0;k<filterN;++k){
            pUab[k]=Uty(k)*UtK(k,j-1);
        }
    }
    pUab=UaUb((factorN+2)*(factorN+3)/2-1);
    for(k=0;k<filterN;++k){
        pUab[k]=Uty(k)*Uty(k);
    }

    return true;
}

bool GEMMA::MLM::analyze(const MML::Mat& Utx,double& outBetax,
                         double& outFstat){
    if(!isValid||
       (Utx.info!=MML::_colvec&&Utx.info!=MML::_rowvec)||
       Utx.getNCol()*Utx.getNRow()!=filterN) return false;

    if(!updateUaUb(Utx)) return false;

    double Lambda,Dll;
    if(!calcuLambda(Lambda,Dll)) return false;
    if(!calcuFWald(Lambda,outBetax,outFstat)) return false;
    return true;
}

bool GEMMA::MLM::analyze(const double* Utx,double& outBetax,
                         double& outFstat){
    if(!isValid) return false;
    if(!updateUaUb(Utx)) return false;
    double Lambda,Dll;
    if(!calcuLambda(Lambda,Dll)) return false;
    if(!calcuFWald(Lambda,outBetax,outFstat)) return false;
    return true;
}

bool GEMMA::MLM::updateUaUb(const MML::Mat& Utx){
    if(Utx.getNRow()!=filterN) return false;
    size_t j=0,k=0;
    double *pUab=nullptr;
    for(j=1;j<=factorN;++j){
        pUab=UaUb((factorN)*(factorN+1)/2+j-1);
        for(k=0;k<filterN;++k){
            pUab[k]=Utx(k)*UtK(k,j-1);
        }
    }
    pUab=UaUb((factorN+2)*(factorN+3)/2-2);
    for(k=0;k<filterN;++k){
        pUab[k]=Utx(k)*Uty(k);
    }
    pUab=UaUb((factorN+1)*(factorN+2)/2-1);
    for(k=0;k<filterN;++k){
        pUab[k]=Utx(k)*Utx(k);
    }
    return true;
}

bool GEMMA::MLM::updateUaUb(const double* Utx){
    size_t j=0,k=0;
    double *pUab=nullptr;
    for(j=1;j<=factorN;++j){
        pUab=UaUb((factorN)*(factorN+1)/2+j-1);
        for(k=0;k<filterN;++k){
            pUab[k]=Utx[k]*UtK(k,j-1);
        }
    }
    pUab=UaUb((factorN+2)*(factorN+3)/2-2);
    for(k=0;k<filterN;++k){
        pUab[k]=Utx[k]*Uty(k);
    }
    pUab=UaUb((factorN+1)*(factorN+2)/2-1);
    for(k=0;k<filterN;++k){
        pUab[k]=Utx[k]*Utx[k];
    }
    return true;
}

bool GEMMA::MLM::calcuPatb(const MML::Mat& D,MML::Mat& atPb){
    atPb.resize(factorN+2,(factorN+2)*(factorN+3)/2);
    long long i=0,j=0,p=0,k=0,ij=0,ip=0,jp=0;
    double tmp=0.0,atPbkk=0.0;
    double* pUab=nullptr;
    for(p=0;p<factorN+2;++p){
        for(i=p+1;i<=factorN+2;++i){
            for(j=p+1;j<=i;++j){
                if(p==0){
                    tmp=0.0;
                    ij=(i-1)*i/2+j-1;
                    pUab=UaUb(ij);
                    for(k=0;k<filterN;++k){
                        tmp+=pUab[k]/D(k);
                    }
                    atPb(p,ij)=tmp;
                }
                else{
                    ij=(i-1)*i/2+j-1;
                    ip=(i-1)*i/2+p-1;
                    jp=(j-1)*j/2+p-1;
                    atPbkk=atPb(p-1,(p-1)*p/2+p-1);
                    if(std::fabs(atPbkk)>eps) {
                        atPb(p,ij)=atPb(p-1,ij)-atPb(p-1,ip)*atPb(p-1,jp)/atPbkk;
                    }
                    else{
                        atPb(p,ij)=atPb(p-1,ij);
                    }
                }
            }
        }
    }
    return true;
}

bool GEMMA::MLM::calcuPPatb(const MML::Mat& D,const MML::Mat& atPb,
                            MML::Mat& atPPb){
    atPPb.resize(factorN+2,(factorN+2)*(factorN+3)/2);
    long long i=0,j=0,p=0,k=0,ij=0,ip=0,jp=0,pp=0;
    double tmp=0.0,atPbkk=0.0;
    double* pUab=nullptr;
    for(p=0;p<factorN+2;++p){
        for(i=p+1;i<=factorN+2;++i){
            for(j=p+1;j<=i;++j){
                if(p==0){
                    tmp=0.0;
                    ij=(i-1)*i/2+j-1;
                    pUab=UaUb(ij);
                    for(k=0;k<filterN;++k){
                        tmp+=pUab[k]/(D(k)*D(k));
                    }
                    atPPb(p,ij)=tmp;
                }
                else{
                    ij=(i-1)*i/2+j-1;
                    ip=(i-1)*i/2+p-1;
                    jp=(j-1)*j/2+p-1;
                    pp=(p-1)*p/2+p-1;
                    atPbkk=atPb(p-1,pp);
                    tmp=0.0;
                    if(std::fabs(atPbkk)>eps) {
                        tmp=atPPb(p-1,ij)+atPb(p-1,ip)*atPb(p-1,jp)*atPPb(p-1,pp)/(atPbkk*atPbkk);
                        tmp-=(atPb(p-1,ip)*atPPb(p-1,jp)/atPbkk+atPb(p-1,jp)*atPPb(p-1,ip)/atPbkk);
                    }
                    else{
                        tmp=atPPb(p-1,ij);
                    }
                    atPPb(p,ij)=tmp;
                }
            }
        }
    }
    return true;
}

bool GEMMA::MLM::calcuPPPatb(const MML::Mat& D,const MML::Mat& atPb,
                             const MML::Mat& atPPb,MML::Mat& atPPPb){
    atPPPb.resize(factorN+2,(factorN+2)*(factorN+3)/2);
    long long i=0,j=0,p=0,k=0,ij=0,ip=0,jp=0,pp=0;
    double tmp=0.0,atPbkk=0.0,atPbkk2=0.0,atPPbkk=0.0;
    double* pUab=nullptr;
    for(p=0;p<factorN+2;++p){
        for(i=p+1;i<=factorN+2;++i){
            for(j=p+1;j<=i;++j){
                if(p==0){
                    tmp=0.0;
                    ij=(i-1)*i/2+j-1;
                    pUab=UaUb(ij);
                    for(k=0;k<filterN;++k){
                        tmp+=pUab[k]/(D(k)*D(k)*D(k));
                    }
                    atPPPb(p,ij)=tmp;
                }
                else{
                    ij=(i-1)*i/2+j-1;
                    ip=(i-1)*i/2+p-1;
                    jp=(j-1)*j/2+p-1;
                    pp=(p-1)*p/2+p-1;
                    atPbkk=atPb(p-1,pp);
                    atPbkk2=atPbkk*atPbkk;
                    atPPbkk=atPPb(p-1,pp);
                    tmp=0.0;
                    if(std::fabs(atPbkk)>eps) {
                        tmp=atPPPb(p-1,ij)-atPb(p-1,ip)*atPb(p-1,jp)*atPPbkk*atPPbkk/(atPbkk2*atPbkk);
                        tmp+=atPb(p-1,ip)*atPb(p-1,jp)*atPPPb(p-1,pp)/atPbkk2;
                        tmp+=atPb(p-1,ip)*atPPb(p-1,jp)*atPPbkk/atPbkk2+atPPb(p-1,ip)*atPb(p-1,jp)*atPPbkk/atPbkk2;
                        tmp-=atPPb(p-1,ip)*atPPb(p-1,jp)/atPbkk;
                        tmp-=(atPb(p-1,ip)*atPPPb(p-1,jp)/atPbkk+atPPPb(p-1,ip)*atPb(p-1,jp)/atPbkk);
                    }
                    else{
                        tmp=atPPPb(p-1,ij);
                    }
                    atPPPb(p,ij)=tmp;
                }
            }
        }
    }
    return true;
}

bool GEMMA::MLM::calcuIatb(MML::Mat& atIb){
    atIb.resize(factorN+2,(factorN+2)*(factorN+3)/2);
    long long i=0,j=0,p=0,k=0,ij=0,ip=0,jp=0;
    double tmp=0.0,atIbkk=0.0;
    double* pUab=nullptr;
    for(p=0;p<factorN+2;++p){
        for(i=p+1;i<=factorN+2;++i){
            for(j=p+1;j<=i;++j){
                if(p==0){
                    tmp=0.0;
                    ij=(i-1)*i/2+j-1;
                    pUab=UaUb(ij);
                    for(k=0;k<filterN;++k){
                        tmp+=pUab[k];
                    }
                    atIb(p,ij)=tmp;
                }
                else{
                    ij=(i-1)*i/2+j-1;
                    ip=(i-1)*i/2+p-1;
                    jp=(j-1)*j/2+p-1;
                    atIbkk=atIb(p-1,(p-1)*p/2+p-1);
                    if(std::fabs(atIbkk)>eps) {
                        atIb(p,ij)=atIb(p-1,ij)-atIb(p-1,ip)*atIb(p-1,jp)/atIbkk;
                    }
                    else{
                        atIb(p,ij)=atIb(p-1,ij);
                    }
                }
            }
        }
    }
    return true;
}

bool GEMMA::MLM::calcuTrP(double lambda,const MML::Mat& atPb,
                          const MML::Mat& atPPb,double& outTrP){
    long long i=0,ii=0,yii=0;
    outTrP=0;
    for(i=0;i<filterN;++i){
        outTrP+=1/(lambda*delta(i)+1);
    }
    for(i=1;i<=factorN+1;++i){
        ii=(i-1)*i/2+i-1;
        outTrP-=atPPb(i-1,ii)/atPb(i-1,ii);
    }
    return true;
}

bool GEMMA::MLM::calcuTrPP(double lambda,const MML::Mat& atPb,
                           const MML::Mat& atPPb,const MML::Mat& atPPPb,
                           double& outTrPP){
    long long i=0,ii=0,yii=0;
    outTrPP=0;
    double wtPw=0,wtPPw=0;
    for(i=0;i<filterN;++i){
        outTrPP+=1/((lambda*delta(i)+1)*(lambda*delta(i)+1));
    }
    for(i=1;i<=factorN+1;++i){
        ii=(i-1)*i/2+i-1;
        wtPw=atPb(i-1,ii);
        wtPPw=atPPb(i-1,ii);
        outTrPP+=(wtPPw*wtPPw/(wtPw*wtPw)-2*atPPPb(i-1,ii)/wtPw);
    }
    return true;
}

bool GEMMA::MLM::calcuREMLLogL(double lambda,const MML::Mat& atPb,
                               const MML::Mat& atIb,double& outLogL){
    double tmp=0.0,Ldelta=0.0,ytPy=0.0,LytPy=0.0,LKxtKx=0.0,LKxtDiKx=0.0;
    long long i=0,j=0,ii=0;
    for(i=1;i<=factorN+1;++i){
        ii=(i-1)*i/2+i-1;
        if(std::fabs(atPb(i-1,ii))<eps||std::fabs(atIb(i-1,ii))<eps) return false;
        LKxtDiKx+=std::log(atPb(i-1,ii));
        LKxtKx+=std::log(atIb(i-1,ii));
    }
    ytPy=atPb(factorN+1,(factorN+3)*(factorN+2)/2-1);
    if(std::fabs(ytPy)<eps) return false;
    LytPy=std::log(ytPy);
    for(i=0;i<filterN;++i){
        if(std::fabs(delta(i))<eps) return false;
        Ldelta+=std::log(delta(i));
    }
    tmp=0.5*(filterN-factorN-1)*(std::log((filterN-factorN-1)/(2*pi))-1-LytPy);
    tmp+=0.5*(LKxtKx-Ldelta-LKxtDiKx);
    outLogL=tmp;
    return true;
}

bool GEMMA::MLM::calcuREMLDLogL1(double lambda,double trP,
                                 const MML::Mat& atPb,const MML::Mat& atPPb,
                                 double& outDLogL1){
    long long yii=0;
    double trPG=0.0,ytPGPy=0.0,ytPy=0.0,ytPPy=0.0;
    trPG=(filterN-factorN-1-trP)/lambda;
    yii=(factorN+2)*(factorN+3)/2-1;
    ytPy=atPb(factorN+1,yii);
    ytPPy=atPPb(factorN+1,yii);
    ytPGPy=(ytPy-ytPPy)/lambda;
    outDLogL1=0.5*((filterN-factorN-1)*ytPGPy/ytPy-trPG);
    return true;
}

bool GEMMA::MLM::calcuREMLDLogL2(double lambda,double trP,
                                 double trPP,const MML::Mat& atPb,
                                 const MML::Mat& atPPb,const MML::Mat& atPPPb,
                                 double& outDLogL2){
    long long yii=0;
    double trPGPG=0,ytPGPGPy=0,ytPGPy=0,ytPy=0,ytPPy=0,ytPPPy=0;
    trPGPG=(filterN-factorN-1+trPP-2*trP)/(lambda*lambda);
    yii=(factorN+2)*(factorN+3)/2-1;
    ytPy=atPb(factorN+1,yii);
    ytPPy=atPPb(factorN+1,yii);
    ytPPPy=atPPPb(factorN+1,yii);
    ytPGPy=(ytPy-ytPPy)/lambda;
    ytPGPGPy=(ytPy+ytPPPy-2*ytPPy)/(lambda*lambda);
    outDLogL2=0.5*(trPGPG-(filterN-factorN-1)*(2*ytPGPGPy*ytPy-ytPGPy*ytPGPy)/(ytPy*ytPy));
    return true;
}

bool GEMMA::MLM::calcuLambda(double& outLambda,double& outLL){
    MML::Mat atPb,atPPb,atPPPb,atIb,D;
    size_t i=0,choosedNum=0,nIter=0;
    double tmpLambda=0,tmpDLL1=0,tmpDLL2=0,deltaDLL=0,tmpTrP=0,tmpTrPP=0,tmpLL=0,reLambda=-1,reLL=-1;
    double lam1,lam2,lamm,uDll,lDll;
    bool isNewtonRight=true;
    calcuIatb(atIb);

    for(i=0;i<Lpara.n+1;++i){
        tmpLambda=std::exp((Lpara.limList)[i]);
        D=delta;
        D.mul(tmpLambda);
        D.add(1);
        calcuPatb(D,atPb);
        calcuPPatb(D,atPb,atPPb);
        calcuTrP(tmpLambda,atPb,atPPb,tmpTrP);
        calcuREMLDLogL1(tmpLambda,tmpTrP,atPb,atPPb,tmpDLL1);
        (Lpara.dll1List)[i]=tmpDLL1;
    }
    for(i=0;i<Lpara.n;++i){
        if((Lpara.dll1List)[i]>0&&(Lpara.dll1List)[i+1]<0){
            (Lpara.uList)[choosedNum]=std::exp((Lpara.limList)[i+1]);
            (Lpara.dList)[choosedNum]=std::exp((Lpara.limList)[i]);
            ++choosedNum;
        }
    }
    if(choosedNum==0){
        if((Lpara.dll1List)[0]>0) tmpLambda=std::exp((Lpara.limList)[Lpara.n]);
        else tmpLambda=std::exp((Lpara.limList)[0]);
        D=delta;
        D.mul(tmpLambda);
        D.add(1);
        calcuPatb(D,atPb);
        calcuPPatb(D,atPb,atPPb);
        calcuTrP(tmpLambda,atPb,atPPb,tmpTrP);
        calcuREMLDLogL1(tmpLambda,tmpTrP,atPb,atPPb,tmpDLL1);
        calcuREMLLogL(tmpLambda,atPb,atIb,tmpLL);
        outLambda=tmpLambda;
        outLL=tmpLL;
        return true;
    }

    for(i=0;i<choosedNum;++i){
        tmpLambda=(Lpara.dList)[i];
        lam1=(Lpara.dList)[i];
        lam2=(Lpara.uList)[i];
        nIter=0;
        isNewtonRight=false;
        uDll=1e300,lDll=-1e300;
        while(nIter<1000){
             D=delta;
             D.mul(tmpLambda);
             D.add(1);
             calcuPatb(D,atPb);
             calcuPPatb(D,atPb,atPPb);
             calcuTrP(tmpLambda,atPb,atPPb,tmpTrP);
             calcuREMLDLogL1(tmpLambda,tmpTrP,atPb,atPPb,tmpDLL1);
             calcuPPPatb(D,atPb,atPPb,atPPPb);
             calcuTrPP(tmpLambda,atPb,atPPb,atPPPb,tmpTrPP);
             calcuREMLDLogL2(tmpLambda,tmpTrP,tmpTrPP,atPb,atPPb,atPPPb,tmpDLL2);
             if(tmpDLL1>0){
                 if(tmpDLL1<uDll){
                     uDll = tmpDLL1;
                     lam2 = tmpLambda;
                 }
             }
             else{
                 if(tmpDLL1>lDll){
                     lDll = tmpDLL1;
                     lam1 = tmpLambda;
                 }
             }
             deltaDLL=tmpDLL1/tmpDLL2;
             tmpLambda-=deltaDLL;
             if(std::abs(deltaDLL)<eps){
                 isNewtonRight=true;
                 break;
             }
             ++nIter;
        }

        if(!isNewtonRight){
            nIter=0;
            while(nIter<10000){
                 lamm=(lam1+lam2)/2;
                 D=delta;
                 D.mul(lamm);
                 D.add(1);
                 calcuPatb(D,atPb);
                 calcuPPatb(D,atPb,atPPb);
                 calcuTrP(lamm,atPb,atPPb,tmpTrP);
                 calcuREMLDLogL1(lamm,tmpTrP,atPb,atPPb,tmpDLL1);
                 tmpLambda=lamm;
                 if(std::abs(lam1-lam2)<eps){
                     break;
                 }
                 if(tmpDLL1>0) lam1=lamm;
                 else lam2=lamm;
                 ++nIter;
            }
        }

        calcuREMLLogL(tmpLambda,atPb,atIb,tmpLL);
        if(reLL<tmpLL){

            reLambda=tmpLambda;
            reLL=tmpLL;
        }
    }

    outLambda=reLambda;
    outLL=reLL;
    return true;
}

bool GEMMA::MLM::calcuFWald(double lambda,double& outBetax,
                            double& outFWald){
    MML::Mat atPb,D;
    long long Ixx,Iyy,Ixy;
    double betax=0,varBetax=0,tau=0;
    Ixx=(factorN)*(factorN+1)/2+factorN; //(i-1)*i/2+j-1;
    Iyy=(factorN+1)*(factorN+2)/2+factorN+1;
    Ixy=(factorN+1)*(factorN+2)/2+factorN;
    D=delta;
    D.mul(lambda);
    D.add(1);
    calcuPatb(D,atPb);
    betax=atPb(factorN,Ixy)/atPb(factorN,Ixx);
    tau=(filterN-factorN-1)/atPb(factorN+1,Iyy);
    varBetax=atPb(factorN+1,Iyy)/((filterN-factorN-1)*atPb(factorN,Ixx));
    outBetax=betax;
    outFWald=(atPb(factorN,Iyy)-atPb(factorN+1,Iyy))*tau;
    return true;
}
/* n = filterN
 * s = sum(K)
 * I = colVec_{n}(1.0)
 * kl = K %*% I --> the colnum vector recording the sum of ench line in K
 * outMat = K - (kl%*%tI+tI%*%tkl)/n + Mat(s/(n*n))_{n*n}
*/
bool GEMMA::MLM::centerKin(const MML::Mat &inMat, MML::Mat &outMat)
{
    if(inMat.info != MML::_sym) return false;
    size_t n = inMat.getNCol();
    double* kl = new double [n];
    size_t i=0,j=0;
    double s = inMat.sum();
    for(i=0;i<n;++i){
        kl[i] = 0;
        for(j=0;j<n;++j){
            kl[i] += inMat(i,j);
        }
    }
    outMat = inMat;
    for(i=0;i<n;++i){
        for(j=0;j<n;++j){
            outMat(i,j) += (s/(n*n) - (kl[i]+kl[j])/n);
        }
    }
    outMat.toSym('L');
    delete [] kl;
    return true;
}

GEMMA::GLM::GLM(const MML::Phenotype&  inPhe,
                const MML::Covariates& inCov)
    :GLM(){
    read(inPhe,inCov);
}

GEMMA::GLM::GLM(){
    eps=MML::EPS;
    isValid=false;
}

GEMMA::GLM::~GLM(){}

bool GEMMA::GLM::read(const MML::Phenotype&  inPhe,
                      const MML::Covariates& inCov){
    double yty,ytKKtKiKty;
    if(!(inPhe.isValid)||
       !(inCov.isValid)||
       inPhe.filterN!=inCov.filterN||
       inCov.factorN==0){
        isValid=false;
    } else {
        K=inCov.getFilterMat();
        y=inPhe.getFilterVec();
        filterN=inPhe.filterN;
        factorN=inCov.factorN;
        K.t(Kt);
        MML::Mat::XtXmul(K,KtK);
        if(!(KtK.symInv(KtKi))){
            isValid=false;
            return false;
        }
        MML::Mat::XVmul(Kt,y,Kty);
        MML::Mat::VtUmul(y,y,yty);
        MML::Mat::XVmul(KtKi,Kty,KtKiKty);
        MML::Mat::VtUmul(KtKiKty,Kty,ytKKtKiKty);
        yPKy=yty-ytKKtKiKty;
        isValid=true;
    }
    return isValid;
}

bool GEMMA::GLM::analyze(const MML::Mat& x,double& outBetax,
                         double& outFstat){
    if(!isValid||
       (x.info!=MML::_colvec&&x.info!=MML::_rowvec)||
       x.getNCol()*x.getNRow()!=filterN) return false;
    MML::Mat Ktx,KtKiKtx;
    double xtx,xtKKtKiKtx,xty,xtKKtKiKty,xPKx,xPKy,B,VarB;
    if(!MML::Mat::XVmul(Kt,x,Ktx)) return false;
    if(!MML::Mat::XVmul(KtKi,Ktx,KtKiKtx)) return false;
    if(!MML::Mat::VtUmul(KtKiKtx,Ktx,xtKKtKiKtx)) return false;
    if(!MML::Mat::VtUmul(KtKiKty,Ktx,xtKKtKiKty)) return false;
    if(!MML::Mat::VtUmul(x,x,xtx)) return false;
    if(!MML::Mat::VtUmul(x,y,xty)) return false;
    xPKx=xtx-xtKKtKiKtx;
    xPKy=xty-xtKKtKiKty;
    if(std::abs(xPKx)<eps) return false;
    B=xPKy/xPKx;
    VarB=(yPKy-xPKy*xPKy/xPKx)/((filterN-factorN-1)*xPKx);
    outBetax=B;
    outFstat=B*B/VarB;
    if(std::abs(VarB)<eps) return false;
    if(outFstat<0) return false;
    return true;
}

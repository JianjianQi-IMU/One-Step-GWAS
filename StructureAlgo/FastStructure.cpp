#include "FastStructure.hpp"
#include <QDebug>

namespace MML {

FastStructureParam::FastStructureParam()
{

}

FastStructurePData::FastStructurePData()
{

}

void FastStructurePData::initialize(const FastStructureParam &inParam)
{
    std::uniform_real_distribution<double> uniDis(0,1);
    std::default_random_engine generator;
    Distribution dis;
    double tVarBetaVal=0.0,tVarGammaVal=0.0;
    if(inParam.prior == FS_SIMPLE){
        PBeta.setData(inParam.nMarker,inParam.nPop,1.0);
        PGamma.setData(inParam.nMarker,inParam.nPop,1.0);
    }
    else if(inParam.prior == FS_LOGISTIC){
        PMu.setData(inParam.nMarker,1,0.0,_colvec);
        PLambda.setData(inParam.nPop,1,1.0,_colvec);
    }
    PVarBeta.resize(inParam.nMarker,inParam.nPop);
    PVarGamma.resize(inParam.nMarker,inParam.nPop);
    PZetaBeta.resize(inParam.nMarker,inParam.nPop);
    PZetaGamma.resize(inParam.nMarker,inParam.nPop);
    for(size_t iMarker=0;iMarker<inParam.nMarker;++iMarker){
        for(size_t iPop=0;iPop<inParam.nPop;++iPop){
            tVarBetaVal=0.1*uniDis(generator)+1.0;
            tVarGammaVal=0.1*uniDis(generator)+10.0;
            PVarBeta(iMarker,iPop)=tVarBetaVal;
            PVarGamma(iMarker,iPop)=tVarGammaVal;
            PZetaBeta(iMarker,iPop)=std::exp(dis.digamma(tVarBetaVal)-dis.digamma(tVarBetaVal+tVarGammaVal));
            PZetaGamma(iMarker,iPop)=std::exp(dis.digamma(tVarGammaVal)-dis.digamma(tVarBetaVal+tVarGammaVal));
        }
    }
}

FastStructureQData::FastStructureQData()
{

}

void FastStructureQData::initialize(const FastStructureParam &inParam)
{
    std::uniform_real_distribution<double> uniDis(0,1);
    std::default_random_engine generator;
    Distribution dis;
    double tSumQVal=0.0,tdiSumQVal=0.0;
    alpha.setData(inParam.nPop,1,1.0/inParam.nPop);
    Q.resize(inParam.nSample,inParam.nPop);
    QXi.resize(inParam.nSample,inParam.nPop);
    for(size_t iSample=0;iSample<inParam.nSample;++iSample){
        tSumQVal=0.0;
        for(size_t iPop=0;iPop<inParam.nPop;++iPop){
            Q(iSample,iPop)=0.1*uniDis(generator)+1.0;
            tSumQVal+=Q(iSample,iPop);
        }
        tdiSumQVal=dis.digamma(tSumQVal);
        for(size_t iPop=0;iPop<inParam.nPop;++iPop){
            QXi(iSample,iPop)=std::exp(dis.digamma(Q(iSample,iPop))-tdiSumQVal);
        }
    }
}

#define GPos(iMarker,iSample,iPloid) (((iMarker)*(nSample)*(nPloid))+((iSample)*(nPloid))+(iPloid))

void FastStructure::updatePSimple()
{
    size_t iMarker,iSample,iPloid,iPop;
    double sumThetaBeta=0.0,sumThetaGamma=0.0;
    double tGeno=0.0;
    double tVarBetaVal=0.0,tVarGammaVal=0.0;
    bool isMissing;
    Mat oldPVarBeta=PVarBeta,oldPVarGamma=PVarGamma;
    for(iMarker=0;iMarker<nMarker;++iMarker){
        for(iPop=0;iPop<nPop;++iPop){
            tmpVarBeta[iPop]  = 0.0;
            tmpVarGamma[iPop] = 0.0;
        }
        for(iSample=0;iSample<nSample;++iSample){
            tGeno = 0.0;
            isMissing = false;
            for(iPloid=0;iPloid<nPloid;++iPloid){
                if(G[GPos(iMarker,iSample,iPloid)]==UNASSIGNED){
                    isMissing=true;
                    break;
                }
                else if(G[GPos(iMarker,iSample,iPloid)]){
                    tGeno += 1.0;
                }
            }
            if(isMissing) continue;
            if(nPloid!=2) tGeno *= 2.0/nPloid;
            sumThetaBeta=0.0,sumThetaGamma=0.0;
            for(iPop=0;iPop<nPop;++iPop){
                sumThetaBeta += QXi(iSample,iPop)*PZetaBeta(iMarker,iPop);
                sumThetaGamma+= QXi(iSample,iPop)*PZetaGamma(iMarker,iPop);
            }
            for(iPop=0;iPop<nPop;++iPop){
                tmpVarBeta[iPop] += tGeno*QXi(iSample,iPop)/sumThetaBeta;
                tmpVarGamma[iPop]+= (2.0-tGeno)*QXi(iSample,iPop)/sumThetaGamma;
            }
        }
        for(iPop=0;iPop<nPop;++iPop){
            PVarBeta(iMarker,iPop) =PBeta(iMarker,iPop)+PZetaBeta(iMarker,iPop)*tmpVarBeta[iPop];
            PVarGamma(iMarker,iPop)=PGamma(iMarker,iPop)+PZetaGamma(iMarker,iPop)*tmpVarGamma[iPop];
        }
    }
    if(PVarBeta.anyNan()) PVarBeta=oldPVarBeta;
    if(PVarGamma.anyNan()) PVarGamma=oldPVarGamma;
    for(iMarker=0;iMarker<nMarker;++iMarker){
        for(iPop=0;iPop<nPop;++iPop){
            tVarBetaVal  = PVarBeta(iMarker,iPop) ;
            tVarGammaVal = PVarGamma(iMarker,iPop);
            PZetaBeta(iMarker,iPop) =std::exp(dis.digamma(tVarBetaVal)-dis.digamma(tVarBetaVal+tVarGammaVal));
            PZetaGamma(iMarker,iPop)=std::exp(dis.digamma(tVarGammaVal)-dis.digamma(tVarBetaVal+tVarGammaVal));

        }
    }
}

void FastStructure::updatePLogistic()
{

}

FastStructure::FastStructure()
{
    prior       = FS_SIMPLE;
    tolThre     = MML::EPS;
    G           = nullptr;
    tmpVarBeta  = nullptr;
    tmpVarGamma = nullptr;
}

FastStructure::~FastStructure()
{
    clear();
}

void FastStructure::resetP()
{
    std::uniform_real_distribution<double> uniDis(0,1);
    double tVarBetaVal=0.0,tVarGammaVal=0.0;
    if(prior == FS_SIMPLE){
        PBeta.setData(nMarker,nPop,1.0);
        PGamma.setData(nMarker,nPop,1.0);
    }
    else if(prior == FS_LOGISTIC){
        PMu.setData(nMarker,1,1.0,_colvec);
        PLambda.setData(nPop,1,1.0,_colvec);
    }
    PVarBeta.resize(nMarker,nPop);
    PVarGamma.resize(nMarker,nPop);
    PZetaBeta.resize(nMarker,nPop);
    PZetaGamma.resize(nMarker,nPop);
    for(size_t iMarker=0;iMarker<nMarker;++iMarker){
        for(size_t iPop=0;iPop<nPop;++iPop){
            tVarBetaVal=0.1*uniDis(generator)+1.0;
            tVarGammaVal=0.1*uniDis(generator)+10.0;
            PVarBeta(iMarker,iPop)=tVarBetaVal;
            PVarGamma(iMarker,iPop)=tVarGammaVal;
            PZetaBeta(iMarker,iPop)=std::exp(dis.digamma(tVarBetaVal)-dis.digamma(tVarBetaVal+tVarGammaVal));
            PZetaGamma(iMarker,iPop)=std::exp(dis.digamma(tVarGammaVal)-dis.digamma(tVarBetaVal+tVarGammaVal));
        }
    }
}

void FastStructure::resetQ()
{
    std::uniform_real_distribution<double> uniDis(0,1);
    double tSumQVal=0.0,tdiSumQVal=0.0;
    alpha.setData(nPop,1,1.0/nPop);
    Q.resize(nSample,nPop);
    QXi.resize(nSample,nPop);
    for(size_t iSample=0;iSample<nSample;++iSample){
        tSumQVal=0.0;
        for(size_t iPop=0;iPop<nPop;++iPop){
            Q(iSample,iPop)=0.1*uniDis(generator)+1.0;
            tSumQVal+=Q(iSample,iPop);
        }
        tdiSumQVal=dis.digamma(tSumQVal);
        for(size_t iPop=0;iPop<nPop;++iPop){
            QXi(iSample,iPop)=std::exp(dis.digamma(Q(iSample,iPop))-tdiSumQVal);
        }
    }
}

void FastStructure::initialize()
{
    clear();

    G=new short[nMarker*nSample*nPloid];
    tmpVarBeta  = new double[nPop];
    tmpVarGamma = new double[nPop];

    resetP();
    resetQ();
}

void FastStructure::saveParam(FastStructureParam &outParam)
{
    outParam.nMarker = nMarker;
    outParam.nPop    = nPop;
    outParam.nAllele = nAllele;
    outParam.nSample = nSample;
    outParam.nPloid  = nPloid;
    outParam.prior   = prior;
}

void FastStructure::loadParam(const FastStructureParam &inParam)
{
    nMarker = inParam.nMarker;
    nPop    = inParam.nPop;
    nAllele = inParam.nAllele;
    nSample = inParam.nSample;
    nPloid  = inParam.nPloid;
    prior   = inParam.prior;
}

void FastStructure::savePData(FastStructurePData &outP)
{
    outP.PBeta      = PBeta     ;
    outP.PGamma     = PGamma    ;
    outP.PMu        = PMu       ;
    outP.PLambda    = PLambda   ;
    outP.PVarBeta   = PVarBeta  ;
    outP.PVarGamma  = PVarGamma ;
    outP.PZetaBeta  = PZetaBeta ;
    outP.PZetaGamma = PZetaGamma;
}

void FastStructure::saveQData(FastStructureQData &outQ)
{
    outQ.Q     = Q    ;
    outQ.QXi   = QXi  ;
    outQ.alpha = alpha;
}

void FastStructure::loadPData(const FastStructurePData &inP)
{
    PBeta      = inP.PBeta     ;
    PGamma     = inP.PGamma    ;
    PMu        = inP.PMu       ;
    PLambda    = inP.PLambda   ;
    PVarBeta   = inP.PVarBeta  ;
    PVarGamma  = inP.PVarGamma ;
    PZetaBeta  = inP.PZetaBeta ;
    PZetaGamma = inP.PZetaGamma;
}

void FastStructure::loadQData(const FastStructureQData &inQ)
{
    Q     = inQ.Q    ;
    QXi   = inQ.QXi  ;
    alpha = inQ.alpha;
}

void FastStructure::updateP()
{
    if(prior == FS_SIMPLE) updatePSimple();
    else if(prior == FS_LOGISTIC) updatePLogistic();
}

void FastStructure::updateP2()
{
    Mat tVB1,tVB2,tVB3,tVG1,tVG2,tVG3;
    Mat dVB1,dVB2,dVG1,dVG2;
    size_t iMarker,iPop;
    double vt=0.0,sumFabsdVB1=0.0,sumFabsdVB2=0.0;
    double sumFabsdVG1=0.0,sumFabsdVG2=0.0;
    bool flag=false,isLess=false;
    tVB1=PVarBeta,tVG1=PVarGamma;
    updateP();
    tVB2=PVarBeta,tVG2=PVarGamma;
    updateP();
    tVB3=PVarBeta,tVG3=PVarGamma;
    dVB1.resize(nMarker,nPop),dVG1.resize(nMarker,nPop);
    dVB2.resize(nMarker,nPop),dVG2.resize(nMarker,nPop);
    for(iMarker=0;iMarker<nMarker;++iMarker){
        for(iPop=0;iPop<nPop;++iPop){
            dVB1(iMarker,iPop)=tVB2(iMarker,iPop)-tVB1(iMarker,iPop);
            dVG1(iMarker,iPop)=tVG2(iMarker,iPop)-tVG1(iMarker,iPop);
            dVB2(iMarker,iPop)=tVB3(iMarker,iPop)-tVB2(iMarker,iPop)-dVB1(iMarker,iPop);
            dVG2(iMarker,iPop)=tVG3(iMarker,iPop)-tVG2(iMarker,iPop)-dVG1(iMarker,iPop);
            sumFabsdVB1+=dVB1(iMarker,iPop)*dVB1(iMarker,iPop);
            sumFabsdVB2+=dVB2(iMarker,iPop)*dVB2(iMarker,iPop);
            sumFabsdVG1+=dVG1(iMarker,iPop)*dVG1(iMarker,iPop);
            sumFabsdVG2+=dVG2(iMarker,iPop)*dVG2(iMarker,iPop);
        }
    }
    vt = -std::sqrt((sumFabsdVB1+sumFabsdVG1)/(sumFabsdVB2+sumFabsdVG2));
    if(vt > -1.0) vt = -1.0;
    while(!flag){
        isLess=false;
        for(iMarker=0;iMarker<nMarker;++iMarker){
            for(iPop=0;iPop<nPop;++iPop){
                PVarBeta(iMarker,iPop) = (vt+1.0)*(vt+1.0)*tVB1(iMarker,iPop)
                        - 2*vt*(vt+1.0)*tVB2(iMarker,iPop)
                        + vt*vt*tVB3(iMarker,iPop);
                PVarGamma(iMarker,iPop) = (vt+1.0)*(vt+1.0)*tVG1(iMarker,iPop)
                        - 2*vt*(vt+1.0)*tVG2(iMarker,iPop)
                        + vt*vt*tVG3(iMarker,iPop);
                if(PVarBeta(iMarker,iPop)<=0 || PVarGamma(iMarker,iPop)<=0) isLess=true;
            }
        }
        if(isLess){
            vt = 0.5*(vt-1);
            if(std::fabs(vt+1.0)<1e-4) vt = -1.0;
        }
        else flag=true;
    }
    if(PVarBeta.anyNan()||PVarGamma.anyNan()){
        PVarBeta=tVB2;
        PVarGamma=tVG2;
    }
    for(iMarker=0;iMarker<nMarker;++iMarker){
        for(iPop=0;iPop<nPop;++iPop){
            PZetaBeta(iMarker,iPop) = std::exp(dis.digamma(PVarBeta(iMarker,iPop))-
                        dis.digamma(PVarBeta(iMarker,iPop)+PVarGamma(iMarker,iPop)));
            PZetaGamma(iMarker,iPop) = std::exp(dis.digamma(PVarGamma(iMarker,iPop))-
                        dis.digamma(PVarBeta(iMarker,iPop)+PVarGamma(iMarker,iPop)));
        }
    }
}

void FastStructure::updateQ()
{
    size_t iMarker,iSample,iPloid,iPop;
    double sumThetaBeta=0.0,sumThetaGamma=0.0;
    double tGeno=0.0;
    double tSumQVal=0.0,tdiSumQVal=0.0;
    bool isMissing;
    Mat oldQ=Q;
    Q.setData(nSample,nPop,0.0);
    for(iSample=0;iSample<nSample;++iSample){
        for(iMarker=0;iMarker<nMarker;++iMarker){
            tGeno = 0.0;
            isMissing = false;
            for(iPloid=0;iPloid<nPloid;++iPloid){
                if(G[GPos(iMarker,iSample,iPloid)]==UNASSIGNED){
                    isMissing=true;
                    break;
                }
                else if(G[GPos(iMarker,iSample,iPloid)]){
                    tGeno += 1.0;
                }
            }
            if(isMissing) continue;
            if(nPloid!=2) tGeno *= 2.0/nPloid;
            sumThetaBeta=0.0,sumThetaGamma=0.0;
            for(iPop=0;iPop<nPop;++iPop){
                sumThetaBeta += QXi(iSample,iPop)*PZetaBeta(iMarker,iPop);
                sumThetaGamma+= QXi(iSample,iPop)*PZetaGamma(iMarker,iPop);
            }
            for(iPop=0;iPop<nPop;++iPop){
                Q(iSample,iPop) += ( (2.0-tGeno)*PZetaGamma(iMarker,iPop)/sumThetaGamma +
                        tGeno*PZetaBeta(iMarker,iPop)/sumThetaBeta ) * QXi(iSample,iPop);
            }
        }
    }
    if(Q.anyNan()) Q=oldQ;
    else{
        for(iSample=0;iSample<nSample;++iSample){
            tSumQVal=0.0;
            for(iPop=0;iPop<nPop;++iPop){
                Q(iSample,iPop) += alpha(iPop);
                tSumQVal += Q(iSample,iPop);
            }
            tdiSumQVal=dis.digamma(tSumQVal);
            for(size_t iPop=0;iPop<nPop;++iPop){
                QXi(iSample,iPop)=std::exp(dis.digamma(Q(iSample,iPop))-tdiSumQVal);
            }
        }
    }
}

void FastStructure::updateQ2()
{
    Mat tQ1,tQ2,tQ3;
    Mat dQ1,dQ2;
    size_t iSample,iPop;
    double vt=0.0,sumFabsdQ1=0.0,sumFabsdQ2=0.0;
    double tSumQVal=0.0,tdiSumQVal=0.0;
    bool flag=false,isLess=false;
    tQ1=Q;
    updateQ();
    tQ2=Q;
    updateQ();
    tQ3=Q;
    dQ1.resize(nSample,nPop),dQ2.resize(nSample,nPop);
    for(iSample=0;iSample<nSample;++iSample){
        for(iPop=0;iPop<nPop;++iPop){
            dQ1(iSample,iPop)=tQ2(iSample,iPop)-tQ1(iSample,iPop);
            dQ2(iSample,iPop)=tQ3(iSample,iPop)-tQ2(iSample,iPop)-dQ1(iSample,iPop);
            sumFabsdQ1+=dQ1(iSample,iPop)*dQ1(iSample,iPop);
            sumFabsdQ2+=dQ2(iSample,iPop)*dQ2(iSample,iPop);
        }
    }
    vt = -std::sqrt(sumFabsdQ1/sumFabsdQ2);
    if(vt > -1.0) vt = -1.0;
    while(!flag){
        isLess=false;
        for(iSample=0;iSample<nSample;++iSample){
            for(iPop=0;iPop<nPop;++iPop){
                Q(iSample,iPop) = (vt+1.0)*(vt+1.0)*tQ1(iSample,iPop)
                        - 2*vt*(vt+1.0)*tQ2(iSample,iPop)
                        + vt*vt*tQ3(iSample,iPop);
                if(Q(iSample,iPop)<=0) isLess=true;
            }
        }
        if(isLess){
            vt = 0.5*(vt-1);
            if(std::fabs(vt+1.0)<1e-4) vt = -1.0;
        }
        else flag=true;
    }
    if(Q.anyNan()) Q=tQ2;
    for(iSample=0;iSample<nSample;++iSample){
        tSumQVal=0.0;
        for(iPop=0;iPop<nPop;++iPop){
            tSumQVal += Q(iSample,iPop);
        }
        tdiSumQVal=dis.digamma(tSumQVal);
        for(size_t iPop=0;iPop<nPop;++iPop){
            QXi(iSample,iPop)=std::exp(dis.digamma(Q(iSample,iPop))-tdiSumQVal);
        }
    }
}

void FastStructure::updatePHyper(bool noLambda)
{
    size_t iMarker,iPop;
    double sumLambda=0.0,tmp=0.0;
    Mat tDat,tDiff;
    if(prior == FS_LOGISTIC){
        tDat.resize(nMarker,nPop);
        tDiff.resize(nMarker,nPop);
        for(iPop=0;iPop<nPop;++iPop){
            sumLambda+=PLambda(iPop);
        }
        for(iMarker=0;iMarker<nMarker;++iMarker){
            tmp=0.0;
            for(iPop=0;iPop<nPop;++iPop){
                tDat(iMarker,iPop)=dis.digamma(PVarBeta(iMarker,iPop)-dis.digamma(PVarGamma(iMarker,iPop)));
                tmp += tDat(iMarker,iPop)*PLambda(iPop);
            }
            PMu(iMarker) = tmp/sumLambda;
        }
        for(iPop=0;iPop<nPop;++iPop){
            for(iMarker=0;iMarker<nMarker;++iMarker){
                tDiff(iMarker,iPop) = tDat(iMarker,iPop)-PMu(iMarker);
            }
        }
        if(!noLambda){
            for(iPop=0;iPop<nPop;++iPop){
                tmp=0.0;
                for(iMarker=0;iMarker<nMarker;++iMarker){
                    tmp += tDiff(iMarker,iPop)*tDiff(iMarker,iPop);
                    tmp += dis.polygamma(1,PVarBeta(iMarker,iPop));
                    tmp += dis.polygamma(1,PVarGamma(iMarker,iPop));
                }
                PLambda(iPop) = nMarker / tmp;
            }
        }
    }
}

double FastStructure::marginalLikelihood()
{
    size_t iMarker,iSample,iPloid,iPop;
    bool isMissing;
    double sumThetaBeta=0.0,sumThetaGamma=0.0;
    double lnSumThetaBeta=0.0,lnSumThetaGamma=0.0;
    double sumAlpha=0.0,lgammaSumAlpha=0.0;
    double t1=0.0,T1=0.0,T2=0.0;
    double tmpE=0.0,E1=0.0,E2=0.0,E3=0.0,out=0.0;
    for(iMarker=0;iMarker<nMarker;++iMarker){
        for(iSample=0;iSample<nSample;++iSample){
            sumThetaBeta=0.0,sumThetaGamma=0.0;
            for(iPop=0;iPop<nPop;++iPop){
                sumThetaBeta += QXi(iSample,iPop)*PZetaBeta(iMarker,iPop);
                sumThetaGamma+= QXi(iSample,iPop)*PZetaGamma(iMarker,iPop);
            }
            lnSumThetaBeta=std::log(sumThetaBeta),lnSumThetaGamma=std::log(sumThetaGamma);
//            qDebug() << "lnSumThetaBeta" << lnSumThetaBeta;
            isMissing = false;
            tmpE=0.0;
            for(iPloid=0;iPloid<nPloid;++iPloid){
                if(G[GPos(iMarker,iSample,iPloid)]==UNASSIGNED){
                    isMissing=true;
                    break;
                }
                else if(G[GPos(iMarker,iSample,iPloid)]){
                    tmpE += lnSumThetaBeta;
                }
                else{
                    tmpE += lnSumThetaGamma;
                }
            }
            if(isMissing) continue;
            E1+=tmpE;
//            qDebug() << "tmpE" << tmpE;
        }
    }
//    qDebug() << "QXi(0,0):" << QXi(0,0);

    for(iPop=0;iPop<nPop;++iPop) sumAlpha+=alpha(iPop);
    lgammaSumAlpha = dis.lgamma(sumAlpha);
    for(iSample=0;iSample<nSample;++iSample){
        T1=0.0,T2=0.0;
        for(iPop=0;iPop<nPop;++iPop){
            t1=dis.lgamma(Q(iSample,iPop))-dis.lgamma(alpha(iPop));
            if(QXi(iSample,iPop)>0){
                t1 -= (Q(iSample,iPop)-alpha(iPop))*std::log(QXi(iSample,iPop));
            }
            T1 += t1;
            T2 += Q(iSample,iPop);
        }
        E2 += T1 - dis.lgamma(T2) + lgammaSumAlpha;
    }
    if(prior==FS_SIMPLE){
        for(iMarker=0;iMarker<nMarker;++iMarker){
            for(iPop=0;iPop<nPop;++iPop){
                t1=dis.lgamma(PVarBeta(iMarker,iPop))-dis.lgamma(PBeta(iMarker,iPop))
                        +dis.lgamma(PVarGamma(iMarker,iPop))-dis.lgamma(PGamma(iMarker,iPop))
                        -dis.lgamma(PVarBeta(iMarker,iPop)+PVarGamma(iMarker,iPop))
                        +dis.lgamma(PBeta(iMarker,iPop)+PGamma(iMarker,iPop));
                if(PZetaBeta(iMarker,iPop)>0){
                    t1 -= (PVarBeta(iMarker,iPop)-PBeta(iMarker,iPop))*std::log(PZetaBeta(iMarker,iPop));
                }
                if(PZetaGamma(iMarker,iPop)>0){
                    t1 -= (PVarGamma(iMarker,iPop)-PGamma(iMarker,iPop))*std::log(PZetaGamma(iMarker,iPop));
                }
                E3 += t1;
            }
        }
    }
    else if(prior==FS_LOGISTIC){

    }
    out = (E1+E2+E3)/(nSample*nMarker);

    return out;
}

short *FastStructure::genotype(size_t iMarker, size_t iSample, size_t iPloid)
{
    return &(G[GPos(iMarker,iSample,iPloid)]);
}

void FastStructure::batchInit(FastStructurePData &outP,
                              FastStructureQData &outQ)
{
    FastStructurePData tPData;
    double LL,tLL,tolIter=1e300;
    size_t iter=0,i;
    updateQ();
    updateP();
    LL=marginalLikelihood();
    while(std::fabs(tolIter)>100*tolThre){
        updateQ2();
        updateP2();
        ++iter;
        if(iter%10==0){
            tLL=marginalLikelihood();
            tolIter=tLL-LL;
            LL=tLL;
            updatePHyper(false);
        }
    }
    savePData(outP);
    saveQData(outQ);
}

void FastStructure::getQ(Mat &outQ)
{
    size_t iSample,iPop;
    double tSum=0.0;
    outQ.resize(nSample,nPop);
    for(iSample=0;iSample<nSample;++iSample){
        tSum=0.0;
        for(iPop=0;iPop<nPop;++iPop){
            tSum += Q(iSample,iPop);
        }
        for(iPop=0;iPop<nPop;++iPop){
            outQ(iSample,iPop) = Q(iSample,iPop)/tSum;
        }
    }
}

const short *FastStructure::getG() const
{
    return G;
}

void FastStructure::clear()
{
    if(G) delete [] G;
    if(tmpVarBeta) delete [] tmpVarBeta;
    if(tmpVarGamma) delete [] tmpVarGamma;

    G           = nullptr;
    tmpVarBeta  = nullptr;
    tmpVarGamma = nullptr;
}

double FastStructure::expectGenotype(const FastStructurePData &P,
                                     const FastStructureQData &Q,
                                     const FastStructureParam &Para,
                                     size_t n,
                                     size_t l)
{
//    size_t nMarker           = Para.nMarker;
    size_t nPop              = Para.nPop   ;
//    size_t nAllele           = Para.nAllele;
//    size_t nSample           = Para.nSample;
//    size_t nPloid            = Para.nPloid ;
//    FastStructurePrior prior = Para.prior  ;
    size_t iPop=0;
    double sumQXin=0.0,tPVal=0.0,out=0.0;
    Mat QXin,QXini,PB,PG,PBi,PGi,PP;

    QXin = Q.QXi(n,0,n,nPop-1);
    for(iPop=0;iPop<nPop;++iPop) sumQXin += QXin(iPop);
    QXini.resize(1,nPop);
    QXin.toRowVec(),QXini.toRowVec();
    for(iPop=0;iPop<nPop;++iPop) QXini(iPop) = QXin(iPop)/sumQXin;
    PB = P.PVarBeta(l,0,l,nPop-1);
    PG = P.PVarGamma(l,0,l,nPop-1);
    PBi.resize(1,nPop),PGi.resize(1,nPop);
    for(iPop=0;iPop<nPop;++iPop){
        PBi(iPop) = PB(iPop)/(PB(iPop)+PG(iPop));
        PGi(iPop) = PG(iPop)/(PB(iPop)+PG(iPop));
    }
    PB.toRowVec(),PG.toRowVec();
    PBi.toRowVec(),PGi.toRowVec();

    /*
    Mat::XtYmul(PGi,PGi,PP);
    for(iPop=0;iPop<nPop;++iPop){
        PP(iPop,iPop)=PGi(iPop)*(PG(iPop)+1)/(PB(iPop)+PG(iPop)+1);
    }
    Mat::VtXUmul(QXini,PP,QXini,tPVal);
    */

    Mat::XtYmul(PBi,PGi,PP);
    for(iPop=0;iPop<nPop;++iPop){
        PP(iPop,iPop)=PBi(iPop)*PG(iPop)/(PB(iPop)+PG(iPop)+1);
    }
    Mat::VtXUmul(QXini,PP,QXini,tPVal);
    out += 2*tPVal;

    Mat::XtYmul(PBi,PBi,PP);
    for(iPop=0;iPop<nPop;++iPop){
        PP(iPop,iPop)=PBi(iPop)*(PB(iPop)+1)/(PB(iPop)+PG(iPop)+1);
    }
    Mat::VtXUmul(QXini,PP,QXini,tPVal);
    out += 2*tPVal;

    return out;
}

void FastStructure::calcuCV(const short *G,
                            const FastStructurePData &P,
                            const FastStructureQData &Q,
                            const FastStructureParam &Para,
                            std::vector<double> &outMeanDeviance,
                            size_t nCV)
{
    size_t nMarker           = Para.nMarker;
    size_t nPop              = Para.nPop   ;
    size_t nAllele           = Para.nAllele;
    size_t nSample           = Para.nSample;
    size_t nPloid            = Para.nPloid ;
    FastStructurePrior prior = Para.prior  ;
    size_t nLoci = nMarker*nSample,nMissing=0,nDevs;
    size_t nSampling;
    size_t iMarker,iSample,iPloid,iLoci,iCV,iter=0;
    bool isMissing,badMask,allMissing;
    std::vector<size_t> samplingList(nLoci);
    double LL,tLL,diffLL,tGeno,nu,tDev,t,sumDev;
    short* bMissing = new short[nLoci];
    short* ibMissing = new short[nLoci];
    FastStructure structure;
    FastStructurePData tP;
    FastStructureQData tQ;

    outMeanDeviance.clear();
    structure.loadParam(Para);
    structure.initialize();

    for(iSample=0;iSample<nSample;++iSample){
        for(iMarker=0;iMarker<nMarker;++iMarker){
            isMissing = false;
            for(iPloid=0;iPloid<nPloid;++iPloid){
                if(G[GPos(iMarker,iSample,iPloid)]==UNASSIGNED){
                    isMissing = true;
                    break;
                }
            }
            if(isMissing){
                nMissing++;
                ibMissing[iMarker*nSample+iSample]=0;
            }
            else{
                ibMissing[iMarker*nSample+iSample]=1;
            }
        }
    }

    nSampling = std::min(size_t(1000),size_t(0.01*(nLoci-nMissing)));

    for(iCV=0;iCV<nCV;++iCV){
        badMask = true;
        while(badMask){
            std::iota(samplingList.begin(),samplingList.end(),0);
            std::random_shuffle(samplingList.begin(),samplingList.end());
            for(iLoci=0;iLoci<nLoci;++iLoci) bMissing[iLoci]=ibMissing[iLoci];
            badMask = false;
            for(iLoci=0;iLoci<2*nSampling;++iLoci){
                iMarker=samplingList[iLoci]/nSample;
                iSample=samplingList[iLoci]%nSample;
                bMissing[iMarker*nSample+iSample]=0;
                allMissing = true;
                for(iSample=0;iSample<nSample;++iSample){
                    if(bMissing[iMarker*nSample+iSample]){
                        allMissing = false;
                        break;
                    }
                }
                if(allMissing){
                    badMask = true;
                    break;
                }
            }
        }
        for(iSample=0;iSample<nSample;++iSample){
            for(iMarker=0;iMarker<nMarker;++iMarker){
                if(bMissing[iMarker*nSample+iSample]){
                    for(iPloid=0;iPloid<nPloid;++iPloid){
                        *(structure.genotype(iMarker,iSample,iPloid))=G[GPos(iMarker,iSample,iPloid)];
                    }
                }
                else{
                    for(iPloid=0;iPloid<nPloid;++iPloid){
                        *(structure.genotype(iMarker,iSample,iPloid))=UNASSIGNED;
                    }
                }
            }
        }
        structure.loadPData(P);
        structure.loadQData(Q);
        iter=0;
        LL=structure.marginalLikelihood();
        while(std::fabs(diffLL)>EPS){
            structure.updateQ2();
            structure.updateQ();
            structure.updateP2();
            structure.updateP();
            if((iter+1)%10==0){
                tLL=structure.marginalLikelihood();
                diffLL=tLL - LL;
                LL = tLL;
                structure.updatePHyper(false);
            }
            ++iter;
        }
        structure.savePData(tP);
        structure.saveQData(tQ);
        sumDev = 0.0;
        nDevs = 0;
        for(iLoci=0;iLoci<2*nSampling;++iLoci){
            iMarker=samplingList[iLoci]/nSample;
            iSample=samplingList[iLoci]%nSample;
            tGeno = 0.0;
            isMissing = false;
            for(iPloid=0;iPloid<nPloid;++iPloid){
                if(G[GPos(iMarker,iSample,iPloid)]==UNASSIGNED){
                    isMissing=true;
                    break;
                }
                else if(G[GPos(iMarker,iSample,iPloid)]){
                    tGeno += 1.0;
                }
            }
            if(isMissing) continue;
            if(nPloid!=2) tGeno *= 2.0/nPloid;
            nu = expectGenotype(tP,tQ,Para,iSample,iMarker);
            tDev = 0.0;
            t = tGeno*std::log(tGeno/nu);
            if(std::isnan(t)) t=0.0;
            tDev += t;
            t = (2-tGeno)*std::log((2-tGeno)/(2-nu));
            if(std::isnan(t)) t=0.0;
            tDev += t;
            sumDev += tDev;
            ++nDevs;
        }
        outMeanDeviance.push_back(sumDev/nDevs);
    }

    delete [] bMissing;
    delete [] ibMissing;
}




#undef GPos

}

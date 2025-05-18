#include "Structure.hpp"

namespace MML {

StructureParam::StructureParam()
{
    nBurnIn=40000;
    nRecord=15000;
    nPopulation=4;
}

void StructureParam::setNBurnIn(size_t n)
{
    if(!n) nBurnIn=n;
}

void StructureParam::setNRecord(size_t n)
{
    if(!n) nRecord=n;
}

void StructureParam::setNPopulation(size_t n)
{
    if(!n) nPopulation=n;
}

#define PPos(iMarker,iPop,iAllele) (((iMarker)*(nPop)*(nAllele))+((iPop)*(nAllele))+(iAllele))
#define QPos(iSample,iPop) (((iSample)*(nPop))+(iPop))
#define ZPos(iMarker,iSample,iPloid) (((iMarker)*(nSample)*(nPloid))+((iSample)*(nPloid))+(iPloid))


Structure::Structure()
{
    G      = nullptr;
    P      = nullptr;
    Q      = nullptr;
    Z      = nullptr;
    lambda = nullptr;
    alpha  = nullptr;
    sumQ   = nullptr;
    isRecord=false;
}

Structure::Structure(size_t inNMarker, size_t inNPop, size_t inNAllele, size_t inNSample, size_t inNPloid)
    :Structure()
{
    setParameter(inNMarker,inNPop,inNAllele,inNSample,inNPloid);
}

Structure::~Structure()
{
    clear();
}

bool Structure::setParameter(size_t inNMarker, size_t inNPop, size_t inNAllele, size_t inNSample, size_t inNPloid)
{
    nMarker = inNMarker;
    nPop    = inNPop   ;
    nAllele = inNAllele;
    nSample = inNSample;
    nPloid  = inNPloid ;
    return true;
}

bool Structure::initialize()
{
    clear();
    G=new short[nMarker*nSample*nPloid];
    P=new double[nMarker*nPop*nAllele];
    Q=new double[nSample*nPop];
    Z=new int[nMarker*nSample*nPloid];
    lambda=new double[nPop];
    alpha=new double[nPop];
    sumQ=new double[nSample*nPop];

    size_t i,j;
    for(i=0;i<nPop;++i){
        lambda[i]=1.0;
        alpha[i]=1.0;
        for(j=0;j<nSample;++j){
            sumQ[QPos(j,i)]=0.0;
        }
    }

    return true;
}

bool Structure::randomizeZ()
{
    size_t iSample,iMarker,iPloid;
    std::uniform_int_distribution<int> distribution(0,nPop-1);
    for(iSample=0;iSample<nSample;++iSample){
        for(iMarker=0;iMarker<nMarker;++iMarker){
            for(iPloid=0;iPloid<nPloid;++iPloid){
                if(G[ZPos(iMarker,iSample,iPloid)]==UNASSIGNED){
                    Z[ZPos(iMarker,iSample,iPloid)]=UNASSIGNED;
                }
                else{
                    Z[ZPos(iMarker,iSample,iPloid)]=distribution(generator);
                }
            }
        }
    }
    return true;
}

bool Structure::updateP()
{
    size_t iSample,iMarker,iPop,iAllele,iPloid,i;
    int tmpPop,tmpAllele;
    size_t *alleleNum=new size_t[nAllele*nPop];
    double *para=new double[nAllele];
    for(iMarker=0;iMarker<nMarker;++iMarker){
        for(i=0;i<nAllele*nPop;++i) alleleNum[i]=0;
        for(iSample=0;iSample<nSample;++iSample){
            for(iPloid=0;iPloid<nPloid;++iPloid){
                tmpAllele=G[ZPos(iMarker,iSample,iPloid)];
                tmpPop=Z[ZPos(iMarker,iSample,iPloid)];
                if(tmpAllele==UNASSIGNED||tmpPop==UNASSIGNED) continue;
                ++(alleleNum[tmpAllele*nPop+tmpPop]);
            }
        }

        for(iPop=0;iPop<nPop;++iPop){
            for(iAllele=0;iAllele<nAllele;++iAllele){
                para[iAllele]=lambda[iPop]+alleleNum[iAllele*nPop+iPop];
            }
            dis.genDirichletRandom(para,P+PPos(iMarker,iPop,0),nAllele);
        }
    }
    delete [] alleleNum;
    delete [] para;
    return true;
}

bool Structure::updateQ()
{
    size_t iSample,iMarker,iPloid,i;
    int tmpPop;
    double *para=new double[nPop];
    for(iSample=0;iSample<nSample;++iSample){
        for(i=0;i<nPop;++i) para[i]=alpha[i];

        for(iMarker=0;iMarker<nMarker;++iMarker){
            for(iPloid=0;iPloid<nPloid;++iPloid){
                tmpPop=Z[ZPos(iMarker,iSample,iPloid)];
                if(tmpPop==UNASSIGNED) continue;
                ++(para[tmpPop]);
            }
        }

        dis.genDirichletRandom(para,Q+QPos(iSample,0),nPop);
        if(isRecord){
            for(i=0;i<nPop;++i){
                sumQ[QPos(iSample,i)]+=Q[QPos(iSample,i)];
            }
        }
    }
    delete [] para;
    return true;
}

bool Structure::updateZ()
{
    size_t iSample,iMarker,iPop,iPloid;
    int tmpAllele;
    double sum=0.0,threshold;
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    double* popPro=new double[nPop];
    for(iMarker=0;iMarker<nMarker;++iMarker){
        for(iSample=0;iSample<nSample;++iSample){
            for(iPloid=0;iPloid<nPloid;++iPloid){
                tmpAllele=G[ZPos(iMarker,iSample,iPloid)];
                if(tmpAllele==UNASSIGNED){
                    Z[ZPos(iMarker,iSample,iPloid)]=UNASSIGNED;
                }
                else{
                    sum = 0.0;
                    for(iPop=0;iPop<nPop;++iPop){
                        popPro[iPop]=Q[QPos(iSample,iPop)]*P[PPos(iMarker,iPop,tmpAllele)];
                        sum+=popPro[iPop];
                    }
                    for(iPop=0;iPop<nPop;++iPop){
                        popPro[iPop]/=sum;
                    }
                    threshold=distribution(generator);
                    sum = 0.0;
                    for(iPop=0;iPop<nPop;++iPop){
                        sum+=popPro[iPop];
                        if(sum-threshold>-MML::EPS) break;
                    }
                    Z[ZPos(iMarker,iSample,iPloid)]=iPop;

                }
            }
        }
    }
    delete [] popPro;
    return true;
}

short *Structure::genotype(size_t iMarker, size_t iSample, size_t iPloid)
{
    return &(G[ZPos(iMarker,iSample,iPloid)]);
}

double Structure::getSumQ(size_t iSample, size_t iPop)
{
    return sumQ[QPos(iSample,iPop)];
}

void Structure::setRecord(bool state)
{
    isRecord=state;
}

void Structure::clear()
{
    if(G)      delete [] G;
    if(P)      delete [] P;
    if(Q)      delete [] Q;
    if(Z)      delete [] Z;
    if(lambda) delete [] lambda;
    if(alpha)  delete [] alpha;
    if(sumQ)   delete [] sumQ;

    G      = nullptr;
    P      = nullptr;
    Q      = nullptr;
    Z      = nullptr;
    lambda = nullptr;
    alpha  = nullptr;
    sumQ   = nullptr;
}

void Structure::onlyResult()
{
    if(G)      delete [] G;
    if(P)      delete [] P;
    if(Q)      delete [] Q;
    if(Z)      delete [] Z;
    if(lambda) delete [] lambda;
    if(alpha)  delete [] alpha;

    G      = nullptr;
    P      = nullptr;
    Q      = nullptr;
    Z      = nullptr;
    lambda = nullptr;
    alpha  = nullptr;
}

#undef PPos
#undef QPos
#undef ZPos


} // namespace MML

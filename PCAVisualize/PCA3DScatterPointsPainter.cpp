#include "PCA3DScatterPointsPainter.hpp"

#include "FileDeal/FileDeal.hpp"

PCA3DScatterPointsPainter::PCA3DScatterPointsPainter() :
    tmpPaintWidget(nullptr),
    sampleName(nullptr),
    sampleData(nullptr),
    pointsData(nullptr),
    popIndex(nullptr),
    selectedSampleIndex(-1),
    searchedSampleIndex(-1),
    popName(nullptr)
{
    colorSearchedPoints = QColor(255, 255, 0);
    colorSelectedPoints = Qt::red;
    colorSelectedNameText = Qt::black;
    pointSize = 6;
    selectedPointsDistance2 = 20;
}

PCA3DScatterPointsPainter::~PCA3DScatterPointsPainter()
{
    if(sampleName){
        for(long long i=0;i<nSample;++i){
            delete [] sampleName[i];
        }
        delete [] sampleName;
        sampleName = nullptr;
    }
    if(sampleData){
        delete [] sampleData;
        sampleData = nullptr;
    }
    if(popIndex){
        delete [] popIndex;
        popIndex = nullptr;
    }
    if(pointsData){
        delete [] pointsData;
        pointsData = nullptr;
    }
    if(popName){
        for(long long i=0;i<nPop;++i){
            delete [] popName[i];
        }
        delete [] popName;
        popName = nullptr;
    }
    delDeleteGL();
}

bool PCA3DScatterPointsPainter::setIndividualsData(const double *inData, const long long *inPopIndex, char **inIndividualsName, long long inNIndividuals)
{
    if(inNIndividuals==0) return false;
    if(sampleName){
        for(long long i=0;i<nSample;++i){
            delete [] sampleName[i];
        }
        delete [] sampleName;
    }
    nSample = inNIndividuals;
    if(sampleData){
        delete [] sampleData;
    }
    if(popIndex){
        delete [] popIndex;
    }
    if(pointsData){
        delete [] pointsData;
    }
    sampleName = new char*[nSample];
    pointsData=new GLfloat[6*nSample];
    for(long long i=0;i<nSample;++i) sampleName[i]=new char[FD::MAXELEMLEN];
    sampleData = new double[3*nSample];
    popIndex = new long long[nSample];
    double min1=1e300,min2=1e300,min3=1e300,max1=-1e300,max2=-1e300,max3=-1e300;
    for(long long i=0;i<3*nSample;++i){
        sampleData[i]=inData[i];
        switch(i%3){
        case 0:
            min1=std::min(min1,sampleData[i]);
            max1=std::max(max1,sampleData[i]);
            break;
        case 1:
            min2=std::min(min2,sampleData[i]);
            max2=std::max(max2,sampleData[i]);
            break;
        case 2:
            min3=std::min(min3,sampleData[i]);
            max3=std::max(max3,sampleData[i]);
            break;
        }
    }
    minPC1=min1-(max1-min1)*0.1;
    maxPC1=max1+(max1-min1)*0.1;
    minPC2=min2-(max2-min2)*0.1;
    maxPC2=max2+(max2-min2)*0.1;
    minPC3=min3-(max3-min3)*0.1;
    maxPC3=max3+(max3-min3)*0.1;
    for(long long i=0;i<nSample;++i){
        popIndex[i]=inPopIndex[i];
        FD::charCopy(sampleName[i],inIndividualsName[i]);
    }
    adjustPaintData();
    adjustPaintColor();
    return true;
}

bool PCA3DScatterPointsPainter::setPopName(char **inPopName, long long inNPop)
{
    long long i;
    if(!inPopName||inNPop==0) return false;
    if(popName){
        for(i=0;i<nPop;++i) delete [] popName[i];
        delete [] popName;
    }
    nPop = inNPop;
    popName = new char*[nPop];
    for(i=0;i<nPop;++i){
        popName[i] = new char[FD::MAXELEMLEN];
        FD::charCopy(popName[i],inPopName[i]);
    }
    return true;
}

void PCA3DScatterPointsPainter::calcuAxisData(short iPC, double &outStart, double &outStride)
{
    double tmax,tmin,range;
    double tstride=1,tprobe;
    int iBit=0,flag=0;
    int i;
    long long tBit10=1;
    switch (iPC) {
    default:
    case 1:
        tmin=minPC1;
        tmax=maxPC1;
        break;
    case 2:
        tmin=minPC2;
        tmax=maxPC2;
        break;
    case 3:
        tmin=minPC3;
        tmax=maxPC3;
        break;
    }
    tprobe=tmax;
    range=tmax-tmin;
    if(range/tstride>6){
        while(range/tstride>6){
            if(flag%2==0){
                tstride*=2;
                flag=(flag+1)%2;
            }
            else{
                tstride*=5;
                flag=(flag+1)%2;
                iBit++;
            }
        }
        for(i=0;i<iBit;++i){
            tBit10*=10;
        }
        tprobe=(long long)(tprobe*tBit10)/tBit10;
    }
    else{
        while(range/tstride<4){
            if(flag%2==0){
                tstride/=5;
                flag=(flag+1)%2;
                iBit++;
            }
            else{
                tstride/=2;
                flag=(flag+1)%2;
            }
        }
        for(i=0;i<iBit;++i){
            tBit10*=10;
        }
        tprobe=(long long)(tprobe*tBit10)/tBit10;
    }
    while(tprobe-tstride>tmin) tprobe-=tstride;
    while(tprobe<tmin) tprobe+=tstride;
    outStart=tprobe;
    outStride=tstride;
}

void PCA3DScatterPointsPainter::initInitializeGL()
{
    idPointsShaderProgram = 0;
    tmpPaintWidget->setGLShaderProgram(":/shader/PCAPointsVertex.shader",
                                       ":/shader/PCAPointsFragment.shader",
                                       idPointsShaderProgram);

    tmpPaintWidget->glGenBuffers(1,&pointsVBO);
    tmpPaintWidget->glGenVertexArrays(1,&pointsVAO);
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,pointsVBO);
    tmpPaintWidget->glBindVertexArray(pointsVAO);
    tmpPaintWidget->glBufferData(GL_ARRAY_BUFFER,6*nSample*sizeof(GLfloat),nullptr,GL_DYNAMIC_DRAW);
    tmpPaintWidget->glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),(void*)0);
    tmpPaintWidget->glEnableVertexAttribArray(0);
    tmpPaintWidget->glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));
    tmpPaintWidget->glEnableVertexAttribArray(1);

    tmpPaintWidget->glBindVertexArray(0);
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,0);
}

void PCA3DScatterPointsPainter::delDeleteGL()
{
    if(tmpPaintWidget -> isValid())
    {
        tmpPaintWidget -> glDeleteBuffers(1,&pointsVBO);
        tmpPaintWidget -> glDeleteVertexArrays(1,&pointsVAO);
    }
}

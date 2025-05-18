#include "PCA2DScatterPointsPainter.hpp"

PCA2DScatterPointsPainter::PCA2DScatterPointsPainter()
{
    nSample = 0;
    sampleName = nullptr;
    sampleData = nullptr;
    popIndex = nullptr;
    popName = nullptr;
    pointsData = nullptr;
    nPop = 0;

    selectedSampleIndex = -1;  //
    searchedSampleIndex = -1; ///
    xMarginFactor = .2; // [0,1]
    yMarginFactor = .2; // [0,1]
    minPC1 = -.1; //
    minPC2 = -.1; //
    maxPC1 = -.1; //
    maxPC2 = -.1; //
    colorSearchedPoints = QColor(255, 255, 0);
    colorSelectedPoints = Qt::red;
    colorSelectedNameText = Qt::black;
    colorSelectedPopText = Qt::black;
    fontSelectedText.setFamily("Times New Roman");
    fontSelectedText.setBold(true);
    selectedPointsDistance2 = 20;
    tmpPaintWidget = nullptr;

    colorPopList.push_back("#8ECFC9");
    colorPopList.push_back("#FFBE7A");
    colorPopList.push_back("#FA7F6F");
    colorPopList.push_back("#82B0D2");
    colorPopList.push_back("#BEB8DC");
    colorPopList.push_back("#E7DAD2");
}

PCA2DScatterPointsPainter::~PCA2DScatterPointsPainter()
{
    delDeleteGL();

    if(sampleName){
        for(long long i=0;i<nSample;++i){
            delete [] sampleName[i];
        }
        delete [] sampleName;
    }
    if(sampleData){
        delete [] sampleData;
    }
    if(popIndex){
        delete [] popIndex;
    }
    if(popName){
        delete [] popName;
    }
    if(pointsData){
        delete [] pointsData;
    }
    sampleName = nullptr;
    sampleData = nullptr;
    popIndex = nullptr;
    popName = nullptr;
    pointsData = nullptr;
    tmpPaintWidget = nullptr;
}

bool PCA2DScatterPointsPainter::loadSampleData(const double *inData, const long long *inPopIndex, char **inSampleName, long long inNSample)
{
    if(inNSample == 0
       || inData == nullptr
       || inPopIndex == nullptr
       || inSampleName == nullptr){
        return false;
    }
    if(sampleName){
        for(long long i=0;i<nSample;++i){
            delete [] sampleName[i];
        }
        delete [] sampleName;
    }
    nSample = inNSample;
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
    for(long long i=0;i<nSample;++i){
        sampleName[i]=new char[FD::MAXELEMLEN];
    }
    sampleData = new double[2*nSample];
    popIndex = new long long[nSample];
    pointsData = new GLfloat[6*nSample];
    double min1=1e300,min2=1e300,max1=-1e300,max2=-1e300;
    double xExpandFactor=xMarginFactor/(2-2*xMarginFactor);
    double yExpandFactor=yMarginFactor/(2-2*yMarginFactor);
    for(long long i=0;i<2*nSample;++i){
        sampleData[i]=inData[i];
        switch(i%2){
        case 0:
            min1=std::min(min1,sampleData[i]);
            max1=std::max(max1,sampleData[i]);
            break;
        case 1:
            min2=std::min(min2,sampleData[i]);
            max2=std::max(max2,sampleData[i]);
            break;
        default:
            break;
        }
    }
    minPC1=min1-(max1-min1)*xExpandFactor*2;
    maxPC1=max1+(max1-min1)*xExpandFactor*2;
    minPC2=min2-(max2-min2)*yExpandFactor*2;
    maxPC2=max2+(max2-min2)*yExpandFactor*2;
    for(long long i=0;i<nSample;++i){
        popIndex[i]=inPopIndex[i];
        FD::charCopy(sampleName[i],inSampleName[i]);
    }
    adjustPaintData();
    adjustPaintColor();
    return true;
}

bool PCA2DScatterPointsPainter::loadPopName(char **inPopName, long long inNPop)
{
    if(inPopName == nullptr
       || inNPop <= 0){
        return false;
    }
    if(popName){
        for(long long i=0;i<nPop;++i){
            delete [] popName[i];
        }
        delete popName;
        popName = nullptr;
    }
    nPop = inNPop;
    popName = new char*[nPop];
    for(long long i=0;i<nPop;++i){
        popName[i] = new char[FD::MAXELEMLEN];
        FD::charCopy(popName[i],inPopName[i]);
    }
    return true;
}

void PCA2DScatterPointsPainter::initInitializeGL()
{
    tmpPaintWidget->glPointSize(6);
    tmpPaintWidget->glEnable(GL_POINT_SMOOTH);

    tmpPaintWidget->setGLShaderProgram(":/shader/PCA2DPointsVertex.shader",":/shader/PCA2DPointsFragment.shader");
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

void PCA2DScatterPointsPainter::calcuAxisData(short iPC, double &outStart, double &outStride)
{
    double tmax,tmin,range;
    double tstride=1,tprobe;
    int iBit=0,flag=0;
    int i;
    long long tBit10=1;
    switch (iPC) {
    case 1:
        tmin=minPC1-(maxPC1-minPC1);
        tmax=maxPC1+(maxPC1-minPC1);
        break;
    case 2:
        tmin=minPC2-(maxPC2-minPC2);
        tmax=maxPC2+(maxPC2-minPC2);
        break;
    default:
        tmin=minPC1-(maxPC1-minPC1);
        tmax=maxPC1+(maxPC1-minPC1);
        break;
    }
    tprobe=tmax;
    range=tmax-tmin;
    if(range/tstride>10){
        while(range/tstride>10){
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
        while(range/tstride<6){
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

void PCA2DScatterPointsPainter::delDeleteGL()
{
    if(tmpPaintWidget -> isValid())
    {
        tmpPaintWidget -> glDeleteBuffers(1,&pointsVBO);
        tmpPaintWidget -> glDeleteVertexArrays(1,&pointsVAO);
    }
}



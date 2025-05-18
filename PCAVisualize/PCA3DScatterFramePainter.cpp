#include "PCA3DScatterFramePainter.hpp"

#include "DataManager/DataManager.hpp"

PCA3DScatterFramePainter::PCA3DScatterFramePainter() :

    tmpPaintWidget(nullptr),

    isDisplayGridLine(true),
    isDisplayAxisValue(true),
    isDisplayAxisFrame(true),

    idAxisShaderProgram(0)
{
    axisValuesData = nullptr;
    gridLinesData = nullptr;
    axisData = nullptr;

    fontAxisValue.setFamily("Times New Roman");
    fontAxisValue.setPointSize(8);
    colorAxisValue = Qt::black;

    colorGridLines = QColor(200,200,200);
    colorAxisPC1   = QColor(Qt::red);
    colorAxisPC2   = QColor(Qt::blue);
    colorAxisPC3   = QColor(Qt::green);

    PC1VarRatio    = 0.0;
    PC2VarRatio    = 0.0;
    PC3VarRatio    = 0.0;
}

PCA3DScatterFramePainter::~PCA3DScatterFramePainter()
{
    if(axisValuesData){
        delete [] axisValuesData;
    }
    if(gridLinesData){
        delete [] gridLinesData;
    }
    if(axisData){
        delete [] axisData;
    }
    axisValuesData = nullptr;
    gridLinesData = nullptr;
    axisData = nullptr;
    delDeleteGL();
}

void PCA3DScatterFramePainter::setPCRange(double inMinPC1, double inMaxPC1, double inMinPC2, double inMaxPC2, double inMinPC3, double inMaxPC3)
{
    minPC1 = inMinPC1;
    minPC2 = inMinPC2;
    minPC3 = inMinPC3;
    maxPC1 = inMaxPC1;
    maxPC2 = inMaxPC2;
    maxPC3 = inMaxPC3;
}

void PCA3DScatterFramePainter::setAxisValuesData(double pcStart1, double pcStride1, double pcStart2, double pcStride2, double pcStart3, double pcStride3)
{
    if(axisValuesData) delete [] axisValuesData;
    double ts,tg,te,tv,range;
    int n=0,i,j;
    xAxisValuePos.clear();
    yAxisValuePos.clear();
    zAxisValuePos.clear();
    xGridValue.clear();
    yGridValue.clear();
    zGridValue.clear();
    ts=pcStart1,tg=pcStride1,te=maxPC1;
    while(ts<te){
        ++n,ts+=tg;
        if(std::abs(ts)<MML::EPS) ts=0;
    }
    ts=pcStart2,tg=pcStride2,te=maxPC2;
    while(ts<te){
        ++n,ts+=tg;
        if(std::abs(ts)<MML::EPS) ts=0;
    }
    ts=pcStart3,tg=pcStride3,te=maxPC3;
    while(ts<te){
        ++n,ts+=tg;
        if(std::abs(ts)<MML::EPS) ts=0;
    }
    nAxisValues=n;
    axisValuesData=new float[12*nAxisValues];
    i=0,j=0;
    tg=pcStride1,tv=pcStart1,te=maxPC1,range=maxPC3-minPC3;
    while(tv<te){
        axisValuesData[i*12]=tv;
        axisValuesData[i*12+1]=minPC2;
        axisValuesData[i*12+2]=maxPC3;
        axisValuesData[i*12+6]=tv;
        axisValuesData[i*12+7]=minPC2;
        axisValuesData[i*12+8]=maxPC3+0.1*range;
        xAxisValuePos.append(QVector3D(
                    tv,
                    minPC2,
                    maxPC3+0.1*range
                    ));
        xGridValue.append(tv);
        ++i,++j;
        tv+=tg;
        if(std::abs(tv)<MML::EPS) tv=0;
    }
    j=0;
    tg=pcStride2,tv=pcStart2,te=maxPC2,range=maxPC1-minPC1;
    while(tv<te){
        axisValuesData[i*12]=minPC1;
        axisValuesData[i*12+1]=tv;
        axisValuesData[i*12+2]=maxPC3;
        axisValuesData[i*12+6]=minPC1-0.1*range;
        axisValuesData[i*12+7]=tv;
        axisValuesData[i*12+8]=maxPC3;
        yAxisValuePos.append(QVector3D(
                    minPC1-0.1*range,
                    tv,
                    maxPC3
                    ));
        yGridValue.append(tv);
        ++i,++j;
        tv+=tg;
        if(std::abs(tv)<MML::EPS) tv=0;
    }
    j=0;
    tg=pcStride3,tv=pcStart3,te=maxPC3,range=maxPC1-minPC1;
    while(tv<te){
        axisValuesData[i*12]=maxPC1;
        axisValuesData[i*12+1]=minPC2;
        axisValuesData[i*12+2]=tv;
        axisValuesData[i*12+6]=maxPC1+0.1*range;
        axisValuesData[i*12+7]=minPC2;
        axisValuesData[i*12+8]=tv;
        zAxisValuePos.append(QVector3D(
                    maxPC1+0.1*range,
                    minPC2,
                    tv
                    ));
        zGridValue.append(tv);
        ++i,++j;
        tv+=tg;
        if(std::abs(tv)<MML::EPS) tv=0;
    }

    for(i=3;i<12*nAxisValues;i+=6){
        axisValuesData[i]=0.7;
        axisValuesData[i+1]=0.7;
        axisValuesData[i+2]=0.0;
    }
    qDebug() << nAxisValues;
    qDebug() << xAxisValuePos.size() + yAxisValuePos.size() + zAxisValuePos.size();
}

void PCA3DScatterFramePainter::initInitializeGL()
{
    if(tmpPaintWidget == nullptr){
        return;
    }
    tmpPaintWidget->glEnable(GL_POINT_SMOOTH);

    idAxisShaderProgram = tmpPaintWidget->addShaderProgram();
    tmpPaintWidget->setGLShaderProgram(":/shader/PCAAxisVertex.shader",
                                       ":/shader/PCAAxisFragment.shader",
                                       idAxisShaderProgram);

    tmpPaintWidget->glGenBuffers(1,&axisVBO);
    tmpPaintWidget->glGenVertexArrays(1,&axisVAO);
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,axisVBO);
    tmpPaintWidget->glBindVertexArray(axisVAO);
    tmpPaintWidget->glBufferData(GL_ARRAY_BUFFER,108*sizeof(GLfloat),nullptr,GL_DYNAMIC_DRAW);
    tmpPaintWidget->glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),(void*)0);
    tmpPaintWidget->glEnableVertexAttribArray(0);
    tmpPaintWidget->glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));
    tmpPaintWidget->glEnableVertexAttribArray(1);

    tmpPaintWidget->glGenBuffers(1,&axisValueVBO);
    tmpPaintWidget->glGenVertexArrays(1,&axisValueVAO);
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,axisValueVBO);
    tmpPaintWidget->glBindVertexArray(axisValueVAO);
    tmpPaintWidget->glBufferData(GL_ARRAY_BUFFER,12*nAxisValues*sizeof(GLfloat),nullptr,GL_DYNAMIC_DRAW);
    tmpPaintWidget->glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),(void*)0);
    tmpPaintWidget->glEnableVertexAttribArray(0);
    tmpPaintWidget->glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));
    tmpPaintWidget->glEnableVertexAttribArray(1);

    tmpPaintWidget->glGenBuffers(1,&gridLineVBO);
    tmpPaintWidget->glGenVertexArrays(1,&gridLineVAO);
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,gridLineVBO);
    tmpPaintWidget->glBindVertexArray(gridLineVAO);
    tmpPaintWidget->glBufferData(GL_ARRAY_BUFFER,12*nGridLines*sizeof(GLfloat),nullptr,GL_DYNAMIC_DRAW);
    tmpPaintWidget->glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),(void*)0);
    tmpPaintWidget->glEnableVertexAttribArray(0);
    tmpPaintWidget->glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));
    tmpPaintWidget->glEnableVertexAttribArray(1);

    tmpPaintWidget->glBindVertexArray(0);
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,0);
}

void PCA3DScatterFramePainter::delDeleteGL()
{
    if(tmpPaintWidget -> isValid())
    {
        tmpPaintWidget -> glDeleteBuffers(1,&axisVBO);
        tmpPaintWidget -> glDeleteVertexArrays(1,&axisVAO);
        tmpPaintWidget -> glDeleteBuffers(1,&axisValueVBO);
        tmpPaintWidget -> glDeleteVertexArrays(1,&axisValueVAO);
        tmpPaintWidget -> glDeleteBuffers(1,&gridLineVBO);
        tmpPaintWidget -> glDeleteVertexArrays(1,&gridLineVAO);
    }
}

void PCA3DScatterFramePainter::setAxisData()
{
    double minv[3] {minPC1,minPC2,minPC3};
    double maxv[3] {maxPC1,maxPC2,maxPC3};
    double color[3];
    size_t i=0,j=0,k=0;
    if(axisData){
        delete [] axisData;
    }
    axisData = new GLfloat[108];
    for(i=0;i<36;i+=6){
        for(j=0;j<3;++j){
            axisData[i+j]=minv[j];
        }
    }
    for((i=6),(j=0);i<36;(i+=12),(j++)){
        axisData[i+j]=maxv[j];
    }
    for(i=3;i<36;i+=12){
        switch(i){
        case 3:
            color[0]=colorAxisPC1.redF();
            color[1]=colorAxisPC1.greenF();
            color[2]=colorAxisPC1.blueF();
            break;
        case 15:
            color[0]=colorAxisPC2.redF();
            color[1]=colorAxisPC2.greenF();
            color[2]=colorAxisPC2.blueF();
            break;
        case 27:
            color[0]=colorAxisPC3.redF();
            color[1]=colorAxisPC3.greenF();
            color[2]=colorAxisPC3.blueF();
            break;
        default:
            break;
        }
        for(j=0;j<12;j+=6){
            for(k=0;k<3;++k){
                axisData[i+j+k]=color[k];
            }
        }
    }
    color[0]=0.0;
    color[1]=0.0;
    color[2]=0.0;
    for(i=36;i<108;i+=6){
        for(j=0;j<3;++j){
            axisData[i+j]=maxv[j];
        }
    }
    for(i=0;i<3;++i){
        for(j=0;j<2;++j){
            axisData[36+i*24+j*12+i]=minv[i];
            axisData[36+i*24+j*12+i+6]=minv[i];
        }
        switch (i) {
        case 0:
            axisData[36+i*24+0*12+1+6]=minv[1];
            axisData[36+i*24+1*12+2+6]=minv[2];
            break;
        case 1:
            axisData[36+i*24+0*12+0+6]=minv[0];
            axisData[36+i*24+1*12+2+6]=minv[2];
            break;
        case 2:
            axisData[36+i*24+0*12+1+6]=minv[1];
            axisData[36+i*24+1*12+0+6]=minv[0];
            break;
        default:
            break;
        }
    }
    for(i=39;i<108;i+=6){
        for(j=0;j<3;++j){
            axisData[i+j]=color[j];
        }
    }
}

void PCA3DScatterFramePainter::setGridLinesData()
{
    int nAxisValuesPC1 = xGridValue.size();
    int nAxisValuesPC2 = yGridValue.size();
    int nAxisValuesPC3 = zGridValue.size();
    nGridLines=2*(
                nAxisValuesPC1
                + nAxisValuesPC2
                + nAxisValuesPC3
                );
    if(gridLinesData){
        delete [] gridLinesData;
    }
    gridLinesData=new GLfloat[nGridLines*12];

    int i=0,n=0;
    for(i=0;i<nAxisValuesPC1;++i,++n){
        gridLinesData[12*n]  =xGridValue[i];
        gridLinesData[12*n+1]=minPC2;
        gridLinesData[12*n+2]=maxPC3;
        gridLinesData[12*n+6]=xGridValue[i];
        gridLinesData[12*n+7]=minPC2;
        gridLinesData[12*n+8]=minPC3;
    }
    for(i=0;i<nAxisValuesPC1;++i,++n){
        gridLinesData[12*n]  =xGridValue[i];
        gridLinesData[12*n+1]=minPC2;
        gridLinesData[12*n+2]=minPC3;
        gridLinesData[12*n+6]=xGridValue[i];
        gridLinesData[12*n+7]=maxPC2;
        gridLinesData[12*n+8]=minPC3;
    }
    for(i=0;i<nAxisValuesPC2;++i,++n){
        gridLinesData[12*n]  =minPC1;
        gridLinesData[12*n+1]=yGridValue[i];
        gridLinesData[12*n+2]=maxPC3;
        gridLinesData[12*n+6]=minPC1;
        gridLinesData[12*n+7]=yGridValue[i];
        gridLinesData[12*n+8]=minPC3;
    }
    for(i=0;i<nAxisValuesPC2;++i,++n){
        gridLinesData[12*n]  =minPC1;
        gridLinesData[12*n+1]=yGridValue[i];
        gridLinesData[12*n+2]=minPC3;
        gridLinesData[12*n+6]=maxPC1;
        gridLinesData[12*n+7]=yGridValue[i];
        gridLinesData[12*n+8]=minPC3;
    }
    for(i=0;i<nAxisValuesPC3;++i,++n){
        gridLinesData[12*n]  =maxPC1;
        gridLinesData[12*n+1]=minPC2;
        gridLinesData[12*n+2]=zGridValue[i];
        gridLinesData[12*n+6]=minPC1;
        gridLinesData[12*n+7]=minPC2;
        gridLinesData[12*n+8]=zGridValue[i];
    }
    for(i=0;i<nAxisValuesPC3;++i,++n){
        gridLinesData[12*n]  =minPC1;
        gridLinesData[12*n+1]=minPC2;
        gridLinesData[12*n+2]=zGridValue[i];
        gridLinesData[12*n+6]=minPC1;
        gridLinesData[12*n+7]=maxPC2;
        gridLinesData[12*n+8]=zGridValue[i];
    }

    for(i=3;i<nGridLines*12;i+=6){
        gridLinesData[i]=colorGridLines.redF();
        gridLinesData[i+1]=colorGridLines.greenF();
        gridLinesData[i+2]=colorGridLines.blueF();
    }
}

void PCA3DScatterFramePainter::sendAxisValuesDataToBuffer()
{
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,axisValueVBO);
    tmpPaintWidget->glBindVertexArray(axisValueVAO);
    tmpPaintWidget->glBufferSubData(GL_ARRAY_BUFFER,0,12*nAxisValues*sizeof(GLfloat),axisValuesData);
    tmpPaintWidget->glBindVertexArray(0);
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,0);
}

void PCA3DScatterFramePainter::sendAxisDataToBuffer()
{
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,axisVBO);
    tmpPaintWidget->glBindVertexArray(axisVAO);
    tmpPaintWidget->glBufferSubData(GL_ARRAY_BUFFER,0,108*sizeof(GLfloat),axisData);
    tmpPaintWidget->glBindVertexArray(0);
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,0);
}

void PCA3DScatterFramePainter::sendGridLinesDataToBuffer()
{
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,gridLineVBO);
    tmpPaintWidget->glBindVertexArray(gridLineVAO);
    tmpPaintWidget->glBufferSubData(GL_ARRAY_BUFFER,0,12*nGridLines*sizeof(GLfloat),gridLinesData);
    tmpPaintWidget->glBindVertexArray(0);
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,0);
}

void PCA3DScatterFramePainter::savePaintAxisValues(QPainter *painter)
{
    long long i;
    QVector4D pos1, pos2;
    QVector4D glPos1, glPos2;
    double xp1, yp1, xp2, yp2;
    QColor color;
    int W = paintSize.width();
    int H = paintSize.height();
    painter -> save();
    for(i = 0; i < nAxisValues; ++i){
        pos1.setX(axisValuesData[i*12]);
        pos1.setY(axisValuesData[i*12+1]);
        pos1.setZ(axisValuesData[i*12+2]);
        pos1.setW(1.0);
        pos2.setX(axisValuesData[i*12+6]);
        pos2.setY(axisValuesData[i*12+7]);
        pos2.setZ(axisValuesData[i*12+8]);
        pos2.setW(1.0);
        color.setRedF(axisValuesData[i*12+3]);
        color.setGreenF(axisValuesData[i*12+4]);
        color.setBlueF(axisValuesData[i*12+5]);

        glPos1 = matProjection*matView*matModel*pos1;
        glPos2 = matProjection*matView*matModel*pos2;


        xp1 = (glPos1.x()/glPos1.w()+1)*W/2;
        yp1 = (1-glPos1.y()/glPos1.w())*H/2;
        xp2 = (glPos2.x()/glPos2.w()+1)*W/2;
        yp2 = (1-glPos2.y()/glPos2.w())*H/2;

        painter -> setPen(QPen(color,1));
        painter -> drawLine(xp1, yp1, xp2, yp2);
    }
    painter -> restore();
}

void PCA3DScatterFramePainter::savePaintAxis(QPainter *painter)
{
    long long i;
    QVector4D pos1, pos2;
    QVector4D glPos1, glPos2;
    double xp1, yp1, xp2, yp2;
    QColor color;
    int W = paintSize.width();
    int H = paintSize.height();
    painter -> save();
    for(i = 0; i < 9; ++i){
        pos1.setX(axisData[i*12]);
        pos1.setY(axisData[i*12+1]);
        pos1.setZ(axisData[i*12+2]);
        pos1.setW(1.0);
        pos2.setX(axisData[i*12+6]);
        pos2.setY(axisData[i*12+7]);
        pos2.setZ(axisData[i*12+8]);
        pos2.setW(1.0);
        color.setRedF(axisData[i*12+3]);
        color.setGreenF(axisData[i*12+4]);
        color.setBlueF(axisData[i*12+5]);

        glPos1 = matProjection*matView*matModel*pos1;
        glPos2 = matProjection*matView*matModel*pos2;


        xp1 = (glPos1.x()/glPos1.w()+1)*W/2;
        yp1 = (1-glPos1.y()/glPos1.w())*H/2;
        xp2 = (glPos2.x()/glPos2.w()+1)*W/2;
        yp2 = (1-glPos2.y()/glPos2.w())*H/2;

        painter -> setPen(QPen(color,1));
        painter -> drawLine(xp1, yp1, xp2, yp2);
    }
    painter -> restore();
}

void PCA3DScatterFramePainter::savePaintGridLines(QPainter *painter)
{
    long long i;
    QVector4D pos1, pos2;
    QVector4D glPos1, glPos2;
    double xp1, yp1, xp2, yp2;
    QColor color = QColor(Qt::black);
    int W = paintSize.width();
    int H = paintSize.height();
    painter -> save();
    for(i = 0; i < nGridLines; ++i){
        pos1.setX(gridLinesData[i*12]);
        pos1.setY(gridLinesData[i*12+1]);
        pos1.setZ(gridLinesData[i*12+2]);
        pos1.setW(1.0);
        pos2.setX(gridLinesData[i*12+6]);
        pos2.setY(gridLinesData[i*12+7]);
        pos2.setZ(gridLinesData[i*12+8]);
        pos2.setW(1.0);
        // color.setRedF(gridLinesData[i*12+3]);
        // color.setGreenF(gridLinesData[i*12+4]);
        // color.setBlueF(gridLinesData[i*12+5]);

        glPos1 = matProjection*matView*matModel*pos1;
        glPos2 = matProjection*matView*matModel*pos2;


        xp1 = (glPos1.x()/glPos1.w()+1)*W/2;
        yp1 = (1-glPos1.y()/glPos1.w())*H/2;
        xp2 = (glPos2.x()/glPos2.w()+1)*W/2;
        yp2 = (1-glPos2.y()/glPos2.w())*H/2;

        painter -> setPen(QPen(color,1));
        painter -> drawLine(xp1, yp1, xp2, yp2);
    }
    painter -> restore();
}

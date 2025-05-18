#include "GWASQQPlotPointsPainter.h"

#include "GenomeAnnotation/AxisValueCalculator.h"

GWASQQPlotPointsPainter::GWASQQPlotPointsPainter() {
    dataPoints = nullptr;
    nPoints = 0;
    tmpPaintWidget = nullptr;
    pointsData = nullptr;

    colorPoints = Qt::black;
    colorSelectedPoints = Qt::red;
    leftBorderFactor = 0.2;
    rightBorderFactor = 0.2;
    topBorderFactor = 0.2;
    buttomBorderFactor = 0.2;
    selectedPointsDistance2 = 20;
    selectedPointIndex = -1;
    maxXAxisData = .0;
    maxYAxisData = .0;
    fontPointsInfo.setFamily("Times New Roman");
    fontPointsInfo.setBold(true);
}

GWASQQPlotPointsPainter::~GWASQQPlotPointsPainter()
{
    delDeleteGL();

    if(dataPoints != nullptr){
        delete [] dataPoints;
        dataPoints = nullptr;
    }
    if(pointsData != nullptr){
        delete [] pointsData;
        pointsData = nullptr;
    }
    nPoints = 0;
    tmpPaintWidget = nullptr;
}

bool GWASQQPlotPointsPainter::loadPoints(ValPoints *inPoints, long long inNPoints)
{
    long long iPoint;
    if(inPoints == nullptr
        || inNPoints <= 0){
        return false;
    }
    nPoints = inNPoints;
    if(dataPoints != nullptr){
        delete [] dataPoints;
        dataPoints = nullptr;
    }
    if(pointsData != nullptr){
        delete [] pointsData;
        pointsData = nullptr;
    }
    maxVal = MML::DOUBLE_ZERO;
    minVal = MML::DOUBLE_MAX;
    dataPoints = new ValPoints2[nPoints+1];
    pointsData = new GLfloat[2*nPoints];
    double* expectedVal = new double [nPoints+1];
    stats.genNegLog10PPoints(nPoints, expectedVal);
    for(iPoint = 0;iPoint < nPoints;++ iPoint){
        dataPoints[iPoint].idchr = inPoints[iPoint].idchr;
        dataPoints[iPoint].pos = inPoints[iPoint].pos;
        dataPoints[iPoint].val1 = inPoints[iPoint].val;
        maxVal = std::max(maxVal, dataPoints[iPoint].val1);
        minVal = std::min(minVal, dataPoints[iPoint].val1);
    }
    std::sort(&(dataPoints[0]),&(dataPoints[nPoints]),ValPoints2CompLess);
    for(iPoint = 0;iPoint < nPoints;++ iPoint){
        dataPoints[iPoint].val2 = expectedVal[nPoints - iPoint - 1];
        maxVal = std::max(maxVal, dataPoints[iPoint].val2);
        minVal = std::min(minVal, dataPoints[iPoint].val2);
    }
    delete [] expectedVal;
    calcuAxisData();
    return true;
}

bool GWASQQPlotPointsPainter::setAxisValue(double xMax, double yMax, bool isReset)
{
    if(isReset) {
        calcuAxisData();
        return true;
    }
    xMax = maxVal > xMax ? maxVal : xMax;
    yMax = maxVal > yMax ? maxVal : yMax;
    AxisValueCalculator::calcuAxisData0XStatic(xMax, xAxisDataVec);
    AxisValueCalculator::calcuAxisData0XStatic(yMax, yAxisDataVec);
    maxXAxisData = xMax;
    maxYAxisData = yMax;
    return true;
}

void GWASQQPlotPointsPainter::initInitializeGL()
{
    tmpPaintWidget->glPointSize(6);
    tmpPaintWidget->glEnable(GL_POINT_SMOOTH);

    idPointsShader = 0;
    tmpPaintWidget->setGLShaderProgram(":/shader/QQPointsVertex.shader",":/shader/QQPointsFragment.shader",idPointsShader);

    tmpPaintWidget->glGenBuffers(1,&pointsVBO);
    tmpPaintWidget->glGenVertexArrays(1,&pointsVAO);
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,pointsVBO);
    tmpPaintWidget->glBindVertexArray(pointsVAO);
    tmpPaintWidget->glBufferData(GL_ARRAY_BUFFER,2*nPoints*sizeof(GLfloat),nullptr,GL_DYNAMIC_DRAW);
    tmpPaintWidget->glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),(void*)0);
    tmpPaintWidget->glEnableVertexAttribArray(0);

    tmpPaintWidget->glBindVertexArray(0);
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,0);
}

void GWASQQPlotPointsPainter::delDeleteGL()
{
    if(tmpPaintWidget -> isValid()){
        tmpPaintWidget -> glDeleteBuffers(1,&pointsVBO);
        tmpPaintWidget -> glDeleteVertexArrays(1,&pointsVAO);
    }
}

void GWASQQPlotPointsPainter::calcuAxisData()
{
    AxisValueCalculator::calcuAxisData0XStatic(maxVal, xAxisDataVec);
    yAxisDataVec = xAxisDataVec;

    maxXAxisData = maxVal;
    maxYAxisData = maxVal;
}

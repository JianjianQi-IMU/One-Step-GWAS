#ifndef PCA2DSCATTERPOINTSPAINTER_HPP
#define PCA2DSCATTERPOINTSPAINTER_HPP

#include <QSize>
#include <QPainter>
#include <QPainterPath>
#include "FileDeal/FileDeal.hpp"
#include "GenomeAnnotation/BaseOpenGLPaintWidget.hpp"
#include "GenomeAnnotation/WidgetTextPainter.hpp"

class PCA2DScatterPointsPainter
{
private:
    QMatrix4x4               matModel;
    QMatrix4x4               matView;
    QSize                    widgetSize;
    GLuint                   pointsVAO;
    GLuint                   pointsVBO;
    char**                   sampleName; ////
    double*                  sampleData; ////
    long long                nSample; ////
    GLfloat*                 pointsData; ////
    long long*               popIndex; ////
    char**                   popName; ////
    long long                nPop; ////
    float                    pointSize;
    long long                selectedSampleIndex;  ////
    long long                searchedSampleIndex; ////
    float                    xMarginFactor; // [0,1]
    float                    yMarginFactor; // [0,1]
    double                   minPC1; //
    double                   minPC2; //
    double                   maxPC1; //
    double                   maxPC2; //
    QColor                   colorSearchedPoints;
    QColor                   colorSelectedPoints;
    QColor                   colorSelectedNameText;
    QColor                   colorSelectedPopText;
    QFont                    fontSelectedText;
    float                    selectedPointsDistance2;
    QVector<QColor>          colorPopList;
    BaseOpenGLPaintWidget*   tmpPaintWidget;
    WidgetTextPainter        selectedTextPainter;

public:
    PCA2DScatterPointsPainter();
    ~PCA2DScatterPointsPainter();
    bool loadSampleData(const double* inData,
                       const long long* inPopIndex,
                       char** inSampleName,
                       long long inNSample);
    bool loadPopName(char** inPopName, long long inNPop);
    void preSetPaintWidget(BaseOpenGLPaintWidget* inPaintWidget)
    {
        tmpPaintWidget = inPaintWidget;
    }
    void setWidgetSize(const QSize& size)
    {
        widgetSize = size;
    }
    void setXMarginFactor(double val)
    {
        xMarginFactor = val;
    }
    void setYMarginFactor(double val)
    {
        yMarginFactor = val;
    }
    void setPCRange1(double inMinPC,double inMaxPC)
    {
        minPC1 = inMinPC;
        maxPC1 = inMaxPC;
    }
    void setPCRange2(double inMinPC,double inMaxPC)
    {
        minPC2 = inMinPC;
        maxPC2 = inMaxPC;
    }
    void setModelMat(const QMatrix4x4& inMat)
    {
        matModel = inMat;
    }
    void setViewMat(const QMatrix4x4& inMat)
    {
        matView = inMat;
    }
    void setSelectedSampleIndex(long long idPaint)
    {
        selectedSampleIndex = idPaint;
    }
    void setSearchedSampleIndex(long long idPaint)
    {
        searchedSampleIndex = idPaint;
    }
    void setSearchedPointsColor(const QColor& col)
    {
        colorSearchedPoints = col;
    }
    void setSelectedPointsColor(const QColor& col)
    {
        colorSelectedPoints = col;
    }
    void setColorPopList(const QVector<QColor>& inColVec,bool isAdjustPaintColor = true)
    {
        colorPopList = inColVec;
        if(isAdjustPaintColor){
            adjustPaintColor();
        }
    }
    void setPointSize(float val) {
        pointSize = val;
    }

    const QVector<QColor>& getColorPopList() const
    {
        return colorPopList;
    }
    float getPointSize() {
        return pointSize;
    }
    double getMinPC1() const {return minPC1;}
    double getMinPC2() const {return minPC2;}
    double getMaxPC1() const {return maxPC1;}
    double getMaxPC2() const {return maxPC2;}

    void initInitializeGL();
    void calcuAxisData(short iPC, double &outStart, double &outStride);
    inline bool isClicked(const QPoint& pos)
    {
        int xpos = pos.x(), ypos = pos.y();
        int width0=widgetSize.width(),height0=widgetSize.height();
        int leftLim=xMarginFactor*width0/2;
        int rightLim=(2-xMarginFactor)*width0/2;
        int upLim=yMarginFactor*height0/2;
        int downLim=(2-yMarginFactor)*height0/2;

        if(xpos >= leftLim && xpos <= rightLim &&
           ypos >= upLim && ypos <= downLim)
        {
            return true;
        }
        return false;
    }
    // false : -1 | true : clicked point id
    inline long long isClickedPoint(const QPoint& pos){
        long long i;
        for(i = 0;i < nSample;++i){
            if(isSelectedPoint(i,pos.x(),pos.y())) break;
        }
        if(i < nSample) return i;
        return -1;
    }
    void paintStart(QPainter* painter){
        painter -> beginNativePainting();
        sendPaintPointsToBuffer();
        tmpPaintWidget->shaderProgramBind();
        tmpPaintWidget->setUniformValue("model",matModel);
        tmpPaintWidget->setUniformValue("view",matView);
        tmpPaintWidget->setUniformValue("xMargin",xMarginFactor);
        tmpPaintWidget->setUniformValue("yMargin",yMarginFactor);
        if(nSample != 0){
            tmpPaintWidget->glPointSize(pointSize);
            tmpPaintWidget->glBindVertexArray(pointsVAO);
            tmpPaintWidget->glDrawArrays(GL_POINTS,0,nSample);
            tmpPaintWidget->glPointSize(1);
        }
        tmpPaintWidget->glBindVertexArray(0);
        tmpPaintWidget->shaderProgramRelease();

        painter -> endNativePainting();

        paintSearchedPoint(painter);
        paintSelectedPoint(painter);
        paintSelectedPointText(painter);
    }
    void savePaintPoints(QPainter* painter){
        long long i;
        QPointF pxy;
        int width0=widgetSize.width(),height0=widgetSize.height();
        int leftLim=xMarginFactor*width0/2;
        int rightLim=(2-xMarginFactor)*width0/2;
        int upLim=yMarginFactor*height0/2;
        int downLim=(2-yMarginFactor)*height0/2;
        int xpos, ypos;
        QColor tColor;
        painter -> save();
        for(i = 0;i < nSample;++i){
            pxy = calcuPointPaint(i);
            xpos = pxy.x(), ypos = pxy.y();
            tColor = colorPopList[popIndex[i]];
            if(xpos >= leftLim && xpos <= rightLim &&
                ypos >= upLim && ypos <= downLim)
            {
                painter -> setPen(QPen(tColor,1.5*pointSize,Qt::SolidLine,Qt::RoundCap));
                painter -> drawPoint(pxy);
            }
        }
        painter -> restore();
    }
private:
    void delDeleteGL();
    QPointF calcuPointPaint(long long idPoint)
    {
        QVector4D outPos,inPos;
        int H = widgetSize.height(), W = widgetSize.width();
        inPos = QVector4D(sampleData[2*idPoint],
                          sampleData[2*idPoint+1],
                          0.0,
                          1);
        outPos=matView * matModel * inPos;
        return QPointF((outPos.x()+1)*W/2, (1-outPos.y())*H/2);
    }
    bool isSelectedPoint(long long idPoint, int x, int y)
    {
        double dx,dy;
        QPointF pxy = calcuPointPaint(idPoint);
        dx=pxy.x()-x,dy=pxy.y()-y;
        if(dx*dx + dy*dy <= selectedPointsDistance2){
            return true;
        }
        return false;
    }
    void adjustPaintData()
    {
        if(nSample == 0){
            return;
        }
        for(long long i=0;i<nSample;++i){
            pointsData[6*i]=sampleData[2*i];
            pointsData[6*i+1]=sampleData[2*i+1];
            pointsData[6*i+2]=0.0;
          }
    }
    void adjustPaintColor()
    {
        if(nSample == 0){
            return;
        }
        long long i,nPopColor=colorPopList.size();
        QColor tColor;
        for(i=0;i<nSample;++i){
            if(popIndex[i]>=nPopColor) tColor=colorPopList[nPopColor-1];
            else tColor=colorPopList[popIndex[i]];
            pointsData[6*i+3]=tColor.redF();
            pointsData[6*i+4]=tColor.greenF();
            pointsData[6*i+5]=tColor.blueF();
        }
    }
    void sendPaintPointsToBuffer()
    {
        tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,pointsVBO);
        tmpPaintWidget->glBindVertexArray(pointsVAO);
        tmpPaintWidget->glBufferSubData(GL_ARRAY_BUFFER,0,6*nSample*sizeof(GLfloat),pointsData);
        tmpPaintWidget->glBindVertexArray(0);
        tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,0);
    }
    void paintSearchedPoint(QPainter *painter)
    {
        if(searchedSampleIndex < 0 || searchedSampleIndex >= nSample) return;

        painter->save();

        int H = widgetSize.height(), W = widgetSize.width();
        QPainterPath labelShape;
        QPointF pxy = calcuPointPaint(searchedSampleIndex);
        if(pxy.x()>=0&&pxy.x()<=W&&pxy.y()>=0&&pxy.y()<=H){
            painter->setPen(QPen(QBrush(colorSearchedPoints),3));
            labelShape.addEllipse(QPointF(pxy.x(),pxy.y()),6,6);
            painter->drawPath(labelShape);
        }

        painter->restore();
    }
    void paintSelectedPoint(QPainter *painter)
    {
        if(selectedSampleIndex < 0 || selectedSampleIndex >= nSample) return;

        painter->save();

        int H = widgetSize.height(), W = widgetSize.width();
        QPainterPath labelShape;
        QPointF pxy = calcuPointPaint(selectedSampleIndex);
        if(pxy.x()>=0&&pxy.x()<=W&&pxy.y()>=0&&pxy.y()<=H){
            painter->setPen(QPen(QBrush(colorSelectedPoints),3));
            labelShape.addEllipse(QPointF(pxy.x(),pxy.y()),6,6);
            painter->drawPath(labelShape);
        }

        painter->restore();
    }
    void paintSelectedPointText(QPainter* painter)
    {
        if(selectedSampleIndex < 0 || selectedSampleIndex >= nSample) return;
        QPointF pxy = calcuPointPaint(selectedSampleIndex);
        selectedTextPainter.clear();
        selectedTextPainter.setPaintPoint(QPoint(pxy.x() + 3, pxy.y() + 3));
        selectedTextPainter.addText(sampleName[selectedSampleIndex],
                                    colorSelectedNameText,
                                    fontSelectedText);
        if(popName != nullptr){
            selectedTextPainter.addText(popName[popIndex[selectedSampleIndex]],
                                        colorPopList[popIndex[selectedSampleIndex]],
                                        fontSelectedText);
        }
        selectedTextPainter.addText("PC1: " + QString::number(sampleData[2*selectedSampleIndex]),
                                    colorSelectedPopText,
                                    fontSelectedText);
        selectedTextPainter.addText("PC2: " + QString::number(sampleData[2*selectedSampleIndex+1]),
                                    colorSelectedPopText,
                                    fontSelectedText);
        selectedTextPainter.paintText(painter);
    }
};

#endif // PCA2DSCATTERPOINTSPAINTER_HPP

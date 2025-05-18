#ifndef PCA2DSCATTERFRAMEPAINTER_HPP
#define PCA2DSCATTERFRAMEPAINTER_HPP

#include <QRect>
#include <QPainter>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QLine>
#include <QVector>
#include <QTextBlock>
#include "DataManager/DataManager.hpp"
#include "StyleChoose/LineStyleParam.hpp"

enum PCA2DScatterFrameForm
{
    PCA2D_FRAME_NONE = 0,
    PCA2D_FRAME_BOX = 1,
    PCA2D_FRAME_OPEN = 2,
};

class PCA2DScatterFramePainter
{
private:
    QMatrix4x4                matTransform;
    QSize                     widgetSize;
    bool                      isDisplayGrid; //
    bool                      isDisplayAxis; //
    bool                      isDisplayFrame;
    bool                      isDisplayPCVar;
    float                     xMarginFactor; // [0,1]
    float                     yMarginFactor; // [0,1]
    float                     axisLengthFactor; // [0,1]
    float                     PCLabelPosFactor; // [0,1]
    LineStyleParam            lineStyleParamGrid;
    QColor                    colorAxis; //
    QColor                    colorFrame; //
    QVector<double>           xGridValue;
    QVector<double>           yGridValue;
    QFont                     fontGridValue;
    QFont                     fontPCVar;

    PCA2DScatterFrameForm     frameForm; //
    double                    PC1VarRatio; //
    double                    PC2VarRatio; //

public:
    explicit PCA2DScatterFramePainter()
    {
        isDisplayGrid      = true;
        isDisplayAxis      = true;
        isDisplayFrame     = true;
        isDisplayPCVar     = true;
        xMarginFactor      = .2;
        yMarginFactor      = .2;
        axisLengthFactor   = .2;
        PCLabelPosFactor   = .4;
        lineStyleParamGrid = LineStyleParam("#dddddd", 1, Qt::DashLine);
        colorAxis          = Qt::black;
        colorFrame         = Qt::black;
        frameForm          = PCA2D_FRAME_BOX;

        PC1VarRatio        = 0;
        PC2VarRatio        = 0;

        fontGridValue.setFamily("Times New Roman");
        fontPCVar.setFamily("Times New Roman");
        fontPCVar.setBold(true);
    }

    void setWidgetSize(const QSize& size)
    {
        widgetSize = size;
    }
    void setColorGrid(const QColor& color)
    {
        lineStyleParamGrid.lineColor = color;
    }
    void setSizeGrid(int size)
    {
        lineStyleParamGrid.lineSize = size;
    }
    void setStyleGrid(Qt::PenStyle penStyle)
    {
        lineStyleParamGrid.lineStyle = penStyle;
    }
    void setStyleParamGrid(const LineStyleParam& styleParam)
    {
        lineStyleParamGrid = styleParam;
    }
    void setColorAxis(const QColor& color)
    {
        colorAxis = color;
    }
    void setColorFrame(const QColor& color)
    {
        colorFrame = color;
    }
    void setIsDisplayGrid(bool flag)
    {
        isDisplayGrid = flag;
    }
    void setIsDisplayAxis(bool flag)
    {
        isDisplayAxis = flag;
    }
    void setIsDisplayFrame(bool flag)
    {
        isDisplayFrame = flag;
    }
    void setIsDisplayPCVar(bool flag)
    {
        isDisplayPCVar = flag;
    }
    void setXMarginFactor(double val)
    {
        xMarginFactor = val;
    }
    void setYMarginFactor(double val)
    {
        yMarginFactor = val;
    }
    void setAxisLengthFactor(double val)
    {
        axisLengthFactor = val;
    }
    void setPCLabelPosFactor(double val)
    {
        PCLabelPosFactor = val;
    }
    void setTransformMat(const QMatrix4x4& inMat)
    {
        matTransform = inMat;
    }
    void setPCVarRatio(double inPC1,double inPC2)
    {
        PC1VarRatio = inPC1;
        PC2VarRatio = inPC2;
    }
    void setPC1Axis(double inPC1Start,double inPC1Stride,double inMinPC1,double inMaxPC1)
    {
        xGridValue.clear();
        double t,tstride,tmax;
        size_t i;
        i=0,tstride=inPC1Stride,t=inPC1Start,tmax=inMaxPC1+0.6*(inMaxPC1-inMinPC1);
        while(t<tmax){
            if(std::abs(t)<MML::EPS){
                xGridValue.append(0);
            }
            else{
                xGridValue.append(t);
            }
            ++i,t+=tstride;
        }
    }
    void setPC2Axis(double inPC2Start,double inPC2Stride,double inMinPC2,double inMaxPC2)
    {
        yGridValue.clear();
        double t,tstride,tmax;
        size_t i;
        i=0,tstride=inPC2Stride,t=inPC2Start,tmax=inMaxPC2+0.6*(inMaxPC2-inMinPC2);
        while(t<tmax){
            if(std::abs(t)<MML::EPS){
                yGridValue.append(0);
            }
            else{
                yGridValue.append(t);
            }
            ++i,t+=tstride;
        }
    }
    void setFrameForm(PCA2DScatterFrameForm form)
    {
        frameForm = form;
    }

    void paintFrame(QPainter* painter)
    {
        int i;
        QVector<QLine> gridLineVec;
        QVector<QLine> axisLineVec;
        QVector<QLine> frameLineVec;
        QVector<QRect> textRectVecX, textRectVecY;
        QVector<QString> textStrVecX, textStrVecY;

        calcuFrameLine(frameLineVec);
        calcuXAxisLine(gridLineVec, axisLineVec, textRectVecX, textStrVecX);
        calcuYAxisLine(gridLineVec, axisLineVec, textRectVecY, textStrVecY);

        painter->save();

        if(isDisplayFrame){
            painter->setPen(colorAxis);
            painter->drawLines(frameLineVec);
        }
        if(isDisplayGrid){
            painter->setPen(QPen(lineStyleParamGrid.lineColor,
                                 lineStyleParamGrid.lineSize,
                                 lineStyleParamGrid.lineStyle));
            painter->drawLines(gridLineVec);
        }
        if(isDisplayAxis){
            painter->setPen(QPen(colorAxis,1,Qt::SolidLine));
            painter->drawLines(axisLineVec);
            painter->setFont(fontGridValue);
            for(i=0;i<textRectVecX.size();++i){
                painter->drawText(textRectVecX[i],
                                  Qt::AlignHCenter|Qt::AlignTop,
                                  textStrVecX[i]);
            }
            for(i=0;i<textRectVecY.size();++i){
                painter->drawText(textRectVecY[i],
                                  Qt::AlignVCenter|Qt::AlignRight,
                                  textStrVecY[i]);
            }
        }
        if(isDisplayPCVar){
            painter->setFont(fontPCVar);
            paintPCVar(painter);
        }

        painter->restore();
    }
private:
    void calcuFrameLine(QVector<QLine>& frameLineVec)
    {
        int width0=widgetSize.width(),height0=widgetSize.height();
        int leftLim=xMarginFactor*width0/2;
        int rightLim=(2-xMarginFactor)*width0/2;
        int upLim=yMarginFactor*height0/2;
        int downLim=(2-yMarginFactor)*height0/2;

        if(frameForm == PCA2D_FRAME_NONE) {
            return;
        }
        if(frameForm == PCA2D_FRAME_BOX){
            frameLineVec.append(QLine(leftLim,  upLim,   rightLim, upLim  ));
            frameLineVec.append(QLine(rightLim, upLim,   rightLim, downLim));
        }
        frameLineVec.append(QLine(leftLim,  upLim,   leftLim , downLim));
        frameLineVec.append(QLine(leftLim,  downLim, rightLim, downLim));
    }
    void calcuXAxisLine(QVector<QLine>& gridLineVec,
                        QVector<QLine>& axisLineVec,
                        QVector<QRect>& textRectVec,
                        QVector<QString>& textStrVec)
    {
        int width0=widgetSize.width(),height0=widgetSize.height();
        int upLim=yMarginFactor*height0/2;
        int downLim=(2-yMarginFactor)*height0/2;
        int xAxisLen=yMarginFactor*axisLengthFactor*height0/2;

        int i;
        QVector4D tVec;
        int xGridSize = xGridValue.size();
        for(i=0;i<xGridSize;++i){
            tVec=QVector4D(xGridValue[i],0,0,1);
            tVec=matTransform*tVec;
            if(tVec.x()>xMarginFactor-1&&
               tVec.x()<1-xMarginFactor){
                if(isDisplayGrid){
                    gridLineVec.append(QLine((tVec.x()+1)*width0/2,
                                      upLim,
                                      (tVec.x()+1)*width0/2,
                                      downLim));
                }
                if(isDisplayAxis){
                    axisLineVec.append(QLine((tVec.x()+1)*width0/2,
                                      downLim,
                                      (tVec.x()+1)*width0/2,
                                      downLim+xAxisLen));
                    textRectVec.append(QRect((tVec.x()+1)*width0/2-25,
                                      downLim+xAxisLen,
                                      50,
                                      40));
                    textStrVec.append(QString::number(xGridValue[i]));
                }
            }
        }
    }
    void calcuYAxisLine(QVector<QLine>& gridLineVec,
                        QVector<QLine>& axisLineVec,
                        QVector<QRect>& textRectVec,
                        QVector<QString>& textStrVec)
    {
        int width0=widgetSize.width(),height0=widgetSize.height();
        int leftLim=xMarginFactor*width0/2;
        int rightLim=(2-xMarginFactor)*width0/2;
        int yAxisLen=xMarginFactor*axisLengthFactor*width0/2;
        int i;
        QVector4D tVec;
        int yGridSize = yGridValue.size();
        for(i=0;i<yGridSize;++i){
            tVec=QVector4D(0,yGridValue[i],0,1);
            tVec=matTransform*tVec;
            if(tVec.y()>yMarginFactor-1&&
               tVec.y()<1-yMarginFactor){
                if(isDisplayGrid){
                    gridLineVec.append(QLine(leftLim,
                                      (1-tVec.y())*height0/2,
                                      rightLim,
                                      (1-tVec.y())*height0/2));
                }
                if(isDisplayAxis){
                    axisLineVec.append(QLine(leftLim,
                                      (1-tVec.y())*height0/2,
                                      leftLim-yAxisLen,
                                      (1-tVec.y())*height0/2));
                    textRectVec.append(QRect(leftLim-yAxisLen-50,
                                      (1-tVec.y())*height0/2-20,
                                      50,
                                      40));
                    textStrVec.append(QString::number(yGridValue[i]));
                }
            }
        }
    }
    void paintPCVar(QPainter *painter)
    {
        QString PC1Label,PC2Label;

        if(PC1VarRatio<MML::EPS){
            PC1Label="PC1";
        }
        else{
            PC1Label=QString("PC1 ")+QString::number(100*PC1VarRatio,'g',2)+"%";
        }
        if(PC2VarRatio<MML::EPS){
            PC2Label="PC2";
        }
        else{
            PC2Label=QString("PC2 ")+QString::number(100*PC2VarRatio,'g',2)+"%";
        }

        painter->save();
        painter->setPen(QPen(Qt::black,3));

        int width0=widgetSize.width(),height0=widgetSize.height();
        int leftLim=xMarginFactor*width0/2;
        int rightLim=(2-xMarginFactor)*width0/2;
        int upLim=yMarginFactor*height0/2;
        int downLim=(2-yMarginFactor)*height0/2;
        float yPC1LabelPos=downLim+yMarginFactor*PCLabelPosFactor*height0/2;
        float xPC1LabelPos=(leftLim+rightLim)/2;

        // PC2 position rotate 90
        float xPC2LabelPos=-(upLim+downLim)/2;
        float yPC2LabelPos=leftLim-xMarginFactor*PCLabelPosFactor*width0/2;

        painter->drawText(xPC1LabelPos-(1-xMarginFactor)*width0/2,
                          yPC1LabelPos,
                          (1-xMarginFactor)*width0,
                          yMarginFactor*PCLabelPosFactor*height0/2,
                          Qt::AlignTop|Qt::AlignHCenter,
                          PC1Label);

        // axis rotate -90
        painter->rotate(-90);

        painter->drawText(xPC2LabelPos-(1-yMarginFactor)*height0/2,
                          yPC2LabelPos-xMarginFactor*PCLabelPosFactor*width0/2,
                          (1-yMarginFactor)*height0,
                          xMarginFactor*PCLabelPosFactor*width0/2,
                          Qt::AlignBottom|Qt::AlignHCenter,
                          PC2Label);

        painter->resetTransform();
        painter->restore();
    }
};

#endif // PCA2DSCATTERFRAMEPAINTER_HPP

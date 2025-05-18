#ifndef GWASTHRESHOLDPAINTER_HPP
#define GWASTHRESHOLDPAINTER_HPP

#include <QColor>
#include <QPainter>
#include <algorithm>

#include "DataManager/ValPoints.hpp"
#include "StyleChoose/LineStyleParam.hpp"

class GWASThresholdPainter
{
private:
    QRect            displayArea;
    double           thresholdValue;
    QColor           lineColor;
    int              lineSize;
    Qt::PenStyle     lineStyle;
    bool             isPaintInfo;
    bool             isPaintLine;
    QFont            fontLineInfo;

    ValPoints*       tmpPoints;
    long long        tmpNPoints;
    double           tmpUpVal;
    double           tmpDownVal;
    long long        tmpUpPointsNum;
    int              tmpLineYPos;
public:
    GWASThresholdPainter(){
        displayArea       = QRect();
        thresholdValue    = tmpDownVal;
        lineColor         = QColor(255,0,0);
        isPaintInfo       = false;
        isPaintLine       = false;
        tmpPoints         = nullptr;
        tmpNPoints        = 0;
        tmpUpVal          = -1;
        tmpDownVal        = -1;
        tmpUpPointsNum    = 0;
        tmpLineYPos       = -1;
        fontLineInfo.setFamily("Times New Roman");
        fontLineInfo.setBold(true);
    }
    void setLineColor(QColor col){lineColor = col;}
    void setLineSize(int size){lineSize = size;}
    void setLineStyle(Qt::PenStyle style){lineStyle = style;}
    void setLineStyleParam(const LineStyleParam& param)
    {
        lineColor = param.lineColor;
        lineSize  = param.lineSize;
        lineStyle = param.lineStyle;
    }
    void getLineStyleParam(LineStyleParam& inParam)
    {
        inParam.lineColor = lineColor;
        inParam.lineSize  = lineSize;
        inParam.lineStyle = lineStyle;
    }
    QColor getLineColor() const{return lineColor;}
    double getThresholdValue() const{return thresholdValue;}

    void preSetIsPaintInfo(bool flag){isPaintInfo = flag;}
    void preSetIsPaintLine(bool flag){isPaintLine = flag;}
    void preSetDisplayArea(QRect rect){
        displayArea = rect;
        calcuLineYPos();
    }
    void preSetValueRange(double downVal,double upVal){
        tmpUpVal = upVal;
        tmpDownVal = downVal;
    }
    void preSetThresholdValue(double val){
        thresholdValue = val;
        thresholdValue = std::min(thresholdValue,tmpUpVal);
        thresholdValue = std::max(thresholdValue,tmpDownVal);
        calcuLineYPos();
        calcuUpPointsNum();
    }
    void preSetPoints(ValPoints* points,long long nPoints){
        tmpPoints = points;
        tmpNPoints = nPoints;
    }
    void paintThreshold(QPainter* painter){
        if(isPaintLine){
            paintLine(painter);
        }
        if(isPaintInfo){
            paintInfo(painter);
        }
    }
private:
    void calcuUpPointsNum(){
        if(tmpPoints == nullptr){
            tmpUpPointsNum = -1;
            return;
        }
        tmpUpPointsNum = 0;
        for(long long i = 0;i < tmpNPoints;++i){
            if(tmpPoints[i].val > thresholdValue){
                tmpUpPointsNum++;
            }
        }
    }
    void calcuLineYPos(){
        double rangeVal = tmpUpVal - tmpDownVal;
        tmpLineYPos = (tmpUpVal - thresholdValue)*displayArea.height()/rangeVal + displayArea.y();
    }
    void paintLine(QPainter* painter){
        int paintx0,paintx1;

        painter -> save();
        painter -> setPen(QPen(lineColor,lineSize,lineStyle));

        paintx0 = displayArea.x();
        paintx1 = displayArea.x() + displayArea.width();
        painter -> drawLine(paintx0,tmpLineYPos,paintx1,tmpLineYPos);

        painter -> restore();
    }
    void paintInfo(QPainter* painter){
        QFontMetrics fontMetrics(fontLineInfo);
        int xpos,ypos;
        int W,H;
        double meanVal = (tmpUpVal + tmpDownVal)/2;
        QString valText,nText;
        long long n = tmpUpPointsNum;
        int fw,fh;
        do{
            nText = ',' + QString::number(n % 1000) + nText;
            n /= 1000;
        } while(n != 0);
        nText.remove(0,1);
        nText = "up n : " + nText;
        valText = "threshold : " + QString::number(thresholdValue);
        fw = std::max(fontMetrics.horizontalAdvance(nText),
                      fontMetrics.horizontalAdvance(valText));
        fh = 2+fontMetrics.height();
        W = fw + 8,H = fh*2+8;
        if(thresholdValue > meanVal){
            ypos = tmpLineYPos - H -2;
        }
        else{
            ypos = tmpLineYPos + 2;
        }
        xpos = displayArea.x() + displayArea.width() - W - 8;
        painter->setFont(fontLineInfo);
        painter->setBrush(QColor(190,190,190));
        painter->setPen(QPen(Qt::transparent));
        painter->drawRect(xpos+2,ypos+2,W,H);
        painter->setBrush(Qt::white);
        painter->setPen(QPen(Qt::black,1));
        painter->drawRect(xpos,ypos,W,H);
        painter->setPen(QPen(Qt::red,1));
        painter->drawText(xpos+4,ypos+6,
                          fw,fh,
                          Qt::AlignLeft|Qt::AlignTop,
                          valText);
        painter->setPen(QPen(Qt::black,1));
        painter->drawText(xpos+4,ypos+fh+6,
                          fw,fh,
                          Qt::AlignLeft|Qt::AlignTop,
                          nText);
    }
};

#endif // GWASTHRESHOLDPAINTER_HPP

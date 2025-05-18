#ifndef PCASCATTERLEGENDPAINTER_HPP
#define PCASCATTERLEGENDPAINTER_HPP

#include <QColor>
#include <QFont>
#include <QSize>
#include <QFontMetrics>
#include <QPainter>
#include <QDebug>

#include "StyleChoose/LineStyleParam.hpp"

class PCAScatterLegendPainter
{
private:
    QPointF                paintPoint;
    QSize                  sizePaint;
    int                    leftBorder;
    int                    rightBorder;
    int                    topBorder;
    int                    buttomBorder;
    LineStyleParam         lineStyleParamFrame;
    QColor                 colorFrameBackground;
    QFont                  fontText;
    QColor                 colorText;
    int                    gapText;
    int                    lineSpace;
    int                    textWidth;
    int                    paintRectSize;
    QVector<QColor>        colorPopList;
    QVector<QString>       namePopList;
    bool                   isPaintLegend;
public:
    explicit PCAScatterLegendPainter() :
        leftBorder(5),
        rightBorder(5),
        topBorder(5),
        buttomBorder(5),
        gapText(8),
        lineSpace(6),
        paintRectSize(16),
        isPaintLegend(true)
    {
        fontText.setFamily("Times New Roman");
        fontText.setBold(true);
        colorText = Qt::black;
        colorFrameBackground = Qt::transparent;
        lineStyleParamFrame.lineColor = Qt::transparent;
        lineStyleParamFrame.lineSize = 1;
        lineStyleParamFrame.lineStyle = Qt::SolidLine;

        paintPoint = QPointF(1.0, 1.0);
    }
    void setPopData(const QVector<QString>& inNameList,
                    const QVector<QColor>& inColorList)
    {
        colorPopList = inColorList;
        namePopList = inNameList;
        setLegendSize();
    }
    void setPopData(char** inNameList,
                    long long inNPop,
                    const QVector<QColor>& inColorList)
    {
        namePopList.clear();
        for(long long i = 0;i < inNPop; ++i){
            namePopList.append(inNameList[i]);
        }
        colorPopList = inColorList;
        setLegendSize();
    }
    void setColorPopList(const QVector<QColor>& inColVec)
    {
        colorPopList = inColVec;
    }
    void setIsPaintLegend(bool flag)
    {
        isPaintLegend = flag;
    }
    void setPaintPoint(const QPointF& point)
    {
        paintPoint = point;
    }

    QPointF getPaintPoint() const{return paintPoint;}
    const QVector<QColor>& getColorPopList() const
    {
        return colorPopList;
    }

    bool isClicked(const QPoint& p)
    {
        int W = sizePaint.width(), H = sizePaint.height();
        int x0 = paintPoint.x(), y0 = paintPoint.y();
        return QRect(x0, y0, W, H).contains(p);
    }
    void paintLegend(QPainter* painter)
    {
        if(isPaintLegend == false){
            return ;
        }
        QColor tColor;
        long long nPop = namePopList.size();
        int x0 = paintPoint.x(), y0 = paintPoint.y();
        int tx, ty;
        painter->save();
        tx = x0 + leftBorder, ty = y0 + topBorder;
        painter->setBrush(colorFrameBackground);
        painter->setPen(QPen(lineStyleParamFrame.lineColor,
                             lineStyleParamFrame.lineSize,
                             lineStyleParamFrame.lineStyle));
        painter->drawRect(x0, y0, sizePaint.width(), sizePaint.height());
        painter->setPen(QPen(Qt::black));
        painter->setFont(fontText);
        for(long long i=0;i < nPop;++i){
            if(i >= colorPopList.size()){
                tColor = colorPopList.back();
            }
            else{
                tColor = colorPopList[i];
            }
            painter->setBrush(tColor);
            painter->drawRect(tx,
                              ty,
                              paintRectSize,
                              paintRectSize);
            painter->drawText(tx + paintRectSize + gapText,
                              ty - .5*lineSpace,
                              textWidth,
                              paintRectSize + lineSpace,
                              Qt::AlignVCenter|Qt::AlignLeft,
                              namePopList[i]);
            ty += (paintRectSize + lineSpace);
        }
        painter->restore();
    }
private:
    void setLegendSize()
    {
        QFontMetrics fm(fontText);
        int fw = 0;
        for(long long i = 0;i < namePopList.size(); ++i){
            fw = std::max(fw,fm.horizontalAdvance(namePopList[i]));
        }
        textWidth = fw;
        int W = leftBorder + paintRectSize + gapText + textWidth + rightBorder;
        int H = topBorder + (namePopList.size()-1)*lineSpace + namePopList.size()*paintRectSize + buttomBorder;
        sizePaint.setHeight(H);
        sizePaint.setWidth(W);
    }
};

#endif // PCASCATTERLEGENDPAINTER_HPP

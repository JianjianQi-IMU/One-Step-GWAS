#ifndef KINSHIPLEGENDPAINTER_HPP
#define KINSHIPLEGENDPAINTER_HPP

#include <QPoint>
#include <QSize>
#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <algorithm>
#include "StyleChoose/LineStyleParam.hpp"

class KinshipLegendPainter
{
private:
    QPoint                 paintPoint;
    QSize                  paintSize;
    int                    rectWidth;
    int                    rectHeight;
    int                    gapText;
    int                    textWidth;
    QFont                  fontText;
    QColor                 colorText;
    QColor                 lowColor;
    QColor                 highColor;
    QColor                 meanColor;
    double                 maxVal;
    double                 minVal;
    double                 meanVal;
    QString                maxValText;
    QString                minValText;
    QString                meanValText;
    int                    leftBorder;
    int                    rightBorder;
    int                    topBorder;
    int                    buttomBorder;
    LineStyleParam         lineStyleParamFrame;
    QColor                 colorFrameBackground;
    bool                   isPaintLegend;

public:
    KinshipLegendPainter()
        : rectWidth(20), rectHeight(160), gapText(8), leftBorder(5), rightBorder(5),
        topBorder(5), buttomBorder(5), isPaintLegend(true)
    {
        fontText.setFamily("Times New Roman");
        fontText.setBold(true);
        colorText = Qt::black;
        lowColor = Qt::blue;
        highColor = Qt::red;
        meanColor = Qt::white;
        colorFrameBackground = Qt::transparent;
        lineStyleParamFrame.lineColor = Qt::transparent;
        lineStyleParamFrame.lineSize = 1;
        lineStyleParamFrame.lineStyle = Qt::SolidLine;

        paintSize.setWidth(leftBorder + rightBorder + rectWidth + gapText + textWidth);
        paintSize.setHeight(topBorder + buttomBorder + rectHeight);

    }
    void setPaintPoint(const QPoint& point)
    {
        paintPoint = point;
    }
    void setRectColor(const QColor& colHigh, const QColor& colLow, const QColor& colMean)
    {
        lowColor = colLow;
        highColor = colHigh;
        meanColor = colMean;
    }
    void setVal(double valMax,double valMin,double valMean)
    {
        QFontMetrics fm(fontText);
        int fw = 0;
        maxVal = valMax;
        minVal = valMin;
        meanVal = valMean;
        maxValText  = QString::number(valMax);
        minValText  = QString::number(valMin);
        meanValText = QString::number(valMean);
        fw = std::max(fw,fm.horizontalAdvance(maxValText));
        fw = std::max(fw,fm.horizontalAdvance(minValText));
        fw = std::max(fw,fm.horizontalAdvance(meanValText));
        textWidth = fw;
        paintSize.setWidth(leftBorder + rightBorder + rectWidth + gapText + textWidth);
    }
    void setRectSize(int inW,int inH)
    {
        rectWidth = inW;
        rectHeight = inH;
        paintSize.setWidth(leftBorder + rightBorder + rectWidth + gapText + textWidth);
        paintSize.setHeight(topBorder + buttomBorder + rectHeight);
    }
    void setIsPaintLegend(bool flag)
    {
        isPaintLegend = flag;
    }

    QPoint getPaintPoint() const
    {
        return paintPoint;
    }

    bool isClicked(const QPoint& p)
    {
        return QRect(paintPoint.x(), paintPoint.y(),
            paintSize.width(), paintSize.height()).contains(p);
    }
    void paintLegend(QPainter* painter)
    {
        if (!isPaintLegend) {
            return;
        }

        QFontMetrics fm(fontText);
        int w0 = rectWidth, h0 = rectHeight / 2;
        int xLegend = paintPoint.x(), yLegend = paintPoint.y();
        QRect highRec(xLegend + leftBorder, yLegend + topBorder, w0, h0);
        QRect lowRec(xLegend + leftBorder, yLegend + topBorder + h0, w0, h0);
        QPainterPath path;
        int fh = fm.height();
        int textCenYHigh = yLegend + topBorder;
        int textCenYMean = yLegend + topBorder + h0;
        int textCenYLow = yLegend + topBorder + 2 * h0;
        int textX = xLegend + leftBorder + rectWidth + gapText;

        QLinearGradient highRecLinear(QPoint(xLegend, yLegend), QPoint(xLegend, yLegend + h0));
        QLinearGradient lowRecLinear(QPoint(xLegend, yLegend + h0), QPoint(xLegend, yLegend + 2 * h0));
        highRecLinear.setColorAt(0, highColor);
        highRecLinear.setColorAt(1, meanColor);
        lowRecLinear.setColorAt(0, meanColor);
        lowRecLinear.setColorAt(1, lowColor);

        highRecLinear.setSpread(QGradient::PadSpread);
        lowRecLinear.setSpread(QGradient::PadSpread);

        path.addRect(QRect(xLegend + leftBorder, yLegend + topBorder, rectWidth, rectHeight));

        painter->save();
        painter->setPen(QPen(lineStyleParamFrame.lineColor, lineStyleParamFrame.lineSize,
            lineStyleParamFrame.lineStyle));
        painter->setBrush(colorFrameBackground);
        painter->drawRect(QRect(xLegend, yLegend, paintSize.width(), paintSize.height()));

        painter->setPen(QPen(Qt::transparent));
        painter->setBrush(highRecLinear);
        painter->drawRect(highRec);

        painter->setBrush(lowRecLinear);
        painter->drawRect(lowRec);

        painter->setPen(QPen(Qt::black));
        painter->setBrush(Qt::transparent);
        painter->drawPath(path);

        painter->setFont(fontText);
        painter->setPen(QPen(colorText));
        painter->drawText(textX, textCenYHigh - 0.5 * fh, textWidth, fh,
            Qt::AlignLeft | Qt::AlignVCenter, maxValText);
        painter->drawText(textX, textCenYMean - 0.5 * fh, textWidth, fh,
            Qt::AlignLeft | Qt::AlignVCenter, meanValText);
        painter->drawText(textX, textCenYLow - 0.5 * fh, textWidth, fh,
            Qt::AlignLeft | Qt::AlignVCenter, minValText);

        painter->restore();
    }
};

#endif // KINSHIPLEGENDPAINTER_HPP

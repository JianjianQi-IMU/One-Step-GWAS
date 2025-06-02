#ifndef GWASQQPLOTFRAMEPAINTER_H
#define GWASQQPLOTFRAMEPAINTER_H

#include <QVector>
#include <QRect>
#include <QPainter>
#include <QFontMetrics>
#include <QPen>
#include <QDebug>

#include "GenomeAnnotation/WidgetTextPainter.hpp"


class GWASQQPlotFramePainter
{
private:
    QRect             displayRect;
    QVector<double>   xAxisDataVec;
    QVector<double>   yAxisDataVec;
    double            maxXAxisData;
    double            maxYAxisData;
    float             leftBorderFactor;
    float             rightBorderFactor;
    float             topBorderFactor;
    float             buttomBorderFactor;
    float             axisLengthFactor; // [0,1]
    QColor            backgroundColor;
    QFont             fontAxisValueText;
    QColor            colorDiagonalLine;

    float             xLabPosFactor;
    float             yLabPosFactor;
    QFont             fontXYLab;
    QColor            colorXYLab;
    QString           strYLab;
    QString           strXLab;
    WidgetTextPainter xLabTextPainter;
    WidgetTextPainter yLabTextPainter;
public:
    GWASQQPlotFramePainter();
    ~GWASQQPlotFramePainter();
    QColor getBackgroundColor() const
    {
        return backgroundColor;
    };
    void setXAxisData(const QVector<double>& inVec, double inMax) {
        xAxisDataVec = inVec;
        maxXAxisData = inMax;
    }
    void setYAxisData(const QVector<double>& inVec, double inMax) {
        yAxisDataVec = inVec;
        maxYAxisData = inMax;
    }
    void setDisplayRect(QRect inRect)
    {
        displayRect = inRect;
    }
    void setLeftBorderFactor(float factor)
    {
        leftBorderFactor = factor;
    }
    void setRightBorderFactor(float factor)
    {
        rightBorderFactor = factor;
    }
    void setTopBorderFactor(float factor)
    {
        topBorderFactor = factor;
    }
    void setButtomBorderFactor(float factor)
    {
        buttomBorderFactor = factor;
    }
    void setDiagonalLineColor(const QColor& col)
    {
        colorDiagonalLine = col;
    }

    const QColor& getDiagonalLineColor() const
    {
        return colorDiagonalLine;
    }

    void paintStart(QPainter* painter)
    {
        paintRect(painter);
        paintAxisValue(painter);
        paintDiagonalLine(painter);
        paintXYLabText(painter);
    }
private:
    void setXYLabPainter();
    void paintRect(QPainter* painter)
    {
        painter -> save();
        painter -> setPen(Qt::black);
        int W = displayRect.width();
        int H = displayRect.height();
        int x0 = displayRect.x();
        int y0 = displayRect.y();
        float x1 = x0 + W * leftBorderFactor;
        float x2 = x0 + W * (1 - rightBorderFactor);
        float y1 = y0 + H * topBorderFactor;
        float y2 = y0 + H * (1 - buttomBorderFactor);
        painter -> drawLine(x1, y1, x1, y2);
        painter -> drawLine(x1, y1, x2, y1);
        painter -> drawLine(x2, y1, x2, y2);
        painter -> drawLine(x1, y2, x2, y2);
        painter -> restore();
    }
    void paintAxisValue(QPainter* painter)
    {
        QVector<QLine> lineVec;
        painter -> save();
        painter -> setPen(Qt::black);
        painter -> setFont(fontAxisValueText);
        QFontMetrics fm(fontAxisValueText);
        double val,tyLabVal,txLabVal;
        int W = displayRect.width();
        int H = displayRect.height();
        int x0 = displayRect.x();
        int y0 = displayRect.y();
        float x1 = x0 + W * leftBorderFactor;
        float y2 = y0 + H * (1 - buttomBorderFactor);
        float xLabLen = H * buttomBorderFactor * axisLengthFactor;
        float yLabLen = W * leftBorderFactor * axisLengthFactor;
        double xRangeVal = maxXAxisData - 0;
        double yRangeVal = maxYAxisData - 0;
        int fH = fm.height(), fW;
        QString textVal;

        // x axis
        for (int i = 0; i < xAxisDataVec.size(); ++i) {
            val = xAxisDataVec[i];
            txLabVal = x0 + W * leftBorderFactor + val * W * (1 - leftBorderFactor - rightBorderFactor) / xRangeVal;
            lineVec.append(QLine(txLabVal, y2, txLabVal, y2 + yLabLen));
            textVal = QString::number(val);
            fW = fm.horizontalAdvance(textVal);
            painter->drawText(txLabVal - .5 * fW, y2 + yLabLen + 2, fW, fH, Qt::AlignHCenter | Qt::AlignTop, textVal);
        }
        // y axis
        for (int i = 0; i < yAxisDataVec.size(); ++i) {
            val = yAxisDataVec[i];
            tyLabVal = y0 + H * (1 - buttomBorderFactor) - val * H * (1 - topBorderFactor - buttomBorderFactor) / yRangeVal;
            lineVec.append(QLine(x1, tyLabVal, x1 - xLabLen, tyLabVal));
            textVal = QString::number(val);
            fW = fm.horizontalAdvance(textVal);
            painter->drawText(x1 - xLabLen - fW - 2, tyLabVal - .5 * fH, fW, fH, Qt::AlignRight | Qt::AlignVCenter, textVal);
        }

        painter -> drawLines(lineVec);
        painter -> restore();
    }
    void paintDiagonalLine(QPainter* painter)
    {
        painter -> save();
        painter -> setPen(colorDiagonalLine);
        int W = displayRect.width();
        int H = displayRect.height();
        int x0 = displayRect.x();
        int y0 = displayRect.y();
        double x1, x2, y1, y2;
        double xMax = maxXAxisData;
        double yMax = maxYAxisData;
        x1 = x0 + W * leftBorderFactor;
        y2 = y0 + H * (1 - buttomBorderFactor);
        if (xMax > yMax) {
            x2 = x0 + W * leftBorderFactor + (yMax / xMax) * W * (1 - leftBorderFactor - rightBorderFactor);
            y1 = y0 + H * topBorderFactor;
        } else {
            x2 = x0 + W * (1 - rightBorderFactor);
            y1 = y0 + H * topBorderFactor + (1 - xMax / yMax) * H * (1 - topBorderFactor - buttomBorderFactor);
        }
        painter -> drawLine(x2, y1, x1, y2);
        painter -> restore();
    }
    void paintXYLabText(QPainter* painter){
        int W = displayRect.width();
        int H = displayRect.height();
        int x0 = displayRect.x();
        int y0 = displayRect.y();
        QSize xSize = xLabTextPainter.getSize();
        QSize ySize = yLabTextPainter.getSize();

        xLabTextPainter.setPaintPoint(QPoint(x0 + (leftBorderFactor + .5 * (1 - leftBorderFactor - rightBorderFactor)) * W - .5 * xSize.width(),
            y0 + (1 - xLabPosFactor) * H - .5 * xSize.height()));
        yLabTextPainter.setPaintPoint(QPoint(x0 + yLabPosFactor * W - .5 * ySize.height(),
            y0 + (topBorderFactor + .5 * (1 - topBorderFactor - buttomBorderFactor)) * H + .5 * ySize.width()));

        xLabTextPainter.paintText(painter);
        yLabTextPainter.paintText(painter);
    }
};

#endif // GWASQQPLOTFRAMEPAINTER_H

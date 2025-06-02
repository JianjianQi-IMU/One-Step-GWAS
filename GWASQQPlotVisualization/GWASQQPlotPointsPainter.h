#ifndef GWASQQPLOTPOINTSPAINTER_H
#define GWASQQPLOTPOINTSPAINTER_H

#include "DataManager/DataManager.hpp"
#include "DataManager/ValPoints.hpp"
#include <QVector>
#include <algorithm>
#include <QColor>
#include <QPainterPath>
#include <QPolygonF>
#include "GenomeAnnotation/BaseOpenGLPaintWidget.hpp"
#include "GenomeAnnotation/WidgetTextPainter.hpp"
#include "GenomeAnnotation/ChromosomeInfoClass.hpp"

class GWASQQPlotPointsPainter
{
private:
    MML::Distribution stats;
    QRect             displayRect;
    ValPoints2*       dataPoints;
    long long         nPoints;
    QColor            colorPoints;
    QColor            colorSelectedPoints;
    GLuint            pointsVAO;
    GLuint            pointsVBO;
    GLfloat*          pointsData;
    int               idPointsShader;
    double            maxVal;
    double            minVal;
    QVector<double>   xAxisDataVec;
    QVector<double>   yAxisDataVec;
    double            maxXAxisData;
    double            maxYAxisData;
    float             leftBorderFactor;
    float             rightBorderFactor;
    float             topBorderFactor;
    float             buttomBorderFactor;
    float             selectedPointsDistance2;
    long long         selectedPointIndex;
    QFont             fontPointsInfo;

    BaseOpenGLPaintWidget*   tmpPaintWidget;
    ChromosomeInfoClass*     tmpChrInfo;
    WidgetTextPainter        selectedTextPainter;
public:
    explicit GWASQQPlotPointsPainter();
    ~GWASQQPlotPointsPainter();

    bool loadPoints(ValPoints *inPoints,long long inNPoints);
    bool setAxisValue(double xMax, double yMax, bool isReset = false);
    void setPaintWidget(BaseOpenGLPaintWidget* inPaintWidget)
    {
        tmpPaintWidget = inPaintWidget;
    }
    void setChrInfo(ChromosomeInfoClass* inChrInfo)
    {
        tmpChrInfo = inChrInfo;
    }
    void setDisplayRect(QRect inRect)
    {
        displayRect = inRect;
    }
    void setPointsColor(const QColor& col)
    {
        colorPoints = col;
    }
    void getXAxisData(QVector<double>& outAxisDataVec) const
    {
        outAxisDataVec = xAxisDataVec;
    }
    void getYAxisData(QVector<double>& outAxisDataVec) const
    {
        outAxisDataVec = yAxisDataVec;
    }
    const QColor& getPointsColor() const
    {
        return colorPoints;
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
    void setSelectedPointIndex(long long idPoint)
    {
        selectedPointIndex = idPoint;
    }
    double getMaxVal() const
    {
        return maxVal;
    }
    double getMinVal() const
    {
        return minVal;
    }

    void initInitializeGL();
    long long isPressedPoint(const QPoint& inPos)
    {
        long long i;
        for (i = 0; i < nPoints; ++i) {
            if (isSelectedPoint(i, inPos.x(), inPos.y())) break;
        }
        if (i < nPoints) return i;
        return -1;
    }
    void paintStart(QPainter* painter)
    {

        adjustPaintData();
        sendPaintPointsToBuffer();
        tmpPaintWidget->shaderProgramBind(idPointsShader);
        tmpPaintWidget->setUniformValue("pointsColor", colorPoints, idPointsShader);
        if (nPoints != 0) {
            tmpPaintWidget->glBindVertexArray(pointsVAO);
            tmpPaintWidget->glDrawArrays(GL_POINTS, 0, nPoints);
        }
        tmpPaintWidget->glBindVertexArray(0);
        tmpPaintWidget->shaderProgramRelease(idPointsShader);

        paintSelectedPoint(painter);
        paintSelectedPointInfo(painter, selectedPointIndex);
    }
    void savePaintStart(QPainter* painter)
    {
        painter -> save();

        long long i;
        QPointF pxy;
        QPolygonF pointVec;
        painter -> setPen(QPen(colorPoints, 6, Qt::SolidLine, Qt::RoundCap));
        for (i = 0; i < nPoints; ++i) {
            pxy = calcuPoint(i);
            pointVec << (pxy);
        }
        painter -> drawPoints(pointVec);

        painter -> restore();
    }
private:
    void adjustPaintData()
    {
        int W0 = tmpPaintWidget->width();
        int H0 = tmpPaintWidget->height();
        if (nPoints == 0) {
            return;
        }
        for (long long i = 0; i < nPoints; ++i) {
            QPointF pos = calcuPoint(i);
            pointsData[2 * i]  = 2 * pos.x() / W0 - 1.0; // expectedVal val2
            pointsData[2 * i + 1]= -2 * pos.y() / H0 + 1.0; // observedVal val1
        }
    }
    void sendPaintPointsToBuffer()
    {
        tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
        tmpPaintWidget->glBindVertexArray(pointsVAO);
        tmpPaintWidget->glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * nPoints * sizeof(GLfloat), pointsData);
        tmpPaintWidget->glBindVertexArray(0);
        tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    QPointF calcuPoint(long long idPoint)
    {
        double xVal, yVal, xRangeVal, yRangeVal;
        int W = displayRect.width();
        int H = displayRect.height();
        int x0 = displayRect.x();
        int y0 = displayRect.y();
        xRangeVal = maxXAxisData;
        yRangeVal = maxYAxisData;
        xVal = x0 + W * leftBorderFactor + W * (1 - leftBorderFactor - rightBorderFactor) * dataPoints[idPoint].val2 / xRangeVal;
        yVal = y0 + H * (1 - buttomBorderFactor) - H * (1 - topBorderFactor - buttomBorderFactor) * dataPoints[idPoint].val1 / yRangeVal;
        return QPointF(xVal, yVal);
    }
    bool isSelectedPoint(long long idPoint, int x, int y)
    {
        double dx, dy;
        QPointF pxy = calcuPoint(idPoint);
        dx = pxy.x() - x, dy = pxy.y() - y;
        if (dx * dx + dy * dy <= selectedPointsDistance2) {
            return true;
        }
        return false;
    }
    void paintSelectedPoint(QPainter *painter)
    {
        if (selectedPointIndex < 0 || selectedPointIndex >= nPoints) {
            return;
        }

        painter->save();

        int H = tmpPaintWidget->height(), W = tmpPaintWidget->width();
        QPainterPath labelShape;
        QPointF pxy = calcuPoint(selectedPointIndex);
        if (pxy.x() >= 0 && pxy.x() <= W && pxy.y() >= 0 && pxy.y() <= H) {
            painter->setPen(QPen(QBrush(colorSelectedPoints), 3));
            labelShape.addEllipse(QPointF(pxy.x(), pxy.y()), 6, 6);
            painter->drawPath(labelShape);
        }

        painter->restore();
    }
    void paintSelectedPointInfo(QPainter *painter, long long idPoint)
    {
        if (idPoint < 0 || idPoint >= nPoints) return;
        QPointF pos = calcuPoint(idPoint);
        char** chrName = tmpChrInfo -> getChrName();
        ValPoints2 pPoint = dataPoints[idPoint];
        QString textChr = QString("Chr : ") + chrName[pPoint.idchr];
        QString textPos = QString("Pos : ") + QString::number(pPoint.pos);
        QString textVal = QString::number(pPoint.val1);

        selectedTextPainter.clear();

        selectedTextPainter.setPaintPoint(QPoint(pos.x() + 4, pos.y() + 4));
        selectedTextPainter.addText(textVal, Qt::red, fontPointsInfo);
        selectedTextPainter.addText(textChr, Qt::black, fontPointsInfo);
        selectedTextPainter.addText(textPos, Qt::black, fontPointsInfo);
        selectedTextPainter.paintText(painter);

    }
    void delDeleteGL();
    void calcuAxisData();
private:
    static bool ValPoints2CompLess(const ValPoints2& a, const ValPoints2& b)
    {
        return a.val1 < b.val1;
    }
};

#endif // GWASQQPLOTPOINTSPAINTER_H

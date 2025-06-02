#ifndef KINSHIPHEATMAPPAINTER_HPP
#define KINSHIPHEATMAPPAINTER_HPP

#include <QColor>
#include <QVector>
#include <QString>
#include <QPoint>
#include <QRectF>
#include <QMatrix4x4>
#include <QVector4D>
#include <QPainter>
#include "Cluster/Cluster.hpp"
#include "DataManager/MatrixLib.hpp"
#include "StyleChoose/LineStyleParam.hpp"
#include "GenomeAnnotation/WidgetTextPainter.hpp"
#include "StyleChoose/LineStyleParam.hpp"

class KinDis
{
private:
    MML::Mat dis;
    MML::Mat val;
public:
    explicit KinDis();
    explicit KinDis(const MML::Mat& inKin);
    ~KinDis();
    void setData(const MML::Mat& inKin);
    double operator ()(long long a, long long b)
    {
        return dis(a, b);
    }
    double operator ()(long long id)
    {
        return val(id);
    }
};

class KinshipHeatmapPainter
{
private:
    MML::Mat               kin;
    QVector<QString>       individualsName;
    QVector<long long>     paintSeq;
    QColor                 lowColor;
    QColor                 highColor;
    QColor                 meanColor;
    double                 maxVal;
    double                 minVal;
    double                 meanVal;
    QMatrix4x4             matView;
    double                 oneSize;
    long long              rowSelected;
    long long              colSelected;
    bool                   isDisplaySelectedText;
    LineStyleParam         lineStyleParamSelectedFrame;
    WidgetTextPainter      selectedTextPainter;
    QColor                 colorSelectedText;
    QFont                  fontSelectedText;
public:
    KinshipHeatmapPainter();
    KinshipHeatmapPainter(const MML::Mat& inKin, char** inName);
    void loadKinshipData(const MML::Mat& inKin, char** inName);
    void setMatView(const QMatrix4x4& mat)
    {
        matView = mat;
    }
    void setRangeValue(double highVal, double lowVal, double middleVal)
    {
        maxVal = highVal;
        minVal = lowVal;
        meanVal = middleVal;
    }
    void setLowColor(const QColor& col)
    {
        lowColor = col;
    }
    void setHighColor(const QColor& col)
    {
        highColor = col;
    }
    void setRangeColor(QColor highCol, QColor lowCol, QColor meanCol)
    {
        highColor = highCol;
        lowColor = lowCol;
        meanColor = meanCol;
    }
    void setMeanColor(const QColor& col)
    {
        meanColor = col;
    }
    void setOnePointSize(double inSize)
    {
        oneSize = inSize;
    }
    void setIsDisplaySelectedText(bool flag)
    {
        isDisplaySelectedText = flag;
    }
    void setSelectedIndex(long long rowIndex, long long colIndex)
    {
        rowSelected = rowIndex;
        colSelected = colIndex;
    }
    double getMaxVal() const
    {
        return maxVal;
    }
    double getMinVal() const
    {
        return minVal;
    }
    double getMeanVal() const
    {
        return meanVal;
    }
    QColor getLowColor() const
    {
        return lowColor;
    }
    QColor getHighColor() const
    {
        return highColor;
    }
    QColor getMeanColor() const
    {
        return meanColor;
    }
    long long getNIndividuals() const
    {
        return kin.getNRow();
    }

    void setClusterIndex();
    void isClickedPoint(const QPoint& point, long long &outRowIndex, long long &outColIndex)
    {
        outRowIndex = -1;
        outColIndex = -1;
        long long nInd = kin.getNRow();
        QRectF rect;
        for (long long i = 0; i < nInd; ++i) {
            for (long long j = 0; j < nInd; ++j) {
                rect = calcuPointRectPix(i, j);
                if (rect.contains(point)) {
                    outRowIndex = i;
                    outColIndex = j;
                    return;
                }
            }
        }
    }
    QRectF calcuPointRectF01(long long idRow, long long idCol)
    {
        double oneSizeF = 1.0 / kin.getNRow();
        QVector4D vec1 = matView * QVector4D(oneSizeF * idRow, oneSizeF * idCol, 0, 1.0);
        QVector4D vec2 = matView * QVector4D(oneSizeF * (idRow + 1), oneSizeF * (idCol + 1), 0, 1.0);
        return QRectF(vec1.x(), vec1.y(), (vec2.x() - vec1.x()), (vec2.y() - vec1.y()));
    }
    QRectF calcuPointRectPix(long long idRow,long long idCol)
    {
        QRectF rect = calcuPointRectF01(idRow, idCol);
        return QRectF(rect.x() * kin.getNRow() * oneSize, rect.y() * kin.getNRow() * oneSize,
            rect.width() * kin.getNRow() * oneSize, rect.height() * kin.getNRow() * oneSize);
    }
    void paintMap(QPainter* painter)
    {
        paintHeatmap(painter);
        paintSelectedPoint(painter);
    }
    void savePaintMap(QPainter* painter)
    {
        paintHeatmap(painter);
    }
private:
    QColor getPointColor(double value)
    {
        QColor tColor;
        double tratio;
        if (value > meanVal) {
            if (value > maxVal) {
                value = maxVal;
            }
            tratio = (value - meanVal) / (maxVal - meanVal);
            tColor = QColor(meanColor.red() + (highColor.red() - meanColor.red()) * tratio,
                meanColor.green() + (highColor.green() - meanColor.green()) * tratio,
                meanColor.blue() + (highColor.blue() - meanColor.blue()) * tratio);
        } else {
            if (value < minVal) {
                value = minVal;
            }
            tratio = (meanVal - value) / (meanVal - minVal);
            tColor = QColor(meanColor.red() + (lowColor.red() - meanColor.red()) * tratio,
                meanColor.green() + (lowColor.green() - meanColor.green()) * tratio,
                meanColor.blue() + (lowColor.blue() - meanColor.blue()) * tratio);
        }
        return tColor;
    }
    void paintHeatmap(QPainter* painter)
    {
        QRectF rect;
        QColor tColor;
        long long nInd = kin.getNRow();
        long long ti, tj;
        double tval;
        painter->save();
        for (long long i = 0; i < nInd; ++i) {
            for (long long j = 0; j < nInd; ++j) {
                rect = calcuPointRectPix(i, j);
                ti = paintSeq[i], tj = paintSeq[j];
                tval = kin(ti, tj);
                tColor = getPointColor(tval);
                painter->fillRect(rect, tColor);
            }
        }
        painter -> restore();
    }
    void paintSelectedPoint(QPainter* painter)
    {
        long long nInd = kin.getNRow();
        if (rowSelected < 0 || rowSelected >= nInd || colSelected < 0 || colSelected >= nInd) {
            return;
        }
        QRectF rect = calcuPointRectPix(rowSelected, colSelected);
        painter -> save();
        painter -> setPen(QPen(lineStyleParamSelectedFrame.lineColor,
            lineStyleParamSelectedFrame.lineSize,
            lineStyleParamSelectedFrame.lineStyle));
        painter -> setBrush(Qt::transparent);
        painter -> drawRect(rect);
        painter -> restore();
        if (isDisplaySelectedText) {
            paintSelectedPointText(painter);
        }
    }
    void paintSelectedPointText(QPainter* painter)
    {
        long long idRow, idCol;
        QString name1,name2;
        double tval;
        QColor tColor;
        QRectF rect = calcuPointRectPix(rowSelected, colSelected);
        idRow = paintSeq[rowSelected], idCol = paintSeq[colSelected];
        name1 = individualsName[idRow];
        name2 = individualsName[idCol];
        tval = kin(idRow, idCol);
        tColor = getPointColor(tval);
        selectedTextPainter.clear();
        selectedTextPainter.setPaintPoint(QPoint(rect.x() + rect.width(), rect.y() + rect.height()));
        selectedTextPainter.addText(QString::number(tval), tColor, fontSelectedText);
        selectedTextPainter.addText(name1 + " & " + name2, colorSelectedText, fontSelectedText);
        selectedTextPainter.paintText(painter);
    }
};

#endif // KINSHIPHEATMAPPAINTER_HPP

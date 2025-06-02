#ifndef LOGPSCATTERVIEWPAINTER_HPP
#define LOGPSCATTERVIEWPAINTER_HPP

#include <QRect>
#include <QPainter>
#include <QVector3D>

#include "DataManager/ValPoints.hpp"
#include "GenomeAnnotation/ChromosomeInfoClass.hpp"
#include "GenomeAnnotation/BaseOpenGLPaintWidget.hpp"
#include "GenomeAnnotation/WidgetTextPainter.hpp"

class LogPScatterViewPainter
{
private:
    QRect                    displayArea;
    QSize                    widgetSize;
    int                      maxAxisLabelNumY;
    float                    selectedPointsDistance2;
    float                    pointSize;
    QVector<QColor>          colorScatterPoint;
    QColor                   selectedPointColor;
    int                      LogPAxisScaleLen;
    bool                     isDisplayMovingLogPLine;
    QColor                   movingLogPLineColor;
    QFont                    fontPointsInfo;
    QFont                    fontAxis;
    QFont                    fontYTitle;
    QString                  strYTitle;
    WidgetTextPainter        yTitlePainter;

    GLuint                  *idPointColor;

    ValPoints               *points;
    long long               *cumPointsPos;
    long long                nPoints;
    double                   maxLogP;
    double                   minLogP;
    int                      intUpLogP;
    int                      intDownLogP;
    int                      logPStride;

    GLfloat                 *paintPoints;
    long long                nPaintPoints;

    long long                currentMinPointsi;
    long long                currentMaxPointsi;

    long long                selectedPointi;
    WidgetTextPainter        selectedPointInfoPainter;
    bool                     displaySelectedPointInfo;

    QVector<long long>       idLabelPoints;
    QColor                   labeledPointColor;
    int                      maxLabelPointsNum;

    GLuint                   idPointsVAO;
    GLuint                   idPointsVBO;
    GLuint                   idColorVBO;

    long double              tmpCurrentLeftPosF;
    long double              tmpCurrentRightPosF;
    ChromosomeInfoClass*     tmpChrInfo;
    BaseOpenGLPaintWidget*   tmpPaintWidget;
    int                      tmpMovingMouseY;
public:
    LogPScatterViewPainter();
    ~LogPScatterViewPainter();
    bool loadPoints(ValPoints *inPoints, long long inNPoints);
    inline ValPoints* getPointValAt(long long idPoint)
    {
        if(idPoint < 0 || idPoint >= nPoints)
        {
            return nullptr;
        }
        return points + idPoint;
    }
    inline float getPointSize() const
    {
        return pointSize;
    }
    inline double getMaxLogP() const
    {
        return maxLogP;
    }
    inline double getMinLogP() const
    {
        return minLogP;
    }
    inline int getIntUpLogP() const
    {
        return intUpLogP;
    }
    inline int getIntDownLogP() const
    {
        return intDownLogP;
    }
    inline ValPoints* getPoints()
    {
        return points;
    }
    inline long long getNPoints() const
    {
        return nPoints;
    }
    inline const QVector<QColor>& getPointsColor() const
    {
        return colorScatterPoint;
    }
    inline int getMaxLabelPointsNum() const
    {
        return maxLabelPointsNum;
    }
    inline long long getNPaintPoints() const
    {
        return nPaintPoints;
    }
    inline long long getLabelPointAt(int row) const
    {
        if (idLabelPoints.size() <= row || row < 0) {
            return -1;
        }
        return idLabelPoints[row];
    }
    inline bool getDisplayMovingLogPLine() const
    {
        return isDisplayMovingLogPLine;
    }

    inline void preSetMovingMouseY(int ypos)
    {
        tmpMovingMouseY = ypos;
    }
    inline void setPointSize(float val)
    {
        pointSize = val;
    }
    inline void setDisplaySelectedPointInfo(bool flag)
    {
        displaySelectedPointInfo = flag;
    }
    inline bool setColorScatterPoint(const QVector<QColor>& inColor)
    {
        if (inColor.empty()) {
            return false;
        }
        colorScatterPoint = inColor;
        if (colorScatterPoint.length() > 8) {
            int n = colorScatterPoint.length();
            colorScatterPoint.remove(8, n - 8);
        }
        return true;
    }
    inline void setDisplayMovingLogPLine(bool flag)
    {
        isDisplayMovingLogPLine = flag;
    }
    inline void clearLabelPoints()
    {
        idLabelPoints.clear();
    }
    inline bool removeLabelPointAt(int row)
    {
        if (idLabelPoints.size() <= row || row < 0) {
            return false;
        }
        idLabelPoints.removeAt(row);
        return true;
    }
    inline int removeLabelPointID(long long idPoint)
    {
        int i = -1;
        bool havePoint = false;
        if (idPoint < 0) {
            return -1;
        }
        for (i = 0; i < idLabelPoints.size(); ++i) {
            if (idLabelPoints[i] == idPoint) {
                havePoint = true;
                break;
            }
        }
        if (!havePoint) {
            return -1;
        }
        idLabelPoints.removeAt(i);
        return i;
    }
    inline int addLabelPoint(long long idPoint)
    {
        bool havePoint = false;
        if (idLabelPoints.size() >= maxLabelPointsNum) {
            return -1;
        }
        if (nPaintPoints) {
            if (idPoint >= 0) {
                for (int j = 0; j < idLabelPoints.size(); ++j) {
                    if (idLabelPoints[j] == idPoint) {
                        havePoint = true;
                        break;
                    }
                }
                if (havePoint) {
                    return -1;
                }
                idLabelPoints.append(idPoint);
            }
        }
        return idLabelPoints.size() - 1;
    }

    inline void preSetPaintWidget(BaseOpenGLPaintWidget* inPaintWidget)
    {
        tmpPaintWidget = inPaintWidget;
    }
    inline void preSetDisplayArea(const QRect& inDisplayArea)
    {
        displayArea = inDisplayArea;
    }
    inline void preSetWidgetSize(const QSize& inWidgetSize)
    {
        widgetSize = inWidgetSize;
    }
    inline void preSetChrInfo(ChromosomeInfoClass* inChrInfo)
    {
        tmpChrInfo = inChrInfo;
    }
    inline void preSetCurrentPosF(const long double& inCurrentLeftPosF, const long double& inCurrentRightPosF)
    {
        tmpCurrentLeftPosF = inCurrentLeftPosF;
        tmpCurrentRightPosF = inCurrentRightPosF;
    }
    inline void preSetSelectedPointi(long long idPoint){
        selectedPointi = idPoint;
    }

    void initInitializeGL();
    inline bool isClicked(const QPoint& pos)
    {
        int xpos = pos.x(), ypos = pos.y();
        if (xpos >= displayArea.x() && xpos <= displayArea.x() + displayArea.width() &&
            ypos >= displayArea.y() && ypos <= displayArea.y() + displayArea.height())
        {
            return true;
        }
        return false;
    }
    // false : -1 | true : clicked point id
    inline long long isClickedPoint(const QPoint& pos)
    {
        if (nPaintPoints) {
            long long i = currentMinPointsi;
            for (; i <= currentMaxPointsi; ++i) {
                if (isSelectedPoint(i, pos.x(), pos.y())) {
                    break;
                }
            }
            if (i <= currentMaxPointsi) {
                return i;
            }
        }
        return -1;
    }
    inline bool isSelectedPoint(long long idPoint, int x, int y)
    {
        int px, py;
        if (!getPointPxy(idPoint, px, py)) {
            return false;
        }
        int dx = px - x, dy = py - y;
        if (dx * dx + dy * dy <= selectedPointsDistance2) {
            return true;
        }
        return false;
    }
    void paintLogPScatterView(QPainter *painter);
    void paintMovingLogPLine(QPainter *painter);

    void savePaintLogPScatterView(QPainter *painter);
private:
    void initYTitlePainter()
    {
        yTitlePainter.addText(strYTitle, Qt::black, fontYTitle);
        yTitlePainter.setRotation(-90);
        yTitlePainter.setIsPaintRect(false);
    }
    void delDeleteGL();
    // origin point is at left-up orientation (outPx, outPy)
    inline bool getPointPxy(long long  idPoint, int &outPx, int &outPy)
    {
        if (idPoint < 0 || idPoint >= nPoints) {
            return false;
        }
        int xpos = displayArea.x(), ypos = displayArea.y();
        int width = displayArea.width(), height = displayArea.height();
        long long* cumChrLen = tmpChrInfo->getCumChrLen();
        long double positionGapLenF = tmpCurrentRightPosF - tmpCurrentLeftPosF;
        outPx = (points[idPoint].pos + cumChrLen[points[idPoint].idchr] - tmpCurrentLeftPosF) * width / positionGapLenF + xpos;
        outPy = height * (long double)(1 - points[idPoint].val / (intUpLogP - intDownLogP)) + ypos;
        return true;
    }
    inline void adjustPaintPointsPos()
    {
        int width0 = widgetSize.width(), height0 = widgetSize.height();
        int xpos = displayArea.x(), ypos = displayArea.y();
        int width = displayArea.width(), height = displayArea.height();
        long double positionGapLenF = tmpCurrentRightPosF - tmpCurrentLeftPosF;
        long long* cumChrLen = tmpChrInfo->getCumChrLen();
        float tx, ty;
        for (long long i = currentMinPointsi; i <= currentMaxPointsi; ++i) {
            tx = 2.0 * (float(points[i].pos + cumChrLen[points[i].idchr] - tmpCurrentLeftPosF) * width / positionGapLenF + xpos) / width0 - 1.0;
            ty = -2.0 * (height * (1 - points[i].val / (intUpLogP - intDownLogP)) + ypos) / height0 + 1.0;
            paintPoints[(i - currentMinPointsi) * 2] = tx;
            paintPoints[(i - currentMinPointsi)  *2 + 1] = ty;
        }
    }
    inline void adjustPaintPointsColor()
    {
        int colorNum = colorScatterPoint.length();
        GLuint idColor = 0;
        for (long long i = currentMinPointsi; i <= currentMaxPointsi; ++i) {
            idColor = points[i].idchr % colorNum;
            idPointColor[i - currentMinPointsi] = idColor;
        }
    }
    void sendPointsToBuffer();
    void paintAxisInPainter(QPainter *painter);
    void paintSelectedPoint(QPainter *painter);
    void paintPointInfo(QPainter *painter, long long idPoint);
    void paintLabelPoints(QPainter *painter);
    void paintYTitle(QPainter *painter);
};

#endif // LOGPSCATTERVIEWPAINTER_HPP

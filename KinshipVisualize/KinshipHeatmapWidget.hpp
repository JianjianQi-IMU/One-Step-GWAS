#ifndef KINSHIPHEATMAPWIDGET_HPP
#define KINSHIPHEATMAPWIDGET_HPP

#include "DataManager/MatrixLib.hpp"
#include "FileDeal/FileDeal.hpp"
#include "Cluster/Cluster.hpp"
#include "GenomeAnnotation/BaseOpenGLPaintWidget.hpp"
#include "KinshipHeatmapPainter.hpp"
#include "KinshipLegendPainter.hpp"
#include "KinshipFramePainter.hpp"
#include "FileDeal/BaseFileSaveDialog.hpp"
#include <QMouseEvent>
#include <QPainter>
#include <QVector>
#include <QLinearGradient>
#include <QPainterPath>

enum KinshipLeftMouseMode
{
    KIN_LEFT_DEFAULT = 0,
    KIN_LEFT_LEGEND = 1,
    KIN_LEFT_MAIN = 2,
};

class KinshipHeatmapWidget : public BaseOpenGLPaintWidget
{
    Q_OBJECT
private:
    explicit KinshipHeatmapWidget(QWidget *parent = nullptr);
public:
    int                frameSize;
    QMatrix4x4         viewMat;

    QPoint             oldLegendPoint;

    float              xCamera;           // [-1,1]
    float              yCamera;           // [-1,1]
    float              zoomValue;         // [1,inf]
    float              maxZoomValue;
    float              minZoomValue;
    float              zoomTick;

    int                maxXCamera;
    int                minXCamera;
    int                maxYCamera;
    int                minYCamera;

    int                middleMousePressedPosX;
    int                middleMousePressedPosY;
    int                leftMousePressedPosX;
    int                leftMousePressedPosY;

    bool               isMiddleMousePressed;
    bool               isLeftMousePressed;

    KinshipLeftMouseMode       leftMousePressedMode;

    KinshipHeatmapPainter*     pHeatmapPainter;
    KinshipLegendPainter*      pLegendPainter;
    KinshipFramePainter*       pFramePainter;

    KinshipHeatmapWidget(const MML::Mat& inKin,
                         char** inName,
                         QWidget *parent = nullptr);
    ~KinshipHeatmapWidget();
    void mouseMoveEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void setClusterIndex();
    void setRangeValue(double highVal, double lowVal, double meanVal);
    void setRangeColor(QColor highCol, QColor lowCol, QColor meanCol);

    void getRangeValue(double& outHighVal,double& outLowVal,double& outMeanVal);
    void getRangeColor(QColor& outHighCol,QColor& outLowCol,QColor& outMeanCol);
signals:

protected:
    void leftMousePressEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void middleMousePressEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void leftMouseReleaseEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void middleMouseReleaseEvent(QMouseEvent* e,bool &isNeedUpdate) override;


private:
    void setViewMat();
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void savePaintCurrentSVG(QString savePath,
                             QSizeF inSize);
    void savePaintCurrentPNG(QString savePath,
                             QSizeF inSize);
public slots:
    void savePlot(FD::SaveFileInfo inInfo);
};

#endif // KINSHIPHEATMAPWIDGET_HPP

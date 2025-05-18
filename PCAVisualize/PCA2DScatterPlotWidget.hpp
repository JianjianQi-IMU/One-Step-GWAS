#ifndef PCA2DSCATTERPLOTWIDGET_HPP
#define PCA2DSCATTERPLOTWIDGET_HPP

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>
#include <QSvgGenerator>
#include "GenomeAnnotation/BaseOpenGLPaintWidget.hpp"
#include "PCA2DScatterFramePainter.hpp"
#include "PCA2DScatterPointsPainter.hpp"
#include "PCAScatterLegendPainter.hpp"
#include "FileDeal/FileDeal.hpp"
#include "FileDeal/BaseFileSaveDialog.hpp"

enum PCA2DLeftMouseMode
{
    PCA_2D_LEFT_DEFAULT = 0,
    PCA_2D_LEFT_LEGEND = 1,
    PCA_2D_LEFT_MAIN = 2,
};

class PCA2DScatterPlotWidget : public BaseOpenGLPaintWidget
{
    Q_OBJECT
private:
    QMatrix4x4 modelMat;
    QMatrix4x4 viewMat;

    float xCamera;           // [-1,1]
    float yCamera;           // [-1,1]
    float zoomValue;         // [1,inf]
    float maxZoomValue;
    float minZoomValue;
    float zoomTick;

    QPointF oldLegendPoint;

    bool isMiddleMousePressed;
    bool isLeftMousePressed;

    int middleMousePressedPosX;
    int middleMousePressedPosY;
    int leftMousePressedPosX;
    int leftMousePressedPosY;

    PCA2DLeftMouseMode leftMousePressedMode;

    PCA2DScatterFramePainter*  pFramePainter;
    PCA2DScatterPointsPainter* pPointsPainter;
    PCAScatterLegendPainter*   pLegendPainter;
private:
    void leftMousePressEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void middleMousePressEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void leftMouseReleaseEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void middleMouseReleaseEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void leftMouseMoveEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void middleMouseMoveEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void wheelEvent(QWheelEvent* e) override;
public:
    explicit PCA2DScatterPlotWidget(QWidget *parent = nullptr);
    PCA2DScatterPlotWidget(const double* inData,
                           const long long* inPopIndex,
                           char** inSampleName,
                           long long inNSample,
                           QWidget *parent = nullptr);
    ~PCA2DScatterPlotWidget();
    bool loadSampleData(const double* inData,
                        const long long* inPopIndex,
                        char** inSampleName,
                        long long inNSample);
    bool setPopName(char** inPopName,long long inNPop);
    bool setVarRatio(double inPC1VarRatio,double inPC2VarRatio);
    void setDisplayLegend(bool flag);
    void setDisplayGridLine(bool flag);
    void setDisplayAxisValue(bool flag);
    void setDisplayPCLabel(bool flag);
    void setColorPopList(const QVector<QColor>& inList);
    void setSelectedSample(long long selectedPointi);
    void setSearchedSample(long long searchedPointi);
    const QVector<QColor>& getColorPopList() const;
    float getPointSize();
    void paintPlot();
signals:
    void pointSelected(long long selectedPointi);
private:
    void initModelMat();
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
    void setPointSize(float val);
};

#endif // PCA2DSCATTERPLOTWIDGET_HPP

#ifndef PCA3DSCATTERPLOTWIDGET_HPP
#define PCA3DSCATTERPLOTWIDGET_HPP

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>
#include <QSvgGenerator>
#include <QVector>
#include "DataManager/DataManager.hpp"
#include "FileDeal/FileDeal.hpp"
#include "GenomeAnnotation/BaseOpenGLPaintWidget.hpp"
#include "PCA3DScatterFramePainter.hpp"
#include "PCA3DScatterPointsPainter.hpp"
#include "PCAScatterLegendPainter.hpp"
#include "FileDeal/BaseFileSaveDialog.hpp"

enum PCA3DLeftMouseMode
{
    PCA_3D_LEFT_DEFAULT = 0,
    PCA_3D_LEFT_LEGEND = 1,
    PCA_3D_LEFT_MAIN = 2,
};

class PCA3DScatterPlotWidget : public BaseOpenGLPaintWidget
{
    Q_OBJECT
private:
    bool isOrthoProjection;

    int samplePCValueDisplayWidth;
    int samplePCValueDisplayHeight;

    QMatrix4x4 modelMat;      // model transform
    QMatrix4x4 projectionMat; // projection transform
    QMatrix4x4 viewMat;       // view transform

    double zoomVal;           // (only ortho projection)
    double r;                 // >= rMin (only perspective projection)
    double theta;             // [0,pi]
    double alpha;             // [0,2*pi]
    double moveTick;          // pi/12
    double goFrontTick;
    double zoomTick;
    double rMax;
    double rMin;
    double zoomMax;
    double zoomMin;
    double thetaMax;
    double thetaMin;
    QVector3D  cameraPos;

    bool isMiddleMousePressed;
    bool isLeftMousePressed;

    int middleMousePressedPosX;
    int middleMousePressedPosY;
    int leftMousePressedPosX;
    int leftMousePressedPosY;

    QPointF oldLegendPoint;

    PCA3DLeftMouseMode leftMousePressedMode;

    PCA3DScatterFramePainter*  pFramePainter;
    PCA3DScatterPointsPainter* pPointsPainter;
    PCAScatterLegendPainter*   pLegendPainter;

    explicit PCA3DScatterPlotWidget(QWidget *parent = nullptr);
    void leftMousePressEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void middleMousePressEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void leftMouseReleaseEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void middleMouseReleaseEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void leftMouseMoveEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void middleMouseMoveEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void wheelEvent(QWheelEvent* e) override;

public:
    PCA3DScatterPlotWidget(const double* inData,
                           const long long* inPopIndex,
                           char** inSampleName,
                           long long inNSample,
                           QWidget *parent = nullptr);
    ~PCA3DScatterPlotWidget();

    bool setSampleData(const double* inData,
                       const long long* inPopIndex,
                       char** inSampleName,
                       long long inNSample);
    bool setPopName(char** inPopName,long long inNPop);
    bool setVarRatio(double inPC1VarRatio,
                     double inPC2VarRatio,
                     double inPC3VarRatio);
    void setDisplayLegend(bool flag);
    void setDisplayGridLine(bool flag);
    void setDisplayAxisValue(bool flag);
    void setDisplayAxisFrame(bool flag);
    void setDisplayPCLabel(bool flag);
    void setColorPopList(const QVector<QColor>& inList);
    void setSearchedSample(long long searchedPointi);
    void setSelectedSample(long long selectedPointi);
    void setOrthoProjection(bool flag);
    void switchProjection();

    float getPointSize();
    const QVector<QColor>& getColorPopList() const;
signals:
    void pointSelected(long long selectedPointi);
protected:
    void initModelMat();
    void initProjectionMat();
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

#endif // PCA3DSCATTERPLOTWIDGET_HPP

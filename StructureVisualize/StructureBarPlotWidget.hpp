#ifndef STRUCTUREBARPLOTWIDGET_HPP
#define STRUCTUREBARPLOTWIDGET_HPP

#include "DataManager/MatrixLib.hpp"
#include "FileDeal/FileDeal.hpp"
#include "StructureBarPainter.hpp"
#include "GenomeAnnotation/BaseOpenGLPaintWidget.hpp"
#include "StructureFramePainter.hpp"
#include "FileDeal/BaseFileSaveDialog.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QBrush>
#include <QVector>
#include <QDebug>


class StructureBarPlotWidget : public BaseOpenGLPaintWidget
{
    Q_OBJECT
private:

    bool               isMiddleMousePressed;
    float              middleMousePressedPosX;
    float              middleMousePressedPosY;
    QSize              paintFrameSize;

    float              xCamera;
    float              yCamera;
    float              minXCamera;
    float              maxXCamera;
    float              minYCamera;
    float              maxYCamera;
    float              zoomValues;
    float              zoomTick;
    float              maxZoomValue;
    float              minZoomValue;

    float              nameRotateAngle;

    QMatrix4x4         viewMat;
    QMatrix4x4         displayRotateMat;

    StructureBarPainter* pBarPainter;
    StructureFramePainter* pFramePainter;
protected:
    void leftMousePressEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void middleMousePressEvent(QMouseEvent* e,bool &isNeedUpdate) override;
    void middleMouseReleaseEvent(QMouseEvent* e,bool &isNeedUpdate) override;
public:
    explicit StructureBarPlotWidget(QWidget *parent = nullptr);
    StructureBarPlotWidget(const MML::Mat &inData,
                           char** inName,
                           QWidget *parent = nullptr);
    ~StructureBarPlotWidget();
    void mouseMoveEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void sortPaintSeq();
    void setIsDisplayMainFrame(bool flag);
    void setIsDisplayBarFrame(bool flag);
    void setBarGapFactor(double factor);
    void setXBarMarginFactor(double factor);
    void setYBarMarginFactor(double factor);
    void changeOrientation();
    void setSearchedSample(long long idPaint);
    void setSelectedSample(long long idPaint);
    void setColorPopList(const QVector<QColor>& inList);
    void setPaintFrameSize(QSize size);
    const QVector<QColor>& getColorPopList() const;
signals:
    void sampleSelected(long long selectedPointi);

private:
    void setVertical();
    void setHorizontal();
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

#endif // STRUCTUREBARPLOTWIDGET_HPP

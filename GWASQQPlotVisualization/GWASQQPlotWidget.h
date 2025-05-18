#ifndef GWASQQPLOTWIDGET_H
#define GWASQQPLOTWIDGET_H

#include <QWidget>
#include <QColor>
#include "GenomeAnnotation/BaseOpenGLPaintWidget.hpp"
#include "FileDeal/BaseFileSaveDialog.hpp"
#include "GWASQQPlotPointsPainter.h"
#include "GWASQQPlotFramePainter.h"

class GWASQQPlotWidget : public BaseOpenGLPaintWidget
{
    Q_OBJECT
private:
    GWASQQPlotPointsPainter*   pPointsPainter;
    GWASQQPlotFramePainter*    pFramePainter;
    ChromosomeInfoClass*       pChrInfo;
public:
    explicit GWASQQPlotWidget(QWidget *parent = nullptr);
    ~GWASQQPlotWidget();
    bool loadChromosome(const std::vector<long long>& inChrLen,
                        const std::vector<std::string>& inChrName);
    bool loadPoints(ValPoints *inPoints,long long inNPoints);

    double getMaxVal() const{return pPointsPainter -> getMaxVal();}
    double getMinVal() const{return pPointsPainter -> getMinVal();}
    const QColor& getPointsColor() const;

    void setPointsColor(const QColor& col);
    void setDiagonalLineColor(const QColor& col);
    bool setAxisValue(double xMax, double yMax, bool isReset = false);

    const QColor& getDiagonalLineColor() const;

    void savePlot(FD::SaveFileInfo inInfo);
private:
    void savePaintCurrentSVG(QString savePath,
                             QSizeF inSize);
    void savePaintCurrentPNG(QString savePath,
                             QSizeF inSize);
protected:
    void leftMousePressEvent(QMouseEvent* e,bool &isNeedUpdate) override;
private:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

signals:
};

#endif // GWASQQPLOTWIDGET_H

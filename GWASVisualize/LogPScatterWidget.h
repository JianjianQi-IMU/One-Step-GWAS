#ifndef LOGPSCATTERWIDGET_H
#define LOGPSCATTERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QtSvg/QSvgGenerator>
#include <QFileDialog>
#include <QVector>
#include <algorithm>
#include <cmath>

#include "GenomeAnnotation/GenomeAnnotation.hpp"
#include "FileDeal/FileDeal.hpp"
#include "DataManager/ValPoints.hpp"
#include "GenomeAnnotation/GenomeAnnoVisualizeWidget.hpp"
#include "LogPScatterViewPainter.hpp"
#include "GWASThresholdPainter.hpp"
#include "FileDeal/BaseFileSaveDialog.hpp"

#include <QDebug>

class LogPScatterVisualizeWidget : public GenomeAnnoVisualizeWidget
{
    Q_OBJECT
private:
    explicit LogPScatterVisualizeWidget(QWidget *parent = nullptr);

protected:
    float                           scatterAreaHeightRatio; // scatterAreaHeightRatio=scatterAreaHeight/(width()-widgetTopMargin)

    long long                       idRightPressPoint;

    QAction*                        actAddLabelPoint;
    QAction*                        actremoveLabelPoint;

    LogPScatterViewPainter*         pPointsPainter;
    GWASThresholdPainter*           pThresholdPainter;
public:
    LogPScatterVisualizeWidget(long long *inChrLen, char** inChrName, int inNChr, ValPoints *inPoints, long long inNPoints,
        QWidget *parent = nullptr);
    LogPScatterVisualizeWidget(const std::vector<long long>& inChrLen, const std::vector<std::string>& inChrName,
        ValPoints *inPoints, long long inNPoints, QWidget *parent = nullptr);
    virtual ~LogPScatterVisualizeWidget();
    bool loadPoints(ValPoints *inPoints, long long inNPoints);
    int getMaxLabelPointsNum() const;
    ValPoints* getPoints();
    long long getNPoints() const;
    double getMaxLogP() const;
    double getMinLogP() const;
    const QVector<QColor>& getPointsColor() const;
    bool createGenome();

    void dealAddPointLabel();
    void dealRemovePointLabel();

    void getThresholdLine(float& val, LineStyleParam& lineStyle) const;
    float getPointSize();

    void paintCurrent(QPainter *painter);
    void setPointsColor(const QVector<QColor>& inColor);

    void getTotalGPUMemory(GLint& valKb);
    void getCurrentGPUMemory(GLint& valKb);

    void doRemoveAllLabelPoint();
    void doRemoveLabelPoint(int row);
    void doLookLabelPoint(int row);

    void calcuNUpThreshold();
    void setThresholdLine(float val, LineStyleParam lineStyle);

    void setDisplayMovingPosLine(bool flag) override;
    void setDisplayMovingLogPLine(bool flag);
protected:
    void leftMousePressEvent(QMouseEvent* e, bool& isNeedUpdate) override;
    void rightMousePressEvent(QMouseEvent* e, bool& isNeedUpdate) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void makeMenu();
private:
    void initializeGL() override; // basic
    void paintGL() override; // basic
    void resizeGL(int w, int h) override; // basic

    void savePaintCurrentSVG(QString savePath, QSizeF inSize,
        long double inLeftPosF, long double inRightPosF);
    void savePaintCurrentPNG(QString savePath, QSizeF inSize,
        long double inLeftPosF, long double inRightPosF);
signals:
    void pointSelected(long long id, long long iGene1, long long iGene2);
    void addLabelPoint(long long idPoint);
    void removeLabelPoint(int row);
public slots:
    void menuExec(const QPoint& pos);
    void savePlot(FD::SaveFileInfo inInfo);
    void setPointSize(float val);
};

#endif // LOGPSCATTERWIDGET_H

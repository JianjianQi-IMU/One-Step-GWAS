#ifndef GWASQQVISUALIZATIONWIDGET_H
#define GWASQQVISUALIZATIONWIDGET_H

#include <QMainWindow>
#include <QTabWidget>
#include "GWASQQPlotWidget.h"
#include "GWASQQMainInfoWidget.h"

#include <QDebug>

namespace Ui {
class GWASQQVisualizationWidget;
}

class GWASQQVisualizationWidget : public QMainWindow
{
    Q_OBJECT

private:
    GWASQQPlotWidget* pPlotWidget;
    QTabWidget* pLeftTabWidget;
    GWASQQMainInfoWidget* pMainInfoWidget;
public:
    explicit GWASQQVisualizationWidget(QWidget *parent = nullptr);
    ~GWASQQVisualizationWidget();
    bool loadChromosome(const std::vector<long long>& inChrLen,
                        const std::vector<std::string>& inChrName);
    bool loadPoints(ValPoints *inPoints,long long inNPoints);
private slots:
    void on_actionExportPlot_triggered();

    void on_actionSetPointsColor_triggered();

    void on_actionSetDiagonalLineColor_triggered();

    void on_actionSetAxis_triggered();

private:
    Ui::GWASQQVisualizationWidget *ui;
};

#endif // GWASQQVISUALIZATIONWIDGET_H

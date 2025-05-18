#ifndef PCA3DVisualizationWidget_HPP
#define PCA3DVisualizationWidget_HPP

#include <QMainWindow>
#include <QVBoxLayout>
#include <QSplitter>
#include "PCA3DScatterPlotWidget.hpp"
#include "StartVisualization/ChooseColorListWidget.hpp"
#include "StartVisualization/IndividualSearchWidget.hpp"
#include "PCAVisualize/PCAMainInfoWidget.hpp"

namespace Ui {
class PCA3DVisualizationWidget;
}

class PCA3DVisualizationWidget : public QMainWindow
{
    Q_OBJECT
private:
    IndividualSearchWidget* pSearch;
    PCAMainInfoWidget*      pMainInfo;
    explicit PCA3DVisualizationWidget(QWidget *parent = nullptr);
    PCA3DScatterPlotWidget* plotWidget;
public:

    PCA3DVisualizationWidget(const double* inData,
                           const long long* inPopIndex,
                           char** inSampleName,
                           long long inNSample,
                           QWidget *parent = nullptr);
    ~PCA3DVisualizationWidget();
    bool isValid();
    bool setPopName(char** inPopName,long long inNPop);
    bool setVarRatio(double inPC1VarRatio,
                     double inPC2VarRatio,
                     double inPC3VarRatio);
private slots:
    void on_actionDisplayGridLine_triggered(bool checked);

    void on_actionDisplayAxisFrame_triggered(bool checked);

    void on_actionDisplayAxisValue_triggered(bool checked);

    void on_actionDisplayPCLabel_triggered(bool checked);

    void on_actionDisplayLegend_triggered(bool checked);

    void on_actionSelectColor_triggered();

    void on_actionSaveSVG_triggered();

    void on_actionSwitchProjection_triggered();

    void on_actionDisplayPCValue_triggered(bool checked);

    void on_actionSetPointSize_triggered();

private:
    Ui::PCA3DVisualizationWidget *ui;
};

#endif // PCA3DVisualizationWidget_HPP

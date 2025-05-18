#ifndef PCA2DVISUALIZATIONWIDGET_HPP
#define PCA2DVISUALIZATIONWIDGET_HPP

#include <QMainWindow>
#include <QSplitter>
#include <QTabWidget>
#include <QVBoxLayout>
#include "PCA2DScatterPlotWidget.hpp"
#include "PCAVisualize/PCAMainInfoWidget.hpp"
#include "StartVisualization/ChooseColorListWidget.hpp"
#include "StartVisualization/IndividualSearchWidget.hpp"


namespace Ui {
class PCA2DVisualizationWidget;
}

class PCA2DVisualizationWidget : public QMainWindow
{
    Q_OBJECT
private:
    PCAMainInfoWidget*      pMainInfo;
    IndividualSearchWidget* pSearch;
    explicit PCA2DVisualizationWidget(QWidget *parent = nullptr);
    PCA2DScatterPlotWidget* plotWidget;
public:
    PCA2DVisualizationWidget(const double* inData,
                             const long long* inPopIndex,
                             char** inSampleName,
                             long long inNSample,
                             QWidget *parent = nullptr);
    ~PCA2DVisualizationWidget();
    bool isValid();
    bool setPopName(char** inPopName,long long inNPop);
    bool setVarRatio(double inPC1VarRatio,double inPC2VarRatio);
private slots:

    void on_actionDisplayGrid_triggered(bool checked);

    void on_actionDisplayAxisValue_triggered(bool checked);

    void on_actionDisplayPC_triggered(bool checked);

    void on_actionDisplayLabel_triggered(bool checked);

    void on_actionSelectColor_triggered();

    void on_actionSaveSVG_triggered();

    void on_actionDisplayPCValue_triggered(bool checked);

    void on_actionSetPointSize_triggered();

private:
    Ui::PCA2DVisualizationWidget *ui;
};

#endif // PCA2DVISUALIZATIONWIDGET_HPP

#ifndef STRUCTUREVISUALIZEWIDGET_HPP
#define STRUCTUREVISUALIZEWIDGET_HPP

#include <QMainWindow>
#include <QSplitter>
#include <QTabWidget>
#include "StructureVisualize/StructureBarPlotWidget.hpp"
#include "StartVisualization/ChooseColorListWidget.hpp"
#include "StructureVisualize/StrucVisualSettingsWidget.hpp"
#include "StructureVisualize/StructureMainInfoWidget.hpp"
#include "StartVisualization/IndividualSearchWidget.hpp"


namespace Ui {
class StructureVisualizeWidget;
}

class StructureVisualizeWidget : public QMainWindow
{
    Q_OBJECT
private:
    StructureMainInfoWidget*   pMainInfo;
    StrucVisualSettingsWidget* pSettingsWidget;
    StructureBarPlotWidget*    plotWidget;
    IndividualSearchWidget*    pSearch;
    explicit StructureVisualizeWidget(QWidget *parent = nullptr);
public:
    StructureVisualizeWidget(const MML::Mat &inData,
                             char** inName,
                             QWidget *parent = nullptr);

    ~StructureVisualizeWidget();

private slots:


    void on_actionSetDisplayMainFrame_triggered(bool checked);

    void on_actionSetDisplayBarMargin_triggered(bool checked);

    void on_actionChangeOrientation_triggered();

    void on_actionSetPopColor_triggered();

    void on_actionSort_triggered();

    void on_actionExportPlot_triggered();

private:
    Ui::StructureVisualizeWidget *ui;
};

#endif // STRUCTUREVISUALIZEWIDGET_HPP

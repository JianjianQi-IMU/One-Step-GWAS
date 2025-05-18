#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QQueue>
#include <QMainWindow>
#include <QFontDatabase>
#include <QSplitter>
#include <QTreeWidget>
#include <QStringListModel>
#include <AnalyzeListView/AnalyzeListView.h>
#include <AnalyzeInfoWidget/AnalyzeInfoWidget.h>
#include <DataInteractTable/DataInteractTableWidget.hpp>
#include <GWASVisualize/GWASVisualize.hpp>
#include "StartVisualization/StartGWASVisualizationWidget.hpp"
#include "StartVisualization/StartPCAVisualizationWidget.hpp"
#include "StartVisualization/StartStrucVisualizationWidget.hpp"
#include "StartVisualization/StartKinVisualizationWidget.hpp"
#include "StartVisualization/StartGWASQQVisualizationWidget.h"
#include "PCAVisualize/PCA2DScatterPlotWidget.hpp"
#include "ProjectTreeView/ProjectTreeView.hpp"
#include "PCAAlgo/FastPCA.hpp"
#include "FileDeal/ProjectFilesRecord.hpp"

#include <Debug/debug.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    void readHistoryProjects();
    void saveHistoryProjects();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void dealwithToolbarButton(runState state);

    void on_actionNewGWASProject_triggered();

    void on_actionRemove_triggered();

    void on_actionEMMAX_triggered();

    void on_actionGEMMA_triggered();

    void on_actionGLM_triggered();

    void on_actiontest_triggered();

    void on_actiontest2_triggered();

    void on_actionFastPCA_triggered();

    void on_actionPCA_triggered();

    void on_actionStructure_triggered();

    void on_actiontest3_triggered();

    void on_actionKinship_triggered();

    void on_actionGWASVisualization_triggered();

    void on_actionPCAVisualization_triggered();

    void on_actionStructureVisualization_triggered();

    void on_actionKinshipVisualization_triggered();

    void on_actionRemoveAll_triggered();

    void on_actionLoad_triggered();

    void on_actionSave_triggered();

    void on_actionVisualize_triggered();

    void on_actionPause_triggered();

    void on_actionStop_triggered();

    void on_actionFastStructure_triggered();

    void on_actionNormalBSA_triggered();

    void on_actionGWASQQVisualization_triggered();

private:
    QQueue<FD::BaseProjectFilesSet*> historyFileSet;
    QQueue<QAction*> historyActList;

    AnalyzeListView* pAnalyzeListView;
    AnalyzeInfoWidget* pAnalyzeInfoBrowser;
    Ui::MainWindow *ui;

public:
    static const QString historyProjectsFile;
};
#endif // MAINWINDOW_H

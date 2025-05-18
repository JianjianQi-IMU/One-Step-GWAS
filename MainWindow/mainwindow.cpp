#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>

#include "FileDeal/ViewFileFormatWidget.hpp"

const QString MainWindow::historyProjectsFile = "./.historyProjects";

void MainWindow::readHistoryProjects()
{
    int len = historyFileSet.size();
    for (int i = 0 ; i < len; ++i) {
        ui->menuFile->removeAction(historyActList[i]);
        delete historyActList[i];
        delete historyFileSet[i];
    }
    historyFileSet.clear();
    historyActList.clear();
    FD::ProjectFilesRecordReader reader;
    reader.getProjects(historyProjectsFile, historyFileSet);
    len = historyFileSet.size();
    for (int i = 0 ; i < len; ++i) {
        QAction* tAct = new QAction(QString::number(i + 1) + ". " + historyFileSet[i]->getName(), this);
        ui->menuFile->addAction(tAct);
        historyActList.push_back(tAct);
    }

}

void MainWindow::saveHistoryProjects()
{
    int len = historyFileSet.size();
    FD::ProjectFilesRecordWriter writer;
    writer.addProjects(historyFileSet);
    writer.write(historyProjectsFile);
    for (int i = 0; i < len; ++i) {
        ui->menuFile->removeAction(historyActList[i]);
        delete historyActList[i];
        delete historyFileSet[i];
    }
    historyFileSet.clear();
    historyActList.clear();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    QIcon mainIcon(":/icon/MainWindowsIcon.png");
    //setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowIcon(mainIcon);

    QFontDatabase::addApplicationFont(":/font/Hack-Regular.ttf"); //Hack

    ui->setupUi(this);
    pAnalyzeListView = new AnalyzeListView(this);
    pAnalyzeInfoBrowser = new AnalyzeInfoWidget(this);
    ProjectTreeView* pTreeWidget = new ProjectTreeView(this);
    pTreeWidget->loadProjectUintList(pAnalyzeListView->pUnitList);
    pAnalyzeInfoBrowser->setProject(pAnalyzeListView->pProjectList);
    QSplitter* centralSplitter = new QSplitter(this);
    QSplitter* rightSplitter = new QSplitter(this);
    centralSplitter->setOrientation(Qt::Horizontal);
    rightSplitter->setOrientation(Qt::Vertical);
    centralSplitter->addWidget(pTreeWidget);
    centralSplitter->addWidget(rightSplitter);
    rightSplitter->addWidget(pAnalyzeListView);
    rightSplitter->addWidget(pAnalyzeInfoBrowser);
    ui->centralwidget->layout()->addWidget(centralSplitter);
    ui->actionVisualize->setDisabled(true);
    ui->actionPause->setDisabled(true);
    ui->actionStop->setDisabled(true);

    connect(pAnalyzeListView, &AnalyzeListView::projectChanged, pTreeWidget, &ProjectTreeView::changeUpdate);
    connect(pAnalyzeListView, &AnalyzeListView::projectRemoved, pTreeWidget, &ProjectTreeView::removeUpdate);
    connect(pAnalyzeListView, &AnalyzeListView::projectInserted, pTreeWidget, &ProjectTreeView::insertUpdate);
    connect(pAnalyzeListView, &AnalyzeListView::allRemoved, pTreeWidget, &ProjectTreeView::removeAllUpdate);
    connect(pAnalyzeListView, &AnalyzeListView::projectSelected, pTreeWidget, &ProjectTreeView::selectProject);
    connect(pAnalyzeListView, &AnalyzeListView::projectSelectedState, this, &MainWindow::dealwithToolbarButton);

    connect(pTreeWidget, &ProjectTreeView::sendAddNewProject, pAnalyzeListView, &AnalyzeListView::addNew);
    connect(pTreeWidget, &ProjectTreeView::sendRemoveProject, pAnalyzeListView, &AnalyzeListView::removeOne);
    connect(pTreeWidget, &ProjectTreeView::sendLoadProject, pAnalyzeListView, &AnalyzeListView::loadFileAt);
    connect(pTreeWidget, &ProjectTreeView::sendSelectProject, pAnalyzeListView, &AnalyzeListView::selectProject);
    connect(pTreeWidget, &ProjectTreeView::sendSelectProjectState, this, &MainWindow::dealwithToolbarButton);

    connect(pAnalyzeListView, &AnalyzeListView::projectRemoved, pAnalyzeInfoBrowser, &AnalyzeInfoWidget::removeOne);
    connect(pAnalyzeListView, &AnalyzeListView::projectLogUpdate, pAnalyzeInfoBrowser, &AnalyzeInfoWidget::updateText);
    connect(pAnalyzeListView, &AnalyzeListView::projectAdd, pAnalyzeInfoBrowser, &AnalyzeInfoWidget::addNew);
    connect(pAnalyzeListView, &AnalyzeListView::allRemoved, pAnalyzeInfoBrowser, &AnalyzeInfoWidget::removeAll);

    readHistoryProjects();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dealwithToolbarButton(runState state)
{
    switch (state) {
    case RUN_UNKNOWN:
    case RUN_DEFAULT:
    case RUN_UNHANDLED:
        ui->actionVisualize->setDisabled(true);
        ui->actionPause->setDisabled(true);
        ui->actionStop->setDisabled(true);
        break;
    case RUN_PAUSE:
        ui->actionVisualize->setDisabled(true);
        ui->actionPause->setEnabled(true);
        ui->actionPause->setIcon(QIcon(":/icon/continueIcon.png"));
        ui->actionPause->setToolTip("continue");
        ui->actionStop->setEnabled(true);
        break;
    case RUN_RUNNING:
        ui->actionVisualize->setDisabled(true);
        ui->actionPause->setEnabled(true);
        ui->actionPause->setIcon(QIcon(":/icon/pauseIcon.png"));
        ui->actionPause->setToolTip("pause");
        ui->actionStop->setEnabled(true);
        break;
    case RUN_FINISHED:
        ui->actionVisualize->setEnabled(true);
        ui->actionPause->setDisabled(true);
        ui->actionStop->setDisabled(true);
        break;
    default:
        break;
    }
}

void MainWindow::on_actionNewGWASProject_triggered()
{
    pAnalyzeListView->addNew();
}

void MainWindow::on_actionRemove_triggered()
{
    pAnalyzeListView->removeCurrent();
}

void MainWindow::on_actionRemoveAll_triggered()
{
    pAnalyzeListView->removeAll();
}

void MainWindow::on_actionLoad_triggered()
{
    pAnalyzeListView->loadFile();
}

void MainWindow::on_actionSave_triggered()
{
    pAnalyzeListView->saveCurrent();
}

void MainWindow::on_actionVisualize_triggered()
{
    pAnalyzeListView->startVisualization();
}

void MainWindow::on_actionPause_triggered()
{
    pAnalyzeListView->pauseOrContinueSelectedProject();
}

void MainWindow::on_actionStop_triggered()
{
    pAnalyzeListView->stopCurrent();
}

void MainWindow::on_actionEMMAX_triggered()
{
    pAnalyzeListView->runEMMAX();
}

void MainWindow::on_actionGEMMA_triggered()
{
    pAnalyzeListView->runGEMMA();
}

void MainWindow::on_actionGLM_triggered()
{
    pAnalyzeListView->runGLM();
}

void MainWindow::on_actionPCA_triggered()
{
    pAnalyzeListView->runPCA();
}

void MainWindow::on_actionFastPCA_triggered()
{
    pAnalyzeListView->runFastPCA();
}

void MainWindow::on_actionStructure_triggered()
{
    pAnalyzeListView->runStructure();
}

void MainWindow::on_actionFastStructure_triggered()
{
    pAnalyzeListView->runFastStructure();
}

void MainWindow::on_actionKinship_triggered()
{
    pAnalyzeListView->runKinship();
}

void MainWindow::on_actionNormalBSA_triggered()
{
    pAnalyzeListView->runNormalBSA();
}

void MainWindow::on_actionGWASVisualization_triggered()
{
    StartGWASVisualizationWidget* pWidget = new StartGWASVisualizationWidget;
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
}

void MainWindow::on_actionGWASQQVisualization_triggered()
{
    StartGWASQQVisualizationWidget* pWidget = new StartGWASQQVisualizationWidget;
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
}

void MainWindow::on_actionPCAVisualization_triggered()
{
    StartPCAVisualizationWidget* pWidget = new StartPCAVisualizationWidget;
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
}

void MainWindow::on_actionStructureVisualization_triggered()
{
    StartStrucVisualizationWidget* pWidget = new StartStrucVisualizationWidget;
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
}

void MainWindow::on_actionKinshipVisualization_triggered()
{
    StartKinVisualizationWidget* pWidget = new StartKinVisualizationWidget;
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
}

void MainWindow::on_actiontest_triggered()
{
    QQueue<FD::BaseProjectFilesSet*> tQueue;
    FD::ProjectFilesRecordWriter writer;
    FD::GWASProjectFilesSet* set1 = new FD::GWASProjectFilesSet;
    FD::GWASProjectFilesSet* set2 = new FD::GWASProjectFilesSet;
    set1->setGenoFile("set1GenoFile");
    set1->setBimFile("set1BimFile");
    set1->setPheFile("set1PheFile");
    set1->setCovFile("set1CovFile");
    set1->setKinFile("set1KinFile");
    set1->setGenoFileForm(FD::FILEFORM_BED);
    set2->setGenoFile("set2GenoFile");
    set2->setBimFile("set2BimFile");
    set2->setPheFile("set2PheFile");
    set2->setCovFile("set2CovFile");
    set2->setKinFile("set2KinFile");
    set2->setGenoFileForm(FD::FILEFORM_PED);
    tQueue.push_back(set1);
    tQueue.push_back(set2);
    writer.addProjects(tQueue);
    writer.write(historyProjectsFile);
    //writer.write("C:\\Users\\minipluto\\Desktop\\historyFile");
    foreach (FD::BaseProjectFilesSet* var, tQueue) {
        delete var;
    }
    tQueue.clear();
}

void MainWindow::on_actiontest2_triggered()
{
    MML::Distribution dis;
    // for(double x=50;x<501;x+=50){
    //     double y=1*x/(1*x+1500);
    //    qDebug() << dis.ibeta(0.5,750,y);
    //    qDebug() << dis.lbeta(0.5,750);
    // }
    // qDebug() << dis.ibeta(2.5,1.5,0.9);
    // qDebug() << dis.ibeta(100,75,0.02);
    // qDebug() << dis.ibeta(120,0.2,0.02);
    // qDebug() << dis.ibeta(202,13,0.9);
    // qDebug() << dis.ibeta(2,0.1,0.1);
    // qDebug() << dis.ibeta(0.5,1000,0.2);
    qDebug() << dis.fcdfTri(6.313726e+01,1,1402);
    qDebug() << dis.fcdfTri(4.069875e+02,1,1402);

    // qDebug() << dis.igammap(12,100);
    // qDebug() << dis.igammap(0.1,2);
    // qDebug() << dis.igammap(12,2);
    // qDebug() << dis.igammap(0.1,0.3);
}

void MainWindow::on_actiontest3_triggered()
{
    ViewFileFormatTable *pWidget = new ViewFileFormatTable();
    QVector<FD::FileFormClass> vec {FD::FILEFORM_VCF, FD::FILEFORM_BED};
    pWidget->load(vec);
    pWidget->show();
}



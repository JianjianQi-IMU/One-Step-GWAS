#include "PCA2DVisualizationWidget.hpp"
#include "ui_PCA2DVisualizationWidget.h"

#include "StyleChoose/SetPointSizeWidget.h"

PCA2DVisualizationWidget::PCA2DVisualizationWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PCA2DVisualizationWidget)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle("PCA visualization");

    plotWidget = nullptr;
    pSearch = nullptr;
    pMainInfo = nullptr;
    ui->setupUi(this);
}

PCA2DVisualizationWidget::PCA2DVisualizationWidget(const double *inData, const long long *inPopIndex,char** inSampleName, long long inNSample, QWidget *parent)
    :PCA2DVisualizationWidget(parent)
{
    plotWidget = new PCA2DScatterPlotWidget(inData,inPopIndex,inSampleName,inNSample,this);
    pSearch = new IndividualSearchWidget(this);
    pMainInfo = new PCAMainInfoWidget(this);
    QTabWidget* pTab = new QTabWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget());
    QSplitter* mainSplitter = new QSplitter(this);
    mainLayout->addWidget(mainSplitter);
    mainSplitter->setOrientation(Qt::Horizontal);

    pMainInfo->setNSample(QString::number(inNSample));
    pMainInfo->setNPop("(NULL)");
    pMainInfo->setPC1("(NULL)");
    pMainInfo->setPC2("(NULL)");
    pMainInfo->setPC3("(NULL)");
    pSearch->loadNameList(inSampleName,inNSample);

    pTab->addTab(pMainInfo,"Main");
    pTab->addTab(pSearch,"Search");

    mainSplitter->addWidget(pTab);
    mainSplitter->addWidget(plotWidget);
    connect(pSearch,&IndividualSearchWidget::sendSelectedChanged,plotWidget,&PCA2DScatterPlotWidget::setSearchedSample);
}

PCA2DVisualizationWidget::~PCA2DVisualizationWidget()
{
    delete ui;
}

bool PCA2DVisualizationWidget::isValid()
{
    if(!plotWidget) return false;
    return plotWidget->isValid();
}

bool PCA2DVisualizationWidget::setPopName(char **inPopName, long long inNPop)
{
    if(!inPopName||!inNPop) return false;
    pMainInfo->setNPop(QString::number(inNPop));
    return plotWidget->setPopName(inPopName,inNPop);
}

bool PCA2DVisualizationWidget::setVarRatio(double inPC1VarRatio, double inPC2VarRatio)
{
    pMainInfo->setPC1(QString::number(100*inPC1VarRatio)+"%");
    pMainInfo->setPC2(QString::number(100*inPC2VarRatio)+"%");
    return plotWidget->setVarRatio(inPC1VarRatio,inPC2VarRatio);
}


void PCA2DVisualizationWidget::on_actionDisplayGrid_triggered(bool checked)
{
    plotWidget->setDisplayGridLine(checked);
}


void PCA2DVisualizationWidget::on_actionDisplayAxisValue_triggered(bool checked)
{
    plotWidget->setDisplayAxisValue(checked);
}


void PCA2DVisualizationWidget::on_actionDisplayPC_triggered(bool checked)
{
    plotWidget->setDisplayPCLabel(checked);
}


void PCA2DVisualizationWidget::on_actionDisplayLabel_triggered(bool checked)
{
    plotWidget->setDisplayLegend(checked);
}


void PCA2DVisualizationWidget::on_actionSelectColor_triggered()
{
    ChooseColorListWidget* pWidget=new ChooseColorListWidget(plotWidget->getColorPopList());
    connect(pWidget,&ChooseColorListWidget::sendColor,plotWidget,&PCA2DScatterPlotWidget::setColorPopList);
    pWidget->show();
}


void PCA2DVisualizationWidget::on_actionSaveSVG_triggered()
{
    FD::PlotFileSaveDialog* pWidget = new FD::PlotFileSaveDialog;
    pWidget -> setAttribute(Qt::WA_ShowModal, true);
    pWidget -> setSize(plotWidget -> size());
    connect(pWidget,&FD::PlotFileSaveDialog::sendSaveFile,
            plotWidget,&PCA2DScatterPlotWidget::savePlot);
    pWidget -> show();
}


void PCA2DVisualizationWidget::on_actionDisplayPCValue_triggered(bool checked)
{
    // plotWidget->setDisplaySamplePCValue(checked);
}


void PCA2DVisualizationWidget::on_actionSetPointSize_triggered()
{
    SetPointSizeWidget* pWidget = new SetPointSizeWidget(plotWidget->getPointSize());
    connect(pWidget, &SetPointSizeWidget::sendPointSize,
            plotWidget, &PCA2DScatterPlotWidget::setPointSize);
    pWidget -> show();
}


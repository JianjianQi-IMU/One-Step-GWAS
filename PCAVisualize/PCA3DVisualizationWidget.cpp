#include "PCA3DVisualizationWidget.hpp"
#include "ui_PCA3DVisualizationWidget.h"

#include "StyleChoose/SetPointSizeWidget.h"

PCA3DVisualizationWidget::PCA3DVisualizationWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PCA3DVisualizationWidget)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    ui->setupUi(this);
    plotWidget = nullptr;
    pSearch = nullptr;
    pMainInfo = nullptr;
}

PCA3DVisualizationWidget::PCA3DVisualizationWidget(const double *inData, const long long *inPopIndex,char** inSampleName, long long inNSample, QWidget *parent)
    :PCA3DVisualizationWidget(parent)
{
    plotWidget = new PCA3DScatterPlotWidget(inData,inPopIndex,inSampleName,inNSample,this);
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

    connect(pSearch,&IndividualSearchWidget::sendSelectedChanged,plotWidget,&PCA3DScatterPlotWidget::setSearchedSample);
}

PCA3DVisualizationWidget::~PCA3DVisualizationWidget()
{
    delete ui;
}

bool PCA3DVisualizationWidget::isValid()
{
    if(!plotWidget) return false;
    return plotWidget->isValid();
}

bool PCA3DVisualizationWidget::setPopName(char **inPopName, long long inNPop)
{
    if(!inPopName||!inNPop) return false;
    pMainInfo->setNPop(QString::number(inNPop));
    return plotWidget->setPopName(inPopName,inNPop);
}

bool PCA3DVisualizationWidget::setVarRatio(double inPC1VarRatio, double inPC2VarRatio, double inPC3VarRatio)
{
    pMainInfo->setPC1(QString::number(100*inPC1VarRatio)+"%");
    pMainInfo->setPC2(QString::number(100*inPC2VarRatio)+"%");
    pMainInfo->setPC3(QString::number(100*inPC3VarRatio)+"%");
    return plotWidget->setVarRatio(inPC1VarRatio,inPC2VarRatio,inPC3VarRatio);
}

void PCA3DVisualizationWidget::on_actionDisplayGridLine_triggered(bool checked)
{
    plotWidget->setDisplayGridLine(checked);
}


void PCA3DVisualizationWidget::on_actionDisplayAxisFrame_triggered(bool checked)
{
    plotWidget->setDisplayAxisFrame(checked);
}


void PCA3DVisualizationWidget::on_actionDisplayAxisValue_triggered(bool checked)
{
    plotWidget->setDisplayAxisValue(checked);
}


void PCA3DVisualizationWidget::on_actionDisplayPCLabel_triggered(bool checked)
{
    plotWidget->setDisplayPCLabel(checked);
}


void PCA3DVisualizationWidget::on_actionDisplayLegend_triggered(bool checked)
{
    plotWidget->setDisplayLegend(checked);
}


void PCA3DVisualizationWidget::on_actionSelectColor_triggered()
{
    ChooseColorListWidget* pWidget=new ChooseColorListWidget(plotWidget->getColorPopList());
    connect(pWidget,&ChooseColorListWidget::sendColor,plotWidget,&PCA3DScatterPlotWidget::setColorPopList);
    pWidget->show();
}


void PCA3DVisualizationWidget::on_actionSaveSVG_triggered()
{
    FD::PlotFileSaveDialog* pWidget = new FD::PlotFileSaveDialog;
    pWidget -> setAttribute(Qt::WA_ShowModal, true);
    pWidget -> setSize(plotWidget -> size());
    connect(pWidget,&FD::PlotFileSaveDialog::sendSaveFile,
            plotWidget,&PCA3DScatterPlotWidget::savePlot);
    pWidget -> show();
}


void PCA3DVisualizationWidget::on_actionSwitchProjection_triggered()
{
    plotWidget->switchProjection();
}


void PCA3DVisualizationWidget::on_actionDisplayPCValue_triggered(bool checked)
{

}


void PCA3DVisualizationWidget::on_actionSetPointSize_triggered()
{
    SetPointSizeWidget* pWidget = new SetPointSizeWidget(plotWidget->getPointSize());
    connect(pWidget, &SetPointSizeWidget::sendPointSize,
            plotWidget, &PCA3DScatterPlotWidget::setPointSize);
    pWidget -> show();
}


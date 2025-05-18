#include "StructureVisualizeWidget.hpp"
#include "ui_StructureVisualizeWidget.h"

StructureVisualizeWidget::StructureVisualizeWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StructureVisualizeWidget)
{
    plotWidget = nullptr;
    pSettingsWidget = new StrucVisualSettingsWidget(this);
    pSearch = new IndividualSearchWidget(this);
    pMainInfo = new StructureMainInfoWidget(this);
    ui->setupUi(this);
}

StructureVisualizeWidget::StructureVisualizeWidget(const MML::Mat &inData,char **inName, QWidget *parent)
    :StructureVisualizeWidget(parent)
{
    plotWidget = new StructureBarPlotWidget(inData,inName);
    QTabWidget* pTab = new QTabWidget(this);
//    QWidget* mainWidget = new QWidget(this);
    QSplitter* mainSplitter=new QSplitter(this);
    mainSplitter->setOrientation(Qt::Horizontal);
    mainSplitter->addWidget(pTab);
    mainSplitter->addWidget(plotWidget);
    setCentralWidget(mainSplitter);

    pTab->addTab(pMainInfo,"Main");
    pTab->addTab(pSettingsWidget,"Setting");
    pTab->addTab(pSearch,"Find");

    pSearch->loadNameList(inName,inData.getNRow());

    pMainInfo->setNSamples(QString::number(inData.getNRow()));
    pMainInfo->setNPops(QString::number(inData.getNCol()));

    connect(pSearch,
            &IndividualSearchWidget::sendSelectedChanged,
            plotWidget,
            &StructureBarPlotWidget::setSearchedSample);
    connect(pSettingsWidget,
            &StrucVisualSettingsWidget::sendBarGapChanged,
            plotWidget,
            &StructureBarPlotWidget::setBarGapFactor);
    connect(pSettingsWidget,
            &StrucVisualSettingsWidget::sendXMarginChanged,
            plotWidget,
            &StructureBarPlotWidget::setXBarMarginFactor);
    connect(pSettingsWidget,
            &StrucVisualSettingsWidget::sendYMarginChanged,
            plotWidget,
            &StructureBarPlotWidget::setYBarMarginFactor);
    connect(pSettingsWidget,
            &StrucVisualSettingsWidget::sendSizeVal,
            plotWidget,
            &StructureBarPlotWidget::setPaintFrameSize);

    pSettingsWidget->setBarGap(0);
    pSettingsWidget->setXMargin(5);
    pSettingsWidget->setYMargin(20);
}

StructureVisualizeWidget::~StructureVisualizeWidget()
{
    delete ui;
}


void StructureVisualizeWidget::on_actionSetDisplayMainFrame_triggered(bool checked)
{
    plotWidget->setIsDisplayMainFrame(checked);
}


void StructureVisualizeWidget::on_actionSetDisplayBarMargin_triggered(bool checked)
{
    plotWidget->setIsDisplayBarFrame(checked);
}


void StructureVisualizeWidget::on_actionChangeOrientation_triggered()
{
    plotWidget->changeOrientation();
}


void StructureVisualizeWidget::on_actionSetPopColor_triggered()
{
    ChooseColorListWidget* pWidget=new ChooseColorListWidget(plotWidget->getColorPopList());
    connect(pWidget,&ChooseColorListWidget::sendColor,plotWidget,&StructureBarPlotWidget::setColorPopList);
    pWidget->show();
}


void StructureVisualizeWidget::on_actionSort_triggered()
{
    plotWidget->sortPaintSeq();
}


void StructureVisualizeWidget::on_actionExportPlot_triggered()
{
    FD::PlotFileSaveDialog* pWidget = new FD::PlotFileSaveDialog;
    pWidget -> setAttribute(Qt::WA_ShowModal, true);
    pWidget -> setSize(plotWidget -> size());
    connect(pWidget,&FD::PlotFileSaveDialog::sendSaveFile,
            plotWidget,&StructureBarPlotWidget::savePlot);
    pWidget -> show();
}


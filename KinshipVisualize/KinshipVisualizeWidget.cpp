#include "KinshipVisualizeWidget.hpp"
#include "ui_KinshipVisualizeWidget.h"


KinshipVisualizeWidget::KinshipVisualizeWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KinshipVisualizeWidget)
{
    setAttribute(Qt::WA_DeleteOnClose,true);

    ui->setupUi(this);
    pHeatmapPlot = nullptr;

    setWindowTitle("Kinship Heatmap");
}

KinshipVisualizeWidget::KinshipVisualizeWidget(const MML::Mat &inKin, char **inName, QWidget *parent)
    :KinshipVisualizeWidget(parent)
{
    pHeatmapPlot = new KinshipHeatmapWidget(inKin,inName,this);
    setCentralWidget(pHeatmapPlot);
}

KinshipVisualizeWidget::~KinshipVisualizeWidget()
{
    delete ui;
}

void KinshipVisualizeWidget::setClusterIndex()
{
    pHeatmapPlot->setClusterIndex();
}

void KinshipVisualizeWidget::setRangeValCol(KinshipRangeValColParam param)
{
    pHeatmapPlot -> setRangeValue(param.valueHigh, param.valueLow, param.valueMean);
    pHeatmapPlot -> setRangeColor(param.colorHigh, param.colorLow, param.colorMean);
    pHeatmapPlot -> update();
}

void KinshipVisualizeWidget::on_actionCluster_triggered()
{
    pHeatmapPlot->setClusterIndex();
}


void KinshipVisualizeWidget::on_actionSetRangeLabel_triggered()
{
    KinshipRangeValColSettingWidget* pWidget = new KinshipRangeValColSettingWidget;
    KinshipRangeValColParam param;
    pHeatmapPlot -> getRangeValue(param.valueHigh, param.valueLow, param.valueMean);
    pHeatmapPlot -> getRangeColor(param.colorHigh, param.colorLow, param.colorMean);
    pWidget -> setParam(param);
    connect(pWidget, &KinshipRangeValColSettingWidget::sendParam,
            this, &KinshipVisualizeWidget::setRangeValCol);
    pWidget -> show();
}


void KinshipVisualizeWidget::on_actionExportPlot_triggered()
{
    FD::PlotFileSaveDialog* pWidget = new FD::PlotFileSaveDialog;
    pWidget -> setAttribute(Qt::WA_ShowModal, true);
    pWidget -> setSize(pHeatmapPlot -> size());
    connect(pWidget,&FD::PlotFileSaveDialog::sendSaveFile,
            pHeatmapPlot,&KinshipHeatmapWidget::savePlot);
    pWidget -> show();
}


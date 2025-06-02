#include "GWASQQVisualizationWidget.h"
#include "ui_GWASQQVisualizationWidget.h"

#include "GWASQQColorSettings.h"
#include "GWASQQAxisSettingWidget.h"

#include <QVBoxLayout>
#include <QSplitter>

GWASQQVisualizationWidget::GWASQQVisualizationWidget(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::GWASQQVisualizationWidget)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Q-Q Plot");

    pPlotWidget = new GWASQQPlotWidget;

    pMainInfoWidget = new GWASQQMainInfoWidget(this);

    pLeftTabWidget = new QTabWidget(this);
    pLeftTabWidget->addTab(pMainInfoWidget, "Main");

    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    QSplitter* mainSplitter = new QSplitter(this);
    mainSplitter->setOrientation(Qt::Horizontal);
    mainSplitter->addWidget(pLeftTabWidget);
    mainSplitter->addWidget(pPlotWidget);
    mainLayout->addWidget(mainSplitter);

    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 6);
    setCentralWidget(mainWidget);

    resize(1200, 600);
}

GWASQQVisualizationWidget::~GWASQQVisualizationWidget()
{
    delete ui;
}

bool GWASQQVisualizationWidget::loadChromosome(const std::vector<long long> &inChrLen, const std::vector<std::string> &inChrName)
{
    if (pPlotWidget) {
        pPlotWidget -> loadChromosome(inChrLen, inChrName);
        return true;
    }
    return false;
}

bool GWASQQVisualizationWidget::loadPoints(ValPoints *inPoints, long long inNPoints)
{
    if (pPlotWidget) {
        pPlotWidget -> loadPoints(inPoints, inNPoints);
        pMainInfoWidget -> setMax(QString::number(pPlotWidget -> getMaxVal()));
        pMainInfoWidget -> setNumber(QString::number(inNPoints));
        return true;
    }
    return false;
}

void GWASQQVisualizationWidget::on_actionExportPlot_triggered()
{
    FD::PlotFileSaveDialog* pWidget = new FD::PlotFileSaveDialog;
    pWidget -> setAttribute(Qt::WA_ShowModal, true);
    pWidget -> setSize(pPlotWidget -> size());
    connect(pWidget, &FD::PlotFileSaveDialog::sendSaveFile, pPlotWidget, &GWASQQPlotWidget::savePlot);
    pWidget -> show();
}


void GWASQQVisualizationWidget::on_actionSetPointsColor_triggered()
{
    GWASQQPointsColorChooseWidget* pWidget = new GWASQQPointsColorChooseWidget;
    pWidget -> setChoosedColor(pPlotWidget -> getPointsColor());
    pWidget -> setPlotWidget(pPlotWidget);
    pWidget -> show();
}


void GWASQQVisualizationWidget::on_actionSetDiagonalLineColor_triggered()
{
    GWASQQDiagonalLineColorChooseWidget* pWidget = new GWASQQDiagonalLineColorChooseWidget;
    pWidget -> setChoosedColor(pPlotWidget -> getPointsColor());
    pWidget -> setPlotWidget(pPlotWidget);
    pWidget -> show();
}


void GWASQQVisualizationWidget::on_actionSetAxis_triggered()
{
    double maxVal = pPlotWidget -> getMaxVal();
    GWASQQAxisSettingWidget* pWidget = new GWASQQAxisSettingWidget(pPlotWidget);
    pWidget -> setLimits(maxVal, maxVal);
    connect(pWidget, &GWASQQAxisSettingWidget::sendAxisValue, pPlotWidget, &GWASQQPlotWidget::setAxisValue);
    pWidget -> show();
}


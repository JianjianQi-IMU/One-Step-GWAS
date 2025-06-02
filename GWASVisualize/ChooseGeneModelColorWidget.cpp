#include "ChooseGeneModelColorWidget.hpp"
#include "ui_ChooseGeneModelColorWidget.h"

ChooseGeneModelColorWidget::ChooseGeneModelColorWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::ChooseGeneModelColorWidget)
{
    ui->setupUi(this);
}

ChooseGeneModelColorWidget::ChooseGeneModelColorWidget(LogPScatterVisualizeWidget *inLogPChart, QWidget *parent)
    : ChooseGeneModelColorWidget(parent)
{
    pLogPChart = inLogPChart;
    UTR5Color = pLogPChart->get5UTRColor();
    UTR3Color = pLogPChart->get3UTRColor();
    CDSColor = pLogPChart->getCDSColor();
    QPalette tPalette;
    ui->CDSColorWidget->setAutoFillBackground(true);
    ui->UTR5ColorWidget->setAutoFillBackground(true);
    ui->UTR3ColorWidget->setAutoFillBackground(true);
    tPalette.setColor(QPalette::Background, UTR5Color);
    ui->UTR5ColorWidget->setPalette(tPalette);
    tPalette.setColor(QPalette::Background, UTR3Color);
    ui->UTR3ColorWidget->setPalette(tPalette);
    tPalette.setColor(QPalette::Background, CDSColor);
    ui->CDSColorWidget->setPalette(tPalette);
}

ChooseGeneModelColorWidget::~ChooseGeneModelColorWidget()
{
    delete ui;
}

void ChooseGeneModelColorWidget::on_CDSColorBtn_clicked()
{
    QColor color = QColorDialog::getColor(CDSColor,this);
    QPalette tPalette;
    if (color.isValid()) {
        CDSColor = color;
        tPalette.setColor(QPalette::Background, CDSColor);
        ui->CDSColorWidget->setPalette(tPalette);
    }
}


void ChooseGeneModelColorWidget::on_UTR5ColorBtn_clicked()
{
    QColor color = QColorDialog::getColor(UTR5Color, this);
    QPalette tPalette;
    if (color.isValid()) {
        UTR5Color = color;
        tPalette.setColor(QPalette::Background, UTR5Color);
        ui->UTR5ColorWidget->setPalette(tPalette);
    }
}


void ChooseGeneModelColorWidget::on_UTR3ColorBtn_clicked()
{
    QColor color = QColorDialog::getColor(UTR3Color, this);
    QPalette tPalette;
    if (color.isValid()) {
        UTR3Color = color;
        tPalette.setColor(QPalette::Background, UTR3Color);
        ui->UTR3ColorWidget->setPalette(tPalette);
    }
}


void ChooseGeneModelColorWidget::on_buttonBox_accepted()
{
    pLogPChart->set3UTRColor(UTR3Color, false);
    pLogPChart->set5UTRColor(UTR5Color, false);
    pLogPChart->setCDSColor(CDSColor);
    close();
}


void ChooseGeneModelColorWidget::on_buttonBox_rejected()
{
    close();
}


#include "ChooseGeneColorWidget.hpp"
#include "ui_ChooseGeneColorWidget.h"

ChooseGeneColorWidget::ChooseGeneColorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChooseGeneColorWidget)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    setAttribute(Qt::WA_ShowModal,true);
    ui->setupUi(this);
}

ChooseGeneColorWidget::ChooseGeneColorWidget(LogPScatterVisualizeWidget *inLogPChart, QWidget *parent)
    :ChooseGeneColorWidget(parent)
{
    pLogPChart=inLogPChart;
    forwardGeneColor=pLogPChart->getForwardGeneColor();
    reverseGeneColor=pLogPChart->getReverseGeneColor();
    geneColor=pLogPChart->getDefaultGeneColor();
    QPalette tPalette;
    ui->forwardColorWidget->setAutoFillBackground(true);
    ui->reverseColorWidget->setAutoFillBackground(true);
    ui->geneColorWidget->setAutoFillBackground(true);
    tPalette.setColor(QPalette::Background,forwardGeneColor);
    ui->forwardColorWidget->setPalette(tPalette);
    tPalette.setColor(QPalette::Background,reverseGeneColor);
    ui->reverseColorWidget->setPalette(tPalette);
    tPalette.setColor(QPalette::Background,geneColor);
    ui->geneColorWidget->setPalette(tPalette);
}

ChooseGeneColorWidget::~ChooseGeneColorWidget()
{
    delete ui;
}

void ChooseGeneColorWidget::on_forwardColorBtn_clicked()
{
    QColor color = QColorDialog::getColor(forwardGeneColor,this);
    QPalette tPalette;
    if(color.isValid()){
        forwardGeneColor=color;
        tPalette.setColor(QPalette::Background,forwardGeneColor);
        ui->forwardColorWidget->setPalette(tPalette);
    }
}


void ChooseGeneColorWidget::on_reverseColorBtn_clicked()
{
    QColor color = QColorDialog::getColor(reverseGeneColor,this);
    QPalette tPalette;
    if(color.isValid()){
        reverseGeneColor=color;
        tPalette.setColor(QPalette::Background,reverseGeneColor);
        ui->reverseColorWidget->setPalette(tPalette);
    }
}


void ChooseGeneColorWidget::on_buttonBox_accepted()
{
    pLogPChart->setForwardGeneColor(forwardGeneColor,false);
    pLogPChart->setReverseGeneColor(reverseGeneColor,false);
    pLogPChart->setDefaultGeneColor(geneColor);
    close();
}


void ChooseGeneColorWidget::on_buttonBox_rejected()
{
    close();
}


void ChooseGeneColorWidget::on_geneColorBtn_clicked()
{
    QColor color = QColorDialog::getColor(geneColor,this);
    QPalette tPalette;
    if(color.isValid()){
        geneColor=color;
        tPalette.setColor(QPalette::Background,geneColor);
        ui->geneColorWidget->setPalette(tPalette);
    }
}


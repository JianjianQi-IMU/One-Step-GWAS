#include "PointInfoWidget.hpp"
#include "ui_PointInfoWidget.h"

PointInfoWidget::PointInfoWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::PointInfoWidget)
{
    ui->setupUi(this);
}

PointInfoWidget::~PointInfoWidget()
{
    delete ui;
}

void PointInfoWidget::setLogP(const QString &text)
{
    ui->labelLogPOut->setText(text);
}

void PointInfoWidget::setChr(const QString &text)
{
    ui->labelChrOut->setText(text);
}

void PointInfoWidget::setPos(const QString &text)
{
    ui->labelPosOut->setText(text);
}

void PointInfoWidget::setGene1(const QString &text)
{
    ui->labelGeneOut1->setText(text);
}

void PointInfoWidget::setGene2(const QString &text)
{
    ui->labelGeneOut2->setText(text);
}

void PointInfoWidget::setInfoNull()
{
    ui->labelLogPOut->setText("(NULL)");
    ui->labelChrOut->setText("(NULL)");
    ui->labelPosOut->setText("(NULL)");
    ui->labelGeneOut1->setText("(NULL)");
    ui->labelGeneOut2->setText("(NULL)");
}

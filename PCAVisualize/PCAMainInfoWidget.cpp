#include "PCAMainInfoWidget.hpp"
#include "ui_PCAMainInfoWidget.h"

PCAMainInfoWidget::PCAMainInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PCAMainInfoWidget)
{
    ui->setupUi(this);
}

PCAMainInfoWidget::~PCAMainInfoWidget()
{
    delete ui;
}

void PCAMainInfoWidget::setNSample(const QString &text)
{
    ui->labelSampleOut->setText(text);
}

void PCAMainInfoWidget::setNPop(const QString &text)
{
    ui->labelPopOut->setText(text);
}

void PCAMainInfoWidget::setPC1(const QString &text)
{
    ui->labelPC1Out->setText(text);
}

void PCAMainInfoWidget::setPC2(const QString &text)
{
    ui->labelPC2Out->setText(text);
}

void PCAMainInfoWidget::setPC3(const QString &text)
{
    ui->labelPC3Out->setText(text);
}

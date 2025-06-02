#include "GWASMainInfoWidget.hpp"
#include "ui_GWASMainInfoWidget.h"

GWASMainInfoWidget::GWASMainInfoWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::GWASMainInfoWidget)
{
    ui->setupUi(this);
}

GWASMainInfoWidget::~GWASMainInfoWidget()
{
    delete ui;
}

void GWASMainInfoWidget::setThreshold(const QString &text)
{
    ui->labelLogPOut->setText(text);
}

void GWASMainInfoWidget::setNumber(const QString &text)
{
    ui->labelNumOut->setText(text);
}

void GWASMainInfoWidget::setMax(const QString &text)
{
    ui->labelMaxOut->setText(text);
}

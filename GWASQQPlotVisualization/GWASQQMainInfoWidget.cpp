#include "GWASQQMainInfoWidget.h"
#include "ui_GWASQQMainInfoWidget.h"

GWASQQMainInfoWidget::GWASQQMainInfoWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::GWASQQMainInfoWidget)
{
    ui->setupUi(this);
}

GWASQQMainInfoWidget::~GWASQQMainInfoWidget()
{
    delete ui;
}

void GWASQQMainInfoWidget::setNumber(const QString &text)
{
    ui->labelNumOut->setText(text);
}

void GWASQQMainInfoWidget::setMax(const QString &text)
{
    ui->labelMaxOut->setText(text);
}

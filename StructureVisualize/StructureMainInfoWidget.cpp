#include "StructureMainInfoWidget.hpp"
#include "ui_StructureMainInfoWidget.h"

StructureMainInfoWidget::StructureMainInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StructureMainInfoWidget)
{
    ui->setupUi(this);
}

StructureMainInfoWidget::~StructureMainInfoWidget()
{
    delete ui;
}

void StructureMainInfoWidget::setNSamples(const QString inText)
{
    ui->labelSampleOut->setText(inText);
}

void StructureMainInfoWidget::setNPops(const QString inText)
{
    ui->labelPopOut->setText(inText);
}

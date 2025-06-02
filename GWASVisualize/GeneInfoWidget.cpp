#include "GeneInfoWidget.hpp"
#include "ui_GeneInfoWidget.h"

GeneInfoWidget::GeneInfoWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::GeneInfoWidget)
{
    ui->setupUi(this);
}

GeneInfoWidget::~GeneInfoWidget()
{
    delete ui;
}

void GeneInfoWidget::setName(const QString &text)
{
    ui->labelNameOut->setText(text);
}

void GeneInfoWidget::setType(const QString &text)
{
    ui->labelTypeOut->setText(text);
}

void GeneInfoWidget::setStrand(const QString &text)
{
    ui->labelStrandOut->setText(text);
}

void GeneInfoWidget::setPos(const QString &text)
{
    ui->labelPosOut->setText(text);
}

void GeneInfoWidget::setAnno(const QString &text)
{
    ui->textAnno->setText(text);
}

void GeneInfoWidget::setLen(const QString &text)
{
    ui->labelLenOut->setText(text);
}

void GeneInfoWidget::setChr(const QString &text)
{
    ui->labelChrOut->setText(text);
}

void GeneInfoWidget::setInfoNull()
{
    ui->labelNameOut->setText("(NULL)");
    ui->labelTypeOut->setText("(NULL)");
    ui->labelStrandOut->setText("(NULL)");
    ui->labelPosOut->setText("(NULL)");
    ui->textAnno->setText("(NULL)");
    ui->labelLenOut->setText("(NULL)");
    ui->labelChrOut->setText("(NULL)");
}

#include "StrucVisualSettingsWidget.hpp"
#include "ui_StrucVisualSettingsWidget.h"

#include "SizeSettingWidget.hpp"

#include <QDebug>

StrucVisualSettingsWidget::StrucVisualSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StrucVisualSettingsWidget)
{
    ui->setupUi(this);
    ui->barGapSlider->setRange(0,100);
    ui->xMarginSlider->setRange(0,100);
    ui->yMarginSlider->setRange(0,100);

    frameSize = QSize(1600,400);
    setSizeVal(frameSize);
}

StrucVisualSettingsWidget::~StrucVisualSettingsWidget()
{
    delete ui;
}

void StrucVisualSettingsWidget::setXMargin(int value)
{
    ui->xMarginSlider->setValue(value);
    ui->xMarginLabel->setText(QString::number(value)+"%");
    emit sendXMarginChanged(double(value)/100);
}

void StrucVisualSettingsWidget::setYMargin(int value)
{
    ui->yMarginSlider->setValue(value);
    ui->yMarginLabel->setText(QString::number(value)+"%");
    emit sendYMarginChanged(double(value)/100);
}

void StrucVisualSettingsWidget::setBarGap(int value)
{
    ui->barGapSlider->setValue(value);
    ui->barGapLabel->setText(QString::number(value)+"%");
    emit sendBarGapChanged(double(value)/100);
}

void StrucVisualSettingsWidget::setSizeVal(QSize size)
{
    frameSize = size;
    ui -> WValLabel -> setText(QString::number(frameSize.width()));
    ui -> HValLabel -> setText(QString::number(frameSize.height()));
    emit sendSizeVal(size);
}

void StrucVisualSettingsWidget::on_barGapSlider_valueChanged(int value)
{
    ui->barGapLabel->setText(QString::number(value)+"%");
    emit sendBarGapChanged(double(value)/100);
}


void StrucVisualSettingsWidget::on_xMarginSlider_valueChanged(int value)
{
    ui->xMarginLabel->setText(QString::number(value)+"%");
    emit sendXMarginChanged(double(value)/100);
}


void StrucVisualSettingsWidget::on_yMarginSlider_valueChanged(int value)
{
    ui->yMarginLabel->setText(QString::number(value)+"%");
    emit sendYMarginChanged(double(value)/100);
}


void StrucVisualSettingsWidget::on_sizeSettingBtn_clicked()
{
    SizeSettingWidget* pSettingWidget = new SizeSettingWidget(frameSize);
    connect(pSettingWidget, &SizeSettingWidget::sendSizeVal,
            this, &StrucVisualSettingsWidget::setSizeVal);
    pSettingWidget -> show();
}


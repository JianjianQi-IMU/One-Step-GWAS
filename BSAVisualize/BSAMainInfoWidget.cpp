#include "BSAMainInfoWidget.hpp"
#include "ui_BSAMainInfoWidget.h"

BSAMainInfoWidget::BSAMainInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BSAMainInfoWidget)
{
    ui->setupUi(this);
}

BSAMainInfoWidget::~BSAMainInfoWidget()
{
    delete ui;
}

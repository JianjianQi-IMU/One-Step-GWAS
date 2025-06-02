#include "GWASQQColorSettings.h"

#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QColorDialog>

GWASQQPointsColorChooseWidget::GWASQQPointsColorChooseWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_ShowModal, true);
    setWindowTitle("Choose Points Color");

    plotWidget = nullptr;

    QPushButton* chooseBtn = new QPushButton(QIcon(":/icon/chooseColorIcon2.png"), "", this);
    QPushButton* setBtn = new QPushButton("set", this);
    QPushButton* cancelBtn = new QPushButton("cancel", this);
    displayWidget = new QWidget(this);

    QWidget* downWidget = new QWidget(this);
    QGroupBox* chooseWidget = new QGroupBox("Points Color", this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* downLayout = new QHBoxLayout(downWidget);
    QHBoxLayout* chooseLayout = new QHBoxLayout(chooseWidget);

    downLayout -> addWidget(setBtn);
    downLayout -> addWidget(cancelBtn);
    chooseLayout -> addWidget(displayWidget);
    chooseLayout -> addWidget(chooseBtn);
    mainLayout -> addWidget(chooseWidget);
    mainLayout -> addWidget(downWidget);

    chooseBtn->setFixedWidth(25);

    connect(chooseBtn, &QPushButton::clicked, this, &GWASQQPointsColorChooseWidget::doChooseColor);
    connect(setBtn, &QPushButton::clicked, this, &GWASQQPointsColorChooseWidget::setColor);
    connect(cancelBtn, &QPushButton::clicked, this, &GWASQQPointsColorChooseWidget::close);
}

GWASQQPointsColorChooseWidget::~GWASQQPointsColorChooseWidget()
{

}

void GWASQQPointsColorChooseWidget::setChoosedColor(const QColor &inCol)
{
    colChoosed = inCol;
    QPalette pte = displayWidget -> palette();
    pte.setColor(QPalette::Background, colChoosed);
    displayWidget -> setPalette(pte);
    update();
}

void GWASQQPointsColorChooseWidget::doChooseColor()
{
    QColor tCol = QColorDialog::getColor(colChoosed, this);
    if (tCol.isValid()) {
        setChoosedColor(tCol);
    }
}

void GWASQQPointsColorChooseWidget::setColor()
{
    if (plotWidget != nullptr) {
        plotWidget -> setPointsColor(colChoosed);
    }
    close();
}

GWASQQDiagonalLineColorChooseWidget::GWASQQDiagonalLineColorChooseWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_ShowModal, true);
    setWindowTitle("Choose Diagonal Line Color");

    plotWidget = nullptr;

    QPushButton* chooseBtn = new QPushButton(QIcon(":/icon/chooseColorIcon2.png"), "", this);
    QPushButton* setBtn = new QPushButton("set", this);
    QPushButton* cancelBtn = new QPushButton("cancel", this);
    displayWidget = new QWidget(this);

    QWidget* downWidget = new QWidget(this);
    QGroupBox* chooseWidget = new QGroupBox("Diagonal Line Color", this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* downLayout = new QHBoxLayout(downWidget);
    QHBoxLayout* chooseLayout = new QHBoxLayout(chooseWidget);

    downLayout -> addWidget(setBtn);
    downLayout -> addWidget(cancelBtn);
    chooseLayout -> addWidget(displayWidget);
    chooseLayout -> addWidget(chooseBtn);
    mainLayout -> addWidget(chooseWidget);
    mainLayout -> addWidget(downWidget);

    chooseBtn->setFixedWidth(25);

    connect(chooseBtn, &QPushButton::clicked, this, &GWASQQDiagonalLineColorChooseWidget::doChooseColor);
    connect(setBtn, &QPushButton::clicked, this, &GWASQQDiagonalLineColorChooseWidget::setColor);
    connect(cancelBtn, &QPushButton::clicked, this, &GWASQQDiagonalLineColorChooseWidget::close);
}

GWASQQDiagonalLineColorChooseWidget::~GWASQQDiagonalLineColorChooseWidget()
{

}

void GWASQQDiagonalLineColorChooseWidget::setChoosedColor(const QColor &inCol)
{
    colChoosed = inCol;
    QPalette pte = displayWidget -> palette();
    pte.setColor(QPalette::Background, colChoosed);
    displayWidget -> setPalette(pte);
    update();
}

void GWASQQDiagonalLineColorChooseWidget::doChooseColor()
{
    QColor tCol = QColorDialog::getColor(colChoosed, this);
    if (tCol.isValid()) {
        setChoosedColor(tCol);
    }
}

void GWASQQDiagonalLineColorChooseWidget::setColor()
{
    if (plotWidget != nullptr) {
        plotWidget -> setDiagonalLineColor(colChoosed);
    }
    close();
}

#include "GWASSetThresholdWidget.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QPalette>
#include <QGroupBox>

GWASSetThresholdWidget::GWASSetThresholdWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);

    pColBtn = new QPushButton("Click Color", this);
    pOKBtn = new QPushButton("save", this);
    pCancelBtn = new QPushButton("cancel", this);
    pColLabel = new QLabel("Color", this);
    pSizeLabel = new QLabel("Size", this);
    pStyleLabel = new QLabel("Style", this);
    pValBox = new QDoubleSpinBox(this);

    pLineSizeComboBox = new LineStyleDisplayComboBox(this);
    pLineStyleComboBox = new LineStyleDisplayComboBox(this);

    QWidget* colorWidget = new QWidget(this);
    QWidget* sizeWidget = new QWidget(this);
    QWidget* styleWidget = new QWidget(this);
    QWidget* downWidget = new QWidget(this);

    QGroupBox* thresholdLineBox = new QGroupBox("Threshold Line", this);
    QGroupBox* thresholdValueBox = new QGroupBox("Threshold Value (- Lg P-value)", this);

    QVBoxLayout* thresholdLineLayout = new QVBoxLayout(thresholdLineBox);
    QVBoxLayout* thresholdValueLayout = new QVBoxLayout(thresholdValueBox);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* colorLayout = new QHBoxLayout(colorWidget);
    QHBoxLayout* sizeLayout = new QHBoxLayout(sizeWidget);
    QHBoxLayout* styleLayout = new QHBoxLayout(styleWidget);
    QHBoxLayout* downLayout = new QHBoxLayout(downWidget);

    thresholdLineLayout->addWidget(colorWidget);
    thresholdLineLayout->addWidget(sizeWidget);
    thresholdLineLayout->addWidget(styleWidget);
    thresholdValueLayout->addWidget(pValBox);

    mainLayout->addWidget(thresholdLineBox);
    mainLayout->addWidget(thresholdValueBox);
    mainLayout->addWidget(downWidget);

    colorLayout->addWidget(pColLabel);
    colorLayout->addWidget(pColBtn);

    sizeLayout->addWidget(pSizeLabel);
    sizeLayout->addWidget(pLineSizeComboBox);

    styleLayout->addWidget(pStyleLabel);
    styleLayout->addWidget(pLineStyleComboBox);

    downLayout->addWidget(pOKBtn);
    downLayout->addWidget(pCancelBtn);

    pValBox->setMinimum(0);
    pValBox->setSingleStep(0.01);

    mapPenStyle[Qt::SolidLine] = 0, vecPenStyle.append(Qt::SolidLine);
    mapPenStyle[Qt::DashLine] = 1, vecPenStyle.append(Qt::DashLine);
    mapPenStyle[Qt::DotLine] = 2, vecPenStyle.append(Qt::DotLine);
    mapPenStyle[Qt::DashDotLine] = 3, vecPenStyle.append(Qt::DashDotLine);
    mapPenStyle[Qt::DashDotDotLine] = 4, vecPenStyle.append(Qt::DashDotDotLine);

    for (int size = 1; size <= 8; ++size) {
        mapSize[size] = vecSize.size();
        vecSize.append(size);
    }

    initLineSizeComboBox();
    initLineStyleComboBox();

    connect(pOKBtn, &QPushButton::clicked, this, &GWASSetThresholdWidget::dealSave);
    connect(pCancelBtn, &QPushButton::clicked, this, &GWASSetThresholdWidget::close);
    connect(pColBtn, &QPushButton::clicked, this, &GWASSetThresholdWidget::chooseCol);

    setWindowTitle("Threshold Settings");
}

void GWASSetThresholdWidget::setLineStyleParam(const LineStyleParam &inParam)
{
    color = inParam.lineColor;
    QPalette tPalette = pColBtn -> palette();
    tPalette.setColor(QPalette::Button, color);
    pColBtn -> setPalette(tPalette);
    int idSize = mapSize[inParam.lineSize];
    int idStyle = mapPenStyle[inParam.lineStyle];
    if (idSize < 0 || idSize >= vecSize.size()) {
        idSize = 0;
    }
    if (idStyle < 0 || idStyle >= vecPenStyle.size()) {
        idStyle = 0;
    }
    pLineSizeComboBox -> setCurrentItem(idSize);
    pLineStyleComboBox -> setCurrentItem(idStyle);
}

void GWASSetThresholdWidget::dealSave()
{
    LineStyleParam tParam;
    int tSize = pLineSizeComboBox->getCurrentParam().lineSize;
    Qt::PenStyle tStyle = pLineStyleComboBox->getCurrentParam().lineStyle;
    tParam.lineColor = color;
    tParam.lineSize = tSize;
    tParam.lineStyle = tStyle;
    emit sendThresholdLine(pValBox->value(), tParam);
    close();
}

void GWASSetThresholdWidget::chooseCol()
{
    color = QColorDialog::getColor(color, this);
    QPalette tPalette=pColBtn -> palette();
    tPalette.setColor(QPalette::Button, color);
    pColBtn -> setPalette(tPalette);
    pColBtn -> update();
}

void GWASSetThresholdWidget::initLineSizeComboBox()
{
    LineStyleParam tParam;
    for (int i = 0; i < vecSize.size(); ++i) {
        tParam.lineSize = vecSize[i];
        pLineSizeComboBox -> addLineItem(QString::number(vecSize[i]) + "pt", tParam);
    }
}

void GWASSetThresholdWidget::initLineStyleComboBox()
{
    LineStyleParam tParam;
    tParam.lineSize = 2;
    for (int i = 0; i < vecPenStyle.size(); ++i) {
        tParam.lineStyle = vecPenStyle[i];
        pLineStyleComboBox -> addLineItem("", tParam);
    }
}

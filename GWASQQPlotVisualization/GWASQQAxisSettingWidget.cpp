#include "GWASQQAxisSettingWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMessageBox>

GWASQQAxisSettingWidget::GWASQQAxisSettingWidget(GWASQQPlotWidget* parent)
{
    plotWidget = parent;

    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Axis Value");

    resetBtn = new QPushButton("reset", this);
    saveBtn = new QPushButton("save", this);
    cancelBtn = new QPushButton("cancel", this);
    xMaxLabel = new QLabel("X Max Limitation", this);
    yMaxLabel = new QLabel("Y Max Limitation", this);
    xMaxValue = new QDoubleSpinBox(this);
    yMaxValue = new QDoubleSpinBox(this);
    xLimits = 0.0;
    yLimits = 0.0;

    xMaxValue -> setMinimum(0);
    yMaxValue -> setMinimum(0);
    xMaxValue -> setMaximum(200);
    yMaxValue -> setMaximum(200);

    xMaxValue -> setSingleStep(0.5);
    yMaxValue -> setSingleStep(0.5);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QGroupBox* upWidget = new QGroupBox("Axis Value",this);
    QVBoxLayout* upLayout = new QVBoxLayout(upWidget);

    QWidget* xMaxWidget = new QWidget(upWidget);
    QWidget* yMaxWidget = new QWidget(upWidget);
    QHBoxLayout* xMaxLayout = new QHBoxLayout(xMaxWidget);
    QHBoxLayout* yMaxLayout = new QHBoxLayout(yMaxWidget);

    QWidget* downWidget = new QWidget(this);
    QHBoxLayout* downLayout = new QHBoxLayout(downWidget);

    xMaxLayout->addWidget(xMaxLabel);
    xMaxLayout->addWidget(xMaxValue);
    yMaxLayout->addWidget(yMaxLabel);
    yMaxLayout->addWidget(yMaxValue);

    upLayout->addWidget(xMaxWidget);
    upLayout->addWidget(yMaxWidget);

    downLayout->addWidget(resetBtn);
    downLayout->addWidget(saveBtn);
    downLayout->addWidget(cancelBtn);

    mainLayout->addWidget(upWidget);
    mainLayout->addWidget(downWidget);

    connect(resetBtn, &QPushButton::clicked,
            this, &GWASQQAxisSettingWidget::do_reset);
    connect(saveBtn, &QPushButton::clicked,
            this, &GWASQQAxisSettingWidget::do_save);
    connect(cancelBtn, &QPushButton::clicked,
            this, &GWASQQAxisSettingWidget::close);
}

void GWASQQAxisSettingWidget::setLimits(double inXLimits, double inYLimits)
{
    xLimits = inXLimits;
    yLimits = inYLimits;
}

void GWASQQAxisSettingWidget::do_reset()
{
    emit sendAxisValue(1, 1, true);
    close();
}

void GWASQQAxisSettingWidget::do_save()
{
    if (xMaxValue->value() < xLimits) {
        QMessageBox::critical(this, "input error", "the x-axis set value is too small.");
        return ;
    }
    if (yMaxValue->value() < yLimits) {
        QMessageBox::critical(this, "input error", "the y-axis set value is too small.");
        return ;
    }
    emit sendAxisValue(xMaxValue->value(), yMaxValue->value(), false);
    close();
}

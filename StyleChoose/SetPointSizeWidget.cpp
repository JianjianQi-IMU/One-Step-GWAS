#include "SetPointSizeWidget.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

SetPointSizeWidget::SetPointSizeWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_ShowModal, true);
    setWindowTitle("Points Size");

    valBox = new QSpinBox(this);
    okBtn = new QPushButton("save", this);
    cancelBtn = new QPushButton("cancel", this);

    QWidget *downWidget = new QWidget(this);
    QHBoxLayout* downLayout = new QHBoxLayout(downWidget);

    QGroupBox* upWidget = new QGroupBox(this);
    QVBoxLayout* upLayout = new QVBoxLayout(upWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(upWidget);
    mainLayout->addWidget(downWidget);

    upLayout -> addWidget(valBox);

    downLayout -> addWidget(okBtn);
    downLayout -> addWidget(cancelBtn);

    valBox->setMinimum(0);
    valBox->setMaximum(128);
    valBox->setValue(2);

    upWidget->setTitle("Points Size");

    connect(okBtn, &QPushButton::clicked, this, &SetPointSizeWidget::savePointSize);
    connect(cancelBtn, &QPushButton::clicked, this, &SetPointSizeWidget::close);
}

SetPointSizeWidget::SetPointSizeWidget(int defaultSize, QWidget *parent)
    : SetPointSizeWidget(parent)
{
    valBox->setValue(defaultSize);
}

void SetPointSizeWidget::savePointSize()
{
    emit sendPointSize((float)valBox->value());
    close();
}

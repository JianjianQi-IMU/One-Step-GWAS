#include "SetGWASParamWidget.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

SetGWASParamWidget::SetGWASParamWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle("Threads number");
    id=0;
    continueBtn = new QPushButton("continue",this);
    cancelBtn = new QPushButton("cancel",this);
    nThreadBox = new QSpinBox(this);
    binFormBox = new QComboBox(this);
    QGroupBox* nThreadGroup = new QGroupBox(this);
    QGroupBox* binFormGroup = new QGroupBox(this);
    nThreadGroup->setTitle("thread number for calculation");
    binFormGroup->setTitle("");
    QWidget* downWidget = new QWidget(this);

    QVBoxLayout* nThreadLayout = new QVBoxLayout(nThreadGroup);
    QVBoxLayout* binFormLayout = new QVBoxLayout(binFormGroup);
    QHBoxLayout* downLayout = new QHBoxLayout(downWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    nThreadLayout->addWidget(nThreadBox);
    binFormLayout->addWidget(binFormBox);
    downLayout->addWidget(continueBtn);
    downLayout->addWidget(cancelBtn);
    mainLayout->addWidget(nThreadGroup);
    mainLayout->addWidget(binFormGroup);
    mainLayout->addWidget(downWidget);

    nThreadBox->setMinimum(1);
    nThreadBox->setMaximum(100);

    binFormBox->addItem("Additive");
    binFormBox->addItem("Ref Dominant");
    binFormBox->addItem("Alt Dominant");

    connect(continueBtn,&QPushButton::clicked,this,&SetGWASParamWidget::dealwithContinue);
    connect(cancelBtn,&QPushButton::clicked,this,&SetGWASParamWidget::close);
}

SetGWASParamWidget::SetGWASParamWidget(size_t inID, ThreadRun::AnalyzeClass inInfo, QWidget *parent)
    :SetGWASParamWidget(parent)
{
    id=inID;
    info=inInfo;
}

SetGWASParamWidget::~SetGWASParamWidget()
{

}

void SetGWASParamWidget::dealwithContinue()
{
    emit startGWAS(id,info,nThreadBox->value());
    close();
}

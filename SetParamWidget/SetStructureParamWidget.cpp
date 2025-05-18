#include "SetStructureParamWidget.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>

SetStructureParamWidget::SetStructureParamWidget(QWidget *parent)
    :QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle("Structure");
    para=nullptr;
    id=0;
    QLabel* burinLabel=new QLabel("burin number",this);
    QLabel* recordLabel=new QLabel("record number",this);
    QLabel* popLabel=new QLabel("population number",this);
    burinBox=new QSpinBox(this);
    recordBox=new QSpinBox(this);
    popBox=new QSpinBox(this);
    continueBtn = new QPushButton("continue",this);
    cancelBtn = new QPushButton("cancel",this);
    QWidget* burinWidget=new QWidget(this);
    QWidget* recordWidget=new QWidget(this);
    QWidget* popWidget=new QWidget(this);
    QWidget* continueWidget=new QWidget(this);
    QHBoxLayout* burinLayout=new QHBoxLayout(burinWidget);
    QHBoxLayout* recordLayout=new QHBoxLayout(recordWidget);
    QHBoxLayout* popLayout=new QHBoxLayout(popWidget);
    QHBoxLayout* continueLayout=new QHBoxLayout(continueWidget);
    QVBoxLayout* mainLayout=new QVBoxLayout(this);
    mainLayout->addWidget(burinWidget);
    mainLayout->addWidget(recordWidget);
    mainLayout->addWidget(popWidget);
    mainLayout->addWidget(continueWidget);
    burinLayout->addWidget(burinLabel);
    burinLayout->addWidget(burinBox);
    recordLayout->addWidget(recordLabel);
    recordLayout->addWidget(recordBox);
    popLayout->addWidget(popLabel);
    popLayout->addWidget(popBox);
    continueLayout->addWidget(continueBtn);
    continueLayout->addWidget(cancelBtn);
    burinBox->setMaximum(100000);
    burinBox->setMinimum(1);
    recordBox->setMaximum(100000);
    recordBox->setMinimum(1);
    popBox->setMaximum(100);
    popBox->setMinimum(1);

    burinBox->setValue(40000);
    recordBox->setValue(15000);

    connect(cancelBtn,&QPushButton::clicked,this,&SetStructureParamWidget::close);
    connect(continueBtn,&QPushButton::clicked,this,&SetStructureParamWidget::dealwithContinue);
}

SetStructureParamWidget::SetStructureParamWidget(size_t inId, MML::StructureParam *inPara, QWidget *parent)
    :SetStructureParamWidget(parent)
{
    para=inPara;
    id=inId;
}

SetStructureParamWidget::~SetStructureParamWidget()
{

}

void SetStructureParamWidget::dealwithContinue()
{
    if(para){
        para->nBurnIn = burinBox->value();
        para->nRecord = recordBox->value();
        para->nPopulation = popBox->value();
    }
    emit startStructure(id,1);
    close();
}

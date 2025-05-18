#include "SetFastStructureParamWidget.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>

SetFastStructureParamWidget::SetFastStructureParamWidget(QWidget *parent)
    :QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Fast Structure");
    para = nullptr;
    id = 0;
    QLabel* popLabel = new QLabel("population number",this);
    popBox = new QSpinBox(this);
    continueBtn = new QPushButton("continue",this);
    cancelBtn = new QPushButton("cancel",this);
    QWidget* popWidget = new QWidget(this);
    QWidget* continueWidget = new QWidget(this);
    QHBoxLayout* popLayout = new QHBoxLayout(popWidget);
    QHBoxLayout* continueLayout = new QHBoxLayout(continueWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(popWidget);
    mainLayout->addWidget(continueWidget);
    continueLayout->addWidget(continueBtn);
    continueLayout->addWidget(cancelBtn);
    popLayout->addWidget(popLabel);
    popLayout->addWidget(popBox);

    connect(cancelBtn,&QPushButton::clicked,this,&SetFastStructureParamWidget::close);
    connect(continueBtn,&QPushButton::clicked,this,&SetFastStructureParamWidget::dealwithContinue);

}

SetFastStructureParamWidget::SetFastStructureParamWidget(size_t inId, MML::FastStructureParam *inPara, QWidget *parent)
    :SetFastStructureParamWidget(parent)
{
    para = inPara;
    id = inId;
}

SetFastStructureParamWidget::~SetFastStructureParamWidget()
{

}

void SetFastStructureParamWidget::dealwithContinue()
{
    if(para){
        para->nPop = popBox->value();
    }
    emit startFastStructure(id,1);
    close();
}

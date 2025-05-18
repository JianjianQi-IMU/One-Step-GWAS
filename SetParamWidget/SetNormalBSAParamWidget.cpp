#include "SetNormalBSAParamWidget.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>

SetNormalBSAParamWidget::SetNormalBSAParamWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);

    continueBtn       = new QPushButton("continue",this);
    cancelBtn         = new QPushButton("cancel",this);
    editWinLen        = new QLineEdit(this);
    editStride        = new QLineEdit(this);
    labelNHP          = new QLabel("n High Pool",this);
    labelNLP          = new QLabel("n Low Pool",this);
    labelDepth        = new QLabel("Depth minimun",this);
    labelLowThre      = new QLabel("Low threshold",this);
    labelHighThre     = new QLabel("High threshold",this);
    labelWinLen       = new QLabel("Windows length",this);
    labelStride       = new QLabel("Stride length",this);
    labelPvalue       = new QLabel("P value",this);
    nHPBox            = new QSpinBox(this);
    nLPBox            = new QSpinBox(this);
    depthBox          = new QSpinBox(this);
    lowThreBox        = new QDoubleSpinBox(this);
    highThreBox       = new QDoubleSpinBox(this);
    pvalueBox         = new QDoubleSpinBox(this);

    QRegExpValidator* posValidator = new QRegExpValidator(QRegExp("[\\d,]*"), this);
    editWinLen->setValidator(posValidator);
    editStride->setValidator(posValidator);

    QVBoxLayout* mainLayout=new QVBoxLayout(this);

    QGroupBox* paraBox=new QGroupBox("Parameter",this);
    QWidget*   HPWidget=new QWidget(this);
    QWidget*   LPWidget=new QWidget(this);
    QWidget*   depthWidget=new QWidget(this);
    QWidget*   lowThreWidget=new QWidget(this);
    QWidget*   highThrewidget=new QWidget(this);
    QWidget*   winLenWidget=new QWidget(this);
    QWidget*   strideWidget=new QWidget(this);
    QWidget*   pvalueWidget=new QWidget(this);
    QHBoxLayout* HPLayout=new QHBoxLayout(HPWidget);
    QHBoxLayout* LPLayout=new QHBoxLayout(LPWidget);
    QHBoxLayout* depthLayout=new QHBoxLayout(depthWidget);
    QHBoxLayout* lowThreLayout=new QHBoxLayout(lowThreWidget);
    QHBoxLayout* highThreLayout=new QHBoxLayout(highThrewidget);
    QHBoxLayout* winLenLayout=new QHBoxLayout(winLenWidget);
    QHBoxLayout* strideLauout=new QHBoxLayout(strideWidget);
    QHBoxLayout* pvalueLayout=new QHBoxLayout(pvalueWidget);
    QVBoxLayout* paraBoxLayout=new QVBoxLayout(paraBox);
    HPLayout->addWidget(labelNHP);
    HPLayout->addWidget(nHPBox);
    LPLayout->addWidget(labelNLP);
    LPLayout->addWidget(nLPBox);
    depthLayout->addWidget(labelDepth);
    depthLayout->addWidget(depthBox);
    lowThreLayout->addWidget(labelLowThre);
    lowThreLayout->addWidget(lowThreBox);
    highThreLayout->addWidget(labelHighThre);
    highThreLayout->addWidget(highThreBox);
    winLenLayout->addWidget(labelWinLen);
    winLenLayout->addWidget(editWinLen);
    strideLauout->addWidget(labelStride);
    strideLauout->addWidget(editStride);
    pvalueLayout->addWidget(labelPvalue);
    pvalueLayout->addWidget(pvalueBox);
    paraBoxLayout->addWidget(HPWidget);
    paraBoxLayout->addWidget(LPWidget);
    paraBoxLayout->addWidget(depthWidget);
    paraBoxLayout->addWidget(lowThreWidget);
    paraBoxLayout->addWidget(highThrewidget);
    paraBoxLayout->addWidget(winLenWidget);
    paraBoxLayout->addWidget(strideWidget);
    paraBoxLayout->addWidget(pvalueWidget);

    QWidget* downWidget=new QWidget(this);
    QHBoxLayout* downLayout=new QHBoxLayout(downWidget);
    downLayout->addWidget(continueBtn);
    downLayout->addWidget(cancelBtn);

    mainLayout->addWidget(paraBox);
    mainLayout->addWidget(downWidget);

    nHPBox->setMinimum(1);
    nLPBox->setMinimum(1);
    depthBox->setMinimum(0);
    lowThreBox->setMinimum(0);
    lowThreBox->setMaximum(1);
    highThreBox->setMinimum(0);
    highThreBox->setMaximum(1);
    pvalueBox->setMinimum(0);
    pvalueBox->setMaximum(0.5);

    depthBox->setValue(7);
    lowThreBox->setValue(0.3);
    highThreBox->setValue(0.7);
    pvalueBox->setValue(0.05);
    editWinLen->setText("1,000,000");
    editStride->setText("10,000");

    connect(continueBtn,&QPushButton::clicked,
            this,&SetNormalBSAParamWidget::dealwithContinue);
    connect(cancelBtn,&QPushButton::clicked,
            this,&SetNormalBSAParamWidget::close);
}

SetNormalBSAParamWidget::SetNormalBSAParamWidget(size_t inId, FD::BSAProject *inProject, QWidget *parent)
    :SetNormalBSAParamWidget(parent)
{
    id=inId;
    project=inProject;
}

SetNormalBSAParamWidget::~SetNormalBSAParamWidget()
{

}

void SetNormalBSAParamWidget::dealwithContinue()
{
    project->setThreDeep(depthBox->value());
    project->setThreLow(lowThreBox->value());
    project->setThreHigh(highThreBox->value());
    project->setPValue(pvalueBox->value());
    project->setNHighPool(nHPBox->value());
    project->setNLowPool(nLPBox->value());
    if(!(editWinLen->text().isEmpty())){
        QString tWinLen = editWinLen->text();
        tWinLen.remove(',');
        project->setWinLength(tWinLen.toULongLong());
    }
    if(!(editStride->text().isEmpty())){
        QString tStride = editStride->text();
        tStride.remove(',');
        project->setStrideLength(tStride.toULongLong());
    }

    emit startNormalBSA(id,1);
    close();
}

#include "SetKinshipParamWidget.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <vector>
#include <string>

std::vector<std::string> TypeNameList {
    "relative",
    "covariance",
    "IBS parwise",
};

SetKinshipParamWidget::SetKinshipParamWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Kinship");
    para = nullptr;
    id = 0;
    QLabel* typeLabel = new QLabel("type",this);
    typeBox = new QComboBox(this);
    continueBtn = new QPushButton("continue",this);
    cancelBtn = new QPushButton("cancel",this);

    QGroupBox* basicWidget = new QGroupBox("Basic", this);
    QWidget* continueWidget = new QWidget(this);
    QVBoxLayout* basicLayout = new QVBoxLayout(basicWidget);
    QHBoxLayout* continueLayout = new QHBoxLayout(continueWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QWidget* typeWidget = new QWidget(basicWidget);
    QHBoxLayout* typeLayout = new QHBoxLayout(typeWidget);

    mainLayout->addWidget(basicWidget);
    mainLayout->addWidget(continueWidget);
    continueLayout->addWidget(continueBtn);
    continueLayout->addWidget(cancelBtn);

    basicLayout->addWidget(typeWidget);

    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(typeBox);

    connect(cancelBtn,&QPushButton::clicked,this,&SetKinshipParamWidget::close);
    connect(continueBtn,&QPushButton::clicked,this,&SetKinshipParamWidget::dealwithContinue);
}

SetKinshipParamWidget::SetKinshipParamWidget(size_t inId, MML::KinshipParam *inPara, QWidget *parent)
    : SetKinshipParamWidget(parent)
{
    para = inPara;
    id = inId;
    if (para->nPloid != 2) {
        typeBox->addItem(TypeNameList[0].c_str());
        typeBox->addItem(TypeNameList[1].c_str());
    } else {
        typeBox->addItem(TypeNameList[0].c_str());
        typeBox->addItem(TypeNameList[1].c_str());
        typeBox->addItem(TypeNameList[2].c_str());
    }
}

void SetKinshipParamWidget::dealwithContinue()
{
    if(para){
        para->mode = (MML::KinshipMode)typeBox->currentIndex();
    }
    emit startKinship(id,1);
    close();
}

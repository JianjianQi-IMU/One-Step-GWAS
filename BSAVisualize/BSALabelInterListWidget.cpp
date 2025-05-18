#include "BSALabelInterListWidget.hpp"

BSALabelInterListWidget::BSALabelInterListWidget(QWidget *parent) : QWidget(parent)
{
    pLabelView = new BSALabelInterListView(this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(pLabelView);

    connect(pLabelView,&BSALabelInterListView::removeLabelInter,
            this,&BSALabelInterListWidget::removeLabelInter);
    connect(pLabelView,&BSALabelInterListView::lookLabelInter,
            this,&BSALabelInterListWidget::lookLabelInter);
    connect(pLabelView,&BSALabelInterListView::removeAllLabelInter,
            this,&BSALabelInterListWidget::removeAllLabelInter);
}

BSALabelInterListWidget::~BSALabelInterListWidget()
{

}

void BSALabelInterListWidget::setInter(FD::BSAInterVal *inInter, size_t inNInter)
{
    inter=inInter;
    nInter=inNInter;

    pLabelView->setInterData(inInter);
}

bool BSALabelInterListWidget::setMaxLabelInterNum(int n)
{
    return pLabelView->setMaxmun(n);
}

bool BSALabelInterListWidget::setChrName(char **pName)
{
    return pLabelView->setChrName(pName);
}

bool BSALabelInterListWidget::addInter(BSAPointID id)
{
    return pLabelView->addInter(id);
}

bool BSALabelInterListWidget::removeInter(int row)
{
    return pLabelView->removeInter(row);
}

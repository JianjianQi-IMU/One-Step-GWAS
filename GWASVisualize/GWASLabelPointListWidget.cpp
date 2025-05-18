#include "GWASLabelPointListWidget.hpp"
#include "ui_GWASLabelPointListWidget.h"

GWASLabelPointListWidget::GWASLabelPointListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GWASLabelPointListWidget)
{
    ui->setupUi(this);
    pLabelView = new LabelPointsListView(this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(pLabelView);

    connect(pLabelView,&LabelPointsListView::removeLabelPoint,
            this,&GWASLabelPointListWidget::removeLabelPoint);
    connect(pLabelView,&LabelPointsListView::lookLabelPoint,
            this,&GWASLabelPointListWidget::lookLabelPoint);
    connect(pLabelView,&LabelPointsListView::removeAllLabelPoint,
            this,&GWASLabelPointListWidget::removeAllLabelPoint);
}

GWASLabelPointListWidget::~GWASLabelPointListWidget()
{
    delete ui;
}

void GWASLabelPointListWidget::setPoints(ValPoints *inPoints, long long inNPoints)
{
    points=inPoints;
    nPoints=inNPoints;

    pLabelView->setPointsData(inPoints);

}

bool GWASLabelPointListWidget::setMaxLabelPointsNum(int n)
{
    return pLabelView->setMaxmun(n);
}

bool GWASLabelPointListWidget::setChrName(char **pName)
{
    return pLabelView->setChrName(pName);
}

bool GWASLabelPointListWidget::addPoint(long long id)
{
    return pLabelView->addPoint(id);
}

bool GWASLabelPointListWidget::removePoint(int row)
{
    return pLabelView->removePoint(row);
}

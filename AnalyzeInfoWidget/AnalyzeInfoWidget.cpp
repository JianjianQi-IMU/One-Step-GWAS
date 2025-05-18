#include "AnalyzeInfoWidget.h"

AnalyzeInfoWidget::AnalyzeInfoWidget(QWidget *parent) : QWidget(parent)
{
    pProjectList = nullptr;
    pTab = new QTabWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->addWidget(pTab);
}

AnalyzeInfoWidget::~AnalyzeInfoWidget()
{

}

void AnalyzeInfoWidget::setProject(QList<FD::BaseProject*>* inProject)
{
    pProjectList = inProject;
}

void AnalyzeInfoWidget::addNew()
{
    QTextBrowser* pTextBrowser = new QTextBrowser(this);
    browserList.append(pTextBrowser);
    pTab->addTab(pTextBrowser, QString("Project ") + QString::number(browserList.size()));
}

void AnalyzeInfoWidget::removeOne(int i)
{
    if (i >= browserList.size()) return;
    pTab->removeTab(i);
    delete browserList[i];
    browserList.removeAt(i);
    for (int j = i; j < browserList.size(); ++j) {
        pTab->setTabText(j, QString("Project ") + QString::number(j + 1));
    }
}

void AnalyzeInfoWidget::removeAll()
{
    int Len = browserList.size();
    for (int i = 0; i < Len; ++i) {
        pTab->removeTab(0);
        delete browserList[0];
        browserList.removeAt(0);
    }
}

void AnalyzeInfoWidget::updateText(int i)
{
    if (i >= browserList.size()) return;
    browserList[i]->setText((*pProjectList)[i]->logInfo.c_str());
}

void AnalyzeInfoWidget::selectProject(int i)
{
    if (i >= browserList.size()) return;
    pTab->setCurrentIndex(i);
}

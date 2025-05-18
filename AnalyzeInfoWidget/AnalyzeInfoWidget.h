#ifndef ANALYZEINFOWIDGET_H
#define ANALYZEINFOWIDGET_H

#include <QSplitter>
#include <QLayout>
#include <QFont>
#include <QTextBrowser>
//#include <QTextDocument>
#include <QPushButton>
#include <QWidget>
#include <QTabWidget>
#include <QList>
#include "ThreadProcess/QThreadRun.hpp"
#include "AnalyzeListView/AnalyzeListView.h"

class AnalyzeInfoWidget : public QWidget
{
    Q_OBJECT
public:
    QList<FD::BaseProject*>* pProjectList;
    QTabWidget* pTab;
    QList<QTextBrowser*> browserList;
    QWidget* topWidget;
    explicit AnalyzeInfoWidget(QWidget *parent = nullptr);
    ~AnalyzeInfoWidget();
    void setProject(QList<FD::BaseProject*>* inProject);

signals:


public slots:
    void addNew();
    void removeOne(int i);
    void removeAll();
    void updateText(int i);
    void selectProject(int i);
};

#endif // ANALYZEINFOWIDGET_H


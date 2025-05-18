#ifndef GENEFINDLIST_HPP
#define GENEFINDLIST_HPP

#include <QWidget>
#include <QListView>
#include <QMenu>
#include <QStyledItemDelegate>
#include <QStringListModel>
#include <QPainter>
#include <QList>
#include <QModelIndex>
#include <QMessageBox>

#include "GenomeAnnotation/GenomeAnnotation.hpp"
#include "StringFindListItemDelegate.hpp"

class GeneFindListView : public QListView
{
    Q_OBJECT
private:

public:
    GenomeAnnotation* genome;
    StringFindListItemDelegate* pDelegate;
    QStringListModel* pModel;
    QList<long long> iGeneList;
    long long nGene;
//    GeneFindListModel* pModel;
    QMenu* pMenu;

    explicit GeneFindListView(QWidget *parent = nullptr);
    GeneFindListView(GenomeAnnotation* inGenome,QWidget *parent = nullptr);
    ~GeneFindListView();
    void setGenome(GenomeAnnotation* inGenome);
    bool removeOne(int row);
    void insertGeneid(long long* inList,long long n);
    void clearAll();
signals:
    void sendLookGenei(long long i);
public slots:
    void lookGenei();
    void removeGenei();
    void menuExec(const QPoint &pos);
};



namespace Ui {
class GeneFindList;
}

class GeneFindList : public QWidget
{
    Q_OBJECT

public:
    explicit GeneFindList(QWidget *parent = nullptr);
    ~GeneFindList();

    void setGenome(GenomeAnnotation* inGenome);

    static const int searchDisplayMaxNum = 1000;

signals:
    void sendLookGenei(long long i);
public slots:
    void getLookGenei(long long i);

private slots:
    void on_searchBtn_clicked();

    void on_clearBtn_clicked();

private:
    Ui::GeneFindList *ui;

};

#endif // GENEFINDLIST_HPP

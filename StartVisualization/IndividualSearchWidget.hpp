#ifndef INDIVIDUALSEARCHWIDGET_HPP
#define INDIVIDUALSEARCHWIDGET_HPP

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QVector>
#include <QAbstractListModel>
#include <QListView>
#include "FileDeal/FileDeal.hpp"
#include "StartVisualization/StringFindListItemDelegate.hpp"

#include <QDebug>

class IndividualSearchOutListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit IndividualSearchOutListModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    //custom
    bool loadNameList(char** inNameList,size_t n);
    bool setIDList(const QModelIndex &index,size_t id,int role = Qt::EditRole);
    size_t getIDSample(int row) const;
private:
    QVector<QString> nameList;
    QVector<size_t>  idDisplayList;
};

class IndividualSearchOutListView : public QListView
{
    Q_OBJECT
private:
    IndividualSearchOutListModel* pModel;
    StringFindListItemDelegate *pDelegate;
public:
    explicit IndividualSearchOutListView(QWidget *parent = nullptr);
    ~IndividualSearchOutListView();
    bool loadNameList(char** inNameList,size_t n);
    void clearList();
    void setIDDisplayList(const QVector<size_t>& inList);
signals:
    void sendSelectedChanged(size_t id);
public slots:
    void doSelectedChanged(const QModelIndex &index);
};


class IndividualSearchWidget : public QWidget
{
    Q_OBJECT
private:
    QLineEdit*       pEdit;
    QPushButton*     pSearchBtn;
    QPushButton*     pClearBtn;
    IndividualSearchOutListView* pList;

    char** nameList;
    long long nSample;
public:
    explicit IndividualSearchWidget(QWidget *parent = nullptr);
    ~IndividualSearchWidget();
    bool loadNameList(char** inNameList,size_t n);
    void searchSample(const char* inKeywords,QVector<size_t>& outList,long long outMaxN) const;
    void clearList();
public slots:
    void doSearchSample();
signals:
    void sendSelectedChanged(long long id);
};

#endif // INDIVIDUALSEARCHWIDGET_HPP

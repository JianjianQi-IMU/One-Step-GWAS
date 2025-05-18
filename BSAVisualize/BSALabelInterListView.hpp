#ifndef BSALABELINTERLISTVIEW_HPP
#define BSALABELINTERLISTVIEW_HPP

#include <QWidget>

#include <QListView>
#include <QAbstractListModel>
#include <QVector>
#include <QIcon>
#include <QMenu>
#include <QAction>

#include <BSAVisualize/BSAIndexScatterWidget.hpp>

class BSALabelPointsListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit BSALabelPointsListModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    //custom
    bool setInterData(FD::BSAInterVal *inInter);
    bool setIDList(const QModelIndex &index,BSAPointID id,int role = Qt::EditRole);
    bool setMaxmun(int n);
    bool setChrName(char** pName);

    QString getDataInfo(short info) const;
private:
    QVector<BSAPointID> idList;
    int maxmun;
    FD::BSAInterVal *inter;
    QIcon icon;
    char** chrName;
};

class BSALabelInterListView : public QListView
{
    Q_OBJECT
private:
    BSALabelPointsListModel* pModel;
    QMenu* pMenu;
public:
    explicit BSALabelInterListView(QWidget *parent = nullptr);
    bool setInterData(FD::BSAInterVal *inInter);
    bool setMaxmun(int n);
    bool setChrName(char** pName);

signals:
    void removeLabelInter(int row);
    void lookLabelInter(int row);
    void removeAllLabelInter();

public slots:
    bool addInter(BSAPointID id);
    bool removeInter(int row);

    bool doRemoveLabelInter();
    bool doLookLabelInter();
    bool doRemoveAllLabelInter();
    void menuExec(const QPoint& pos);

};

#endif // BSALABELINTERLISTVIEW_HPP

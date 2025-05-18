#ifndef LABELPOINTSLISTVIEW_HPP
#define LABELPOINTSLISTVIEW_HPP

#include <QListView>
#include <QAbstractListModel>
#include <QVector>
#include <QIcon>
#include <QMenu>
#include <QAction>

#include <GWASVisualize/LogPScatterWidget.h>

class LabelPointsListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit LabelPointsListModel(QObject *parent = nullptr);

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
    bool setPointsData(ValPoints *inPoints);
    bool setIDList(const QModelIndex &index,long long id,int role = Qt::EditRole);
    bool setMaxmun(int n);
    bool setChrName(char** pName);
private:
    QVector<long long> idList;
    int maxmun;
    ValPoints *points;
    QIcon icon;
    char** chrName;
};

class LabelPointsListView : public QListView
{
    Q_OBJECT
private:
    LabelPointsListModel* pModel;
    QMenu* pMenu;
public:
    explicit LabelPointsListView(QWidget *parent = nullptr);
    bool setPointsData(ValPoints *inPoints);
    bool setMaxmun(int n);
    bool setChrName(char** pName);

signals:
    void removeLabelPoint(int row);
    void lookLabelPoint(int row);
    void removeAllLabelPoint();

public slots:
    bool addPoint(long long id);
    bool removePoint(int row);

    bool doRemoveLabelPoint();
    bool doLookLabelPoint();
    bool doRemoveAllLabelPoint();
    void menuExec(const QPoint& pos);
};

#endif // LABELPOINTSLISTVIEW_HPP

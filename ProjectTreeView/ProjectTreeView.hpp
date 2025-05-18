#ifndef PROJECTTREEVIEW_HPP
#define PROJECTTREEVIEW_HPP

#include "AnalyzeListView/AnalyzeListView.h"
#include <QTreeView>
#include <QStandardItem>
#include <QAbstractItemModel>
#include <QHeaderView>

class ProjectTreeItemModelNode{
public:
    explicit ProjectTreeItemModelNode();
    ~ProjectTreeItemModelNode();
    QString name;
    QString fileName;
    QString iconFile;
    int role;
    ProjectTreeItemModelNode* parent;
    QList<ProjectTreeItemModelNode*> child;
    void clear();
};


class ProjectTreeItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ProjectTreeItemModel(QObject *parent = nullptr);
    explicit ProjectTreeItemModel(QList<AnalyzeUnitInfo>* inUnitList,QObject *parent = nullptr);
    ~ProjectTreeItemModel();
    // Header:
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

//    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    // Fetch data dynamically:
//    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

//    bool canFetchMore(const QModelIndex &parent) const override;
//    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    bool setDataRole(const QModelIndex &index, int inRole,
                     int role = Qt::EditRole);
    bool setDataFileName(const QModelIndex &index, const QString& inFileName,
                         int role = Qt::EditRole);
    bool setDataIconFile(const QModelIndex &index, const QString& inIconFile,
                         int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex& index) const override;

//     Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    // custom:
    void loadProjectUintList(QList<AnalyzeUnitInfo>* inUnitList);
    ProjectTreeItemModelNode* getNode(int row, int column,
                                               const QModelIndex &parent = QModelIndex());
    void clearRoot();
    bool isRootNode(const ProjectTreeItemModelNode* p);
    runState projectState(int id);
private:
   ProjectTreeItemModelNode* rootNode;
   QList<AnalyzeUnitInfo>* pUnitList;

public slots:
   void removeUpdate(int row);
   void insertUpdate(int row);
   void removeAllUpdate();
   void changeUpdate(int row);
};

class ProjectTreeView : public QTreeView
{
    Q_OBJECT
private:
    QMenu* pMenu;
    QAction* actRemove;
    QAction* actLoadGWAS;
    QAction* actLoadBSA;
    QAction* actAddNew;
public:
    ProjectTreeItemModel* pModel;
    explicit ProjectTreeView(QWidget *parent = nullptr);
    void loadProjectUintList(QList<AnalyzeUnitInfo>* inUnitList);
    ProjectTreeView(QList<AnalyzeUnitInfo>* inUnitList,QWidget *parent = nullptr);
//    void loadProjectUint(AnalyzeUnitInfo* inUnit,const QString& name);
signals:
    void sendRemoveProject(int id);
    void sendLoadProject(int id,FD::ProjectInfo info);
    void sendAddNewProject();
    void sendSelectProject(int id);
    void sendSelectProjectState(runState state);
public slots:
    void removeProject();
    void loadGWASProject();
    void loadBSAProject();
    void loadProject(FD::ProjectInfo info);
    void removeUpdate(int row);
    void insertUpdate(int row);
    void removeAllUpdate();
    void changeUpdate(int row);
    void selectProject(int row);
    void widgetCliked(const QModelIndex& index);
    void menuExec(const QPoint& pos);
};

#endif // PROJECTTREEVIEW_HPP

#include "ProjectTreeView.hpp"

void printNode(ProjectTreeItemModelNode* root){
    qDebug() << root->name << ' ' << root->child.size();
    for(int i=0;i<root->child.size();++i){
        if((root->child)[i]) printNode((root->child)[i]);
    }
}

ProjectTreeItemModelNode::ProjectTreeItemModelNode()
    :parent(nullptr),role(0)
{

}

ProjectTreeItemModelNode::~ProjectTreeItemModelNode()
{
    for(int i=0;i<child.size();++i){
        if(child[i]) delete child[i];
    }
    if(parent){
        parent->child.removeOne(this);
    }
}

void ProjectTreeItemModelNode::clear()
{
    for(int i=0;i<child.size();++i){
        if(child[i]) delete child[i];
    }
    name.clear();
    parent=nullptr;
    child.clear();
}

ProjectTreeItemModel::ProjectTreeItemModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootNode=new ProjectTreeItemModelNode;
    pUnitList=nullptr;
}

ProjectTreeItemModel::ProjectTreeItemModel(QList<AnalyzeUnitInfo>* inUnitList, QObject *parent)
    :ProjectTreeItemModel(parent)
{
    loadProjectUintList(inUnitList);
}

ProjectTreeItemModel::~ProjectTreeItemModel()
{
    if(rootNode) {
        delete rootNode;
    }
}

//QVariant ProjectTreeItemModel::headerData(int section, Qt::Orientation orientation, int role) const
//{

//    // FIXME: Implement me!
//}

//bool ProjectTreeItemModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
//{
//    if (value != headerData(section, orientation, role)) {
//        // FIXME: Implement me!
//        emit headerDataChanged(orientation, section, section);
//        return true;
//    }
//    return false;
//}

QModelIndex ProjectTreeItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!parent.isValid())
        return createIndex(row,column,rootNode->child[row]);
    ProjectTreeItemModelNode* pParent=static_cast<ProjectTreeItemModelNode*>(parent.internalPointer());
    if(column<=1&&column>=0&&row>=0&&row<(pParent->child).size()){
        return createIndex(row,column,(pParent->child)[row]);
    }
    return QModelIndex();
    // FIXME: Implement me!
}

QModelIndex ProjectTreeItemModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) return QModelIndex();
    ProjectTreeItemModelNode* pointer=static_cast<ProjectTreeItemModelNode*>(index.internalPointer());
    ProjectTreeItemModelNode* pParent=pointer->parent;
    if(pParent&&pParent!=rootNode){
        int parentIndex=((pParent->parent)->child).indexOf(pParent);
        return createIndex(parentIndex,0,pParent);
    }
    return QModelIndex();
    // FIXME: Implement me!
}

int ProjectTreeItemModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return (rootNode->child).size();
    ProjectTreeItemModelNode* pParent=static_cast<ProjectTreeItemModelNode*>(parent.internalPointer());
    return (pParent->child).size();
    // FIXME: Implement me!
}

int ProjectTreeItemModel::columnCount(const QModelIndex &parent) const
{
    ProjectTreeItemModelNode* pParent=static_cast<ProjectTreeItemModelNode*>(parent.internalPointer());
    if(pParent==rootNode) return 1;
    return 2;
    // FIXME: Implement me!
}

//bool ProjectTreeItemModel::hasChildren(const QModelIndex &parent) const
//{
//    // FIXME: Implement me!
//}

//bool ProjectTreeItemModel::canFetchMore(const QModelIndex &parent) const
//{
//    // FIXME: Implement me!
//    return false;
//}

//void ProjectTreeItemModel::fetchMore(const QModelIndex &parent)
//{
//    // FIXME: Implement me!
//}

QVariant ProjectTreeItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    ProjectTreeItemModelNode* pointer=static_cast<ProjectTreeItemModelNode*>(index.internalPointer());
    if(pointer!=rootNode){
        if(role==Qt::DisplayRole){
            switch(index.column()){
            case 0:
                return pointer->name;
            case 1:
                return pointer->fileName;
            }
        }
        else if(role==Qt::DecorationRole&&index.column()==0){
            return QIcon(pointer->iconFile);
        }
        else if(role==Qt::ToolTipRole&&index.column()==1){
            return pointer->fileName;
        }
    }
    // FIXME: Implement me!
    return QVariant();
}

bool ProjectTreeItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        ProjectTreeItemModelNode* pointer=static_cast<ProjectTreeItemModelNode*>(index.internalPointer());
        if(pointer!=rootNode){
            pointer->name=value.toString();
            emit dataChanged(index, index, QVector<int>() << role);
            return true;
        }

    }
    return false;
}

bool ProjectTreeItemModel::setDataRole(const QModelIndex &index, int inRole, int role)
{
    ProjectTreeItemModelNode* pointer=static_cast<ProjectTreeItemModelNode*>(index.internalPointer());
    if(pointer!=rootNode){
        pointer->role=inRole;
//            emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

bool ProjectTreeItemModel::setDataFileName(const QModelIndex &index, const QString &inFileName, int role)
{
    ProjectTreeItemModelNode* pointer=static_cast<ProjectTreeItemModelNode*>(index.internalPointer());
    if(pointer!=rootNode){
        pointer->fileName=inFileName;
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

bool ProjectTreeItemModel::setDataIconFile(const QModelIndex &index, const QString &inIconFile, int role)
{
    ProjectTreeItemModelNode* pointer=static_cast<ProjectTreeItemModelNode*>(index.internalPointer());
    if(pointer!=rootNode){
        pointer->iconFile=inIconFile;
//        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags ProjectTreeItemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    ProjectTreeItemModelNode* pointer=static_cast<ProjectTreeItemModelNode*>(index.internalPointer());
    if(pointer==rootNode) return Qt::NoItemFlags;
    if(pointer->role==Qt::DisplayRole){
        return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
    }// FIXME: Implement me!
    return Qt::NoItemFlags;
}

//void ProjectTreeItemModel::loadProjectUint(AnalyzeUnitInfo *inUnit,const QString& name)
//{
//    QModelIndex tParentIndex=QModelIndex();
//    if(inUnit){
//        pUnit=inUnit;
//        removeRows(0,rootNode->child.size());
//        insertRows(0,1);
//        int nName=inUnit->getNFile();
//        tParentIndex=index(0,0);
//        setData(tParentIndex,name);
//        setDataRole(tParentIndex,Qt::DisplayRole);
//        for(int i=0;i<nName;++i){
//            insertRows(i,1,tParentIndex);
//            setData(index(i,0,tParentIndex),inUnit->getIFile(i));
//            setDataFileName(index(i,0,tParentIndex),inUnit->getIFileName(i));
//            setDataRole(index(i,0,tParentIndex),Qt::DisplayRole);
//        }
//        printNode(rootNode);
//    }
//}

ProjectTreeItemModelNode *ProjectTreeItemModel::getNode(int row, int column, const QModelIndex &parent)
{
    if (!parent.isValid())
        return rootNode->child[row];
    ProjectTreeItemModelNode* pointer=static_cast<ProjectTreeItemModelNode*>(parent.internalPointer());
    if(pointer) return pointer->child[row];
    return nullptr;
}

void ProjectTreeItemModel::clearRoot()
{
    rootNode->clear();
}

bool ProjectTreeItemModel::isRootNode(const ProjectTreeItemModelNode *p)
{
    return p==rootNode;
}

runState ProjectTreeItemModel::projectState(int id)
{
    if(id<0||id>=rowCount()) return RUN_UNKNOWN;
    return (*pUnitList)[id].state;
}

void ProjectTreeItemModel::removeUpdate(int row)
{
    int rootChildCount=rootNode->child.size();
    removeRows(row,1);
    for(int i=row;i<rootChildCount-1;++i){
        setData(index(i,0),QString("Project ")+QString::number(i+1));
    }
}

void ProjectTreeItemModel::insertUpdate(int row)
{
    insertRows(row,1);
    QString tFileType;
    AnalyzeUnitInfo& tUnit=(*pUnitList)[row];
    int nName=tUnit.getNFile();
    QModelIndex tParentIndex=index(row,0);
    setData(tParentIndex,QString("Project ")+QString::number(row+1));
    setDataRole(tParentIndex,Qt::DisplayRole);
    setDataIconFile(tParentIndex,":/icon/projectIcon.png");
    for(int i=0;i<nName;++i){
        tFileType=tUnit.getIFile(i);
        insertRows(i,1,tParentIndex);
        setData(index(i,0,tParentIndex),tFileType);
        setDataFileName(index(i,0,tParentIndex),tUnit.getIFileName(i));
        setDataRole(index(i,0,tParentIndex),Qt::DisplayRole);
        setDataIconFile(index(i,0,tParentIndex),tUnit.getIFileIcon(i));
    }
}

void ProjectTreeItemModel::removeAllUpdate()
{
    removeRows(0,rootNode->child.size());
}

void ProjectTreeItemModel::changeUpdate(int row)
{
    AnalyzeUnitInfo& tUnit=(*pUnitList)[row];
    QString tFileType;
    QModelIndex tParentIndex=index(row,0);
    int nName=tUnit.getNFile();
    removeRows(0,rowCount(tParentIndex),tParentIndex);
    for(int i=0;i<nName;++i){
        tFileType=tUnit.getIFile(i);
        insertRows(i,1,tParentIndex);
        setData(index(i,0,tParentIndex),tFileType);
        setDataFileName(index(i,0,tParentIndex),tUnit.getIFileName(i));
        setDataRole(index(i,0,tParentIndex),Qt::DisplayRole);
        setDataIconFile(index(i,0,tParentIndex),tUnit.getIFileIcon(i));
    }
}

bool ProjectTreeItemModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if(count<=0) return false;
    ProjectTreeItemModelNode* pParent=static_cast<ProjectTreeItemModelNode*>(parent.internalPointer());
    ProjectTreeItemModelNode* tmp=nullptr;
    if(!pParent) pParent=rootNode;
    if(row>(pParent->child).size()) return false;
    beginInsertRows(parent, row, row + count - 1);
    for(int i=0;i<count;++i){
        tmp=new ProjectTreeItemModelNode();
        tmp->parent=pParent;
        (pParent->child).insert(row,tmp);
    }
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool ProjectTreeItemModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    return false;
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();
}

bool ProjectTreeItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(count<=0) return false;
    ProjectTreeItemModelNode* pParent=static_cast<ProjectTreeItemModelNode*>(parent.internalPointer());
    if(!pParent) pParent=rootNode;
    if(pParent->child.size()<count+row) return false;
    beginRemoveRows(parent, row, row + count - 1);
    for(int i=0;i<count;++i){
        delete (pParent->child)[row];
    }
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

bool ProjectTreeItemModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    return false;
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();
}

void ProjectTreeItemModel::loadProjectUintList(QList<AnalyzeUnitInfo> *inUnitList)
{
    pUnitList=inUnitList;
}

ProjectTreeView::ProjectTreeView(QWidget *parent) : QTreeView(parent)
{
    pModel=new ProjectTreeItemModel(this);
    setModel(pModel);
    header()->setVisible(false);

    pMenu=new QMenu(this);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,&ProjectTreeView::customContextMenuRequested,this,&ProjectTreeView::menuExec);
    actRemove = new QAction("remove project",this);
    actLoadGWAS = new QAction(QIcon(":/icon/openIcon.png"),"load GWAS file",this);
    actLoadBSA = new QAction(QIcon(":/icon/openIcon.png"),"load BSA file",this);
    actAddNew = new QAction("create new project",this);
    actRemove->setIcon(QIcon(":/icon/removeProjectIcon.png"));
    actAddNew->setIcon(QIcon(":/icon/newProjectIcon.png"));

    pMenu->addAction(actLoadGWAS);
    pMenu->addAction(actLoadBSA);
    pMenu->addAction(actRemove);
    pMenu->addAction(actAddNew);
    connect(actAddNew,&QAction::triggered,this,&ProjectTreeView::sendAddNewProject);
    connect(actLoadGWAS,&QAction::triggered,this,&ProjectTreeView::loadGWASProject);
    connect(actLoadBSA,&QAction::triggered,this,&ProjectTreeView::loadBSAProject);
    connect(actRemove,&QAction::triggered,this,&ProjectTreeView::removeProject);
    connect(this,&ProjectTreeView::clicked,this,&ProjectTreeView::widgetCliked);
}

void ProjectTreeView::loadProjectUintList(QList<AnalyzeUnitInfo> *inUnitList)
{
    pModel->loadProjectUintList(inUnitList);
}

ProjectTreeView::ProjectTreeView(QList<AnalyzeUnitInfo>* inUnitList, QWidget *parent)
    :ProjectTreeView(parent)
{
    pModel->loadProjectUintList(inUnitList);
}

void ProjectTreeView::removeProject()
{
    QModelIndex index = currentIndex();
    QModelIndex parent = index.parent();
    ProjectTreeItemModelNode* pParent=static_cast<ProjectTreeItemModelNode*>(parent.internalPointer());
    if(pModel->isRootNode(pParent)||!parent.isValid()){
        emit sendRemoveProject(index.row());
    }
    else{
        emit sendRemoveProject(parent.row());
    }
}

void ProjectTreeView::loadGWASProject()
{
    loadProject(FD::PROJECT_GWAS);
}

void ProjectTreeView::loadBSAProject()
{
    loadProject(FD::PROJECT_BSA);
}

void ProjectTreeView::loadProject(FD::ProjectInfo info)
{
    QModelIndex index = currentIndex();
    QModelIndex parent = index.parent();
    ProjectTreeItemModelNode* pParent=static_cast<ProjectTreeItemModelNode*>(parent.internalPointer());
    if(pModel->isRootNode(pParent)||!parent.isValid()){
        emit sendLoadProject(index.row(),info);
    }
    else{
        emit sendLoadProject(parent.row(),info);
    }
}

//void ProjectTreeView::loadProjectUint(AnalyzeUnitInfo *inUnit, const QString &name)
//{
//    pModel->loadProjectUint(inUnit,name);
//    update();
//}

void ProjectTreeView::removeUpdate(int row)
{
    pModel->removeUpdate(row);
}

void ProjectTreeView::insertUpdate(int row)
{
    pModel->insertUpdate(row);
}

void ProjectTreeView::removeAllUpdate()
{
    pModel->removeAllUpdate();
}

void ProjectTreeView::changeUpdate(int row)
{
    pModel->changeUpdate(row);
}

void ProjectTreeView::selectProject(int row)
{
    if(row<0||row>=pModel->rowCount()) return;
    setCurrentIndex(pModel->index(row,0));
}

void ProjectTreeView::widgetCliked(const QModelIndex &index)
{
    if(!index.isValid()) return;
    QModelIndex parent = index.parent();
    ProjectTreeItemModelNode* pParent=static_cast<ProjectTreeItemModelNode*>(parent.internalPointer());
    int id;
    if(pModel->isRootNode(pParent)||!parent.isValid()){
        id=index.row();
    }
    else{
        id=parent.row();
    }
    emit sendSelectProject(id);
    emit sendSelectProjectState(pModel->projectState(id));
}

void ProjectTreeView::menuExec(const QPoint &pos)
{
    pMenu->exec(this->mapToGlobal(pos));
}



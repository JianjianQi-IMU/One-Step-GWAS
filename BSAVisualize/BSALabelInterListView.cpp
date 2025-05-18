#include "BSALabelInterListView.hpp"

BSALabelPointsListModel::BSALabelPointsListModel(QObject *parent)
: QAbstractListModel(parent)
{
    maxmun=10000;
    icon.addFile(":/icon/opoints.png");
}

QVariant BSALabelPointsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    return QVariant();
}

int BSALabelPointsListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;
    return idList.size();
    // FIXME: Implement me!
}

QVariant BSALabelPointsListModel::data(const QModelIndex &index, int role) const
{
    FD::BSAInterVal tmp;
    QString out;
    if (!index.isValid())
        return QVariant();
    if(role==Qt::DisplayRole){
        tmp=inter[idList.at(index.row()).id];
        out=QString(chrName[tmp.idChr])+":"+QString::number(tmp.start);
        out+=".."+QString::number(tmp.stop);
        return out;
    }
    else if(role==Qt::DecorationRole){
        return icon;
    }
    // FIXME: Implement me!
    return QVariant();
}


bool BSALabelPointsListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if(idList.size()+count>maxmun||row<0||row>idList.size()||count<=0) return false;
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    for(int i=0;i<count;++i) idList.insert(row,BSAPointID());
    endInsertRows();
    return true;
}

bool BSALabelPointsListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(row+count>idList.size()||row<0||row>=idList.size()||count<=0) return false;
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    idList.remove(row,count);
    endRemoveRows();
    return true;
}

bool BSALabelPointsListModel::setInterData(FD::BSAInterVal *inInter)
{
    if(!inInter) return false;
    inter=inInter;
    return true;
}

bool BSALabelPointsListModel::setIDList(const QModelIndex &index, BSAPointID id,int role)
{
    if(!index.isValid()) return false;
    int row = index.row();
    if(row<0||row>=idList.size()) return false;
    idList[row] = id;
    emit dataChanged(index, index, QVector<int>() << role);
    return true;
}

bool BSALabelPointsListModel::setMaxmun(int n)
{
    if(n<0) return false;
    maxmun=n;
    return true;
}

bool BSALabelPointsListModel::setChrName(char **pName)
{
    if(!pName) return false;
    chrName=pName;
    return true;
}

QString BSALabelPointsListModel::getDataInfo(short info) const
{
    QString out;
    switch (info) {
    case 0: out="High Pool Index"; break;
    case 1: out="Low Pool Index"; break;
    case 2: out="Delta Index"; break;
    case 3: out="Up Threshold"; break;
    case 4: out="Down Threshold"; break;
    default: break;
    }
    return out;
}


BSALabelInterListView::BSALabelInterListView(QWidget *parent) : QListView(parent)
{
    pModel = new BSALabelPointsListModel(this);
    setModel(pModel);
    pMenu = new QMenu(this);

    QAction* actLookInter = new QAction("Look");
    QAction* actRemoveInter = new QAction("Remove");
    QAction* actRemoveAllInter = new QAction("Remove All");

    pMenu->addAction(actLookInter);
    pMenu->addAction(actRemoveInter);
    pMenu->addAction(actRemoveAllInter);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,&BSALabelInterListView::customContextMenuRequested,this,&BSALabelInterListView::menuExec);

    connect(actLookInter,&QAction::triggered,
            this,&BSALabelInterListView::doLookLabelInter);
    connect(actRemoveInter,&QAction::triggered,
            this,&BSALabelInterListView::doRemoveLabelInter);
    connect(actRemoveAllInter,&QAction::triggered,
            this,&BSALabelInterListView::doRemoveAllLabelInter);
}

bool BSALabelInterListView::setInterData(FD::BSAInterVal *inInter)
{
    return pModel->setInterData(inInter);
}

bool BSALabelInterListView::setMaxmun(int n)
{
    return pModel->setMaxmun(n);
}

bool BSALabelInterListView::setChrName(char **pName)
{
    return pModel->setChrName(pName);
}

bool BSALabelInterListView::addInter(BSAPointID id)
{
    if(!(pModel->insertRows(pModel->rowCount(),1))) return false;
    return pModel->setIDList(pModel->index(pModel->rowCount()-1),id);
}

bool BSALabelInterListView::removeInter(int row)
{
    return pModel->removeRows(row,1);
}

bool BSALabelInterListView::doRemoveLabelInter()
{
    int row = currentIndex().row();
    if(!(pModel->removeRows(row,1))) return false;
    emit removeLabelInter(row);
    return true;
}

bool BSALabelInterListView::doLookLabelInter()
{
    int row = currentIndex().row();
    emit lookLabelInter(row);
    return true;
}

bool BSALabelInterListView::doRemoveAllLabelInter()
{
    if(!(pModel->removeRows(0,pModel->rowCount()))) return false;
    emit removeAllLabelInter();
    return true;
}

void BSALabelInterListView::menuExec(const QPoint &pos)
{
    pMenu->exec(this->mapToGlobal(pos));
}


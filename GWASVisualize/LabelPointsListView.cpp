#include "LabelPointsListView.hpp"

LabelPointsListModel::LabelPointsListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    maxmun=10000;
    icon.addFile(":/icon/opoints.png");
}

QVariant LabelPointsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    return QVariant();
}

int LabelPointsListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid()) {
        return 0;
    }
    return idList.size();
    // FIXME: Implement me!
}

QVariant LabelPointsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (role == Qt::DisplayRole) {
        ValPoints tmp = points[idList.at(index.row())];
        return QString(chrName[tmp.idchr]) + ":" + QString::number(tmp.pos);
    } else if (role == Qt::DecorationRole) {
        return icon;
    }
    // FIXME: Implement me!
    return QVariant();
}


bool LabelPointsListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (idList.size() + count > maxmun || row < 0 || row > idList.size() || count <= 0) {
        return false;
    }
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    for (int i = 0; i < count; ++i) {
        idList.insert(row, 1);
    }
    endInsertRows();
    return true;
}

bool LabelPointsListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row + count > idList.size() || row < 0 || row >= idList.size() || count <= 0) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    idList.remove(row, count);
    endRemoveRows();
    return true;
}

bool LabelPointsListModel::setPointsData(ValPoints *inPoints)
{
    if (!inPoints) {
        return false;
    }
    points = inPoints;
    return true;
}

bool LabelPointsListModel::setIDList(const QModelIndex &index, long long id,int role)
{
    if (!index.isValid()) {
        return false;
    }
    int row = index.row();
    if (row < 0 || row >= idList.size()) {
        return false;
    }
    idList[row] = id;
    emit dataChanged(index, index, QVector<int>() << role);
    return true;
}

bool LabelPointsListModel::setMaxmun(int n)
{
    if (n < 0) {
        return false;
    }
    maxmun = n;
    return true;
}

bool LabelPointsListModel::setChrName(char **pName)
{
    if (!pName) {
        return false;
    }
    chrName = pName;
    return true;
}

LabelPointsListView::LabelPointsListView(QWidget *parent)
    : QListView(parent)
{
    pModel = new LabelPointsListModel(this);
    setModel(pModel);
    pMenu = new QMenu(this);

    QAction* actLookPoint = new QAction("Look");
    QAction* actRemovePoint = new QAction("Remove");
    QAction* actRemoveAllPoint = new QAction("Remove All");

    pMenu->addAction(actLookPoint);
    pMenu->addAction(actRemovePoint);
    pMenu->addAction(actRemoveAllPoint);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &LabelPointsListView::customContextMenuRequested, this, &LabelPointsListView::menuExec);
    connect(actLookPoint, &QAction::triggered, this, &LabelPointsListView::doLookLabelPoint);
    connect(actRemovePoint, &QAction::triggered, this, &LabelPointsListView::doRemoveLabelPoint);
    connect(actRemoveAllPoint, &QAction::triggered, this, &LabelPointsListView::doRemoveAllLabelPoint);
}

bool LabelPointsListView::setPointsData(ValPoints *inPoints)
{
    return pModel->setPointsData(inPoints);
}

bool LabelPointsListView::setMaxmun(int n)
{
    return pModel->setMaxmun(n);
}

bool LabelPointsListView::setChrName(char **pName)
{
    return pModel->setChrName(pName);
}

bool LabelPointsListView::addPoint(long long id)
{
    if (!(pModel->insertRows(pModel->rowCount(), 1))) {
        return false;
    }
    return pModel->setIDList(pModel->index(pModel->rowCount() - 1), id);
}

bool LabelPointsListView::removePoint(int row)
{
    return pModel->removeRows(row, 1);
}

bool LabelPointsListView::doRemoveLabelPoint()
{
    int row = currentIndex().row();
    if (!(pModel->removeRows(row, 1))) {
        return false;
    }
    emit removeLabelPoint(row);
    return true;
}

bool LabelPointsListView::doLookLabelPoint()
{
    int row = currentIndex().row();
    emit lookLabelPoint(row);
    return true;
}

bool LabelPointsListView::doRemoveAllLabelPoint()
{
    if (!(pModel->removeRows(0, pModel->rowCount()))) {
        return false;
    }
    emit removeAllLabelPoint();
    return true;
}

void LabelPointsListView::menuExec(const QPoint &pos)
{
    pMenu->exec(this->mapToGlobal(pos));
}

#include "DataInteractTableView.hpp"

DataInteractTableModel::DataInteractTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    editFlag = Qt::NoItemFlags;
}

QVariant DataInteractTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (section > 0 && section < colName.size() && role == Qt::DisplayRole && colRole == Qt::DisplayRole)
        {
           return colName.at(section);
        }
    } else if(orientation == Qt::Vertical) {
        if (section > 0 && section < rowName.size() && role == Qt::DisplayRole && rowRole == Qt::DisplayRole)
        {
           return rowName.at(section);
        }
    }
    return QAbstractTableModel::headerData(section,orientation,role);

}

bool DataInteractTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (editFlag == Qt::NoItemFlags) return false;
    if (value != headerData(section, orientation, role)) {
        if (orientation == Qt::Horizontal) {
            if (section > 0 && section < colName.size())
            {
               colName[section] = value.toString();
            }
        } else if (orientation == Qt::Vertical) {
            if (section > 0 && section < rowName.size()) {
               rowName[section] = value.toString();
            }
        }
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}


int DataInteractTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return rowName.size();
}

int DataInteractTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return colName.size();
}

QVariant DataInteractTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.row() < rowName.size() && index.column() < colName.size()) {
        if (role == Qt::DisplayRole) {
            if (mainData[index.row()][index.column()] == MML::DATA_NA) {
                return QString("NA");
            } else {
                return QString::number(mainData[index.row()][index.column()], 'f', 6);
            }
        }
    }
    return QVariant();
}

bool DataInteractTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    QString tmp;
    bool transOK;
    double tData;
    if (data(index, role) != value) {
        if (index.row() < rowName.size() && index.column() < colName.size()) {
            tmp = value.toString();
            if (tmp == "NA" || tmp == "-") {
                mainData[index.row()][index.column()] = MML::DATA_NA;
                emit dataChanged(index, index, QVector<int>() << role);
                return true;
            } else {
                tData = tmp.toDouble(&transOK);
                if (transOK) {
                    mainData[index.row()][index.column()] = tData;
                    emit dataChanged(index, index, QVector<int>() << role);
                    return true;
                }
            }
        }

    }
    return false;
}

Qt::ItemFlags DataInteractTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;

    return editFlag; // FIXME: Implement me!
}

bool DataInteractTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row > rowName.size()) return false;
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        rowName.insert(row, "NULL");
        mainData.insert(row, QVector<double>(colName.size(), 0));
    }
    endInsertRows();
    return true;
}

bool DataInteractTableModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    if (column < 0 || column > colName.size()) return false;
    beginInsertColumns(parent, column, column + count - 1);
    for (int i = 0; i < count; ++i) {
        colName.insert(column, "NULL");
    }
    for (int i = 0; i < rowName.size(); ++i) {
        mainData[i].insert(column, count, 0);
    }
    endInsertColumns();
    return true;
}

bool DataInteractTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || count <= 0 || row + count > rowName.size()) return false;
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        rowName.removeAt(row);
    }
    mainData.remove(row, count);
    endRemoveRows();
    return true;
}

bool DataInteractTableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    if (column < 0 || count <= 0 || column + count > colName.size()) {
        return false;
    }
    beginRemoveColumns(parent, column, column + count - 1);
    for (int i = 0; i < count; ++i) {
        colName.removeAt(column);
    }
    for (int i = 0; i < rowName.size(); ++i) {
        mainData[i].remove(column, count);
    }
    endRemoveColumns();
    return true;
}

bool DataInteractTableModel::setAllData(double *inMainData, int inNRow, int inNCol, char **inRowName, char **inColName)
{
    if (!inMainData || inNRow == 0 || inNCol == 0) return false;
    mainData.clear();
    rowName.clear();
    colName.clear();
    for (int i = 0; i < inNRow; ++i) {
        mainData.push_back(QVector<double>());
        for (int j = 0; j < inNCol; ++j) {
            mainData[i].push_back(inMainData[i * inNCol + j]);
        }
    }
    if (inRowName) {
        for (int i = 0; i < inNRow; ++i) {
            rowName.push_back(inRowName[i]);
        }
        rowRole = Qt::DisplayRole;
    } else {
        for (int i = 0; i < inNRow; ++i) {
            rowName.push_back("NULL");
        }
        rowRole = Qt::SizeHintRole;
    }
    if (inColName) {
        for (int i = 0; i < inNCol; ++i) {
            colName.push_back(inColName[i]);
        }
        colRole = Qt::DisplayRole;
    } else {
        for (int i = 0;i < inNCol; ++i) {
            colName.push_back("NULL");
        }
        colRole = Qt::SizeHintRole;
    }
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
    return true;
}

bool DataInteractTableModel::output(double *outMainData, int outNRow, int outNCol, char **outRowName, char **outColName)
{
    if (!outMainData || outNRow != rowName.size() || outNCol != colName.size()) {
        return false;
    }
    int i, j;
    for (i = 0; i < outNRow; ++i) {
        for (j = 0; j < outNCol; ++j) {
            outMainData[i * outNCol + j] = mainData[i][j];
        }
    }
    if (outRowName) {
        for (i = 0; i < outNRow; ++i) {
            for (j = 0; j < rowName[i].length(); ++j) {
                outRowName[i][j] = (rowName[i].toLatin1().data())[j];
            }
            outRowName[i][j] = '\0';
        }
    }
    if (outColName) {
        for (i = 0; i < outNCol; ++i) {
            for (j = 0; j < colName[i].length(); ++j) {
                outColName[i][j] = (colName[i].toLatin1().data())[j];
            }
            outColName[i][j] = '\0';
        }
    }
    return true;
}

void DataInteractTableModel::setEditFlag(Qt::ItemFlags inFlag)
{
    editFlag = inFlag;
}

DataInteractTableView::DataInteractTableView(QWidget *parent) : QTableView(parent)
{
    mainData = nullptr;
    nRow = 0;
    nCol = 0;
    rowName = nullptr;
    colName = nullptr;
    pModel = new DataInteractTableModel(this);
    setModel(pModel);
    pMenu = nullptr;
    isValid = true;

    pMenu = new QMenu(this);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &DataInteractTableView::customContextMenuRequested, this, &DataInteractTableView::menuExec);

    QAction* actCopy = new QAction("Copy", this);
    QAction* actPaste = new QAction("Paste", this);

    pMenu->addAction(actCopy);
    pMenu->addAction(actPaste);

    connect(actCopy, &QAction::triggered, this, &DataInteractTableView::copySelected);
    connect(actPaste, &QAction::triggered, this, &DataInteractTableView::pasteSelected);

}

void DataInteractTableView::setEditEnable()
{
    pModel->setEditFlag(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
}

bool DataInteractTableView::load(double *inMainData, size_t inNRow, size_t inNCol, char **inRowName, char **inColName)
{
    if (!inMainData || inNRow == 0 || inNCol == 0) {
        return false;
    }
    mainData = inMainData;
    nRow = inNRow;
    nCol = inNCol;
    rowName = inRowName;
    colName = inColName;
    isValid = true;
    return pModel->setAllData(mainData, nRow, nCol, rowName, colName);
}

bool DataInteractTableView::pasteData(const QString& text,int iRow,int iCol)
{
    QStringList linesList = text.split('\n'), lineList;
    int i, j, n = linesList.size(), m;
    if (iRow < 0 || iRow >= nRow || iCol < 0 || iCol >= nCol) return false;
    for (i = 0; i < n && i + iRow < nRow; ++i) {
        while (!linesList[i].isEmpty() && (linesList[i].back() == '\n' || linesList[i].back() == '\r' ||
            linesList[i].back() == '\t' || linesList[i].back() == ' ')) linesList[i].chop(1);
        if (linesList[i].isEmpty()) continue;
        lineList = linesList[i].split('\t');
        m = lineList.size();
        for (j = 0; j < m && j + iCol < nCol; ++j) {
            pModel->setData(pModel->index(i + iRow, j + iCol), lineList[j]);
        }
    }
    return true;
}

QString DataInteractTableView::copyData(int iRow1, int iCol1, int iRow2, int iCol2)
{
    QString outString;
    if (iRow1 < 0 || iRow1 >= nRow || iCol1 < 0 ||iCol1 >= nCol ||
       iRow2 < 0 || iRow2 >= nRow || iCol2 < 0 || iCol2 >= nCol ||
       iRow1 > iRow2 || iCol1 > iCol2) return QString();
    for (int i = iRow1; i <= iRow2; ++i) {
        for (int j = iCol1; j <= iCol2; ++j) {
            outString += pModel->data(pModel->index(i, j)).toString();
            outString.append(QLatin1Char('\t'));
        }
        outString.chop(1);
        outString.append(QLatin1Char('\n'));
    }
    return outString;
}

bool DataInteractTableView::event(QEvent *e)
{
    bool re = QTableView::event(e);
    QKeyEvent* keye = nullptr;
    if (e->type() == QEvent::KeyPress) {
        keye = dynamic_cast<QKeyEvent*>(e);
        if (keye->matches(QKeySequence::Paste)) {
            pasteSelected();
        } else if (keye->matches(QKeySequence::Copy)) {
            copySelected();
        }
    }

    return re;
}

bool DataInteractTableView::write()
{
    if (isValid) {
        return pModel->output(mainData, nRow, nCol, rowName, colName);
    }
    return false;
}

void DataInteractTableView::pasteSelected()
{
    QModelIndexList indexes = selectedIndexes();
    QModelIndex index0 = indexes.first();
    pasteData((QApplication::clipboard())->text(), index0.row(), index0.column());
}

void DataInteractTableView::copySelected()
{
    QModelIndexList indexes = selectedIndexes();
    QModelIndex index0 = indexes.first(), index1 = indexes.last();
    QString tmp = copyData(index0.row(), index0.column(), index1.row(), index1.column());
    (QApplication::clipboard())->setText(tmp);
}

void DataInteractTableView::menuExec(const QPoint &pos)
{
    pMenu->exec(this->mapToGlobal(pos));
}

void DataInteractTableView::reload()
{
    pModel->setAllData(mainData, nRow, nCol, rowName, colName);
}

#ifndef DATAINTERACTTABLEVIEW_HPP
#define DATAINTERACTTABLEVIEW_HPP


#include <QWidget>
#include <QAction>
#include <QMenu>
#include <QTableView>
#include <QPushButton>
#include <QSplitter>
#include <QLayout>
#include <QEvent>
#include <QKeyEvent>
#include <QAbstractTableModel>
#include <QApplication>
#include <QClipboard>
#include <QKeySequence>
#include <QMainWindow>
#include "DataManager/DataManager.hpp"

#include <QDebug>

class DataInteractTableModel : public QAbstractTableModel
{
    Q_OBJECT
private:

public:
    explicit DataInteractTableModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    // Custom function:
    bool setAllData(double* inMainData, int inNRow, int inNCol,char** inRowName = nullptr, char** inColName = nullptr);
    bool output(double* inMainData, int inNRow, int inNCol, char** inRowName = nullptr, char** inColName = nullptr);
    void setEditFlag(Qt::ItemFlags inFlag);
private:
    QStringList rowName;
    QStringList colName;
    int rowRole;
    int colRole;
    QVector<QVector<double>> mainData;
    Qt::ItemFlags editFlag;
};

class DataInteractTableView : public QTableView
{
    Q_OBJECT
public:
    double* mainData;
    size_t nRow;
    size_t nCol;
    char** rowName;
    char** colName;
    QMenu* pMenu;
    bool isValid;
    DataInteractTableModel* pModel;
    explicit DataInteractTableView(QWidget *parent = nullptr);
    void setEditEnable();
    bool load(double* inMainData, size_t inNRow, size_t inNCol, char** inRowName = nullptr, char** inColName = nullptr);
    bool pasteData(const QString& text, int iRow, int iCol);
    QString copyData(int iRow1,int iCol1, int iRow2, int iCol2);
    bool event(QEvent* e) override;
signals:

public slots:
    bool write();
    void pasteSelected();
    void copySelected();
    void menuExec(const QPoint& pos);
    void reload();
};

#endif // DATAINTERACTTABLEVIEW_HPP

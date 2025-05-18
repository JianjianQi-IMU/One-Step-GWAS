#ifndef DATAINTERACTTABLEWIDGET_H
#define DATAINTERACTTABLEWIDGET_H

#include "DataInteractTableView.hpp"

namespace Ui {
class DataInteractTableWidget;
}

class DataInteractTableWidget : public QMainWindow
{
    Q_OBJECT
public:
    bool isEdit;
    DataInteractTableView* pView;
    explicit DataInteractTableWidget(QWidget *parent = nullptr, bool isEditable = false);
    ~DataInteractTableWidget();
    bool load(double* inMainData, size_t inNRow, size_t inNCol, char** inRowName = nullptr, char** inColName = nullptr);

signals:

private slots:
    void on_actionWriteIn_triggered();

    void on_actionReset_triggered();

    void on_actionSave_triggered();

private:
    Ui::DataInteractTableWidget *ui;

};

#endif // DATAINTERACTTABLEWIDGET_H

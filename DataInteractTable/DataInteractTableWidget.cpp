#include "DataInteractTableWidget.hpp"
#include "ui_DataInteractTableWidget.h"

DataInteractTableWidget::DataInteractTableWidget(QWidget *parent,bool isEditable)
    : QMainWindow(parent), ui(new Ui::DataInteractTableWidget)
{
    setAttribute(Qt::WA_DeleteOnClose, true);

    pView = new DataInteractTableView(this);
    setCentralWidget(pView);
    isEdit = isEditable;
    if (isEditable) {
        pView->setEditEnable();
    } else {
        ui->actionSave->setDisabled(true);
    }
}

DataInteractTableWidget::~DataInteractTableWidget()
{
}

bool DataInteractTableWidget::load(double *inMainData, size_t inNRow, size_t inNCol, char **inRowName, char **inColName)
{
    return pView->load(inMainData, inNRow, inNCol, inRowName, inColName);
}

void DataInteractTableWidget::on_actionWriteIn_triggered()
{
    if (!isEdit) return;
    pView->write();
}

void DataInteractTableWidget::on_actionReset_triggered()
{
    pView->reload();
}


void DataInteractTableWidget::on_actionSave_triggered()
{
}


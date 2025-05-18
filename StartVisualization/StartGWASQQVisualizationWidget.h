#ifndef STARTGWASQQVISUALIZATIONWIDGET_H
#define STARTGWASQQVISUALIZATIONWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include "GWASQQPlotVisualization/GWASQQVisualizationWidget.h"

#include <QDebug>

class StartGWASQQVisualizationWidget : public QWidget
{
    Q_OBJECT
private:
    QPushButton*    continueBtn;
    QPushButton*    cancelBtn;
    QLineEdit*      bimPathEdit;
    QLineEdit*      logPPathEdit;
    QLineEdit*      lenInfoPathEdit;
    QSpinBox*       colIndexBox;

public:
    explicit StartGWASQQVisualizationWidget(QWidget *parent = nullptr);

public slots:
    void openBimFile();
    void openLogPFile();
    void openLenInfoFile();
    void startVisualization();
signals:
};

#endif // STARTGWASQQVISUALIZATIONWIDGET_H

#ifndef STARTKINVISUALIZATIONWIDGET_HPP
#define STARTKINVISUALIZATIONWIDGET_HPP

#include "FileDeal/FileDeal.hpp"
#include "KinshipVisualize/KinshipVisualizeWidget.hpp"
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

class StartKinVisualizationWidget : public QWidget
{
    Q_OBJECT
private:
    QLineEdit*    filePathEdit;
    QLineEdit*    kinPathEdit;
    QPushButton*  continueBtn;
    QPushButton*  cancelBtn;
    MML::Mat      kin;
public:
    explicit StartKinVisualizationWidget(QWidget *parent = nullptr);
    ~StartKinVisualizationWidget();
public slots:
    void openFile();
    void openKin();
    void createVisualize();
};

#endif // STARTKINVISUALIZATIONWIDGET_HPP

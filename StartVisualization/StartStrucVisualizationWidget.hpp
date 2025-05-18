#ifndef STARTSTRUCVISUALIZATIONWIDGET_HPP
#define STARTSTRUCVISUALIZATIONWIDGET_HPP

#include "FileDeal/FileDeal.hpp"
#include "StructureVisualize/StructureVisualizeWidget.hpp"
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

class StartStrucVisualizationWidget : public QWidget
{
    Q_OBJECT
private:
    QLineEdit*    filePathEdit;
    QLineEdit*    strucPathEdit;
    QPushButton*  continueBtn;
    QPushButton*  cancelBtn;
    MML::Mat      struc;
public:
    explicit StartStrucVisualizationWidget(QWidget *parent = nullptr);
    ~StartStrucVisualizationWidget();
public slots:
    void openFile();
    void openStruc();
    void createVisualize();
};

#endif // STARTSTRUCVISUALIZATIONWIDGET_HPP

#ifndef STARTGWASVISUALIZATIONWIDGET_HPP
#define STARTGWASVISUALIZATIONWIDGET_HPP

#include "FileDeal/FileDeal.hpp"
#include "GFFSetting/SetGffReadAttribute.hpp"
#include "GWASVisualize/GWASVisualize.hpp"
#include <queue>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>

class StartGWASVisualizationLessWithLogP{
private:

    double* data;
public:
    StartGWASVisualizationLessWithLogP();
    StartGWASVisualizationLessWithLogP(double* inCompList);
    void setCompList(double* inCompList);
    bool operator () (size_t a,size_t b);
};

class StartGWASVisualizationWidget : public QWidget
{
    Q_OBJECT
private:
    QLineEdit*      lenInfoPathEdit;
    QLineEdit*      tsvPathEdit;
    QLineEdit*      gffPathEdit;
    QRadioButton*   tsvRadio;
    QRadioButton*   gffRadio;
    QRadioButton*   topRadio;
    QRadioButton*   threRadio;
    QPushButton*    openTsvBtn;
    QPushButton*    openGffBtn;
    QPushButton*    setGffReadBtn;
    QPushButton*    continueBtn;
    QPushButton*    cancelBtn;
    QLineEdit*      bimPathEdit;
    QLineEdit*      logPPathEdit;
    QSpinBox*       colIndexBox;
    QLabel*         openBimLabel;
    QLabel*         openLogPLabel;
    QLabel*         colLabel;
    QDoubleSpinBox* topFactorBox;
    QDoubleSpinBox* thresholdBox;
    FD::genomeRead  genomeReader;
public:
    explicit StartGWASVisualizationWidget(QWidget *parent = nullptr);
    ~StartGWASVisualizationWidget();
signals:

public slots:
    void openLenInfoFile();
    void openTsv();
    void openGff();
    void topRadioSelected();
    void threRadioSelected();
    void tsvRadioSelected();
    void gffRadioSelected();
    void openBimFile();
    void openLogPFile();
    void startVisualization();
    void createGffReadSetting();
};

#endif // STARTGWASVISUALIZATIONWIDGET_HPP

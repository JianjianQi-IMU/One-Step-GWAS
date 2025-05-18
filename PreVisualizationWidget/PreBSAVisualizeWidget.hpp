#ifndef _PREVISUALIZATIONWIDGET_PREBSAVISUALIZEWIDGET_HPP_
#define _PREVISUALIZATIONWIDGET_PREBSAVISUALIZEWIDGET_HPP_

#include "PreVisualizationWidgetHeader.hpp"

class PreBSAVisualizeWidget : public QWidget
{
    Q_OBJECT
public:
    QLineEdit*      tsvPathEdit;
    QLineEdit*      gffPathEdit;
    QRadioButton*   tsvRadio;
    QRadioButton*   gffRadio;
    QPushButton*    openTsvBtn;
    QPushButton*    openGffBtn;
    QPushButton*    setGffReadBtn;
    QPushButton*    continueBtn;
    QPushButton*    cancelBtn;
    FD::genomeRead  genomeReader;
    ThreadRun::QNormalBSAThread* thread;
    explicit PreBSAVisualizeWidget(QWidget *parent = nullptr);
    PreBSAVisualizeWidget(ThreadRun::QNormalBSAThread* inThread,QWidget *parent = nullptr);
    ~PreBSAVisualizeWidget();
public slots:
    void openTsv();
    void openGff();
    void tsvRadioSelected();
    void gffRadioSelected();
    void createVisualize();
    void createGffReadSetting();
};

#endif

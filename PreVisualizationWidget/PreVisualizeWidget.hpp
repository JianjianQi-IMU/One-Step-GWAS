#ifndef _PREVISUALIZATIONWIDGET_PREVISUALIZEWIDGET_HPP_
#define _PREVISUALIZATIONWIDGET_PREVISUALIZEWIDGET_HPP_

#include "PreVisualizationWidgetHeader.hpp"

class PreVisualizeWidget : public QWidget
{
    Q_OBJECT
public:
    QLineEdit*      filePathEdit;
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
    FD::BIMLogPData* bim;
    QDoubleSpinBox* topFactorBox;
    QDoubleSpinBox* thresholdBox;
    FD::genomeRead  genomeReader;
    explicit PreVisualizeWidget(QWidget *parent = nullptr);
    PreVisualizeWidget(FD::BIMLogPData* inBim,QWidget *parent = nullptr);
    ~PreVisualizeWidget();
public slots:
    void openFile();
    void openTsv();
    void openGff();
    void topRadioSelected();
    void threRadioSelected();
    void tsvRadioSelected();
    void gffRadioSelected();
    void createVisualize();
    void createGffReadSetting();
};

#endif

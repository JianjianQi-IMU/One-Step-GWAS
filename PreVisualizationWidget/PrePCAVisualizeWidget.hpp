#ifndef _PREVISUALIZATIONWIDGET_PREPCAVISUALIZEWIDGET_HPP_
#define _PREVISUALIZATIONWIDGET_PREPCAVISUALIZEWIDGET_HPP_

#include "PreVisualizationWidgetHeader.hpp"

class PrePCAVisualizeWidget : public QWidget
{
    Q_OBJECT
private:
    QLineEdit*    filePathEdit;
    QPushButton*  openBtn;
    QPushButton*  continueBtn;
    QPushButton*  cancelBtn;
    QRadioButton* D2Radio;
    QRadioButton* D3Radio;
    MML::Mat PCs;
    double PC1VarRatio;
    double PC2VarRatio;
    double PC3VarRatio;
    explicit PrePCAVisualizeWidget(QWidget *parent = nullptr);
public:
    PrePCAVisualizeWidget(MML::Mat* inPCs,
                          QWidget *parent = nullptr);
    ~PrePCAVisualizeWidget();
    bool setVarRatio(double inPC1VarRatio,
                     double inPC2VarRatio,
                     double inPC3VarRatio);
public slots:
    void openFile();
    void createVisualize();
};

#endif

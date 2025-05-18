#ifndef _PREVISUALIZATIONWIDGET_PREKINSHIPVISUALIZEWIDGET_HPP_
#define _PREVISUALIZATIONWIDGET_PREKINSHIPVISUALIZEWIDGET_HPP_

#include "PreVisualizationWidgetHeader.hpp"

class PreKinshipVisualizeWidget : public QWidget
{
    Q_OBJECT
private:
    QLineEdit*    filePathEdit;
    QPushButton*  openBtn;
    QPushButton*  continueBtn;
    QPushButton*  cancelBtn;
    MML::Mat     kin;
    explicit PreKinshipVisualizeWidget(QWidget *parent = nullptr);
public:
    PreKinshipVisualizeWidget(MML::Mat &inKin,
                            QWidget *parent = nullptr);
    ~PreKinshipVisualizeWidget();
public slots:
    void openFile();
    void createVisualize();
};

#endif

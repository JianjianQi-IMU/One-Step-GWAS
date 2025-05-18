#ifndef _PREVISUALIZATIONWIDGET_PRESTRUCVISUALIZEWIDGET_HPP_
#define _PREVISUALIZATIONWIDGET_PRESTRUCVISUALIZEWIDGET_HPP_

#include "PreVisualizationWidgetHeader.hpp"

class PreStrucVisualizeWidget : public QWidget
{
    Q_OBJECT
private:
    QLineEdit*    filePathEdit;
    QPushButton*  openBtn;
    QPushButton*  continueBtn;
    QPushButton*  cancelBtn;
    MML::Mat     struc;
    explicit PreStrucVisualizeWidget(QWidget *parent = nullptr);
public:
    PreStrucVisualizeWidget(MML::Mat &inStruc,
                            QWidget *parent = nullptr);
    ~PreStrucVisualizeWidget();
public slots:
    void openFile();
    void createVisualize();
};

#endif

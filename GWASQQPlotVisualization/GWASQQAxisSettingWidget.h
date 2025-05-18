#ifndef GWASQQAXISSETTINGWIDGET_H
#define GWASQQAXISSETTINGWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QDoubleSpinBox>

#include "GWASQQPlotWidget.h"

class GWASQQAxisSettingWidget : public QWidget
{
    Q_OBJECT
private:
    double          xLimits;
    double          yLimits;
    QPushButton*    resetBtn;
    QPushButton*    saveBtn;
    QPushButton*    cancelBtn;
    QLabel*         xMaxLabel;
    QLabel*         yMaxLabel;
    QDoubleSpinBox* xMaxValue;
    QDoubleSpinBox* yMaxValue;

    GWASQQPlotWidget* plotWidget;
public:
    explicit GWASQQAxisSettingWidget(GWASQQPlotWidget *parent);
    void setLimits(double inXLimits, double inYLimits);

signals:
    void sendAxisValue(double xMax, double yMax, bool isReset);

public slots:
    void do_reset();
    void do_save();
};

#endif // GWASQQAXISSETTINGWIDGET_H

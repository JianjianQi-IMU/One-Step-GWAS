#ifndef STRUCVISUALSETTINGSWIDGET_HPP
#define STRUCVISUALSETTINGSWIDGET_HPP

#include <QWidget>
#include <QFont>
#include <QFontDialog>
#include "StartVisualization/IndividualSearchWidget.hpp"

namespace Ui {
class StrucVisualSettingsWidget;
}

class StrucVisualSettingsWidget : public QWidget
{
    Q_OBJECT
private:
    QFont font;
    QSize frameSize;
public:
    explicit StrucVisualSettingsWidget(QWidget *parent = nullptr);
    ~StrucVisualSettingsWidget();
    bool setSampleName(char** inSampleName,size_t n);

    void setXMargin(int value);
    void setYMargin(int value);
    void setBarGap(int value);
    void setSizeVal(QSize size);

private slots:
    void on_barGapSlider_valueChanged(int value);

    void on_xMarginSlider_valueChanged(int value);

    void on_yMarginSlider_valueChanged(int value);

    void on_sizeSettingBtn_clicked();

signals:
    void sendBarGapChanged(double x);
    void sendXMarginChanged(double x);
    void sendYMarginChanged(double x);
    void sendSizeVal(QSize size);

private:
    Ui::StrucVisualSettingsWidget *ui;
};

#endif // STRUCVISUALSETTINGSWIDGET_HPP

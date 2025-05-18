#ifndef CHOOSEGENECOLORWIDGET_HPP
#define CHOOSEGENECOLORWIDGET_HPP

#include <QWidget>
#include <QColorDialog>
#include "GWASVisualize/LogPScatterWidget.h"

namespace Ui {
class ChooseGeneColorWidget;
}

class ChooseGeneColorWidget : public QWidget
{
    Q_OBJECT
private:
    explicit ChooseGeneColorWidget(QWidget *parent = nullptr);
public:
    QColor forwardGeneColor;
    QColor reverseGeneColor;
    QColor geneColor;
    LogPScatterVisualizeWidget* pLogPChart;
    explicit ChooseGeneColorWidget(LogPScatterVisualizeWidget* inLogPChart,QWidget *parent = nullptr);
    ~ChooseGeneColorWidget();

private slots:
    void on_forwardColorBtn_clicked();

    void on_reverseColorBtn_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_geneColorBtn_clicked();

private:
    Ui::ChooseGeneColorWidget *ui;
};

#endif // CHOOSEGENECOLORWIDGET_HPP

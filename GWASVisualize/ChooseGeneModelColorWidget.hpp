#ifndef CHOOSEGENEMODELCOLORWIDGET_HPP
#define CHOOSEGENEMODELCOLORWIDGET_HPP

#include <QWidget>
#include <QColorDialog>
#include "GWASVisualize/LogPScatterWidget.h"

namespace Ui {
class ChooseGeneModelColorWidget;
}

class ChooseGeneModelColorWidget : public QWidget
{
    Q_OBJECT
private:
    explicit ChooseGeneModelColorWidget(QWidget *parent = nullptr);
public:
    QColor UTR5Color;
    QColor UTR3Color;
    QColor CDSColor;
    LogPScatterVisualizeWidget* pLogPChart;

    explicit ChooseGeneModelColorWidget(LogPScatterVisualizeWidget *inLogPChart,
                                        QWidget *parent = nullptr);
    ~ChooseGeneModelColorWidget();

private slots:
    void on_CDSColorBtn_clicked();

    void on_UTR5ColorBtn_clicked();

    void on_UTR3ColorBtn_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::ChooseGeneModelColorWidget *ui;
};

#endif // CHOOSEGENEMODELCOLORWIDGET_HPP

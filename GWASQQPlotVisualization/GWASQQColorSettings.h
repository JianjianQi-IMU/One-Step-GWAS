#ifndef GWASQQCOLORSETTINGS_H
#define GWASQQCOLORSETTINGS_H

#include <QWidget>
#include "./GWASQQPlotWidget.h"

class GWASQQPointsColorChooseWidget : public QWidget
{
    Q_OBJECT
private:
    QColor colChoosed;
    GWASQQPlotWidget* plotWidget;
    QWidget* displayWidget;
public:
    explicit GWASQQPointsColorChooseWidget(QWidget *parent = nullptr);
    ~GWASQQPointsColorChooseWidget();
    void setPlotWidget(GWASQQPlotWidget* inWidget)
    {
        plotWidget = inWidget;
    }
    void setChoosedColor(const QColor& inCol);

public slots:
    void doChooseColor();
    void setColor();

signals:
};

class GWASQQDiagonalLineColorChooseWidget : public QWidget
{
    Q_OBJECT
private:
    QColor colChoosed;
    GWASQQPlotWidget* plotWidget;
    QWidget* displayWidget;
public:
    explicit GWASQQDiagonalLineColorChooseWidget(QWidget *parent = nullptr);
    ~GWASQQDiagonalLineColorChooseWidget();
    void setPlotWidget(GWASQQPlotWidget* inWidget)
    {
        plotWidget = inWidget;
    }
    void setChoosedColor(const QColor& inCol);

public slots:
    void doChooseColor();
    void setColor();

signals:
};

#endif // GWASQQCOLORSETTINGS_H

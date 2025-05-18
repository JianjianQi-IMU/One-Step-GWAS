#ifndef GWASVISUALIZESETTINGWIDGET_HPP
#define GWASVISUALIZESETTINGWIDGET_HPP

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class GWASVisualizeSettingPara
{
public:
    GWASVisualizeSettingPara();
};

class GWASVisualizeSettingWidget : public QWidget
{
    Q_OBJECT
private:
    QListWidget* pListWidget;

public:
    explicit GWASVisualizeSettingWidget(QWidget *parent = nullptr);

signals:

};

#endif // GWASVISUALIZESETTINGWIDGET_HPP

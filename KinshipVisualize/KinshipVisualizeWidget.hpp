#ifndef KINSHIPVISUALIZEWIDGET_HPP
#define KINSHIPVISUALIZEWIDGET_HPP

#include <QMainWindow>
#include "KinshipHeatmapWidget.hpp"
#include "KinshipRangeValColSettingWidget.hpp"

namespace Ui
{
class KinshipVisualizeWidget;
}

class KinshipVisualizeWidget : public QMainWindow
{
    Q_OBJECT
private:
    explicit KinshipVisualizeWidget(QWidget *parent = nullptr);
public:
    KinshipHeatmapWidget* pHeatmapPlot;
    KinshipVisualizeWidget(const MML::Mat& inKin, char** inName, QWidget *parent = nullptr);
    ~KinshipVisualizeWidget();

private:
    Ui::KinshipVisualizeWidget *ui;
public slots:
    void setClusterIndex();
    void setRangeValCol(KinshipRangeValColParam param);
private slots:
    void on_actionCluster_triggered();
    void on_actionSetRangeLabel_triggered();
    void on_actionExportPlot_triggered();
};

#endif // KINSHIPVISUALIZEWIDGET_HPP

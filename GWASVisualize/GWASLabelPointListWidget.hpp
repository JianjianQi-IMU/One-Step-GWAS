#ifndef GWASLABELPOINTLISTWIDGET_H
#define GWASLABELPOINTLISTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include "GWASVisualize/LabelPointsListView.hpp"

namespace Ui
{
class GWASLabelPointListWidget;
}

class GWASLabelPointListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GWASLabelPointListWidget(QWidget *parent = nullptr);
    ~GWASLabelPointListWidget();
    void setPoints(ValPoints* inPoints, long long inNPoints);
    bool setMaxLabelPointsNum(int n);
    bool setChrName(char** pName);


private:
    ValPoints* points;
    long long nPoints;

    LabelPointsListView* pLabelView;
    Ui::GWASLabelPointListWidget *ui;
signals:
    void removeLabelPoint(int row);
    void lookLabelPoint(int row);
    void removeAllLabelPoint();
public slots:
    bool addPoint(long long id);
    bool removePoint(int row);
};

#endif // GWASLABELPOINTLISTWIDGET_H

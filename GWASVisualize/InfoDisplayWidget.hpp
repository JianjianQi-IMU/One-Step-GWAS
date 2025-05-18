#ifndef INFODISPLAYWIDGET_HPP
#define INFODISPLAYWIDGET_HPP

#include <QTabWidget>
#include "GWASVisualize/GeneInfoWidget.hpp"
#include "GWASVisualize/PointInfoWidget.hpp"
#include "GWASVisualize/LogPScatterWidget.h"
#include "GWASVisualize/LabelPointsListView.hpp"

namespace Ui {
class InfoDisplayWidget;
}

class InfoDisplayWidget : public QTabWidget
{
    Q_OBJECT

public:
    GeneInfoWidget* pGeneWidget;
    PointInfoWidget* pPointWidget;
    GenomeAnnotation* genome;
    ValPoints* points;
    long long nPoints;
    explicit InfoDisplayWidget(QWidget *parent = nullptr);
    void setGenome(GenomeAnnotation* inGenome);
    void setPoints(ValPoints* inPoints,size_t inNPoints);
    void displayGeneInfo(long long id);
    void displayPointInfo(long long id,long long iGene1,long long iGene2);
    ~InfoDisplayWidget();

private:
    Ui::InfoDisplayWidget *ui;

};

#endif // INFODISPLAYWIDGET_HPP

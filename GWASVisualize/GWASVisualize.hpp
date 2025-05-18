#ifndef GWASVISUALIZE_HPP
#define GWASVISUALIZE_HPP

#include <QMainWindow>
#include <QSplitter>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QTabWidget>
#include "FileDeal/FileDeal.hpp"
#include "InfoDisplayWidget.hpp"
#include "StartVisualization/GeneFindList.hpp"
#include "StartVisualization/LoadGenomeSequenceWidget.hpp"
#include "GWASMainInfoWidget.hpp"
#include "LogPScatterWidget.h"
#include "ChooseGeneColorWidget.hpp"
#include "StartVisualization/ChooseColorListWidget.hpp"
#include "ChooseGeneModelColorWidget.hpp"
#include "GWASLabelPointListWidget.hpp"
#include "GWASSetThresholdWidget.hpp"
#include "GenomeAnnotation/GenomeAnnoSlideWidget.h"

namespace Ui {
class GWASVisualize;
}

class GWASVisualize : public QMainWindow
{
    Q_OBJECT
private:
    explicit GWASVisualize(QWidget *parent = nullptr);
    void initialization(long long nPoints);
    void setIntervalLenLabel(long double length);
    void setIntervalLenLabel(long long length);
public:
    QWidget*      areaSearchToolBar;
    QHBoxLayout*  areaSearchToolBarLayout;
    QComboBox*    leftChrChooseBox;
    QComboBox*    rightChrChooseBox;
    QLineEdit*    leftChrPosBox;
    QLineEdit*    rightChrPosBox;
    QPushButton*  searchBtn;
    QLabel*       intervalLenLabel;
    QPushButton*  leftMoveBtn;
    QPushButton*  rightMoveBtn;
    QRegExpValidator* posValidator;

    QTabWidget*   pLeftTabWidget;

    InfoDisplayWidget* pInfoWidget;
    GeneFindList* pFindWidget;
    GWASMainInfoWidget* pMainInfoWidget;
    LogPScatterVisualizeWidget* pLogPChart;
    GWASLabelPointListWidget* pLabelPointList;
    GenomeAnnoSlideWidget* pSlideWidget;
    GWASVisualize(long long *inChrLen,char** inChrName,int inNChr,
                  ValPoints *inPoints,long long inNPoints,
                  QWidget *parent = nullptr);
    GWASVisualize(const std::vector<long long>& inChrLen,
                  const std::vector<std::string>& inChrName,
                  ValPoints *inPoints,long long inNPoints,
                  QWidget *parent = nullptr);
    void createGenome();
    void setGenome(const GenomeAnnotation& inGenome);
    void doLoadGenomeSeq(QString filePath);
    void setPointsColor(const QVector<QColor>& inColor);
    ~GWASVisualize();

private slots:
    void searchArea();
    void setSearchArea();
    void on_actSaveSVG_triggered();

    void on_actionLoadGenomeSeq_triggered();

    void on_actionZoomIn_triggered();

    void on_actionZoomOut_triggered();

    void on_actionGridDisplay1_triggered(bool checked);

    void on_actionGridDisplay2_triggered(bool checked);

    void on_actionSwichGeneModel_triggered();

    void on_actionDisplayGene_triggered(bool checked);

    void on_actionDisplayGeneModel_triggered(bool checked);

    void on_actionDisplayGenome_triggered(bool checked);

    void on_actionChoosePointsColor_triggered();

    void on_actionSetGeneColor_triggered();

    void on_actionSetGeneModelColor_triggered();

    void on_actionSetThreshold_triggered();

    void on_actionDisplayChrLine_triggered(bool checked);

    void on_actionSetPointSize_triggered();

private:
    Ui::GWASVisualize *ui;
};

#endif // GWASVISUALIZE_HPP

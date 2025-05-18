#ifndef BSAVISUALIZE_HPP
#define BSAVISUALIZE_HPP

#include <QMainWindow>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include "BSAVisualize/BSAIndexScatterWidget.hpp"
#include "BSAVisualize/BSAMainInfoWidget.hpp"
#include "BSAVisualize/BSALabelInterListWidget.hpp"
#include "StartVisualization/GeneFindList.hpp"
#include "StartVisualization/LoadGenomeSequenceWidget.hpp"

namespace Ui {
class BSAVisualize;
}

class BSAVisualize : public QMainWindow
{
    Q_OBJECT
private:
    explicit BSAVisualize(QWidget *parent = nullptr);
public:
    QWidget*      areaSearchToolBar;
    QHBoxLayout*  areaSearchToolBarLayout;
    QComboBox*    leftChrChooseBox;
    QComboBox*    rightChrChooseBox;
    QLineEdit*    leftChrPosBox;
    QLineEdit*    rightChrPosBox;
    QPushButton*  searchBtn;
    QPushButton*  leftMoveBtn;
    QPushButton*  rightMoveBtn;
    QRegExpValidator* posValidator;

    QTabWidget*   pLeftTabWidget;

    GeneFindList* pFindWidget;
    BSAMainInfoWidget* pMainInfoWidget;
    BSAIndexScatterWidget* pIndexChart;
    BSALabelInterListWidget* pLabelInterList;
    BSAVisualize(size_t *inChrLen,char** inChrName,int inNChr,
                 FD::BSAInterVal *inInter,size_t inNInter,
                 ValPoints2* inMarkers,size_t inNMarkers,
                 QWidget *parent = nullptr);
    ~BSAVisualize();

    void createGenome();
    void setGenome(const GenomeAnnotation& inGenome);
    void doLoadGenomeSeq(QString filePath);
private:
    Ui::BSAVisualize *ui;

private slots:
    void searchArea();
    void setSearchArea();
    void on_actionLoadGenomeSeq_triggered();
    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();
    void on_actionGridDisplay1_triggered(bool checked);
    void on_actionGridDisplay2_triggered(bool checked);
    void on_actionDisplayGene_triggered(bool checked);
    void on_actionSwichGeneModel_triggered();
    void on_actionDisplayGeneModel_triggered(bool checked);
    void on_actionDisplayGenome_triggered(bool checked);
};

#endif // BSAVISUALIZE_HPP

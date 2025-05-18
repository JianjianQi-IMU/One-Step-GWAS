#include "GWASVisualize.hpp"
#include "ui_GWASVisualize.h"

#include "FileDeal/BaseFileSaveDialog.hpp"
#include "StyleChoose/SetPointSizeWidget.h"

GWASVisualize::GWASVisualize(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GWASVisualize)
{
    ui->setupUi(this);
    pInfoWidget=nullptr;
    pFindWidget=nullptr;
    pLogPChart=nullptr;

    areaSearchToolBar = new QWidget(this);
    QHBoxLayout* areaSearchToolBarLayout = new QHBoxLayout(areaSearchToolBar);
    leftChrChooseBox       = new QComboBox(areaSearchToolBar);
    rightChrChooseBox      = new QComboBox(areaSearchToolBar);
    leftChrPosBox          = new QLineEdit(areaSearchToolBar);
    rightChrPosBox         = new QLineEdit(areaSearchToolBar);
    QLabel* interTo        = new QLabel(" to ",areaSearchToolBar);
    QLabel* interFrom      = new QLabel("from ",areaSearchToolBar);
    searchBtn              = new QPushButton(areaSearchToolBar);
    leftMoveBtn            = new QPushButton(areaSearchToolBar);
    rightMoveBtn           = new QPushButton(areaSearchToolBar);
    QWidget* retainWidget  = new QWidget(areaSearchToolBar);
    QFrame* line1          = new QFrame(areaSearchToolBar);
    intervalLenLabel       = new QLabel(areaSearchToolBar);
    posValidator = new QRegExpValidator(QRegExp("[\\d,]*"), this);

    leftChrPosBox->setValidator(posValidator);
    rightChrPosBox->setValidator(posValidator);

    areaSearchToolBarLayout->setMargin(0);
    areaSearchToolBarLayout->addWidget(leftMoveBtn);
    areaSearchToolBarLayout->addWidget(rightMoveBtn);
    areaSearchToolBarLayout->addWidget(line1);
    areaSearchToolBarLayout->addWidget(interFrom);
    areaSearchToolBarLayout->addWidget(leftChrChooseBox);
    areaSearchToolBarLayout->addWidget(leftChrPosBox);
    areaSearchToolBarLayout->addWidget(interTo);
    areaSearchToolBarLayout->addWidget(rightChrChooseBox);
    areaSearchToolBarLayout->addWidget(rightChrPosBox);
    areaSearchToolBarLayout->addWidget(searchBtn);
    areaSearchToolBarLayout->addWidget(intervalLenLabel);
    areaSearchToolBarLayout->addWidget(retainWidget);


    line1->setFrameShape(QFrame::VLine);
    areaSearchToolBar->setFixedHeight(32);
    leftChrPosBox->setFixedWidth(160);
    rightChrPosBox->setFixedWidth(160);
    searchBtn->setFixedWidth(32);
    searchBtn->setText("Go");
    // searchBtn->setIcon(QIcon(":/icon/searchIcon3.png"));
    interTo->setFixedWidth(30);
    interFrom->setFixedWidth(40);
    intervalLenLabel->setFixedWidth(80);
    leftChrChooseBox->setMaximumWidth(100);
    rightChrChooseBox->setMaximumWidth(100);
    leftMoveBtn->setFixedWidth(64);
    rightMoveBtn->setFixedWidth(64);
    leftMoveBtn->setIcon(QIcon(":/icon/leftMoveIcon.png"));
    rightMoveBtn->setIcon(QIcon(":/icon/rightMoveIcon.png"));

    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Manhattan Plot");
}

void GWASVisualize::initialization(long long nPoints)
{
    pLeftTabWidget=new QTabWidget(this);
    pLeftTabWidget->addTab(pMainInfoWidget,"Main");
    pLeftTabWidget->addTab(pFindWidget,"Find");
    pLeftTabWidget->addTab(pLabelPointList,"Label");

    pInfoWidget->setPoints(pLogPChart->getPoints(),pLogPChart->getNPoints());

    pLabelPointList->setPoints(pLogPChart->getPoints(),pLogPChart->getNPoints());
    pLabelPointList->setMaxLabelPointsNum(pLogPChart->getMaxLabelPointsNum());
    pLabelPointList->setChrName(pLogPChart->getChrName());

    pMainInfoWidget->setMax(QString::number(pLogPChart->getMaxLogP(),'f',2));
    pMainInfoWidget->setNumber(QString::number(nPoints));
    pMainInfoWidget->setThreshold(QString::number(pLogPChart->getMinLogP(),'f',2));

    pSlideWidget -> setFixedHeight(32);

    connect(pFindWidget,&GeneFindList::sendLookGenei,pLogPChart,&LogPScatterVisualizeWidget::lookGene);
    connect(pLogPChart,&LogPScatterVisualizeWidget::pointSelected,pInfoWidget,&InfoDisplayWidget::displayPointInfo);
    connect(pLogPChart,&LogPScatterVisualizeWidget::geneSelected,pInfoWidget,&InfoDisplayWidget::displayGeneInfo);
    connect(pLogPChart,&LogPScatterVisualizeWidget::currentIntervalChanged,this,&GWASVisualize::setSearchArea);
    connect(leftMoveBtn,&QPushButton::clicked,pLogPChart,&LogPScatterVisualizeWidget::moveLeft);
    connect(rightMoveBtn,&QPushButton::clicked,pLogPChart,&LogPScatterVisualizeWidget::moveRight);
    connect(searchBtn,&QPushButton::clicked,this,&GWASVisualize::searchArea);
    connect(pLogPChart,&LogPScatterVisualizeWidget::addLabelPoint,
            pLabelPointList,&GWASLabelPointListWidget::addPoint);
    connect(pLogPChart,&LogPScatterVisualizeWidget::removeLabelPoint,
            pLabelPointList,&GWASLabelPointListWidget::removePoint);
    connect(pLabelPointList,&GWASLabelPointListWidget::removeLabelPoint,
            pLogPChart,&LogPScatterVisualizeWidget::doRemoveLabelPoint);
    connect(pLabelPointList,&GWASLabelPointListWidget::lookLabelPoint,
            pLogPChart,&LogPScatterVisualizeWidget::doLookLabelPoint);
    connect(pLabelPointList,&GWASLabelPointListWidget::removeAllLabelPoint,
            pLogPChart,&LogPScatterVisualizeWidget::doRemoveAllLabelPoint);

    connect(pSlideWidget,&GenomeAnnoSlideWidget::sendCurrentPosChanged,
            pLogPChart,QOverload<long double,long double>::of(&LogPScatterVisualizeWidget::lookInterval));
    connect(pLogPChart,&LogPScatterVisualizeWidget::currentIntervalChanged,
            pSlideWidget,&GenomeAnnoSlideWidget::setCurrentPos);


    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    QSplitter* mainSplitter=new QSplitter(this);
    mainSplitter->setOrientation(Qt::Horizontal);
    mainSplitter->addWidget(pLeftTabWidget);
    mainSplitter->addWidget(pLogPChart);
    mainSplitter->addWidget(pInfoWidget);
    mainLayout->addWidget(areaSearchToolBar);
    mainLayout->addWidget(pSlideWidget);
    mainLayout->addWidget(mainSplitter);

    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 6);
    mainSplitter->setStretchFactor(2, 1);
    setCentralWidget(mainWidget);

    resize(1200,600);
}

void GWASVisualize::setIntervalLenLabel(long double length)
{
    QString valueStr;
    if(length >= 1000000){
        valueStr = "(" + QString::number(length*1e-6,10,2) + " Mb)";
    } else if(length >= 1000) {
        valueStr = "(" + QString::number(length*1e-3,10,2) + " Kb)";
    } else {
        valueStr = "(" + QString::number(length,10,0) + " bp)";
    }

    intervalLenLabel->setText(valueStr);
}

void GWASVisualize::setIntervalLenLabel(long long length)
{
    QString valueStr;
    if(length >= 1000000){
        valueStr = "(" + QString::number(length*1e-6,10,2) + " Mb)";
    } else if(length >= 1000) {
        valueStr = "(" + QString::number(length*1e-3,10,2) + " Kb)";
    } else {
        valueStr = "(" + QString::number(length,10,0) + " bp)";
    }

    intervalLenLabel->setText(valueStr);
}

GWASVisualize::GWASVisualize(long long *inChrLen, char **inChrName, int inNChr, ValPoints *inPoints, long long inNPoints, QWidget *parent)
    :GWASVisualize(parent)
{
    pLogPChart = new LogPScatterVisualizeWidget(inChrLen,inChrName,inNChr,inPoints,inNPoints,this);
    pInfoWidget = new InfoDisplayWidget(this);
    pMainInfoWidget = new GWASMainInfoWidget(this);
    pFindWidget = new GeneFindList(this);
    pLabelPointList = new GWASLabelPointListWidget(this);
    pSlideWidget = new GenomeAnnoSlideWidget(this);

    pSlideWidget -> loadChromosome(inChrLen, inChrName, inNChr);

    for(int i=0;i<inNChr;++i){
        leftChrChooseBox->addItem(inChrName[i]);
        rightChrChooseBox->addItem(inChrName[i]);
    }

    initialization(inNPoints);
}

GWASVisualize::GWASVisualize(const std::vector<long long> &inChrLen, const std::vector<std::string> &inChrName, ValPoints *inPoints, long long inNPoints, QWidget *parent)
    :GWASVisualize(parent)
{
    int inNChr = inChrLen.size();

    pLogPChart = new LogPScatterVisualizeWidget(inChrLen,inChrName,inPoints,inNPoints,this);
    pInfoWidget = new InfoDisplayWidget(this);
    pMainInfoWidget = new GWASMainInfoWidget(this);
    pFindWidget = new GeneFindList(this);
    pLabelPointList = new GWASLabelPointListWidget(this);
    pSlideWidget = new GenomeAnnoSlideWidget(this);

    pSlideWidget -> loadChromosome(inChrLen, inChrName);

    for(int i=0;i<inNChr;++i){
        leftChrChooseBox->addItem(inChrName[i].c_str());
        rightChrChooseBox->addItem(inChrName[i].c_str());
    }

    initialization(inNPoints);
}

void GWASVisualize::createGenome()
{
    pLogPChart->createGenome();
}

void GWASVisualize::setGenome(const GenomeAnnotation &inGenome)
{
    pLogPChart->setGenome(inGenome);

    pInfoWidget->setGenome(pLogPChart->getGenome());
    pFindWidget->setGenome(pLogPChart->getGenome());


}

void GWASVisualize::doLoadGenomeSeq(QString filePath)
{
    if(QFile::exists(filePath)){
        if(pLogPChart->loadGenomeSeq(filePath.toLatin1().data())){
            //close();
        }
        else{
            QMessageBox::critical(this,"error!","error reading file.");
        }
    }
}

void GWASVisualize::setPointsColor(const QVector<QColor> &inColor)
{
    if(pLogPChart != nullptr){
        pLogPChart -> setPointsColor(inColor);
    }
    // if(pSlideWidget != nullptr){
    //     pSlideWidget -> setColorListBar(inColor);
    // }
}

GWASVisualize::~GWASVisualize()
{
    delete ui;
}

void GWASVisualize::searchArea()
{
    int idChr1=leftChrChooseBox->currentIndex();
    int idChr2=rightChrChooseBox->currentIndex();
    QString leftString,rightString;
    long long leftVal,rightVal;
    if(leftChrPosBox->text().isEmpty()||rightChrPosBox->text().isEmpty()){
        setSearchArea();
        return;
    }
    leftString=leftChrPosBox->text(),rightString=rightChrPosBox->text();
    leftString.remove(','),rightString.remove(',');
    leftVal=leftString.toLongLong();
    rightVal=rightString.toLongLong();
    if(!leftVal||!rightVal) return ;
    pLogPChart->lookInterval(idChr1,idChr2,leftVal,rightVal);
}

void GWASVisualize::setSearchArea()
{
    int idChr1,idChr2;
    long double leftPos,rightPos;
    long double cumLeftPos,cumRightPos;
    long long leftVal,rightVal,interLen;
    QString leftString,rightString;
    pLogPChart->getCurrentInterval(idChr1,leftPos,idChr2,rightPos);
    pLogPChart->getCurrentInterval(cumLeftPos,cumRightPos);
    leftVal=leftPos+0.5,rightVal=rightPos+0.5;
    interLen = cumRightPos - cumLeftPos + 1;
    leftString=QString::number(leftVal);
    rightString=QString::number(rightVal);
    for(int i=leftString.length()-3;i>0;i-=3){
        leftString.insert(i,',');
    }
    for(int i=rightString.length()-3;i>0;i-=3){
        rightString.insert(i,',');
    }
    leftChrChooseBox->setCurrentIndex(idChr1);
    rightChrChooseBox->setCurrentIndex(idChr2);
    leftChrPosBox->setText(leftString);
    rightChrPosBox->setText(rightString);
    setIntervalLenLabel(interLen);
}

void GWASVisualize::on_actSaveSVG_triggered()
{
    FD::PlotFileSaveDialog* pWidget = new FD::PlotFileSaveDialog;
    pWidget -> setAttribute(Qt::WA_ShowModal, true);
    pWidget -> setSize(pLogPChart -> size());
    connect(pWidget,&FD::PlotFileSaveDialog::sendSaveFile,
            pLogPChart,&LogPScatterVisualizeWidget::savePlot);
    pWidget -> show();
}

void GWASVisualize::on_actionLoadGenomeSeq_triggered()
{
    LoadGenomeSequenceWidget* pWidget = new LoadGenomeSequenceWidget();
    pWidget->setAttribute(Qt::WA_ShowModal,true);
    connect(pWidget,&LoadGenomeSequenceWidget::sendFilePath,
           this,&GWASVisualize::doLoadGenomeSeq);
    pWidget->show();
}

void GWASVisualize::on_actionZoomIn_triggered()
{
    pLogPChart->zoomIn();
}

void GWASVisualize::on_actionZoomOut_triggered()
{
    pLogPChart->zoomOut();
}

void GWASVisualize::on_actionGridDisplay1_triggered(bool checked)
{
    pLogPChart->setDisplayMinorGridline(checked);
}

void GWASVisualize::on_actionGridDisplay2_triggered(bool checked)
{
    pLogPChart->setDisplayMajorGridline(checked);
}

void GWASVisualize::on_actionSwichGeneModel_triggered()
{
    pLogPChart->switchGeneDisplayMode();
}

void GWASVisualize::on_actionDisplayGene_triggered(bool checked)
{
    pLogPChart->setDisplayGene(checked);
}

void GWASVisualize::on_actionDisplayGeneModel_triggered(bool checked)
{
    pLogPChart->setDisplayGeneModel(checked);
}

void GWASVisualize::on_actionDisplayGenome_triggered(bool checked)
{
    pLogPChart->setDisplayGenome(checked);
}

void GWASVisualize::on_actionChoosePointsColor_triggered()
{
    ChooseColorListWidget* pWidget = new ChooseColorListWidget(pLogPChart->getPointsColor(),2,1);
    pWidget -> setIsSilentTouchMaximun(false);
    connect(pWidget,&ChooseColorListWidget::sendColor,
            this,&GWASVisualize::setPointsColor);
    pWidget->setAttribute(Qt::WA_DeleteOnClose,true);
    pWidget->setAttribute(Qt::WA_ShowModal,true);
    pWidget->show();
}

void GWASVisualize::on_actionSetGeneColor_triggered()
{
    ChooseGeneColorWidget* pWidget = new ChooseGeneColorWidget(pLogPChart);
    pWidget->show();
}

void GWASVisualize::on_actionSetGeneModelColor_triggered()
{
    ChooseGeneModelColorWidget* pWidget = new ChooseGeneModelColorWidget(pLogPChart);
    pWidget->show();
}


void GWASVisualize::on_actionSetThreshold_triggered()
{
    LineStyleParam tParam;
    float  val;
    pLogPChart->getThresholdLine(val,tParam);
    GWASSetThresholdWidget* pWidget = new GWASSetThresholdWidget();
    pWidget -> setAttribute(Qt::WA_ShowModal,true);
    if(val >= 0)
    {
        pWidget -> setLineStyleParam(tParam);
    }
    connect(pWidget,&GWASSetThresholdWidget::sendThresholdLine,
            pLogPChart,&LogPScatterVisualizeWidget::setThresholdLine);
    pWidget -> show();
}


void GWASVisualize::on_actionDisplayChrLine_triggered(bool checked)
{
    pLogPChart->setDisplayChrGridLine(checked);
}


void GWASVisualize::on_actionSetPointSize_triggered()
{
    SetPointSizeWidget* pWidget = new SetPointSizeWidget(pLogPChart->getPointSize());
    connect(pWidget, &SetPointSizeWidget::sendPointSize,
            pLogPChart, &LogPScatterVisualizeWidget::setPointSize);
    pWidget -> show();
}


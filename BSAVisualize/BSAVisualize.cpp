#include "BSAVisualize.hpp"
#include "ui_BSAVisualize.h"

BSAVisualize::BSAVisualize(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BSAVisualize)
{
    ui->setupUi(this);

    pFindWidget=nullptr;
    pIndexChart=nullptr;

    areaSearchToolBar = new QWidget(this);
    QHBoxLayout* areaSearchToolBarLayout = new QHBoxLayout(areaSearchToolBar);
    leftChrChooseBox       = new QComboBox(areaSearchToolBar);
    rightChrChooseBox      = new QComboBox(areaSearchToolBar);
    leftChrPosBox          = new QLineEdit(areaSearchToolBar);
    rightChrPosBox         = new QLineEdit(areaSearchToolBar);
    QLabel* fromTo         = new QLabel("..",areaSearchToolBar);
    searchBtn              = new QPushButton(areaSearchToolBar);
    leftMoveBtn            = new QPushButton(areaSearchToolBar);
    rightMoveBtn           = new QPushButton(areaSearchToolBar);
    QWidget* retainWidget  = new QWidget(areaSearchToolBar);
    QFrame* line1          = new QFrame(areaSearchToolBar);
    posValidator = new QRegExpValidator(QRegExp("[\\d,]*"), this);

    leftChrPosBox->setValidator(posValidator);
    rightChrPosBox->setValidator(posValidator);

    areaSearchToolBarLayout->setMargin(0);
    areaSearchToolBarLayout->addWidget(leftMoveBtn);
    areaSearchToolBarLayout->addWidget(rightMoveBtn);
    areaSearchToolBarLayout->addWidget(line1);
    areaSearchToolBarLayout->addWidget(leftChrChooseBox);
    areaSearchToolBarLayout->addWidget(leftChrPosBox);
    areaSearchToolBarLayout->addWidget(fromTo);
    areaSearchToolBarLayout->addWidget(rightChrChooseBox);
    areaSearchToolBarLayout->addWidget(rightChrPosBox);
    areaSearchToolBarLayout->addWidget(searchBtn);
    areaSearchToolBarLayout->addWidget(retainWidget);

    line1->setFrameShape(QFrame::VLine);
    areaSearchToolBar->setFixedHeight(32);
    leftChrPosBox->setFixedWidth(160);
    rightChrPosBox->setFixedWidth(160);
    searchBtn->setFixedWidth(25);
    searchBtn->setIcon(QIcon(":/icon/searchIcon3.png"));
    fromTo->setFixedWidth(20);
    leftChrChooseBox->setMaximumWidth(100);
    rightChrChooseBox->setMaximumWidth(100);
    leftMoveBtn->setFixedWidth(64);
    rightMoveBtn->setFixedWidth(64);
    leftMoveBtn->setIcon(QIcon(":/icon/leftMoveIcon.png"));
    rightMoveBtn->setIcon(QIcon(":/icon/rightMoveIcon.png"));
}

BSAVisualize::BSAVisualize(size_t *inChrLen, char **inChrName, int inNChr, FD::BSAInterVal *inInter, size_t inNInter,ValPoints2* inMarkers,size_t inNMarkers, QWidget *parent)
    :BSAVisualize(parent)
{
    pIndexChart=new BSAIndexScatterWidget(inChrLen,inChrName,inNChr,inInter,inNInter,inMarkers,inNMarkers,this);
//    pInfoWidget=new InfoDisplayWidget(this);
    pMainInfoWidget=new BSAMainInfoWidget(this);
    pFindWidget=new GeneFindList(this);
    pLabelInterList = new BSALabelInterListWidget(this);

    pLeftTabWidget=new QTabWidget(this);
    pLeftTabWidget->addTab(pMainInfoWidget,"Main");
    pLeftTabWidget->addTab(pFindWidget,"Find");
    pLeftTabWidget->addTab(pLabelInterList,"Label");
//    pInfoWidget->setPoints(pLogPChart->getPoints(),pLogPChart->getNPoints());

    pLabelInterList->setInter(pIndexChart->getPoints(),pIndexChart->getNPoints());
    pLabelInterList->setMaxLabelInterNum(pIndexChart->getMaxLabelPointsNum());
    pLabelInterList->setChrName(pIndexChart->getChrName());

//    pMainInfoWidget->setMax(QString::number(pLogPChart->getMaxLogP(),'f',2));
//    pMainInfoWidget->setNumber(QString::number(inNPoints));
//    pMainInfoWidget->setThreshold(QString::number(pLogPChart->getMinLogP(),'f',2));
    connect(pFindWidget,&GeneFindList::sendLookGenei,pIndexChart,&BSAIndexScatterWidget::lookGene);
//    connect(pLogPChart,&LogPScatterVisualizeWidget::pointSelected,pInfoWidget,&InfoDisplayWidget::displayPointInfo);
//    connect(pLogPChart,&LogPScatterVisualizeWidget::geneSelected,pInfoWidget,&InfoDisplayWidget::displayGeneInfo);
    connect(pIndexChart,&BSAIndexScatterWidget::currentIntervalChanged,this,&BSAVisualize::setSearchArea);
    connect(leftMoveBtn,&QPushButton::clicked,pIndexChart,&BSAIndexScatterWidget::moveLeft);
    connect(rightMoveBtn,&QPushButton::clicked,pIndexChart,&BSAIndexScatterWidget::moveRight);
    connect(searchBtn,&QPushButton::clicked,this,&BSAVisualize::searchArea);
    connect(pIndexChart,&BSAIndexScatterWidget::addLabelPoint,
            pLabelInterList,&BSALabelInterListWidget::addInter);
    connect(pIndexChart,&BSAIndexScatterWidget::removeLabelPoint,
            pLabelInterList,&BSALabelInterListWidget::removeInter);
    connect(pLabelInterList,&BSALabelInterListWidget::removeLabelInter,
            pIndexChart,&BSAIndexScatterWidget::doRemoveLabelPoint);
    connect(pLabelInterList,&BSALabelInterListWidget::lookLabelInter,
            pIndexChart,&BSAIndexScatterWidget::doLookLabelPoint);
    connect(pLabelInterList,&BSALabelInterListWidget::removeAllLabelInter,
            pIndexChart,&BSAIndexScatterWidget::doRemoveAllLabelPoint);

    for(int i=0;i<inNChr;++i){
        leftChrChooseBox->addItem(inChrName[i]);
        rightChrChooseBox->addItem(inChrName[i]);
    }

    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    QSplitter* mainSplitter=new QSplitter(this);
    mainSplitter->setOrientation(Qt::Horizontal);
    mainSplitter->addWidget(pLeftTabWidget);
    mainSplitter->addWidget(pIndexChart);
//    mainSplitter->addWidget(pInfoWidget);
    mainLayout->addWidget(areaSearchToolBar);
    mainLayout->addWidget(mainSplitter);
    setCentralWidget(mainWidget);
}

BSAVisualize::~BSAVisualize()
{
    delete ui;
}

void BSAVisualize::createGenome()
{
    pIndexChart->createGenome();
}

void BSAVisualize::setGenome(const GenomeAnnotation &inGenome)
{
    pIndexChart->setGenome(inGenome);

//    pInfoWidget->setGenome(pLogPChart->getGenome());
    pFindWidget->setGenome(pIndexChart->getGenome());
}


void BSAVisualize::doLoadGenomeSeq(QString filePath)
{
    if(QFile::exists(filePath)){
        if(pIndexChart->loadGenomeSeq(filePath.toLatin1().data())){
            close();
        }
        else{
            QMessageBox::critical(this,"error!","error reading file.");
        }
    }
}


void BSAVisualize::searchArea()
{
    int idChr1=leftChrChooseBox->currentIndex();
    int idChr2=rightChrChooseBox->currentIndex();
    QString leftString,rightString;
    long long leftVal,rightVal;
    if(leftChrPosBox->text().isEmpty()||rightChrPosBox->text().isEmpty()) return;
    leftString=leftChrPosBox->text(),rightString=rightChrPosBox->text();
    leftString.remove(','),rightString.remove(',');
    leftVal=leftString.toLongLong();
    rightVal=rightString.toLongLong();
    if(!leftVal||!rightVal) return ;
    pIndexChart->lookInterval(idChr1,idChr2,leftVal,rightVal);
}

void BSAVisualize::setSearchArea()
{
    int idChr1,idChr2;
    long double leftPos,rightPos;
    long long leftVal,rightVal;
    QString leftString,rightString;
    pIndexChart->getCurrentInterval(idChr1,leftPos,idChr2,rightPos);
    leftVal=leftPos+0.5,rightVal=rightPos+0.5;
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
}

void BSAVisualize::on_actionLoadGenomeSeq_triggered()
{
    LoadGenomeSequenceWidget* pWidget = new LoadGenomeSequenceWidget();
    pWidget->setAttribute(Qt::WA_ShowModal,true);
    connect(pWidget,&LoadGenomeSequenceWidget::sendFilePath,
           this,&BSAVisualize::doLoadGenomeSeq);
    pWidget->show();
}


void BSAVisualize::on_actionZoomIn_triggered()
{
    pIndexChart->zoomIn();
}


void BSAVisualize::on_actionZoomOut_triggered()
{
    pIndexChart->zoomOut();
}


void BSAVisualize::on_actionGridDisplay1_triggered(bool checked)
{
    pIndexChart->setDisplayMinorGridline(checked);
}


void BSAVisualize::on_actionGridDisplay2_triggered(bool checked)
{
    pIndexChart->setDisplayMajorGridline(checked);
}


void BSAVisualize::on_actionDisplayGene_triggered(bool checked)
{
    pIndexChart->setDisplayGene(checked);
}


void BSAVisualize::on_actionSwichGeneModel_triggered()
{
    pIndexChart->switchGeneDisplayMode();
}


void BSAVisualize::on_actionDisplayGeneModel_triggered(bool checked)
{
    pIndexChart->setDisplayGeneModel(checked);
}


void BSAVisualize::on_actionDisplayGenome_triggered(bool checked)
{
    pIndexChart->setDisplayGenome(checked);
}


#include "StartGWASQQVisualizationWidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>

StartGWASQQVisualizationWidget::StartGWASQQVisualizationWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle("Load QQ Plot File");

    continueBtn   = new QPushButton("continue",this);
    cancelBtn     = new QPushButton("cancel",this);
    lenInfoPathEdit = new QLineEdit(this);
    bimPathEdit   = new QLineEdit(this);
    logPPathEdit  = new QLineEdit(this);
    colIndexBox   = new QSpinBox(this);
    QLabel* openBimLabel  = new QLabel("choose markers annotation file (.bim):",this);
    QLabel* openLogPLabel = new QLabel("choose GWAS output file:",this);
    QLabel* colLabel      = new QLabel("input -log P colnum:",this);
    QLabel* lenInfoLabel  = new QLabel("choose chromosome length annotation file:",this);
    QPushButton* openBimBtn    = new QPushButton(this);
    QPushButton* openLogPBtn   = new QPushButton(this);
    QPushButton* openLenInfoBtn=new QPushButton(this);

    QGroupBox* basicBox=new QGroupBox("Basic",this);
    QVBoxLayout* basicBoxLayout=new QVBoxLayout(basicBox);

    QWidget* bimWidget    = new QWidget(this);
    QWidget* logPWidget   = new QWidget(this);
    QWidget* colWidget    = new QWidget(this);
    QWidget* buttomWidget = new QWidget(this);
    QVBoxLayout* mainLayout=new QVBoxLayout(this);
    QWidget* lenInfoWidget=new QWidget(this);
    QHBoxLayout* lenInfoLayout=new QHBoxLayout(lenInfoWidget);
    QHBoxLayout* bimLayout    = new QHBoxLayout(bimWidget);
    QHBoxLayout* logPLayout   = new QHBoxLayout(logPWidget);
    QHBoxLayout* colLayout    = new QHBoxLayout(colWidget);
    QHBoxLayout* buttomLayout = new QHBoxLayout(buttomWidget);

    lenInfoLayout->addWidget(lenInfoPathEdit);
    lenInfoLayout->addWidget(openLenInfoBtn);
    bimLayout->addWidget(bimPathEdit);
    bimLayout->addWidget(openBimBtn);
    logPLayout->addWidget(logPPathEdit);
    logPLayout->addWidget(openLogPBtn);
    colLayout->addWidget(colLabel);
    colLayout->addWidget(colIndexBox);
    buttomLayout->addWidget(continueBtn);
    buttomLayout->addWidget(cancelBtn);
    basicBoxLayout->addWidget(lenInfoLabel);
    basicBoxLayout->addWidget(lenInfoWidget);
    basicBoxLayout->addWidget(openLogPLabel);
    basicBoxLayout->addWidget(logPWidget);
    basicBoxLayout->addWidget(openBimLabel);
    basicBoxLayout->addWidget(bimWidget);
    basicBoxLayout->addWidget(colWidget);
    mainLayout->addWidget(basicBox);
    mainLayout->addWidget(buttomWidget);

    connect(openLenInfoBtn ,&QPushButton::clicked,this ,&StartGWASQQVisualizationWidget::openLenInfoFile);
    connect(continueBtn    ,&QPushButton::clicked,this ,&StartGWASQQVisualizationWidget::startVisualization);
    connect(cancelBtn      ,&QPushButton::clicked,this ,&StartGWASQQVisualizationWidget::close);
    connect(openBimBtn     ,&QPushButton::clicked,this ,&StartGWASQQVisualizationWidget::openBimFile);
    connect(openLogPBtn    ,&QPushButton::clicked,this ,&StartGWASQQVisualizationWidget::openLogPFile);

    continueBtn->setFixedWidth(80);
    cancelBtn  ->setFixedWidth(80);

    openLenInfoBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openBimBtn ->setIcon(QIcon(":/icon/openIcon.png"));
    openLogPBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openLenInfoBtn->setFixedWidth(25);
    openBimBtn ->setFixedWidth(25);
    openLogPBtn->setFixedWidth(25);
    colIndexBox->setMaximum(100);
    colIndexBox->setMinimum(1);
    colIndexBox->setSingleStep(1);
    colIndexBox->setValue(4);

}

void StartGWASQQVisualizationWidget::openBimFile()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose markers annotation file (.bim)");
    if(!filePath.isEmpty()) bimPathEdit->setText(filePath);
}

void StartGWASQQVisualizationWidget::openLogPFile()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose GWAS output file");
    if(!filePath.isEmpty()) logPPathEdit->setText(filePath);
}

void StartGWASQQVisualizationWidget::openLenInfoFile()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose chromosome length annotation file.");
    if(!filePath.isEmpty()) lenInfoPathEdit->setText(filePath);
}

void StartGWASQQVisualizationWidget::startVisualization()
{
    FD::GWASOutread GWASreader;
    FD::BIMread BIMreader;
    FD::BIMData bimData;
    std::vector<std::string> chrNameVec;
    std::vector<long long> chrLenVec;
    std::map<std::string, int> chrNameLenMap;
    ValPoints* outPoints=nullptr;
    FD::BIMNode* bimList=nullptr;
    double* GWASLogP=nullptr;
    long long nOutPoints=0,i=0,tpqid,nBimChr=0,nBimPoints=0;
    char** bimChrName=nullptr;
    FD::ChrLenReadTsvState chrLenState;
    bool errorFlag=false;
    GWASQQVisualizationWidget* pVisualization=nullptr;
    // read bim file
    if(!QFile::exists(bimPathEdit->text())){
        QMessageBox::critical(this,"error","the .bim file don\'t exist.");
        return;
    }
    if(!QFile::exists(logPPathEdit->text())){
        QMessageBox::critical(this,"error","the -log P file don\'t exist.");
        return;
    }
    qDebug() << "step 1";
    // read gwas file
    GWASreader.loadFile(logPPathEdit->text().toLatin1().data());
    GWASreader.setLogCol(colIndexBox->value()-1);
    if(!GWASreader.readAll()){
        QMessageBox::critical(this,"error","reading -log P file error.");
        return;
    }
    BIMreader.load(bimPathEdit->text().toLatin1().data());
    if(!BIMreader.readAll(bimData)){
        QMessageBox::critical(this,"error","reading .bim file error.");
        return;
    }
    qDebug() << "step 2";
    // if the number of markers of bim is the number of markers of gwas
    if(bimData.nMarker != GWASreader.nMarkers){
        QMessageBox::critical(this,"error","the number of markers in .bim file isn\'t equal to GWAS output file.");
        return;
    }
    // QQ plot
    qDebug() << "step 3";
    if(!QFile::exists(lenInfoPathEdit->text())){
        QMessageBox::critical(this,"error","the chromosome length file don\'t exist.");
        return;
    }
    nBimPoints=bimData.nMarker;
    nBimChr=bimData.getNChr();
    bimList=bimData.data;
    bimChrName=bimData.getChrName();
    GWASLogP = new double[nBimPoints];
    for(i=0;i<nBimPoints;++i){
        GWASLogP[i]=-std::log10(GWASreader.getValueAt(i));
        if(GWASLogP[i]<0.0) GWASLogP[i]=0.0;
    }

    nOutPoints = nBimPoints;
    outPoints=new ValPoints[nOutPoints];
    for(i=0;i<nBimPoints;++i){
        outPoints[i].idchr=bimList[i].idChr;
        outPoints[i].pos  =bimList[i].pos;
        outPoints[i].val  =GWASLogP[i];
    }
    if((chrLenState = FD::chrLenReadTsv(lenInfoPathEdit->text().toLatin1().data(),chrNameVec,chrLenVec))
        != FD::CHRLENREADTSV_OK){
        if(outPoints) delete [] outPoints;
        if(GWASLogP) delete [] GWASLogP;
        switch(chrLenState){
        case FD::CHRLENREADTSV_ERRORCOLNUM:
            QMessageBox::critical(this,"error","Colnum is error in the chromosome length annotation file");
            break;
        case FD::CHRLENREADTSV_FILEPATHNULL:
            QMessageBox::critical(this,"error","Can't find the chromosome length annotation file");
            break;
        case FD::CHRLENREADTSV_REPEATCHR:
            QMessageBox::critical(this,"error","There are repetitive chromosome in chromosome length annotation file");
            break;
        default:
            QMessageBox::critical(this,"error","chromosome length annotation file reading error");
            break;
        }
        return;
    }
    for(i = 0;i < chrNameVec.size();++i){
        chrNameLenMap[chrNameVec[i]] = i;
    }
    qDebug() << "step 4";
    for(i=0;i<nBimChr;++i){
        if(chrNameLenMap.find(bimChrName[i]) == chrNameLenMap.end()){
            errorFlag=true;
            break;
        }
    }
    if(errorFlag){
        if(outPoints) delete [] outPoints;
        if(GWASLogP) delete [] GWASLogP;
        QMessageBox::critical(this,"error","some chromosomes can not be found in chromosome length annotation file");
        return;
    }
    for(i=0;i<nOutPoints;++i){
        outPoints[i].idchr = chrNameLenMap[bimChrName[outPoints[i].idchr]];
    }
    qDebug() << "step 5";
    pVisualization = new GWASQQVisualizationWidget;
    qDebug() << "step 6";
    pVisualization -> loadChromosome(chrLenVec, chrNameVec);
    qDebug() << "step 7";
    pVisualization -> loadPoints(outPoints, nOutPoints);
    qDebug() << "step 8";
    pVisualization -> show();
    if(outPoints) delete [] outPoints;
    if(GWASLogP) delete [] GWASLogP;
    close();
}

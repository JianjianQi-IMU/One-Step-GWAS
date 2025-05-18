#include "StartGWASVisualizationWidget.hpp"

StartGWASVisualizationLessWithLogP::StartGWASVisualizationLessWithLogP()
{
    data=nullptr;
}

StartGWASVisualizationLessWithLogP::StartGWASVisualizationLessWithLogP(double *inCompList)
    :StartGWASVisualizationLessWithLogP()
{
    data=inCompList;
}

void StartGWASVisualizationLessWithLogP::setCompList(double *inCompList)
{
    data=inCompList;
}

bool StartGWASVisualizationLessWithLogP::operator ()(size_t a, size_t b)
{
    return data[a]<data[b];
}

StartGWASVisualizationWidget::StartGWASVisualizationWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle("Load Manhattan Plot File");

    continueBtn   = new QPushButton("continue",this);
    cancelBtn     = new QPushButton("cancel",this);
    bimPathEdit   = new QLineEdit(this);
    logPPathEdit  = new QLineEdit(this);
    colIndexBox   = new QSpinBox(this);
    openBimLabel  = new QLabel("choose markers annotation file (.bim):",this);
    openLogPLabel = new QLabel("choose GWAS output file:",this);
    colLabel      = new QLabel("input -log P colnum:",this);
    QPushButton* openBimBtn    = new QPushButton(this);
    QPushButton* openLogPBtn   = new QPushButton(this);

    QGroupBox* basicBox=new QGroupBox("Basic",this);
    QVBoxLayout* basicBoxLayout=new QVBoxLayout(basicBox);
    QGroupBox* annotationBox=new QGroupBox("Genome Annotation",this);
    QVBoxLayout* annotationBoxLayout=new QVBoxLayout(annotationBox);
    tsvRadio=new QRadioButton("TSV",this);
    gffRadio=new QRadioButton("GFF",this);
    QLabel* lenInfoLabel=new QLabel(this);
    QLabel* genomeInfoLabel=new QLabel(this);
    QLabel* topLabel=new QLabel("top(%)",this);
    topRadio=new QRadioButton("top percentage",this);
    threRadio=new QRadioButton("-logP threshold",this);
    lenInfoPathEdit=new QLineEdit(this);
    tsvPathEdit=new QLineEdit(this);
    gffPathEdit=new QLineEdit(this);
    QPushButton* openLenInfoBtn=new QPushButton(this);
    openTsvBtn=new QPushButton(this);
    openGffBtn=new QPushButton(this);
    topFactorBox=new QDoubleSpinBox(this);
    thresholdBox=new QDoubleSpinBox(this);
    setGffReadBtn=new QPushButton(">>>set read",this);
    QButtonGroup* basicBtnGroup=new QButtonGroup(this);
    QButtonGroup* annotationBtnGroup=new QButtonGroup(this);

    QWidget* bimWidget    = new QWidget(this);
    QWidget* logPWidget   = new QWidget(this);
    QWidget* colWidget    = new QWidget(this);
    QWidget* buttomWidget = new QWidget(this);
    QVBoxLayout* mainLayout=new QVBoxLayout(this);
    QWidget* lenInfoWidget=new QWidget(this);
    QWidget* topFactorWidget=new QWidget(this);
    QWidget* tsvOpenWidget=new QWidget(this);
    QWidget* gffOpenWidget=new QWidget(this);
//    QWidget* buttomBtnWidget=new QWidget(this);
    QHBoxLayout* lenInfoLayout=new QHBoxLayout(lenInfoWidget);
    QHBoxLayout* topFactorLayout=new QHBoxLayout(topFactorWidget);
    QHBoxLayout* tsvOpenLayout=new QHBoxLayout(tsvOpenWidget);
    QHBoxLayout* gffOpenLayout=new QHBoxLayout(gffOpenWidget);
//    QHBoxLayout* buttomBtnLayout=new QHBoxLayout(buttomBtnWidget);
    QHBoxLayout* bimLayout    = new QHBoxLayout(bimWidget);
    QHBoxLayout* logPLayout   = new QHBoxLayout(logPWidget);
    QHBoxLayout* colLayout    = new QHBoxLayout(colWidget);
    QHBoxLayout* buttomLayout = new QHBoxLayout(buttomWidget);

    mainLayout->addWidget(basicBox);
    mainLayout->addWidget(annotationBox);
    mainLayout->addWidget(buttomWidget);
    basicBoxLayout->addWidget(lenInfoLabel);
    basicBoxLayout->addWidget(lenInfoWidget);
    basicBoxLayout->addWidget(openLogPLabel);
    basicBoxLayout->addWidget(logPWidget);
    basicBoxLayout->addWidget(openBimLabel);
    basicBoxLayout->addWidget(bimWidget);
    basicBoxLayout->addWidget(colWidget);
    basicBoxLayout->addWidget(topRadio);
    basicBoxLayout->addWidget(topFactorWidget);
    basicBoxLayout->addWidget(threRadio);
    basicBoxLayout->addWidget(thresholdBox);
    annotationBoxLayout->addWidget(genomeInfoLabel);
    annotationBoxLayout->addWidget(tsvRadio);
    annotationBoxLayout->addWidget(tsvOpenWidget);
    annotationBoxLayout->addWidget(gffRadio);
    annotationBoxLayout->addWidget(gffOpenWidget);
    annotationBoxLayout->addWidget(setGffReadBtn);
    lenInfoLayout->addWidget(lenInfoPathEdit);
    lenInfoLayout->addWidget(openLenInfoBtn);
    topFactorLayout->addWidget(topFactorBox);
    topFactorLayout->addWidget(topLabel);
    tsvOpenLayout->addWidget(tsvPathEdit);
    tsvOpenLayout->addWidget(openTsvBtn);
    gffOpenLayout->addWidget(gffPathEdit);
    gffOpenLayout->addWidget(openGffBtn);
    bimLayout->addWidget(bimPathEdit);
    bimLayout->addWidget(openBimBtn);
    logPLayout->addWidget(logPPathEdit);
    logPLayout->addWidget(openLogPBtn);
    colLayout->addWidget(colLabel);
    colLayout->addWidget(colIndexBox);
    buttomLayout->addWidget(continueBtn);
    buttomLayout->addWidget(cancelBtn);

    lenInfoLabel->setText("Please choose chromosome length annotation file:");
    genomeInfoLabel->setText("Please choose genome annotation file:");

//    topWidget->setFixedHeight(25);
//    topLabel->setFixedWidth(15);
    topFactorBox->setMinimum(0);
    topFactorBox->setMaximum(100);
    topFactorBox->setSuffix("%");
    topFactorBox->setSingleStep(0.05);

    thresholdBox->setMinimum(0);
    thresholdBox->setMaximum(200);
    thresholdBox->setSingleStep(0.01);

    continueBtn->setFixedWidth(80);
    cancelBtn  ->setFixedWidth(80);
    setGffReadBtn->setFixedWidth(120);

    basicBtnGroup->addButton(topRadio);
    basicBtnGroup->addButton(threRadio);
    basicBtnGroup->setExclusive(true);
    annotationBtnGroup->addButton(tsvRadio);
    annotationBtnGroup->addButton(gffRadio);
    annotationBtnGroup->setExclusive(true);

    openLenInfoBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openTsvBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openGffBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openBimBtn ->setIcon(QIcon(":/icon/openIcon.png"));
    openLogPBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openLenInfoBtn->setFixedWidth(25);
    openTsvBtn->setFixedWidth(25);
    openGffBtn->setFixedWidth(25);
    openBimBtn ->setFixedWidth(25);
    openLogPBtn->setFixedWidth(25);
    colIndexBox->setMaximum(100);
    colIndexBox->setMinimum(1);
    colIndexBox->setSingleStep(1);
    colIndexBox->setValue(4);

    connect(openLenInfoBtn ,&QPushButton::clicked,this ,&StartGWASVisualizationWidget::openLenInfoFile);
    connect(openTsvBtn     ,&QPushButton::clicked,this ,&StartGWASVisualizationWidget::openTsv);
    connect(openGffBtn     ,&QPushButton::clicked,this ,&StartGWASVisualizationWidget::openGff);
    connect(topRadio       ,&QRadioButton::clicked,this,&StartGWASVisualizationWidget::topRadioSelected);
    connect(threRadio      ,&QRadioButton::clicked,this,&StartGWASVisualizationWidget::threRadioSelected);
    connect(tsvRadio       ,&QRadioButton::clicked,this,&StartGWASVisualizationWidget::tsvRadioSelected);
    connect(gffRadio       ,&QRadioButton::clicked,this,&StartGWASVisualizationWidget::gffRadioSelected);
    connect(setGffReadBtn  ,&QRadioButton::clicked,this,&StartGWASVisualizationWidget::createGffReadSetting);
    connect(continueBtn    ,&QPushButton::clicked,this ,&StartGWASVisualizationWidget::startVisualization);
    connect(cancelBtn      ,&QPushButton::clicked,this ,&StartGWASVisualizationWidget::close);
    connect(openBimBtn     ,&QPushButton::clicked,this ,&StartGWASVisualizationWidget::openBimFile);
    connect(openLogPBtn    ,&QPushButton::clicked,this ,&StartGWASVisualizationWidget::openLogPFile);

    topRadio->setChecked(true);
    tsvRadio->setChecked(true);
    topRadioSelected();
    tsvRadioSelected();
}

StartGWASVisualizationWidget::~StartGWASVisualizationWidget()
{

}

void StartGWASVisualizationWidget::openLenInfoFile()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose chromosome length annotation file.");
    if(!filePath.isEmpty()) lenInfoPathEdit->setText(filePath);
}

void StartGWASVisualizationWidget::openTsv()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose genome annotation file.");
    if(!filePath.isEmpty()) tsvPathEdit->setText(filePath);
}

void StartGWASVisualizationWidget::openGff()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose genome annotation file.");
    if(!filePath.isEmpty()) gffPathEdit->setText(filePath);
}

void StartGWASVisualizationWidget::topRadioSelected()
{
    topFactorBox->setEnabled(true);
    thresholdBox->setEnabled(false);
}

void StartGWASVisualizationWidget::threRadioSelected()
{
    topFactorBox->setEnabled(false);
    thresholdBox->setEnabled(true);
}

void StartGWASVisualizationWidget::tsvRadioSelected()
{
    tsvPathEdit->setEnabled(true);
    openTsvBtn->setEnabled(true);
    gffPathEdit->setEnabled(false);
    openGffBtn->setEnabled(false);
    setGffReadBtn->setEnabled(false);
}

void StartGWASVisualizationWidget::gffRadioSelected()
{
    tsvPathEdit->setEnabled(false);
    openTsvBtn->setEnabled(false);
    gffPathEdit->setEnabled(true);
    openGffBtn->setEnabled(true);
    setGffReadBtn->setEnabled(true);
}

void StartGWASVisualizationWidget::openBimFile()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose markers annotation file (.bim)");
    if(!filePath.isEmpty()) bimPathEdit->setText(filePath);
}

void StartGWASVisualizationWidget::openLogPFile()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose GWAS output file");
    if(!filePath.isEmpty()) logPPathEdit->setText(filePath);
}

void StartGWASVisualizationWidget::startVisualization()
{
    FD::GWASOutread GWASreader;
    FD::BIMread BIMreader;
    FD::BIMData bimData;
    std::vector<std::string> chrNameVec;
    std::vector<long long> chrLenVec;
    std::map<std::string, int> chrNameLenMap;
    StartGWASVisualizationLessWithLogP comp;
    ValPoints* outPoints=nullptr;
    FD::BIMNode* bimList=nullptr;
    double* GWASLogP=nullptr;
//    FD::BIMnode* bimPoints=nullptr,*tBimPoint=nullptr;
    long long nOutPoints=0,i=0,j=0,tpqid,nBimChr=0,nBimPoints=0;
    char** bimChrName=nullptr;
    FD::ChrLenReadTsvState chrLenState;
    bool errorFlag=false;
    if(!QFile::exists(bimPathEdit->text())){
        QMessageBox::critical(this,"error","the .bim file don\'t exist.");
        return;
    }
    if(!QFile::exists(logPPathEdit->text())){
        QMessageBox::critical(this,"error","the -log P file don\'t exist.");
        return;
    }
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
    if(bimData.nMarker!=GWASreader.nMarkers){
        QMessageBox::critical(this,"error","the number of markers in .bim file isn\'t equal to GWAS output file.");
        return;
    }
    GWASVisualize* pVisualization=nullptr;
    bool isTsv=true;
    QString genoAnnoFilePath;
    if(tsvRadio->isChecked()){
        genoAnnoFilePath=tsvPathEdit->text();
    }
    else{
        genoAnnoFilePath=gffPathEdit->text();
        isTsv=false;
    }

    double filterValue=0;
    bool isTop=true;
    if(topRadio->isChecked()){
        filterValue=topFactorBox->value()/100;
    }
    else{
        filterValue=thresholdBox->value();
        isTop=false;
    }
    if(!QFile::exists(lenInfoPathEdit->text())){
        QMessageBox::critical(this,"error","the chromosome length file don\'t exist.");
        return;
    }
    if(!QFile::exists(genoAnnoFilePath)){
        QMessageBox::critical(this,"error","the genome annotation file don\'t exist.");
        return;
    }
    nBimPoints=bimData.nMarker;
    nBimChr=bimData.getNChr();
//    bimPoints=BIMreader.data;
    bimList=bimData.data;
    bimChrName=bimData.getChrName();
    GWASLogP = new double[nBimPoints];
    for(i=0;i<nBimPoints;++i){
        GWASLogP[i]=-std::log10(GWASreader.getValueAt(i));
        if(GWASLogP[i]<0.0) GWASLogP[i]=0.0;
    }
    comp.setCompList(GWASLogP);

    // get top num points
    if(isTop){
        nOutPoints=nBimPoints*filterValue;
        nOutPoints=std::min(nOutPoints,nBimPoints);
        if(!nOutPoints){
            QMessageBox::critical(this,"number error","the number displayed is zero");
            if(GWASLogP) delete [] GWASLogP;
            return;
        }

        if(!bimChrName) {
            QMessageBox::critical(this,"error","result is error");
            if(GWASLogP) delete [] GWASLogP;
            return;
        }
        std::priority_queue<size_t,std::vector<size_t>,StartGWASVisualizationLessWithLogP> pq(comp);
        for(i=0;i<nBimPoints;++i) pq.push(i);
        outPoints=new ValPoints[nOutPoints];
        for(i=0;i<nOutPoints;++i){
            tpqid=pq.top();
            pq.pop();
            outPoints[i].idchr=bimList[tpqid].idChr;
            outPoints[i].pos  =bimList[tpqid].pos;
            outPoints[i].val  =GWASLogP[tpqid];
        }
    }
    else{
        nOutPoints=0;
        for(i=0;i<nBimPoints;++i){
            if(GWASLogP[i]>=filterValue) ++nOutPoints;
        }
        if(!nOutPoints){
            QMessageBox::critical(this,"number error","the number displayed is zero");
            if(GWASLogP) delete [] GWASLogP;
            return;
        }

        if(!bimChrName) {
            QMessageBox::critical(this,"error","result is error");
            if(GWASLogP) delete [] GWASLogP;
            return;
        }
        outPoints=new ValPoints[nOutPoints];
        for(i=0,j=0;i<nBimPoints&&j<nOutPoints;++i){
            if(GWASLogP[i]>=filterValue){
                outPoints[j].idchr=bimList[i].idChr;
                outPoints[j].pos  =bimList[i].pos;
                outPoints[j].val  =GWASLogP[i];
                ++j;
            }
        }
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

    genomeReader.open(genoAnnoFilePath.toLatin1().data(),chrNameVec);
    if(isTsv) genomeReader.readTsv();
    else genomeReader.readGff();
    if(!genomeReader.getGenome()||!(genomeReader.getGenome()->getValid())){
        if(outPoints) delete [] outPoints;
        if(GWASLogP) delete [] GWASLogP;
        QMessageBox::critical(this,"error","genome annotation file reading error");
        return;
    }
    pVisualization=new GWASVisualize(chrLenVec,chrNameVec,outPoints,nOutPoints);
    pVisualization->createGenome();
    pVisualization->setGenome(*(genomeReader.getGenome()));
    pVisualization->setAttribute(Qt::WA_DeleteOnClose,true);
    pVisualization->show();
    if(outPoints) delete [] outPoints;
    if(GWASLogP) delete [] GWASLogP;
    close();
}

void StartGWASVisualizationWidget::createGffReadSetting()
{
    SetGffReadAttribute* tWidget=new SetGffReadAttribute(&genomeReader);
    tWidget->setAttribute(Qt::WA_ShowModal,true);
    tWidget->show();
}


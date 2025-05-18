#include "PreVisualizeWidget.hpp"

#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QFileDialog>

#include <queue>

#include "GWASVisualize/GWASVisualize.hpp"
#include "GFFSetting/SetGffReadAttribute.hpp"

PreVisualizeWidget::PreVisualizeWidget(QWidget *parent):QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle("GWAS Visualization");

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
    filePathEdit=new QLineEdit(this);
    tsvPathEdit=new QLineEdit(this);
    gffPathEdit=new QLineEdit(this);
    QPushButton* openBtn=new QPushButton(this);
    openTsvBtn=new QPushButton(this);
    openGffBtn=new QPushButton(this);
    topFactorBox=new QDoubleSpinBox(this);
    thresholdBox=new QDoubleSpinBox(this);
    continueBtn=new QPushButton("Continue",this);
    cancelBtn  =new QPushButton("Cancel",this);
    setGffReadBtn=new QPushButton(">>>set read",this);
    QButtonGroup* basicBtnGroup=new QButtonGroup(this);
    QButtonGroup* annotationBtnGroup=new QButtonGroup(this);

    QVBoxLayout* mainLayout=new QVBoxLayout(this);
    QWidget* lenInfoWidget=new QWidget(this);
    QWidget* topFactorWidget=new QWidget(this);
    QWidget* tsvOpenWidget=new QWidget(this);
    QWidget* gffOpenWidget=new QWidget(this);
    QWidget* buttomBtnWidget=new QWidget(this);
    QHBoxLayout* lenInfoLayout=new QHBoxLayout(lenInfoWidget);
    QHBoxLayout* topFactorLayout=new QHBoxLayout(topFactorWidget);
    QHBoxLayout* tsvOpenLayout=new QHBoxLayout(tsvOpenWidget);
    QHBoxLayout* gffOpenLayout=new QHBoxLayout(gffOpenWidget);
    QHBoxLayout* buttomBtnLayout=new QHBoxLayout(buttomBtnWidget);

    mainLayout->addWidget(basicBox);
    mainLayout->addWidget(annotationBox);
    mainLayout->addWidget(buttomBtnWidget);
    basicBoxLayout->addWidget(lenInfoLabel);
    basicBoxLayout->addWidget(lenInfoWidget);
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
    lenInfoLayout->addWidget(filePathEdit);
    lenInfoLayout->addWidget(openBtn);
    topFactorLayout->addWidget(topFactorBox);
    topFactorLayout->addWidget(topLabel);
    tsvOpenLayout->addWidget(tsvPathEdit);
    tsvOpenLayout->addWidget(openTsvBtn);
    gffOpenLayout->addWidget(gffPathEdit);
    gffOpenLayout->addWidget(openGffBtn);
    buttomBtnLayout->addWidget(continueBtn);
    buttomBtnLayout->addWidget(cancelBtn);

    lenInfoLabel->setText("Please choose chromosome length annotation file:");
    genomeInfoLabel->setText("Please choose genome annotation file:");

    openBtn   ->setIcon(QIcon(":/icon/openIcon.png"));
    openTsvBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openGffBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openBtn   ->setFixedWidth(25);
    openTsvBtn->setFixedWidth(25);
    openGffBtn->setFixedWidth(25);
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
//    cenWidget2->setFixedHeight(25);
//    cenWidget->setFixedHeight(25);
//    cancelBtn->setFixedWidth(20);
//    continueBtn->setFixedWidth(20);
//    downWidget->setFixedHeight(25);

    connect(openBtn,&QPushButton::clicked,this,&PreVisualizeWidget::openFile);
    connect(openTsvBtn,&QPushButton::clicked,this,&PreVisualizeWidget::openTsv);
    connect(openGffBtn,&QPushButton::clicked,this,&PreVisualizeWidget::openGff);
    connect(continueBtn,&QPushButton::clicked,this,&PreVisualizeWidget::createVisualize);
    connect(cancelBtn,&QPushButton::clicked,this,&PreVisualizeWidget::close);
    connect(topRadio,&QRadioButton::clicked,this,&PreVisualizeWidget::topRadioSelected);
    connect(threRadio,&QRadioButton::clicked,this,&PreVisualizeWidget::threRadioSelected);
    connect(tsvRadio,&QRadioButton::clicked,this,&PreVisualizeWidget::tsvRadioSelected);
    connect(gffRadio,&QRadioButton::clicked,this,&PreVisualizeWidget::gffRadioSelected);
    connect(setGffReadBtn,&QRadioButton::clicked,this,&PreVisualizeWidget::createGffReadSetting);

    topRadio->setChecked(true);
    tsvRadio->setChecked(true);
    topRadioSelected();
    tsvRadioSelected();

    bim=nullptr;
}

PreVisualizeWidget::PreVisualizeWidget(FD::BIMLogPData *inBim, QWidget *parent)
    :PreVisualizeWidget(parent)
{
    bim=inBim;
}

PreVisualizeWidget::~PreVisualizeWidget(){

}

void PreVisualizeWidget::openFile()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose chromosome length annotation file.");
    if(!filePath.isEmpty()) filePathEdit->setText(filePath);
}

void PreVisualizeWidget::openTsv()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose genome annotation file.");
    if(!filePath.isEmpty()) tsvPathEdit->setText(filePath);
}

void PreVisualizeWidget::openGff()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose genome annotation file.");
    if(!filePath.isEmpty()) gffPathEdit->setText(filePath);
}

void PreVisualizeWidget::topRadioSelected()
{
    topFactorBox->setEnabled(true);
    thresholdBox->setEnabled(false);
}

void PreVisualizeWidget::threRadioSelected()
{
    topFactorBox->setEnabled(false);
    thresholdBox->setEnabled(true);
}

void PreVisualizeWidget::tsvRadioSelected()
{
    tsvPathEdit->setEnabled(true);
    openTsvBtn->setEnabled(true);
    gffPathEdit->setEnabled(false);
    openGffBtn->setEnabled(false);
    setGffReadBtn->setEnabled(false);
}

void PreVisualizeWidget::gffRadioSelected()
{
    tsvPathEdit->setEnabled(false);
    openTsvBtn->setEnabled(false);
    gffPathEdit->setEnabled(true);
    openGffBtn->setEnabled(true);
    setGffReadBtn->setEnabled(true);
}

void PreVisualizeWidget::createVisualize()
{
    FD::ChrLenReadTsvState chrLenState;

    if(!bim->isValid) return;
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
//    long long nGenes=0,iGene=0;
//    std::map<std::string,int> chrIdMap;

//    QFile file(filePathEdit->text());
    std::vector<std::string> chrNameVec;
    std::vector<long long> chrLenVec;
    std::map<std::string, int> chrNameLenMap;

    std::priority_queue<FD::BIMLogPNode*,std::vector<FD::BIMLogPNode*>,FD::BIMLogPNodeLessWithLogP> pq;
    ValPoints* outPoints=nullptr;
    FD::BIMLogPNode* bimPoints=nullptr,*tBimPoint=nullptr;
    long long nOutPoints=0,i=0,j=0,nBimChr=0,nBimPoints=0;
    char** bimChrName=nullptr;

    bool errorFlag=false;
    if(QFile::exists(filePathEdit->text())&&QFile::exists(genoAnnoFilePath)){
        nBimPoints=bim->nMarker;
        nBimChr=bim->getNChr();
        bimPoints=bim->data;
        bimChrName=bim->getChrName();

        // get top num points
        if(isTop){
            nOutPoints=nBimPoints*filterValue;
            nOutPoints=std::min(nOutPoints,nBimPoints);
            if(!nOutPoints){
                QMessageBox::critical(this,"number error","the number displayed is zero");
                return;
            }

            if(!bimChrName) {
                QMessageBox::critical(this,"error","result is error");
                return;
            }
            for(i=0;i<nBimPoints;++i) pq.push(&bimPoints[i]);
            outPoints=new ValPoints[nOutPoints];
            for(i=0;i<nOutPoints;++i){
                tBimPoint=pq.top();
                pq.pop();
                outPoints[i].idchr=tBimPoint->idChr;
                outPoints[i].pos  =tBimPoint->pos;
                outPoints[i].val  =tBimPoint->log10PValue;
            }
        }
        else{
            nOutPoints=0;
            for(i=0;i<nBimPoints;++i){
                if(bimPoints[i].log10PValue>=filterValue) ++nOutPoints;
            }
            if(!nOutPoints){
                QMessageBox::critical(this,"number error","the number displayed is zero");
                return;
            }

            if(!bimChrName) {
                QMessageBox::critical(this,"error","result is error");
                return;
            }
            outPoints=new ValPoints[nOutPoints];
            for(i=0,j=0;i<nBimPoints&&j<nOutPoints;++i){
                if(bimPoints[i].log10PValue>=filterValue){
                    outPoints[j].idchr=bimPoints[i].idChr;
                    outPoints[j].pos  =bimPoints[i].pos;
                    outPoints[j].val  =bimPoints[i].log10PValue;
                    ++j;
                }
            }
        }
//        qDebug() << "step 1";


        if((chrLenState = FD::chrLenReadTsv(filePathEdit->text().toLatin1().data(),chrNameVec,chrLenVec))
                != FD::CHRLENREADTSV_OK){
            if(outPoints) delete [] outPoints;
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
            QMessageBox::critical(this,"error","genome annotation file reading error");
            return;
        }

        pVisualization=new GWASVisualize(chrLenVec,chrNameVec,outPoints,nOutPoints);
        pVisualization->createGenome();
        pVisualization->setGenome(*(genomeReader.getGenome()));
        pVisualization->setAttribute(Qt::WA_DeleteOnClose,true);
        pVisualization->show();
        if(outPoints) delete [] outPoints;
        close();
    }
    else{
        QMessageBox::critical(this,"error","the file don\'t exist");
    }
}

void PreVisualizeWidget::createGffReadSetting()
{
    SetGffReadAttribute* tWidget=new SetGffReadAttribute(&genomeReader);
    tWidget->setAttribute(Qt::WA_ShowModal,true);
    tWidget->show();
}

#include "PreBSAVisualizeWidget.hpp"

#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QFileDialog>
#include <QMessageBox>

#include "GFFSetting/SetGffReadAttribute.hpp"
#include "BSAVisualize/BSAVisualize.hpp"

PreBSAVisualizeWidget::PreBSAVisualizeWidget(QWidget *parent)
    :QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);

    QGroupBox* annotationBox=new QGroupBox("Genome Annotation",this);
    QVBoxLayout* annotationBoxLayout=new QVBoxLayout(annotationBox);
    tsvRadio=new QRadioButton("TSV",this);
    gffRadio=new QRadioButton("GFF",this);
    QLabel* genomeInfoLabel=new QLabel(this);
    tsvPathEdit=new QLineEdit(this);
    gffPathEdit=new QLineEdit(this);
    openTsvBtn=new QPushButton(this);
    openGffBtn=new QPushButton(this);
    continueBtn=new QPushButton("continue",this);
    cancelBtn  =new QPushButton("cancel",this);
    setGffReadBtn=new QPushButton(">>>set read",this);
    QButtonGroup* basicBtnGroup=new QButtonGroup(this);
    QButtonGroup* annotationBtnGroup=new QButtonGroup(this);

    QVBoxLayout* mainLayout=new QVBoxLayout(this);
    QWidget* tsvOpenWidget=new QWidget(this);
    QWidget* gffOpenWidget=new QWidget(this);
    QWidget* buttomBtnWidget=new QWidget(this);
    QHBoxLayout* tsvOpenLayout=new QHBoxLayout(tsvOpenWidget);
    QHBoxLayout* gffOpenLayout=new QHBoxLayout(gffOpenWidget);
    QHBoxLayout* buttomBtnLayout=new QHBoxLayout(buttomBtnWidget);

    mainLayout->addWidget(annotationBox);
    mainLayout->addWidget(buttomBtnWidget);
    annotationBoxLayout->addWidget(genomeInfoLabel);
    annotationBoxLayout->addWidget(tsvRadio);
    annotationBoxLayout->addWidget(tsvOpenWidget);
    annotationBoxLayout->addWidget(gffRadio);
    annotationBoxLayout->addWidget(gffOpenWidget);
    annotationBoxLayout->addWidget(setGffReadBtn);
    tsvOpenLayout->addWidget(tsvPathEdit);
    tsvOpenLayout->addWidget(openTsvBtn);
    gffOpenLayout->addWidget(gffPathEdit);
    gffOpenLayout->addWidget(openGffBtn);
    buttomBtnLayout->addWidget(continueBtn);
    buttomBtnLayout->addWidget(cancelBtn);

    genomeInfoLabel->setText("Please choose genome annotation file:");

    openTsvBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openGffBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openTsvBtn->setFixedWidth(25);
    openGffBtn->setFixedWidth(25);
//    topWidget->setFixedHeight(25);
//    topLabel->setFixedWidth(15);

    continueBtn->setFixedWidth(80);
    cancelBtn  ->setFixedWidth(80);
    setGffReadBtn->setFixedWidth(120);

    basicBtnGroup->setExclusive(true);
    annotationBtnGroup->addButton(tsvRadio);
    annotationBtnGroup->addButton(gffRadio);
    annotationBtnGroup->setExclusive(true);
//    cenWidget2->setFixedHeight(25);
//    cenWidget->setFixedHeight(25);
//    cancelBtn->setFixedWidth(20);
//    continueBtn->setFixedWidth(20);
//    downWidget->setFixedHeight(25);

    connect(openTsvBtn,&QPushButton::clicked,this,&PreBSAVisualizeWidget::openTsv);
    connect(openGffBtn,&QPushButton::clicked,this,&PreBSAVisualizeWidget::openGff);
    connect(continueBtn,&QPushButton::clicked,this,&PreBSAVisualizeWidget::createVisualize);
    connect(cancelBtn,&QPushButton::clicked,this,&PreBSAVisualizeWidget::close);
    connect(tsvRadio,&QRadioButton::clicked,this,&PreBSAVisualizeWidget::tsvRadioSelected);
    connect(gffRadio,&QRadioButton::clicked,this,&PreBSAVisualizeWidget::gffRadioSelected);
    connect(setGffReadBtn,&QRadioButton::clicked,this,&PreBSAVisualizeWidget::createGffReadSetting);

    tsvRadio->setChecked(true);
    tsvRadioSelected();

    thread=nullptr;
}

PreBSAVisualizeWidget::PreBSAVisualizeWidget(ThreadRun::QNormalBSAThread* inThread, QWidget *parent)
    :PreBSAVisualizeWidget(parent)
{
    thread = inThread;
}

PreBSAVisualizeWidget::~PreBSAVisualizeWidget()
{

}

void PreBSAVisualizeWidget::openTsv()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose genome annotation file.");
    if(!filePath.isEmpty()) tsvPathEdit->setText(filePath);
}

void PreBSAVisualizeWidget::openGff()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose genome annotation file.");
    if(!filePath.isEmpty()) gffPathEdit->setText(filePath);
}

void PreBSAVisualizeWidget::tsvRadioSelected()
{
    tsvPathEdit->setEnabled(true);
    openTsvBtn->setEnabled(true);
    gffPathEdit->setEnabled(false);
    openGffBtn->setEnabled(false);
    setGffReadBtn->setEnabled(false);
}

void PreBSAVisualizeWidget::gffRadioSelected()
{
    tsvPathEdit->setEnabled(false);
    openTsvBtn->setEnabled(false);
    gffPathEdit->setEnabled(true);
    openGffBtn->setEnabled(true);
    setGffReadBtn->setEnabled(true);
}

void PreBSAVisualizeWidget::createVisualize()
{
    FD::BSAProject* project = thread->project;
    FD::BSAInterVal* inter = thread->inter;
    size_t nInter = thread->nInter;
    BSAVisualize* pVisualization=nullptr;
    size_t* chrLenList = nullptr;
    int nChr = 0;

    bool isTsv=true;
    QString genoAnnoFilePath;
    if(tsvRadio->isChecked()){
        genoAnnoFilePath=tsvPathEdit->text();
    }
    else{
        genoAnnoFilePath=gffPathEdit->text();
        isTsv=false;
    }

//    long long nGenes=0,iGene=0;
//    std::map<std::string,int> chrIdMap;

//    QFile file(filePathEdit->text());
    if(QFile::exists(genoAnnoFilePath)){

        nChr = (project->chrName).size();
        genomeReader.open(genoAnnoFilePath.toLatin1().data(),project->getChrName(),nChr);
        if(isTsv) genomeReader.readTsv();
        else genomeReader.readGff();
        if(!genomeReader.getGenome()||!(genomeReader.getGenome()->getValid())){
            QMessageBox::critical(this,"error","genome annotation file reading error");
            return;
        }
        chrLenList = new size_t[nChr];
        for(int i=0;i<nChr;++i){
            chrLenList[i] = (project->chrLen)[i];
        }
        pVisualization=new BSAVisualize(chrLenList,project->getChrName(),nChr,inter,nInter,project->points,project->data.getNRow());
        pVisualization->createGenome();
        pVisualization->setGenome(*(genomeReader.getGenome()));
        qDebug() << "step 1";
        pVisualization->setAttribute(Qt::WA_DeleteOnClose,true);
        pVisualization->show();
        if(chrLenList) delete [] chrLenList;
        close();
    }
    else{
        QMessageBox::critical(this,"error","the file don\'t exist");
    }
}

void PreBSAVisualizeWidget::createGffReadSetting()
{
    SetGffReadAttribute* tWidget=new SetGffReadAttribute(&genomeReader);
    tWidget->setAttribute(Qt::WA_ShowModal,true);
    tWidget->show();
}

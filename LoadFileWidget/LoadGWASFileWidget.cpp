#include "LoadGWASFileWidget.hpp"

#include <QLineEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

#include "FileDeal/ViewFileFormatWidget.hpp"

LoadGWASFileWidget::LoadGWASFileWidget(QWidget *parent)
    :QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle("Load");

    continueBtn  = nullptr;
    cancelBtn    = nullptr;
    openPlinkBtn = nullptr;
    openPheBtn   = nullptr;
    openKinBtn   = nullptr;
    openCovBtn   = nullptr;
    fileFormBtn  = nullptr;
    fileFormChooseBox = nullptr;
    editFile1      = nullptr;
    editFile2      = nullptr;
    editPhe      = nullptr;
    editKin      = nullptr;
    editCov      = nullptr;
    labelPlink   = nullptr;
    labelPhe     = nullptr;
    labelKin     = nullptr;
    labelCov     = nullptr;
}

LoadGWASFileWidget::LoadGWASFileWidget(AnalyzeUnitInfo* inUnitInfo,
                                       QList<FD::BaseProject*>* inProjectList,
                                       int inRow,
                                       QWidget* parent)
    :LoadGWASFileWidget(parent)
{
    continueBtn  = new QPushButton("continue",this);
    cancelBtn    = new QPushButton("cancel",this);
    openPlinkBtn = new QPushButton(QIcon(":/icon/openIcon.png"),QString(),this);
    openPheBtn   = new QPushButton(QIcon(":/icon/openIcon.png"),QString(),this);
    openKinBtn   = new QPushButton(QIcon(":/icon/openIcon.png"),QString(),this);
    openCovBtn   = new QPushButton(QIcon(":/icon/openIcon.png"),QString(),this);
    fileFormBtn  = new QPushButton(QIcon(":/icon/openIcon.png"),QString(),this);
    fileFormChooseBox = new QComboBox(this);
    editFile1    = new QLineEdit(this);
    editFile2    = new QLineEdit(this);
    editPhe      = new QLineEdit(this);
    editKin      = new QLineEdit(this);
    editCov      = new QLineEdit(this);
    labelPlink   = new QLabel("Please choose the genotype file:",this);
    labelPhe     = new QLabel("Please choose the phenotype file:",this);
    labelKin     = new QLabel("Please choose the kinship file:",this);
    labelCov     = new QLabel("Please choose the covariate file:",this);

    QWidget* plinkWidget   = new QWidget(this);
    QWidget* covWidget     = new QWidget(this);
    QWidget* pheWidget     = new QWidget(this);
    QWidget* kinWidget     = new QWidget(this);
    QWidget* btnWidget     = new QWidget(this);

    QGroupBox* markerFileGroup = new QGroupBox(this);
    QGroupBox* pheFileGroup    = new QGroupBox(this);
    QGroupBox* covFileGroup    = new QGroupBox(this);
    QGroupBox* kinFileGroup    = new QGroupBox(this);
    QVBoxLayout* markerFileGroupLayout = new QVBoxLayout(markerFileGroup);
    QVBoxLayout* pheFileGroupLayout = new QVBoxLayout(pheFileGroup);
    QVBoxLayout* covFileGroupLayout = new QVBoxLayout(covFileGroup);
    QVBoxLayout* kinFileGroupLayout = new QVBoxLayout(kinFileGroup);

    QVBoxLayout* mainLayout  = new QVBoxLayout(this);
    QGridLayout* plinkLayout = new QGridLayout(plinkWidget);
    QHBoxLayout* covLayout   = new QHBoxLayout(covWidget);
    QHBoxLayout* pheLayout   = new QHBoxLayout(pheWidget);
    QHBoxLayout* kinLayout   = new QHBoxLayout(kinWidget);
    QHBoxLayout* btnLayout   = new QHBoxLayout(btnWidget);

    markerFileGroupLayout->addWidget(labelPlink);
    markerFileGroupLayout->addWidget(plinkWidget);
    pheFileGroupLayout   ->addWidget(labelPhe);
    pheFileGroupLayout   ->addWidget(pheWidget);
    covFileGroupLayout   ->addWidget(labelCov);
    covFileGroupLayout   ->addWidget(covWidget);
    kinFileGroupLayout   ->addWidget(labelKin);
    kinFileGroupLayout   ->addWidget(kinWidget);

    mainLayout->addWidget(markerFileGroup);
    mainLayout->addWidget(pheFileGroup);
    mainLayout->addWidget(covFileGroup);
    mainLayout->addWidget(kinFileGroup);
    mainLayout->addWidget(btnWidget);

    plinkLayout->addWidget(editFile1,0,0);
    plinkLayout->addWidget(editFile2,1,0);
    plinkLayout->addWidget(openPlinkBtn,0,1);
    plinkLayout->addWidget(fileFormChooseBox,2,0);
    plinkLayout->addWidget(fileFormBtn,2,1);
    covLayout->addWidget(editCov);
    covLayout->addWidget(openCovBtn);
    pheLayout->addWidget(editPhe);
    pheLayout->addWidget(openPheBtn);
    kinLayout->addWidget(editKin);
    kinLayout->addWidget(openKinBtn);
    btnLayout->addWidget(continueBtn);
    btnLayout->addWidget(cancelBtn);

    openPlinkBtn->setFixedWidth(25);
    openPheBtn  ->setFixedWidth(25);
    openKinBtn  ->setFixedWidth(25);
    openCovBtn  ->setFixedWidth(25);
    fileFormBtn ->setFixedWidth(25);

    markerFileGroup->setTitle("Variants Data *");
    pheFileGroup->setTitle("Phenotype Data *");
    covFileGroup->setTitle("Covariance Data");
    kinFileGroup->setTitle("Kinship Data");

    fileFormChooseBox->addItem("PLINK Format File (.bed .bim)");
    fileFormChooseBox->addItem("VCF File");
    fileFormChooseBox->addItem("Simplified Variants File");

    connect(continueBtn,&QPushButton::clicked,this,&LoadGWASFileWidget::save);
    connect(cancelBtn,&QPushButton::clicked,this,&LoadGWASFileWidget::close);
    connect(openPlinkBtn,&QPushButton::clicked,this,&LoadGWASFileWidget::loadGenoFile);
    connect(openPheBtn,&QPushButton::clicked,this,&LoadGWASFileWidget::loadPhe);
    connect(openKinBtn,&QPushButton::clicked,this,&LoadGWASFileWidget::loadKin);
    connect(openCovBtn,&QPushButton::clicked,this,&LoadGWASFileWidget::loadCov);
    connect(fileFormChooseBox,QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,&LoadGWASFileWidget::changeGenoFileInputMode);
    connect(fileFormBtn,&QPushButton::clicked,this,&LoadGWASFileWidget::viewFileFormat);

    setUnitInfo(inUnitInfo);
    pProjectList=inProjectList;
    row=inRow;
}

LoadGWASFileWidget::~LoadGWASFileWidget()
{
    if(continueBtn ) delete continueBtn ;
    if(cancelBtn   ) delete cancelBtn   ;
    if(openPlinkBtn) delete openPlinkBtn;
    if(openPheBtn  ) delete openPheBtn  ;
    if(openKinBtn  ) delete openKinBtn  ;
    if(openCovBtn  ) delete openCovBtn  ;
    if(editFile1   ) delete editFile1   ;
    if(editFile2   ) delete editFile2   ;
    if(editPhe     ) delete editPhe     ;
    if(editKin     ) delete editKin     ;
    if(editCov     ) delete editCov     ;
    if(labelPlink  ) delete labelPlink  ;
    if(labelPhe    ) delete labelPhe    ;
    if(labelKin    ) delete labelKin    ;
    if(labelCov    ) delete labelCov    ;
}

void LoadGWASFileWidget::setFileSet(FD::BaseProjectFilesSet *inProject)
{
    if(inProject -> getProjectInfo() != FD::PROJECT_GWAS){
        return;
    }
    FD::GWASProjectFilesSet *tPro = dynamic_cast<FD::GWASProjectFilesSet *>(inProject);
    if(tPro -> getGenoFileForm() == FD::FILEFORM_BED){
        fileFormChooseBox -> setCurrentIndex(0);
        editFile1->setText(tPro -> getGenoFile());
        editFile2->setText(tPro -> getBimFile());
    } else if(tPro -> getGenoFileForm() == FD::FILEFORM_VCF){
        fileFormChooseBox -> setCurrentIndex(1);
        editFile1->setText(tPro -> getGenoFile());
    } else if(tPro -> getGenoFileForm() == FD::FILEFORM_PPED){
        fileFormChooseBox -> setCurrentIndex(2);
        editFile1->setText(tPro -> getGenoFile());
        editFile2->setText(tPro -> getBimFile());
    }


    editPhe->setText(tPro -> getPheFile());
    editKin->setText(tPro -> getKinFile());
    editCov->setText(tPro -> getCovFile());
}

void LoadGWASFileWidget::changeGenoFileInputMode(int id)
{
    switch (id) {
    case 0:
    case 2:
        editFile2 -> setEnabled(true);
        break;
    case 1:
        editFile2 -> setDisabled(true);
        break;
    default:
        editFile2 -> setEnabled(true);
        break;
    }
    editFile2 -> setText("");
    editFile1 -> setText("");
}

void LoadGWASFileWidget::setUnitInfo(AnalyzeUnitInfo *inUnitInfo)
{
    pUnitInfo=inUnitInfo;
}

void LoadGWASFileWidget::loadGenoFile()
{
    QString fileName,mName,bimName;

    if(fileFormChooseBox->currentIndex() == 0) {
        fileName=QFileDialog::getOpenFileName(this,"open PLINK format file (.bed .bim)",QString(),"*.bed *.bim");
        if(fileName.isEmpty()) return ;
        while((!fileName.isEmpty())&&
              fileName.back().toLatin1()!='.'&&
              fileName.back().toLatin1()!='/'&&
              fileName.back().toLatin1()!='\\') fileName.chop(1);
        mName=fileName+"bed";
        bimName=fileName+"bim";
        QFile bedFile(mName),bimFile(bimName);
        if(!bedFile.exists())
        {
         QMessageBox::warning(this,"Cannot find","The file (.bed) can\'t be found.");
         return ;
        }
        if(!bimFile.exists())
        {
         QMessageBox::warning(this,"Cannot find","The file (.bim) can\'t be found.");
         return ;
        }
        editFile1->setText(mName);
        editFile2->setText(bimName);
    }
    else if(fileFormChooseBox->currentIndex()==1){
        fileName=QFileDialog::getOpenFileName(this,"open VCF file (.vcf .gvcf)",QString(),"*.gvcf *.vcf");
        if(fileName.isEmpty()) return ;
        QFile vcfFile(fileName);
        if(!vcfFile.exists())
        {
         QMessageBox::warning(this,"Cannot find","The file "+fileName+" can\'t be found.");
         return ;
        }
        editFile1->setText(fileName);
    }
    else if(fileFormChooseBox->currentIndex()==2){
        fileName=QFileDialog::getOpenFileName(this,"open file (.pped .bim)",QString(),"*.pped *.bim");
        if(fileName.isEmpty()) return ;
        while((!fileName.isEmpty())&&
              fileName.back().toLatin1()!='.'&&
              fileName.back().toLatin1()!='/'&&
              fileName.back().toLatin1()!='\\') fileName.chop(1);
        mName=fileName+"pped";
        bimName=fileName+"bim";
        QFile ppedFile(mName),bimFile(bimName);
        if(!ppedFile.exists())
        {
         QMessageBox::warning(this,"Cannot find","The file (.pped) can\'t be found.");
         return ;
        }
        if(!bimFile.exists())
        {
         QMessageBox::warning(this,"Cannot find","The file (.bim) can\'t be found.");
         return ;
        }
        editFile1->setText(mName);
        editFile2->setText(bimName);
    }
}

void LoadGWASFileWidget::loadCov()
{
    QString fileName=QFileDialog::getOpenFileName(this,"open covariate file");
    if(fileName.isEmpty()) return ;
    QFile tFile(fileName);
    if(!tFile.exists())
    {
        QMessageBox::warning(this,"Cannot find","The covariate file can\'t be found.");
        return ;
    }
    editCov->setText(fileName);
}

void LoadGWASFileWidget::loadPhe()
{
    QString fileName=QFileDialog::getOpenFileName(this,"open phenotype file");
    if(fileName.isEmpty()) return ;
    QFile tFile(fileName);
    if(!tFile.exists())
    {
        QMessageBox::warning(this,"Cannot find","The phenotype file can\'t be found.");
        return ;
    }
    editPhe->setText(fileName);
}

void LoadGWASFileWidget::loadKin()
{
    QString fileName=QFileDialog::getOpenFileName(this,"open kinship file");
    if(fileName.isEmpty()) return ;
    QFile tFile(fileName);
    if(!tFile.exists())
    {
        QMessageBox::warning(this,"Cannot find","The kinship file can\'t be found.");
        return ;
    }
    editKin->setText(fileName);
}

void LoadGWASFileWidget::save()
{
    if((*pProjectList)[row]) FD::deleteProject((*pProjectList)[row]);
    FD::GWASProject* tproject=new FD::GWASProject;
    (*pProjectList)[row]=tproject;
    pUnitInfo->clear();
    if(!(editFile1->text().isEmpty())){
        pUnitInfo->addFile(editFile1->text(),"bed",":/icon/bedFile.png");
    }
    if(!(editFile2->text().isEmpty())){
        pUnitInfo->addFile(editFile2->text(),"bim",":/icon/bimFile.png");
    }
    if(!(editPhe->text().isEmpty())){
        pUnitInfo->addFile(editPhe->text(),"phe",":/icon/pheFile.png");
    }
    if(!(editCov->text().isEmpty())){
        pUnitInfo->addFile(editCov->text(),"cov",":/icon/covFile.png");
    }
    if(!(editKin->text().isEmpty())){
        pUnitInfo->addFile(editKin->text(),"kin",":/icon/kinFile.png");
    }
    std::cout << "read 1" << std::endl;
    if(fileFormChooseBox->currentIndex()==0){
        tproject->read(
                    editFile1->text().toLatin1().data(),
                    editFile2->text().toLatin1().data(),
                    editPhe->text().toLatin1().data(),
                    editCov->text().toLatin1().data(),
                    editKin->text().toLatin1().data()
                    );
    }
    else if(fileFormChooseBox->currentIndex()==1){
        tproject->readVCF(
                    editFile1->text().toLatin1().data(),
                    editPhe->text().toLatin1().data(),
                    editCov->text().toLatin1().data(),
                    editKin->text().toLatin1().data()
                    );
    }
    else if(fileFormChooseBox->currentIndex()==2){
        tproject->readTPolyPed(
                    editFile1->text().toLatin1().data(),
                    editFile2->text().toLatin1().data(),
                    editPhe->text().toLatin1().data(),
                    editCov->text().toLatin1().data(),
                    editKin->text().toLatin1().data()
                    );
    }
    std::cout << "read 2" << std::endl;
    if(tproject->isValid) {
        pUnitInfo->addNumber(tproject->getNSample(),"Samples",":/icon/n_phenotypeIcon.png");
        pUnitInfo->addNumber(tproject->getNMarker(),"Markers",":/icon/n_markersIcon.png" );
        pUnitInfo->addNumber(tproject->getNFactor(),"Factors",":/icon/n_covariateIcon.png");
        pUnitInfo->state=RUN_UNHANDLED;
    }
    else{
        switch (tproject->state) {
            case FD::FD_READINGERROR:
                QMessageBox::critical(this,"error","file reading error.");
                break;
            case FD::FD_N_SAMPLEERROR:
                QMessageBox::critical(this,"error","the sample number is conflicted.");
                break;
            case FD::FD_N_MARKERERROR:
                QMessageBox::critical(this,"error","the markers number is conflicted.");
                break;
            case FD::FD_N_FACTORERROR:
                QMessageBox::critical(this,"error","the factors number is inappropriate.");
                break;
            default: break;
        }
        FD::deleteProject(tproject);
        (*pProjectList)[row]=nullptr;
        pUnitInfo->clear();
    }
    std::cout << "read 3" << std::endl;
    emit loadOK(row);
    close();
}

void LoadGWASFileWidget::viewFileFormat()
{
    ViewFileFormatWidget* pWidget = nullptr;
    ViewFileFormatTable* pTable = nullptr;

    if(fileFormChooseBox->currentIndex() == 0){
        pTable = new ViewFileFormatTable();
        QVector<FD::FileFormClass> vec {FD::FILEFORM_BED, FD::FILEFORM_BIM};
        pTable->load(vec);
        pTable->show();
    } else if (fileFormChooseBox->currentIndex() == 1){
        pWidget = new ViewFileFormatWidget();
        pWidget->load(FD::FILEFORM_VCF);
        pWidget->show();
    } else if (fileFormChooseBox->currentIndex() == 2){
        pTable = new ViewFileFormatTable();
        QVector<FD::FileFormClass> vec {FD::FILEFORM_PPED, FD::FILEFORM_BIM};
        pTable->load(vec);
        pTable->show();
    }
}

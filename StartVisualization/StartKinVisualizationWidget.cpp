#include "StartKinVisualizationWidget.hpp"

StartKinVisualizationWidget::StartKinVisualizationWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Load Kinship Heatmap File");

    QLabel* chooseFileLabel = new QLabel("Please choose individuals annotation file (.fam):",this);
    QLabel* chooseKinLabel = new QLabel("Please choose kinship matrix file:",this);
    QWidget* openWidget = new QWidget(this);
    QWidget* openKinWidget = new QWidget(this);
    QWidget* buttomBtnWidget=new QWidget(this);
    QHBoxLayout* openLayout = new QHBoxLayout(openWidget);
    QHBoxLayout* openKinLayout = new QHBoxLayout(openKinWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* buttomBtnLayout=new QHBoxLayout(buttomBtnWidget);
    QPushButton* openBtn = new QPushButton(this);
    QPushButton* openKinBtn = new QPushButton(this);
    filePathEdit = new QLineEdit(this);
    kinPathEdit  = new QLineEdit(this);
    continueBtn  = new QPushButton("continue",this);
    cancelBtn    = new QPushButton("cancel",this);

    buttomBtnLayout->addWidget(continueBtn);
    buttomBtnLayout->addWidget(cancelBtn);
    openLayout->addWidget(filePathEdit);
    openLayout->addWidget(openBtn);
    openKinLayout->addWidget(kinPathEdit);
    openKinLayout->addWidget(openKinBtn);
    mainLayout->addWidget(chooseFileLabel);
    mainLayout->addWidget(openWidget);
    mainLayout->addWidget(chooseKinLabel);
    mainLayout->addWidget(openKinWidget);
    mainLayout->addWidget(buttomBtnWidget);

    openBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openBtn->setFixedWidth(25);
    openKinBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openKinBtn->setFixedWidth(25);

    connect(openBtn,&QPushButton::clicked,this,&StartKinVisualizationWidget::openFile);
    connect(openKinBtn,&QPushButton::clicked,this,&StartKinVisualizationWidget::openKin);
    connect(continueBtn,&QPushButton::clicked,this,&StartKinVisualizationWidget::createVisualize);
    connect(cancelBtn,&QPushButton::clicked,this,&StartKinVisualizationWidget::close);

}

StartKinVisualizationWidget::~StartKinVisualizationWidget()
{

}

void StartKinVisualizationWidget::openFile()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose individuals annotation file (.fam)");
    if(!filePath.isEmpty()) filePathEdit->setText(filePath);
}

void StartKinVisualizationWidget::openKin()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose kinship matrix file");
    if(!filePath.isEmpty()) kinPathEdit->setText(filePath);
}

void StartKinVisualizationWidget::createVisualize()
{
    FD::FAMread reader;
//    char** sampleName=nullptr;
//    size_t nSample,i;
    if(!QFile::exists(filePathEdit->text())){
        QMessageBox::critical(this,"error","the .fam file don\'t exist");
        return;
    }
    if(!QFile::exists(kinPathEdit->text())){
        QMessageBox::critical(this,"error","the kinship matrix file don\'t exist");
        return;
    }
    reader.load(filePathEdit->text().toLatin1().data());
    reader.readAll();
    if(!reader.isValid){
        QMessageBox::critical(this,"error","reading .fam file error");
        return;
    }
    if(!FD::kinRead(kinPathEdit->text().toLatin1().data(),kin)){
        QMessageBox::critical(this,"error","reading kinship matrix file error");
        return;
    }
    if(kin.getNRow()!=reader.sampleN||
       kin.getNCol()!=reader.sampleN){
        QMessageBox::critical(this,"error","the number of individuals in .fam file is not equal to PCA result");
        return;
    }
    char** inName=new char*[reader.sampleN];
    for(size_t i=0;i<reader.sampleN;++i){
        inName[i]=new char[FD::MAXELEMLEN];
        FD::charCopy(inName[i],reader.getSampleName(i));
    }
    KinshipVisualizeWidget* pWidget=new KinshipVisualizeWidget(kin,inName);
    pWidget->show();
    if(inName){
        for(size_t i=0;i<reader.sampleN;++i) delete [] inName[i];
        delete [] inName;
    }
//    if(sampleName){
//        for(i=0;i<nSample;++i){
//            delete [] sampleName[i];
//        }
//        delete [] sampleName;
//    }
    close();
}

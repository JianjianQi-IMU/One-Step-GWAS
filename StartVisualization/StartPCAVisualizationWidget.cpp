#include "StartPCAVisualizationWidget.hpp"

StartPCAVisualizationWidget::StartPCAVisualizationWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Load PCA Plot File");

    QLabel* chooseFileLabel = new QLabel("Please choose individuals annotation file (.fam):",this);
    QLabel* choosePCALabel = new QLabel("Please choose PCA output file:",this);
    QButtonGroup* displayClassGroup = new QButtonGroup(this);
    QGroupBox* displayClassBox = new QGroupBox("display",this);
    QHBoxLayout* displayClassLayout = new QHBoxLayout(displayClassBox);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QWidget* chooseFileWidget = new QWidget(this);
    QWidget* choosePCAWidget = new QWidget(this);
    QWidget* buttomBtnWidget=new QWidget(this);
    QHBoxLayout* chooseFileLayout = new QHBoxLayout(chooseFileWidget);
    QHBoxLayout* choosePCALayout = new QHBoxLayout(choosePCAWidget);
    QHBoxLayout* buttomBtnLayout=new QHBoxLayout(buttomBtnWidget);
    QPushButton* openBtn = new QPushButton(this);
    QPushButton* openPCABtn = new QPushButton(this);
    filePathEdit = new QLineEdit(this);
    PCAPathEdit  = new QLineEdit(this);
    continueBtn = new QPushButton("continue",this);
    cancelBtn = new QPushButton("cancel",this);
    D2Radio = new QRadioButton("2D",this);
    D3Radio = new QRadioButton("3D",this);

    openBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openBtn->setFixedWidth(25);
    openPCABtn->setIcon(QIcon(":/icon/openIcon.png"));
    openPCABtn->setFixedWidth(25);
    displayClassGroup->setExclusive(true);
    chooseFileLayout->addWidget(filePathEdit);
    chooseFileLayout->addWidget(openBtn);
    choosePCALayout->addWidget(PCAPathEdit);
    choosePCALayout->addWidget(openPCABtn);
    displayClassGroup->addButton(D2Radio);
    displayClassGroup->addButton(D3Radio);
    displayClassLayout->addWidget(D2Radio);
    displayClassLayout->addWidget(D3Radio);
    buttomBtnLayout->addWidget(continueBtn);
    buttomBtnLayout->addWidget(cancelBtn);

    mainLayout->addWidget(chooseFileLabel);
    mainLayout->addWidget(chooseFileWidget);
    mainLayout->addWidget(choosePCALabel);
    mainLayout->addWidget(choosePCAWidget);
    mainLayout->addWidget(displayClassBox);
    mainLayout->addWidget(buttomBtnWidget);

    D2Radio->setChecked(true);
    connect(openBtn,&QPushButton::clicked,this,&StartPCAVisualizationWidget::openFile);
    connect(openPCABtn,&QPushButton::clicked,this,&StartPCAVisualizationWidget::openPCA);
    connect(continueBtn,&QPushButton::clicked,this,&StartPCAVisualizationWidget::createVisualize);
    connect(cancelBtn,&QPushButton::clicked,this,&StartPCAVisualizationWidget::close);
}

StartPCAVisualizationWidget::~StartPCAVisualizationWidget()
{

}

void StartPCAVisualizationWidget::openFile()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose individuals annotation file (.fam)");
    if(!filePath.isEmpty()) filePathEdit->setText(filePath);
}

void StartPCAVisualizationWidget::openPCA()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose PCA output file");
    if(!filePath.isEmpty()) PCAPathEdit->setText(filePath);
}

void StartPCAVisualizationWidget::createVisualize()
{
    FD::FAMread reader;
    PCA3DVisualizationWidget* D3Visual=nullptr;
    PCA2DVisualizationWidget* D2Visual=nullptr;
    MML::Mat PCs;
    char** sampleName=nullptr;
    size_t nSample,i;
    if(!QFile::exists(filePathEdit->text())){
        QMessageBox::critical(this,"error","the .file file don\'t exist");
        return;
    }
    if(!QFile::exists(PCAPathEdit->text())){
        QMessageBox::critical(this,"error","the PCA output file don\'t exist");
        return;
    }
    reader.load(filePathEdit->text().toLatin1().data());
    reader.readAll();
    if(!reader.isValid){
        QMessageBox::critical(this,"error","reading .file file error");
        return;
    }
    if(D3Radio->isChecked()){
        if(!FD::PCAOutread(PCAPathEdit->text().toLatin1().data(),PCs,3)){
            QMessageBox::critical(this,"error","reading PCA output file error");
            return;
        }
    }
    else{
        if(!FD::PCAOutread(PCAPathEdit->text().toLatin1().data(),PCs,2)){
            QMessageBox::critical(this,"error","reading PCA output file error");
            return;
        }
    }
    if(PCs.getNRow()!=reader.sampleN){
        QMessageBox::critical(this,"error","the number of individuals in .fam file is not equal to PCA result");
        return;
    }
    nSample=PCs.getNRow();
    sampleName=new char*[nSample];
    for(i=0;i<nSample;++i){
        sampleName[i]=new char[FD::MAXELEMLEN];
        FD::charCopy(sampleName[i],reader.getSampleName(i));
    }
    if(D3Radio->isChecked()){
        qDebug() << "-> 1";
        D3Visual=new PCA3DVisualizationWidget(PCs.data,reader.popIndex,sampleName,PCs.getNRow());
        qDebug() << "-> 2";
        D3Visual->setPopName(reader.popName,reader.nPop);
        qDebug() << "-> 3";
        D3Visual->show();
    }
    else{
        D2Visual=new PCA2DVisualizationWidget(PCs.data,reader.popIndex,sampleName,PCs.getNRow());
        D2Visual->setPopName(reader.popName,reader.nPop);
        D2Visual->show();
    }
    if(sampleName){
        for(i=0;i<nSample;++i){
            delete [] sampleName[i];
        }
        delete [] sampleName;
    }
    close();
}

#include "StartStrucVisualizationWidget.hpp"

StartStrucVisualizationWidget::StartStrucVisualizationWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Load Structure Plot File");

    QLabel* chooseFileLabel = new QLabel("Please choose individuals annotation file (.fam):",this);
    QLabel* chooseStrucLabel = new QLabel("Please choose structure matrix file:",this);
    QWidget* openWidget = new QWidget(this);
    QWidget* openStrucWidget = new QWidget(this);
    QWidget* buttomBtnWidget=new QWidget(this);
    QHBoxLayout* openLayout = new QHBoxLayout(openWidget);
    QHBoxLayout* openStrucLayout = new QHBoxLayout(openStrucWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* buttomBtnLayout=new QHBoxLayout(buttomBtnWidget);
    QPushButton* openBtn = new QPushButton(this);
    QPushButton* openStrucBtn = new QPushButton(this);
    filePathEdit = new QLineEdit(this);
    strucPathEdit= new QLineEdit(this);
    continueBtn  = new QPushButton("continue",this);
    cancelBtn    = new QPushButton("cancel",this);

    buttomBtnLayout->addWidget(continueBtn);
    buttomBtnLayout->addWidget(cancelBtn);
    openLayout->addWidget(filePathEdit);
    openLayout->addWidget(openBtn);
    openStrucLayout->addWidget(strucPathEdit);
    openStrucLayout->addWidget(openStrucBtn);
    mainLayout->addWidget(chooseFileLabel);
    mainLayout->addWidget(openWidget);
    mainLayout->addWidget(chooseStrucLabel);
    mainLayout->addWidget(openStrucWidget);
    mainLayout->addWidget(buttomBtnWidget);

    openBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openBtn->setFixedWidth(25);
    openStrucBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openStrucBtn->setFixedWidth(25);

    connect(openBtn,&QPushButton::clicked,this,&StartStrucVisualizationWidget::openFile);
    connect(openStrucBtn,&QPushButton::clicked,this,&StartStrucVisualizationWidget::openStruc);
    connect(continueBtn,&QPushButton::clicked,this,&StartStrucVisualizationWidget::createVisualize);
    connect(cancelBtn,&QPushButton::clicked,this,&StartStrucVisualizationWidget::close);

}

StartStrucVisualizationWidget::~StartStrucVisualizationWidget()
{

}

void StartStrucVisualizationWidget::openFile()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose individuals annotation file (.fam)");
    if(!filePath.isEmpty()) filePathEdit->setText(filePath);
}

void StartStrucVisualizationWidget::openStruc()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose structure matrix file");
    if(!filePath.isEmpty()) strucPathEdit->setText(filePath);
}

void StartStrucVisualizationWidget::createVisualize()
{
    FD::FAMread reader;
    if(!QFile::exists(filePathEdit->text())){
        QMessageBox::critical(this,"error","the .fam file don\'t exist");
        return;
    }
    if(!QFile::exists(strucPathEdit->text())){
        QMessageBox::critical(this,"error","the structure matrix file don\'t exist");
        return;
    }
    reader.load(filePathEdit->text().toLatin1().data());
    reader.readAll();
    if(!reader.isValid){
        QMessageBox::critical(this,"error","reading .fam file error");
        return;
    }
    if(!FD::covRead(strucPathEdit->text().toLatin1().data(),struc)){
        QMessageBox::critical(this,"error","reading structure matrix file error");
        return;
    }
    if(struc.getNRow()!=reader.sampleN){
        QMessageBox::critical(this,"error","the number of individuals in .fam file is not equal to PCA result");
        return;
    }
    char** inName=new char*[reader.sampleN];
    for(size_t i=0;i<reader.sampleN;++i){
        inName[i]=new char[FD::MAXELEMLEN];
        FD::charCopy(inName[i],reader.getSampleName(i));
    }
    StructureVisualizeWidget* pWidget=new StructureVisualizeWidget(struc,inName);

    pWidget->show();
    for(size_t i=0;i<reader.sampleN;++i) delete [] inName[i];
    delete [] inName;
    close();
}

#include "PreStrucVisualizeWidget.hpp"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "StructureVisualize/StructureVisualizeWidget.hpp"

PreStrucVisualizeWidget::PreStrucVisualizeWidget(QWidget *parent)
    :QWidget(parent)
{
    filePathEdit = nullptr;
    openBtn      = nullptr;
    continueBtn  = nullptr;
    cancelBtn    = nullptr;

    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Structure Visualization");
}

PreStrucVisualizeWidget::PreStrucVisualizeWidget(MML::Mat &inStruc, QWidget *parent)
    :PreStrucVisualizeWidget(parent)
{
    QLabel* chooseFileLabel = new QLabel("Please choose individuals annotation file (.fam):",this);
    QWidget* openWidget = new QWidget(this);
    QWidget* buttomBtnWidget=new QWidget(this);
    QHBoxLayout* openLayout = new QHBoxLayout(openWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* buttomBtnLayout=new QHBoxLayout(buttomBtnWidget);
    filePathEdit = new QLineEdit(this);
    openBtn      = new QPushButton(this);
    continueBtn  = new QPushButton("continue",this);
    cancelBtn    = new QPushButton("cancel",this);

    buttomBtnLayout->addWidget(continueBtn);
    buttomBtnLayout->addWidget(cancelBtn);
    openLayout->addWidget(filePathEdit);
    openLayout->addWidget(openBtn);
    mainLayout->addWidget(chooseFileLabel);
    mainLayout->addWidget(openWidget);
    mainLayout->addWidget(buttomBtnWidget);

    openBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openBtn->setFixedWidth(25);

    connect(openBtn,&QPushButton::clicked,this,&PreStrucVisualizeWidget::openFile);
    connect(continueBtn,&QPushButton::clicked,this,&PreStrucVisualizeWidget::createVisualize);
    connect(cancelBtn,&QPushButton::clicked,this,&PreStrucVisualizeWidget::close);

    struc=inStruc;
}

PreStrucVisualizeWidget::~PreStrucVisualizeWidget()
{

}

void PreStrucVisualizeWidget::openFile()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose individuals annotation file (.fam)");
    if(!filePath.isEmpty()) filePathEdit->setText(filePath);
}

void PreStrucVisualizeWidget::createVisualize()
{
    FD::FAMread reader;
    if(!QFile::exists(filePathEdit->text())){
        QMessageBox::critical(this,"error","the file don\'t exist");
        return;
    }
    reader.load(filePathEdit->text().toLatin1().data());
    reader.readAll();
    if(!reader.isValid){
        QMessageBox::critical(this,"error","the file reading error");
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

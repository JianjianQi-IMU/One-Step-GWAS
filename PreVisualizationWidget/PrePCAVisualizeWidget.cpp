#include "PrePCAVisualizeWidget.hpp"

#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QMessageBox>
#include <QFileDialog>

#include "PCAVisualize/PCA3DVisualizationWidget.hpp"
#include "PCAVisualize/PCA2DVisualizationWidget.hpp"

PrePCAVisualizeWidget::PrePCAVisualizeWidget(QWidget *parent)
    :QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle("PCA visualization");

    filePathEdit = nullptr;
    openBtn      = nullptr;
    continueBtn  = nullptr;
    cancelBtn    = nullptr;
    D2Radio      = nullptr;
    D3Radio      = nullptr;

    PC1VarRatio = 0;
    PC2VarRatio = 0;
    PC3VarRatio = 0;
}

PrePCAVisualizeWidget::PrePCAVisualizeWidget(MML::Mat *inPCs, QWidget *parent)
    :PrePCAVisualizeWidget(parent)
{
    if(inPCs->getNCol()<3) return;
    PCs=(*inPCs)(0,0,inPCs->getNRow()-1,2);
    QLabel* chooseFileLabel = new QLabel("Please choose individuals annotation file (.fam):",this);
    QButtonGroup* displayClassGroup = new QButtonGroup(this);
    QGroupBox* displayClassBox = new QGroupBox("display",this);
    QHBoxLayout* displayClassLayout = new QHBoxLayout(displayClassBox);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QWidget* chooseFileWidget = new QWidget(this);
    QWidget* buttomBtnWidget=new QWidget(this);
    QHBoxLayout* chooseFileLayout = new QHBoxLayout(chooseFileWidget);
    QHBoxLayout* buttomBtnLayout=new QHBoxLayout(buttomBtnWidget);
    filePathEdit = new QLineEdit(this);
    openBtn = new QPushButton(this);
    continueBtn = new QPushButton("continue",this);
    cancelBtn = new QPushButton("cancel",this);
    D2Radio = new QRadioButton("2D",this);
    D3Radio = new QRadioButton("3D",this);

    openBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openBtn->setFixedWidth(25);
    displayClassGroup->setExclusive(true);
    chooseFileLayout->addWidget(filePathEdit);
    chooseFileLayout->addWidget(openBtn);
    displayClassGroup->addButton(D2Radio);
    displayClassGroup->addButton(D3Radio);
    displayClassLayout->addWidget(D2Radio);
    displayClassLayout->addWidget(D3Radio);
    buttomBtnLayout->addWidget(continueBtn);
    buttomBtnLayout->addWidget(cancelBtn);

    mainLayout->addWidget(chooseFileLabel);
    mainLayout->addWidget(chooseFileWidget);
    mainLayout->addWidget(displayClassBox);
    mainLayout->addWidget(buttomBtnWidget);

    D2Radio->setChecked(true);
    connect(openBtn,&QPushButton::clicked,this,&PrePCAVisualizeWidget::openFile);
    connect(continueBtn,&QPushButton::clicked,this,&PrePCAVisualizeWidget::createVisualize);
    connect(cancelBtn,&QPushButton::clicked,this,&PrePCAVisualizeWidget::close);
}

PrePCAVisualizeWidget::~PrePCAVisualizeWidget()
{

}

bool PrePCAVisualizeWidget::setVarRatio(double inPC1VarRatio, double inPC2VarRatio, double inPC3VarRatio)
{
    PC1VarRatio = inPC1VarRatio;
    PC2VarRatio = inPC2VarRatio;
    PC3VarRatio = inPC3VarRatio;
    return true;
}

void PrePCAVisualizeWidget::openFile()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose individuals annotation file (.fam)");
    if(!filePath.isEmpty()) filePathEdit->setText(filePath);
}

void PrePCAVisualizeWidget::createVisualize()
{
    FD::FAMread reader;
    PCA3DVisualizationWidget* D3Visual=nullptr;
    PCA2DVisualizationWidget* D2Visual=nullptr;
    char** sampleName=nullptr;
    long long nSample,i;
    MML::Mat PCs2D;
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
        D3Visual=new PCA3DVisualizationWidget(PCs.data,reader.popIndex,sampleName,(long long)PCs.getNRow());
        D3Visual->setPopName(reader.popName,reader.nPop);
        D3Visual->setVarRatio(PC1VarRatio,PC2VarRatio,PC3VarRatio);
        D3Visual->show();
    }
    else{
        PCs2D=PCs(0,0,PCs.getNRow()-1,1);
        D2Visual=new PCA2DVisualizationWidget(PCs2D.data,reader.popIndex,sampleName,(long long)PCs2D.getNRow());
        D2Visual->setPopName(reader.popName,reader.nPop);
        D2Visual->setVarRatio(PC1VarRatio,PC2VarRatio);
        D2Visual->show();
    }
    for(i=0;i<nSample;++i){
        delete [] sampleName[i];
    }
    delete [] sampleName;
    close();
}

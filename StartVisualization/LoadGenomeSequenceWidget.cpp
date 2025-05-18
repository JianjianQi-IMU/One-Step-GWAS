#include "LoadGenomeSequenceWidget.hpp"

LoadGenomeSequenceWidget::LoadGenomeSequenceWidget(QWidget* parent)
    :QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);

    continueBtn  = new QPushButton("continue",this);
    cancelBtn    = new QPushButton("cancel",this);
    openBtn      = new QPushButton(this);
    filePathEdit = new QLineEdit(this);
    QLabel*  chooseText = new QLabel("please choose the path of genome sequence file:",this);
    QWidget* openWidget = new QWidget(this);
    QWidget* downWidget = new QWidget(this);
    QHBoxLayout* openLayout = new QHBoxLayout(openWidget);
    QHBoxLayout* downLayout = new QHBoxLayout(downWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    openLayout->addWidget(filePathEdit);
    openLayout->addWidget(openBtn);
    downLayout->addWidget(continueBtn);
    downLayout->addWidget(cancelBtn);
    mainLayout->addWidget(chooseText);
    mainLayout->addWidget(openWidget);
    mainLayout->addWidget(downWidget);
    openBtn->setIcon(QIcon(":/icon/openIcon.png"));
    openBtn->setFixedWidth(25);
    connect(openBtn,&QPushButton::clicked,this,&LoadGenomeSequenceWidget::openFile);
    connect(cancelBtn,&QPushButton::clicked,this,&LoadGenomeSequenceWidget::close);
    connect(continueBtn,&QPushButton::clicked,this,&LoadGenomeSequenceWidget::loadGenomeSeqFile);
}

LoadGenomeSequenceWidget::~LoadGenomeSequenceWidget()
{

}

void LoadGenomeSequenceWidget::loadGenomeSeqFile()
{
    QFile tFile(filePathEdit->text());
    if(!tFile.exists()){
        QMessageBox::critical(this,"error!","the path of genome sequence is incorrect.");
        return ;
    }
    emit sendFilePath(filePathEdit->text());
    close();
}

void LoadGenomeSequenceWidget::openFile()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName(this,"choose genome sequence file.");
    if(!filePath.isEmpty()) filePathEdit->setText(filePath);
}

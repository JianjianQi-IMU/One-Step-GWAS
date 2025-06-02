#include "LoadBSAFileWidget.hpp"

#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "FileDeal/ViewFileFormatWidget.hpp"

LoadBSAFileWidget::LoadBSAFileWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Load");

    continueBtn       = nullptr;
    cancelBtn         = nullptr;
    fileFormBtn       = nullptr;
    openMarkerBtn     = nullptr;
    openChrLenBtn     = nullptr;
    fileFormChooseBox = nullptr;
    editMarker        = nullptr;
    editChrLen        = nullptr;
    labelMarker       = nullptr;
    labelChrLen       = nullptr;
    pUnitInfo         = nullptr;
    pProjectList      = nullptr;
}

LoadBSAFileWidget::LoadBSAFileWidget(AnalyzeUnitInfo *inUnitInfo, QList<FD::BaseProject *> *inProjectList, int inRow, QWidget *parent)
    : LoadBSAFileWidget(parent)
{
    continueBtn       = new QPushButton("continue", this);
    cancelBtn         = new QPushButton("cancel", this);
    fileFormBtn       = new QPushButton(QIcon(":/icon/openIcon.png"), QString(), this);
    openMarkerBtn     = new QPushButton(QIcon(":/icon/openIcon.png"), QString(), this);
    openChrLenBtn     = new QPushButton(QIcon(":/icon/openIcon.png"), QString(), this);
    fileFormChooseBox = new QComboBox(this);
    editMarker        = new QLineEdit(this);
    editChrLen        = new QLineEdit(this);
    labelMarker       = new QLabel("Load markers data, and choose file format please.", this);
    labelChrLen       = new QLabel("Load chromosome length file please.", this);

    QVBoxLayout* mainLayout=new QVBoxLayout(this);

    QGroupBox* fileBox = new QGroupBox("Input",this);
    QWidget* markerWidget = new QWidget(this);
    QWidget* chrLenWidget = new QWidget(this);
    QWidget* fileFormWidget = new QWidget(this);
    QHBoxLayout* markerLayout = new QHBoxLayout(markerWidget);
    QHBoxLayout* chrLenLayout = new QHBoxLayout(chrLenWidget);
    QHBoxLayout* fileFormLayout = new QHBoxLayout(fileFormWidget);
    QVBoxLayout* fileBoxLayout = new QVBoxLayout(fileBox);
    markerLayout->addWidget(editMarker);
    markerLayout->addWidget(openMarkerBtn);
    chrLenLayout->addWidget(editChrLen);
    chrLenLayout->addWidget(openChrLenBtn);
    fileFormLayout->addWidget(fileFormChooseBox);
    fileFormLayout->addWidget(fileFormBtn);
    fileBoxLayout->addWidget(labelMarker);
    fileBoxLayout->addWidget(markerWidget);
    fileBoxLayout->addWidget(fileFormWidget);
    fileBoxLayout->addWidget(labelChrLen);
    fileBoxLayout->addWidget(chrLenWidget);

    QWidget* downWidget = new QWidget(this);
    QHBoxLayout* downLayout = new QHBoxLayout(downWidget);
    downLayout->addWidget(continueBtn);
    downLayout->addWidget(cancelBtn);

    mainLayout->addWidget(fileBox);
    mainLayout->addWidget(downWidget);

    fileFormChooseBox->addItem("Variant Call Format (.vcf)");
    fileFormChooseBox->addItem("Simplified Allele Depth Format (4 column)");
    fileFormChooseBox->addItem("Simplified Allele Depth Format (6 column)");

    openMarkerBtn->setFixedWidth(25);
    openChrLenBtn->setFixedWidth(25);
    fileFormBtn->setFixedWidth(25);

    connect(continueBtn, &QPushButton::clicked, this, &LoadBSAFileWidget::save);
    connect(cancelBtn, &QPushButton::clicked, this, &LoadBSAFileWidget::close);
    connect(openMarkerBtn, &QPushButton::clicked, this, &LoadBSAFileWidget::loadMarkerFile);
    connect(openChrLenBtn, &QPushButton::clicked, this, &LoadBSAFileWidget::loadChrLenFile);
    connect(fileFormBtn, &QPushButton::clicked, this, &LoadBSAFileWidget::viewFileForm);

    setUnitInfo(inUnitInfo);
    pProjectList = inProjectList;
    row = inRow;
}

LoadBSAFileWidget::~LoadBSAFileWidget()
{

}

void LoadBSAFileWidget::setUnitInfo(AnalyzeUnitInfo *inUnitInfo)
{
    pUnitInfo = inUnitInfo;
}

void LoadBSAFileWidget::loadMarkerFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "open markers file");
    if (fileName.isEmpty()) {
        return ;
    }
    QFile tFile(fileName);
    if (!tFile.exists()) {
        QMessageBox::warning(this, "Cannot find", "The markers file can\'t be found.");
        return ;
    }
    editMarker->setText(fileName);
}

void LoadBSAFileWidget::loadChrLenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "open chromosome length file");
    if (fileName.isEmpty()) {
        return ;
    }
    QFile tFile(fileName);
    if (!tFile.exists()) {
        QMessageBox::warning(this, "Cannot find", "The chromosome length file can\'t be found.");
        return ;
    }
    editChrLen->setText(fileName);
}

void LoadBSAFileWidget::viewFileForm()
{
    int fIndex = fileFormChooseBox->currentIndex();
    ViewFileFormatWidget* pWidget = new ViewFileFormatWidget();
    switch (fIndex) {
        case 0:
            pWidget->load(FD::FileFormatSet::getFileFormat(FD::FILEFORM_VCF));
            break;
        case 1:
            break;
        case 2:
            break;
        default:
            break;
    }
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
}

void LoadBSAFileWidget::save()
{
    QString mFileName = editMarker->text();
    QString cFileName = editChrLen->text();
    QFile mFile(mFileName), cFile(cFileName);
    bool fileValid = false;
    if (!mFile.exists()) {
        QMessageBox::warning(this,"Cannot find","The markers file can\'t be found.");
        return ;
    }
    if(!cFile.exists()){
        QMessageBox::warning(this,"Cannot find","The chromosome length file can\'t be found.");
        return ;
    }
    if ((*pProjectList)[row]) {
        FD::deleteProject((*pProjectList)[row]);
    }
    FD::BSAProject* tproject = new FD::BSAProject;
    (*pProjectList)[row] = tproject;
    pUnitInfo->clear();
    pUnitInfo->addFile(editMarker->text(), "marker", ":/icon/file.png");
    pUnitInfo->addFile(editChrLen->text(), "chr", ":/icon/file.png");
    pUnitInfo->state=RUN_UNHANDLED;
    // 0:.vcf 1:specific1 2:specific2
    int fIndex = fileFormChooseBox->currentIndex();
    switch (fIndex) {
        case 0:
            fileValid = tproject->readFileVCF(mFileName.toLatin1().data());
            break;
        case 1:
            fileValid = tproject->readFile1(mFileName.toLatin1().data());
            break;
        case 2:
            break;
        default:
            break;
    }
    if (!fileValid) {
        QMessageBox::warning(this, "error", "Reading the markers file error.");
        FD::deleteProject(tproject);
        (*pProjectList)[row] = nullptr;
        return ;
    }
    fileValid = tproject->readChrFile(cFileName.toLatin1().data());
    if (!fileValid) {
        QMessageBox::warning(this, "error", "Reading the chromosome length file error.");
        FD::deleteProject(tproject);
        (*pProjectList)[row] = nullptr;
        return ;
    }
    tproject->sortPoints();
    pUnitInfo->addNumber(tproject->data.getNRow(), "Markers", ":/icon/n_markersIcon.png");
    emit loadOK(row);
    close();
}

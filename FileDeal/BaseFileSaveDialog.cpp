#include "BaseFileSaveDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QGroupBox>

#include "WidgetUtil/WidgetUtil.hpp"

namespace FD {

BaseFileSaveDialog::BaseFileSaveDialog(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle("Save");

    chooseSavePathBtn          = nullptr;
    saveBtn                    = nullptr;
    cancelBtn                  = nullptr;
    fileFormLook               = nullptr;
    savePathEdit               = nullptr;
    widthEdit                  = nullptr;
    heightEdit                 = nullptr;
    fileFormBox                = nullptr;
    sizeDisplayWidget          = nullptr;
    sizeValidator              = nullptr;
}

QWidget *BaseFileSaveDialog::init_SaveAndCancel()
{
    if(saveBtn != nullptr){
        delete saveBtn;
    }
    if(cancelBtn != nullptr){
        delete cancelBtn;
    }
    saveBtn = new QPushButton("Save",this);
    cancelBtn = new QPushButton("Cancel",this);
    QWidget* outWidget = new QWidget(this);
    QHBoxLayout* outLayout = new QHBoxLayout(outWidget);
    outLayout -> addWidget(saveBtn);
    outLayout -> addWidget(cancelBtn);

    saveBtn -> setFixedWidth(100);
    cancelBtn -> setFixedWidth(100);

    connect(saveBtn,&QPushButton::clicked,
            this,&BaseFileSaveDialog::do_Save);
    connect(cancelBtn,&QPushButton::clicked,
            this,&BaseFileSaveDialog::close);

    return outWidget;
}

QWidget *BaseFileSaveDialog::init_SavePath()
{
    if(savePathEdit != nullptr){
        delete savePathEdit;
    }
    if(chooseSavePathBtn != nullptr){
        delete chooseSavePathBtn;
    }
    savePathEdit = new QLineEdit(this);
    chooseSavePathBtn = new QPushButton(QIcon(":/icon/openIcon.png"),"",this);
    chooseSavePathBtn -> setFixedWidth(25);
    QLabel *pLabel = new QLabel("Save Path:",this);
    QWidget* outWidget = new QWidget(this);
    QHBoxLayout* outLayout = new QHBoxLayout(outWidget);
    outLayout -> addWidget(pLabel);
    outLayout -> addWidget(savePathEdit);
    outLayout -> addWidget(chooseSavePathBtn);
    outWidget -> setFixedHeight(54);

    connect(chooseSavePathBtn,&QPushButton::clicked,
            this,&BaseFileSaveDialog::do_OpenSavePath);

    return outWidget;
}

QWidget *BaseFileSaveDialog::init_FileFormChoose()
{
    if(fileFormLook != nullptr){
        delete fileFormLook;
    }
    if(fileFormBox != nullptr){
        delete fileFormBox;
    }
    fileFormLook = new QPushButton(QIcon(":/icon/openIcon.png"),"",this);
    fileFormLook -> setFixedWidth(25);
    fileFormBox = new QComboBox(this);
    QLabel *pLabel = new QLabel("File Format:",this);
    QWidget* outWidget = new QWidget(this);
    QHBoxLayout* outLayout = new QHBoxLayout(outWidget);
    int pLabelW = WidgetUtil::GetQLabelDefWidth("File Format:", "Times New Roman", -1);
    pLabel->setFixedWidth(pLabelW);
    outLayout -> addWidget(pLabel);
    outLayout -> addWidget(fileFormBox);
    outLayout -> addWidget(fileFormLook);
    outWidget -> setFixedHeight(54);

    connect(fileFormLook,&QPushButton::clicked,
            this,&BaseFileSaveDialog::do_LookFileForm);
    connect(fileFormBox,QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,&BaseFileSaveDialog::do_FileFormChanged);

    return outWidget;
}

QWidget *BaseFileSaveDialog::init_SizeSettings(bool flag)
{
    if(widthEdit != nullptr){
        delete widthEdit;
    }
    if(heightEdit != nullptr){
        delete heightEdit;
    }
    if(sizeValidator != nullptr){
        delete sizeValidator;
    }
    sizeValidator = new QRegExpValidator(QRegExp("[\\d,.]*"), this);
    widthEdit  = new QLineEdit(this);
    heightEdit = new QLineEdit(this);
    QLabel *preLabel = new QLabel("(Width x Height):",this);
    QLabel *mutiLabel = new QLabel(" x ",this);
    QWidget* outWidget = new QWidget(this);
    QHBoxLayout* outLayout = new QHBoxLayout(outWidget);
    int preLabelW = WidgetUtil::GetQLabelDefWidth("(Width x Height):", "Times New Roman", -1);
    int mutiLabelW = WidgetUtil::GetQLabelDefWidth(" x ", "Times New Roman", -1);
    preLabel->setFixedWidth(preLabelW);
    mutiLabel->setFixedWidth(mutiLabelW);
    outLayout -> addWidget(preLabel);
    outLayout -> addWidget(widthEdit);
    outLayout -> addWidget(mutiLabel);
    outLayout -> addWidget(heightEdit);
    // widthEdit -> setFixedWidth(64);
    // heightEdit -> setFixedWidth(64);
    outWidget -> setFixedHeight(54);

    connect(widthEdit,&QLineEdit::textChanged,
            this,&BaseFileSaveDialog::do_SizeChanged);
    connect(heightEdit,&QLineEdit::textChanged,
            this,&BaseFileSaveDialog::do_SizeChanged);

    widthEdit->setValidator(sizeValidator);
    heightEdit->setValidator(sizeValidator);

    if(flag){
        widthEdit->setEnabled(true);
        heightEdit->setEnabled(true);
    }
    else{
        widthEdit->setEnabled(false);
        heightEdit->setEnabled(false);
    }

    return outWidget;
}

void BaseFileSaveDialog::init_SizeDisplay()
{
    if(widthEdit == nullptr || heightEdit == nullptr){
        return ;
    }
    if(sizeDisplayWidget != nullptr){
        delete sizeDisplayWidget;
    }
    sizeDisplayWidget = new RectSizeDisplayWidget(this);
}

QSize PlotFileSaveDialog::getSize()
{
    if(widthEdit && heightEdit){
        QString wStr = widthEdit->text();
        QString hStr = heightEdit->text();
        wStr.remove(',');
        hStr.remove(',');
        int w = wStr.toInt(), h = hStr.toInt();
        return QSize(w, h);
    }
    return QSize();
}

QSizeF PlotFileSaveDialog::getSizeF()
{
    if(widthEdit && heightEdit){
        QString wStr = widthEdit->text();
        QString hStr = heightEdit->text();
        wStr.remove(',');
        hStr.remove(',');
        double w = wStr.toDouble(), h = hStr.toDouble();
        return QSizeF(w, h);
    }
    return QSizeF();
}

PlotFileSaveDialog::PlotFileSaveDialog(QWidget *parent)
    :BaseFileSaveDialog(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QGroupBox* pGroupBox = new QGroupBox("Save Settings",this);
    QHBoxLayout* pGroupLayout = new QHBoxLayout(pGroupBox);
    QWidget* leftWidget = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    QWidget* rightWidget = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    QWidget* leftDiscardWidget = new QWidget(this);

    leftLayout -> addWidget(init_SizeSettings(true));
    leftLayout -> addWidget(init_FileFormChoose());
    leftLayout -> addWidget(init_SavePath());
    leftLayout -> addWidget(leftDiscardWidget);
    init_SizeDisplay();
    rightLayout -> addWidget(sizeDisplayWidget);
    pGroupLayout -> addWidget(leftWidget);
    pGroupLayout -> addWidget(rightWidget);
    mainLayout -> addWidget(pGroupBox);
    mainLayout -> addWidget(init_SaveAndCancel());
    fileFormBox -> addItem("SVG File (.svg)");
    fileFormBox -> addItem("PNG File (.png)");
    QSizeF tSize = QSizeF(7.5,4.5);
    widthEdit->setText("7.5"), heightEdit->setText("4.5");
    sizeDisplayWidget->setDisplayRectSizeF(tSize);
    // setFixedSize(minimumSize());
}

void PlotFileSaveDialog::setSize(QSize inSize)
{
    widthEdit -> setText(QString::number(inSize.width()));
    heightEdit-> setText(QString::number(inSize.height()));
}

void PlotFileSaveDialog::do_Save()
{
    SaveFileInfo outInfo;
    QString filePath = savePathEdit -> text();
    bool flag = false;
    if(filePath.isEmpty()){
        QMessageBox::critical(this,"error","the path of saving is null");
        return;
    }
    switch (fileFormBox -> currentIndex()) {
    default:
    case 0: {
        SVGFileFormInfo svgInfo;
        QSizeF tSize = getSizeF();
        if(tSize.width() < 1.0 || tSize.height() < 1.0){
            QMessageBox::critical(this,"error","the size of plot is too small");
            break;
        }
        svgInfo.saveSize = tSize;
        svgInfo.filePath = filePath;
        outInfo.setFileForm(&svgInfo);
        emit sendSaveFile(outInfo);
        flag = true;
        break;
    }
    case 1:
        PNGFileFormInfo pngInfo;
        QSize tSize = getSize();
        if(tSize.width() < 10 || tSize.height() < 10){
            QMessageBox::critical(this,"error","the size of plot is too small");
            break;
        }
        pngInfo.saveSize = tSize;
        pngInfo.filePath = filePath;
        outInfo.setFileForm(&pngInfo);
        emit sendSaveFile(outInfo);
        flag = true;
        break;
    }
    if(flag){
        close();
    }
}

void PlotFileSaveDialog::do_OpenSavePath()
{
    QString filePath;
    filePath=QFileDialog::getSaveFileName(this,"Save",QString());
    if(!filePath.isEmpty()) savePathEdit->setText(filePath);
}

void PlotFileSaveDialog::do_LookFileForm()
{

}

void PlotFileSaveDialog::do_SizeChanged()
{
    switch (fileFormBox -> currentIndex()) {
    default:
    case 0: {
        QSizeF tSize = getSizeF();
        sizeDisplayWidget->setDisplayRectSizeF(tSize);
        break;
    }
    case 1:{
        QSize tSize = getSize();
        sizeDisplayWidget->setDisplayRectSizeI(tSize);
        break;
    }
    }
    update();
}

void PlotFileSaveDialog::do_FileFormChanged()
{
    switch (fileFormBox -> currentIndex()) {
    default:
    case 0: {
        QSizeF tSize = QSizeF(7.5,4.5);
        widthEdit->setText("7.5"), heightEdit->setText("4.5");
        sizeDisplayWidget->setDisplayRectSizeF(tSize);
        break;
    }
    case 1:{
        QSize tSize = QSize(1200, 800);
        widthEdit->setText("1,200"), heightEdit->setText("800");
        sizeDisplayWidget->setDisplayRectSizeI(tSize);
        break;
    }
    }
    update();
}

}

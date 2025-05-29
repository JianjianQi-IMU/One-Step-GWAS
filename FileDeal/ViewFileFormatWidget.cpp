#include "ViewFileFormatWidget.hpp"

QString ViewFileFormatWidget::exchangeNewLineChar(const QString &inStr)
{
    QString tmpStr = inStr;
    tmpStr.replace('<', "&lt;");
    tmpStr.replace('>', "&gt;");
    tmpStr.replace('\n', "<br/>");
    tmpStr.replace('\t', "&emsp;&emsp;&emsp;&emsp;");
    tmpStr.replace(' ', "&emsp;");
    return tmpStr;
}

ViewFileFormatWidget::ViewFileFormatWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);

    nameBox = new QGroupBox(this);
    browser = new QTextBrowser(this);
    doc = new QTextDocument(this);

    QFont nameFont;
    QFont abbreFont;
    QFont desFont;

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QVBoxLayout* boxLayout = new QVBoxLayout(nameBox);

    mainLayout->addWidget(nameBox);

    boxLayout->addWidget(browser);

    nameFont.setFamily("Times New Roman");
    nameFont.setPointSize(16);
    abbreFont.setFamily("Times New Roman");
    abbreFont.setBold(true);
    abbreFont.setPointSize(12);
    desFont.setFamily("Times New Roman");

    nameBox->setFont(nameFont);
    browser->setDocument(doc);
}

ViewFileFormatWidget::ViewFileFormatWidget(const FD::BaseFileFormat &inFormat, QWidget *parent)
    : ViewFileFormatWidget(parent)
{
    load(inFormat);
}

void ViewFileFormatWidget::load(const FD::BaseFileFormat &inFormat)
{
    setWindowTitle("File Format - " + inFormat.name);
    format = inFormat;
    nameBox->setTitle(inFormat.name);
    QString htmlStr;
    htmlStr += "<html>";
    htmlStr += "<body>";
    htmlStr += "<font size=6 color=black>";
    htmlStr += "<b>" + format.abbreviation + "</b>";
    htmlStr += "<hr/>";
    htmlStr += "<font size=5 color=black>";
    htmlStr += exchangeNewLineChar(format.description);
    htmlStr += "<hr/>";
    htmlStr += "<font size=5 color=black>";
    htmlStr += "<b><i>example<br/></i></b>";
    htmlStr += "<font size=4 color=black>";
    htmlStr += exchangeNewLineChar(format.example);
    htmlStr += "</body>";
    htmlStr += "</html>";
    doc->clear();
    doc->setHtml(htmlStr);
}

void ViewFileFormatWidget::load(FD::FileFormClass inFormat)
{
    FD::BaseFileFormat format = FD::FileFormatSet::getFileFormat(inFormat);
    load(format);
}

bool ViewFileFormatTable::pri_addNewTab(FD::FileFormClass inFormat)
{
    ViewFileFormatWidget* pWidget = new ViewFileFormatWidget(this);
    FD::BaseFileFormat tmpFormat = FD::FileFormatSet::getFileFormat(inFormat);
    pWidget->load(tmpFormat);
    if (tmpFormat.idType == FD::FILEFORM_NONE) {
        delete pWidget;
        return false;
    }
    if (tabWidget) {
        tabWidget -> addTab(pWidget, tmpFormat.idStr);
        return true;
    }
    return false;
}

ViewFileFormatTable::ViewFileFormatTable(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);

    tabWidget = new QTabWidget(this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout -> addWidget(tabWidget);

    setWindowTitle("File Format Information");
}

ViewFileFormatTable::ViewFileFormatTable(const QVector<FD::FileFormClass> &inVec, QWidget *parent)
    : ViewFileFormatTable(parent)
{
    load(inVec);
}

void ViewFileFormatTable::load(const QVector<FD::FileFormClass> &inVec)
{
    int iFile;
    for (iFile = 0; iFile < inVec.size(); ++iFile) {
        pri_addNewTab(inVec[iFile]);
    }
}

bool ViewFileFormatTable::addNewTab(FD::FileFormClass inFormat)
{
    return pri_addNewTab(inFormat);
}

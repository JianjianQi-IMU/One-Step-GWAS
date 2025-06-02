#include "SetGffReadAttribute.hpp"
#include "ui_SetGffReadAttribute.h"

SetGffReadAttribute::SetGffReadAttribute(QWidget *parent)
    : QWidget(parent), ui(new Ui::SetGffReadAttribute)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
}

SetGffReadAttribute::SetGffReadAttribute(FD::genomeRead *inGenomeReader, QWidget *parent)
    : SetGffReadAttribute(parent)
{
    pGenomeReader = inGenomeReader;
    displayDemo();
    connect(ui->saveBtn, &QPushButton::clicked, this, &SetGffReadAttribute::saveSetting);
    connect(ui->defaultBtn, &QPushButton::clicked, this, &SetGffReadAttribute::setDefault);
}

SetGffReadAttribute::~SetGffReadAttribute()
{
    delete ui;
}

void SetGffReadAttribute::displayDemo()
{
    ui->demoTextBrowser->setText(getDemoText());
}

void SetGffReadAttribute::saveSetting()
{
    if (!ui->geneLineIdEdit->text().isEmpty()) {
        pGenomeReader->geneTypeLabel = ui->geneLineIdEdit->text().toStdString();
    }
    if (!ui->geneIdEdit->text().isEmpty()) {
        pGenomeReader->geneIdKey = ui->geneIdEdit->text().toStdString();
    }
    if (!ui->annoIdEdit->text().isEmpty()) {
        pGenomeReader->geneAnnoKey = ui->annoIdEdit->text().toStdString();
    }
    if (!ui->CDSLineIdEdit->text().isEmpty()) {
        pGenomeReader->CDSTypeLabel = ui->CDSLineIdEdit->text().toStdString();
    }
    if (!ui->UTR3LineIdEdit->text().isEmpty()) {
        pGenomeReader->UTR3TypeLabel = ui->UTR3LineIdEdit->text().toStdString();
    }
    if (!ui->UTR5LineIdEdit->text().isEmpty()) {
        pGenomeReader->UTR5TypeLabel = ui->UTR5LineIdEdit->text().toStdString();
    }
    displayDemo();
}

void SetGffReadAttribute::setDefault()
{
    pGenomeReader->setGffLabelDefault();
    ui->demoTextBrowser->setText(getDemoText());
}

QString SetGffReadAttribute::getDemoText()
{
    QString outString;
    outString += QString("<font color=black>") + "chr02" + QString("</font>");
    outString += QString(" <font color=black>") + "." + QString("</font>");
    outString += QString(" <font color=#DC143C>") + pGenomeReader->geneTypeLabel.c_str() + QString("</font>");
    outString += QString(" <font color=black>") + "2386" + QString("</font>");
    outString += QString(" <font color=black>") + "5783" + QString("</font>");
    outString += QString(" <font color=black>") + "." + QString("</font>");
    outString += QString(" <font color=black>") + "+" + QString("</font>");
    outString += QString(" <font color=black>") + "." + QString("</font>");
    outString += QString(" <font color=#0000FF>") + pGenomeReader->geneIdKey.c_str() + QString("</font>");
    outString += QString("<font color=black>") + "=XXXXXXXX.X;" + QString("</font>");
    outString += QString("<font color=#00FFFF>") + pGenomeReader->geneAnnoKey.c_str() + QString("</font>");
    outString += QString("<font color=black>") + "=XXXXXXXXXX" + QString("</font>");

    outString+=QString("<br/><font color=black>") + "chr02" + QString("</font>");
    outString+=QString(" <font color=black>") + "." + QString("</font>");
    outString+=QString(" <font color=#7FFF00>") + pGenomeReader->UTR5TypeLabel.c_str() + QString("</font>");
    outString+=QString(" <font color=black>") + "2386" + QString("</font>");
    outString+=QString(" <font color=black>") + "2402" + QString("</font>");
    outString+=QString(" <font color=black>") + "." + QString("</font>");
    outString+=QString(" <font color=black>") + "+" + QString("</font>");
    outString+=QString(" <font color=black>") + "." + QString("</font>");
    outString+=QString(" <font color=black>") + "..." + QString("</font>");

    outString+=QString("<br/><font color=black>") + "chr02" + QString("</font>");
    outString+=QString(" <font color=black>") + "." + QString("</font>");
    outString+=QString(" <font color=#DEB887>") + pGenomeReader->CDSTypeLabel.c_str() + QString("</font>");
    outString+=QString(" <font color=black>") + "2403" + QString("</font>");
    outString+=QString(" <font color=black>") + "3611"+QString("</font>");
    outString+=QString(" <font color=black>") + "." + QString("</font>");
    outString+=QString(" <font color=black>") + "+" + QString("</font>");
    outString+=QString(" <font color=black>") + "." + QString("</font>");
    outString+=QString(" <font color=black>") + "..." + QString("</font>");

    outString+=QString("<br/><font color=black>") + "chr02" + QString("</font>");
    outString+=QString(" <font color=black>") + "." + QString("</font>");
    outString+=QString(" <font color=#6495ED>") + pGenomeReader->UTR3TypeLabel.c_str() + QString("</font>");
    outString+=QString(" <font color=black>") + "5562" + QString("</font>");
    outString+=QString(" <font color=black>") + "5783" + QString("</font>");
    outString+=QString(" <font color=black>") + "." + QString("</font>");
    outString+=QString(" <font color=black>") + "+" + QString("</font>");
    outString+=QString(" <font color=black>") + "." + QString("</font>");
    outString+=QString(" <font color=black>") + "..." + QString("</font>");
    return outString;
}

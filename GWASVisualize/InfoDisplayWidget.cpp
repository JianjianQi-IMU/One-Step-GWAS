#include "InfoDisplayWidget.hpp"
#include "ui_InfoDisplayWidget.h"

InfoDisplayWidget::InfoDisplayWidget(QWidget *parent)
    : QTabWidget(parent), ui(new Ui::InfoDisplayWidget)
{
    ui->setupUi(this);
    pGeneWidget = new GeneInfoWidget(this);
    pPointWidget = new PointInfoWidget(this);

    addTab(pPointWidget, "Point infomation");
    addTab(pGeneWidget, "Gene infomation");

    genome = nullptr;
    points = nullptr;

}

void InfoDisplayWidget::setGenome(GenomeAnnotation *inGenome)
{
    genome = inGenome;
}

void InfoDisplayWidget::setPoints(ValPoints *inPoints, size_t inNPoints)
{
    points = inPoints;
    nPoints = inNPoints;
}

void InfoDisplayWidget::displayGeneInfo(long long id)
{
    if (genome != nullptr && id > 0) {
        GeneAnnotation* pGene = genome->getGene(id);
        pGeneWidget->setChr(genome->getChrName(pGene->idChr));
        pGeneWidget->setLen(QString::number(pGene->stop - pGene->start+1) + " bp");
        pGeneWidget->setStrand(QString(pGene->strand));
        pGeneWidget->setPos(QString::number(pGene->start) + ".." + QString::number(pGene->stop));
        pGeneWidget->setAnno(pGene->annotation);
        pGeneWidget->setName(pGene->name);
        setCurrentIndex(1);
    }
}

void InfoDisplayWidget::displayPointInfo(long long id, long long iGene1, long long iGene2)
{
    if (points != nullptr && genome != nullptr && id < nPoints && id > 0) {
        ValPoints tPoint = points[id];
        pPointWidget->setChr(genome->getChrName(tPoint.idchr));
        pPointWidget->setPos(QString::number(tPoint.pos));
        pPointWidget->setLogP(QString::number(tPoint.val, 'f', 2));
        if (iGene1 < genome->getNGenes() && iGene1 > 0) {
            pPointWidget->setGene1(genome->getGene(iGene1)->name);
        } else {
            pPointWidget->setGene1("(null)");
        }
        if (iGene2 < genome->getNGenes() && iGene2 > 0) {
            pPointWidget->setGene2(genome->getGene(iGene2)->name);
        } else {
            pPointWidget->setGene2("(null)");
        }
        setCurrentIndex(0);
    }
}

InfoDisplayWidget::~InfoDisplayWidget()
{
    delete ui;
}


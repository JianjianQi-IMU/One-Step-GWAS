#include "GenomeAnnoSlideWidget.h"

GenomeAnnoSlideWidget::GenomeAnnoSlideWidget(QWidget *parent)
    : QWidget(parent)
{
    pSlidePainter = new GenomeAnnoSlidePainter;
    pChrInfo = new ChromosomeInfoClass;


    pSlidePainter -> preSetChrInfo(pChrInfo);
    chrHFactor = .8;
    isLookIntervalRectDrag = false;
    topBorder = 5;
    downBorder = 5;
}

GenomeAnnoSlideWidget::~GenomeAnnoSlideWidget()
{
    if(pSlidePainter != nullptr){
        delete pSlidePainter;
        pSlidePainter = nullptr;
    }
    if(pChrInfo){
        delete pChrInfo;
        pChrInfo = nullptr;
    }
}

bool GenomeAnnoSlideWidget::loadChromosome(long long *inChrLen, char **inChrName, int inNChr)
{
    return pChrInfo -> loadChromosome(inChrLen, inChrName, inNChr);
}

bool GenomeAnnoSlideWidget::loadChromosome(const std::vector<long long> &inChrLen, const std::vector<std::string> &inChrName)
{
    return pChrInfo -> loadChromosome(inChrLen, inChrName);
}

void GenomeAnnoSlideWidget::setCurrentPos(long double inCurrentLeftPosF, long double inCurrentRightPosF)
{
    pSlidePainter -> preSetCurrentPosF(inCurrentLeftPosF, inCurrentRightPosF);
    pChrInfo -> preAdjustPaintChri(inCurrentLeftPosF, inCurrentRightPosF);
    update();
}

void GenomeAnnoSlideWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QSize tSize = size();
    double chrH = chrHFactor*(tSize.height() - topBorder - downBorder);
    pSlidePainter -> setHeightChooseFrame(tSize.height() - topBorder - downBorder);
    pSlidePainter -> setBarDisplayRect(QRect(topBorder,(tSize.height()-chrH)/2,tSize.width(),chrH));

    QPainter Painter;
    Painter.begin(this);
    pSlidePainter -> paintAnnoSlide(&Painter);
    Painter.end();
}

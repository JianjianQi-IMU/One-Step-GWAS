#include "GenomeAnnoDisplayView.hpp"

#include <QFontMetrics>

GenomeAnnoDisplayView::GenomeAnnoDisplayView()
{
    displayArea                          = QRect();
    geneDisplayYFactor                   = 0.25;
    geneModelDisplayYFactor              = 0.50;
    baseDisplayYFactor                   = 0.75;
    geneModelHeight                      = 20;
    baseHeight                           = 20;
    displayAreaLabelGap                  = 10;
    geneDisplayMaxPosGapLen              = 1000000;
    geneDisplayMode                      = GENEDISPLAY_MODE1;
    isDisplayGene                        = true;
    isDisplayGeneModel                   = true;
    isDisplayGenomeSeq                   = true;
    isDisplayAreaLabel                   = true;
    colorSelectedFrame                   = QColor(0, 255, 0);
    colorForwardGene                     = QColor(0, 180, 0);
    colorReverseGene                     = QColor(180, 0, 0);
    colorNullStrandGene                  = QColor(100, 100, 100);
    colorDefaultGene                     = QColor(0,0,255);
    color5UTR                            = QColor(142,207,201);
    color3UTR                            = QColor(255,190,122);
    colorCDS                             = QColor(0,0,255);
    baseA                                = QColor(16, 150, 72);
    baseT                                = QColor(214, 40, 57);
    baseC                                = QColor(37, 92, 153);
    baseG                                = QColor(247, 179, 43);
    baseN                                = QColor(100, 100, 100);
    base_                                = QColor(52, 52, 52);
    displaySelectedGeneInfo              = false;
    genome                               = nullptr;
    genomeSeq                            = nullptr;
    textZoomInToLook                     = QString("(Zoom in to see details)");
    fontZoomInToLook.setFamily("Times New Roman");
    fontZoomInToLook.setItalic(true);
    geneNameFont.setFamily("Times New Roman");
    geneNameFont.setItalic(true);
    geneNameFont.setBold(true);

    geneInfoFont.setFamily("Times New Roman");
    geneInfoFont.setBold(true);

    displayAreaLabelFont.setFamily("Times New Roman");
    displayAreaLabelFont.setBold(true);
    displayAreaLabelFont.setWeight(3);

    currentMinGenei                      = 0;
    currentMaxGenei                      = 0;
    nPaintGenes                          = 0;

    tmpSelectedPointGenei                = 0;
    tmpSelectedGenei                     = 0;
    tmpChrInfo                           = nullptr;
    tmpCurrentLeftPosF                   = 0.0;
    tmpCurrentRightPosF                  = 0.0;
}

GenomeAnnoDisplayView::~GenomeAnnoDisplayView()
{
    if (genome) {
        delete genome;
    }
    if (genomeSeq) {
        delete genomeSeq;
    }

    genome = nullptr;
    genomeSeq = nullptr;
}

bool GenomeAnnoDisplayView::loadGenomeSeq(const char *inFile)
{
    long long* chrLen = tmpChrInfo->getChrLen();
    char** chrName = tmpChrInfo->getChrName();
    int nChr = tmpChrInfo->getNChr();
    if (genomeSeq) {
        genomeSeq->clear();
    } else {
        genomeSeq = new GenomeSequence;
    }
    if (FD::genomeSeqRead(inFile, chrName, nChr, chrLen, *genomeSeq)) {
        isDisplayGenomeSeq = true;
        return true;
    }
    return false;
}

bool GenomeAnnoDisplayView::createGenome()
{
    if (!genome) {
        genome = new GenomeAnnotation;
        return true;
    }
    return false;
}

bool GenomeAnnoDisplayView::createGenome(char **inChrName, int inNChr, long long inNGenes)
{
    if (!genome) {
        genome = new GenomeAnnotation(inChrName, inNChr, inNGenes);
    } else {
        genome->clear();
        if(!(genome->loadChromosome(inChrName, inNChr))) {
            return false;
        }
        if(!(genome->initGenes(inNGenes))) {
            return false;
        }
    }
    return true;
}

bool GenomeAnnoDisplayView::setGenome(const GenomeAnnotation &inGenome)
{
    if (!genome) {
        return false;
    }
    *genome = inGenome;
    if (genome->getValid()) {
        return true;
    }
    return false;
}

void GenomeAnnoDisplayView::adjustPaintGenesi()
{
    if (!genome || !genome->getValid()) {
        nPaintGenes = 0;
        return;
    }
    int currentMinChri;
    int currentMaxChri;
    tmpChrInfo->getCurrentChri(currentMinChri, currentMaxChri);
    long long* cumChrLen = tmpChrInfo->getCumChrLen();
    int nChr = tmpChrInfo->getNChr();
    long long i = 0;
    long double posa = tmpCurrentLeftPosF - cumChrLen[currentMinChri];
    long double posb = tmpCurrentRightPosF - cumChrLen[currentMaxChri];
    long long posai = genome->search(currentMinChri, posa - 0.5, GenomeAnnotation::GA_STOP);
    long long posbi = genome->searchLower(currentMaxChri, posb + 0.5);
    if (posai > genome->getNGenes()) {
        if (currentMinChri != nChr - 1) {
            for (i = currentMinChri + 1; i <= currentMaxChri; ++i) {
                if (!genome->isEmptyChr(i)) {
                    posai = genome->getChrFirsti(i);
                    break;
                }
            }
        }
        if (posai > genome->getNGenes()) {
            nPaintGenes = 0;
            return;
        }
    }
    if (posbi > genome->getNGenes()) {
        if (currentMaxChri != 0) {
            for (i = currentMaxChri - 1; i >= currentMinChri; --i) {
                if (!genome->isEmptyChr(i)) {
                    posbi = genome->getChrLasti(i);
                    break;
                }
            }
        }
        if (posbi > genome->getNGenes()) {
            nPaintGenes = 0;
            return;
        }
    }
    if (posai <= posbi) {
        currentMaxGenei = posbi, currentMinGenei = posai;
        nPaintGenes = currentMaxGenei - currentMinGenei + 1;
    } else {
        nPaintGenes = 0;
    }
}

void GenomeAnnoDisplayView::paintGenesInPainter(QPainter *painter)
{
    painter->save();

    int xpos = displayArea.x(), ypos = displayArea.y();
    int width = displayArea.width(), height = displayArea.height();
    long long* cumChrLen = tmpChrInfo->getCumChrLen();
    long long i;
    int ceny0 = ypos + geneDisplayYFactor * height;
    int painty0 = ceny0 - 0.5 * geneModelHeight;
    int paintx0, paintl;
    int paintGeneNamey0, paintGeneNamex0;
    long long tGeneStart = 0, tGeneStop = 0;
    long double tPaintStart, tPaintStop;
    long double positionGapLenF = tmpCurrentRightPosF - tmpCurrentLeftPosF;
    GeneAnnotation* pGene = nullptr;
    QPolygonF geneTriangle;
    QColor geneFillColor;
    QRect geneRect;

    painter->setPen(QPen(Qt::black, 2));
    painter->setPen(QPen(Qt::black, 1));

    if (nPaintGenes <= 0) {
        painter->restore();
        return;
    }

    if (currentMaxGenei - currentMinGenei > 2000) {
        painter->setFont(fontZoomInToLook);
        painter->drawText(xpos, painty0, width, geneModelHeight, Qt::AlignVCenter | Qt::AlignHCenter, textZoomInToLook);
        painter->restore();
        return;
    }

    paintGeneNamey0 = (painty0 + ypos) / 2;
    if (painty0 - paintGeneNamey0 > 50) {
        paintGeneNamey0 = painty0 - 50;
    }

    for (i = currentMinGenei; i <= currentMaxGenei; ++i) {
        pGene = (genome->getGene(i));
        tGeneStart = pGene->start + cumChrLen[pGene->idChr];
        tGeneStop = pGene->stop + cumChrLen[pGene->idChr];
        tPaintStart = tGeneStart - 0.5, tPaintStop = tGeneStop + 0.5;
        tPaintStart = std::max(tPaintStart, tmpCurrentLeftPosF);
        tPaintStop = std::min(tPaintStop, tmpCurrentRightPosF);
        paintx0 = (tPaintStart - tmpCurrentLeftPosF) / positionGapLenF * width + xpos;
        paintl = std::max((long double)(tPaintStop - tPaintStart) / positionGapLenF * width, (long double)1.0);
        geneRect = QRect(paintx0, painty0, paintl, geneModelHeight);
        switch (pGene -> strand) {
            case '+':{
                geneFillColor = colorForwardGene;
            }
                break;
            case '-':{
                geneFillColor = colorReverseGene;
            }
                break;
            default:{
                geneFillColor = colorDefaultGene;
            }
            break;
        }

        switch (geneDisplayMode) {
            case GENEDISPLAY_MODE1: {
                paintOneGeneMode1(geneRect, pGene -> strand, colorDefaultGene, painter);
            }
                break;
            case GENEDISPLAY_MODE2: {
                paintOneGeneMode2(geneRect, pGene -> strand, geneFillColor, painter);
            }
                break;
            default: {
                paintOneGeneMode1(geneRect, pGene -> strand, colorDefaultGene, painter);
            }
                break;
        }

        painter->setPen(QPen(Qt::black, 3));
        painter->setFont(geneNameFont);
        if (currentMaxGenei-currentMinGenei <= 20) {
            paintGeneNamex0 = paintx0;
            painter->drawText(paintGeneNamex0, paintGeneNamey0, 300, 100, Qt::AlignLeft | Qt::AlignTop, pGene->name);
        }
    }

    painter->restore();
}

void GenomeAnnoDisplayView::paintGenesModelInPainter(QPainter *painter)
{
    if (!(genome->haveGeneModel())) {
        return;
    }

    painter->save();

    int xpos = displayArea.x(), ypos = displayArea.y();
    int width = displayArea.width(), height = displayArea.height();
    long long* cumChrLen = tmpChrInfo->getCumChrLen();
    QBrush CDSBrush(colorCDS);
    QBrush UTR5Brush(color5UTR);
    QBrush UTR3Brush(color3UTR);
    QBrush basicGeneBrush(colorNullStrandGene);
    long long i;
    GeneAnnotation* pGene = nullptr;
    std::list<long long>* pCDSList = nullptr;
    std::list<long long>::iterator iter;
    int ceny0 = ypos + geneModelDisplayYFactor * height;
    int painty0 = ceny0 - 0.5 * geneModelHeight;
    int paintBasicy0 = ceny0 - 0.15 * geneModelHeight;
    int paintx0, paintl;
    long long tGeneStart = 0, tGeneStop = 0, tStart = 0, tStop = 0;
    long double tPaintStart, tPaintStop;
    long double positionGapLenF = tmpCurrentRightPosF - tmpCurrentLeftPosF;

    painter->setPen(QPen(Qt::black, 2));
    painter->setPen(QPen(Qt::black, 1));

    if (nPaintGenes <= 0) {
        painter->restore();
        return;
    }

    if (currentMaxGenei - currentMinGenei > 1000) {
        painter->setFont(fontZoomInToLook);
        painter->drawText(xpos, painty0, width, geneModelHeight, Qt::AlignVCenter | Qt::AlignHCenter, textZoomInToLook);
        painter->restore();
        return;
    }

    for (i = currentMinGenei; i <= currentMaxGenei; ++i) {
        pGene = (genome->getGene(i));

        // paint basic gene model
        tGeneStart = pGene->start + cumChrLen[pGene->idChr], tGeneStop = pGene->stop + cumChrLen[pGene->idChr];
        tPaintStart = tGeneStart - 0.5, tPaintStop = tGeneStop + 0.5;
        tPaintStart = std::max(tPaintStart, tmpCurrentLeftPosF);
        tPaintStop = std::min(tPaintStop, tmpCurrentRightPosF);
        paintx0 = (long double)(tPaintStart - tmpCurrentLeftPosF) / positionGapLenF * width + xpos;
        paintl = std::max((long double)(tPaintStop - tPaintStart) / positionGapLenF * width, (long double)1.0);
        painter->fillRect(QRectF(paintx0, paintBasicy0, paintl, 0.3 * geneModelHeight), basicGeneBrush);

        // paint CDS
        pCDSList = pGene->getCDSList();
        iter = pCDSList->begin();
        while (iter != pCDSList->end()) {
            tStart = *iter + cumChrLen[pGene->idChr];
            iter++;
            tStop = *iter + cumChrLen[pGene->idChr];
            iter++;
            tPaintStart = tStart - 0.5, tPaintStop = tStop + 0.5;
            if ((long double)tPaintStart>tmpCurrentRightPosF ||
                (long double)tPaintStop<tmpCurrentLeftPosF) {
                continue;
            }
            tPaintStart = std::max(tPaintStart, tmpCurrentLeftPosF);
            tPaintStop = std::min(tPaintStop, tmpCurrentRightPosF);
            paintx0 = (long double)(tPaintStart - tmpCurrentLeftPosF) / positionGapLenF * width + xpos;
            paintl = std::max((long double)(tPaintStop - tPaintStart) / positionGapLenF * width, (long double)1.0);
            painter->fillRect(QRectF(paintx0, painty0, paintl, geneModelHeight), CDSBrush);
        }

        // paint UTR
        if (pGene->startUTR5 > 0 && pGene->stopUTR5 > 0) {
            tStart = pGene->startUTR5 + cumChrLen[pGene->idChr];
            tStop = pGene->stopUTR5 + cumChrLen[pGene->idChr];
            tPaintStart = tStart - 0.5, tPaintStop = tStop + 0.5;
            if ((long double)tPaintStart <= tmpCurrentRightPosF || (long double)tPaintStop >= tmpCurrentLeftPosF) {
                tPaintStart = std::max(tPaintStart, tmpCurrentLeftPosF);
                tPaintStop = std::min(tPaintStop, tmpCurrentRightPosF);
                paintx0 = (long double)(tPaintStart - tmpCurrentLeftPosF) / positionGapLenF * width + xpos;
                paintl = std::max((long double)(tPaintStop - tPaintStart) / positionGapLenF * width, (long double)1.0);
                painter->fillRect(QRectF(paintx0, painty0, paintl, geneModelHeight), UTR5Brush);
            }
        }
        if (pGene->startUTR3 > 0 && pGene->stopUTR3 > 0) {
            tStart = pGene->startUTR3 + cumChrLen[pGene->idChr];
            tStop = pGene->stopUTR3 + cumChrLen[pGene->idChr];
            tPaintStart = tStart - 0.5, tPaintStop = tStop + 0.5;
            if ((long double)tPaintStart <= tmpCurrentRightPosF || (long double)tPaintStop >= tmpCurrentLeftPosF) {
                tPaintStart = std::max(tPaintStart, tmpCurrentLeftPosF);
                tPaintStop = std::min(tPaintStop, tmpCurrentRightPosF);
                paintx0 = (long double)(tPaintStart - tmpCurrentLeftPosF) / positionGapLenF * width + xpos;
                paintl = std::max((long double)(tPaintStop - tPaintStart) / positionGapLenF * width, (long double)1.0);
                painter->fillRect(QRectF(paintx0, painty0, paintl, geneModelHeight), UTR3Brush);
            }
        }
    }

    painter->restore();
}

void GenomeAnnoDisplayView::paintSelectedGene(QPainter *painter)
{
    if (tmpSelectedGenei < currentMinGenei || tmpSelectedGenei > currentMaxGenei || !geneDisplayMode) {
        return;
    }
    painter->save();

    int xpos = displayArea.x(), ypos = displayArea.y();
    int width = displayArea.width(), height = displayArea.height();
    long long* cumChrLen = tmpChrInfo->getCumChrLen();
    int ceny0 = ypos + geneDisplayYFactor * height;
    int painty0 = ceny0 - 0.5 * geneModelHeight;
    int paintx0, paintl;
    long long tGeneStart = 0, tGeneStop = 0;
    long double tPaintStart, tPaintStop;
    long double positionGapLenF = tmpCurrentRightPosF - tmpCurrentLeftPosF;
    GeneAnnotation* pGene = genome->getGene(tmpSelectedGenei);

    tGeneStart = pGene->start + cumChrLen[pGene->idChr];
    tGeneStop = pGene->stop + cumChrLen[pGene->idChr];
    tPaintStart = tGeneStart - 0.5, tPaintStop = tGeneStop + 0.5;
    tPaintStart = std::max(tPaintStart, tmpCurrentLeftPosF);
    tPaintStop = std::min(tPaintStop, tmpCurrentRightPosF);
    paintx0 = (long double)(tPaintStart - tmpCurrentLeftPosF) / positionGapLenF * width + xpos;
    paintl = std::max((long double)(tPaintStop - tPaintStart) / positionGapLenF * width, (long double)1.0);
    painter->setPen(QPen(QColor(255, 0, 0), 4));
    painter->drawRect(QRectF(paintx0 - 2, painty0 - 2, paintl + 4, geneModelHeight + 4));
    if (displaySelectedGeneInfo) {
        paintGeneInfo(painter);
    }

    painter->restore();
}

void GenomeAnnoDisplayView::paintGenomeSeq(QPainter *painter)
{
    int xpos = displayArea.x(), ypos = displayArea.y();
    int width = displayArea.width(), height = displayArea.height();
    long long* cumChrLen = tmpChrInfo->getCumChrLen();
    long long* chrLen = tmpChrInfo->getChrLen();
    int currentMinChri;
    int currentMaxChri;
    int ceny0 = ypos + baseDisplayYFactor * height;
    int painty0 = ceny0 - 0.5 * baseHeight;
    long double positionGapLenF = tmpCurrentRightPosF - tmpCurrentLeftPosF;
    long double oneBaseLen = width / positionGapLenF;
    long double tStart, tStop;
    long long tStarti, tNBase, tIBase, tPos, i;
    short tBaseValue;
    char* tSeq = nullptr;
    char base1, base2;
    QBrush basicGeneBrush(colorNullStrandGene);
    tmpChrInfo->getCurrentChri(currentMinChri, currentMaxChri);
    painter->save();

    if (positionGapLenF > 1000) {
        painter->setFont(fontZoomInToLook);
        painter->drawText(xpos, painty0, width, geneModelHeight, Qt::AlignVCenter | Qt::AlignHCenter, textZoomInToLook);
        painter->restore();
        return ;
    }
    if (oneBaseLen < 1) {
        oneBaseLen = 1;
    }
    long long iChr;
    for (iChr = currentMinChri; iChr <= currentMaxChri; ++iChr) {
        tStart = std::max((long double)cumChrLen[iChr] + 1, tmpCurrentLeftPosF);
        tStop = std::min((long double)cumChrLen[iChr] + chrLen[iChr], tmpCurrentRightPosF);
        tStarti = tStart - cumChrLen[iChr] - 0.5;
        tNBase = tStop - tStart + 1;
        if (tNBase + tStart < (long long)(tStop + 0.5) + 0.5) {
            ++tNBase;
        }
        tSeq = genomeSeq->getSeq(iChr, tStart - cumChrLen[iChr]);
        tIBase = 0, i = 0, tPos = tStart + 0.5;
        while (tIBase<tNBase) {
            tBaseValue = tSeq[i];
            base2 = GenomeSequence::Value2Base(tBaseValue & 0b1111);
            tBaseValue >>= 4;
            base1 = GenomeSequence::Value2Base(tBaseValue & 0b1111);
            if (tIBase == 0) {
                if (tStarti % 2 == 0) {
                    paintOneBase(base1, ((long double)tPos - tmpCurrentLeftPosF) * width / positionGapLenF + xpos,
                        oneBaseLen, painter);
                    ++tPos;
                    paintOneBase(base2, ((long double)tPos - tmpCurrentLeftPosF) * width / positionGapLenF + xpos,
                        oneBaseLen, painter);
                    ++tPos;
                    tIBase += 2;
                } else {
                    paintOneBase(base2, ((long double)tPos - tmpCurrentLeftPosF) * width / positionGapLenF + xpos,
                        oneBaseLen, painter);
                    ++tPos;
                    ++tIBase;
                }
            } else {
                paintOneBase(base1, ((long double)tPos - tmpCurrentLeftPosF) * width / positionGapLenF + xpos,
                    oneBaseLen, painter);
                ++tPos;
                ++tIBase;
                if (tIBase < tNBase) {
                    paintOneBase(base2, ((long double)tPos - tmpCurrentLeftPosF) * width / positionGapLenF + xpos,
                        oneBaseLen, painter);
                    ++tPos;
                    ++tIBase;
                }
            }
            ++i;
        }
    }

    painter->restore();
}

void GenomeAnnoDisplayView::paintOneBase(char inBase, int xPos, int oneBaseLen, QPainter *painter)
{
    int xpos = displayArea.x(), ypos = displayArea.y();
    int width = displayArea.width(), height = displayArea.height();
    int ceny0 = ypos + baseDisplayYFactor * height;
    int painty0 = ceny0 - 0.5 * baseHeight;
    int paintx0 = xPos - oneBaseLen / 2;
    int paintx1 = xPos + oneBaseLen / 2;
    int paintLen;
    if (paintx0 < xpos) paintx0 = xpos;
    if (paintx1 > width + xpos) paintx1 = width + xpos;
    paintLen = paintx1 - paintx0 + 1;
    if (paintLen <= 0) return;
    switch (inBase) {
        case 'A':
            painter->fillRect(QRect(paintx0, painty0, paintLen, baseHeight), baseA);
            break;
        case 'T':
            painter->fillRect(QRect(paintx0, painty0, paintLen, baseHeight), baseT);
            break;
        case 'C':
            painter->fillRect(QRect(paintx0, painty0, paintLen, baseHeight), baseC);
            break;
        case 'G':
            painter->fillRect(QRect(paintx0, painty0, paintLen, baseHeight), baseG);
            break;
        case 'N':
            painter->fillRect(QRect(paintx0, painty0, paintLen, baseHeight), baseN);
            break;
        default:
            painter->fillRect(QRect(paintx0, painty0, paintLen, baseHeight), base_);
            break;
    }
    if (paintLen > 10) {
        switch (inBase) {
            case 'A':
                painter->drawText(QRect(paintx0, painty0, paintLen, baseHeight), Qt::AlignHCenter | Qt::AlignVCenter, "A");
                break;
            case 'T':
                painter->drawText(QRect(paintx0, painty0, paintLen, baseHeight), Qt::AlignHCenter | Qt::AlignVCenter, "T");
                break;
            case 'C':
                painter->drawText(QRect(paintx0, painty0, paintLen, baseHeight), Qt::AlignHCenter | Qt::AlignVCenter, "C");
                break;
            case 'G':
                painter->drawText(QRect(paintx0, painty0, paintLen, baseHeight), Qt::AlignHCenter | Qt::AlignVCenter, "G");
                break;
            case 'N':
                painter->drawText(QRect(paintx0, painty0, paintLen, baseHeight), Qt::AlignHCenter | Qt::AlignVCenter, "N");
                break;
            default:
                break;
        }
    }
}

void GenomeAnnoDisplayView::paintGeneInfo(QPainter *painter)
{
    if (tmpSelectedGenei < 0) return;
    if (tmpSelectedGenei < currentMinGenei || tmpSelectedGenei > currentMaxGenei) return;
    painter->save();
    QFontMetrics fontMetrics(geneInfoFont);
    long long* cumChrLen = tmpChrInfo->getCumChrLen();
    char** chrName = tmpChrInfo->getChrName();
    int xpos = displayArea.x(), ypos = displayArea.y();
    int width = displayArea.width(), height = displayArea.height();
    int ceny0 = ypos + geneDisplayYFactor * height;
    int painty0 = ceny0 - 0.5 * geneModelHeight;
    long long tGeneStart = 0, tGeneStop = 0;
    long double tPaintStart;
    long double positionGapLenF = tmpCurrentRightPosF - tmpCurrentLeftPosF;
    GeneAnnotation* pGene = genome->getGene(tmpSelectedGenei);
    tGeneStart = pGene->start + cumChrLen[pGene->idChr];
    tGeneStop = pGene->stop + cumChrLen[pGene->idChr];
    tPaintStart = tGeneStart - 0.5;
    tPaintStart = std::max(tPaintStart, tmpCurrentLeftPosF);
    QString textChr = QString("Chr:") + chrName[pGene->idChr];
    QString textPos = QString("Pos:") + QString::number(tGeneStart - cumChrLen[pGene->idChr]) +
        ".." + QString::number(tGeneStop - cumChrLen[pGene->idChr]);
    QString textName = pGene->name;
    int fw = 14 + std::max(fontMetrics.horizontalAdvance(textChr),
        std::max(fontMetrics.horizontalAdvance(textPos), fontMetrics.horizontalAdvance(textName)));
    int fh = 2 + fontMetrics.height();
    int px = (long double)(tPaintStart - tmpCurrentLeftPosF) / positionGapLenF * width + xpos;
    int py = painty0 + geneModelHeight;


    painter->setFont(geneInfoFont);

    painter->setBrush(QColor(190, 190, 190));
    painter->setPen(QPen(Qt::transparent));
    painter->drawRect(px + 2, py + 2, fw, fh * 3 + 12);
    painter->setBrush(Qt::white);
    painter->setPen(QPen(Qt::black, 1));
    painter->drawRect(px, py, fw, fh * 3 + 12);

    if (pGene->strand == '+') {
        painter->setPen(QPen(colorForwardGene, 1));
    } else {
        painter->setPen(QPen(colorReverseGene, 1));
    }

    painter->drawText(px, py + 6, fw + 6, fh, Qt::AlignLeft | Qt::AlignTop, textName);
    painter->setPen(QPen(Qt::black, 1));
    painter->drawText(px, py + fh + 6, fw + 6, fh, Qt::AlignLeft | Qt::AlignTop, textChr);
    painter->drawText(px, py + fh * 2 + 6, fw + 6, fh, Qt::AlignLeft | Qt::AlignTop, textPos);
    painter->restore();

}

void GenomeAnnoDisplayView::paintDisplayAreaLabel(QPainter *painter)
{
    painter->save();
    QFontMetrics fm(displayAreaLabelFont);
    int xpos = displayArea.x(), ypos = displayArea.y();
    int height = displayArea.height();
    int baseCeny0 = ypos + baseDisplayYFactor * height;
    int geneCeny0 = ypos + geneDisplayYFactor * height;
    int geneModelCeny0 = ypos + geneModelDisplayYFactor * height;
    QString geneStr("Gene");
    QString geneModelStr("Gene Model");
    QString seqStr("Sequence");
    int geneFW = fm.horizontalAdvance(geneStr);
    int geneModelFW = fm.horizontalAdvance(geneModelStr);
    int seqFW = fm.horizontalAdvance(seqStr);
    int FH = fm.height();
    painter -> setFont(displayAreaLabelFont);
    painter -> drawText(xpos - geneFW - displayAreaLabelGap, geneCeny0 - FH / 2, geneFW,
        FH, Qt::AlignRight | Qt::AlignVCenter, geneStr);
    painter -> drawText(xpos - geneModelFW - displayAreaLabelGap, geneModelCeny0 - FH / 2,
        geneModelFW, FH, Qt::AlignRight | Qt::AlignVCenter, geneModelStr);
    painter -> drawText(xpos - seqFW - displayAreaLabelGap, baseCeny0 - FH / 2,
        seqFW, FH, Qt::AlignRight | Qt::AlignVCenter, seqStr);
    painter->restore();
}


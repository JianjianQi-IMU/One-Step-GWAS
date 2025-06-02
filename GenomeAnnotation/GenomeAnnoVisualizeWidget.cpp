#include "GenomeAnnoVisualizeWidget.hpp"
#include <QMouseEvent>

GenomeAnnoVisualizeWidget::GenomeAnnoVisualizeWidget(QWidget *parent)
    : BaseOpenGLPaintWidget(parent)
{
    zoomFactor                           = 2;
    moveFactor                           = 0.5;

    isLeftMousePressed                   = false;
    isMiddleMousePressed                 = false;
    isRightMousePressed                  = false;
    leftMousePressedPosX                 = -1;
    leftMouseReleasedPosX                = -1;
    leftMouseMovingPosX                  = -1;
    middleMousePressedPosX               = -1;
    middleMouseMovingPosX                = -1;
    RightMousePressedPosX                = -1;
    RightMousePressedPosY                = -1;
    middleMousePressedLeftPosF           = -1;
    middleMousePressedRightPosF          = -1;


    pMenu = nullptr;
    pAnnoPainter = new GenomeAnnoDisplayView;
    pFramePainter = new GenomeViewFrame;
    pChrInfo = new ChromosomeInfoClass;
    pTextPainter = new GenomeViewTextVectorPainter;

    pAnnoPainter -> preSetChrInfo(pChrInfo);
    pFramePainter -> preSetChrInfo(pChrInfo);

    setMouseTracking(true);
}

GenomeAnnoVisualizeWidget::~GenomeAnnoVisualizeWidget()
{
    if (pMenu) {
        delete pMenu;
        pMenu = nullptr;
    }
    if (pAnnoPainter) {
        delete pAnnoPainter;
        pAnnoPainter = nullptr;
    }
    if (pFramePainter) {
        delete pFramePainter;
        pFramePainter = nullptr;
    }
    if (pChrInfo) {
        delete pChrInfo;
        pChrInfo = nullptr;
    }
    if (pTextPainter) {
        delete pTextPainter;
        pTextPainter = nullptr;
    }
}

bool GenomeAnnoVisualizeWidget::loadChromosome(long long *inChrLen, char **inChrName, int inNChr)
{
    bool flag = pChrInfo -> loadChromosome(inChrLen, inChrName, inNChr);
    if (flag) {
        currentRightPosF = pChrInfo -> getMaxCumPos();
        currentLeftPosF = 1;
    }
    return flag;
}

bool GenomeAnnoVisualizeWidget::loadChromosome(const std::vector<long long> &inChrLen, const std::vector<std::string> &inChrName)
{

    bool flag = pChrInfo -> loadChromosome(inChrLen, inChrName);
    if (flag) {
        currentRightPosF = pChrInfo -> getMaxCumPos();
        currentLeftPosF = 1;
    }
    return flag;
}

bool GenomeAnnoVisualizeWidget::createGenome(char **inChrName, int inNChr, long long inNGenes)
{
    return pAnnoPainter -> createGenome(inChrName, inNChr, inNGenes);
}

bool GenomeAnnoVisualizeWidget::setGenome(const GenomeAnnotation &inGenome)
{
    if (pAnnoPainter -> setGenome(inGenome)) {
        update();
        return true;
    }
    return false;
}

bool GenomeAnnoVisualizeWidget::loadGenomeSeq(const char *inFile)
{
    if (pAnnoPainter -> loadGenomeSeq(inFile)) {
        update();
        return true;
    }
    return false;
}

void GenomeAnnoVisualizeWidget::zoomIn()
{
    bool isNeedUpdate = false;
    doZoomIn(isNeedUpdate);
    update();
}

void GenomeAnnoVisualizeWidget::zoomOut()
{
    bool isNeedUpdate = false;
    doZoomOut(isNeedUpdate);
    update();
}

void GenomeAnnoVisualizeWidget::moveLeft()
{
    long double positionGapLenF = currentRightPosF - currentLeftPosF;
    long double moveLeftPos = std::max((long double)1, (currentLeftPosF - moveFactor * positionGapLenF));
    long double moveLength = currentLeftPosF - moveLeftPos;
    currentLeftPosF = moveLeftPos;
    currentRightPosF = currentRightPosF - moveLength;
    emit currentIntervalChanged(currentLeftPosF, currentRightPosF);
    update();

}

void GenomeAnnoVisualizeWidget::moveRight()
{
    long double positionGapLenF = currentRightPosF - currentLeftPosF;
    long long maxCumPos = pChrInfo -> getMaxCumPos();
    long double moveRightPos = std::min((long double)(maxCumPos), (currentRightPosF + moveFactor * positionGapLenF));
    long double moveLength = moveRightPos - currentRightPosF;
    currentLeftPosF = currentLeftPosF + moveLength;
    currentRightPosF = moveRightPos;
    emit currentIntervalChanged(currentLeftPosF, currentRightPosF);
    update();

}

void GenomeAnnoVisualizeWidget::addTextFrame(const GenomeViewTextPainter &textInfo)
{
    pTextPainter -> addPaintItem(textInfo);
}

void GenomeAnnoVisualizeWidget::removeTextFrameAt(int iItem)
{
    pTextPainter -> removePaintItemAt(iItem);
}

void GenomeAnnoVisualizeWidget::lookGene(long long idGene)
{
    GenomeAnnotation* pGenome = pAnnoPainter -> getGenome();
    if (!pGenome) {
        return ;
    }
    if (idGene >= 0 && idGene < pGenome->getNGenes()) {
        GeneAnnotation* pGene = pGenome->getGene(idGene);
        long long* cumChrLen = pChrInfo->getCumChrLen();
        long long maxCumPos = pChrInfo->getMaxCumPos();
        long long tStart = pGene->start + cumChrLen[pGene->idChr];
        long long tStop = pGene->stop + cumChrLen[pGene->idChr];
        long long positionGapLen = tStop - tStart + 1;
        currentLeftPosF = std::max((long long)1, (long long)(tStart - positionGapLen / 2));
        currentRightPosF = std::min((long long)maxCumPos, (long long)(tStop + positionGapLen / 2));
        emit currentIntervalChanged(currentLeftPosF, currentRightPosF);
        update();
    }
}

void GenomeAnnoVisualizeWidget::lookInterval(int leftIdChr, int rightIdChr, long double leftChrPos, long double rightChrPos)
{
    long double maxLeftLen, maxRightLen, tmp;
    int tmpID;
    int nChr = pChrInfo -> getNChr();
    long long* chrLen = pChrInfo -> getChrLen();
    long long* cumChrLen = pChrInfo -> getCumChrLen();
    long long minIntervalBaseNum = pFramePainter -> getMinIntervalBaseNum();
    if (leftIdChr >= nChr) leftIdChr = nChr - 1;
    if (leftIdChr < 0) leftIdChr = 0;
    if (rightIdChr >= nChr) rightIdChr = nChr - 1;
    if (rightIdChr < 0) rightIdChr = 0;
    if (leftIdChr > rightIdChr) {
        tmpID = leftIdChr;
        leftIdChr = rightIdChr;
        rightIdChr = tmpID;
        tmp = leftChrPos;
        leftChrPos = rightChrPos;
        rightChrPos = tmp;
    }
    maxLeftLen = chrLen[leftIdChr], maxRightLen = chrLen[rightIdChr];
    if (leftChrPos < 1) leftChrPos = 1;
    if (leftChrPos > maxLeftLen) leftChrPos = maxLeftLen;
    if (rightChrPos < 1) rightChrPos = 1;
    if (rightChrPos > maxRightLen) rightChrPos = maxRightLen;
    if (leftIdChr == rightIdChr && rightChrPos < leftChrPos) {
        tmp = leftChrPos;
        leftChrPos = rightChrPos;
        rightChrPos = tmp;
    }
    if (leftIdChr == rightIdChr && rightChrPos - leftChrPos < minIntervalBaseNum) {
        return ;
    }
    currentLeftPosF = leftChrPos + cumChrLen[leftIdChr];
    currentRightPosF = rightChrPos + cumChrLen[rightIdChr];
    emit currentIntervalChanged(currentLeftPosF, currentRightPosF);
    update();
}

void GenomeAnnoVisualizeWidget::lookInterval(long double inCurrentLeftPosF, long double inCurrentRightPosF)
{
    long long maxCumChrLen = pChrInfo -> getMaxCumPos();
    if (inCurrentLeftPosF > inCurrentRightPosF) {
        return;
    }
    if (inCurrentLeftPosF < 1) {
        inCurrentLeftPosF = 1;
    }
    if (inCurrentRightPosF > maxCumChrLen) {
        inCurrentRightPosF = maxCumChrLen;
    }
    currentLeftPosF = inCurrentLeftPosF;
    currentRightPosF = inCurrentRightPosF;
    emit currentIntervalChanged(currentLeftPosF, currentRightPosF);
    update();
}

void GenomeAnnoVisualizeWidget::getCurrentInterval(int &leftIdChr, long double &leftChrPos, int &rightIdChr, long double &rightChrPos)
{
    long long* cumChrLen = pChrInfo -> getCumChrLen();
    int currentMinChri, currentMaxChri;
    pChrInfo -> preAdjustPaintChri(currentLeftPosF, currentRightPosF);
    pChrInfo -> getCurrentChri(currentMinChri, currentMaxChri);
    leftIdChr = currentMinChri;
    leftChrPos = currentLeftPosF - cumChrLen[leftIdChr];
    rightIdChr = currentMaxChri;
    rightChrPos = currentRightPosF - cumChrLen[rightIdChr];
}

void GenomeAnnoVisualizeWidget::getCurrentInterval(long double &outCurrentLeftPosF, long double &outCurrentRightPosF)
{
    outCurrentLeftPosF = currentLeftPosF;
    outCurrentRightPosF = currentRightPosF;
}

void GenomeAnnoVisualizeWidget::leftMousePressEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    int mouseX = e->x(), mouseY = e->y();
    int widgetTopMargin = pFramePainter->getWidgetTopMargin();
    int widgetLeftMargin = pFramePainter->getWidgetLeftMargin();
    pAnnoPainter->setDisplaySelectedGeneInfo(false);

    if (mouseY <= widgetTopMargin) {
        if (mouseX >= widgetLeftMargin) {
            isLeftMousePressed = true;
            isMiddleMousePressed = false;
            leftMousePressedPosX = mouseX;
            leftMousePressedMode = G_LEFT_ZOOMOUT;
            setCursor(QCursor(Qt::CrossCursor));
            isNeedUpdate = true;
        }
    } else if (mouseX >= widgetLeftMargin) {
        if (pAnnoPainter->isClicked(QPoint(mouseX, mouseY))) {
            long long iClickedGene = pAnnoPainter->isClickedGene(QPoint(mouseX, mouseY));
            if (iClickedGene >= 0) {
                pAnnoPainter->preSetSelectedGenei(iClickedGene);
                pAnnoPainter->setDisplaySelectedGeneInfo(true);
                emit geneSelected(iClickedGene);
                isNeedUpdate = true;
            }
        }
    }
}

void GenomeAnnoVisualizeWidget::middleMousePressEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    int mouseX = e->x();
    isLeftMousePressed = false;
    isMiddleMousePressed = true;
    middleMousePressedPosX = mouseX;
    middleMouseMovingPosX = mouseX;
    middleMousePressedLeftPosF = currentLeftPosF;
    middleMousePressedRightPosF = currentRightPosF;
    setCursor(QCursor(Qt::OpenHandCursor));
    isNeedUpdate = true;
}

void GenomeAnnoVisualizeWidget::leftMouseReleaseEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    long double leftX, rightX;
    if (!isLeftMousePressed) {
        return ;
    }
    isLeftMousePressed = false;
    leftMouseReleasedPosX = e->x();
    leftMousePressedMode = G_LEFT_DEFAULT;

    pFramePainter->preSetCurrentPosF(currentLeftPosF, currentRightPosF);
    pFramePainter->preSetWidgetSize(size());
    if (pFramePainter->mouseXToPointX(leftMousePressedPosX, leftMouseReleasedPosX, leftX, rightX)) {
        currentLeftPosF = leftX, currentRightPosF = rightX;
        emit currentIntervalChanged(currentLeftPosF, currentRightPosF);
    }
    leftMousePressedPosX = -1;
    leftMouseMovingPosX = -1;
    setCursor(QCursor(Qt::ArrowCursor));
    isNeedUpdate = true;
}

void GenomeAnnoVisualizeWidget::rightMouseReleaseEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    Q_UNUSED(e);
    Q_UNUSED(isNeedUpdate);
    if (!isRightMousePressed) {
        return ;
    }
    isRightMousePressed = false;
}

void GenomeAnnoVisualizeWidget::middleMouseReleaseEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    Q_UNUSED(e);
    if (!isMiddleMousePressed) {
        return ;
    }
    isMiddleMousePressed = false;
    middleMousePressedPosX = -1;
    setCursor(QCursor(Qt::ArrowCursor));
    isNeedUpdate = true;
    emit currentIntervalChanged(currentLeftPosF, currentRightPosF);
}

void GenomeAnnoVisualizeWidget::leftMouseMoveEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    if (!isLeftMousePressed) {
        return ;
    }
    leftMouseMovingPosX = e->x();
    isNeedUpdate = true;
}

void GenomeAnnoVisualizeWidget::middleMouseMoveEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    if (!isMiddleMousePressed) {
        return ;
    }
    int mouseX = e->x();
    int posXGapLen;
    long double posDisplayWidth = width() - pFramePainter -> getWidgetLeftMargin();
    long double chrPos;
    long double chrPosGapLenF,moveChrLen;
    long long maxCumPos = pChrInfo -> getMaxCumPos();
    chrPosGapLenF = currentRightPosF - currentLeftPosF;
    if (mouseX > middleMouseMovingPosX) {
        posXGapLen = mouseX - middleMouseMovingPosX;
        chrPos = std::max((long double)1, (currentLeftPosF - (long double)(posXGapLen) / posDisplayWidth * chrPosGapLenF));
        moveChrLen = currentLeftPosF - chrPos;
        currentLeftPosF -= moveChrLen;
        currentRightPosF -= moveChrLen;
    } else {
        posXGapLen = middleMouseMovingPosX - mouseX;
        chrPos = std::min((long double)maxCumPos, (((long double)(posXGapLen) / posDisplayWidth * chrPosGapLenF) + currentRightPosF));
        moveChrLen = chrPos - currentRightPosF;
        currentLeftPosF += moveChrLen;
        currentRightPosF += moveChrLen;

    }
    middleMouseMovingPosX = mouseX;
    isNeedUpdate = true;
    emit currentIntervalChanged(currentLeftPosF, currentRightPosF);
}

void GenomeAnnoVisualizeWidget::mouseMoveEvent(QMouseEvent *e)
{
    bool isNeedUpdate = false;
    leftMouseMoveEvent(e, isNeedUpdate);
    middleMouseMoveEvent(e, isNeedUpdate);
    rightMouseMoveEvent(e, isNeedUpdate);
    QOpenGLWidget::mouseMoveEvent(e);
    if (pFramePainter -> getDisplayMovingPosLine()) {
        isNeedUpdate = true;
        pFramePainter -> preSetMovingMouseX(e->x());
    }
    if (isNeedUpdate) {
        update();
    }
}

void GenomeAnnoVisualizeWidget::wheelEvent(QWheelEvent *e)
{
    bool isNeedUpdate = false;
    if (e->angleDelta().y() > 0) {
        doZoomIn(isNeedUpdate);
    } else if (e->angleDelta().y() < 0) {
        doZoomOut(isNeedUpdate);
    }
    if (isNeedUpdate) {
        update();
    }
}

void GenomeAnnoVisualizeWidget::doZoomIn(bool& isNeedUpdate)
{
    long double positionGapLenF = currentRightPosF - currentLeftPosF;
    float tmpFactor1 = .5 - 1.0 / (2 * zoomFactor), tmpFactor2 = .5 + 1.0 / (2 * zoomFactor);
    long double pos1 = (positionGapLenF * tmpFactor1) + currentLeftPosF;
    long double pos2 = (positionGapLenF * tmpFactor2) + currentLeftPosF;
    long long minIntervalBaseNum = pFramePainter -> getMinIntervalBaseNum();
    if (pos2 - pos1 < minIntervalBaseNum) {
        return;
    }
    currentLeftPosF = pos1;
    currentRightPosF = pos2;
    emit currentIntervalChanged(currentLeftPosF, currentRightPosF);
    isNeedUpdate = true;
}

void GenomeAnnoVisualizeWidget::doZoomOut(bool& isNeedUpdate)
{
    long double positionGapLenF = currentRightPosF - currentLeftPosF;
    float tmpFactor = 0.5 * zoomFactor;
    long long maxCumPos = pChrInfo -> getMaxCumPos();
    long double pos1 = std::max((long double)(1), (currentLeftPosF) - (positionGapLenF * tmpFactor));
    long double pos2 = std::min((long double)(maxCumPos), (currentRightPosF) + (positionGapLenF * tmpFactor));
    currentLeftPosF = pos1;
    currentRightPosF = pos2;
    emit currentIntervalChanged(currentLeftPosF, currentRightPosF);
    isNeedUpdate = true;
}

void GenomeAnnoVisualizeWidget::initializeGL()
{
    initializeOpenGLFunctions();

    int widgetLeftMargin = pFramePainter -> getWidgetLeftMargin();
    int widgetTopMargin = pFramePainter -> getWidgetTopMargin();
    int width0 = width(), height0 = height();

    pFramePainter -> preSetWidgetSize(size());
    pAnnoPainter  -> preSetDisplayArea(QRect(widgetLeftMargin,
                                           widgetTopMargin,
                                           width0 - widgetLeftMargin,
                                           height0 - widgetTopMargin));
}

void GenomeAnnoVisualizeWidget::paintGL()
{
    QColor backgroundColor = pFramePainter->getBackgroundColor();
    glClearColor(backgroundColor.redF(), backgroundColor.greenF(),
        backgroundColor.blueF(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int widgetLeftMargin = pFramePainter->getWidgetLeftMargin();
    int widgetTopMargin = pFramePainter->getWidgetTopMargin();
    int width0 = width(), height0 = height();

    pChrInfo->preAdjustPaintChri(currentLeftPosF, currentRightPosF);
    pFramePainter->preSetWidgetSize(size());
    pFramePainter->preSetCurrentPosF(currentLeftPosF, currentRightPosF);
    pAnnoPainter->preSetCurrentPosF(currentLeftPosF, currentRightPosF);
    pAnnoPainter->preAdjustPaintGenesi();
    pAnnoPainter->preSetDisplayArea(QRect(widgetLeftMargin, widgetTopMargin,
        width0 - widgetLeftMargin, height0 - widgetTopMargin));

    QPainter Painter;
    Painter.begin(this);

    pFramePainter->paintGenomeViewFrame(isLeftMousePressed, leftMousePressedPosX,
        leftMouseMovingPosX, &Painter);
    pAnnoPainter->paintGenomeAnnoDisplayView(&Painter);

    Painter.end();
}

void GenomeAnnoVisualizeWidget::resizeGL(int w, int h)
{
    iResolution = QVector3D(w, h, 1);
}

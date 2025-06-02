#include "LogPScatterWidget.h"

LogPScatterVisualizeWidget::LogPScatterVisualizeWidget(QWidget *parent)
    :GenomeAnnoVisualizeWidget(parent)
{
    scatterAreaHeightRatio       = 0.6;

    idRightPressPoint            = -1;

    actAddLabelPoint             = nullptr;
    actremoveLabelPoint          = nullptr;

    pPointsPainter = new LogPScatterViewPainter;
    pThresholdPainter = new GWASThresholdPainter;
    pPointsPainter -> preSetChrInfo(pChrInfo);
    pPointsPainter -> preSetPaintWidget(this);
    makeMenu();
}

LogPScatterVisualizeWidget::LogPScatterVisualizeWidget(long long *inChrLen, char **inChrName, int inNChr, ValPoints *inPoints, long long inNPoints, QWidget *parent)
    : LogPScatterVisualizeWidget(parent)
{
    loadChromosome(inChrLen, inChrName, inNChr);
    loadPoints(inPoints, inNPoints);

    setDisplayMovingPosLine(false);
    setDisplayMovingLogPLine(false);
}

LogPScatterVisualizeWidget::LogPScatterVisualizeWidget(const std::vector<long long> &inChrLen, const std::vector<std::string> &inChrName, ValPoints *inPoints, long long inNPoints, QWidget *parent)
    : LogPScatterVisualizeWidget(parent)
{
    loadChromosome(inChrLen, inChrName);
    loadPoints(inPoints, inNPoints);

    setDisplayMovingPosLine(false);
    setDisplayMovingLogPLine(false);
}

LogPScatterVisualizeWidget::~LogPScatterVisualizeWidget()
{
    if (pPointsPainter) {
        delete pPointsPainter;
        pPointsPainter = nullptr;
    }
    if (pThresholdPainter) {
        delete pThresholdPainter;
        pThresholdPainter = nullptr;
    }
}

bool LogPScatterVisualizeWidget::loadPoints(ValPoints *inPoints, long long inNPoints)
{
    bool flag = pPointsPainter -> loadPoints(inPoints, inNPoints);
    if (flag) {
        pThresholdPainter -> preSetValueRange(pPointsPainter->getIntDownLogP(), pPointsPainter->getIntUpLogP());
        pThresholdPainter -> preSetPoints(pPointsPainter->getPoints(), pPointsPainter->getNPoints());
    }
    return flag;
}

int LogPScatterVisualizeWidget::getMaxLabelPointsNum() const
{
    return pPointsPainter -> getMaxLabelPointsNum();
}

ValPoints *LogPScatterVisualizeWidget::getPoints()
{
    return pPointsPainter -> getPoints();
}

long long LogPScatterVisualizeWidget::getNPoints() const
{
    return pPointsPainter -> getNPoints();
}

double LogPScatterVisualizeWidget::getMaxLogP() const
{
    return pPointsPainter -> getMaxLogP();
}

double LogPScatterVisualizeWidget::getMinLogP() const
{
    return pPointsPainter -> getMinLogP();
}

const QVector<QColor> &LogPScatterVisualizeWidget::getPointsColor() const
{
    return pPointsPainter -> getPointsColor();
}

bool LogPScatterVisualizeWidget::createGenome()
{
    return pAnnoPainter -> createGenome();
}

void LogPScatterVisualizeWidget::getThresholdLine(float &val, LineStyleParam& lineStyle) const
{
    val = pThresholdPainter -> getThresholdValue();
    pThresholdPainter -> getLineStyleParam(lineStyle);
}

float LogPScatterVisualizeWidget::getPointSize()
{
    return pPointsPainter -> getPointSize();
}

void LogPScatterVisualizeWidget::dealRemovePointLabel()
{
    int id;
    if (pPointsPainter -> getNPaintPoints() > 0) {
        if ((id = pPointsPainter -> removeLabelPointID(idRightPressPoint)) >= 0) {
            emit removeLabelPoint(id);
            update();
        }
    }
}

void LogPScatterVisualizeWidget::dealAddPointLabel()
{
    if (pPointsPainter -> getNPaintPoints() > 0) {
        if (pPointsPainter -> addLabelPoint(idRightPressPoint) >= 0) {
            emit addLabelPoint(idRightPressPoint);
            update();
        }
    }
}

void LogPScatterVisualizeWidget::paintCurrent(QPainter *painter)
{
//    paintVerticalLineInPainter(painter);
//    paintAxisInPainter(painter);
//    if(genome&&geneDisplayMode) paintGenesInPainter(painter);
//    if(isDisplayGeneModel) paintGenesModelInPainter(painter);
//    if(genomeSeq&&isDisplayGenomeSeq) paintGenomeSeq(painter);
}

void LogPScatterVisualizeWidget::setPointsColor(const QVector<QColor>& inColor)
{
    pPointsPainter -> setColorScatterPoint(inColor);
    update();
}


void LogPScatterVisualizeWidget::getTotalGPUMemory(GLint &valKb)
{
    glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &valKb);
}

void LogPScatterVisualizeWidget::getCurrentGPUMemory(GLint &valKb)
{
    glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &valKb);
}

void LogPScatterVisualizeWidget::doRemoveAllLabelPoint()
{
    pPointsPainter -> clearLabelPoints();
    update();
}

void LogPScatterVisualizeWidget::doRemoveLabelPoint(int row)
{
    pPointsPainter -> removeLabelPointAt(row);
    update();
}

void LogPScatterVisualizeWidget::doLookLabelPoint(int row)
{
    long long idPoint = pPointsPainter -> getLabelPointAt(row);
    ValPoints *tPoint = nullptr;
    long long* cumChrLen = pChrInfo -> getCumChrLen();
    long long maxCumPos = pChrInfo -> getMaxCumPos();
    if (idPoint > 0) {
        tPoint = pPointsPainter -> getPointValAt(idPoint);
        long double tmpPos = tPoint->pos + cumChrLen[tPoint->idchr];
        long double positionGapLenF = currentRightPosF - currentLeftPosF;
        if (positionGapLenF > 1000000) {
            positionGapLenF = 1000000;
        }
        long double tStart = tmpPos-positionGapLenF / 2;
        long double tStop = tmpPos+positionGapLenF / 2;
        currentLeftPosF = tStart < 1.0 ? 1 : tStart;
        currentRightPosF = tStop > maxCumPos ? maxCumPos : tStop;
        emit currentIntervalChanged(currentLeftPosF, currentRightPosF);
        update();
    }
}

void LogPScatterVisualizeWidget::setThresholdLine(float val, LineStyleParam lineStyle)
{
    pThresholdPainter -> setLineStyleParam(lineStyle);
    pThresholdPainter -> preSetThresholdValue(val);
    pThresholdPainter -> preSetIsPaintInfo(true);
    pThresholdPainter -> preSetIsPaintLine(true);
    update();
}

void LogPScatterVisualizeWidget::setDisplayMovingPosLine(bool flag)
{
    pFramePainter -> setDisplayMovingPosLine(flag);
    bool traceFlag = (flag || pPointsPainter -> getDisplayMovingLogPLine());
    setMouseTracking(traceFlag);
    update();
}

void LogPScatterVisualizeWidget::setDisplayMovingLogPLine(bool flag)
{
    pPointsPainter -> setDisplayMovingLogPLine(flag);
    bool traceFlag = (flag || pFramePainter -> getDisplayMovingPosLine());
    setMouseTracking(traceFlag);
    update();
}

void LogPScatterVisualizeWidget::leftMousePressEvent(QMouseEvent *e,bool& isNeedUpdate)
{
    int mouseX = e->x(), mouseY = e->y();
    int widgetTopMargin = pFramePainter->getWidgetTopMargin();
    int widgetLeftMargin = pFramePainter->getWidgetLeftMargin();
    //displaySelectedPointInfo=false;
    pAnnoPainter->setDisplaySelectedGeneInfo(false);
    pPointsPainter->setDisplaySelectedPointInfo(false);
    pPointsPainter->preSetSelectedPointi(-1);
    QPoint clickPoint = QPoint(mouseX, mouseY);
    GenomeAnnotation* genome = pAnnoPainter->getGenome();
    ValPoints* points = pPointsPainter->getPoints();
    long long nGenes = pAnnoPainter->getNGenes();
    if (pThresholdPainter) {
        pThresholdPainter->preSetIsPaintInfo(false);
    }

    if (mouseY <= widgetTopMargin) {
        if(mouseX>=widgetLeftMargin) {
            isLeftMousePressed = true;
            isMiddleMousePressed = false;
            leftMousePressedPosX = mouseX;
            leftMousePressedMode = G_LEFT_ZOOMOUT;
            setCursor(QCursor(Qt::CrossCursor));
        }
    } else if (mouseX >= widgetLeftMargin) {
        if (pAnnoPainter->isClicked(clickPoint)) {
            long long iClickedGene = pAnnoPainter->isClickedGene(clickPoint);
            if (iClickedGene >= 0) {
                pAnnoPainter->preSetSelectedGenei(iClickedGene);
                pAnnoPainter->setDisplaySelectedGeneInfo(true);
                emit geneSelected(iClickedGene);
            }
        }
        if (pPointsPainter->isClicked(clickPoint)) {
            long long iClickedPoint = pPointsPainter->isClickedPoint(clickPoint);
            if (iClickedPoint >= 0) {
                long long idGene = genome->search(points[iClickedPoint].idchr, points[iClickedPoint].pos, GenomeAnnotation::GA_STOP);
                int idChr = points[iClickedPoint].idchr;
                long long pos = points[iClickedPoint].pos;
                pAnnoPainter->preSetSelectedPointGenei(-1);
                pPointsPainter->preSetSelectedPointi(iClickedPoint);
                pPointsPainter->setDisplaySelectedPointInfo(true);
                if (nGenes > 0) {
                    if (idGene > 0 && genome->getGene(idGene)->contain(idChr, pos)) {
                        pAnnoPainter->preSetSelectedPointGenei(idGene);
                        emit pointSelected(iClickedPoint, idGene, idGene);
                    } else {
                        emit pointSelected(iClickedPoint, idGene - 1, idGene);
                    }
                } else {
                    emit pointSelected(iClickedPoint, -1, -1);
                }
            }
        }
    }
    isNeedUpdate = true;
}

void LogPScatterVisualizeWidget::rightMousePressEvent(QMouseEvent *e, bool& isNeedUpdate)
{
    Q_UNUSED(isNeedUpdate);
    int mouseX = e->x(), mouseY = e->y();

    isRightMousePressed = true;
    RightMousePressedPosX = mouseX;
    RightMousePressedPosY = mouseY;
    idRightPressPoint = pPointsPainter -> isClickedPoint(QPoint(mouseX, mouseY));
    if (idRightPressPoint >= 0) {
        actAddLabelPoint->setEnabled(true);
        actremoveLabelPoint->setEnabled(true);
    } else {
        actAddLabelPoint->setDisabled(true);
        actremoveLabelPoint->setDisabled(true);
    }
}

void LogPScatterVisualizeWidget::mouseMoveEvent(QMouseEvent *e)
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
    if (pPointsPainter -> getDisplayMovingLogPLine()) {
        isNeedUpdate = true;
        pPointsPainter -> preSetMovingMouseY(e->y());
    }
    if (isNeedUpdate) {
        update();
    }
}

void LogPScatterVisualizeWidget::makeMenu()
{
    pMenu = new QMenu(this);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &LogPScatterVisualizeWidget::customContextMenuRequested, this, &LogPScatterVisualizeWidget::menuExec);
    actAddLabelPoint = new QAction("Label", pMenu);
    actremoveLabelPoint = new QAction("Remove Label", pMenu);
    QAction* actZoomIn = new QAction("Zoom in", pMenu);
    QAction* actZoomOut = new QAction("Zoom out", pMenu);
    pMenu->addAction(actAddLabelPoint);
    pMenu->addAction(actremoveLabelPoint);
    pMenu->addSeparator();
    pMenu->addAction(actZoomIn);
    pMenu->addAction(actZoomOut);
    connect(actAddLabelPoint, &QAction::triggered, this, &LogPScatterVisualizeWidget::dealAddPointLabel);
    connect(actremoveLabelPoint, &QAction::triggered, this, &LogPScatterVisualizeWidget::dealRemovePointLabel);
    connect(actZoomIn, &QAction::triggered, this, &LogPScatterVisualizeWidget::zoomIn);
    connect(actZoomOut, &QAction::triggered, this, &LogPScatterVisualizeWidget::zoomOut);
}

void LogPScatterVisualizeWidget::initializeGL()
{
    initializeOpenGLFunctions();

    pPointsPainter -> initInitializeGL();

    update();
}

void LogPScatterVisualizeWidget::paintGL()
{
    QColor backgroundColor = pFramePainter -> getBackgroundColor();

    glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int widgetLeftMargin = pFramePainter->getWidgetLeftMargin();
    int widgetTopMargin = pFramePainter->getWidgetTopMargin();
    int width0 = width(), height0 = height();
    int scatterAreaHeight = scatterAreaHeightRatio * (height0 - widgetTopMargin);
    int annoAreaHeight = height0 - widgetTopMargin - scatterAreaHeight;

    pChrInfo->preAdjustPaintChri(currentLeftPosF, currentRightPosF);
    pFramePainter->preSetWidgetSize(size());
    pFramePainter->preSetCurrentPosF(currentLeftPosF, currentRightPosF);
    pAnnoPainter->preSetCurrentPosF(currentLeftPosF, currentRightPosF);
    pAnnoPainter->preAdjustPaintGenesi();
    pAnnoPainter->preSetDisplayArea(QRect(widgetLeftMargin, widgetTopMargin + scatterAreaHeight,
        width0 - widgetLeftMargin, annoAreaHeight));
    pPointsPainter->preSetDisplayArea(QRect(widgetLeftMargin, widgetTopMargin,
        width0 - widgetLeftMargin, scatterAreaHeight));
    pPointsPainter->preSetWidgetSize(size());
    pPointsPainter->preSetCurrentPosF(currentLeftPosF, currentRightPosF);
    pThresholdPainter->preSetDisplayArea(QRect(widgetLeftMargin, widgetTopMargin,
        width0 - widgetLeftMargin, scatterAreaHeight));

    QPainter Painter;
    Painter.begin(this);

    pFramePainter->paintGenomeViewFrame(isLeftMousePressed, leftMousePressedPosX, leftMouseMovingPosX, &Painter);
    pPointsPainter->paintLogPScatterView(&Painter);
    pAnnoPainter->paintGenomeAnnoDisplayView(&Painter);

    pFramePainter->paintMovingPosLine(&Painter);
    pPointsPainter->paintMovingLogPLine(&Painter);
    pThresholdPainter->paintThreshold(&Painter);

    Painter.end();
}

void LogPScatterVisualizeWidget::resizeGL(int w, int h)
{
    iResolution = QVector3D(w,h,1);
    // update();
}

void LogPScatterVisualizeWidget::savePaintCurrentSVG(QString savePath, QSizeF inSize, long double inLeftPosF, long double inRightPosF)
{
    if (savePath.isEmpty()) {
        return;
    }
    QSvgGenerator generator;
    int width0 = inSize.width(), height0 = inSize.height();
    generator.setFileName(savePath);
    generator.setSize(QSize(inSize.width(), inSize.height()));

    int widgetLeftMargin = pFramePainter -> getWidgetLeftMargin();
    int widgetTopMargin = pFramePainter -> getWidgetTopMargin();
    int scatterAreaHeight = scatterAreaHeightRatio*(height0 - widgetTopMargin);
    int annoAreaHeight = height0 - widgetTopMargin - scatterAreaHeight;

    QPainter Painter;

    pChrInfo -> preAdjustPaintChri(inLeftPosF, inRightPosF);
    pFramePainter -> preSetWidgetSize(QSize(inSize.width(), inSize.height()));
    pFramePainter -> preSetCurrentPosF(inLeftPosF, inRightPosF);
    pAnnoPainter  -> preSetCurrentPosF(inLeftPosF, inRightPosF);
    pAnnoPainter  -> preAdjustPaintGenesi();
    pAnnoPainter  -> preSetDisplayArea(QRect(widgetLeftMargin, widgetTopMargin + scatterAreaHeight,
        width0 - widgetLeftMargin, annoAreaHeight));
    pPointsPainter -> preSetDisplayArea(QRect(widgetLeftMargin, widgetTopMargin,
        width0 - widgetLeftMargin, scatterAreaHeight));
    pPointsPainter -> preSetWidgetSize(size());
    pPointsPainter -> preSetCurrentPosF(inLeftPosF, inRightPosF);
    pThresholdPainter -> preSetDisplayArea(QRect(widgetLeftMargin, widgetTopMargin,
        width0 - widgetLeftMargin, scatterAreaHeight));

    Painter.begin(&generator);

    pFramePainter->savePaintGenomeViewFrame(&Painter);
    pPointsPainter->savePaintLogPScatterView(&Painter);
    pAnnoPainter->savePaintGenomeAnnoDisplayView(&Painter);
    pThresholdPainter->paintThreshold(&Painter);

    Painter.end();
}

void LogPScatterVisualizeWidget::savePaintCurrentPNG(QString savePath, QSizeF inSize, long double inLeftPosF, long double inRightPosF)
{
    if(savePath.isEmpty()) return;
    QPixmap pixmap(QSize(inSize.width(),inSize.height()));
    pixmap.fill(Qt::white);
    int width0=inSize.width(),height0=inSize.height();

    int widgetLeftMargin = pFramePainter -> getWidgetLeftMargin();
    int widgetTopMargin = pFramePainter -> getWidgetTopMargin();
    int scatterAreaHeight = scatterAreaHeightRatio*(height0 - widgetTopMargin);
    int annoAreaHeight = height0 - widgetTopMargin - scatterAreaHeight;

    QPainter Painter;

    pChrInfo -> preAdjustPaintChri(inLeftPosF,inRightPosF);
    pFramePainter -> preSetWidgetSize(QSize(inSize.width(),inSize.height()));
    pFramePainter -> preSetCurrentPosF(inLeftPosF,inRightPosF);
    pAnnoPainter  -> preSetCurrentPosF(inLeftPosF,inRightPosF);
    pAnnoPainter  -> preAdjustPaintGenesi();
    pAnnoPainter  -> preSetDisplayArea(QRect(widgetLeftMargin, widgetTopMargin + scatterAreaHeight,
        width0 - widgetLeftMargin, annoAreaHeight));
    pPointsPainter -> preSetDisplayArea(QRect(widgetLeftMargin, widgetTopMargin,
        width0 - widgetLeftMargin, scatterAreaHeight));
    pPointsPainter -> preSetWidgetSize(size());
    pPointsPainter -> preSetCurrentPosF(inLeftPosF, inRightPosF);
    pThresholdPainter -> preSetDisplayArea(QRect(widgetLeftMargin, widgetTopMargin,
        width0 - widgetLeftMargin, scatterAreaHeight));

    Painter.begin(&pixmap);

    pFramePainter->savePaintGenomeViewFrame(&Painter);
    pPointsPainter->savePaintLogPScatterView(&Painter);
    pAnnoPainter->savePaintGenomeAnnoDisplayView(&Painter);
    pThresholdPainter->paintThreshold(&Painter);

    Painter.end();

    pixmap.save(savePath);
}

void LogPScatterVisualizeWidget::menuExec(const QPoint &pos)
{
    pMenu->exec(this->mapToGlobal(pos));
}

void LogPScatterVisualizeWidget::savePlot(FD::SaveFileInfo inInfo)
{
    FD::SVGFileFormInfo *svgInfo = nullptr;
    FD::PNGFileFormInfo *pngInfo = nullptr;
    switch (inInfo.getIDForm()) {
        case FD::FILEFORM_SVG: {
            if (inInfo.getFileInfo()) {
                svgInfo = dynamic_cast<FD::SVGFileFormInfo *>(inInfo.getFileInfo());
                savePaintCurrentSVG(svgInfo->filePath, svgInfo->saveSize,
                    currentLeftPosF, currentRightPosF);
            }
            break;
        }
        case FD::FILEFORM_PNG: {
            if (inInfo.getFileInfo()) {
                pngInfo = dynamic_cast<FD::PNGFileFormInfo *>(inInfo.getFileInfo());
                savePaintCurrentPNG(pngInfo->filePath, pngInfo->saveSize,
                    currentLeftPosF, currentRightPosF);
            }
            break;
        }
        default:
            break;
    }
}

void LogPScatterVisualizeWidget::setPointSize(float val)
{
    pPointsPainter -> setPointSize(val);
    update();
}

#include "KinshipHeatmapWidget.hpp"

#include <QSvgGenerator>

KinshipHeatmapWidget::KinshipHeatmapWidget(QWidget *parent)
    : BaseOpenGLPaintWidget(parent)
{
    leftMousePressedMode = KIN_LEFT_DEFAULT;

    pHeatmapPainter = new KinshipHeatmapPainter;
    pLegendPainter = new KinshipLegendPainter;
    pFramePainter = new KinshipFramePainter;
}

KinshipHeatmapWidget::KinshipHeatmapWidget(const MML::Mat &inKin, char** inName, QWidget *parent)
    : KinshipHeatmapWidget(parent)
{
    xCamera        = 0.0;
    yCamera        = 0.0;
    zoomValue      = 1.0;
    maxZoomValue   = 8.0;
    minZoomValue   = 0.2;
    zoomTick       = 0.2;

    minXCamera     = -2;
    maxXCamera     = 2;
    minYCamera     = -2;
    maxYCamera     = 2;

    pHeatmapPainter -> loadKinshipData(inKin,inName);
    pLegendPainter -> setIsPaintLegend(true);
    pLegendPainter -> setVal(pHeatmapPainter -> getMaxVal(),
                             pHeatmapPainter -> getMinVal(),
                             pHeatmapPainter -> getMeanVal());
    pLegendPainter -> setRectColor(pHeatmapPainter -> getLowColor(),
                                   pHeatmapPainter -> getHighColor(),
                                   pHeatmapPainter -> getMeanColor());
    setViewMat();
}

KinshipHeatmapWidget::~KinshipHeatmapWidget()
{
    if(pHeatmapPainter){
        delete pHeatmapPainter;
        pHeatmapPainter = nullptr;
    }
    if(pLegendPainter){
        delete pLegendPainter;
        pLegendPainter = nullptr;
    }
    if(pFramePainter){
        delete pFramePainter;
        pFramePainter = nullptr;
    }
}

void KinshipHeatmapWidget::mouseMoveEvent(QMouseEvent *e)
{
    int x1=e->x(),y1=e->y();
    int dx=0,dy=0;
    bool isUpdate = false;
    if(isMiddleMousePressed){
        dx=x1-middleMousePressedPosX;
        dy=y1-middleMousePressedPosY;
        yCamera-=dy/(zoomValue*frameSize);
        xCamera-=dx/(zoomValue*frameSize);
        if(xCamera>maxXCamera) xCamera=maxXCamera;
        if(xCamera<minXCamera) xCamera=minXCamera;
        if(yCamera>maxYCamera) yCamera=maxYCamera;
        if(yCamera<minYCamera) yCamera=minYCamera;
        middleMousePressedPosX=x1;
        middleMousePressedPosY=y1;
        isUpdate = true;
    }
    if(isLeftMousePressed){
        if(leftMousePressedMode == KIN_LEFT_LEGEND){
            pLegendPainter -> setPaintPoint(
                        QPoint(oldLegendPoint.x()+x1-leftMousePressedPosX,
                               oldLegendPoint.y()+y1-leftMousePressedPosY));
            isUpdate = true;
        }
    }
    if(isUpdate) update();
    QOpenGLWidget::mouseMoveEvent(e);
}

void KinshipHeatmapWidget::wheelEvent(QWheelEvent *e)
{
    if(e->angleDelta().y()>0) zoomValue+=zoomTick;
    else if(e->angleDelta().y()<0) zoomValue-=zoomTick;
    if(zoomValue>maxZoomValue) zoomValue=maxZoomValue;
    if(zoomValue<minZoomValue) zoomValue=minZoomValue;
    update();
}

void KinshipHeatmapWidget::setViewMat()
{
    QMatrix4x4 moveMat,reshapeMat;
    moveMat.setToIdentity();
    reshapeMat.setToIdentity();
    moveMat(0,3)=-xCamera;
    moveMat(1,3)=-yCamera;
    reshapeMat(0,0)=zoomValue;
    reshapeMat(1,1)=zoomValue;
    viewMat=reshapeMat*moveMat;
}

void KinshipHeatmapWidget::setClusterIndex()
{
    pHeatmapPainter -> setClusterIndex();
    update();
}

void KinshipHeatmapWidget::setRangeValue(double highVal, double lowVal, double meanVal)
{
    pHeatmapPainter -> setRangeValue(highVal,lowVal,meanVal);
    pLegendPainter -> setVal(highVal,lowVal,meanVal);
}

void KinshipHeatmapWidget::setRangeColor(QColor highCol, QColor lowCol, QColor meanCol)
{
    pHeatmapPainter -> setRangeColor(highCol,lowCol,meanCol);
    pLegendPainter -> setRectColor(highCol,lowCol,meanCol);
}

void KinshipHeatmapWidget::getRangeValue(double &outHighVal, double &outLowVal, double& outMeanVal)
{
    outHighVal = pHeatmapPainter -> getMaxVal();
    outLowVal = pHeatmapPainter -> getMinVal();
    outMeanVal = pHeatmapPainter -> getMeanVal();
}

void KinshipHeatmapWidget::getRangeColor(QColor &outHighCol, QColor &outLowCol, QColor &outMeanCol)
{
    outHighCol = pHeatmapPainter -> getHighColor();
    outLowCol = pHeatmapPainter -> getLowColor();
    outMeanCol = pHeatmapPainter -> getMeanColor();
}

void KinshipHeatmapWidget::leftMousePressEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    int x1=e->x(),y1=e->y();
    isLeftMousePressed=true;
    leftMousePressedPosX = x1;
    leftMousePressedPosY = y1;
    if(pLegendPainter -> isClicked(QPoint(x1,y1))){
        leftMousePressedMode = KIN_LEFT_LEGEND;
        oldLegendPoint = pLegendPainter -> getPaintPoint();
        return ;
    }
    long long iRow, iCol;
    pHeatmapPainter -> setIsDisplaySelectedText(false);
    pHeatmapPainter -> isClickedPoint(QPoint(x1,y1), iRow, iCol);
    if(iRow < 0 || iCol < 0){
        return ;
    }
    leftMousePressedMode = KIN_LEFT_MAIN;
    pHeatmapPainter -> setSelectedIndex(iRow, iCol);
    pHeatmapPainter -> setIsDisplaySelectedText(true);
    isNeedUpdate = true;
}

void KinshipHeatmapWidget::middleMousePressEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    int x1=e->x(),y1=e->y();
    isMiddleMousePressed=true;
    middleMousePressedPosX = x1;
    middleMousePressedPosY = y1;
    setCursor(QCursor(Qt::OpenHandCursor));
}

void KinshipHeatmapWidget::leftMouseReleaseEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    isLeftMousePressed=false;
    leftMousePressedMode = KIN_LEFT_DEFAULT;
}

void KinshipHeatmapWidget::middleMouseReleaseEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    isMiddleMousePressed=false;
    setCursor(QCursor(Qt::ArrowCursor));
}

void KinshipHeatmapWidget::initializeGL()
{
    initializeOpenGLFunctions();
}

void KinshipHeatmapWidget::paintGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    setViewMat();

    pHeatmapPainter -> setOnePointSize(double(frameSize)/pHeatmapPainter -> getNIndividuals());
    pHeatmapPainter -> setMatView(viewMat);

    QPainter Painter;

    Painter.begin(this);

    pFramePainter -> calcuPaintRect(viewMat,frameSize);
    pFramePainter -> paintFrame(&Painter);
    pHeatmapPainter -> paintMap(&Painter);
    pLegendPainter -> paintLegend(&Painter);

    Painter.end();
}

void KinshipHeatmapWidget::resizeGL(int w, int h)
{
    frameSize = std::min(w,h)*zoomValue;
}

void KinshipHeatmapWidget::savePaintCurrentSVG(QString savePath, QSizeF inSize)
{
    if(savePath.isEmpty()) return;
    QSvgGenerator generator;
    generator.setFileName(savePath);
    generator.setSize(QSize(inSize.width(),inSize.height()));

    setViewMat();

    pHeatmapPainter -> setOnePointSize(double(frameSize)/pHeatmapPainter -> getNIndividuals());
    pHeatmapPainter -> setMatView(viewMat);

    QPainter Painter;
    Painter.begin(&generator);

    pFramePainter -> calcuPaintRect(viewMat,frameSize);
    pFramePainter -> paintFrame(&Painter);
    pHeatmapPainter -> savePaintMap(&Painter);
    pLegendPainter -> paintLegend(&Painter);

    Painter.end();
}

void KinshipHeatmapWidget::savePaintCurrentPNG(QString savePath, QSizeF inSize)
{
    if(savePath.isEmpty()) return;
    QPixmap pixmap(QSize(inSize.width(),inSize.height()));
    pixmap.fill(Qt::white);

    setViewMat();
    pHeatmapPainter -> setOnePointSize(double(frameSize)/pHeatmapPainter -> getNIndividuals());
    pHeatmapPainter -> setMatView(viewMat);

    QPainter Painter;

    Painter.begin(&pixmap);

    pFramePainter -> calcuPaintRect(viewMat,frameSize);
    pFramePainter -> paintFrame(&Painter);
    pHeatmapPainter -> savePaintMap(&Painter);
    pLegendPainter -> paintLegend(&Painter);

    Painter.end();

    pixmap.save(savePath);
}

void KinshipHeatmapWidget::savePlot(FD::SaveFileInfo inInfo)
{
    FD::SVGFileFormInfo *svgInfo = nullptr;
    FD::PNGFileFormInfo *pngInfo = nullptr;
    switch (inInfo.getIDForm()) {
    case FD::FILEFORM_SVG:{
        if(inInfo.getFileInfo()){
            svgInfo = dynamic_cast<FD::SVGFileFormInfo *>(inInfo.getFileInfo());
            savePaintCurrentSVG(svgInfo->filePath,svgInfo->saveSize);
        }
        break;
    }
    case FD::FILEFORM_PNG:{
        if(inInfo.getFileInfo()){
            pngInfo = dynamic_cast<FD::PNGFileFormInfo *>(inInfo.getFileInfo());
            savePaintCurrentPNG(pngInfo->filePath,pngInfo->saveSize);
        }
        break;
    }
    default:
        break;
    }

    update();
}


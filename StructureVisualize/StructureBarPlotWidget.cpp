#include "StructureBarPlotWidget.hpp"

#include <QSvgGenerator>

void StructureBarPlotWidget::leftMousePressEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    int x0=e->x(),y0=e->y();
    pBarPainter -> setIsDisplaySelectedText(false);
    long long idPaint = pBarPainter -> isClickedInd(QPoint(x0,y0));
    if(idPaint >=0 ){
        pBarPainter -> setIsDisplaySelectedText(true);
    }
    pBarPainter -> setSelectedInd(idPaint);
    isNeedUpdate = true;
}

void StructureBarPlotWidget::middleMousePressEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    Q_UNUSED(isNeedUpdate);
    int x0=e->x(),y0=e->y();
    isMiddleMousePressed=true;
    middleMousePressedPosX=x0;
    middleMousePressedPosY=y0;
    setCursor(QCursor(Qt::OpenHandCursor));
}

void StructureBarPlotWidget::middleMouseReleaseEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    isMiddleMousePressed=false;
    middleMousePressedPosX=0;
    middleMousePressedPosY=0;
    setCursor(QCursor(Qt::ArrowCursor));
}

StructureBarPlotWidget::StructureBarPlotWidget(QWidget *parent)
    : BaseOpenGLPaintWidget(parent)
{
    minXCamera               = -2;
    maxXCamera               = 2;
    minYCamera               = -2;
    maxYCamera               = 2;
    zoomTick                 = 0.1;
    maxZoomValue             = 8;
    minZoomValue             = 0.2;
    xCamera                  = 0;
    yCamera                  = 0;
    zoomValues               = 0.8;
    nameRotateAngle          = -90;
    paintFrameSize           = QSize(1600,400);

    displayRotateMat.setToIdentity();

    pBarPainter = new StructureBarPainter;
    pBarPainter -> setFrameSize(paintFrameSize);
    pBarPainter -> setXBarAreaFactor(1.0);
    pBarPainter -> setYBarAreaFactor(1.0);

    pFramePainter = new StructureFramePainter;
    pFramePainter -> setBackgroundColor(QColor(190,233,214));
}

StructureBarPlotWidget::StructureBarPlotWidget(const MML::Mat &inData,char **inName, QWidget *parent)
    :StructureBarPlotWidget(parent)
{
    if(inData.info==MML::_null||
       inData.getNCol()==0||
       inData.getNRow()==0||
       (!inName)) return;
    pBarPainter -> loadIndData(inData,inName);
}

StructureBarPlotWidget::~StructureBarPlotWidget()
{
    if(pBarPainter){
        delete pBarPainter;
        pBarPainter = nullptr;
    }
}

void StructureBarPlotWidget::mouseMoveEvent(QMouseEvent *e)
{
    int x0 = e->x(),y0 = e->y();
    int dx = 0,dy = 0;
    int xLen,yLen;
    if(pBarPainter -> getIsVertical()){
        xLen = paintFrameSize.height();
        yLen = paintFrameSize.width();
    }
    else{
        xLen = paintFrameSize.width();
        yLen = paintFrameSize.height();
    }
    if(isMiddleMousePressed == true){
        dx = x0 - middleMousePressedPosX;
        dy = y0 - middleMousePressedPosY;
        yCamera -= dy/(zoomValues*yLen);
        xCamera -= dx/(zoomValues*xLen);
        if(xCamera > maxXCamera) xCamera = maxXCamera;
        if(xCamera < minXCamera) xCamera = minXCamera;
        if(yCamera > maxYCamera) yCamera = maxYCamera;
        if(yCamera < minYCamera) yCamera = minYCamera;
        middleMousePressedPosX = x0;
        middleMousePressedPosY = y0;
        update();
    }
    QOpenGLWidget::mouseMoveEvent(e);
}

void StructureBarPlotWidget::wheelEvent(QWheelEvent *e)
{
    if(e->angleDelta().y()>0) zoomValues+=zoomTick;
    else if(e->angleDelta().y()<0) zoomValues-=zoomTick;
    if(zoomValues>maxZoomValue) zoomValues=maxZoomValue;
    if(zoomValues<minZoomValue) zoomValues=minZoomValue;
    setViewMat();
    update();
    QOpenGLWidget::wheelEvent(e);
}

void StructureBarPlotWidget::sortPaintSeq()
{
    pBarPainter -> sortPaintSeq();
    update();
}

void StructureBarPlotWidget::setIsDisplayMainFrame(bool flag)
{

}

void StructureBarPlotWidget::setIsDisplayBarFrame(bool flag)
{
    pBarPainter -> setIsDisplayBarFrame(flag);
    update();
}

void StructureBarPlotWidget::setBarGapFactor(double factor)
{
    if(factor < 0 || factor > 1) return;
    pBarPainter -> setBarGapFactor(factor);
    update();
}

void StructureBarPlotWidget::setXBarMarginFactor(double factor)
{
    if(factor < 0 || factor > 1) return;
    pBarPainter -> setXBarAreaFactor(factor);
    update();
}

void StructureBarPlotWidget::setYBarMarginFactor(double factor)
{
    if(factor < 0 || factor > 1) return;
    pBarPainter -> setYBarAreaFactor(factor);
    update();
}

void StructureBarPlotWidget::changeOrientation()
{
    if(pBarPainter -> getIsVertical()){
        setHorizontal();
    }
    else{
        setVertical();
    }
}

void StructureBarPlotWidget::setSearchedSample(long long idPaint)
{
    long long nInd = pBarPainter -> getNIndividuals();
    if(idPaint < 0||idPaint >= nInd) return;
    for(long long i=0;i<nInd;++i){
        if(pBarPainter -> getIdPaintSeq(i) == idPaint){
            pBarPainter -> setSearchedInd(i);
            break;
        }
    }
    update();
}

void StructureBarPlotWidget::setSelectedSample(long long idPaint)
{
    long long nInd = pBarPainter -> getNIndividuals();
    if(idPaint < 0||idPaint >= nInd) return;
    for(long long i=0;i<nInd;++i){
        if(pBarPainter -> getIdPaintSeq(i) == idPaint){
            pBarPainter -> setSelectedInd(i);
            break;
        }
    }
    update();
}

void StructureBarPlotWidget::setColorPopList(const QVector<QColor> &inList)
{
    if(inList.empty()) return;
    pBarPainter -> setColorPopList(inList);
    update();
}

void StructureBarPlotWidget::setPaintFrameSize(QSize size)
{
    if(size.width() <= 0 || size.height() <= 0){
        return;
    }
    paintFrameSize = size;
    pBarPainter -> setFrameSize(paintFrameSize);
    update();
}

const QVector<QColor> &StructureBarPlotWidget::getColorPopList() const
{
    return pBarPainter -> getColorPopList();
}

void StructureBarPlotWidget::setVertical()
{
    if(pBarPainter -> getIsVertical()) return;
    pBarPainter -> setIsVertical(true);
    nameRotateAngle+=90;
    QMatrix4x4 move2(1,0,0,0.5,
                     0,1,0,0.5,
                     0,0,1,0,
                     0,0,0,1);
    QMatrix4x4 move1(1,0,0,-0.5,
                     0,1,0,-0.5,
                     0,0,1,0,
                     0,0,0,1);
    QMatrix4x4 transform1(0,-1,0,0,
                         1,0,0,0,
                         0,0,1,0,
                         0,0,0,1);
    QMatrix4x4 transform2(-1,0,0,0,
                          0,1,0,0,
                          0,0,1,0,
                          0,0,0,1);
    displayRotateMat=move2*transform2*transform1*move1;
    update();
}

void StructureBarPlotWidget::setHorizontal()
{
    if(!(pBarPainter -> getIsVertical())) return;
    pBarPainter -> setIsVertical(false);
    nameRotateAngle-=90;
    displayRotateMat.setToIdentity();
    update();
}

void StructureBarPlotWidget::setViewMat()
{
    QMatrix4x4 moveMat,reshapeMat;
    moveMat.setToIdentity();
    reshapeMat.setToIdentity();
    moveMat(0,3)=-xCamera;
    moveMat(1,3)=-yCamera;
    reshapeMat(0,0)=zoomValues;
    reshapeMat(1,1)=zoomValues;
    viewMat=reshapeMat*moveMat;
}

void StructureBarPlotWidget::initializeGL()
{
    initializeOpenGLFunctions();
}

void StructureBarPlotWidget::paintGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    setViewMat();
    pBarPainter -> setViewMat(viewMat);
    pBarPainter -> setRotationMat(displayRotateMat);

    QPainter Painter;

    Painter.begin(this);
    pFramePainter -> calcuPaintRect(paintFrameSize,viewMat,
                                    displayRotateMat,pBarPainter -> getIsVertical());
    pFramePainter -> paintFrame(&Painter);
    pBarPainter -> paintStart(&Painter);
    Painter.end();
}

void StructureBarPlotWidget::resizeGL(int w, int h)
{

}

void StructureBarPlotWidget::savePaintCurrentSVG(QString savePath, QSizeF inSize)
{
    if(savePath.isEmpty()) return;
    QSvgGenerator generator;
    generator.setFileName(savePath);
    generator.setSize(QSize(inSize.width(),inSize.height()));

    setViewMat();
    pBarPainter -> setViewMat(viewMat);
    pBarPainter -> setRotationMat(displayRotateMat);

    QPainter Painter;
    Painter.begin(&generator);

    pFramePainter -> calcuPaintRect(paintFrameSize,viewMat,
                                  displayRotateMat,pBarPainter -> getIsVertical());
    pFramePainter -> paintFrame(&Painter);
    pBarPainter -> savePaintBar(&Painter);

    Painter.end();
}

void StructureBarPlotWidget::savePaintCurrentPNG(QString savePath, QSizeF inSize)
{
    if(savePath.isEmpty()) return;
    QPixmap pixmap(QSize(inSize.width(),inSize.height()));
    pixmap.fill(Qt::white);

    setViewMat();
    pBarPainter -> setViewMat(viewMat);
    pBarPainter -> setRotationMat(displayRotateMat);

    QPainter Painter;

    Painter.begin(&pixmap);

    pFramePainter -> calcuPaintRect(paintFrameSize,viewMat,
                                  displayRotateMat,pBarPainter -> getIsVertical());
    pFramePainter -> paintFrame(&Painter);
    pBarPainter -> savePaintBar(&Painter);

    Painter.end();

    pixmap.save(savePath);
}

void StructureBarPlotWidget::savePlot(FD::SaveFileInfo inInfo)
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

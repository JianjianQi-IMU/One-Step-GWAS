#include "PCA2DScatterPlotWidget.hpp"

PCA2DScatterPlotWidget::PCA2DScatterPlotWidget(QWidget *parent)
    : BaseOpenGLPaintWidget(parent)
{
    pFramePainter  = new PCA2DScatterFramePainter;
    pPointsPainter = new PCA2DScatterPointsPainter;
    pLegendPainter = new PCAScatterLegendPainter;

    xCamera       = 0.0;
    yCamera       = 0.0;
    zoomValue     = 1.0;
    maxZoomValue  = 6;
    minZoomValue  = 1;
    zoomTick      = 0.2;


    QVector<QColor> tmpColorPopList;
    tmpColorPopList.push_back("#8ECFC9");
    tmpColorPopList.push_back("#FFBE7A");
    tmpColorPopList.push_back("#FA7F6F");
    tmpColorPopList.push_back("#82B0D2");
    tmpColorPopList.push_back("#BEB8DC");
    tmpColorPopList.push_back("#E7DAD2");

    pLegendPainter -> setPaintPoint(QPointF(1,1));

    pFramePainter -> setXMarginFactor(.2);
    pFramePainter -> setYMarginFactor(.2);
    pFramePainter -> setAxisLengthFactor(.2);
    pFramePainter -> setPCLabelPosFactor(.4);

    pPointsPainter -> preSetPaintWidget(this);
    pPointsPainter -> setXMarginFactor(.2);
    pPointsPainter -> setYMarginFactor(.2);

    pPointsPainter -> setColorPopList(tmpColorPopList,false);
}

PCA2DScatterPlotWidget::PCA2DScatterPlotWidget(const double *inData, const long long *inPopIndex,char** inSampleName, long long inNSample, QWidget *parent)
    : PCA2DScatterPlotWidget(parent)
{
    loadSampleData(inData,inPopIndex,inSampleName,inNSample);
}

PCA2DScatterPlotWidget::~PCA2DScatterPlotWidget()
{
    if(pFramePainter){
        delete pFramePainter;
    }
    if(pPointsPainter){
        delete pPointsPainter;
    }
    if(pLegendPainter){
        delete pLegendPainter;
    }
    pFramePainter  = nullptr;
    pPointsPainter = nullptr;
    pLegendPainter = nullptr;
}

void PCA2DScatterPlotWidget::leftMousePressEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    int x0=e->x(), y0=e->y();
    QPoint clickedPos = QPoint(x0, y0);
    isLeftMousePressed=true;
    pPointsPainter -> setSelectedSampleIndex(-1);
    leftMousePressedPosX = x0;
    leftMousePressedPosY = y0;
    if(pLegendPainter -> isClicked(clickedPos))
    {
        leftMousePressedMode = PCA_2D_LEFT_LEGEND;
        oldLegendPoint = pLegendPainter -> getPaintPoint();
    }
    else if(pPointsPainter -> isClicked(clickedPos))
    {
        long long idPoint =
            pPointsPainter -> isClickedPoint(clickedPos);
        if(idPoint >= 0)
        {
            pPointsPainter -> setSelectedSampleIndex(idPoint);
            emit pointSelected(idPoint);
        }
    }
    isNeedUpdate = true;
}

void PCA2DScatterPlotWidget::middleMousePressEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    Q_UNUSED(isNeedUpdate);
    isMiddleMousePressed=true;
    middleMousePressedPosX=e->x();
    middleMousePressedPosY=e->y();
    setCursor(QCursor(Qt::OpenHandCursor));
}

void PCA2DScatterPlotWidget::leftMouseReleaseEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    Q_UNUSED(e);
    Q_UNUSED(isNeedUpdate);
    isLeftMousePressed=false;
    leftMousePressedMode = PCA_2D_LEFT_DEFAULT;
    leftMousePressedPosX=0;
    leftMousePressedPosY=0;
}

void PCA2DScatterPlotWidget::middleMouseReleaseEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    Q_UNUSED(e);
    Q_UNUSED(isNeedUpdate);
    isMiddleMousePressed=false;
    middleMousePressedPosX=0;
    middleMousePressedPosY=0;
    setCursor(QCursor(Qt::ArrowCursor));
}

void PCA2DScatterPlotWidget::leftMouseMoveEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    int x1=e->x(),y1=e->y();
    if(isLeftMousePressed){
        if(leftMousePressedMode == PCA_2D_LEFT_LEGEND){
            pLegendPainter -> setPaintPoint(
                        QPointF(oldLegendPoint.x()+x1-leftMousePressedPosX,
                                oldLegendPoint.y()+y1-leftMousePressedPosY));
            isNeedUpdate = true;
        }
    }
}

void PCA2DScatterPlotWidget::middleMouseMoveEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    int x1=e->x(),y1=e->y();
    int dx=0,dy=0;
    int width0=width(),height0=height();
    if(isMiddleMousePressed==true){
        dx=x1-middleMousePressedPosX;
        dy=y1-middleMousePressedPosY;
        yCamera+=2*dy/(zoomValue*height0);
        xCamera-=2*dx/(zoomValue*width0);
        if(xCamera>1) xCamera=1;
        if(xCamera<-1) xCamera=-1;
        if(yCamera>1) yCamera=1;
        if(yCamera<-1) yCamera=-1;
        setViewMat();
        middleMousePressedPosX=x1;
        middleMousePressedPosY=y1;
        isNeedUpdate = true;
    }
}

void PCA2DScatterPlotWidget::wheelEvent(QWheelEvent *e)
{
    if(e->angleDelta().y()>0) zoomValue+=zoomTick;
    else if(e->angleDelta().y()<0) zoomValue-=zoomTick;
    if(zoomValue>maxZoomValue) zoomValue=maxZoomValue;
    if(zoomValue<minZoomValue) zoomValue=minZoomValue;
    setViewMat();
    update();
}

bool PCA2DScatterPlotWidget::loadSampleData(const double *inData, const long long *inPopIndex,char** inSampleName, long long inNSample)
{
    bool flag = false;
    double tmpStart = 0, tmpStride = 0;
    double tmpMinPC, tmpMaxPC;
    flag = pPointsPainter -> loadSampleData(inData, inPopIndex, inSampleName, inNSample);
    if(flag){
        initModelMat();
        pPointsPainter -> calcuAxisData(1,tmpStart,tmpStride);
        tmpMinPC = pPointsPainter -> getMinPC1();
        tmpMaxPC = pPointsPainter -> getMaxPC1();
        pFramePainter -> setPC1Axis(tmpStart,tmpStride,tmpMinPC,tmpMaxPC);
        pPointsPainter -> calcuAxisData(2,tmpStart,tmpStride);
        tmpMinPC = pPointsPainter -> getMinPC2();
        tmpMaxPC = pPointsPainter -> getMaxPC2();
        pFramePainter -> setPC2Axis(tmpStart,tmpStride,tmpMinPC,tmpMaxPC);
    }
    return flag;
}

bool PCA2DScatterPlotWidget::setPopName(char **inPopName, long long inNPop)
{
    bool flag = false;
    flag = pPointsPainter -> loadPopName(inPopName, inNPop);
    if(flag){
        pLegendPainter -> setPopData(inPopName, inNPop,
            pPointsPainter -> getColorPopList());
    }
    return flag;
}

bool PCA2DScatterPlotWidget::setVarRatio(double inPC1VarRatio, double inPC2VarRatio)
{
    pFramePainter -> setPCVarRatio(inPC1VarRatio, inPC2VarRatio);
    return true;
}

void PCA2DScatterPlotWidget::setDisplayLegend(bool flag)
{
    pLegendPainter -> setIsPaintLegend(flag);
    update();
}

void PCA2DScatterPlotWidget::setDisplayGridLine(bool flag)
{
    pFramePainter -> setIsDisplayGrid(flag);
    update();
}

void PCA2DScatterPlotWidget::setDisplayAxisValue(bool flag)
{
    pFramePainter -> setIsDisplayAxis(flag);
    update();
}

void PCA2DScatterPlotWidget::setDisplayPCLabel(bool flag)
{
    pFramePainter -> setIsDisplayPCVar(flag);
    update();
}

void PCA2DScatterPlotWidget::setColorPopList(const QVector<QColor> &inList)
{
    if(inList.empty()) return;
    pPointsPainter -> setColorPopList(inList);
    pLegendPainter -> setColorPopList(inList);
    update();
}

void PCA2DScatterPlotWidget::setSelectedSample(long long selectedPointi)
{
    pPointsPainter -> setSelectedSampleIndex(selectedPointi);
    update();
}

void PCA2DScatterPlotWidget::setSearchedSample(long long searchedPointi)
{
    pPointsPainter -> setSearchedSampleIndex(searchedPointi);
    update();
}

const QVector<QColor>& PCA2DScatterPlotWidget::getColorPopList() const
{
    return pPointsPainter -> getColorPopList();
}

float PCA2DScatterPlotWidget::getPointSize()
{
    return pPointsPainter -> getPointSize();
}

void PCA2DScatterPlotWidget::initModelMat()
{
    double minPC1 = pPointsPainter -> getMinPC1();
    double maxPC1 = pPointsPainter -> getMaxPC1();
    double minPC2 = pPointsPainter -> getMinPC2();
    double maxPC2 = pPointsPainter -> getMaxPC2();
    float xMean=(minPC1+maxPC1)/2,yMean=(minPC2+maxPC2)/2;
    float xRange=(maxPC1-minPC1),yRange=(maxPC2-minPC2);
    QMatrix4x4 moveMat,reshapeMat;
    moveMat.setToIdentity();
    moveMat(0,3)=-xMean;
    moveMat(1,3)=-yMean;
    reshapeMat.setToIdentity();
    reshapeMat(0,0)=2/xRange;
    reshapeMat(1,1)=2/yRange;
    modelMat=reshapeMat*moveMat;
}

void PCA2DScatterPlotWidget::setViewMat()
{
    QMatrix4x4 moveMat,reshapeMat;
    moveMat.setToIdentity();
    moveMat(0,3)=-xCamera;
    moveMat(1,3)=-yCamera;
    reshapeMat.setToIdentity();
    reshapeMat(0,0)=zoomValue;
    reshapeMat(1,1)=zoomValue;
    viewMat=reshapeMat*moveMat;
}

void PCA2DScatterPlotWidget::initializeGL()
{
    initializeOpenGLFunctions();

    pPointsPainter -> initInitializeGL();
}

void PCA2DScatterPlotWidget::paintGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    setViewMat();
    QSize tmpWidgetSize = size();
    QMatrix4x4 transformMat = viewMat * modelMat;

    pFramePainter -> setWidgetSize(tmpWidgetSize);
    pFramePainter -> setTransformMat(transformMat);
    pPointsPainter -> setWidgetSize(tmpWidgetSize);
    pPointsPainter -> setModelMat(modelMat);
    pPointsPainter -> setViewMat(viewMat);
    QPainter Painter;
    Painter.begin(this);

    pFramePainter -> paintFrame(&Painter);
    pPointsPainter -> paintStart(&Painter);
    pLegendPainter -> paintLegend(&Painter);

    Painter.end();
}

void PCA2DScatterPlotWidget::resizeGL(int w, int h)
{

}

void PCA2DScatterPlotWidget::savePaintCurrentSVG(QString savePath, QSizeF inSize)
{
    if(savePath.isEmpty()) return;
    QSvgGenerator generator;
    generator.setFileName(savePath);
    generator.setSize(QSize(inSize.width(),inSize.height()));

    setViewMat();
    QSize tmpWidgetSize = QSize(inSize.width(),inSize.height());
    QMatrix4x4 transformMat = viewMat * modelMat;

    pFramePainter -> setWidgetSize(tmpWidgetSize);
    pFramePainter -> setTransformMat(transformMat);
    pPointsPainter -> setWidgetSize(tmpWidgetSize);
    pPointsPainter -> setModelMat(modelMat);
    pPointsPainter -> setViewMat(viewMat);
    QPainter Painter;

    Painter.begin(&generator);

    pFramePainter -> paintFrame(&Painter);
    pPointsPainter -> savePaintPoints(&Painter);
    pLegendPainter -> paintLegend(&Painter);

    Painter.end();
}

void PCA2DScatterPlotWidget::savePaintCurrentPNG(QString savePath, QSizeF inSize)
{
    if(savePath.isEmpty()) return;
    QPixmap pixmap(QSize(inSize.width(),inSize.height()));
    pixmap.fill(Qt::white);

    setViewMat();
    QSize tmpWidgetSize = QSize(inSize.width(),inSize.height());
    QMatrix4x4 transformMat = viewMat * modelMat;

    pFramePainter -> setWidgetSize(tmpWidgetSize);
    pFramePainter -> setTransformMat(transformMat);
    pPointsPainter -> setWidgetSize(tmpWidgetSize);
    pPointsPainter -> setModelMat(modelMat);
    pPointsPainter -> setViewMat(viewMat);

    QPainter Painter;

    Painter.begin(&pixmap);

    pFramePainter -> paintFrame(&Painter);
    pPointsPainter -> savePaintPoints(&Painter);
    pLegendPainter -> paintLegend(&Painter);

    Painter.end();

    pixmap.save(savePath);
}

void PCA2DScatterPlotWidget::savePlot(FD::SaveFileInfo inInfo)
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

void PCA2DScatterPlotWidget::setPointSize(float val)
{
    return pPointsPainter->setPointSize(val);
}

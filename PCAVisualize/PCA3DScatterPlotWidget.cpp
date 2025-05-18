#include "PCA3DScatterPlotWidget.hpp"

PCA3DScatterPlotWidget::PCA3DScatterPlotWidget(QWidget *parent)
    : BaseOpenGLPaintWidget(parent)
{
    pFramePainter  = new PCA3DScatterFramePainter;
    pPointsPainter = new PCA3DScatterPointsPainter;
    pLegendPainter = new PCAScatterLegendPainter;

    QVector<QColor> tmpColorPopList;
    tmpColorPopList.push_back("#8ECFC9");
    tmpColorPopList.push_back("#FFBE7A");
    tmpColorPopList.push_back("#FA7F6F");
    tmpColorPopList.push_back("#82B0D2");
    tmpColorPopList.push_back("#BEB8DC");
    tmpColorPopList.push_back("#E7DAD2");

    moveTick       = MML::PI/1200;
    goFrontTick    = 0.2;
    zoomTick       = 0.2;
    rMax           = 4;
    rMin           = 0.2;
    zoomMax        = 4;
    zoomMin        = 0.2;
    thetaMax       = 255*MML::PI/265;
    thetaMin       = MML::PI/265;
    zoomVal        = 1;
    samplePCValueDisplayWidth  = 200;
    samplePCValueDisplayHeight = 60;

    isMiddleMousePressed   = false;
    middleMousePressedPosX = 0;
    middleMousePressedPosY = 0;

    isLeftMousePressed     = false;
    isOrthoProjection      = true;

    r=3;
    theta=MML::PI/2;
    alpha=MML::PI/4;
    cameraPos=QVector3D(r*std::sin(theta)*std::cos(alpha),
                        r*std::cos(theta),
                        r*std::sin(theta)*std::sin(alpha));
    pFramePainter->setPaintWidget(this);
    pPointsPainter->setPaintWidget(this);
    pLegendPainter->setIsPaintLegend(true);
    pPointsPainter->setSearchedSampleIndex(-1);
    pPointsPainter->setSelectedSampleIndex(-1);
    pPointsPainter->setColorPopList(tmpColorPopList,false);
    pLegendPainter->setColorPopList(tmpColorPopList);
}

void PCA3DScatterPlotWidget::leftMousePressEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    int x0=e->x(), y0=e->y();
    QPoint clickedPos = QPoint(x0, y0);
    long long idPoint;
    isLeftMousePressed=true;
    pPointsPainter -> setSelectedSampleIndex(-1);
    leftMousePressedPosX = x0;
    leftMousePressedPosY = y0;
    if(pLegendPainter -> isClicked(clickedPos))
    {
        leftMousePressedMode = PCA_3D_LEFT_LEGEND;
        oldLegendPoint = pLegendPainter -> getPaintPoint();
    }
    else if((idPoint = pPointsPainter -> isClickedPoint(clickedPos)) >= 0)
    {
        leftMousePressedMode = PCA_3D_LEFT_MAIN;
        pPointsPainter -> setSelectedSampleIndex(idPoint);
        emit pointSelected(idPoint);
    }
    isNeedUpdate = true;
}

void PCA3DScatterPlotWidget::middleMousePressEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    Q_UNUSED(isNeedUpdate);
    isMiddleMousePressed = true;
    middleMousePressedPosX = e->x();
    middleMousePressedPosY = e->y();
    setCursor(QCursor(Qt::OpenHandCursor));
}

void PCA3DScatterPlotWidget::leftMouseReleaseEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    Q_UNUSED(isNeedUpdate);
    Q_UNUSED(e);
    isLeftMousePressed = false;
    leftMousePressedPosX = 0;
    leftMousePressedPosY = 0;
    leftMousePressedMode = PCA_3D_LEFT_DEFAULT;
}

void PCA3DScatterPlotWidget::middleMouseReleaseEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    Q_UNUSED(isNeedUpdate);
    Q_UNUSED(e);
    isMiddleMousePressed = false;
    middleMousePressedPosX = 0;
    middleMousePressedPosY = 0;
    setCursor(QCursor(Qt::ArrowCursor));
}

void PCA3DScatterPlotWidget::leftMouseMoveEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    int x1=e->x(),y1=e->y();
    if(isLeftMousePressed){
        if(leftMousePressedMode == PCA_3D_LEFT_LEGEND){
            pLegendPainter -> setPaintPoint(
                        QPointF(oldLegendPoint.x()+x1-leftMousePressedPosX,
                                oldLegendPoint.y()+y1-leftMousePressedPosY));
            isNeedUpdate = true;
        }
    }
}

void PCA3DScatterPlotWidget::middleMouseMoveEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    int x1=e->x(),y1=e->y();
    int dx=0,dy=0;
    if(isMiddleMousePressed==true){
        dx=x1-middleMousePressedPosX;
        dy=y1-middleMousePressedPosY;
        alpha+=dx*moveTick;
        theta-=dy*moveTick;
        if(theta>thetaMax) theta=thetaMax;
        if(theta<thetaMin) theta=thetaMin;
        while(alpha<0) alpha+=2*MML::PI;
        while(alpha>2*MML::PI) alpha-=2*MML::PI;
        cameraPos=QVector3D(r*std::sin(theta)*std::cos(alpha),
                            r*std::cos(theta),
                            r*std::sin(theta)*std::sin(alpha));
        middleMousePressedPosX=x1;
        middleMousePressedPosY=y1;
        isNeedUpdate = true;
    }
}

void PCA3DScatterPlotWidget::wheelEvent(QWheelEvent *e)
{
    if(isOrthoProjection){
        if(e->angleDelta().y()>0) zoomVal-=zoomTick;
        else if(e->angleDelta().y()<0) zoomVal+=zoomTick;
        if(zoomVal>zoomMax) zoomVal=zoomMax;
        if(zoomVal<zoomMin) zoomVal=zoomMin;
        projectionMat.setToIdentity();
        projectionMat.ortho(-1*zoomVal,1*zoomVal,-1*zoomVal,1*zoomVal,-999,999);
    }
    else{
        if(e->angleDelta().y()>0) r-=goFrontTick;
        else if(e->angleDelta().y()<0) r+=goFrontTick;
        if(r>rMax) r=rMax;
        if(r<rMin) r=rMin;
        cameraPos=QVector3D(r*std::sin(theta)*std::cos(alpha),
                            r*std::cos(theta),
                            r*std::sin(theta)*std::sin(alpha));
        setViewMat();
    }
    update();
}

PCA3DScatterPlotWidget::PCA3DScatterPlotWidget(const double *inData, const long long *inPopIndex,char** inSampleName, long long inNSample, QWidget *parent)
    :PCA3DScatterPlotWidget(parent)
{
    bool flag = false;

    flag = setSampleData(inData,inPopIndex,inSampleName,inNSample);
    double tStart1, tStride1;
    double tStart2, tStride2;
    double tStart3, tStride3;
    if(flag){
        pPointsPainter -> calcuAxisData(1, tStart1, tStride1);
        pPointsPainter -> calcuAxisData(2, tStart2, tStride2);
        pPointsPainter -> calcuAxisData(3, tStart3, tStride3);
        pFramePainter -> setPCRange(pPointsPainter -> getMinPC1(),
                                    pPointsPainter -> getMaxPC1(),
                                    pPointsPainter -> getMinPC2(),
                                    pPointsPainter -> getMaxPC2(),
                                    pPointsPainter -> getMinPC3(),
                                    pPointsPainter -> getMaxPC3());
        pFramePainter -> setAxisValuesData(tStart1, tStride1,
                                           tStart2, tStride2,
                                           tStart3, tStride3);
        pFramePainter->setAxisData();
        pFramePainter->setGridLinesData();
        initModelMat();
        initProjectionMat();
        setViewMat();
    }
}

PCA3DScatterPlotWidget::~PCA3DScatterPlotWidget()
{
    if(pFramePainter){
        delete pFramePainter;
        pFramePainter = nullptr;
    }
    if(pPointsPainter){
        delete pPointsPainter;
        pPointsPainter = nullptr;
    }
    if(pLegendPainter){
        delete pLegendPainter;
        pLegendPainter = nullptr;
    }
}

bool PCA3DScatterPlotWidget::setSampleData(const double *inData, const long long* inPopIndex,char** inSampleName, long long inNSample)
{
    return pPointsPainter -> setIndividualsData(inData, inPopIndex, inSampleName, inNSample);
}

bool PCA3DScatterPlotWidget::setPopName(char **inPopName, long long inNPop)
{
    bool flag = false;
    flag = pPointsPainter -> setPopName(inPopName, inNPop);
    if(flag){
        pLegendPainter -> setPopData(inPopName, inNPop,
            pPointsPainter -> getColorPopList());
    }
    return flag;
}

bool PCA3DScatterPlotWidget::setVarRatio(double inPC1VarRatio, double inPC2VarRatio, double inPC3VarRatio)
{
    return pFramePainter -> setVarRatio(inPC1VarRatio, inPC2VarRatio, inPC3VarRatio);
}

void PCA3DScatterPlotWidget::setDisplayLegend(bool flag)
{
    pLegendPainter -> setIsPaintLegend(flag);
    update();
}

void PCA3DScatterPlotWidget::setDisplayGridLine(bool flag)
{
    pFramePainter -> setIsDisplayGridLine(flag);
    update();
}

void PCA3DScatterPlotWidget::setDisplayAxisValue(bool flag)
{
    pFramePainter -> setIsDisplayAxisValue(flag);
    update();
}

void PCA3DScatterPlotWidget::setDisplayAxisFrame(bool flag)
{
    pFramePainter -> setIsDisplayAxisFrame(flag);
    update();
}

void PCA3DScatterPlotWidget::setDisplayPCLabel(bool flag)
{
    pFramePainter -> setIsDisplayPCVar(flag);
    update();
}

void PCA3DScatterPlotWidget::setColorPopList(const QVector<QColor> &inList)
{
    if(inList.empty()) return;
    pPointsPainter -> setColorPopList(inList);
    pLegendPainter -> setColorPopList(inList);
    update();
}

void PCA3DScatterPlotWidget::setSearchedSample(long long searchedPointi)
{
    pPointsPainter -> setSearchedSampleIndex(searchedPointi);
    update();
}

void PCA3DScatterPlotWidget::setSelectedSample(long long selectedPointi)
{
    pPointsPainter -> setSelectedSampleIndex(selectedPointi);
    update();
}

void PCA3DScatterPlotWidget::setOrthoProjection(bool flag)
{
//    r=3;
//    theta=MML::PI/2;
//    alpha=MML::PI/4;
    projectionMat.setToIdentity();
    if(flag){
        projectionMat.ortho(-1,1,-1,1,-999,999);
    }
    else{
        projectionMat.perspective(45,(float)width()/height(),0.1,100);
    }
    isOrthoProjection=flag;
    update();
}

void PCA3DScatterPlotWidget::switchProjection()
{
    setOrthoProjection(!isOrthoProjection);
}

float PCA3DScatterPlotWidget::getPointSize()
{
    return pPointsPainter->getPointSize();
}

const QVector<QColor> &PCA3DScatterPlotWidget::getColorPopList() const
{
    return pPointsPainter->getColorPopList();
}

void PCA3DScatterPlotWidget::initModelMat()
{
    double maxPC1, minPC1;
    double maxPC2, minPC2;
    double maxPC3, minPC3;
    maxPC1 = pPointsPainter -> getMaxPC1(), minPC1 = pPointsPainter -> getMinPC1();
    maxPC2 = pPointsPainter -> getMaxPC2(), minPC2 = pPointsPainter -> getMinPC2();
    maxPC3 = pPointsPainter -> getMaxPC3(), minPC3 = pPointsPainter -> getMinPC3();
    double meanPC1=(maxPC1+minPC1)/2;
    double meanPC2=(maxPC2+minPC2)/2;
    double meanPC3=(maxPC3+minPC3)/2;
    double rangePC1=maxPC1-minPC1;
    double rangePC2=maxPC2-minPC2;
    double rangePC3=maxPC3-minPC3;
    QMatrix4x4 moveMat(
                1.0,0,0,-meanPC1,
                0,1.0,0,-meanPC2,
                0,0,1.0,-meanPC3,
                0,0,0,1.0
                );
    QMatrix4x4 reshapeMat(
                1.0/rangePC1,0,0,0,
                0,1.0/rangePC2,0,0,
                0,0,1.0/rangePC3,0,
                0,0,0,1.0
                );
    modelMat=reshapeMat*moveMat;
}

void PCA3DScatterPlotWidget::initProjectionMat()
{
    projectionMat.ortho(-1,1,-1,1,-999,999);
//    projectionMat.perspective(45,(float)width()/height(),0.1,100);
}

void PCA3DScatterPlotWidget::setViewMat()
{
    viewMat.setToIdentity();
    viewMat.lookAt(cameraPos,QVector3D(0,0,0),QVector3D(0,1,0));
}

void PCA3DScatterPlotWidget::initializeGL()
{
    initializeOpenGLFunctions();

    pPointsPainter -> initInitializeGL();
    pFramePainter -> initInitializeGL();
}

void PCA3DScatterPlotWidget::paintGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    setViewMat();

    pFramePainter -> setTransformMat(modelMat, projectionMat, viewMat);
    pFramePainter -> setPaintSize(size());
    pPointsPainter -> setTransformMat(modelMat, projectionMat, viewMat);
    pPointsPainter -> setPaintSize(size());

    QPainter Painter;

    Painter.begin(this);

    pFramePainter -> paintStart(&Painter, alpha);
    pPointsPainter -> paintStart(&Painter);
    pLegendPainter -> paintLegend(&Painter);

//    Painter.setBrush(Qt::black);
//    Painter.drawRect(QRect(1,1,30,30));

    Painter.end();

}

void PCA3DScatterPlotWidget::resizeGL(int w, int h)
{

}

void PCA3DScatterPlotWidget::savePaintCurrentSVG(QString savePath, QSizeF inSize)
{
    if(savePath.isEmpty()) return;
    QSvgGenerator generator;
    generator.setFileName(savePath);
    generator.setSize(QSize(inSize.width(),inSize.height()));

    setViewMat();
    QSize tmpWidgetSize = QSize(inSize.width(),inSize.height());

    pFramePainter -> setTransformMat(modelMat, projectionMat, viewMat);
    pFramePainter -> setPaintSize(tmpWidgetSize);
    pPointsPainter -> setTransformMat(modelMat, projectionMat, viewMat);
    pPointsPainter -> setPaintSize(tmpWidgetSize);
    QPainter Painter;

    Painter.begin(&generator);

    pFramePainter -> savePaintFrame(&Painter, alpha);
    pPointsPainter -> savePaintPoints(&Painter);
    pLegendPainter -> paintLegend(&Painter);

    Painter.end();
}

void PCA3DScatterPlotWidget::savePaintCurrentPNG(QString savePath, QSizeF inSize)
{
    if(savePath.isEmpty()) return;
    QPixmap pixmap(QSize(inSize.width(),inSize.height()));
    pixmap.fill(Qt::white);

    setViewMat();
    QSize tmpWidgetSize = QSize(inSize.width(),inSize.height());

    pFramePainter -> setTransformMat(modelMat, projectionMat, viewMat);
    pFramePainter -> setPaintSize(tmpWidgetSize);
    pPointsPainter -> setTransformMat(modelMat, projectionMat, viewMat);
    pPointsPainter -> setPaintSize(tmpWidgetSize);

    QPainter Painter;

    Painter.begin(&pixmap);

    pFramePainter -> savePaintFrame(&Painter, alpha);
    pPointsPainter -> savePaintPoints(&Painter);
    pLegendPainter -> paintLegend(&Painter);

    Painter.end();

    pixmap.save(savePath);
}

void PCA3DScatterPlotWidget::savePlot(FD::SaveFileInfo inInfo)
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

void PCA3DScatterPlotWidget::setPointSize(float val)
{
    pPointsPainter -> setPointSize(val);
    update();
}

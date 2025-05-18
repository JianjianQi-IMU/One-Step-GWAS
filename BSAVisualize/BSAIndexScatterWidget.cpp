#include "BSAIndexScatterWidget.hpp"

BSAPointID::BSAPointID()
    :BSAPointID(0,-1)
{

}

BSAPointID::BSAPointID(size_t inID, short inInfo)
{
    id=inID;
    info=inInfo;
}

bool BSAPointID::operator==(const BSAPointID &B) const
{
    return id==B.id&&info==B.info;
}

int BSAIndexScatterWidget::chrNameMaxLen = 256;

BSAIndexScatterWidget::BSAIndexScatterWidget(QWidget *parent)
    :QOpenGLWidget(parent)
{
    init();
    makeMenu();
}

BSAIndexScatterWidget::BSAIndexScatterWidget(size_t *inChrLen, char **inChrName, int inNChr, FD::BSAInterVal *inPoints, size_t inNPoints,ValPoints2* inMarkers,size_t inNMarkers, QWidget *parent)
    :BSAIndexScatterWidget(parent)
{
    loadChromosome(inChrLen,inChrName,inNChr);
    loadPoints(inPoints,inNPoints);
    loadMarkers(inMarkers,inNMarkers);
}

BSAIndexScatterWidget::~BSAIndexScatterWidget()
{
    if(chrLen) delete [] chrLen;
    if(cumChrLen) delete [] cumChrLen;
    if(chrName){
        for(int i=0;i<nChr;++i) delete [] chrName[i];
        delete [] chrName;
    }
    if(points) delete [] points;
    if(markers) delete [] markers;
    if(cumLPointsPos) delete [] cumLPointsPos;
    if(cumRPointsPos) delete [] cumRPointsPos;
    if(cumMarkersPos) delete [] cumMarkersPos;
    if(paintHPoints) delete [] paintHPoints;
    if(paintLPoints) delete [] paintLPoints;
    if(paintDPoints) delete [] paintDPoints;
    if(paintT1Points) delete [] paintT1Points;
    if(paintT2Points) delete [] paintT2Points;
    if(paintHMarkers) delete [] paintHMarkers;
    if(paintLMarkers) delete [] paintLMarkers;
    if(paintDMarkers) delete [] paintDMarkers;
    if(genome) delete genome;
    if(genomeSeq) delete genomeSeq;
    if(isValid()){
        glDeleteBuffers(1,&idPointsVBOH);
        glDeleteBuffers(1,&idPointsVBOL);
        glDeleteBuffers(1,&idPointsVBOD);
        glDeleteBuffers(1,&idPointsVBOT1);
        glDeleteBuffers(1,&idPointsVBOT2);
        glDeleteBuffers(1,&idMarkersVBOH);
        glDeleteBuffers(1,&idMarkersVBOL);
        glDeleteBuffers(1,&idMarkersVBOD);
        glDeleteVertexArrays(1,&idPointsVAOH);
        glDeleteVertexArrays(1,&idPointsVAOL);
        glDeleteVertexArrays(1,&idPointsVAOD);
        glDeleteVertexArrays(1,&idPointsVAOT1);
        glDeleteVertexArrays(1,&idPointsVAOT2);
        glDeleteVertexArrays(1,&idMarkersVAOH);
        glDeleteVertexArrays(1,&idMarkersVAOL);
        glDeleteVertexArrays(1,&idMarkersVAOD);
    }
}

void BSAIndexScatterWidget::mousePressEvent(QMouseEvent *e)
{
    int x1=e->x(),y1=e->y();
    int height0=height();
    if(e->button()==Qt::LeftButton){
        if(y1<=widgetTopMargin){
            isLeftMousePressed=true;
            isMiddleMousePressed=false;
            leftMousePressedPosX=x1;
            setCursor(QCursor(Qt::CrossCursor));
            update();
        }
        else if(x1>=widgetLeftMargin){
            if(y1<=widgetTopMargin+scatterAreaHeightRatio*(height0-widgetTopMargin)){
                dealLeftMousePressPoint(x1,y1);
                update();
            }
            else{
                dealLeftMousePressGene(x1,y1);
                update();
            }
        }
    }
    else if(e->button()==Qt::MiddleButton){
        isLeftMousePressed=false;
        isMiddleMousePressed=true;
        middleMousePressedPosX=x1;
        middleMouseMovingPosX=x1;
        middleMousePressedLeftPosF=currentLeftPosF;
        middleMousePressedRightPosF=currentRightPosF;
        setCursor(QCursor(Qt::OpenHandCursor));
    }
    else if(e->button()==Qt::RightButton){
        isRightMousePressed = true;
        RightMousePressedPosX = x1;
        RightMousePressedPosY = y1;
        idRightPressPoint=isPointPos(x1,y1);
        if(idRightPressPoint.info>=0){
            actAddLabelPoint->setEnabled(true);
            actremoveLabelPoint->setEnabled(true);
        }
        else{
            actAddLabelPoint->setDisabled(true);
            actremoveLabelPoint->setDisabled(true);
        }
    }
    QOpenGLWidget::mousePressEvent(e);
}

void BSAIndexScatterWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(isLeftMousePressed){
        isLeftMousePressed=false;
        leftMouseReleasedPosX=e->x();

        if(mouseXToPointX(leftMousePressedPosX,leftMouseReleasedPosX)){
            adjustPaintChri();
            paintPointsToBuffer();
            paintMarkersToBuffer();
            adjustPaintGenesi();
            emit currentIntervalChanged();
        }
        leftMousePressedPosX=-1;
        leftMouseMovingPosX=-1;
        setCursor(QCursor(Qt::ArrowCursor));
        update();
    }
    if(isMiddleMousePressed){
        isMiddleMousePressed=false;
        adjustPaintChri();
        paintPointsToBuffer();
        paintMarkersToBuffer();
        adjustPaintGenesi();
        middleMousePressedPosX=-1;
        setCursor(QCursor(Qt::ArrowCursor));
        update();
        emit currentIntervalChanged();
    }
    if(isRightMousePressed){
        isRightMousePressed = false;
    }
    QOpenGLWidget::mouseReleaseEvent(e);
}

void BSAIndexScatterWidget::mouseMoveEvent(QMouseEvent *e)
{
    int scatterWidth=width()-widgetLeftMargin;
    int posXGapLen,x0=e->x();
    long double chrPos;
    long double chrPosGapLenF,moveChrLen;

    if(isLeftMousePressed){
        leftMouseMovingPosX=e->x();
        update();
    }
    if(isMiddleMousePressed){
        x0=e->x();
        chrPosGapLenF=currentRightPosF-currentLeftPosF;
        if(x0>middleMousePressedPosX){
            posXGapLen=x0-middleMouseMovingPosX;
            chrPos=std::max((long double)1,(currentLeftPosF-(long double)(posXGapLen)/scatterWidth*chrPosGapLenF));
            moveChrLen=currentLeftPosF-chrPos;
            currentLeftPosF-=moveChrLen;
            currentRightPosF-=moveChrLen;
        }
        else{
            posXGapLen=middleMouseMovingPosX-x0;
            chrPos=std::min((long double)maxCumPos,(((long double)(posXGapLen)/scatterWidth*chrPosGapLenF)+currentRightPosF));
            moveChrLen=chrPos-currentRightPosF;
            currentLeftPosF+=moveChrLen;
            currentRightPosF+=moveChrLen;

        }
        adjustPaintChri();
        paintPointsToBuffer();
        paintMarkersToBuffer();
        adjustPaintGenesi();
        middleMouseMovingPosX=x0;
        update();
        emit currentIntervalChanged();
    }
    QOpenGLWidget::mouseMoveEvent(e);
}

void BSAIndexScatterWidget::wheelEvent(QWheelEvent *e)
{
    if(e->angleDelta().y()>0) zoomIn();
    else if(e->angleDelta().y()<0) zoomOut();
}

bool BSAIndexScatterWidget::isSelectedPoint(long long pointi, int x, int y, short info)
{
    int py=0,dx=0,dy=0;
    long double meanPos=(points[pointi].start+points[pointi].stop)/2.0;

    py=calcuChartY(BSAPointID(pointi,info));
    dy=py-y;
    dx=calcuChartX(points[pointi].idChr,meanPos)-x;
    if(dx*dx+dy*dy<=selectedPointsDistance2) return true;
    return false;
}

bool BSAIndexScatterWidget::isSelectedGene(long long genei, int x, int y)
{
    float painterAxleFactor=0.75;
    int height0=height(),width0=width();
    int painty0=height0-painterAxleFactor*(height0-widgetTopMargin)*(1-scatterAreaHeightRatio)-0.5*geneModelHeight;
    int painty1=painty0+geneModelHeight-1;
    long double positionGapLenF=currentRightPosF-currentLeftPosF;
    int paintAreaWidth=width0-widgetLeftMargin;
    GeneAnnotation *pGene=(genome->getGene(genei));
    long long tGeneStart=std::max((long double)(pGene->start+cumChrLen[pGene->idChr]),currentLeftPosF);
    long long tGeneStop=std::min((long double)(pGene->stop+cumChrLen[pGene->idChr]),currentRightPosF);
    int paintx0=double(tGeneStart-currentLeftPosF)/positionGapLenF*paintAreaWidth+widgetLeftMargin;
    int paintl=std::max(int(double(tGeneStop-tGeneStart+1)/positionGapLenF*paintAreaWidth),1);
    int paintx1=paintx0+paintl-1;
    if(x>=paintx0&&x<=paintx1&&y>=painty0&&y<=painty1) return true;
    return false;
}

bool BSAIndexScatterWidget::loadPoints(FD::BSAInterVal *inPoints, long long inNPoints)
{
    if(!inPoints||!inNPoints) return false;
    long long i;
    if(nPoints!=inNPoints){
        if(points) delete [] points;
        if(cumLPointsPos) delete [] cumLPointsPos;
        if(cumRPointsPos) delete [] cumRPointsPos;
        if(paintHPoints) delete [] paintHPoints;
        if(paintLPoints) delete [] paintLPoints;
        if(paintDPoints) delete [] paintDPoints;
        if(paintT1Points) delete [] paintT1Points;
        if(paintT2Points) delete [] paintT2Points;
        points=new FD::BSAInterVal[inNPoints+1];
        cumLPointsPos=new long long[inNPoints+1];
        cumRPointsPos=new long long[inNPoints+1];
        paintHPoints=new GLfloat[inNPoints*2];
        paintLPoints=new GLfloat[inNPoints*2];
        paintDPoints=new GLfloat[inNPoints*2];
        paintT1Points=new GLfloat[inNPoints*2];
        paintT2Points=new GLfloat[inNPoints*2];
    }
    nPoints=inNPoints;
    for(i=0;i<nPoints;++i){
        points[i]=inPoints[i];
    }
    std::sort(points,&points[nPoints]);
    for(i=0;i<nPoints;++i){
        cumLPointsPos[i]=points[i].start+cumChrLen[points[i].idChr];
        cumRPointsPos[i]=points[i].stop+cumChrLen[points[i].idChr];
    }
    currentLeftPosF=1;
    currentRightPosF=maxCumPos;
    currentMinPointsi=0;
    currentMaxPointsi=nPoints-1;
    return true;
}

bool BSAIndexScatterWidget::loadChromosome(size_t *inChrLen, char **inChrName, int inNChr)
{
    if(!inChrLen||!inChrName||!inNChr) return false;
    int i,j;
    if(nChr!=inNChr){
        if(chrLen) delete [] chrLen;
        if(cumChrLen) delete [] cumChrLen;
        if(chrName){
            for(i=0;i<nChr;++i) delete [] chrName[i];
            delete [] chrName;
        }
        chrLen=new long long[inNChr];
        chrName=new char*[inNChr];
        cumChrLen=new long long[inNChr+1];
    }
    nChr=inNChr;
    for(i=0;i<nChr;++i){
        chrLen[i]=inChrLen[i];
        chrName[i]=new char[chrNameMaxLen];
        for(j=0;j<chrNameMaxLen-1&&inChrName[i][j]!='\0';++j){
            chrName[i][j]=inChrName[i][j];
        }
        chrName[i][j]='\0';
        if(i==0) cumChrLen[i]=0;
        else cumChrLen[i]=cumChrLen[i-1]+chrLen[i-1];
    }
    maxCumPos=cumChrLen[nChr-1]+chrLen[nChr-1];
    cumChrLen[nChr]=maxCumPos;
    currentMaxChri=0;
    currentMinChri=nChr-1;
    return true;
}

bool BSAIndexScatterWidget::loadGenomeSeq(const char *inFile)
{
    if(genomeSeq) genomeSeq->clear();
    else genomeSeq = new GenomeSequence;
    if(FD::genomeSeqRead(inFile,chrName,nChr,chrLen,*genomeSeq)){
        update();
        isDisplayGenomeSeq = true;
        return true;
    }
    return false;
}

bool BSAIndexScatterWidget::loadMarkers(ValPoints2 *inMarkers, long long inNMarkers)
{
    if(!inMarkers||!inNMarkers) return false;
    if(markers) delete [] markers;
    if(cumMarkersPos) delete [] cumMarkersPos;
    if(paintHMarkers) delete [] paintHMarkers;
    if(paintLMarkers) delete [] paintLMarkers;
    if(paintDMarkers) delete [] paintDMarkers;
    long long i=0;
    nMarkers = inNMarkers;
    markers = new ValPoints2[nMarkers+1];
    cumMarkersPos = new long long[nMarkers+1];
    paintHMarkers = new GLfloat[2*nMarkers];
    paintLMarkers = new GLfloat[2*nMarkers];
    paintDMarkers = new GLfloat[2*nMarkers];
    for(i=0;i<nMarkers;++i){
        markers[i]=inMarkers[i];
    }

    std::sort(markers,&(markers[nMarkers]));
    for(i=0;i<nMarkers;++i){
        cumMarkersPos[i]=markers[i].pos+cumChrLen[markers[i].idchr];
    }
    currentMinMarkersi=0;
    currentMaxMarkersi=nMarkers-1;
    return true;
}

bool BSAIndexScatterWidget::createGenome()
{
    if(!genome){
        genome=new GenomeAnnotation;
        geneDisplayMode=1;
        return true;
    }
    return false;
}

bool BSAIndexScatterWidget::createGenome(char **inChrName, int inNChr, long long inNGenes)
{
    if(!genome) genome=new GenomeAnnotation(inChrName,inNChr,inNGenes);
    else{
        genome->clear();
        if(!(genome->loadChromosome(inChrName,inNChr))) return false;
        if(!(genome->initGenes(inNGenes))) return false;
    }
    geneDisplayMode=1;
    return true;
}

bool BSAIndexScatterWidget::setGenome(const GenomeAnnotation &inGenome)
{
    if(!genome) return false;
    *genome=inGenome;
    if(genome->getValid()){
        adjustPaintChri();
        adjustPaintGenesi();
    }
    return true;
}

bool BSAIndexScatterWidget::mouseXToPointX(int x1, int x2)
{
    int width0=width();
    long double pos1,pos2;
    long double positionGapLen=currentRightPosF-currentLeftPosF;
    x1=x1<widgetLeftMargin?widgetLeftMargin:x1;
    x2=x2<widgetLeftMargin?widgetLeftMargin:x2;
    if(x1==x2) return false;
    if(x1>x2){
        pos1=double(x1-widgetLeftMargin)/(width0-widgetLeftMargin)*positionGapLen+currentLeftPosF;
        pos2=double(x2-widgetLeftMargin)/(width0-widgetLeftMargin)*positionGapLen+currentLeftPosF;
        if(pos1-pos2+1<minPosGapLen) return false;
        currentLeftPosF=pos2;
        currentRightPosF=pos1;
    }
    else{
        pos1=double(x1-widgetLeftMargin)/(width0-widgetLeftMargin)*positionGapLen+currentLeftPosF;
        pos2=double(x2-widgetLeftMargin)/(width0-widgetLeftMargin)*positionGapLen+currentLeftPosF;
        if(pos2-pos1+1<minPosGapLen) return false;
        currentLeftPosF=pos1;
        currentRightPosF=pos2;
    }
    if(currentLeftPosF<1) currentLeftPosF=1;
    if(currentRightPosF>maxCumPos) currentRightPosF=maxCumPos;
    return true;
}

BSAPointID BSAIndexScatterWidget::isPointPos(int x, int y)
{
    if(nPaintPoints){
        long long i=currentMinPointsi;
        short iInfo=0;
        for(iInfo=0;iInfo<5;++iInfo){
            for(;i<=currentMaxPointsi;++i){
                if(isSelectedPoint(i,x,y,iInfo)) break;
            }
            if(i<=currentMaxPointsi) return BSAPointID(i,iInfo);
        }
    }
    return BSAPointID(0,-1);
}

double BSAIndexScatterWidget::calcuChartX(long long idChr,long double pos)
{
    long double positionGapLenF=currentRightPosF-currentLeftPosF;
    return (long double)(pos+cumChrLen[idChr]-currentLeftPosF)*(width()-widgetLeftMargin)/positionGapLenF+widgetLeftMargin;
}

double BSAIndexScatterWidget::calcuChartY(double val, short info)
{
    if(info<0) return -1;
    double py=-1;
    int height0=height();
    double indexChartYGap=indexChartMaximun-indexChartMinimun;
    double deltaChartYGap=deltaChartMaximun-deltaChartMinimun;
    switch(info){
    case 0:
        py=scatterAreaHeightRatio*subScatterAreaHeightRatio*(height0-widgetTopMargin)*((indexChartMaximun-val)/indexChartYGap)
                +widgetTopMargin;
        break;
    case 1:
        py=scatterAreaHeightRatio*subScatterAreaHeightRatio*(height0-widgetTopMargin)*((indexChartMaximun-val)/indexChartYGap)
                +widgetTopMargin;
        break;
    case 2:
        py=scatterAreaHeightRatio*subScatterAreaHeightRatio*(height0-widgetTopMargin)*((deltaChartMaximun-val)/deltaChartYGap)
                +widgetTopMargin+scatterAreaHeightRatio*(1-subScatterAreaHeightRatio)*(height0-widgetTopMargin);
        break;
    case 3:
        py=scatterAreaHeightRatio*subScatterAreaHeightRatio*(height0-widgetTopMargin)*((deltaChartMaximun-val)/deltaChartYGap)
                +widgetTopMargin+scatterAreaHeightRatio*(1-subScatterAreaHeightRatio)*(height0-widgetTopMargin);
        break;
    case 4:
        py=scatterAreaHeightRatio*subScatterAreaHeightRatio*(height0-widgetTopMargin)*((deltaChartMaximun+val)/deltaChartYGap)
                +widgetTopMargin+scatterAreaHeightRatio*(1-subScatterAreaHeightRatio)*(height0-widgetTopMargin);
        break;
    default: break;
    }
    return py;
}

double BSAIndexScatterWidget::calcuChartY(const BSAPointID &p)
{
    if(p.info<0) return -1;
    double py=-1;
    int height0=height();
    double indexChartYGap=indexChartMaximun-indexChartMinimun;
    double deltaChartYGap=deltaChartMaximun-deltaChartMinimun;
    switch(p.info){
    case 0:
        py=scatterAreaHeightRatio*subScatterAreaHeightRatio*(height0-widgetTopMargin)*((indexChartMaximun-points[p.id].indexH)/indexChartYGap)
                +widgetTopMargin;
        break;
    case 1:
        py=scatterAreaHeightRatio*subScatterAreaHeightRatio*(height0-widgetTopMargin)*((indexChartMaximun-points[p.id].indexL)/indexChartYGap)
                +widgetTopMargin;
        break;
    case 2:
        py=scatterAreaHeightRatio*subScatterAreaHeightRatio*(height0-widgetTopMargin)*((deltaChartMaximun-points[p.id].val)/deltaChartYGap)
                +widgetTopMargin+scatterAreaHeightRatio*(1-subScatterAreaHeightRatio)*(height0-widgetTopMargin);
        break;
    case 3:
        py=scatterAreaHeightRatio*subScatterAreaHeightRatio*(height0-widgetTopMargin)*((deltaChartMaximun-points[p.id].thre)/deltaChartYGap)
                +widgetTopMargin+scatterAreaHeightRatio*(1-subScatterAreaHeightRatio)*(height0-widgetTopMargin);
        break;
    case 4:
        py=scatterAreaHeightRatio*subScatterAreaHeightRatio*(height0-widgetTopMargin)*((deltaChartMaximun+points[p.id].thre)/deltaChartYGap)
                +widgetTopMargin+scatterAreaHeightRatio*(1-subScatterAreaHeightRatio)*(height0-widgetTopMargin);
        break;
    default: break;
    }
    return py;
}

void BSAIndexScatterWidget::dealLeftMousePressPoint(int x, int y)
{
    if(nPaintPoints){
        BSAPointID ip=isPointPos(x,y);
        if(ip.info>=0){
            selectedPointi=ip;
            emit pointSelected(ip);
        }
    }
}

void BSAIndexScatterWidget::dealLeftMousePressGene(int x, int y)
{
    if(nPaintGenes){
        long long i=currentMinGenei;
        for(;i<=currentMaxGenei;++i){
            if(isSelectedGene(i,x,y)) break;
        }
        if(i<=currentMaxGenei){
            selectedGenei=i;
            emit geneSelected(selectedGenei);
        }
    }
}

void BSAIndexScatterWidget::dealAddPointLabel()
{
    bool havePoint = false;
    BSAPointID ip=idRightPressPoint;
    if(idLabelPoints.size()>=maxLabelPointsNum) return ;
    if(nPaintPoints){
        if(ip.info>=0){
            for(int j=0;j<idLabelPoints.size();++j){
                if(idLabelPoints[j]==ip){
                    havePoint = true;
                    break;
                }
            }
            if(havePoint) return ;
            idLabelPoints.append(ip);
            emit addLabelPoint(ip);
        }
    }
}

void BSAIndexScatterWidget::dealRemovePointLabel()
{
    bool havePoint = false;
    int j;
    BSAPointID ip=idRightPressPoint;
    if(nPaintPoints){
        if(ip.info>=0){
            for(j=0;j<idLabelPoints.size();++j){
                if(idLabelPoints[j]==ip){
                    havePoint = true;
                    break;
                }
            }
            if(!havePoint) return ;
            idLabelPoints.removeAt(j);
            emit removeLabelPoint(j);
        }
    }
}

void BSAIndexScatterWidget::paintAxisInPainter(QPainter *painter)
{
    int width0=width(),height0=height();
    int areaLen=(height0-widgetTopMargin)*scatterAreaHeightRatio;
    int axisLen=(height0-widgetTopMargin)*scatterAreaHeightRatio*subScatterAreaHeightRatio;
    int indexAxisy0=widgetTopMargin;
    int deltaAxisy0=widgetTopMargin+(height0-widgetTopMargin)*scatterAreaHeightRatio*(1-subScatterAreaHeightRatio);

    double indexChartYGap=indexChartMaximun-indexChartMinimun;
    double deltaChartYGap=deltaChartMaximun-deltaChartMinimun;
    painter->save();

    int painty=0;
    painter->setPen(Qt::black);
    painter->drawLine(widgetLeftMargin,widgetTopMargin,width0,widgetTopMargin);
    painter->drawLine(widgetLeftMargin,areaLen+widgetTopMargin,width0,areaLen+widgetTopMargin);
    painter->drawLine(widgetLeftMargin,widgetTopMargin,widgetLeftMargin,areaLen+widgetTopMargin);
    for(int i=0;i<6;++i){
        painty=indexAxisy0+axisLen*(indexChartMaximun-0.2*i)/indexChartYGap;
        painter->drawLine(
                        widgetLeftMargin,
                        painty,
                        widgetLeftMargin-scatterScaleLen,
                        painty);
        painter->drawText(
                        QRectF(1,painty-20,widgetLeftMargin-scatterScaleLen-5,40),
                        Qt::AlignRight|Qt::AlignVCenter,
                        QString::number(0.2*i,'g',2)
                    );
    }

    for(int i=0;i<9;++i){
        painty=deltaAxisy0+axisLen*(deltaChartMaximun-(0.25*i-1))/deltaChartYGap;
        painter->drawLine(
                        widgetLeftMargin,
                        painty,
                        widgetLeftMargin-scatterScaleLen,
                        painty);
        painter->drawText(
                        QRectF(1,painty-20,widgetLeftMargin-scatterScaleLen-5,40),
                        Qt::AlignRight|Qt::AlignVCenter,
                        QString::number(0.25*i-1,'g',2)
                    );
    }

    painter->restore();
}

void BSAIndexScatterWidget::paintVerticalLineInPainter(QPainter *painter)
{
    int width0=width(),height0=height();
    int posLabely=widgetTopMargin-positionLabelHeight;
    long long positionGapLen=currentRightPosF-currentLeftPosF;
    long long scatterVLineGap1=1,scatterVLineGap2=1;
    long double positionGapLenF=currentRightPosF-currentLeftPosF;
    long long i,j,k;
    long double tVLineX;
    while(positionGapLen) (positionGapLen/=10),scatterVLineGap1*=10;
    scatterVLineGap1=std::max(scatterVLineGap1,(long long)1000);
    scatterVLineGap1/=10;
    positionGapLen=currentRightPosF-currentLeftPosF;
    scatterVLineGap2=scatterVLineGap1/10;

    painter->save();

    painter->fillRect(QRectF(widgetLeftMargin,posLabely,width0-widgetLeftMargin,positionLabelHeight),QBrush(QColor("#bbbbbb")));

    painter->setPen("#eeeeee");

    if(isDisplayMinorGridline){
        i=((long long)currentLeftPosF-cumChrLen[currentMinChri])-((long long)currentLeftPosF-cumChrLen[currentMinChri])%scatterVLineGap2;
        k=i+cumChrLen[currentMinChri];
        j=currentMinChri+1;
        while((long double)k<=currentRightPosF){
            if(j<=currentMaxChri&&k>(long long)cumChrLen[j]){
                i=(k-cumChrLen[j])-(k-cumChrLen[j])%scatterVLineGap2;
                k=i+cumChrLen[j];
                j++;
            }
            else{
                if((long double)k>=currentLeftPosF){
                    tVLineX=(k-currentLeftPosF)/(long double)(positionGapLenF)*(width0-widgetLeftMargin)+widgetLeftMargin;
                    painter->drawLine(tVLineX,widgetTopMargin,tVLineX,height0);
                }
                k+=scatterVLineGap2;
            }
        }
    }

    if(isDisplayMajorGridline){
        i=(currentLeftPosF-cumChrLen[currentMinChri])-((long long)currentLeftPosF-cumChrLen[currentMinChri])%scatterVLineGap1;
        k=i+cumChrLen[currentMinChri];
        j=currentMinChri+1;
        while((long double)k<=currentRightPosF){
            if((long long)j<=currentMaxChri&&k>(long long)cumChrLen[j]){
                i=(k-cumChrLen[j])-(k-cumChrLen[j])%scatterVLineGap1;
                k=i+cumChrLen[j];
                j++;
            }
            else{
                if((long double)k>=currentLeftPosF&&k-cumChrLen[j-1]){
                    tVLineX=(k-currentLeftPosF)/(long double)(positionGapLenF)*(width0-widgetLeftMargin)+widgetLeftMargin;
                    painter->setPen("#bbbbbb");
                    painter->drawLine(tVLineX,widgetTopMargin+1,tVLineX,height0);
                    painter->setPen("#000000");
                    painter->drawText(tVLineX-positionLabelWidth/2,
                                      posLabely,
                                      positionLabelWidth,
                                      positionLabelHeight,
                                      Qt::AlignCenter,
                                      QString::number(k-cumChrLen[j-1]));
                }
                k+=scatterVLineGap1;
            }
        }
    }

    painter->setPen(QPen(Qt::darkGreen,2));
    painter->setFont(QFont("Arial",-1,1));
    for(i=currentMinChri+1;(long long)i<=currentMaxChri;++i){
        tVLineX=(cumChrLen[i]-currentLeftPosF)/(long double)(positionGapLenF)*(width0-widgetLeftMargin)+widgetLeftMargin+1;
        painter->drawLine(tVLineX,widgetTopMargin,tVLineX,height0);
        painter->drawText(tVLineX-positionLabelWidth/2,
                          posLabely,
                          positionLabelWidth,
                          positionLabelHeight,
                          Qt::AlignCenter,
                          chrName[i]);
    }

    painter->restore();
}

void BSAIndexScatterWidget::paintZoomRegionLineInPainter(QPainter *painter)
{
    QPen tPen;
    int height0=height();
    long long i,j;

    if(!isLeftMousePressed) return;

    painter->save();

    tPen.setColor(Qt::red);
    painter->setPen(tPen);
    painter->drawLine(leftMousePressedPosX,1,leftMousePressedPosX,height0);
    if(leftMouseMovingPosX>0){
        painter->drawLine(leftMouseMovingPosX,1,leftMouseMovingPosX,height0);
    }
    tPen.setWidth(3);
    painter->setPen(tPen);
    painter->drawLine(leftMousePressedPosX,1,leftMousePressedPosX,widgetTopMargin);
    if(leftMouseMovingPosX>0){
        painter->drawLine(leftMouseMovingPosX,1,leftMouseMovingPosX,widgetTopMargin);
        tPen.setColor(QColor("#fd6702"));
        painter->setPen(tPen);
//        painter->drawLine(leftMousePressedPosX,widgetTopMargin/2,leftMouseMovingPosX,widgetTopMargin/2);
        i=std::min(leftMousePressedPosX,leftMouseMovingPosX);
        j=std::max(leftMousePressedPosX,leftMouseMovingPosX);
        painter->fillRect(QRectF(i,1,j-i+1,widgetTopMargin),QBrush(QColor(255,0,0,160)));
//        if(j-i+1>=16){
//            painter->drawLine(i+2,widgetTopMargin/2,i+7,widgetTopMargin/2+5);
//            painter->drawLine(i+2,widgetTopMargin/2,i+7,widgetTopMargin/2-5);
//            painter->drawLine(j-2,widgetTopMargin/2,j-7,widgetTopMargin/2+5);
//            painter->drawLine(j-2,widgetTopMargin/2,j-7,widgetTopMargin/2-5);
//        }
    }

    painter->restore();
}

void BSAIndexScatterWidget::paintGenesInPainter(QPainter *painter)
{
    painter->save();

    double painterAxleFactor=0.75;
    double geneTriangleMaxLen=geneModelHeight*std::sqrt(3)*0.5;
    double dynamicTriangleLen;
    QBrush forwardBrush(colorForwardGene);
    QBrush reverseBrush(colorReverseGene);
    QBrush nullStrandBrush(colorNullStrandGene);
    QBrush defaultBrush(colorDefaultGene);
    QBrush selectedBrush(colorSelectedFrame);
    QBrush basicGeneBrush(colorNullStrandGene);
    QBrush tBrush;
    QPen forwardPen(forwardBrush,2);
    QPen reversePen(reverseBrush,2);
    long long i;
    int height0=height(),width0=width();
    int painty0=height0-painterAxleFactor*(height0-widgetTopMargin)*(1-scatterAreaHeightRatio)-0.5*geneModelHeight;
    int paintx0,paintl;
    int paintGeneNamey0,paintGeneNamex0;
    int annoAreay0=(height0-widgetTopMargin)*scatterAreaHeightRatio+widgetTopMargin;
    long long tGeneStart=0,tGeneStop=0;
    long double tPaintStart,tPaintStop;
    long double positionGapLenF=currentRightPosF-currentLeftPosF;
    int paintAreaWidth=width0-widgetLeftMargin;
    GeneAnnotation* pGene=nullptr;
    QPolygonF geneTriangle;
    QPainterPath geneTrianglePath;

    painter->setPen(QPen(Qt::black,2));
    painter->drawLine(widgetLeftMargin,
                      height0-painterAxleFactor*(height0-widgetTopMargin)*(1-scatterAreaHeightRatio),
                      width0,
                      height0-painterAxleFactor*(height0-widgetTopMargin)*(1-scatterAreaHeightRatio));
    painter->setPen(QPen(Qt::black,1));

    if(nPaintGenes<=0){
        painter->restore();
        return;
    }

    if(currentMaxGenei-currentMinGenei>2000){
        painter->fillRect(QRectF(widgetLeftMargin,
                                 painty0,
                                 width0-widgetLeftMargin,
                                 geneModelHeight),basicGeneBrush);
        painter->restore();
        return;
    }

    paintGeneNamey0 = (painty0+annoAreay0)/2;
    if(painty0-paintGeneNamey0>50) paintGeneNamey0=painty0-50;

    if(geneDisplayMode==1){
        for(i=currentMinGenei;i<=currentMaxGenei;++i){
            pGene=(genome->getGene(i));
            tGeneStart=pGene->start+cumChrLen[pGene->idChr],tGeneStop=pGene->stop+cumChrLen[pGene->idChr];
            tPaintStart=tGeneStart-0.5,tPaintStop=tGeneStop+0.5;
            tPaintStart=std::max(tPaintStart,currentLeftPosF);
            tPaintStop=std::min(tPaintStop,currentRightPosF);
            paintx0=(tPaintStart-currentLeftPosF)/positionGapLenF*paintAreaWidth+widgetLeftMargin;
            paintl=std::max((long double)(tPaintStop-tPaintStart)/positionGapLenF*paintAreaWidth,(long double)1.0);
            painter->fillRect(QRectF(paintx0,painty0,paintl,geneModelHeight),defaultBrush);
            if(paintl>10){
                switch(pGene->strand){
                case '+':
                    painter->setPen(forwardPen);
                    painter->drawLine(paintx0+paintl-3,painty0-13,paintx0+paintl,painty0-10);
                    break;
                case '-':
                    painter->setPen(reversePen);
                    painter->drawLine(paintx0,painty0-10,paintx0+3,painty0-13);
                    break;
                default: break;
                }
                painter->drawLine(paintx0,painty0-10,paintx0+paintl,painty0-10);
            }
            painter->setPen(QPen(Qt::black,3));
            painter->setFont(geneNameFont);
            if(currentMaxGenei-currentMinGenei<=20){
                paintGeneNamex0 = paintx0;
                painter->drawText(paintGeneNamex0,
                                  paintGeneNamey0,
                                  300,
                                  100,
                                  Qt::AlignLeft|Qt::AlignTop,
                                  pGene->name);
            }
        }
    }
    else if(geneDisplayMode==2){
        for(i=currentMinGenei;i<=currentMaxGenei;++i){
            pGene=(genome->getGene(i));
            tGeneStart=pGene->start+cumChrLen[pGene->idChr],tGeneStop=pGene->stop+cumChrLen[pGene->idChr];
            tPaintStart=tGeneStart-0.5,tPaintStop=tGeneStop+0.5;
            tPaintStart=std::max(tPaintStart,currentLeftPosF);
            tPaintStop=std::min(tPaintStop,currentRightPosF);
            paintx0=(long double)(tPaintStart-currentLeftPosF)/positionGapLenF*paintAreaWidth+widgetLeftMargin;
            paintl=std::max((long double)(tPaintStop-tPaintStart)/positionGapLenF*paintAreaWidth,(long double)1.0);
            if(paintl<=10){
                switch(pGene->strand){
                case '+':
                    painter->fillRect(QRectF(paintx0,painty0,paintl,geneModelHeight),forwardBrush);
                    break;
                case '-':
                    painter->fillRect(QRectF(paintx0,painty0,paintl,geneModelHeight),reverseBrush);
                    break;
                default:
                    painter->fillRect(QRectF(paintx0,painty0,paintl,geneModelHeight),nullStrandBrush);
                    break;
                }
            }
            else{
                dynamicTriangleLen=0.2*paintl;
                dynamicTriangleLen=std::min(dynamicTriangleLen,geneTriangleMaxLen);
                geneTriangle.clear();
                geneTrianglePath.clear();
                switch(pGene->strand){
                case '+':
                    geneTriangle.append(QPointF(paintx0+paintl-1,painty0+0.5*geneModelHeight));
                    geneTriangle.append(QPointF(paintx0+paintl-dynamicTriangleLen,painty0));
                    geneTriangle.append(QPointF(paintx0+paintl-dynamicTriangleLen,painty0+geneModelHeight));
                    geneTrianglePath.addPolygon(geneTriangle);
                    tBrush=forwardBrush;
                    painter->fillPath(geneTrianglePath,tBrush);
                    painter->fillRect(QRectF(paintx0,painty0,paintl-dynamicTriangleLen,geneModelHeight),tBrush);
                    break;
                case '-':
                    geneTriangle.append(QPointF(paintx0,painty0+0.5*geneModelHeight));
                    geneTriangle.append(QPointF(paintx0+dynamicTriangleLen,painty0));
                    geneTriangle.append(QPointF(paintx0+dynamicTriangleLen,painty0+geneModelHeight));
                    geneTrianglePath.addPolygon(geneTriangle);
                    tBrush=reverseBrush;
                    painter->fillPath(geneTrianglePath,tBrush);
                    painter->fillRect(QRectF(paintx0+dynamicTriangleLen,painty0,paintl-dynamicTriangleLen,geneModelHeight),tBrush);
                    break;
                default:
                    tBrush=nullStrandBrush;
                    painter->fillRect(QRectF(paintx0,painty0,paintl,geneModelHeight),tBrush);
                    break;
                }
            }
            painter->setPen(QPen(Qt::black,3));
            painter->setFont(geneNameFont);
            if(currentMaxGenei-currentMinGenei<=20){
                paintGeneNamex0 = paintx0;
                painter->drawText(paintGeneNamex0,
                                  paintGeneNamey0,
                                  300,
                                  100,
                                  Qt::AlignLeft|Qt::AlignTop,
                                  pGene->name);
            }
        }
    }

    painter->restore();
}

void BSAIndexScatterWidget::paintGenesModelInPainter(QPainter *painter)
{
    if(!(genome->haveGeneModel())) return;

    painter->save();

    float painterAxleFactor=0.5;
    QBrush CDSBrush(colorCDS);
    QBrush UTR5Brush(color5UTR);
    QBrush UTR3Brush(color3UTR);
    QBrush basicGeneBrush(colorNullStrandGene);
    long long i;
    GeneAnnotation* pGene=nullptr;
    std::list<long long>* pCDSList=nullptr;
    std::list<long long>::iterator iter;
    int height0=height(),width0=width();
    int painty0=height0-painterAxleFactor*(height0-widgetTopMargin)*(1-scatterAreaHeightRatio)-0.5*geneModelHeight;
    int paintBasicy0=height0-painterAxleFactor*(height0-widgetTopMargin)*(1-scatterAreaHeightRatio)-0.15*geneModelHeight;
    int paintx0,paintl;
    long long tGeneStart=0,tGeneStop=0,tStart=0,tStop=0;
    long double tPaintStart,tPaintStop;
    long double positionGapLenF=currentRightPosF-currentLeftPosF;
    int paintAreaWidth=width0-widgetLeftMargin;

    painter->setPen(QPen(Qt::black,2));
    painter->drawLine(widgetLeftMargin,
                      height0-painterAxleFactor*(height0-widgetTopMargin)*(1-scatterAreaHeightRatio),
                      width0,
                      height0-painterAxleFactor*(height0-widgetTopMargin)*(1-scatterAreaHeightRatio));
    painter->setPen(QPen(Qt::black,1));


    if(nPaintGenes<=0){
        painter->restore();
        return;
    }

    if(currentMaxGenei-currentMinGenei>1000){
        painter->fillRect(QRectF(widgetLeftMargin,
                                 painty0,
                                 width0-widgetLeftMargin,
                                 geneModelHeight),basicGeneBrush);
        painter->restore();
        return;
    }

    for(i=currentMinGenei;i<=currentMaxGenei;++i){


        pGene=(genome->getGene(i));

        // paint basic gene model
        tGeneStart=pGene->start+cumChrLen[pGene->idChr],tGeneStop=pGene->stop+cumChrLen[pGene->idChr];
        tPaintStart=tGeneStart-0.5,tPaintStop=tGeneStop+0.5;
        tPaintStart=std::max(tPaintStart,currentLeftPosF);
        tPaintStop=std::min(tPaintStop,currentRightPosF);
        paintx0=(long double)(tPaintStart-currentLeftPosF)/positionGapLenF*paintAreaWidth+widgetLeftMargin;
        paintl=std::max((long double)(tPaintStop-tPaintStart)/positionGapLenF*paintAreaWidth,(long double)1.0);
        painter->fillRect(QRectF(paintx0,paintBasicy0,paintl,0.3*geneModelHeight),basicGeneBrush);

        // paint CDS
        pCDSList=pGene->getCDSList();
        iter=pCDSList->begin();
        while(iter!=pCDSList->end()){
            tStart=*iter+cumChrLen[pGene->idChr];
            iter++;
            tStop=*iter+cumChrLen[pGene->idChr];
            iter++;
            tPaintStart=tStart-0.5,tPaintStop=tStop+0.5;
            if((long double)tPaintStart>currentRightPosF||
               (long double)tPaintStop<currentLeftPosF) continue;

            tPaintStart=std::max(tPaintStart,currentLeftPosF);
            tPaintStop=std::min(tPaintStop,currentRightPosF);
            paintx0=(long double)(tPaintStart-currentLeftPosF)/positionGapLenF*paintAreaWidth+widgetLeftMargin;
            paintl=std::max((long double)(tPaintStop-tPaintStart)/positionGapLenF*paintAreaWidth,(long double)1.0);
            painter->fillRect(QRectF(paintx0,painty0,paintl,geneModelHeight),CDSBrush);
        }

        // paint UTR
        if(pGene->startUTR5>0&&pGene->stopUTR5>0){
            tStart=pGene->startUTR5+cumChrLen[pGene->idChr],tStop=pGene->stopUTR5+cumChrLen[pGene->idChr];
            tPaintStart=tStart-0.5,tPaintStop=tStop+0.5;
            if((long double)tPaintStart<=currentRightPosF||
               (long double)tPaintStop>=currentLeftPosF){
                tPaintStart=std::max(tPaintStart,currentLeftPosF);
                tPaintStop=std::min(tPaintStop,currentRightPosF);
                paintx0=(long double)(tPaintStart-currentLeftPosF)/positionGapLenF*paintAreaWidth+widgetLeftMargin;
                paintl=std::max((long double)(tPaintStop-tPaintStart)/positionGapLenF*paintAreaWidth,(long double)1.0);
                painter->fillRect(QRectF(paintx0,painty0,paintl,geneModelHeight),UTR5Brush);

            }
        }
        if(pGene->startUTR3>0&&pGene->stopUTR3>0){
            tStart=pGene->startUTR3+cumChrLen[pGene->idChr],tStop=pGene->stopUTR3+cumChrLen[pGene->idChr];
            tPaintStart=tStart-0.5,tPaintStop=tStop+0.5;
            if((long double)tPaintStart<=currentRightPosF||
               (long double)tPaintStop>=currentLeftPosF){
                tPaintStart=std::max(tPaintStart,currentLeftPosF);
                tPaintStop=std::min(tPaintStop,currentRightPosF);
                paintx0=(long double)(tPaintStart-currentLeftPosF)/positionGapLenF*paintAreaWidth+widgetLeftMargin;
                paintl=std::max((long double)(tPaintStop-tPaintStart)/positionGapLenF*paintAreaWidth,(long double)1.0);
                painter->fillRect(QRectF(paintx0,painty0,paintl,geneModelHeight),UTR3Brush);

            }
        }
    }


    painter->restore();
}

void BSAIndexScatterWidget::paintSelectedPoint(QPainter *painter)
{
    if(selectedPointi.id<currentMinPointsi||selectedPointi.id>currentMaxPointsi) return;
    painter->save();
    int width0=width(),height0=height();
    int lx=widgetLeftMargin,rx=width0;
    int px=0,py=0,px1,px2;
    size_t tStart=points[selectedPointi.id].start;
    size_t tStop=points[selectedPointi.id].stop;
    long double meanPos=(tStart+tStop)/2.0;
    if(meanPos+cumChrLen[points[selectedPointi.id].idChr]<currentLeftPosF||
            meanPos+cumChrLen[points[selectedPointi.id].idChr]>currentRightPosF){
        painter->restore();
        return ;
    }
    px=calcuChartX(points[selectedPointi.id].idChr,meanPos);
    py=calcuChartY(selectedPointi);
    painter->setPen(QPen(QBrush(QColor("#00ff00")),8));
    painter->drawPoint(px,py);

    painter->restore();
}

void BSAIndexScatterWidget::paintSelectedGene(QPainter *painter)
{
    if(selectedGenei<currentMinGenei||
       selectedGenei>currentMaxGenei||
       !geneDisplayMode) return;
    painter->save();
    double painterAxleFactor=0.75;
    int height0=height(),width0=width();
    int painty0=height0-painterAxleFactor*(height0-widgetTopMargin)*(1-scatterAreaHeightRatio)-0.5*geneModelHeight;
    int paintx0,paintl;
    long long tGeneStart=0,tGeneStop=0;
    long double tPaintStart,tPaintStop;
    long double positionGapLenF=currentRightPosF-currentLeftPosF;
    int paintAreaWidth=width0-widgetLeftMargin;
    GeneAnnotation* pGene=genome->getGene(selectedGenei);
    tGeneStart=pGene->start+cumChrLen[pGene->idChr],tGeneStop=pGene->stop+cumChrLen[pGene->idChr];
    tPaintStart=tGeneStart-0.5,tPaintStop=tGeneStop+0.5;
    tPaintStart=std::max(tPaintStart,currentLeftPosF);
    tPaintStop=std::min(tPaintStop,currentRightPosF);
    paintx0=(long double)(tPaintStart-currentLeftPosF)/positionGapLenF*paintAreaWidth+widgetLeftMargin;
    paintl=std::max((long double)(tPaintStop-tPaintStart)/positionGapLenF*paintAreaWidth,(long double)1.0);
    painter->setPen(QPen(QColor("#ff0000"),4));
    painter->drawRect(QRectF(paintx0-2,painty0-2,paintl+4,geneModelHeight+4));
//    painter->fillRect(QRectF(paintx0,painty0,paintl,geneModelHeight),QBrush(QColor("#00ff00")));
    painter->restore();
}

void BSAIndexScatterWidget::paintGenomeSeq(QPainter *painter)
{
    int height0=height(),width0=width();
    double painterAxleFactor=0.25;
    int painty0=height0-painterAxleFactor*(height0-widgetTopMargin)*(1-scatterAreaHeightRatio)-0.5*baseHeight;
    int paintAreaWidth=width0-widgetLeftMargin;
    long double positionGapLenF=currentRightPosF-currentLeftPosF;
    long double oneBaseLen=paintAreaWidth/positionGapLenF;
    long double tStart,tStop;
    long long tStarti,tNBase,tIBase,tPos,i;
    short tBaseValue;
    char* tSeq=nullptr;
    char base1,base2;
    QBrush basicGeneBrush(colorNullStrandGene);

    painter->save();

    if(positionGapLenF>1000){
        painter->fillRect(QRectF(widgetLeftMargin,
                                 painty0,
                                 width0-widgetLeftMargin,
                                 geneModelHeight),basicGeneBrush);
        painter->restore();
        return ;
    }
    if(oneBaseLen<1) oneBaseLen=1;

    long long iChr;
    for(iChr=currentMinChri;iChr<=currentMaxChri;++iChr){
        tStart=std::max((long double)cumChrLen[iChr]+1,currentLeftPosF);
        tStop=std::min((long double)cumChrLen[iChr]+chrLen[iChr],currentRightPosF);
        tStarti=tStart-cumChrLen[iChr]-0.5;
        tNBase=tStop-tStart+1;
        if(tNBase+tStart<(long long)(tStop+0.5)+0.5) ++tNBase;
        tSeq=genomeSeq->getSeq(iChr,tStart-cumChrLen[iChr]);
        tIBase=0,i=0,tPos=tStart+0.5;
        while(tIBase<tNBase){
            tBaseValue=tSeq[i];
//            qDebug() << QString::number(tBaseValue,2);
            base2=GenomeSequence::Value2Base(tBaseValue&0b1111);
            tBaseValue>>=4;
            base1=GenomeSequence::Value2Base(tBaseValue&0b1111);
            if(tIBase==0){
                if(tStarti%2==0){
                    paintOneBase(base1,
                                 ((long double)tPos-currentLeftPosF)*paintAreaWidth/positionGapLenF+widgetLeftMargin,
                                 oneBaseLen,
                                 painter);
                    ++tPos;
                    paintOneBase(base2,
                                 ((long double)tPos-currentLeftPosF)*paintAreaWidth/positionGapLenF+widgetLeftMargin,
                                 oneBaseLen,
                                 painter);
                    ++tPos;
                    tIBase+=2;
                }
                else{
                    paintOneBase(base2,
                                 ((long double)tPos-currentLeftPosF)*paintAreaWidth/positionGapLenF+widgetLeftMargin,
                                 oneBaseLen,
                                 painter);
                    ++tPos;
                    ++tIBase;
                }
            }
            else{
                paintOneBase(base1,
                             ((long double)tPos-currentLeftPosF)*paintAreaWidth/positionGapLenF+widgetLeftMargin,
                             oneBaseLen,
                             painter);
                ++tPos;
                ++tIBase;
                if(tIBase<tNBase){
                    paintOneBase(base2,
                                 ((long double)tPos-currentLeftPosF)*paintAreaWidth/positionGapLenF+widgetLeftMargin,
                                 oneBaseLen,
                                 painter);
                    ++tPos;
                    ++tIBase;
                }
            }
            ++i;
        }
    }

    painter->restore();
}

void BSAIndexScatterWidget::paintOneBase(char inBase, int xPos, int oneBaseLen, QPainter *painter)
{
    double painterAxleFactor=0.25;
    int height0=height(),width0=width();
    int painty0=height0-painterAxleFactor*(height0-widgetTopMargin)*(1-scatterAreaHeightRatio)-0.5*baseHeight;
    int paintx0=xPos-oneBaseLen/2;
    int paintx1=xPos+oneBaseLen/2;
    int paintLen;
    if(paintx0<widgetLeftMargin) paintx0=widgetLeftMargin;
    if(paintx1>width0) paintx1=width0;
    paintLen=paintx1-paintx0+1;
    if(paintLen<=0) return;
//    qDebug() << inBase;
    switch (inBase) {
    case 'A':
        painter->fillRect(QRect(paintx0,painty0,paintLen,baseHeight),baseA);
        break;
    case 'T':
        painter->fillRect(QRect(paintx0,painty0,paintLen,baseHeight),baseT);
        break;
    case 'C':
        painter->fillRect(QRect(paintx0,painty0,paintLen,baseHeight),baseC);
        break;
    case 'G':
        painter->fillRect(QRect(paintx0,painty0,paintLen,baseHeight),baseG);
        break;
    case 'N':
        painter->fillRect(QRect(paintx0,painty0,paintLen,baseHeight),baseN);
        break;
    default:
        painter->fillRect(QRect(paintx0,painty0,paintLen,baseHeight),base_);
        break;
    }
    if(paintLen>10){
        switch (inBase) {
        case 'A':
            painter->drawText(QRect(paintx0,painty0,paintLen,baseHeight),Qt::AlignHCenter|Qt::AlignVCenter,"A");
            break;
        case 'T':
            painter->drawText(QRect(paintx0,painty0,paintLen,baseHeight),Qt::AlignHCenter|Qt::AlignVCenter,"T");
            break;
        case 'C':
            painter->drawText(QRect(paintx0,painty0,paintLen,baseHeight),Qt::AlignHCenter|Qt::AlignVCenter,"C");
            break;
        case 'G':
            painter->drawText(QRect(paintx0,painty0,paintLen,baseHeight),Qt::AlignHCenter|Qt::AlignVCenter,"G");
            break;
        case 'N':
            painter->drawText(QRect(paintx0,painty0,paintLen,baseHeight),Qt::AlignHCenter|Qt::AlignVCenter,"N");
            break;
        default:
            break;
        }
    }
}

void BSAIndexScatterWidget::paintLabelPoints(QPainter *painter)
{
    painter->save();
    int px=0,py=0;
    long long tStart,tStop;
    long double meanPos;
    BSAPointID ip(0,-1);
    painter->setPen(QPen(QBrush(QColor("#ff0000")),8));
    for(long long i=0;i<idLabelPoints.size();++i){
        ip=idLabelPoints[i];
        tStart=points[ip.id].start;
        tStop=points[ip.id].stop;
        meanPos=(tStart+tStop)/2.0;
        if(meanPos+cumChrLen[points[selectedPointi.id].idChr]<currentLeftPosF||
                meanPos+cumChrLen[points[selectedPointi.id].idChr]>currentRightPosF){
            continue;
        }
        px=calcuChartX(points[ip.id].idChr,meanPos);
        py=calcuChartY(ip);
        painter->drawPoint(px,py);
    }

    painter->restore();
}

void BSAIndexScatterWidget::paintPointsToBuffer()
{
    long long mid,ta=0,tb=nPoints-1;
    while(ta<=tb){
        mid=(ta+tb)/2;
        if((long double)(cumRPointsPos[mid])>=currentLeftPosF) tb=mid-1;
        else ta=mid+1;
    }
    currentMinPointsi=ta;
    ta=0,tb=nPoints-1;
    while(ta<=tb){
        mid=(ta+tb)/2;
        if((long double)(cumLPointsPos[mid])>currentRightPosF) tb=mid-1;
        else ta=mid+1;
    }
    currentMaxPointsi=tb;
    if(currentMinPointsi>currentMaxPointsi){
        ta=currentMinPointsi,tb=currentMaxPointsi;
        currentMinPointsi=tb,currentMaxPointsi=ta;
    }
    if(currentMinPointsi<0||currentMaxPointsi<0||
       currentMinPointsi>=nMarkers||currentMaxPointsi>=nMarkers){
        nPaintPoints = 0;
        return;
    }
    adjustPaintPointsPos();
    glBindVertexArray(idPointsVAOH);
    glBindBuffer(GL_ARRAY_BUFFER,idPointsVBOH);
    glBufferSubData(GL_ARRAY_BUFFER,0,2*nPaintPoints*sizeof(GLfloat),paintHPoints);
    glBindVertexArray(idPointsVAOL);
    glBindBuffer(GL_ARRAY_BUFFER,idPointsVBOL);
    glBufferSubData(GL_ARRAY_BUFFER,0,2*nPaintPoints*sizeof(GLfloat),paintLPoints);
    glBindVertexArray(idPointsVAOD);
    glBindBuffer(GL_ARRAY_BUFFER,idPointsVBOD);
    glBufferSubData(GL_ARRAY_BUFFER,0,2*nPaintPoints*sizeof(GLfloat),paintDPoints);
    glBindVertexArray(idPointsVAOT1);
    glBindBuffer(GL_ARRAY_BUFFER,idPointsVBOT1);
    glBufferSubData(GL_ARRAY_BUFFER,0,2*nPaintPoints*sizeof(GLfloat),paintT1Points);
    glBindVertexArray(idPointsVAOT2);
    glBindBuffer(GL_ARRAY_BUFFER,idPointsVBOT2);
    glBufferSubData(GL_ARRAY_BUFFER,0,2*nPaintPoints*sizeof(GLfloat),paintT2Points);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

}

void BSAIndexScatterWidget::paintMarkersToBuffer()
{
    if(!markers||currentRightPosF-currentLeftPosF>1000000){
        nPaintMarkers = 0;
        return;
    }
    long long mid,ta=0,tb=nMarkers-1;
    while(ta<=tb){
        mid=(ta+tb)/2;
        if((long double)(cumMarkersPos[mid])>=currentLeftPosF) tb=mid-1;
        else ta=mid+1;
    }
    currentMinMarkersi=ta;
    ta=0,tb=nMarkers-1;
    while(ta<=tb){
        mid=(ta+tb)/2;
        if((long double)(cumMarkersPos[mid])>currentRightPosF) tb=mid-1;
        else ta=mid+1;
    }
    currentMaxMarkersi=tb;
    if(currentMinMarkersi<=currentMaxMarkersi){
        adjustPaintMarkersPos();
        glBindVertexArray(idMarkersVAOH);
        glBindBuffer(GL_ARRAY_BUFFER,idMarkersVBOH);
        glBufferSubData(GL_ARRAY_BUFFER,0,2*nPaintMarkers*sizeof(GLfloat),paintHMarkers);
        glBindVertexArray(idMarkersVAOL);
        glBindBuffer(GL_ARRAY_BUFFER,idMarkersVBOL);
        glBufferSubData(GL_ARRAY_BUFFER,0,2*nPaintMarkers*sizeof(GLfloat),paintLMarkers);
        glBindVertexArray(idMarkersVAOD);
        glBindBuffer(GL_ARRAY_BUFFER,idMarkersVBOD);
        glBufferSubData(GL_ARRAY_BUFFER,0,2*nPaintMarkers*sizeof(GLfloat),paintDMarkers);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER,0);

    }
    else{
        nPaintMarkers = 0;
    }
}

void BSAIndexScatterWidget::adjustPaintPointsPos()
{
    int width0=width(),height0=height();
    float tx,ty;
    double px,py,meanPos;
    size_t tN=0;
    FD::BSAInterVal inter;

    for(long long i=currentMinPointsi;i<=currentMaxPointsi;++i){
        inter=points[i];
        meanPos=(inter.start+inter.stop)/2.0+cumChrLen[inter.idChr];
        //if(meanPos>currentRightPosF||meanPos<currentLeftPosF) continue;
        px=calcuChartX(inter.idChr,(long double)(inter.start+inter.stop)/2.0);
        tx=2*px/width0-1;

        py=calcuChartY(BSAPointID(i,0));
        ty=1-2*py/height0;
        paintHPoints[tN*2]=tx;
        paintHPoints[tN*2+1]=ty;

        py=calcuChartY(BSAPointID(i,1));
        ty=1-2*py/height0;
        paintLPoints[tN*2]=tx;
        paintLPoints[tN*2+1]=ty;

        py=calcuChartY(BSAPointID(i,2));
        ty=1-2*py/height0;
        paintDPoints[tN*2]=tx;
        paintDPoints[tN*2+1]=ty;

        py=calcuChartY(BSAPointID(i,3));
        ty=1-2*py/height0;
        paintT1Points[tN*2]=tx;
        paintT1Points[tN*2+1]=ty;

        py=calcuChartY(BSAPointID(i,4));
        ty=1-2*py/height0;
        paintT2Points[tN*2]=tx;
        paintT2Points[tN*2+1]=ty;

        ++tN;
    }

    nPaintPoints = tN;
}

void BSAIndexScatterWidget::adjustPaintMarkersPos()
{
    ValPoints2 tp;
    size_t tpos,tN=0;
    float px,py,tx,ty;
    int width0=width(),height0=height();
    for(long long i=currentMinMarkersi;i<=currentMaxMarkersi;++i){
        tp=markers[i];
        tpos=tp.pos+cumChrLen[tp.idchr];
        if(tpos<currentLeftPosF||tpos>currentRightPosF) continue;
        px=calcuChartX(tp.idchr,tp.pos);
        tx=2*px/width0-1;

        py=calcuChartY(tp.val1,0);
        ty=1-2*py/height0;
        paintHMarkers[2*tN]   = tx;
        paintHMarkers[2*tN+1] = ty;

        py=calcuChartY(tp.val2,1);
        ty=1-2*py/height0;
        paintLMarkers[2*tN]   = tx;
        paintLMarkers[2*tN+1] = ty;

        py=calcuChartY(tp.val1-tp.val2,2);
        ty=1-2*py/height0;
        paintDMarkers[2*tN]   = tx;
        paintDMarkers[2*tN+1] = ty;

        ++tN;
    }

    nPaintMarkers = tN;
}

void BSAIndexScatterWidget::adjustPaintChri()
{
    int a=0,b=nChr,m=0;
    while(a<=b){
        m=(a+b)/2;
        if((long double)(cumChrLen[m])>=currentLeftPosF) b=m-1;
        else a=m+1;
    }
    currentMinChri=b;
    a=0,b=nChr;
    while(a<=b){
        m=(a+b)/2;
        if((long double)cumChrLen[m]>=currentRightPosF) b=m-1;
        else a=m+1;
    }
    currentMaxChri=b;
}

void BSAIndexScatterWidget::adjustPaintGenesi()
{
    if(!genome||!genome->getValid()){
        nPaintGenes=0;
        return;
    }
    long long i=0;
    long double posa=currentLeftPosF-cumChrLen[currentMinChri];
    long double posb=currentRightPosF-cumChrLen[currentMaxChri];
    long long posai=genome->search(currentMinChri,posa-0.5,GenomeAnnotation::GA_STOP);
    long long posbi=genome->searchLower(currentMaxChri,posb+0.5);
    if(posai>genome->getNGenes()){
        if(currentMinChri!=nChr-1){
            for(i=currentMinChri+1;i<=currentMaxChri;++i){
                if(!genome->isEmptyChr(i)){
                    posai=genome->getChrFirsti(i);
                    break;
                }
            }
        }
        if(posai>genome->getNGenes()){
            nPaintGenes=0;
            return;
        }
    }
    if(posbi>genome->getNGenes()){
        if(currentMaxChri!=0){
            for(i=currentMaxChri-1;i>=currentMinChri;--i){
                if(!genome->isEmptyChr(i)){
                    posbi=genome->getChrLasti(i);
                    break;
                }
            }
        }
        if(posbi>genome->getNGenes()){
            nPaintGenes=0;
            return;
        }
    }
    if(posai<=posbi) {
        currentMaxGenei=posbi,currentMinGenei=posai;
        nPaintGenes=currentMaxGenei-currentMinGenei+1;
    }
    else nPaintGenes=0;
}

void BSAIndexScatterWidget::lookGene(long long iGene)
{
    if(iGene>=0&&iGene<genome->getNGenes()){
        GeneAnnotation* pGene=genome->getGene(iGene);
        long long tStart=pGene->start+cumChrLen[pGene->idChr];
        long long tStop=pGene->stop+cumChrLen[pGene->idChr];
        long long positionGapLen=tStop-tStart+1;
        currentLeftPosF=std::max((long long)1,(long long)(tStart-positionGapLen/2));
        currentRightPosF=std::min((long long)maxCumPos,(long long)(tStop+positionGapLen/2));
        paintPointsToBuffer();
        paintMarkersToBuffer();
        adjustPaintChri();
        adjustPaintGenesi();
        emit currentIntervalChanged();
        update();
    }
}

void BSAIndexScatterWidget::lookInterval(int leftIdChr, int rightIdChr, long double leftChrPos, long double rightChrPos)
{
    long double maxLeftLen,maxRightLen,tmp;
    int tmpId;
    if(leftIdChr>=nChr) leftIdChr=nChr-1;
    if(leftIdChr<0) leftIdChr=0;
    if(rightIdChr>=nChr) rightIdChr=nChr-1;
    if(rightIdChr<0) rightIdChr=0;
    if(leftIdChr>rightIdChr){
        tmpId=leftIdChr;
        leftIdChr=rightIdChr;
        rightIdChr=tmpId;
        tmp=leftChrPos;
        leftChrPos=rightChrPos;
        rightChrPos=tmp;
    }
    maxLeftLen=chrLen[leftIdChr],maxRightLen=chrLen[rightIdChr];
    if(leftChrPos<1) leftChrPos=1;
    if(leftChrPos>maxLeftLen) leftChrPos=maxLeftLen;
    if(rightChrPos<1) rightChrPos=1;
    if(rightChrPos>maxRightLen) rightChrPos=maxRightLen;
    if(leftIdChr==rightIdChr&&rightChrPos<leftChrPos){
        tmp=leftChrPos;
        leftChrPos=rightChrPos;
        rightChrPos=tmp;
    }
    if(leftIdChr==rightIdChr&&rightChrPos-leftChrPos<minPosGapLen){
        return ;
    }
    currentLeftPosF=leftChrPos+cumChrLen[leftIdChr];
    currentRightPosF=rightChrPos+cumChrLen[rightIdChr];
    currentMinChri=leftIdChr;
    currentMaxChri=rightIdChr;
    paintPointsToBuffer();
    paintMarkersToBuffer();
    adjustPaintChri();
    adjustPaintGenesi();
    emit currentIntervalChanged();
    update();
}

void BSAIndexScatterWidget::getCurrentInterval(int &leftIdChr, long double &leftChrPos, int &rightIdChr, long double &rightChrPos)
{
    leftIdChr=currentMinChri;
    leftChrPos=currentLeftPosF-cumChrLen[leftIdChr];
    rightIdChr=currentMaxChri;
    rightChrPos=currentRightPosF-cumChrLen[rightIdChr];
}

GenomeAnnotation *BSAIndexScatterWidget::getGenome()
{
    return genome;
}

FD::BSAInterVal *BSAIndexScatterWidget::getPoints()
{
    return points;
}

size_t BSAIndexScatterWidget::getNPoints()
{
    return nPoints;
}

char **BSAIndexScatterWidget::getChrName()
{
    return chrName;
}

QColor BSAIndexScatterWidget::getPointsColor() const
{
    return colorScatterPoint;
}

QColor BSAIndexScatterWidget::getForwardGeneColor() const
{
    return colorForwardGene;
}

QColor BSAIndexScatterWidget::getReverseGeneColor() const
{
    return colorReverseGene;
}

QColor BSAIndexScatterWidget::getDefaultGeneColor() const
{
    return colorDefaultGene;
}

QColor BSAIndexScatterWidget::get5UTRColor() const
{
    return color5UTR;
}

QColor BSAIndexScatterWidget::get3UTRColor() const
{
    return color3UTR;
}

QColor BSAIndexScatterWidget::getCDSColor() const
{
    return colorCDS;
}

int BSAIndexScatterWidget::getMaxLabelPointsNum() const
{
    return maxLabelPointsNum;
}

void BSAIndexScatterWidget::zoomIn()
{
    long double positionGapLenF=currentRightPosF-currentLeftPosF;
    float tmpFactor1=.5-1.0/(2*zoomFactor),tmpFactor2=.5+1.0/(2*zoomFactor);
    long double pos1=(positionGapLenF*tmpFactor1)+currentLeftPosF;
    long double pos2=(positionGapLenF*tmpFactor2)+currentLeftPosF;
    if(pos2-pos1<minPosGapLen) return;
    currentLeftPosF=pos1;
    currentRightPosF=pos2;
    paintPointsToBuffer();
    paintMarkersToBuffer();
    adjustPaintChri();
    adjustPaintGenesi();
    emit currentIntervalChanged();
    update();
}

void BSAIndexScatterWidget::zoomOut()
{
    long double positionGapLenF=currentRightPosF-currentLeftPosF;
    float tmpFactor=0.5*zoomFactor;
    long double pos1=std::max((long double)(1),(currentLeftPosF)-(positionGapLenF*tmpFactor));
    long double pos2=std::min((long double)(maxCumPos),(currentRightPosF)+(positionGapLenF*tmpFactor));
    currentLeftPosF=pos1;
    currentRightPosF=pos2;
    paintPointsToBuffer();
    paintMarkersToBuffer();
    adjustPaintChri();
    adjustPaintGenesi();
    emit currentIntervalChanged();
    update();
}

void BSAIndexScatterWidget::moveLeft()
{
    long double positionGapLenF=currentRightPosF-currentLeftPosF;
    long double moveLeftPos=std::max((long double)1,(currentLeftPosF-moveFactor*positionGapLenF));
    long double moveLength=currentLeftPosF-moveLeftPos;
    currentLeftPosF  = moveLeftPos;
    currentRightPosF = currentRightPosF-moveLength;
    paintPointsToBuffer();
    paintMarkersToBuffer();
    adjustPaintChri();
    adjustPaintGenesi();
    emit currentIntervalChanged();
    update();
}

void BSAIndexScatterWidget::moveRight()
{
    long double positionGapLenF=currentRightPosF-currentLeftPosF;
    long double moveRightPos=std::min((long double)(maxCumPos),(currentRightPosF+moveFactor*positionGapLenF));
    long double moveLength=moveRightPos-currentRightPosF;
    currentLeftPosF  = currentLeftPosF+moveLength;
    currentRightPosF = moveRightPos;
    paintPointsToBuffer();
    paintMarkersToBuffer();
    adjustPaintChri();
    adjustPaintGenesi();
    emit currentIntervalChanged();
    update();
}

void BSAIndexScatterWidget::makeMenu()
{
    pMenu=new QMenu(this);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,&BSAIndexScatterWidget::customContextMenuRequested,this,&BSAIndexScatterWidget::menuExec);
    actAddLabelPoint=new QAction("Label",pMenu);
    actremoveLabelPoint=new QAction("Remove Label",pMenu);
    QAction* actZoomIn=new QAction("Zoom in",pMenu);
    QAction* actZoomOut=new QAction("Zoom out",pMenu);
    pMenu->addAction(actAddLabelPoint);
    pMenu->addAction(actremoveLabelPoint);
    pMenu->addSeparator();
    pMenu->addAction(actZoomIn);
    pMenu->addAction(actZoomOut);
    connect(actAddLabelPoint,&QAction::triggered,
            this,&BSAIndexScatterWidget::dealAddPointLabel);
    connect(actremoveLabelPoint,&QAction::triggered,
            this,&BSAIndexScatterWidget::dealRemovePointLabel);
    connect(actZoomIn,&QAction::triggered,this,&BSAIndexScatterWidget::zoomIn);
    connect(actZoomOut,&QAction::triggered,this,&BSAIndexScatterWidget::zoomOut);
}

void BSAIndexScatterWidget::setDisplayMajorGridline(bool flag)
{
    isDisplayMajorGridline=flag;
    update();
}

void BSAIndexScatterWidget::setDisplayMinorGridline(bool flag)
{
    isDisplayMinorGridline=flag;
    update();
}

void BSAIndexScatterWidget::switchGeneDisplayMode()
{
    if(geneDisplayMode==0) return;
    if(geneDisplayMode==1) geneDisplayMode=2;
    else geneDisplayMode=1;
    update();
}

void BSAIndexScatterWidget::setDisplayGene(bool flag)
{
    if(flag) geneDisplayMode=1;
    else geneDisplayMode=0;
    update();
}

void BSAIndexScatterWidget::setDisplayGeneModel(bool flag)
{
    isDisplayGeneModel=flag;
    update();
}

void BSAIndexScatterWidget::setDisplayGenome(bool flag)
{
    isDisplayGenomeSeq=flag;
    update();
}

void BSAIndexScatterWidget::setForwardGeneColor(QColor inColor,bool isUpdate)
{
    colorForwardGene=inColor;
    if(isUpdate) update();
}

void BSAIndexScatterWidget::setReverseGeneColor(QColor inColor,bool isUpdate)
{
    colorReverseGene=inColor;
    if(isUpdate) update();
}

void BSAIndexScatterWidget::setDefaultGeneColor(QColor inColor,bool isUpdate)
{
    colorDefaultGene=inColor;
    if(isUpdate) update();
}

void BSAIndexScatterWidget::set5UTRColor(QColor inColor,bool isUpdate)
{
    color5UTR=inColor;
    if(isUpdate) update();
}

void BSAIndexScatterWidget::set3UTRColor(QColor inColor,bool isUpdate)
{
    color3UTR=inColor;
    if(isUpdate) update();
}

void BSAIndexScatterWidget::setCDSColor(QColor inColor,bool isUpdate)
{
    colorCDS=inColor;
    if(isUpdate) update();
}

void BSAIndexScatterWidget::doRemoveAllLabelPoint()
{
    idLabelPoints.clear();
    update();
}

void BSAIndexScatterWidget::doRemoveLabelPoint(int row)
{
    idLabelPoints.removeAt(row);
    update();
}

void BSAIndexScatterWidget::doLookLabelPoint(int row)
{
    if(row>=0&&row<idLabelPoints.size()){
        long double tmpPos = (points[idLabelPoints[row].id].start+points[idLabelPoints[row].id].stop)/2
                +cumChrLen[points[idLabelPoints[row].id].idChr];
        long double positionGapLenF=currentRightPosF-currentLeftPosF;
        if(positionGapLenF>1000000) positionGapLenF=1000000;
        long double tStart=tmpPos-positionGapLenF/2;
        long double tStop=tmpPos+positionGapLenF/2;
        currentLeftPosF=tStart<1.0?1:tStart;
        currentRightPosF=tStop>maxCumPos?maxCumPos:tStop;
        paintPointsToBuffer();
        paintMarkersToBuffer();
        adjustPaintChri();
        adjustPaintGenesi();
        emit currentIntervalChanged();
        update();
    }
}

void BSAIndexScatterWidget::init()
{
    widgetLeftMargin=100;
    widgetTopMargin=100;
    scatterAreaHeightRatio=0.6;
    subScatterAreaHeightRatio=0.4;
    scatterScaleLen=20;
    zoomFactor=2;
    minPosGapLen=5;
    geneModelHeight=20;
    baseHeight=20;
    positionLabelHeight=20;
    positionLabelWidth=200;
    moveFactor=0.5;
    geneDisplayMaxPosGapLen=1000000;
    selectedPointsDistance2=20;
    geneDisplayMode=0;
    maxLabelPointsNum=1000;
    isDisplayMajorGridline=true;
    isDisplayMinorGridline=true;
    isDisplayGeneModel=true;
    isDisplayGenomeSeq=false;
    geneNameFont.setFamily("Times New Roman");
    geneNameFont.setItalic(true);
    geneNameFont.setBold(true);

    indexChartMaximun = 1;
    indexChartMinimun = 0;
    deltaChartMaximun = 1;
    deltaChartMinimun = -1;

    chrLen=nullptr;
    cumChrLen=nullptr;
    chrName=nullptr;
    nChr=0;
    maxCumPos=0;
    points=nullptr;
    cumLPointsPos=nullptr;
    cumRPointsPos=nullptr;
    nPoints=0;
    markers=nullptr;
    cumMarkersPos=nullptr;
    nMarkers=0;
    paintHPoints=nullptr;
    paintLPoints=nullptr;
    paintDPoints=nullptr;
    paintT1Points=nullptr;
    paintT2Points=nullptr;
    nPaintPoints=0;
    paintHMarkers=nullptr;
    paintLMarkers=nullptr;
    paintDMarkers=nullptr;
    nPaintMarkers=0;
    currentLeftPosF=0.0;
    currentRightPosF=0.0;
//    currentLeftPos=0;
//    currentRightPos=0;
    currentMaxPointsi=0;
    currentMinPointsi=0;
    currentMinMarkersi=0;
    currentMaxMarkersi=0;
    currentMaxChri=0;
    currentMinChri=0;
    currentMinGenei=0;
    currentMaxGenei=0;
    nPaintGenes=0;
    leftMousePressedPosX=0;
    leftMouseReleasedPosX=0;
    leftMouseMovingPosX=0;
    middleMousePressedPosX=0;
    isLeftMousePressed=false;
    genome=nullptr;
    genomeSeq=nullptr;
    maxLabelNum=10;
    middleMousePressedLeftPosF=0.0;
    middleMousePressedRightPosF=0.0;


    colorScatterPoint=QColor(255,127,51);
    colorSelectedFrame=QColor(0, 255, 0);
    colorForwardGene=QColor(0, 180, 0);
    colorReverseGene=QColor(180, 0, 0);
    colorNullStrandGene=QColor(100, 100, 100);
    colorDefaultGene=QColor(0,0,255);
    color5UTR=QColor(142,207,201);
    color3UTR=QColor(255,190,122);
    colorCDS=QColor(0,0,255);
    baseA = QColor(16, 150, 72);
    baseT = QColor(214, 40, 57);
    baseC = QColor(37, 92, 153);
    baseG = QColor(247, 179, 43);
    baseN = QColor(100, 100, 100);
    base_ = QColor(52, 52, 52);

    colorHP = QColor(0,0,196);
    colorLP = QColor(150,150,0);
    colorDP = QColor(196,0,0);
    colorTP = QColor(0,0,0);
}

void BSAIndexScatterWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glPointSize(2);
    glEnable(GL_POINT_SMOOTH);

    glGenBuffers(1,&idPointsVBOH);
    glGenBuffers(1,&idPointsVBOL);
    glGenBuffers(1,&idPointsVBOD);
    glGenBuffers(1,&idPointsVBOT1);
    glGenBuffers(1,&idPointsVBOT2);
    glGenVertexArrays(1,&idPointsVAOH);
    glGenVertexArrays(1,&idPointsVAOL);
    glGenVertexArrays(1,&idPointsVAOD);
    glGenVertexArrays(1,&idPointsVAOT1);
    glGenVertexArrays(1,&idPointsVAOT2);

    glGenBuffers(1,&idMarkersVBOH);
    glGenBuffers(1,&idMarkersVBOL);
    glGenBuffers(1,&idMarkersVBOD);
    glGenVertexArrays(1,&idMarkersVAOH);
    glGenVertexArrays(1,&idMarkersVAOL);
    glGenVertexArrays(1,&idMarkersVAOD);

    qDebug() << "nMarkers" << nMarkers;

    glBindVertexArray(idPointsVAOH);
    glBindBuffer(GL_ARRAY_BUFFER,idPointsVBOH);
    glBufferData(GL_ARRAY_BUFFER,2*nPoints*sizeof(GLfloat),NULL,GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),(void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(idPointsVAOL);
    glBindBuffer(GL_ARRAY_BUFFER,idPointsVBOL);
    glBufferData(GL_ARRAY_BUFFER,2*nPoints*sizeof(GLfloat),NULL,GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),(void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(idPointsVAOD);
    glBindBuffer(GL_ARRAY_BUFFER,idPointsVBOD);
    glBufferData(GL_ARRAY_BUFFER,2*nPoints*sizeof(GLfloat),NULL,GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),(void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(idPointsVAOT1);
    glBindBuffer(GL_ARRAY_BUFFER,idPointsVBOT1);
    glBufferData(GL_ARRAY_BUFFER,2*nPoints*sizeof(GLfloat),NULL,GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),(void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(idPointsVAOT2);
    glBindBuffer(GL_ARRAY_BUFFER,idPointsVBOT2);
    glBufferData(GL_ARRAY_BUFFER,2*nPoints*sizeof(GLfloat),NULL,GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),(void*)0);
    glEnableVertexAttribArray(0);


    glBindVertexArray(idMarkersVAOH);
    glBindBuffer(GL_ARRAY_BUFFER,idMarkersVBOH);
    glBufferData(GL_ARRAY_BUFFER,2*nMarkers*sizeof(GLfloat),NULL,GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),(void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(idMarkersVAOL);
    glBindBuffer(GL_ARRAY_BUFFER,idMarkersVBOL);
    glBufferData(GL_ARRAY_BUFFER,2*nMarkers*sizeof(GLfloat),NULL,GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),(void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(idMarkersVAOD);
    glBindBuffer(GL_ARRAY_BUFFER,idMarkersVBOD);
    glBufferData(GL_ARRAY_BUFFER,2*nMarkers*sizeof(GLfloat),NULL,GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),(void*)0);
    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    shaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,":/shader/BSAPointsVertex.shader");
    shaderProgram.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,":/shader/BSAPointsFragment.shader");
    shaderProgram.link();

    paintPointsToBuffer();
    paintMarkersToBuffer();
    adjustPaintChri();

}

void BSAIndexScatterWidget::paintGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    QVector3D pointColor;

    GLfloat leftMargin = 2.0*widgetLeftMargin/width()-1.0;

    QPainter Painter;
    Painter.begin(this);

    paintVerticalLineInPainter(&Painter);
    paintAxisInPainter(&Painter);
    if(genome&&geneDisplayMode) paintGenesInPainter(&Painter);

    if(isLeftMousePressed) paintZoomRegionLineInPainter(&Painter);

    Painter.end();


    if(currentLeftPosF>=0&&currentRightPosF>=0&&nPaintPoints){
        shaderProgram.bind();

        shaderProgram.setUniformValue("iResolution",iResolution);
        shaderProgram.setUniformValue("leftMargin",leftMargin);

        glBindVertexArray(idPointsVAOH);
        pointColor=QVector3D(colorHP.redF(),colorHP.greenF(),colorHP.blueF());
        shaderProgram.setUniformValue("pColor",pointColor);
        glDrawArrays(GL_LINE_STRIP,0,nPaintPoints);

        glBindVertexArray(idPointsVAOL);
        pointColor=QVector3D(colorLP.redF(),colorLP.greenF(),colorLP.blueF());
        shaderProgram.setUniformValue("pColor",pointColor);
        glDrawArrays(GL_LINE_STRIP,0,nPaintPoints);

        glBindVertexArray(idPointsVAOD);
        pointColor=QVector3D(colorDP.redF(),colorDP.greenF(),colorDP.blueF());
        shaderProgram.setUniformValue("pColor",pointColor);
        glDrawArrays(GL_LINE_STRIP,0,nPaintPoints);

        glBindVertexArray(idPointsVAOT1);
        pointColor=QVector3D(colorTP.redF(),colorTP.greenF(),colorTP.blueF());
        shaderProgram.setUniformValue("pColor",pointColor);
        glDrawArrays(GL_LINE_STRIP,0,nPaintPoints);

        glBindVertexArray(idPointsVAOT2);
        pointColor=QVector3D(colorTP.redF(),colorTP.greenF(),colorTP.blueF());
        shaderProgram.setUniformValue("pColor",pointColor);
        glDrawArrays(GL_LINE_STRIP,0,nPaintPoints);


        shaderProgram.release();
        glBindVertexArray(0);

    }

    if(nPaintMarkers){
        shaderProgram.bind();

        shaderProgram.setUniformValue("iResolution",iResolution);
        shaderProgram.setUniformValue("leftMargin",leftMargin);

        glBindVertexArray(idMarkersVAOH);
        pointColor=QVector3D(colorHP.redF(),colorHP.greenF(),colorHP.blueF());
        shaderProgram.setUniformValue("pColor",pointColor);
        glDrawArrays(GL_POINTS,0,nPaintMarkers);

        glBindVertexArray(idMarkersVAOL);
        pointColor=QVector3D(colorLP.redF(),colorLP.greenF(),colorLP.blueF());
        shaderProgram.setUniformValue("pColor",pointColor);
        glDrawArrays(GL_POINTS,0,nPaintMarkers);

        glBindVertexArray(idMarkersVAOD);
        pointColor=QVector3D(colorDP.redF(),colorDP.greenF(),colorDP.blueF());
        shaderProgram.setUniformValue("pColor",pointColor);
        glDrawArrays(GL_POINTS,0,nPaintMarkers);

        shaderProgram.release();
        glBindVertexArray(0);
    }

    Painter.begin(this);

    paintLabelPoints(&Painter);
    if(selectedPointi.info>=0) paintSelectedPoint(&Painter);
    if(selectedGenei>=0) paintSelectedGene(&Painter);
    if(isDisplayGeneModel) paintGenesModelInPainter(&Painter);
    if(genomeSeq&&isDisplayGenomeSeq) paintGenomeSeq(&Painter);
//    if(selectedPointGenei>=0) paintSelectedPointGene(&Painter);

    Painter.end();
}

void BSAIndexScatterWidget::resizeGL(int w, int h)
{
    iResolution=QVector3D(w,h,0);
    paintPointsToBuffer();
    paintMarkersToBuffer();
    adjustPaintChri();
}

void BSAIndexScatterWidget::menuExec(const QPoint &pos)
{
    pMenu->exec(this->mapToGlobal(pos));
}



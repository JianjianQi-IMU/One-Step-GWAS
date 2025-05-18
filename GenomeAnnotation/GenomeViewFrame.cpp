#include "GenomeViewFrame.hpp"

#include <QFontMetrics>

GenomeViewFrame::GenomeViewFrame()
{
    widgetLeftMargin            = 100;
    widgetTopMargin             = 100;
    isDisplayMajorGridline      = true;
    isDisplayMinorGridline      = true;
    isDisplayChrGridLine        = true;
    isDisplayMovingPosLine      = true;
    backgroundColor             = QColor(0.85f*255, 0.95f*255, 0.8f*255);
    SlideZoomAreaColor          = QColor(211,211,240);
    movingPosLineColor          = QColor(0,0,0);
    positionLabelHeight         = 20;
    minIntervalBaseNum          = 5;

    tmpChrInfo                  = nullptr;
    tmpWidgetSize               = QSize();
    tmpCurrentLeftPosF          = 0.0;
    tmpCurrentRightPosF         = 0.0;

    positionGridFont.setFamily("Times New Roman");
    chrFont.setFamily("Times New Roman");
    chrFont.setBold(true);
    chrColor = QColor(Qt::darkGreen);
}

void GenomeViewFrame::setDisplayMajorGridline(bool flag)
{
    isDisplayMajorGridline = flag;
}

void GenomeViewFrame::setDisplayMinorGridline(bool flag)
{
    isDisplayMinorGridline = flag;
}

void GenomeViewFrame::setDisplayChrGridLine(bool flag)
{
    isDisplayChrGridLine = flag;
}

void GenomeViewFrame::setDisplayMovingPosLine(bool flag)
{
    isDisplayMovingPosLine = flag;
}

bool GenomeViewFrame::mouseXToPointX(int inMouseX1, int inMouseX2, long double &outLeftX, long double &outRightX)
{
    int width0=tmpWidgetSize.width();
    long double pos1,pos2;
    long double positionGapLen=tmpCurrentRightPosF-tmpCurrentLeftPosF;
    long long maxCumPos = tmpChrInfo -> getMaxCumPos();
    inMouseX1=inMouseX1<widgetLeftMargin?widgetLeftMargin:inMouseX1;
    inMouseX2=inMouseX2<widgetLeftMargin?widgetLeftMargin:inMouseX2;
    if(inMouseX1 == inMouseX2) return false;
    if(inMouseX1 > inMouseX2){
        pos1=double(inMouseX1-widgetLeftMargin)/(width0-widgetLeftMargin)*positionGapLen+tmpCurrentLeftPosF;
        pos2=double(inMouseX2-widgetLeftMargin)/(width0-widgetLeftMargin)*positionGapLen+tmpCurrentLeftPosF;
        if(pos1-pos2+1<minIntervalBaseNum) return false;
        outLeftX=pos2;
        outRightX=pos1;
    }
    else{
        pos1=double(inMouseX1-widgetLeftMargin)/(width0-widgetLeftMargin)*positionGapLen+tmpCurrentLeftPosF;
        pos2=double(inMouseX2-widgetLeftMargin)/(width0-widgetLeftMargin)*positionGapLen+tmpCurrentLeftPosF;
        if(pos2-pos1+1<minIntervalBaseNum) return false;
        outLeftX=pos1;
        outRightX=pos2;
    }
    if(outLeftX<1)
    {
        outLeftX=1;
    }
    if(outRightX>maxCumPos)
    {
        outRightX=maxCumPos;
    }
    return true;
}

void GenomeViewFrame::paintMovingPosLine(QPainter *painter)
{
    if(!isDisplayMovingPosLine || tmpMovingMouseX < widgetLeftMargin || tmpMovingMouseX > tmpWidgetSize.width())
    {
        return;
    }

    painter->save();

    int ypos = widgetTopMargin - positionLabelHeight;
    int lineLen = tmpWidgetSize.height() - ypos;
    long double mousePos = ((long double)(tmpMovingMouseX-widgetLeftMargin))/tmpWidgetSize.width();

    painter -> setBrush(movingPosLineColor);
    painter -> drawLine(tmpMovingMouseX,ypos,tmpMovingMouseX,ypos+lineLen);

    painter->restore();
}

void GenomeViewFrame::paintVerticalLineInPainter(QPainter *painter)
{
    QFontMetrics fm(positionGridFont);
    int width0=tmpWidgetSize.width(),height0=tmpWidgetSize.height();
    int posLabely=widgetTopMargin-positionLabelHeight;
    long long positionGapLen=tmpCurrentRightPosF-tmpCurrentLeftPosF;
    long long scatterVLineGap1=1,scatterVLineGap2=1;
    long double positionGapLenF=tmpCurrentRightPosF-tmpCurrentLeftPosF;
    long long i,j,k;
    long double tVLineX;
    int positionLabelWidth;
    QString posStr;

    int currentMinChri;
    int currentMaxChri;
    tmpChrInfo -> getCurrentChri(currentMinChri,currentMaxChri);
    long long* cumChrLen = tmpChrInfo -> getCumChrLen();


    while(positionGapLen) (positionGapLen/=10),scatterVLineGap1*=10;
    scatterVLineGap1=std::max(scatterVLineGap1,(long long)1000);
    scatterVLineGap1/=10;
    positionGapLen=tmpCurrentRightPosF-tmpCurrentLeftPosF;
    scatterVLineGap2=scatterVLineGap1/10;

    painter->save();

    painter->fillRect(QRectF(widgetLeftMargin,posLabely,width0-widgetLeftMargin,positionLabelHeight),QBrush(QColor("#bbbbbb")));

    painter->setPen("#eeeeee");
    painter->setFont(positionGridFont);

    if(isDisplayMinorGridline){
        i=((long long)tmpCurrentLeftPosF-cumChrLen[currentMinChri])
                -((long long)tmpCurrentLeftPosF-cumChrLen[currentMinChri])%scatterVLineGap2;
        k=i+cumChrLen[currentMinChri];
        j=currentMinChri+1;
        while((long double)k<=tmpCurrentRightPosF){
            if(j<=currentMaxChri&&k>(long long)cumChrLen[j]){
                i=(k-cumChrLen[j])-(k-cumChrLen[j])%scatterVLineGap2;
                k=i+cumChrLen[j];
                j++;
            }
            else{
                if((long double)k>=tmpCurrentLeftPosF){
                    tVLineX=(k-tmpCurrentLeftPosF)/(long double)(positionGapLenF)*(width0-widgetLeftMargin)+widgetLeftMargin;
                    painter->drawLine(tVLineX,widgetTopMargin,tVLineX,height0);
                }
                k+=scatterVLineGap2;
            }
        }
    }

    if(isDisplayMajorGridline){
        i=(tmpCurrentLeftPosF-cumChrLen[currentMinChri])
                -((long long)tmpCurrentLeftPosF-cumChrLen[currentMinChri])%scatterVLineGap1;
        k=i+cumChrLen[currentMinChri];
        j=currentMinChri+1;
        while((long double)k<=tmpCurrentRightPosF){
            if((long long)j<=currentMaxChri&&k>(long long)cumChrLen[j]){
                i=(k-cumChrLen[j])-(k-cumChrLen[j])%scatterVLineGap1;
                k=i+cumChrLen[j];
                j++;
            }
            else{
                if((long double)k>=tmpCurrentLeftPosF&&k-cumChrLen[j-1]){
                    posStr = getThousandthInt(k-cumChrLen[j-1]);
                    positionLabelWidth = fm.horizontalAdvance(posStr);
                    tVLineX=(k-tmpCurrentLeftPosF)/(long double)(positionGapLenF)*(width0-widgetLeftMargin)+widgetLeftMargin;
                    painter->setPen("#bbbbbb");
                    painter->drawLine(tVLineX,widgetTopMargin+1,tVLineX,height0);
                    painter->setPen("#000000");
                    painter->drawText(tVLineX-positionLabelWidth/2,
                                      posLabely,
                                      positionLabelWidth,
                                      positionLabelHeight,
                                      Qt::AlignCenter,
                                      posStr);
                }
                k+=scatterVLineGap1;
            }
        }
    }

    painter->restore();
}

void GenomeViewFrame::paintChrGridLineInPainter(QPainter *painter)
{
    if(!isDisplayChrGridLine) return;
    QFontMetrics fm(chrFont);
    int width0=tmpWidgetSize.width(),height0=tmpWidgetSize.height();
    long double tVLineX,tChrNameX,leftChrLineX,rightChrLineX;
    int posLabely=widgetTopMargin-positionLabelHeight;
    long double positionGapLenF=tmpCurrentRightPosF-tmpCurrentLeftPosF;
    int positionLabelWidth;
    QString chrStr;

    int currentMinChri;
    int currentMaxChri;
    tmpChrInfo -> getCurrentChri(currentMinChri,currentMaxChri);
    long long* cumChrLen = tmpChrInfo -> getCumChrLen();
    char** chrName = tmpChrInfo -> getChrName();
    int nChr = tmpChrInfo -> getNChr();
    long double maxCumPos = tmpChrInfo -> getMaxCumPos();

    painter->save();
    painter->setPen(chrColor);
    painter->setFont(chrFont);
    for(int i=currentMinChri+1;i<=currentMaxChri;++i){
        tVLineX=(cumChrLen[i]-tmpCurrentLeftPosF)/(long double)(positionGapLenF)*(width0-widgetLeftMargin)+widgetLeftMargin+1;
        painter->drawLine(tVLineX,widgetTopMargin,tVLineX,height0);

    }
    for(int i=0;i<nChr;++i){
        chrStr = chrName[i];
        positionLabelWidth = fm.horizontalAdvance(chrStr);
        leftChrLineX = (cumChrLen[i]-tmpCurrentLeftPosF)/(long double)(positionGapLenF)*(width0-widgetLeftMargin)+widgetLeftMargin+1;
        if(i == nChr - 1){
            rightChrLineX = (maxCumPos-tmpCurrentLeftPosF)/(long double)(positionGapLenF)*(width0-widgetLeftMargin)+widgetLeftMargin+1;
        } else {
            rightChrLineX = (cumChrLen[i+1]-tmpCurrentLeftPosF)/(long double)(positionGapLenF)*(width0-widgetLeftMargin)+widgetLeftMargin+1;
        }
        tChrNameX = leftChrLineX + .5*(rightChrLineX - leftChrLineX);
        if(tChrNameX > width0 || tChrNameX < widgetLeftMargin){
            continue;
        }
        painter->drawText(tChrNameX-positionLabelWidth/2,
                          posLabely,
                          positionLabelWidth,
                          positionLabelHeight,
                          Qt::AlignCenter,
                          chrStr);
    }
    painter->restore();
}

void GenomeViewFrame::paintZoomRegionLineInPainter(int leftMousePressedPosX,
                                                   int leftMouseMovingPosX,
                                                   QPainter *painter)
{
    QPen tPen;
    int height0=tmpWidgetSize.height();
    long long i,j;

    // if(!isLeftMousePressed) return;

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
        i=std::min(leftMousePressedPosX,leftMouseMovingPosX);
        j=std::max(leftMousePressedPosX,leftMouseMovingPosX);
        painter->fillRect(QRectF(i,1,j-i+1,widgetTopMargin),QBrush(QColor(255,0,0,160)));
    }

    painter->restore();
}

void GenomeViewFrame::paintBackground(QPainter *painter)
{
    painter->save();

    int width0=tmpWidgetSize.width(),height0=tmpWidgetSize.height();
    QColor penCol;
    penCol.setRedF(SlideZoomAreaColor.redF()*0.8);
    penCol.setGreenF(SlideZoomAreaColor.greenF()*0.8);
    penCol.setBlueF(SlideZoomAreaColor.blueF()*0.8);

    painter->setBrush(Qt::white);
    painter->setPen(QPen(Qt::transparent,1));

    painter->drawRect(widgetLeftMargin,widgetTopMargin,
                      width0-widgetLeftMargin,height0-widgetTopMargin);

    painter->setBrush(SlideZoomAreaColor);

    painter->drawRect(widgetLeftMargin,0,
                      width0-widgetLeftMargin,widgetTopMargin);

    painter->setFont(QFont("Hack",32));

    painter->setPen(QPen(penCol));
    if(width0-widgetLeftMargin > 300){
        painter->drawText(widgetLeftMargin,0,
                          width0-widgetLeftMargin,widgetTopMargin,
                          Qt::AlignHCenter|Qt::AlignVCenter,
                          "Slide Zoom Area");
    }

    int gradW=widgetLeftMargin*0.3;
    QLinearGradient linearTop(QPoint(widgetLeftMargin,0),QPoint(widgetLeftMargin-gradW,0));
    QLinearGradient linearMain(QPoint(widgetLeftMargin,0),QPoint(widgetLeftMargin-gradW,0));
    QRect rectTop(widgetLeftMargin-gradW,0,gradW,widgetTopMargin);
    QRect rectMain(widgetLeftMargin-gradW,widgetTopMargin,gradW,height0-widgetTopMargin);
    linearTop.setColorAt(0,SlideZoomAreaColor);
    linearTop.setColorAt(1,backgroundColor);
    linearMain.setColorAt(0,Qt::white);
    linearMain.setColorAt(1,backgroundColor);
    painter -> setPen(QPen(Qt::transparent,1));
    painter -> setBrush(linearTop);
    painter -> drawRect(rectTop);
    painter -> setBrush(linearMain);
    painter -> drawRect(rectMain);

    painter->restore();
}

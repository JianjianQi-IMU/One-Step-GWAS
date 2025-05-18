#ifndef GENOMEVIEWFRAME_HPP
#define GENOMEVIEWFRAME_HPP

#include <QColor>
#include <QPainter>
#include <QSize>


#include "ChromosomeInfoClass.hpp"

class GenomeViewFrame
{
private:
    int               widgetLeftMargin;
    int               widgetTopMargin;
    bool              isDisplayMajorGridline;
    bool              isDisplayMinorGridline;
    bool              isDisplayChrGridLine;
    bool              isDisplayMovingPosLine;
    QColor            backgroundColor;
    QColor            SlideZoomAreaColor;
    QColor            movingPosLineColor;
    int               positionLabelHeight;
    long long         minIntervalBaseNum;
    QFont             positionGridFont;
    QFont             chrFont;
    QColor            chrColor;

    ChromosomeInfoClass* tmpChrInfo;
    QSize                tmpWidgetSize;
    long double          tmpCurrentLeftPosF;
    long double          tmpCurrentRightPosF;
    int                  tmpMovingMouseX;
public:
    GenomeViewFrame();
    void setDisplayMajorGridline(bool flag); // Frame
    void setDisplayMinorGridline(bool flag); // Frame
    void setDisplayChrGridLine(bool flag); // Frame
    void setDisplayMovingPosLine(bool flag);
    int  getWidgetLeftMargin() const
    {
        return widgetLeftMargin;
    }
    int  getWidgetTopMargin() const
    {
        return widgetTopMargin;
    }
    QColor getBackgroundColor() const
    {
        return backgroundColor;
    }
    long long getMinIntervalBaseNum() const
    {
        return minIntervalBaseNum;
    }
    bool getDisplayMovingPosLine() const
    {
        return isDisplayMovingPosLine;
    }

    void preSetMovingMouseX(int xpos)
    {
        tmpMovingMouseX = xpos;
    }
    void preSetChrInfo(ChromosomeInfoClass* inChrInfo)
    {
        tmpChrInfo = inChrInfo;
    }
    void preSetWidgetSize(const QSize& inWidgetSize)
    {
        tmpWidgetSize = inWidgetSize;
    }
    void preSetCurrentPosF(const long double& inCurrentLeftPosF, const long double& inCurrentRightPosF)
    {
        tmpCurrentLeftPosF  = inCurrentLeftPosF;
        tmpCurrentRightPosF = inCurrentRightPosF;
    }

    void paintGenomeViewFrame(bool isLeftMousePressed, int leftMousePressedPosX, int leftMouseMovingPosX,
                              QPainter *painter)
    {
        paintBackground(painter);
        paintVerticalLineInPainter(painter);
        paintChrGridLineInPainter(painter);
        if (isLeftMousePressed)
        {
            paintZoomRegionLineInPainter(leftMousePressedPosX, leftMouseMovingPosX, painter);
        }
    }
    bool mouseXToPointX(int inMouseX1, int inMouseX2,
                        long double& outLeftX, long double& outRightX);
    void paintMovingPosLine(QPainter *painter);

    void savePaintGenomeViewFrame(QPainter *painter)
    {
        paintVerticalLineInPainter(painter);
        paintChrGridLineInPainter(painter);
    }
private:
    void paintVerticalLineInPainter(QPainter *painter); // Frame
    void paintChrGridLineInPainter(QPainter *painter); // Frame
    void paintZoomRegionLineInPainter(int leftMousePressedPosX, int leftMouseMovingPosX, QPainter *painter); // Frame
    void paintBackground(QPainter *painter); // Frame
    QString getThousandthInt(long long x)
    {
        bool isNegetive = false;
        if (x < 0)
        {
            isNegetive = true;
            x = -x;
        }
        if (x == 0)
        {
            return "0";
        }
        QString out,tnum;
        while (x != 0)
        {
            tnum = QString::number(x % 1000);
            out = ',' + QString(3 - tnum.size(),'0') + tnum + out;
            x /= 1000;
        }
        while (!out.isEmpty() && (out[0] == '0' || out[0] == ','))
        {
            out.remove(0,1);
        }
        if (isNegetive)
        {
            out[0] = '-';
        }
        return out;
    }
};

#endif // GENOMEVIEWFRAME_HPP

#ifndef SHAPEPAINTER_FRAMEPAINTER_HPP
#define SHAPEPAINTER_FRAMEPAINTER_HPP

#include <QSize>
#include <QColor>
#include <QRect>
#include <QPainter>
#include "StyleChoose/LineStyleParam.hpp"

class FramePainter
{
protected:
    QRect                     rectPaint;
    int                       topBorder;
    int                       buttomBorder;
    int                       leftBorder;
    int                       rightBorder;
    float                     radius;
    QColor                    colorBackground;
    LineStyleParam            lineStyleParam;
    bool                      isPaint;
public:
    FramePainter():
        topBorder(10),
        buttomBorder(10),
        leftBorder(10),
        rightBorder(10),
        radius(6),
        isPaint(true)
    {
        colorBackground = Qt::transparent;
        lineStyleParam.lineStyle = Qt::SolidLine;
        lineStyleParam.lineColor = Qt::black;
        lineStyleParam.lineSize = 2;
    }
    void setIsPaint(bool flag){isPaint = flag;}
    void setPaintRect(const QRect& rect){rectPaint = rect;}
    void setPaintPoint(const QPoint& point)
    {
        rectPaint.setX(point.x());
        rectPaint.setY(point.y());
    }
    void setPaintSize(const QSize& size)
    {
        rectPaint.setWidth(size.width());
        rectPaint.setHeight(size.height());
    }
    void setTopBorder(int border){topBorder = border;}
    void setButtomBorder(int border){buttomBorder = border;}
    void setLeftBorder(int border){leftBorder = border;}
    void setRightBorder(int border){rightBorder = border;}
    void setRectRadius(float rad){radius = rad;}
    void setBorder(int left,int right,int top,int buttom)
    {
        topBorder    = top;
        buttomBorder = buttom;
        leftBorder   = left;
        rightBorder  = right;
    }
    void setBackgroundColor(const QColor& col){colorBackground = col;}

    void paintFrame(QPainter* painter)
    {
        if(isPaint == false)
        {
            return ;
        }
        QRect rect = QRect(rectPaint.x() - leftBorder,
                           rectPaint.y() - topBorder,
                           rectPaint.width() + leftBorder + rightBorder,
                           rectPaint.height() + topBorder + buttomBorder);
        painter -> save();
        painter -> setPen(QPen(lineStyleParam.lineColor,lineStyleParam.lineSize,lineStyleParam.lineStyle));
        painter -> setBrush(colorBackground);
        painter -> drawRoundedRect(rect,radius,radius);
        painter -> restore();
    }
};

#endif

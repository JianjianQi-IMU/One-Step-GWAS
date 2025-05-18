#ifndef LINESTYLEPARAM_HPP
#define LINESTYLEPARAM_HPP

#include <QColor>

class LineStyleParam
{
public:
    QColor           lineColor;
    int              lineSize;
    Qt::PenStyle     lineStyle;
    LineStyleParam()
    {
        lineColor = Qt::black;
        lineSize  = 1;
        lineStyle = Qt::SolidLine;
    }
    LineStyleParam(const QColor& col,
                   int size,
                   Qt::PenStyle penStyle)
    {
        lineColor = col;
        lineSize  = size;
        lineStyle = penStyle;
    }
};

#endif // LINESTYLEPARAM_HPP

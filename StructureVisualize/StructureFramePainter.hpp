#ifndef STRUCTUREFRAMEPAINTER_HPP
#define STRUCTUREFRAMEPAINTER_HPP

#include <QMatrix4x4>
#include <QColor>
#include <QPainter>
#include <QPointF>
#include <QVector4D>
#include "ShapePainter/FramePainter.hpp"

class StructureFramePainter : public FramePainter
{
public:
    StructureFramePainter():
        FramePainter()
    {}

    void calcuPaintRect(const QSize& frameSize,
                        const QMatrix4x4& matView,
                        const QMatrix4x4& matRotation,
                        bool isVertical)
    {
        QVector4D P1 = QVector4D(0,0,0,1);
        QVector4D P2 = QVector4D(1,1,0,1);
        P1 = matView*matRotation*P1;
        P2 = matView*matRotation*P2;
        QRect rect;
        if(isVertical){
            rect = QRect(P1.x()*frameSize.height(),
                         P1.y()*frameSize.width(),
                         (P2.x()-P1.x())*frameSize.height(),
                         (P2.y()-P1.y())*frameSize.width());
        }
        else{
            rect = QRect(P1.x()*frameSize.width(),
                         P1.y()*frameSize.height(),
                         (P2.x()-P1.x())*frameSize.width(),
                         (P2.y()-P1.y())*frameSize.height());
        }
        rectPaint = rect;
    }
};

#endif // STRUCTUREFRAMEPAINTER_HPP

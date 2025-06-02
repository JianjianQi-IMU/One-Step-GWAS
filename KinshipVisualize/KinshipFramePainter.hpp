#ifndef KINSHIPFRAMEPAINTER_HPP
#define KINSHIPFRAMEPAINTER_HPP

#include <QSize>
#include <QColor>
#include <QPainter>
#include <QVector4D>

#include "ShapePainter/FramePainter.hpp"

class KinshipFramePainter : public FramePainter
{
public:
    KinshipFramePainter():
        FramePainter()
    {

    }

    void calcuPaintRect(const QMatrix4x4& matView,
                        int frameSize)
    {
        QVector4D P1 = QVector4D(0, 0, 0, 1);
        QVector4D P2 = QVector4D(1, 1, 0, 1);
        P1 = matView * P1;
        P2 = matView * P2;
        rectPaint = QRect(P1.x() * frameSize, P1.y() * frameSize,
            (P2.x() - P1.x()) * frameSize, (P2.y() - P1.y()) * frameSize);
    }
};


#endif // KINSHIPFRAMEPAINTER_HPP

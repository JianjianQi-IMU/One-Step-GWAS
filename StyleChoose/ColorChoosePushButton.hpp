#ifndef COLORCHOOSEPUSHBUTTON_HPP
#define COLORCHOOSEPUSHBUTTON_HPP

#include <QPushButton>
#include <QPainter>
#include <algorithm>

class ColorChoosePushButton : public QPushButton
{
    Q_OBJECT
private:
    float         leftBorderFactor;
    float         rightBorderFactor;
    float         topBorderFactor;
    float         buttomBorderFactor;
    float         radiusFactor;
    QColor        colorChoose;
public:
    explicit ColorChoosePushButton(QWidget* parent = nullptr):
        QPushButton(parent)
    {
        colorChoose = Qt::white;
        leftBorderFactor = 0.2;
        rightBorderFactor = 0.2;
        topBorderFactor = 0.2;
        buttomBorderFactor = 0.2;
        radiusFactor = 0.05;
    }
    explicit ColorChoosePushButton(QColor initCol,QWidget* parent = nullptr):
        ColorChoosePushButton(parent)
    {colorChoose = initCol;}
    const QColor& getChooseColor() const{return colorChoose;}
public slots:
    void setChooseColor(QColor col)
    {
        colorChoose = col;
        update();
    }
private:
    void paintEvent(QPaintEvent *e) override
    {
        QPushButton::paintEvent(e);
        int width0 = width(), height0 = height();
        QRectF rect = QRectF(width0*leftBorderFactor,
                             height0*topBorderFactor,
                             width0*(1.0 - leftBorderFactor - rightBorderFactor),
                             height0*(1.0 - topBorderFactor - buttomBorderFactor));
        float radius = radiusFactor*std::min(width0, height0);
        QPainter Painter(this);
        Painter.begin(this);
        Painter.setBrush(colorChoose);
        Painter.setPen(Qt::black);
        Painter.drawRoundedRect(rect, radius, radius);
        Painter.end();
    }
};

#endif // COLORCHOOSEPUSHBUTTON_HPP

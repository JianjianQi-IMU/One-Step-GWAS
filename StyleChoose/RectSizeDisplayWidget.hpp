#ifndef RECTSIZEDISPLAYWIDGET_HPP
#define RECTSIZEDISPLAYWIDGET_HPP

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QBrush>
#include <QRectF>
#include <QFontMetrics>
#include <cmath>

#include "LineStyleParam.hpp"

class RectSizeDisplayWidget : public QWidget
{
    Q_OBJECT
private:
    int                    border;
    double                 baseCircleRadius;
    LineStyleParam lineStyleParam;
    QFont fontSizeText;
    QColor colorSizeText;

    QSizeF sizeDisplayF;
    QSize sizeDisplayI;
    bool isRealNum;
public:
    RectSizeDisplayWidget(QWidget* parent  = nullptr)
        : QWidget(parent)
    {
        border       = 36;
        baseCircleRadius = 120;
        lineStyleParam.lineColor = Qt::black;
        lineStyleParam.lineSize = 2;
        lineStyleParam.lineStyle = Qt::DashLine;
        fontSizeText.setFamily("Times New Roman");
        fontSizeText.setBold(true);
        colorSizeText = Qt::black;
        setMinSize();
        isRealNum = false;

        setStyleSheet("QWidget{background-color:#ffffff;}");
    }
    void setBorder(int val){border = val;setMinSize();}
    void setBaseCircleRadius(double val){baseCircleRadius = val;setMinSize();}
    void setRectLineStyleParam(const LineStyleParam& inParam)
    {
        lineStyleParam = inParam;
    }
    void setSizeTextFont(QFont inFont){fontSizeText = inFont;}
    void setSizeTextColor(QColor inColor){colorSizeText = inColor;}

    void setDisplayRectSizeF(QSizeF inSize){
        sizeDisplayF = inSize;
        isRealNum = true;
    }
    void setDisplayRectSizeI(QSize inSize){
        sizeDisplayI = inSize;
        isRealNum = false;
    }
private:
    void setMinSize()
    {
        setMinimumSize(border*2 + 2*baseCircleRadius,border*2 + 2*baseCircleRadius);
    }
    void paintEvent(QPaintEvent *event) override
    {
        QWidget::paintEvent(event);

        QFontMetrics fm(fontSizeText);
        QString hString;
        QString wString;
        QSizeF sizeDisplay;
        QPainter Painter;
        if(isRealNum){
            hString = QString::number(sizeDisplayF.height());
            wString = QString::number(sizeDisplayF.width());
            sizeDisplay = sizeDisplayF;
        }
        else{
            hString = QString::number(sizeDisplayI.height());
            wString = QString::number(sizeDisplayI.width());
            sizeDisplay = sizeDisplayI;
        }
        double x0 = width()*.5;
        double y0 = height()*.5;
        double rRadius = .5*std::sqrt(sizeDisplay.height()*sizeDisplay.height()
                                      + sizeDisplay.width()*sizeDisplay.width());
        double wRect = sizeDisplay.width()*baseCircleRadius/rRadius;
        double hRect = sizeDisplay.height()*baseCircleRadius/rRadius;
        QRectF rectDisplay = QRectF(x0 - .5*wRect,
                                    y0 - .5*hRect,
                                    wRect,
                                    hRect);

        int widthWString = fm.horizontalAdvance(wString);
        int widthHString = fm.horizontalAdvance(hString);
        int heightString = fm.height();
        int gapText = 4;
        Painter.begin(this);
        Painter.setPen(QPen(lineStyleParam.lineColor,
                            lineStyleParam.lineSize,
                            lineStyleParam.lineStyle));
        Painter.setBrush(Qt::transparent);
        Painter.drawRect(rectDisplay);
        Painter.setFont(fontSizeText);
        Painter.setPen(colorSizeText);
        Painter.drawText(x0 - .5*wRect + gapText,
                         y0 - .5*heightString,
                         widthHString,
                         heightString,
                         Qt::AlignHCenter|Qt::AlignRight,
                         hString);
        Painter.drawText(x0 - .5*widthWString,
                         y0 + .5*hRect + gapText,
                         widthWString,
                         heightString,
                         Qt::AlignVCenter|Qt::AlignTop,
                         wString);
        Painter.end();
    }
};

#endif // RECTSIZEDISPLAYWIDGET_HPP

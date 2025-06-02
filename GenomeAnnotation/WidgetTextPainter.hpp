#ifndef WIDGETTEXTPAINTER_HPP
#define WIDGETTEXTPAINTER_HPP

#include <QPoint>
#include <QSize>
#include <QString>
#include <QList>
#include <QColor>
#include <QFontMetrics>
#include <QFont>
#include <QPainter>

class WidgetTextPainter
{
private:
    QPoint              paintPoint;
    QSize               paintSize;
    QList<QString>      listText;
    QList<QColor>       listColor;
    QList<QFont>        listFont;
    QList<int>          listLineHeight;
    QColor              colorbackground;
    int                 leftBorder;
    int                 rightBorder;
    int                 topBorder;
    int                 buttomBorder;
    int                 lineSpace;
    float               angleRotation;
    bool                isPaintShadow;
    bool                isPaintRect;
public:
    WidgetTextPainter()
        : leftBorder(4), rightBorder(4), topBorder(6),
        buttomBorder(6), lineSpace(2), isPaintShadow(false), isPaintRect(true)
    {
        angleRotation = .0;
        colorbackground = Qt::white;
    }
    void setLineSpace(int val)
    {
        lineSpace = val;
        calcuFrameSize();
    }
    void setPaintPoint(const QPoint& pos)
    {
        paintPoint = pos;
    }
    void setTextList(const QList<QString>& list)
    {
        listText = list;
        calcuFrameSize();
    }
    void setColorList(const QList<QColor>& list)
    {
        listColor = list;
        calcuFrameSize();
    }
    void setFontList(const QList<QFont>& list)
    {
        listFont = list;
        calcuFrameSize();
    }
    void setLeftBorder(int border)
    {
        leftBorder = border;
        calcuFrameSize();
    }
    void setRightBorder(int border)
    {
        rightBorder = border;
        calcuFrameSize();
    }
    void setTopBorder(int border)
    {
        topBorder = border;
        calcuFrameSize();
    }
    void setButtomBorder(int border)
    {
        buttomBorder = border;
        calcuFrameSize();
    }
    void setBackgroundColor(QColor col)
    {
        colorbackground = col;
    }
    void setIsPaintShadow(bool flag)
    {
        isPaintShadow = flag;
    }
    void setIsPaintRect(bool flag)
    {
        isPaintRect = flag;
    }
    void setRotation(float angle)
    {
        angleRotation = angle;
    }
    void clear()
    {
        listText.clear();
        listColor.clear();
        listFont.clear();
    }
    QSize getSize() const
    {
        return paintSize;
    }
    void addText(const QString& str, const QColor& col, const QFont& font)
    {
        listText.append(str);
        listColor.append(col);
        listFont.append(font);
        calcuFrameSize();
    }
    void paintText(QPainter* painter)
    {
        int W = paintSize.width(), H = paintSize.height();
        int cumH = topBorder;
        painter -> save();
        preProcessPainter(painter);
        if (isPaintShadow) {
            painter->setBrush(QColor(190, 190, 190));
            painter->setPen(QPen(Qt::transparent));
            painter->drawRect(2, 2, W, H);
        }
        if (isPaintRect) {
            painter->setBrush(colorbackground);
            painter->setPen(QPen(Qt::black, 1));
            painter->drawRect(0, 0, W, H);
        }
        painter -> setBrush(Qt::transparent);
        for (int i = 0; i < listText.size(); ++i) {
            painter->setPen(QPen(listColor[i]));
            painter->setFont(listFont[i]);
            painter->drawText(leftBorder, cumH, W, listLineHeight[i],
                Qt::AlignLeft | Qt::AlignTop, listText[i]);
            cumH += listLineHeight[i];
        }
        painter->restore();
    }

private:
    void preProcessPainter(QPainter* painter)
    {
        painter->translate(paintPoint);
        painter->rotate(angleRotation);
    }
    void calcuFrameSize()
    {
        int w = 0, h = 0;
        int W = 0, H = 0;
        listLineHeight.clear();
        for (int i = 0; i < listText.size(); ++i) {
            QFontMetrics fm(listFont[i]);
            w = fm.horizontalAdvance(listText[i]);
            W = w < W ? W : w;
            h = fm.height();
            listLineHeight.append(h + lineSpace);
            H += h;
        }
        H += (listText.size() - 1) * lineSpace + topBorder + buttomBorder;
        W += leftBorder + rightBorder;
        paintSize = QSize(W, H);
    }
};

#endif // WIDGETTEXTPAINTER_HPP

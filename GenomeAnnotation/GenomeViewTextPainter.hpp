#ifndef GENOMEVIEWTEXTPAINTER_HPP
#define GENOMEVIEWTEXTPAINTER_HPP

#include <QPainter>
#include <QPoint>
#include <QVector>
#include <QString>
#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <algorithm>

class GenomeViewTextPainter
{
private:
    long double      displayGenomePos;
    int              displayYPos;
    QVector<QString> displayTextVec;
    QColor           displayBackgroundColor;
    QVector<QColor>  displayTextColorVec;
    QColor           displayFrameColor;
    int              displayTextLineSpace;
    int              displayFrameLineSize;
    int              displayFrameTopBorderWidth;
    int              displayFrameButtomBorderWidth;
    int              displayFrameLeftBorderWidth;
    int              displayFrameRightBorderWidth;
    QFont            displayTextFont;
    QSize            displayFontSize;
    QSize            displayFrameSize;
public:
    GenomeViewTextPainter():
        displayGenomePos(-1),
        displayYPos(-1),
        displayBackgroundColor(Qt::transparent),
        displayFrameColor(Qt::transparent),
        displayFrameLineSize(0),
        displayFrameTopBorderWidth(0),
        displayFrameButtomBorderWidth(0),
        displayFrameLeftBorderWidth(0),
        displayFrameRightBorderWidth(0)
    {

    }
    GenomeViewTextPainter(long double genomePos, int yPos, const QVector<QString>& textVec):
        GenomeViewTextPainter()
    {
        displayGenomePos = genomePos;
        displayYPos = yPos;
        displayTextVec = textVec;
        calcuPaintRect();
    }
    void setDisplayPos(long double genomePos, int yPos)
    {
        displayGenomePos = genomePos;
        displayYPos = yPos;
    }
    void setTextVec(const QVector<QString>& textVec)
    {
        displayTextVec = textVec;
    }
    void addText(const QString& text, const QColor& col)
    {
        displayTextVec.append(text);
        displayTextColorVec.append(col);
        calcuPaintRect();
    }
    void setBackgroundColor(const QColor& color)
    {
        displayBackgroundColor = color;
    }
    void setTextFont(const QFont& font)
    {
        displayTextFont = font;
    }
    void setTextColorVec(const QVector<QColor>& colorVec)
    {
        displayTextColorVec = colorVec;
    }
    void setTextLineSpace(int space)
    {
        displayTextLineSpace = space;
    }
    void setFrameColor(const QColor& color)
    {
        displayFrameColor = color;
    }
    void setFrameLineSize(int size)
    {
        displayFrameLineSize = size;
    }
    void setFrameTopBorderWidth(int width)
    {
        displayFrameTopBorderWidth = width;
    }
    void setFrameButtomBorderWidth(int width)
    {
        displayFrameButtomBorderWidth = width;
    }
    void setFrameLeftBorderWidth(int width)
    {
        displayFrameLeftBorderWidth = width;
    }
    void setFrameRightBorderWidth(int width)
    {
        displayFrameRightBorderWidth = width;
    }
    void paintText(QPainter* painter, const long double& currentLeftPosF, const long double& currentRightPosF,
                   const int& leftBorder, const int& rightBorder)
    {
        int textNum;
        QRect frameRect;
        int fh = displayFontSize.height(), fw = displayFontSize.width();
        int xpos, ypos;
        long double currentGapPos = currentRightPosF - currentLeftPosF;
        int gapBorder = rightBorder - leftBorder;

        if (currentLeftPosF > displayGenomePos || currentRightPosF < displayGenomePos) {
            return;
        }

        textNum = displayTextVec.size();
        if (textNum == 0) {
            return;
        }
        ypos = displayYPos;
        xpos = gapBorder * ((displayGenomePos - currentLeftPosF) / currentGapPos) + leftBorder;
        frameRect = QRect(xpos,ypos, fw + displayFrameLeftBorderWidth + displayFrameRightBorderWidth,
                          fh * textNum + displayFrameTopBorderWidth + displayFrameButtomBorderWidth);
        painter->save();
        painter->setFont(displayTextFont);
        painter->setBrush(displayBackgroundColor);
        painter->setPen(QPen(displayFrameColor,displayFrameLineSize));
        painter->drawRect(frameRect);
        painter->setBrush(Qt::transparent);
        for (int iText = 0; iText < textNum; ++iText) {
            painter->setPen(displayTextColorVec[iText]);
            painter->drawText(xpos + displayFrameLeftBorderWidth,
                              ypos + displayFrameTopBorderWidth + iText * fh, fw, fh,
                              Qt::AlignLeft | Qt::AlignTop, displayTextVec[iText]);
        }
        painter->restore();
    }
private:
    void calcuPaintRect()
    {
        int fh = 0,fw = 0;
        QFontMetrics fm(displayTextFont);
        int textNum = displayTextVec.size();
        if (textNum == 0) {
            return;
        }

        for (int iText = 0; iText < textNum; ++iText) {
            fw = std::max(fw, fm.horizontalAdvance(displayTextVec[iText]));
        }
        fh = displayTextLineSpace + fm.height();
        displayFontSize = QSize(fw, fh);
        displayFrameSize = QSize(fw + displayFrameLeftBorderWidth + displayFrameRightBorderWidth,
                                 fh * textNum + displayFrameTopBorderWidth + displayFrameButtomBorderWidth);
    }
};

class GenomeViewTextVectorPainter
{
private:
    long double tmpCurrentLeftPosF;
    long double tmpCurrentRightPosF;
    int tmpLeftBorder;
    int tmpRightBorder;
    QVector<GenomeViewTextPainter> paintVec;
public:
    GenomeViewTextVectorPainter()
    {
    }
    void addPaintItem(const GenomeViewTextPainter& item)
    {
        paintVec.append(item);
    }
    void removePaintItemAt(int iItem)
    {
        if (iItem < 0 || iItem >= paintVec.size()) {
            return;
        }
        paintVec.removeAt(iItem);
    }
    void clear()
    {
        paintVec.clear();
    }

    void preSetBorder(int leftBorder, int rightBorder)
    {
        tmpLeftBorder  = leftBorder;
        tmpRightBorder = rightBorder;
    }
    void preSetCurrentPos(long double currentLeftPosF, long double currentRightPosF)
    {
        tmpCurrentLeftPosF  = currentLeftPosF;
        tmpCurrentRightPosF = currentRightPosF;
    }
    void paintTextVec(QPainter* painter)
    {
        if (paintVec.size() == 0) {
            return;
        }
        int nItem = paintVec.size();
        for (int iItem = 0; iItem < nItem; ++iItem) {
            paintVec[iItem].paintText(painter, tmpCurrentLeftPosF, tmpCurrentRightPosF,
                                      tmpLeftBorder, tmpRightBorder);
        }
    }
};

#endif // GENOMEVIEWTEXTPAINTER_HPP

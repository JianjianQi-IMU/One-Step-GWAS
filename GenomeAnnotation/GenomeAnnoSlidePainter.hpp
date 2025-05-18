#ifndef GENOMEANNOSLIDEPAINTER_HPP
#define GENOMEANNOSLIDEPAINTER_HPP

#include <QColor>
#include <QPainter>
#include <QSize>
#include <QRect>

#include "ChromosomeInfoClass.hpp"

class GenomeAnnoSlidePainter
{
private:
    QRect                         barDisplayRect;
    QVector<QRectF>               barDisplayChrRectList;
    int                           heightChooseFrame;
    int                           widthChooseFrame;
    QVector<QColor>               colorListBar;
    QColor                        colorChooseFrame;
    QFont                         fontChrName;

    ChromosomeInfoClass*          tmpChrInfo;
    long double                   tmpCurrentLeftPosF;
    long double                   tmpCurrentRightPosF;
    QRectF                        tmpRect;
public:
    GenomeAnnoSlidePainter()
    {
        heightChooseFrame  = 0;
        widthChooseFrame   = 2;
        colorChooseFrame   = QColor(255, 0, 0);
        colorListBar.append(QColor(255, 127, 51, 0));
        fontChrName.setFamily("Times New Roman");
    }
    void setBarDisplayRect(const QRect& inRect)
    {
        barDisplayRect = inRect;
        calcuRect();
    }
    void setHeightChooseFrame(int inHeight)
    {
        heightChooseFrame = inHeight;
    }
    void setColorListBar(const QVector<QColor>& inList)
    {
        colorListBar = inList;
    }

    void preSetChrInfo(ChromosomeInfoClass* inChrInfo)
    {
        tmpChrInfo = inChrInfo;
    }
    void preSetCurrentPosF(const long double& inCurrentLeftPosF, const long double& inCurrentRightPosF)
    {
        tmpCurrentLeftPosF  = inCurrentLeftPosF;
        tmpCurrentRightPosF = inCurrentRightPosF;
        calcuRect();
    }
    bool isPressed(const QPoint& point)
    {
        int H = tmpRect.height();
        int W = tmpRect.width();
        int x0 = tmpRect.x(), y0 = tmpRect.y();
        int x = point.x();
        int y = point.y();
        if (x >= x0 && x <= x0 + W &&
           y >= y0 && y <= y0 + H)
        {
            return true;
        }
        return false;
    }
    void getNewCurrentPosF(int leftMoveMouse, long double& outCurrentLeftPosF,
                           long double& outCurrentRightPosF)
    {
        long long maxCumPos = tmpChrInfo -> getMaxCumPos();
        long double posFMove = (long double)(leftMoveMouse) * maxCumPos / barDisplayRect.width();
        if (posFMove > 0 && tmpCurrentRightPosF + posFMove > maxCumPos)
        {
            posFMove = maxCumPos - tmpCurrentRightPosF;
        } else if (tmpCurrentLeftPosF + posFMove < 1) {
            posFMove = 1 - tmpCurrentLeftPosF;
        }
        outCurrentLeftPosF  = tmpCurrentLeftPosF  + posFMove;
        outCurrentRightPosF = tmpCurrentRightPosF + posFMove;
    }
    void paintAnnoSlide(QPainter* painter)
    {
        paintBar(painter);
        paintChooseFrame(painter);
    }
private:
    void calcuRect()
    {
        barDisplayChrRectList.clear();
        long long maxCumPos = tmpChrInfo -> getMaxCumPos();
        long long leftPos, rightPos;
        int nChr = tmpChrInfo -> getNChr();
        long long* chrCumLen = tmpChrInfo -> getCumChrLen();
        int H = barDisplayRect.height();
        int W = barDisplayRect.width();
        int x0 = barDisplayRect.x(), y0 = barDisplayRect.y();
        int leftX = (tmpCurrentLeftPosF / maxCumPos) * W + x0;
        int rightX = (tmpCurrentRightPosF / maxCumPos) * W + x0;
        for (int idChr = 0; idChr < nChr; ++idChr) {
            leftPos = (chrCumLen[idChr] * 1.0 / maxCumPos) * W + x0;
            rightPos = (chrCumLen[idChr+1] * 1.0 / maxCumPos) * W + x0;
            barDisplayChrRectList.append(QRectF(leftPos, y0, rightPos - leftPos, H));
        }
        tmpRect = QRectF(leftX, y0 - (heightChooseFrame - H) / 2,
                    rightX - leftX, heightChooseFrame);
    }
    void paintBar(QPainter* painter)
    {
        painter->save();
        QColor tCol;
        int nChr = tmpChrInfo -> getNChr();
        char** chrName = tmpChrInfo -> getChrName();
        painter->setFont(fontChrName);
        for (int idChr = 0; idChr < nChr; ++idChr) {
            tCol = colorListBar[idChr % colorListBar.size()];
            painter->setBrush(tCol);
            painter->setPen(QPen(Qt::black, 2));
            painter->drawRoundedRect(barDisplayChrRectList[idChr], barDisplayRect.height() / 2.0, barDisplayRect.height() / 2.0);
            painter->setPen(QPen(Qt::black, 1));
            painter->drawText(barDisplayChrRectList[idChr], Qt::AlignVCenter | Qt::AlignHCenter, chrName[idChr]);
        }
        painter->restore();
    }
    void paintChooseFrame(QPainter* painter)
    {
        painter->save();
        painter->setPen(QPen(colorChooseFrame, widthChooseFrame));
        painter->setBrush(Qt::transparent);
        painter->drawRoundedRect(tmpRect, 3, 3);
        painter->restore();
    }
};

#endif // GENOMEANNOSLIDEPAINTER_HPP

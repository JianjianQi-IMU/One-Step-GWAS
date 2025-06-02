#ifndef GENOMEANNODISPLAYVIEW_HPP
#define GENOMEANNODISPLAYVIEW_HPP

#include <QFont>
#include <QColor>
#include <QVector>
#include <QRect>
#include <QPainter>
#include <QPainterPath>

#include "DataManager/MatrixLib.hpp"
#include "GenomeAnnotation.hpp"
#include "ChromosomeInfoClass.hpp"
#include "FileDeal/FileDeal.hpp"

enum GeneDisplayMode
{
    GENEDISPLAY_MODE1,
    GENEDISPLAY_MODE2,
};

const int GeneDisplayModeNum = 2;

class GenomeAnnoDisplayView
{
private:
    QRect                 displayArea;
    float                 geneDisplayYFactor;
    float                 geneModelDisplayYFactor;
    float                 baseDisplayYFactor;
    int                   geneModelHeight;
    int                   baseHeight;
    int                   displayAreaLabelGap;
    long long             geneDisplayMaxPosGapLen;
    GeneDisplayMode       geneDisplayMode;
    bool                  isDisplayGeneModel;
    bool                  isDisplayGene;
    bool                  isDisplayGenomeSeq;
    bool                  isDisplayAreaLabel;
    QFont                 displayAreaLabelFont;
    QFont                 geneNameFont;
    QFont                 geneInfoFont;
    QFont                 fontZoomInToLook;
    QColor                colorSelectedFrame;
    QColor                colorForwardGene;
    QColor                colorReverseGene;
    QColor                colorNullStrandGene;
    QColor                colorDefaultGene;
    QColor                color5UTR;
    QColor                color3UTR;
    QColor                colorCDS;
    QColor                baseA;
    QColor                baseT;
    QColor                baseC;
    QColor                baseG;
    QColor                baseN;
    QColor                base_;
    QString               textZoomInToLook;
    bool                  displaySelectedGeneInfo;
    GenomeAnnotation*     genome;
    GenomeSequence*       genomeSeq;

    long long             currentMinGenei;
    long long             currentMaxGenei;
    long long             nPaintGenes;

    long long             tmpSelectedPointGenei;
    long long             tmpSelectedGenei;
    ChromosomeInfoClass*  tmpChrInfo;
    long double           tmpCurrentLeftPosF;
    long double           tmpCurrentRightPosF;


public:
    GenomeAnnoDisplayView();
    ~GenomeAnnoDisplayView();
    bool loadGenomeSeq(const char* inFile); // update
    bool createGenome(); // GenomeAnno
    bool createGenome(char** inChrName,int inNChr, long long inNGenes); // GenomeAnno
    bool setGenome(const GenomeAnnotation& inGenome); // adjustPaintChri(); adjustPaintGenesi();

    inline bool isSelectedGene(long long idGene, int x, int y)
    {

        long long* cumChrLen = tmpChrInfo -> getCumChrLen();
        int xpos = displayArea.x(), ypos = displayArea.y();
        int width = displayArea.width(), height = displayArea.height();
        int painty0 = ypos + geneDisplayYFactor * height - 0.5 * geneModelHeight;
        int painty1 = painty0 + geneModelHeight - 1;
        long double positionGapLenF = tmpCurrentRightPosF - tmpCurrentLeftPosF;
        GeneAnnotation *pGene = (genome->getGene(idGene));
        long long tGeneStart = std::max((long double)(pGene->start + cumChrLen[pGene->idChr]), tmpCurrentLeftPosF);
        long long tGeneStop = std::min((long double)(pGene->stop + cumChrLen[pGene->idChr]), tmpCurrentRightPosF);
        int paintx0 = double(tGeneStart - tmpCurrentLeftPosF) / positionGapLenF * width + xpos;
        int paintl = std::max(int(double(tGeneStop - tGeneStart + 1) / positionGapLenF * width), 1);
        int paintx1 = paintx0 + paintl - 1;
        if (x >= paintx0 && x <= paintx1 && y >= painty0 && y <= painty1) {
            return true;
        }
        return false;
    }
    inline bool isClicked(const QPoint& pos)
    {
        int xpos = pos.x(), ypos = pos.y();
        if (xpos >= displayArea.x() && xpos <= displayArea.x() + displayArea.width() &&
           ypos >= displayArea.y() && ypos <= displayArea.y() + displayArea.height())
        {
            return true;
        }
        return false;
    }

    // false : -1 | true : clicked gene id
    inline long long isClickedGene(const QPoint& pos)
    {
        int xpos = pos.x(), ypos = pos.y();
        if (genome && nPaintGenes) {
            long long iGene = currentMinGenei;
            for (; iGene <= currentMaxGenei; ++iGene) {
                if (isSelectedGene(iGene, xpos, ypos)) break;
            }
            if (iGene <= currentMaxGenei) {
                return iGene;
            }
        }
        return -1;
    }
    inline GenomeAnnotation* getGenome()
    {
        return genome;
    } // GenomeAnno
    inline QColor getForwardGeneColor() const
    {
        return colorForwardGene;
    } // GenomeAnno
    inline QColor getReverseGeneColor() const
    {
        return colorReverseGene;
    } // GenomeAnno
    inline QColor getDefaultGeneColor() const
    {
        return colorDefaultGene;
    } // GenomeAnno
    inline QColor get5UTRColor() const
    {
        return color5UTR;
    } // GenomeAnno
    inline QColor get3UTRColor() const
    {
        return color3UTR;
    } // GenomeAnno
    inline QColor getCDSColor() const
    {
        return colorCDS;
    } // GenomeAnno
    inline long long getNGenes() const
    {
        if (genome) {
            return genome -> getNGenes();
        }
        return 0;
    }
    inline void switchGeneDisplayMode() //update;
    {
        geneDisplayMode = (GeneDisplayMode)(((int)geneDisplayMode + 1) % GeneDisplayModeNum);
    }
    inline void setDisplayGene(bool flag) //update;
    {
        isDisplayGene = flag;
    }
    inline void setDisplayGeneModel(bool flag)
    {
        isDisplayGeneModel = flag;
    } //update;
    inline void setDisplayGenome(bool flag)
    {
        isDisplayGenomeSeq = flag;
    } //update;
    inline void setForwardGeneColor(QColor inColor)
    {
        colorForwardGene = inColor;
    } //update;
    inline void setReverseGeneColor(QColor inColor)
    {
        colorReverseGene = inColor;
    } //update;
    inline void setDefaultGeneColor(QColor inColor)
    {
        colorDefaultGene = inColor;
    } //update;
    inline void set5UTRColor(QColor inColor)
    {
        color5UTR = inColor;
    } //update;
    inline void set3UTRColor(QColor inColor)
    {
        color3UTR = inColor;
    } //update;
    inline void setCDSColor(QColor inColor)
    {
        colorCDS = inColor;
    } //update;
    inline void setDisplaySelectedGeneInfo(bool flag)
    {
        displaySelectedGeneInfo = flag;
    }

    inline void preSetDisplayArea(const QRect& inDisplayArea)
    {
        displayArea = inDisplayArea;
    }
    inline void preSetSelectedPointGenei(long long inSelectedPointGenei)
    {
        tmpSelectedPointGenei = inSelectedPointGenei;
    }
    inline void preSetSelectedGenei(long long inSelectedGenei)
    {
        tmpSelectedGenei = inSelectedGenei;
    }
    inline void preSetChrInfo(ChromosomeInfoClass* inChrInfo)
    {
        tmpChrInfo = inChrInfo;
    }
    inline void preSetCurrentPosF(const long double& inCurrentLeftPosF, const long double& inCurrentRightPosF)
    {
        tmpCurrentLeftPosF  = inCurrentLeftPosF;
        tmpCurrentRightPosF = inCurrentRightPosF;
    }
    inline void preAdjustPaintGenesi()
    {
        adjustPaintGenesi();
    }

    inline void paintGenomeAnnoDisplayView(QPainter *painter)
    {
        if (genome)
        {
            if (isDisplayGene) {
                paintGenesInPainter(painter);
            }
            if (isDisplayGeneModel) {
                paintGenesModelInPainter(painter);
            }
            paintSelectedGene(painter);
            if (displaySelectedGeneInfo)
            {
                paintGeneInfo(painter);
            }
        }
        if (genomeSeq && isDisplayGenomeSeq)
        {
            paintGenomeSeq(painter);
        }
        if (isDisplayAreaLabel)
        {
            paintDisplayAreaLabel(painter);
        }
    }

    inline void savePaintGenomeAnnoDisplayView(QPainter *painter)
    {
        if (genome)
        {
            paintGenesInPainter(painter);
            paintGenesModelInPainter(painter);
            paintSelectedGene(painter);
            if (displaySelectedGeneInfo)
            {
                paintGeneInfo(painter);
            }
        }
        if (genomeSeq)
        {
            paintGenomeSeq(painter);
        }
        if (isDisplayAreaLabel)
        {
            paintDisplayAreaLabel(painter);
        }
    }
private:
    void adjustPaintGenesi();
    void paintGenesInPainter(QPainter *painter); // GenomeAnno
    void paintGenesModelInPainter(QPainter *painter); // GenomeAnno
    void paintSelectedGene(QPainter *painter); // GenomeAnno
    void paintGenomeSeq(QPainter *painter); // GenomeAnno
    void paintOneBase(char inBase, // GenomeAnno
                      int xPos,
                      int oneBaseLen,
                      QPainter *painter);
    void paintGeneInfo(QPainter *painter); // GenomeAnno
    void paintDisplayAreaLabel(QPainter *painter);
    void paintOneGeneMode1(QRect geneRect, char strand, QColor geneColor, QPainter *painter)
    {
        int triaW = 2 * geneRect.height();
        int rectW = geneRect.width() - triaW;
        double cenY = geneRect.y() + 0.5 * geneRect.height();
        QRect rect;
        QPolygonF triangle;
        QPainterPath trianglePath;
        QBrush geneBrush = QBrush(geneColor);
        bool isPaintTria = true;

        switch (strand) {
        case '+':{
            if (rectW > 0) {
                rect = QRect(geneRect.x(), geneRect.y(), rectW,geneRect.height());
                triangle.append(QPointF(geneRect.x() + rectW - 1, cenY - geneRect.height()));
                triangle.append(QPointF(geneRect.x() + rectW - 1, cenY + geneRect.height()));
                triangle.append(QPointF(geneRect.x() + geneRect.width(), cenY));
            } else {
                isPaintTria = false;
                rect = geneRect;
            }
        }
            break;
        case '-':{
            if (rectW > 0) {
                rect = QRect(geneRect.x() + triaW,geneRect.y(), rectW, geneRect.height());
                triangle.append(QPointF(geneRect.x() + triaW + 1, cenY - geneRect.height()));
                triangle.append(QPointF(geneRect.x() + triaW + 1, cenY + geneRect.height()));
                triangle.append(QPointF(geneRect.x(), cenY));
            } else {
                isPaintTria = false;
                rect = geneRect;
            }
        }
            break;
        default:{
            rect = geneRect;
            isPaintTria = false;
            }
            break;
        }

        trianglePath.addPolygon(triangle);

        painter -> save();

        painter->fillRect(rect,geneBrush);
        if (isPaintTria) {
            painter->fillPath(trianglePath, geneBrush);
        }

        painter -> restore();
    }
    void paintOneGeneMode2(QRect geneRect, char strand, QColor geneColor, QPainter *painter)
    {
        double geneTriangleMaxLen = geneModelHeight * std::sqrt(3) * 0.5;
        int paintW = geneRect.width();
        int paintH = geneRect.height();
        double cenY = geneRect.y() + 0.5 * geneRect.height();
        double triaW = 0.2 * paintW;
        double rectW;
        QRect rect;
        QPolygonF triangle;
        QPainterPath trianglePath;
        QBrush geneBrush = QBrush(geneColor);
        bool isPaintTria = true;

        triaW = std::min(triaW, geneTriangleMaxLen);
        rectW = paintW - triaW;

        switch (strand) {
        case '+':{
            if (paintW > 10) {
                rect = QRect(geneRect.x(), geneRect.y(), rectW, paintH);
                triangle.append(QPointF(geneRect.x() + rectW - 1, cenY - .5 * paintH));
                triangle.append(QPointF(geneRect.x() + rectW - 1, cenY + .5 * paintH));
                triangle.append(QPointF(geneRect.x() + geneRect.width(), cenY));
            } else {
                rect = geneRect;
                isPaintTria = false;
            }
        }
            break;
        case '-':{
            if(paintW > 10)
            {
                rect = QRect(geneRect.x() + triaW,geneRect.y(), rectW, paintH);
                triangle.append(QPointF(geneRect.x() + triaW + 1, cenY - .5 * paintH));
                triangle.append(QPointF(geneRect.x() + triaW + 1, cenY + .5 * paintH));
                triangle.append(QPointF(geneRect.x(), cenY));
            } else {
                rect = geneRect;
                isPaintTria = false;
            }
        }
            break;
        default:{
                rect = geneRect;
                isPaintTria = false;
            }
            break;
        }

        trianglePath.addPolygon(triangle);

        painter -> save();

        painter->fillRect(rect, geneBrush);
        if (isPaintTria) {
            painter->fillPath(trianglePath, geneBrush);
        }

        painter -> restore();
    }
};

#endif // GENOMEANNODISPLAYVIEW_HPP

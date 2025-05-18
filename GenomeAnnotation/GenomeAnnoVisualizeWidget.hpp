#ifndef GENOMEANNOVISUALIZEWIDGET_HPP
#define GENOMEANNOVISUALIZEWIDGET_HPP

#include <QMenu>
#include <QVector3D>

#include "BaseOpenGLPaintWidget.hpp"
#include "GenomeAnnoDisplayView.hpp"
#include "GenomeViewFrame.hpp"
#include "GenomeViewTextPainter.hpp"
#include "InsertTextEditorFrame.hpp"
#include "GenomeAnnoSlidePainter.hpp"

enum GenomeLeftMouseMode
{
    G_LEFT_DEFAULT = 0,
    G_LEFT_ZOOMOUT = 1,
};

class GenomeAnnoVisualizeWidget : public BaseOpenGLPaintWidget
{
    Q_OBJECT
protected:
    int                           zoomFactor; // basic
    float                         moveFactor; // basic
    long double                   currentLeftPosF; // basic
    long double                   currentRightPosF; // basic
    bool                          isLeftMousePressed; // basic
    bool                          isMiddleMousePressed; // basic
    bool                          isRightMousePressed; // basic
    int                           leftMousePressedPosX; // basic
    int                           leftMouseReleasedPosX; // basic
    int                           leftMouseMovingPosX; // basic
    int                           middleMousePressedPosX; // basic
    int                           middleMouseMovingPosX; // basic
    int                           RightMousePressedPosX; // basic
    int                           RightMousePressedPosY; // basic
    long double                   middleMousePressedLeftPosF; // basic
    long double                   middleMousePressedRightPosF; // basic
    GenomeLeftMouseMode           leftMousePressedMode;

    QMenu*                        pMenu; // basic
    GenomeAnnoDisplayView*        pAnnoPainter;
    GenomeViewFrame*              pFramePainter;
    ChromosomeInfoClass*          pChrInfo;
    GenomeViewTextVectorPainter*  pTextPainter;
public:
    explicit GenomeAnnoVisualizeWidget(QWidget *parent = nullptr);
    virtual ~GenomeAnnoVisualizeWidget();
    bool loadChromosome(long long *inChrLen,char** inChrName, int inNChr);
    bool loadChromosome(const std::vector<long long>& inChrLen, const std::vector<std::string>& inChrName);
    bool createGenome(char** inChrName, int inNChr, long long inNGenes); // GenomeAnno
    bool setGenome(const GenomeAnnotation& inGenome); // adjustPaintChri(); adjustPaintGenesi();
    bool loadGenomeSeq(const char* inFile); // update
    void zoomIn(); // basic
    void zoomOut(); // basic
    void moveLeft(); // basic
    void moveRight(); // basic
    void addTextFrame(const GenomeViewTextPainter& textInfo);
    void removeTextFrameAt(int iItem);

    void lookGene(long long idGene); // basic
    void lookInterval(int leftIdChr, // basic
                      int rightIdChr,
                      long double leftChrPos,
                      long double rightChrPos);
    void lookInterval(long double inCurrentLeftPosF,
                      long double inCurrentRightPosF);
    void getCurrentInterval(int& leftIdChr, long double& leftChrPos,
                              int& rightIdChr, long double& rightChrPos);
    void getCurrentInterval(long double& outCurrentLeftPosF, long double& outCurrentRightPosF);

    GenomeAnnotation* getGenome()
    {
        return pAnnoPainter->getGenome();
    } // GenomeAnno
    char** getChrName()
    {
        return pChrInfo->getChrName();
    } // Chromosome
    QColor getForwardGeneColor() const
    {
        return pAnnoPainter->getForwardGeneColor();
    } // GenomeAnno
    QColor getReverseGeneColor() const
    {
        return pAnnoPainter->getReverseGeneColor();
    } // GenomeAnno
    QColor getDefaultGeneColor() const
    {
        return pAnnoPainter->getDefaultGeneColor();
    } // GenomeAnno
    QColor get5UTRColor() const
    {
        return pAnnoPainter->get5UTRColor();
    } // GenomeAnno
    QColor get3UTRColor() const
    {
        return pAnnoPainter->get3UTRColor();
    } // GenomeAnno
    QColor getCDSColor() const
    {
        return pAnnoPainter->getCDSColor();
    } // GenomeAnno
    bool getDisplayMovingPosLine()
    {
        return pFramePainter -> getDisplayMovingPosLine();
    }

    void setDisplayMajorGridline(bool flag)
    {
        pFramePainter->setDisplayMajorGridline(flag);
        update();
    } // Frame
    void setDisplayMinorGridline(bool flag)
    {
        pFramePainter->setDisplayMinorGridline(flag);
        update();
    } // Frame
    void setDisplayChrGridLine(bool flag)
    {
        pFramePainter->setDisplayChrGridLine(flag);
        update();
    } // Frame
    void switchGeneDisplayMode()
    {
        pAnnoPainter->switchGeneDisplayMode();
        update();
    } // GenomeAnno
    void setDisplayGene(bool flag)
    {
        pAnnoPainter->setDisplayGene(flag);
        update();
    } // GenomeAnno
    void setDisplayGeneModel(bool flag)
    {
        pAnnoPainter->setDisplayGeneModel(flag);
        update();
    } // GenomeAnno
    void setDisplayGenome(bool flag)
    {
        pAnnoPainter->setDisplayGenome(flag);
        update();
    } // GenomeAnno
    void setForwardGeneColor(QColor inColor, bool isUpdate = true)
    {
        pAnnoPainter->setForwardGeneColor(inColor);
        if (isUpdate)
        {
            update();
        }
    } // GenomeAnno
    void setReverseGeneColor(QColor inColor, bool isUpdate = true)
    {
        pAnnoPainter->setReverseGeneColor(inColor);
        if (isUpdate)
        {
            update();
        }
    } // GenomeAnno
    void setDefaultGeneColor(QColor inColor, bool isUpdate = true)
    {
        pAnnoPainter->setDefaultGeneColor(inColor);
        if (isUpdate)
        {
            update();
        }
    } // GenomeAnno
    void set5UTRColor(QColor inColor, bool isUpdate = true)
    {
        pAnnoPainter->set5UTRColor(inColor);
        if (isUpdate)
        {
            update();
        }
    } // GenomeAnno
    void set3UTRColor(QColor inColor, bool isUpdate = true)
    {
        pAnnoPainter->set3UTRColor(inColor);
        if (isUpdate)
        {
            update();
        }
    } // GenomeAnno
    void setCDSColor(QColor inColor, bool isUpdate = true)
    {
        pAnnoPainter->setCDSColor(inColor);
        if (isUpdate)
        {
            update();
        }
    } // GenomeAnno
    virtual void setDisplayMovingPosLine(bool flag)
    {
        pFramePainter -> setDisplayMovingPosLine(flag);
        setMouseTracking(flag);
    }

protected:
    void leftMousePressEvent(QMouseEvent* e, bool &isNeedUpdate) override;
    void middleMousePressEvent(QMouseEvent* e, bool &isNeedUpdate) override;
    void leftMouseReleaseEvent(QMouseEvent* e, bool &isNeedUpdate) override;
    void rightMouseReleaseEvent(QMouseEvent* e, bool &isNeedUpdate) override;
    void middleMouseReleaseEvent(QMouseEvent* e, bool &isNeedUpdate) override;
    void leftMouseMoveEvent(QMouseEvent* e, bool &isNeedUpdate) override;
    void middleMouseMoveEvent(QMouseEvent* e, bool &isNeedUpdate) override;

    void mouseMoveEvent(QMouseEvent* e) override; // basic
    void wheelEvent(QWheelEvent* e) override; // basic

    void doZoomIn(bool &isNeedUpdate); // basic
    void doZoomOut(bool &isNeedUpdate); // basic
private:
    void initializeGL() override; // basic
    void paintGL() override; // basic
    void resizeGL(int w, int h) override; // basic

signals:
    void currentIntervalChanged(long double cumLeftPos, long double cumRightPos);
    void geneSelected(long long idGene);
};

#endif // GENOMEANNOVISUALIZEWIDGET_HPP

#ifndef GENOMEANNOSLIDEWIDGET_H
#define GENOMEANNOSLIDEWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QDebug>
#include "GenomeAnnoSlidePainter.hpp"

class GenomeAnnoSlideWidget : public QWidget
{
    Q_OBJECT
private:
    GenomeAnnoSlidePainter* pSlidePainter;
    ChromosomeInfoClass* pChrInfo;
    double chrHFactor;
    bool isLookIntervalRectDrag;
    QPoint pointLeftButtonMoved;
    int topBorder;
    int downBorder;
public:
    explicit GenomeAnnoSlideWidget(QWidget *parent = nullptr);
    ~GenomeAnnoSlideWidget();

    bool loadChromosome(long long *inChrLen, char** inChrName, int inNChr);
    bool loadChromosome(const std::vector<long long>& inChrLen,
                        const std::vector<std::string>& inChrName);


    void setColorListBar(const QVector<QColor>& inList)
    {
        pSlidePainter -> setColorListBar(inList);
        update();
    }
    void setCurrentPos(long double inCurrentLeftPosF, long double inCurrentRightPosF);

signals:
    void sendCurrentPosChanged(long double currentLeftPosF, long double currentRightPosF);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    void mousePressEvent(QMouseEvent* e) override
    {
        if (e->button() == Qt::LeftButton) {
            pointLeftButtonMoved = e->pos();
            if (pSlidePainter -> isPressed(e->pos())) {
                isLookIntervalRectDrag = true;
            }
        }
    }
    void mouseMoveEvent(QMouseEvent* e) override
    {
        if (isLookIntervalRectDrag) {
            int dX = e->pos().x() - pointLeftButtonMoved.x();
            long double tmpLeftCurrentPosF, tmpRightCurrentPosF;
            pSlidePainter -> getNewCurrentPosF(dX, tmpLeftCurrentPosF, tmpRightCurrentPosF);
            pSlidePainter -> preSetCurrentPosF(tmpLeftCurrentPosF, tmpRightCurrentPosF);
            emit sendCurrentPosChanged(tmpLeftCurrentPosF, tmpRightCurrentPosF);
            update();
        }
        pointLeftButtonMoved = e->pos();
    }
    void mouseReleaseEvent(QMouseEvent* e) override
    {
        if (e->button() == Qt::LeftButton) {
            isLookIntervalRectDrag = false;
        }
    }
};

#endif // GENOMEANNOSLIDEWIDGET_H

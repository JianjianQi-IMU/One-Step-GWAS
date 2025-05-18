#ifndef STRUCTUREBARPAINTER_HPP
#define STRUCTUREBARPAINTER_HPP

#include <QMatrix4x4>
#include <QColor>
#include <QString>
#include <QPainter>

#include "StructureSortPaintSeq.hpp"
#include "GenomeAnnotation/WidgetTextPainter.hpp"
#include "DataManager/MatrixLib.hpp"

class StructureBarPainter
{
private:
    QMatrix4x4                matView;
    QMatrix4x4                matRotation;
    QSizeF                    frameSize;
    QVector<long long>        paintSeq;
    QVector<QColor>           colorPopList;
    MML::Mat                  individualsData;
    MML::Mat                  individualsCumData;
    QVector<QString>          individualsName;
    float                     xBarAreaFactor;
    float                     yBarAreaFactor;
    float                     barGapFactor;
    bool                      isDisplayBarFrame;
    bool                      isVertical;
    bool                      isDisplaySelectedText;
    long long                 idSelectedInd;
    long long                 idSearchedInd;
    QColor                    colorSelected;
    QColor                    colorSearched;
    QColor                    colorSelectedText;
    QFont                     fontSelectedText;

    WidgetTextPainter                                                      textSelectedPainter;
    // for sort
    stdVectorSmall<double,long long>                                       transVectorMod;
    StructureSortPaintSeq<stdVectorSmall<double,long long>,long long>      sortPaintSeqMod;
public:
    StructureBarPainter();

    void loadIndData(const MML::Mat &inData, char **inName);
    void setFrameSize(QSizeF size){frameSize = size;}
    void setViewMat(const QMatrix4x4& inMat){matView = inMat;}
    void setRotationMat(const QMatrix4x4& inMat){matRotation = inMat;}
    void setColorPopList(const QVector<QColor>& colorList){colorPopList = colorList;}
    void setXBarAreaFactor(float val){xBarAreaFactor = val;}
    void setYBarAreaFactor(float val){yBarAreaFactor = val;}
    void setBarGapFactor(float val){barGapFactor = val;}
    void setIsDisplayBarFrame(bool flag){isDisplayBarFrame = flag;}
    void setIsVertical(bool flag){isVertical = flag;}
    void setSelectedInd(long long idPaint){
        if(idPaint < 0 || idPaint >= (long long)individualsData.getNRow()){
            return;
        }
        idSelectedInd = idPaint;
    }
    void setSearchedInd(long long idPaint){
        if(idPaint < 0 || idPaint >= (long long)individualsData.getNRow()){
            return;
        }
        idSearchedInd = idPaint;
    }
    void setIsDisplaySelectedText(bool flag){isDisplaySelectedText = flag;}

    QMatrix4x4 getViewMat() const{return matView;}
    QMatrix4x4 getRotationMat() const{return matRotation;}
    const QVector<QColor>& getColorPopList() const{return colorPopList;}
    float getXBarAreaFactor() const{return xBarAreaFactor;}
    float getYBarAreaFactor() const{return yBarAreaFactor;}
    float getBarGapFactor() const{return barGapFactor;}
    bool getIsDisplayBarFrame() const{return isDisplayBarFrame;}
    bool getIsVertical() const{return isVertical;}
    long long getNIndividuals() const{return individualsData.getNRow();}
    long long getIdPaintSeq(long long idPaint) const{return paintSeq[idPaint];}

    long long isClickedInd(const QPoint& p);
    void reversePaintSeq() {
        QVector<long long> tmp;
        int size = paintSeq.size();
        for (int i = 0; i < paintSeq.size(); ++i) {
            tmp.push_back(paintSeq[size - i - 1]);
        }
        paintSeq = tmp;
    }
    inline void sortPaintSeq(){
        long long nPop = individualsData.getNCol(),iPop;
        long long i,n = individualsData.getNRow();
        long long iMaxPop = 0;
        double val;
        QVector<long long> outList;
        std::vector<long long> idList,idPopList;
        std::vector<double> valList;

        transVectorMod.setVector(&valList);
        for(i = 0;i < n;++i){
            iMaxPop = n;
            val = -1;;
            for(iPop = 0;iPop < nPop;++iPop){
                if(val < 0||individualsData(i,iPop) > val){
                    val = individualsData(i,iPop);
                    iMaxPop = iPop;
                }
            }
            idPopList.push_back(iMaxPop);
            valList.push_back(val);
        }
        for(iPop = 0;iPop < nPop;++iPop){
            idList.clear();
            for(i = 0;i < n;++i){
                if(idPopList[i] == iPop){
                    idList.push_back(i);
                }
            }
            sortPaintSeqMod.sortUp(0,idList.size()-1,idList);
            for(i = 0;i < (long long)idList.size();++i){
                outList.append(idList[i]);
            }
        }
        paintSeq = outList;
        reversePaintSeq();
    }
    inline QRectF calcuBarPosF01(long long idPaint,int idCov = -1){
        if(idPaint < 0 || idPaint >= (long long)individualsData.getNRow()){
            return QRectF();
        }
        long long id = paintSeq[idPaint];
        size_t nSample=individualsData.getNRow();
        float yMarginFactor=1-yBarAreaFactor,xMarginFactor=1-xBarAreaFactor;
        float y0=yMarginFactor/2,x0=xMarginFactor/2;
        float barHeight=yBarAreaFactor;
        float paintHeight=yBarAreaFactor;
        float barGap=barGapFactor*xBarAreaFactor/(nSample-barGapFactor);
        float barWidth=(1-barGapFactor)*xBarAreaFactor/(nSample-barGapFactor);
        QVector4D P1,P2;

        if(idCov >=0 && idCov < (long long)individualsData.getNCol())
        {
            y0 += barHeight*(individualsCumData(id,idCov)-individualsData(id,idCov));
            paintHeight = barHeight*individualsData(id,idCov);
        }
        P1=QVector4D(x0+idPaint*(barGap+barWidth),y0,
                     0,1);
        P2=QVector4D(x0+idPaint*(barGap+barWidth)+barWidth,y0+paintHeight,
                     0,1);
        P1=matView*matRotation*P1;
        P2=matView*matRotation*P2;
        return QRectF(P1.x(),
                      P1.y(),
                      (P2.x()-P1.x()),
                      (P2.y()-P1.y()));
    }
    inline QRectF calcuBarPosPix(long long idPaint,int idCov = -1){
        if(idPaint < 0 || idPaint >= (long long)individualsData.getNRow()){
            return QRect();
        }
        QRectF rect = calcuBarPosF01(idPaint, idCov);
        QRectF out;
        if(isVertical){
            out = QRectF(rect.x()*frameSize.height(),
                         rect.y()*frameSize.width(),
                         rect.width()*frameSize.height(),
                         rect.height()*frameSize.width());
        }
        else{
            out = QRectF(rect.x()*frameSize.width(),
                         rect.y()*frameSize.height(),
                         rect.width()*frameSize.width(),
                         rect.height()*frameSize.height());
        }
        return out;
    }
    inline void paintStart(QPainter* painter)
    {
        paintBar(painter);
        paintSearchedBar(painter);
        paintSelectedBar(painter);
    }
    inline void savePaintBar(QPainter* painter){
        paintBar(painter);
    }
private:
    inline void paintSelectedBar(QPainter* painter)
    {
        if(idSelectedInd < 0 || idSelectedInd >= (long long)individualsData.getNRow()){
            return ;
        }
        long long id = paintSeq[idSelectedInd];
        QRectF tRectF = calcuBarPosPix(idSelectedInd);
        painter -> save();
        painter -> setPen(QPen(colorSelected,2,Qt::DashLine));
        painter -> setBrush(Qt::transparent);
        painter -> drawRect(tRectF);
        painter -> restore();
        if(isDisplaySelectedText){
            textSelectedPainter.clear();
            textSelectedPainter.addText(individualsName[id],colorSelectedText,fontSelectedText);
            if(isVertical){
                textSelectedPainter.setPaintPoint(QPoint(tRectF.x()+tRectF.width(),tRectF.y()));
            }
            else{
                textSelectedPainter.setPaintPoint(QPoint(tRectF.x(),tRectF.y()+tRectF.height()));
            }
            textSelectedPainter.paintText(painter);
        }
    }
    inline void paintSearchedBar(QPainter* painter)
    {
        if(idSearchedInd < 0 || idSearchedInd >= (long long)individualsData.getNRow()){
            return ;
        }
        long long id = paintSeq[idSearchedInd];
        QRectF tRectF = calcuBarPosPix(idSearchedInd);
        painter -> save();
        painter -> setPen(QPen(colorSearched,2,Qt::DashDotLine));
        painter -> setBrush(Qt::transparent);
        painter -> drawRect(tRectF);
        painter -> restore();
    }
    inline void paintBar(QPainter* painter)
    {
        long long nSample = individualsData.getNRow();
        int nCov = individualsData.getNCol();
        QColor tmpColor;
        QRectF tRect;
        long long id;
        painter -> save();
        for(long long i=0;i<nSample;++i){
            id = paintSeq[i];
            for(int j=0;j<nCov;++j){
                if(j>=colorPopList.size()) tmpColor=colorPopList.back();
                else tmpColor=colorPopList[j];
                tRect=calcuBarPosPix(id,j);
                painter->fillRect(tRect,QBrush(tmpColor));
            }
        }
        painter -> restore();
    }

};

#endif // STRUCTUREBARPAINTER_HPP

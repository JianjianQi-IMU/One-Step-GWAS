#ifndef PCA3DSCATTERPOINTSPAINTER_HPP
#define PCA3DSCATTERPOINTSPAINTER_HPP

#include <QWidget>
#include <QPainter>
#include <QPainterPath>

#include "GenomeAnnotation/BaseOpenGLPaintWidget.hpp"
#include "GenomeAnnotation/WidgetTextPainter.hpp"

class PCA3DScatterPointsPainter
{
protected:
    QSize                    paintSize;
    BaseOpenGLPaintWidget*   tmpPaintWidget;
    float                    selectedPointsDistance2; ////
    GLuint                   pointsVAO; // //
    GLuint                   pointsVBO; // //
    char**                   sampleName; // //
    double*                  sampleData; // //
    long long                nSample; // //
    GLfloat*                 pointsData; // //
    long long*               popIndex; // //
    long long                selectedSampleIndex; // //
    long long                searchedSampleIndex; // //
    char**                   popName; // //
    long long                nPop; // //
    QColor                   colorSearchedPoints;
    QColor                   colorSelectedPoints;
    QColor                   colorSelectedNameText;
    QColor                   colorSelectedPopText;
    QFont                    fontSelectedText;
    WidgetTextPainter        selectedTextPainter;
    float                    pointSize;

    QMatrix4x4 matModel;      // model transform
    QMatrix4x4 matProjection; // projection transform
    QMatrix4x4 matView;       // view transform

    double minPC1; //
    double minPC2; //
    double minPC3; //
    double maxPC1; //
    double maxPC2; //
    double maxPC3; //

    QVector<QColor> colorPopList;

    int idPointsShaderProgram;
public:
    explicit PCA3DScatterPointsPainter();
    ~PCA3DScatterPointsPainter();

    void setPaintWidget(BaseOpenGLPaintWidget* inWidget)
    {
        tmpPaintWidget = inWidget;
    }
    bool setIndividualsData(const double* inData,
                       const long long* inPopIndex,
                       char** inIndividualsName,
                       long long inNIndividuals);
    bool setPopName(char** inPopName,long long inNPop);
    void setSelectedSampleIndex(long long idPaint)
    {
        selectedSampleIndex = idPaint;
    }
    void setSearchedSampleIndex(long long idPaint)
    {
        searchedSampleIndex = idPaint;
    }
    void setSearchedPointsColor(const QColor& col)
    {
        colorSearchedPoints = col;
    }
    void setSelectedPointsColor(const QColor& col)
    {
        colorSelectedPoints = col;
    }
    void setColorPopList(const QVector<QColor>& inColVec,bool isAdjustPaintColor = true)
    {
        colorPopList = inColVec;
        if(isAdjustPaintColor){
            adjustPaintColor();
        }
    }
    void setTransformMat(const QMatrix4x4& inMatModel,
                         const QMatrix4x4& inMatProjection,
                         const QMatrix4x4& inMatView)
    {
        matModel      = inMatModel;
        matProjection = inMatProjection;
        matView       = inMatView;
    }
    void setPaintSize(QSize inSize) {
        paintSize = inSize;
    }
    void setPointSize(float val) {
        pointSize = val;
    }

    float getPointSize() {
        return pointSize;
    }
    const QVector<QColor>& getColorPopList() const
    {
        return colorPopList;
    }

    double getMinPC1() const {return minPC1;}
    double getMinPC2() const {return minPC2;}
    double getMinPC3() const {return minPC3;}
    double getMaxPC1() const {return maxPC1;}
    double getMaxPC2() const {return maxPC2;}
    double getMaxPC3() const {return maxPC3;}
    void calcuAxisData(short iPC, double &outStart, double &outStride);

    void initInitializeGL();
    long long isClickedPoint(const QPoint& pos){
        long long i;
        for(i = 0;i < nSample;++i){
            if(isSelectedPoint(i,pos.x(),pos.y())) break;
        }
        if(i < nSample) return i;
        return -1;
    }

    void paintStart(QPainter* painter){
        painter->beginNativePainting();

        sendPaintPointsToBuffer();
        tmpPaintWidget->shaderProgramBind(idPointsShaderProgram);
        tmpPaintWidget->setUniformValue("model",matModel,idPointsShaderProgram);
        tmpPaintWidget->setUniformValue("projection",matProjection,idPointsShaderProgram);
        tmpPaintWidget->setUniformValue("view",matView,idPointsShaderProgram);

        if(nSample!=0){
            tmpPaintWidget->glPointSize(pointSize);
            tmpPaintWidget->glBindVertexArray(pointsVAO);
            tmpPaintWidget->glDrawArrays(GL_POINTS,0,nSample);
            tmpPaintWidget->glPointSize(1);
        }
        tmpPaintWidget->glBindVertexArray(0);

        tmpPaintWidget->shaderProgramRelease(idPointsShaderProgram);

        painter -> endNativePainting();

        paintSearchedPoint(painter);
        paintSelectedPoint(painter);
        paintSelectedPointText(painter);
    }
    void savePaintPoints(QPainter* painter){
        long long i;
        QPointF pxy;
        int W = paintSize.width();
        int H = paintSize.height();
        QColor tColor;
        painter -> save();
        for(i = 0; i < nSample; ++i){
            pxy = calcuPointPaint(i);
            tColor = colorPopList[popIndex[i]];
            if(pxy.x() < W && pxy.x() >= 0
               && pxy.y() < H && pxy.y() >= 0)
            {
                painter -> setPen(QPen(tColor,1.5*pointSize,Qt::SolidLine,Qt::RoundCap));
                painter -> drawPoint(pxy);
            }
        }
        painter -> restore();
    }
private:
    void delDeleteGL();
    bool isSelectedPoint(long long idPoint, int x, int y)
    {
        double dx,dy;
        QPointF pxy = calcuPointPaint(idPoint);
        dx=pxy.x()-x,dy=pxy.y()-y;
        if(dx*dx + dy*dy <= selectedPointsDistance2){
            return true;
        }
        return false;
    }
    QPointF calcuPointPaint(long long idPoint)
    {
        int height0=paintSize.height(),width0=paintSize.width();
        QVector4D outPos,inPos(sampleData[3*idPoint],
                               sampleData[3*idPoint+1],
                               sampleData[3*idPoint+2],
                               1);
        outPos=matProjection*matView*matModel*inPos;
        double xp,yp;
        xp=(outPos.x()/outPos.w()+1)*width0/2;
        yp=(1-outPos.y()/outPos.w())*height0/2;
        return QPointF(xp, yp);
    }
    void adjustPaintData()
    {
        if(nSample==0) return;
        long long i;
        for(i=0;i<nSample;++i){
            pointsData[6*i]  =sampleData[3*i];
            pointsData[6*i+1]=sampleData[3*i+1];
            pointsData[6*i+2]=sampleData[3*i+2];
        }
    }
    void adjustPaintColor()
    {
        if(nSample==0) return;
        long long i,nPopColor=colorPopList.size();
        QColor tColor;
        for(i=0;i<nSample;++i){
            if(popIndex[i]>=nPopColor){
                tColor=colorPopList[nPopColor-1];
            }
            else{
                tColor=colorPopList[popIndex[i]];
            }
            pointsData[6*i+3]=tColor.redF();
            pointsData[6*i+4]=tColor.greenF();
            pointsData[6*i+5]=tColor.blueF();
        }
    }
    void sendPaintPointsToBuffer()
    {
        tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,pointsVBO);
        tmpPaintWidget->glBindVertexArray(pointsVAO);
        tmpPaintWidget->glBufferSubData(GL_ARRAY_BUFFER,0,6*nSample*sizeof(GLfloat),pointsData);
        tmpPaintWidget->glBindVertexArray(0);
        tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER,0);
    }
    void paintSearchedPoint(QPainter *painter)
    {
        if(searchedSampleIndex < 0
           || searchedSampleIndex >= nSample)
        {
            return;
        }
        QPainterPath labelShape;
        QPointF pxy = calcuPointPaint(searchedSampleIndex);
        painter -> save();
        painter->setPen(QPen(QBrush(colorSearchedPoints),3));
        labelShape.addEllipse(QPointF(pxy.x(),pxy.y()),6,6);
        painter->drawPath(labelShape);
        painter -> restore();
    }
    void paintSelectedPoint(QPainter *painter)
    {
        if(selectedSampleIndex < 0
           || selectedSampleIndex >= nSample)
        {
            return;
        }
        QPainterPath labelShape;
        QPointF pxy = calcuPointPaint(selectedSampleIndex);
        painter -> save();
        painter->setPen(QPen(QBrush(colorSelectedPoints),3));
        labelShape.addEllipse(QPointF(pxy.x(),pxy.y()),6,6);
        painter->drawPath(labelShape);
        painter -> restore();
    }
    void paintSelectedPointText(QPainter* painter)
    {
        if(selectedSampleIndex < 0 || selectedSampleIndex >= nSample) return;
        QPointF pxy = calcuPointPaint(selectedSampleIndex);
        selectedTextPainter.clear();
        selectedTextPainter.setPaintPoint(QPoint(pxy.x() + 3, pxy.y() + 3));
        selectedTextPainter.addText(sampleName[selectedSampleIndex],
                                    colorSelectedNameText,
                                    fontSelectedText);
        if(popName != nullptr){
            selectedTextPainter.addText(popName[popIndex[selectedSampleIndex]],
                                        colorPopList[popIndex[selectedSampleIndex]],
                                        fontSelectedText);
        }
        selectedTextPainter.addText("PC1: " + QString::number(sampleData[3*selectedSampleIndex]),
                                    colorSelectedPopText,
                                    fontSelectedText);
        selectedTextPainter.addText("PC2: " + QString::number(sampleData[3*selectedSampleIndex+1]),
                                    colorSelectedPopText,
                                    fontSelectedText);
        selectedTextPainter.addText("PC3: " + QString::number(sampleData[3*selectedSampleIndex+2]),
                                    colorSelectedPopText,
                                    fontSelectedText);
        painter -> save();
        selectedTextPainter.paintText(painter);
        painter -> restore();
    }
signals:

};

#endif // PCA3DSCATTERPOINTSPAINTER_HPP

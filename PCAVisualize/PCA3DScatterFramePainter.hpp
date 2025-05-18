#ifndef PCA3DSCATTERFRAMEPAINTER_HPP
#define PCA3DSCATTERFRAMEPAINTER_HPP

#include <QPainter>
#include <QFont>
#include <DataManager/DataManager.hpp>
#include "GenomeAnnotation/BaseOpenGLPaintWidget.hpp"

enum PCA3DScatterFrameForm
{
    PCA3D_FRAME_NONE = 0,
    PCA3D_FRAME_BOX = 1,
    PCA3D_FRAME_OPEN = 2,
};

class PCA3DScatterFramePainter
{
private:
    BaseOpenGLPaintWidget*   tmpPaintWidget;
    PCA3DScatterFrameForm    frameForm;
    QSize paintSize;
    bool isDisplayGridLine; //
    bool isDisplayAxisValue; //
    bool isDisplayAxisFrame; //
    bool isDisplayPCVar;
    double minPC1; //
    double minPC2; //
    double minPC3; //
    double maxPC1; //
    double maxPC2; //
    double maxPC3; //
    QFont  fontAxisValue;
    QColor colorAxisValue;

    QMatrix4x4 matModel;      // model transform
    QMatrix4x4 matProjection; // projection transform
    QMatrix4x4 matView;       // view transform

    double PC1VarRatio; //
    double PC2VarRatio; //
    double PC3VarRatio; //

    GLuint axisVAO; //
    GLuint axisVBO; //
    GLuint axisValueVAO; //
    GLuint axisValueVBO; //
    GLuint gridLineVAO; //
    GLuint gridLineVBO; //

    GLfloat* axisData; //
    GLfloat* axisValuesData; //
    GLfloat* gridLinesData;  //
    int nAxisValues; //
    int nGridLines; //

    QColor colorGridLines; //
    QColor colorAxisPC1; //
    QColor colorAxisPC2; //
    QColor colorAxisPC3; //

    QVector<double>           xGridValue;
    QVector<double>           yGridValue;
    QVector<double>           zGridValue;
    QVector<QVector3D>        xAxisValuePos;
    QVector<QVector3D>        yAxisValuePos;
    QVector<QVector3D>        zAxisValuePos;

    int idAxisShaderProgram;

public:
    PCA3DScatterFramePainter();
    ~PCA3DScatterFramePainter();
    void setPaintWidget(BaseOpenGLPaintWidget* inWidget)
    {
        tmpPaintWidget = inWidget;
    }
    bool setVarRatio(double inPC1VarRatio,
                     double inPC2VarRatio,
                     double inPC3VarRatio)
    {
        PC1VarRatio = inPC1VarRatio;
        PC2VarRatio = inPC2VarRatio;
        PC3VarRatio = inPC3VarRatio;
        return true;
    }
    void setPCRange(double inMinPC1, double inMaxPC1,
                    double inMinPC2, double inMaxPC2,
                    double inMinPC3, double inMaxPC3);
    void setAxisValuesData(double pcStart1, double pcStride1,
                       double pcStart2, double pcStride2,
                       double pcStart3, double pcStride3);
    void setIsDisplayGridLine(bool flag){isDisplayGridLine = flag;}
    void setIsDisplayAxisValue(bool flag){isDisplayAxisValue = flag;}
    void setIsDisplayAxisFrame(bool flag){isDisplayAxisFrame = flag;}
    void setIsDisplayPCVar(bool flag){isDisplayPCVar = flag;}
    void setTransformMat(const QMatrix4x4& inMatModel,
                         const QMatrix4x4& inMatProjection,
                         const QMatrix4x4& inMatView)
    {
        matModel      = inMatModel;
        matProjection = inMatProjection;
        matView       = inMatView;
    }
    void setAxisData();
    void setGridLinesData();
    void setPaintSize(QSize inSize){paintSize = inSize;}

    void initInitializeGL();
    void paintStart(QPainter *painter, double alpha)
    {
        painter -> endNativePainting();

        sendAxisValuesDataToBuffer();
        sendAxisDataToBuffer();
        sendGridLinesDataToBuffer();

        tmpPaintWidget->shaderProgramBind(idAxisShaderProgram);
        tmpPaintWidget->setUniformValue("model",matModel,idAxisShaderProgram);
        tmpPaintWidget->setUniformValue("projection",matProjection,idAxisShaderProgram);
        tmpPaintWidget->setUniformValue("view",matView,idAxisShaderProgram);

        if(isDisplayGridLine){
            tmpPaintWidget->glBindVertexArray(gridLineVAO);
            tmpPaintWidget->glDrawArrays(GL_LINES,0,2*nGridLines);
        }

        if(isDisplayAxisFrame){
            tmpPaintWidget->glBindVertexArray(axisVAO);
            tmpPaintWidget->glDrawArrays(GL_LINES,0,18);
        }

        if(isDisplayAxisValue){
            tmpPaintWidget->glBindVertexArray(axisValueVAO);
            tmpPaintWidget->glDrawArrays(GL_LINES,0,2*nAxisValues);
        }
        tmpPaintWidget->glBindVertexArray(0);
        tmpPaintWidget->shaderProgramRelease(idAxisShaderProgram);

        painter -> beginNativePainting();
        if(isDisplayPCVar){
           paintPCVar(painter);
        }
        if(isDisplayAxisValue){
           paintAxisValue(painter, alpha);
        }
    }
    void savePaintFrame(QPainter *painter, double alpha){
        if(isDisplayGridLine){
            savePaintGridLines(painter);
        }

        if(isDisplayAxisFrame){
            savePaintAxis(painter);
        }

        if(isDisplayAxisValue){
            savePaintAxisValues(painter);
        }
        if(isDisplayPCVar){
            paintPCVar(painter);
        }
        if(isDisplayAxisValue){
            paintAxisValue(painter, alpha);
        }
    }
private:
    void delDeleteGL();
    void sendAxisValuesDataToBuffer();
    void sendAxisDataToBuffer();
    void sendGridLinesDataToBuffer();
    void savePaintAxisValues(QPainter *painter);
    void savePaintAxis(QPainter *painter);
    void savePaintGridLines(QPainter *painter);
    void paintPCVar(QPainter *painter)
    {
        painter->save();

        QString PC1Label,PC2Label,PC3Label;
        int labelL=80,labelH=20;
        int width0=paintSize.width();
        int height0=paintSize.height();
        double xp,yp;
        double range1=maxPC3-minPC3,range2=maxPC1-minPC1,range3=maxPC1-minPC1;
        QVector4D PC1Pos((maxPC1+minPC1)/2,minPC2,maxPC3+0.25*range1,1),
                  PC2Pos(minPC1-0.25*range2,(maxPC2+minPC2)/2,maxPC3,1),
                  PC3Pos(maxPC1+0.25*range3,minPC2,(maxPC3+minPC3)/2,1);
        QVector4D outPos;
        if(PC1VarRatio<MML::EPS){
            PC1Label="PC1";
        }
        else{
            PC1Label=QString("PC1 ")+QString::number(100*PC1VarRatio,'g',2)+"%";
        }
        if(PC2VarRatio<MML::EPS){
            PC2Label="PC2";
        }
        else{
            PC2Label=QString("PC2 ")+QString::number(100*PC2VarRatio,'g',2)+"%";
        }
        if(PC3VarRatio<MML::EPS){
            PC3Label="PC3";
        }
        else{
            PC3Label=QString("PC3 ")+QString::number(100*PC3VarRatio,'g',2)+"%";
        }

        painter->setPen(QPen(Qt::black,3));

        outPos=matProjection*matView*matModel*PC1Pos;
        if(outPos.x()/outPos.w()>=-1&&outPos.x()/outPos.w()<=1&&
           outPos.y()/outPos.w()>=-1&&outPos.y()/outPos.w()<=1&&
           outPos.z()>=0){
            xp=(outPos.x()/outPos.w()+1)*width0/2;
            yp=(1-outPos.y()/outPos.w())*height0/2;
            painter->drawText(xp-labelL/2,
                              yp-labelH/2,
                              labelL,
                              labelH,
                              Qt::AlignVCenter|Qt::AlignHCenter,
                              PC1Label);
        }
        outPos=matProjection*matView*matModel*PC2Pos;
        if(outPos.x()/outPos.w()>=-1&&outPos.x()/outPos.w()<=1&&
           outPos.y()/outPos.w()>=-1&&outPos.y()/outPos.w()<=1&&
           outPos.z()>=0){
            xp=(outPos.x()/outPos.w()+1)*width0/2;
            yp=(1-outPos.y()/outPos.w())*height0/2;
            painter->drawText(xp-labelL/2,
                              yp-labelH/2,
                              labelL,
                              labelH,
                              Qt::AlignVCenter|Qt::AlignHCenter,
                              PC2Label);
        }
        outPos=matProjection*matView*matModel*PC3Pos;
        if(outPos.x()/outPos.w()>=-1&&outPos.x()/outPos.w()<=1&&
           outPos.y()/outPos.w()>=-1&&outPos.y()/outPos.w()<=1&&
           outPos.z()>=0){
            xp=(outPos.x()/outPos.w()+1)*width0/2;
            yp=(1-outPos.y()/outPos.w())*height0/2;
            painter->drawText(xp-labelL/2,
                              yp-labelH/2,
                              labelL,
                              labelH,
                              Qt::AlignVCenter|Qt::AlignHCenter,
                              PC3Label);
        }

        painter->restore();
    }
    void paintAxisValue(QPainter *painter, double alpha)
    {
        painter->save();

        QVector4D outPos,inPos;

        int i;
        int width0=paintSize.width(),height0=paintSize.height();
        double xp,yp;
        QString outText;
        int nXAxisValues = xAxisValuePos.size();
        int nYAxisValues = yAxisValuePos.size();
        int nZAxisValues = zAxisValuePos.size();
        painter->setPen(QPen(colorAxisValue));
        painter->setFont(fontAxisValue);
        for(i=0;i<nXAxisValues;++i){
            inPos=QVector4D(xAxisValuePos[i],1);
            outText=QString::number(xGridValue[i]);
            outPos=matProjection*matView*matModel*inPos;
            if(outPos.x()/outPos.w()<-1||outPos.x()/outPos.w()>1||
               outPos.y()/outPos.w()<-1||outPos.y()/outPos.w()>1||
               outPos.z()<0) continue;
            xp=(outPos.x()/outPos.w()+1)*width0/2;
            yp=(1-outPos.y()/outPos.w())*height0/2;
            if(alpha>MML::PI/2&&alpha<MML::PI*3/2){
                painter->drawText(xp,yp-20,50,40,Qt::AlignVCenter|Qt::AlignLeft,outText);
            }
            else{
                painter->drawText(xp-50,yp-20,50,40,Qt::AlignVCenter|Qt::AlignRight,outText);
            }
        }
        for(i=0;i<nYAxisValues;++i){
            inPos=QVector4D(yAxisValuePos[i],1);
            outText=QString::number(yGridValue[i]);
            outPos=matProjection*matView*matModel*inPos;
            if(outPos.x()/outPos.w()<-1||outPos.x()/outPos.w()>1||
               outPos.y()/outPos.w()<-1||outPos.y()/outPos.w()>1||
               outPos.z()<0) continue;
            xp=(outPos.x()/outPos.w()+1)*width0/2;
            yp=(1-outPos.y()/outPos.w())*height0/2;
            if(alpha>MML::PI){
                painter->drawText(xp,yp-20,50,40,Qt::AlignVCenter|Qt::AlignLeft,outText);
            }
            else{
                painter->drawText(xp-50,yp-20,50,40,Qt::AlignVCenter|Qt::AlignRight,outText);
            }
        }
        for(i=0;i<nZAxisValues;++i){
            inPos=QVector4D(zAxisValuePos[i],1);
            outText=QString::number(zGridValue[i]);
            outPos=matProjection*matView*matModel*inPos;
            if(outPos.x()/outPos.w()<-1||outPos.x()/outPos.w()>1||
               outPos.y()/outPos.w()<-1||outPos.y()/outPos.w()>1||
               outPos.z()<0) continue;
            xp=(outPos.x()/outPos.w()+1)*width0/2;
            yp=(1-outPos.y()/outPos.w())*height0/2;
            if(alpha>MML::PI){
                painter->drawText(xp-50,yp-20,50,40,Qt::AlignVCenter|Qt::AlignRight,outText);
            }
            else{
                painter->drawText(xp,yp-20,50,40,Qt::AlignVCenter|Qt::AlignLeft,outText);
            }
        }
        painter->restore();
    }

};

#endif // PCA3DSCATTERFRAMEPAINTER_HPP

#include "StructureBarPainter.hpp"

StructureBarPainter::StructureBarPainter()
{
    xBarAreaFactor = 0.8;
    yBarAreaFactor = 0.8;
    barGapFactor = 0.9;
    isDisplayBarFrame = false;
    isVertical = false;
    isDisplaySelectedText = false;
    idSelectedInd = -1;
    idSearchedInd = -1;
    colorSelected = QColor(255,0,0);
    colorSearched = QColor(180,100,0);
    colorSelectedText = Qt::black;
    fontSelectedText.setFamily("Times New Roman");
    fontSelectedText.setBold(true);

    sortPaintSeqMod.setCompValue(&transVectorMod);

    colorPopList.append(QColor(211, 191, 216));
    colorPopList.append(QColor(220, 127, 154));
    colorPopList.append(QColor(163, 176, 211));
    colorPopList.append(QColor(197, 227, 227));
    colorPopList.append(QColor(14, 181, 171));
    colorPopList.append(QColor(192, 219, 148));
    colorPopList.append(QColor(116, 43, 98));
    colorPopList.append(QColor(242, 170, 60));
}

void StructureBarPainter::loadIndData(const MML::Mat &inData, char **inName)
{
    if(inData.info==MML::_null||
       inData.getNCol()==0||
       inData.getNRow()==0||
       (!inName)) return;
    size_t nSample=inData.getNRow(),i;
    individualsName.clear();
    if(inName){
        for(i=0;i<nSample;++i){
            individualsName.append(inName[i]);
        }
    }
    individualsData = inData;
    individualsCumData.setData(individualsData.getNRow(),individualsData.getNCol(),.0);
    double sum = 0,cum = 0;
    paintSeq.clear();
    for(long long i = 0;i < (long long)individualsData.getNRow();++i){
        sum = 0,cum = 0;
        for(long long j = 0;j < (long long)individualsData.getNCol();++j){
            sum += individualsData(i,j);
        }
        for(long long j = 0;j < (long long)individualsData.getNCol();++j){
            individualsData(i,j) = individualsData(i,j)/sum;
            cum += individualsData(i,j);
            individualsCumData(i,j) = cum;
        }
        paintSeq.push_back(i);
    }
}

long long StructureBarPainter::isClickedInd(const QPoint &p)
{
    int xpos = p.x(),ypos = p.y();
    long long nSample = individualsData.getNRow();
    QRectF tRectF;
    for(long long i=0;i<nSample;++i){
        tRectF=calcuBarPosPix(i);
        if(tRectF.contains(xpos,ypos))
        {
            return i;
        }
    }
    return -1;
}

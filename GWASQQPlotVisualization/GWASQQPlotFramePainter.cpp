#include "GWASQQPlotFramePainter.h"

GWASQQPlotFramePainter::GWASQQPlotFramePainter() {
    leftBorderFactor       = .2;
    rightBorderFactor      = .2;
    topBorderFactor        = .2;
    buttomBorderFactor     = .2;
    axisLengthFactor       = .07;
    maxXAxisData           = .0;
    maxYAxisData           = .0;
    xLabPosFactor          = .1;
    yLabPosFactor          = .1;

    strYLab = "Observed";
    strXLab = "Expected";

    backgroundColor = Qt::white;
    colorDiagonalLine = Qt::red;

    fontAxisValueText.setFamily("Times New Roman");

    colorXYLab = Qt::black;
    fontXYLab.setFamily("Times New Roman");
    // fontXYLab.setBold(true);
    fontXYLab.setPointSize(16);

    setXYLabPainter();
}

GWASQQPlotFramePainter::~GWASQQPlotFramePainter()
{

}

void GWASQQPlotFramePainter::setXYLabPainter()
{
    xLabTextPainter.clear();
    yLabTextPainter.clear();

    xLabTextPainter.addText(strXLab, colorXYLab, fontXYLab);
    yLabTextPainter.addText(strYLab, colorXYLab, fontXYLab);

    yLabTextPainter.setRotation(-90);

    xLabTextPainter.setIsPaintRect(false);
    yLabTextPainter.setIsPaintRect(false);
}

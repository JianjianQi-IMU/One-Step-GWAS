#include "GWASQQPlotWidget.h"
#include <QSvgGenerator>

GWASQQPlotWidget::GWASQQPlotWidget(QWidget *parent)
    : BaseOpenGLPaintWidget(parent)
{
    pPointsPainter = nullptr;
    pFramePainter = nullptr;
    pChrInfo = nullptr;

    pPointsPainter = new GWASQQPlotPointsPainter;
    pFramePainter = new GWASQQPlotFramePainter;
    pChrInfo = new ChromosomeInfoClass;

    pPointsPainter -> setChrInfo(pChrInfo);
    pPointsPainter -> setPaintWidget(this);
}

GWASQQPlotWidget::~GWASQQPlotWidget()
{
    if (pPointsPainter) {
        delete pPointsPainter;
        pPointsPainter = nullptr;
    }
    if (pFramePainter) {
        delete pFramePainter;
        pFramePainter = nullptr;
    }
    if (pChrInfo) {
        delete pChrInfo;
        pChrInfo = nullptr;
    }
}

bool GWASQQPlotWidget::loadChromosome(const std::vector<long long>& inChrLen, const std::vector<std::string>& inChrName)
{
    bool flag = pChrInfo -> loadChromosome(inChrLen, inChrName);
    return flag;
}

bool GWASQQPlotWidget::loadPoints(ValPoints *inPoints, long long inNPoints)
{
    bool flag = pPointsPainter -> loadPoints(inPoints, inNPoints);
    if (flag) {
        QVector<double> tVec;
        pPointsPainter -> getXAxisData(tVec);
        pFramePainter -> setXAxisData(tVec, tVec.back());
        pPointsPainter -> getYAxisData(tVec);
        pFramePainter -> setYAxisData(tVec, tVec.back());
    }
    return flag;
}

const QColor &GWASQQPlotWidget::getPointsColor() const
{
    return pPointsPainter -> getPointsColor();
}

void GWASQQPlotWidget::setPointsColor(const QColor &col)
{
    pPointsPainter -> setPointsColor(col);
    update();
}

void GWASQQPlotWidget::setDiagonalLineColor(const QColor &col)
{
    pFramePainter -> setDiagonalLineColor(col);
    update();
}

bool GWASQQPlotWidget::setAxisValue(double xMax, double yMax, bool isReset)
{
    if (xMax <= 0 || yMax <= 0) {
        return false;
    }
    pPointsPainter -> setAxisValue(xMax, yMax, isReset);
    QVector<double> tVec;
    pPointsPainter -> getXAxisData(tVec);
    pFramePainter -> setXAxisData(tVec, xMax);
    pPointsPainter -> getYAxisData(tVec);
    pFramePainter -> setYAxisData(tVec, yMax);
    update();
    return true;
}

const QColor &GWASQQPlotWidget::getDiagonalLineColor() const
{
    return pFramePainter -> getDiagonalLineColor();
}

void GWASQQPlotWidget::savePlot(FD::SaveFileInfo inInfo)
{
    FD::SVGFileFormInfo *svgInfo = nullptr;
    FD::PNGFileFormInfo *pngInfo = nullptr;
    switch (inInfo.getIDForm()) {
        case FD::FILEFORM_SVG: {
            if (inInfo.getFileInfo()) {
                svgInfo = dynamic_cast<FD::SVGFileFormInfo *>(inInfo.getFileInfo());
                savePaintCurrentSVG(svgInfo->filePath, svgInfo->saveSize);
            }
            break;
        }
        case FD::FILEFORM_PNG: {
            if (inInfo.getFileInfo()) {
                pngInfo = dynamic_cast<FD::PNGFileFormInfo *>(inInfo.getFileInfo());
                savePaintCurrentPNG(pngInfo->filePath, pngInfo->saveSize);
            }
            break;
        }
        default:
            break;
    }
}

void GWASQQPlotWidget::savePaintCurrentSVG(QString savePath, QSizeF inSize)
{
    if(savePath.isEmpty()) return;
    QSvgGenerator generator;
    generator.setFileName(savePath);
    generator.setSize(QSize(inSize.width(), inSize.height()));

    int tSize = std::min(inSize.width(), inSize.height());
    pPointsPainter -> setDisplayRect(QRect(1, 1, tSize, tSize));
    pFramePainter -> setDisplayRect(QRect(1, 1, tSize, tSize));

    QPainter Painter;
    Painter.begin(&generator);

    pFramePainter -> paintStart(&Painter);
    pPointsPainter -> savePaintStart(&Painter);

    Painter.end();
}

void GWASQQPlotWidget::savePaintCurrentPNG(QString savePath, QSizeF inSize)
{
    if (savePath.isEmpty()) return;
    QPixmap pixmap(QSize(inSize.width(), inSize.height()));
    pixmap.fill(Qt::white);
    int tSize = std::min(inSize.width(), inSize.height());
    pPointsPainter -> setDisplayRect(QRect(1, 1, tSize, tSize));
    pFramePainter -> setDisplayRect(QRect(1, 1, tSize, tSize));

    QPainter Painter;

    Painter.begin(&pixmap);

    pFramePainter -> paintStart(&Painter);
    pPointsPainter -> savePaintStart(&Painter);

    Painter.end();

    pixmap.save(savePath);
}

void GWASQQPlotWidget::leftMousePressEvent(QMouseEvent *e, bool &isNeedUpdate)
{
    long long idPoint = -1;
    pPointsPainter -> setSelectedPointIndex(-1);
    if ((idPoint = pPointsPainter -> isPressedPoint(QPoint(e->x(), e->y()))) != -1) {
        pPointsPainter -> setSelectedPointIndex(idPoint);
        isNeedUpdate = true;
    }
}

void GWASQQPlotWidget::initializeGL()
{
    initializeOpenGLFunctions();
    pPointsPainter -> initInitializeGL();
}

void GWASQQPlotWidget::paintGL()
{
    QColor backgroundColor = pFramePainter -> getBackgroundColor();

    glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int tSize = std::min(width(), height());
    pPointsPainter -> setDisplayRect(QRect(1, 1, tSize, tSize));
    pFramePainter -> setDisplayRect(QRect(1, 1, tSize, tSize));

    QPainter Painter;
    Painter.begin(this);
    pFramePainter -> paintStart(&Painter);
    pPointsPainter -> paintStart(&Painter);
    Painter.end();
}

void GWASQQPlotWidget::resizeGL(int w, int h)
{

}

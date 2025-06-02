#include "LogPScatterViewPainter.hpp"

LogPScatterViewPainter::LogPScatterViewPainter()
{
    displayArea                         = QRect();
    maxAxisLabelNumY                    = 10;
    selectedPointsDistance2             = 20;
    pointSize                           = 2;
    selectedPointColor                  = QColor(0, 255, 0);
    maxLabelPointsNum                   = 1000;
    LogPAxisScaleLen                    = 8;
    isDisplayMovingLogPLine             = true;
    movingLogPLineColor                 = QColor(0,0,0);

    labeledPointColor                   = QColor(255, 0, 0);

    idPointColor                        = nullptr;

    points                              = nullptr;
    cumPointsPos                        = nullptr;
    nPoints                             = 0;
    maxLogP                             = 0;
    minLogP                             = 0;
    intUpLogP                           = 0;
    intDownLogP                         = 0;
    logPStride                          = 0;

    paintPoints                         = nullptr;
    nPaintPoints                        = 0;

    currentMinPointsi                   = 0;
    currentMaxPointsi                   = 0;

    selectedPointi                      = -1;
    displaySelectedPointInfo            = false;

    colorScatterPoint.append(QColor(255, 127, 51));
    colorScatterPoint.append(QColor(122, 127, 233));

    fontPointsInfo.setFamily("Times New Roman");
    fontPointsInfo.setBold(true);

    fontAxis.setFamily("Times New Roman");
    fontAxis.setPointSizeF(7);

    fontYTitle.setFamily("Times New Roman");
    fontYTitle.setPointSizeF(16);
    strYTitle = "- Lg P-value";

    selectedPointInfoPainter.setBackgroundColor(QColor(255, 255, 255, 100));
}

LogPScatterViewPainter::~LogPScatterViewPainter()
{
    delDeleteGL();

    if (points) delete [] points;
    if (idPointColor) delete [] idPointColor;
    if (cumPointsPos) delete [] cumPointsPos;
    if (paintPoints) delete [] paintPoints;

    points = nullptr;
    idPointColor = nullptr;
    cumPointsPos = nullptr;
    paintPoints = nullptr;
}

bool LogPScatterViewPainter::loadPoints(ValPoints *inPoints, long long inNPoints)
{
    if (!inPoints || !inNPoints) {
        return false;
    }
    long long i;
    long long* cumChrLen = tmpChrInfo -> getCumChrLen();
    if (nPoints != inNPoints) {
        if (points) delete [] points;
        if (idPointColor) delete [] idPointColor;
        if (cumPointsPos) delete [] cumPointsPos;
        if (paintPoints) delete [] paintPoints;
        points = new ValPoints[inNPoints + 1];
        cumPointsPos = new int64_t[inNPoints + 1];
        paintPoints = new GLfloat[inNPoints * 2];
        idPointColor = new GLuint[inNPoints];
    }
    nPoints = inNPoints;
    maxLogP = 0;
    minLogP = 1e300;
    for (i = 0; i < nPoints; ++i) {
        points[i] = inPoints[i];
        maxLogP = std::max(maxLogP, points[i].val);
        minLogP = std::min(minLogP, points[i].val);
    }
    intUpLogP = maxLogP;
    intUpLogP++;
    logPStride = 1;
    i = 0;
    while (intUpLogP / logPStride > maxAxisLabelNumY) {
        if (i % 2 == 0) {
            logPStride *= 5;
        } else {
            logPStride *= 2;
        }
        i = (i + 1) % 2;
    }
    intUpLogP = (intUpLogP / logPStride + 1) * logPStride;
    std::sort(points, &points[nPoints]);
    for (i = 0; i < nPoints; ++i) {
        cumPointsPos[i] = points[i].pos + cumChrLen[points[i].idchr];
    }
    return true;
}

void LogPScatterViewPainter::initInitializeGL()
{
    tmpPaintWidget->glGenBuffers(1, &idPointsVBO);
    tmpPaintWidget->glGenBuffers(1, &idColorVBO);
    tmpPaintWidget->glGenVertexArrays(1, &idPointsVAO);

    tmpPaintWidget->glBindVertexArray(idPointsVAO);

    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER, idPointsVBO);
    tmpPaintWidget->glBufferData(GL_ARRAY_BUFFER, 2 * nPoints * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    tmpPaintWidget->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    tmpPaintWidget->glEnableVertexAttribArray(0);

    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER, idColorVBO);
    tmpPaintWidget->glBufferData(GL_ARRAY_BUFFER, nPoints * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
    tmpPaintWidget->glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(GLuint), (void*)0);
    tmpPaintWidget->glEnableVertexAttribArray(1);

    tmpPaintWidget->setGLShaderProgram(":/shader/pointsVertex.shader", ":/shader/pointsFragment.shader");
    tmpPaintWidget->glPointSize(10);

    tmpPaintWidget->glBindVertexArray(0);
    tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LogPScatterViewPainter::paintLogPScatterView(QPainter *painter)
{
    QVector3D* pointColor = new QVector3D[8];
    long long i;
    for (i = 0; i < colorScatterPoint.length(); ++i) {
        pointColor[i].setX(colorScatterPoint[i].redF());
        pointColor[i].setY(colorScatterPoint[i].greenF());
        pointColor[i].setZ(colorScatterPoint[i].blueF());
    }
    QColor tColor = colorScatterPoint.back();
    for (; i < 8; ++i) {
        pointColor[i].setX(tColor.redF());
        pointColor[i].setY(tColor.greenF());
        pointColor[i].setZ(tColor.blueF());
    }

    paintAxisInPainter(painter);
    painter->endNativePainting();

    sendPointsToBuffer();

    if (tmpCurrentLeftPosF >= 0 && tmpCurrentRightPosF >= 0 && nPaintPoints != 0) {
        tmpPaintWidget->glBindVertexArray(idPointsVAO);
        tmpPaintWidget->shaderProgramBind();
        tmpPaintWidget->setUniformValueArray("colorList", pointColor, 8);
        tmpPaintWidget->setUniformValue("pSize", pointSize);
        tmpPaintWidget->setUniformValue("iResolution", tmpPaintWidget->iResolution);
        tmpPaintWidget->glDrawArrays(GL_POINTS, 0, nPaintPoints);
        tmpPaintWidget->shaderProgramRelease();
        tmpPaintWidget->glBindVertexArray(0);
    }

    painter->beginNativePainting();
    paintLabelPoints(painter);
    paintSelectedPoint(painter);
    paintPointInfo(painter,selectedPointi);

    delete [] pointColor;
}

void LogPScatterViewPainter::paintMovingLogPLine(QPainter *painter)
{
    if (!isDisplayMovingLogPLine || tmpMovingMouseY > displayArea.y() + displayArea.height() || tmpMovingMouseY < displayArea.y()) {
        return;
    }

    painter->save();

    int xpos = displayArea.x();
    int lineLen = displayArea.width();
    long double mouseLogP = maxLogP * (1.0 - ((long double)(tmpMovingMouseY - displayArea.y())) / displayArea.height());

    painter -> setBrush(movingLogPLineColor);
    painter -> drawLine(xpos, tmpMovingMouseY, xpos + lineLen, tmpMovingMouseY);

    painter->restore();
}

void LogPScatterViewPainter::savePaintLogPScatterView(QPainter *painter)
{
    int xpos = displayArea.x(), ypos = displayArea.y();
    int colorNum = colorScatterPoint.length();
    int width = displayArea.width(), height = displayArea.height();
    long double positionGapLenF = tmpCurrentRightPosF - tmpCurrentLeftPosF;
    long long* cumChrLen = tmpChrInfo -> getCumChrLen();
    double tx, ty;
    QColor tColor;
    painter -> save();
    paintAxisInPainter(painter);
    painter -> setPen(QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap));
    for (long long i = currentMinPointsi; i <= currentMaxPointsi; ++i) {
        tx = float(points[i].pos + cumChrLen[points[i].idchr] - tmpCurrentLeftPosF) * width / positionGapLenF + xpos;
        ty = height * (1 - points[i].val / (intUpLogP - intDownLogP)) + ypos;
        tColor = colorScatterPoint[points[i].idchr % colorNum];
        painter -> setPen(QPen(tColor, 1.5 * pointSize, Qt::SolidLine, Qt::RoundCap));
        painter -> drawPoint(QPointF(tx, ty));
    }
    painter -> restore();
}

void LogPScatterViewPainter::delDeleteGL()
{
    if (tmpPaintWidget -> isValid()) {
        tmpPaintWidget -> glDeleteBuffers(1, &idPointsVBO);
        tmpPaintWidget -> glDeleteBuffers(1, &idColorVBO);
        tmpPaintWidget -> glDeleteVertexArrays(1, &idPointsVAO);
    }
}

void LogPScatterViewPainter::sendPointsToBuffer()
{
    long long mid, ta = 0, tb = nPoints - 1;
    while (ta <= tb) {
        mid = (ta + tb) / 2;
        if ((long double)(cumPointsPos[mid]) >= tmpCurrentLeftPosF) {
            tb = mid - 1;
        } else {
            ta = mid + 1;
        }
    }
    currentMinPointsi = ta;
    ta = 0, tb = nPoints - 1;
    while (ta <= tb) {
        mid = (ta + tb) / 2;
        if ((long double)(cumPointsPos[mid]) > tmpCurrentRightPosF) {
            tb = mid - 1;
        } else {
            ta = mid + 1;
        }
    }
    currentMaxPointsi = tb;
    if (currentMinPointsi <= currentMaxPointsi) {
        nPaintPoints = currentMaxPointsi - currentMinPointsi + 1;
        adjustPaintPointsPos();
        adjustPaintPointsColor();
        tmpPaintWidget->glBindVertexArray(idPointsVAO);

        tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER, idPointsVBO);
        tmpPaintWidget->glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * nPaintPoints * sizeof(GLfloat), paintPoints);

        tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER, idColorVBO);
        tmpPaintWidget->glBufferSubData(GL_ARRAY_BUFFER, 0, nPaintPoints * sizeof(GLuint), idPointColor);

        tmpPaintWidget->glBindBuffer(GL_ARRAY_BUFFER, 0);
        tmpPaintWidget->glBindVertexArray(0);
    } else {
        nPaintPoints = 0;
    }
}

void LogPScatterViewPainter::paintAxisInPainter(QPainter *painter)
{
    int width = displayArea.width();
    int height = displayArea.height();
    int xpos = displayArea.x(), ypos = displayArea.y();

    painter->save();

    painter->setFont(fontAxis);

    int painty = 0;
    painter->setPen(QPen(Qt::black, 1));
    painter->drawLine(xpos, ypos, xpos + width, ypos);
    painter->drawLine(xpos, height + ypos, xpos + width, height + ypos);
    painter->drawLine(xpos, ypos, xpos, ypos + height);
    painter->drawLine(xpos + width, ypos, xpos + width, ypos + height);
    for (long long i = intDownLogP; i <= intUpLogP; i += logPStride) {
        painty = ypos + float(intUpLogP - i) / intUpLogP * height;
        painter->drawLine(xpos, painty, xpos - LogPAxisScaleLen, painty);
        painter->drawText(QRectF(1, painty - 20, xpos - LogPAxisScaleLen - 5, 40),
            Qt::AlignRight | Qt::AlignVCenter, QString::number(i) + ".0");
    }

    painter->restore();
}

void LogPScatterViewPainter::paintSelectedPoint(QPainter *painter)
{
    if (selectedPointi < currentMinPointsi || selectedPointi > currentMaxPointsi) {
        return;
    }
    int px, py;
    painter->save();
    if (!getPointPxy(selectedPointi, px, py)) {
        return ;
    }
    painter->setPen(QPen(QBrush(selectedPointColor), 8));
    painter->drawPoint(px, py);
    painter->restore();

}

void LogPScatterViewPainter::paintPointInfo(QPainter *painter, long long idPoint)
{
    if (idPoint < currentMinPointsi || idPoint > currentMaxPointsi) {
        return;
    }
    int px, py;
    if (!getPointPxy(idPoint, px, py)) {
        return ;
    }
    char** chrName = tmpChrInfo -> getChrName();
    ValPoints pPoint = points[idPoint];
    QString textChr = QString("Chr : ") + chrName[pPoint.idchr];
    QString textPos = QString("Pos : ") + QString::number(pPoint.pos);
    QString textVal = QString("-logP : ") + QString::number(pPoint.val);

    selectedPointInfoPainter.clear();

    selectedPointInfoPainter.setPaintPoint(QPoint(px + 4, py + 4));
    selectedPointInfoPainter.addText(textVal, Qt::red, fontPointsInfo);
    selectedPointInfoPainter.addText(textChr, Qt::black, fontPointsInfo);
    selectedPointInfoPainter.addText(textPos, Qt::black, fontPointsInfo);
    selectedPointInfoPainter.paintText(painter);
}

void LogPScatterViewPainter::paintLabelPoints(QPainter *painter)
{
    long long tPos;
    int px, py;
    if (idLabelPoints.size() == 0) {
        return ;
    }
    painter->save();

    for (int i = 0; i < idLabelPoints.size(); ++i) {
        tPos = idLabelPoints[i];
        if (tPos < currentMinPointsi || tPos > currentMaxPointsi) {
            continue;
        }
        if (!getPointPxy(tPos, px, py)) {
            continue;
        }
        painter->setPen(QPen(QBrush(labeledPointColor), 8));
        painter->drawPoint(px, py);
    }

    painter->restore();

}

void LogPScatterViewPainter::paintYTitle(QPainter *painter)
{
    painter->save();

    painter->restore();
}


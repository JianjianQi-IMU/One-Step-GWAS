#ifndef GWASSETTHRESHOLDWIDGET_H
#define GWASSETTHRESHOLDWIDGET_H

#include <QWidget>
#include <QColor>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <map>

#include <QDebug>

#include "GWASThresholdPainter.hpp"
#include "StyleChoose/LineStyleChoose.hpp"

class GWASSetThresholdWidget : public QWidget
{
    Q_OBJECT
private:
    std::map<Qt::PenStyle,int>   mapPenStyle;
    QVector<Qt::PenStyle>        vecPenStyle;
    std::map<int,int>            mapSize;
    QVector<int>                 vecSize;
    LineStyleDisplayComboBox*    pLineSizeComboBox;
    LineStyleDisplayComboBox*    pLineStyleComboBox;
    QPushButton*                 pColBtn;
    QLabel*                      pColLabel;
    QLabel*                      pSizeLabel;
    QLabel*                      pStyleLabel;
    QDoubleSpinBox*              pValBox;
    QPushButton*                 pOKBtn;
    QPushButton*                 pCancelBtn;
    QColor                       color;
public:
    explicit GWASSetThresholdWidget(QWidget *parent = nullptr);
    void setLineStyleParam(const LineStyleParam& inParam);

signals:
    void sendThresholdLine(float val,LineStyleParam param);
public slots:
    void dealSave();
    void chooseCol();
private:
    void initLineSizeComboBox();
    void initLineStyleComboBox();
};

#endif // GWASSETTHRESHOLDWIDGET_H

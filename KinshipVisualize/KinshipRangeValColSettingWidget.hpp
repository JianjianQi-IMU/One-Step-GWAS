#ifndef KINSHIPRANGEVALCOLSETTINGWIDGET_H
#define KINSHIPRANGEVALCOLSETTINGWIDGET_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QColorDialog>

#include "StyleChoose/ColorChoosePushButton.hpp"

class KinshipRangeValColParam
{
public:
    QColor colorHigh;
    QColor colorLow;
    QColor colorMean;
    double valueHigh;
    double valueLow;
    double valueMean;
};

class KinshipRangeValColSettingWidget : public QWidget
{
    Q_OBJECT
private:
    QDoubleSpinBox*             pHighBox;
    QDoubleSpinBox*             pLowBox;
    QDoubleSpinBox*             pMeanBox;
    ColorChoosePushButton*      pHighColBtn;
    ColorChoosePushButton*      pLowColBtn;
    ColorChoosePushButton*      pMeanColBtn;
    QPushButton*                pSave;
    QPushButton*                pCancel;
public:
    KinshipRangeValColSettingWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_DeleteOnClose, true);
        setAttribute(Qt::WA_ShowModal, true);

        pHighBox = new QDoubleSpinBox(this);
        pLowBox = new QDoubleSpinBox(this);
        pMeanBox = new QDoubleSpinBox(this);
        pHighColBtn = new ColorChoosePushButton(this);
        pLowColBtn = new ColorChoosePushButton(this);
        pMeanColBtn = new ColorChoosePushButton(this);
        pSave = new QPushButton("Save", this);
        pCancel = new QPushButton("Cancel", this);
        QLabel* highLabel = new QLabel("Max Value", this);
        QLabel* lowLabel = new QLabel("Min Value", this);
        QLabel* meanLabel = new QLabel("Mean Value", this);
        QGroupBox* pGroupBox = new QGroupBox("Value Range", this);
        QVBoxLayout* pGroupLayout = new QVBoxLayout(pGroupBox);
        QVBoxLayout* mainLayout = new QVBoxLayout(this);

        QWidget* pHighWidget = new QWidget(this);
        QWidget* pLowWidget = new QWidget(this);
        QWidget* pMeanWidget = new QWidget(this);
        QWidget* pDownWidget = new QWidget(this);

        QHBoxLayout* pHighLayout = new QHBoxLayout(pHighWidget);
        QHBoxLayout* pLowLayout = new QHBoxLayout(pLowWidget);
        QHBoxLayout* pMeanLayout = new QHBoxLayout(pMeanWidget);
        QHBoxLayout* pDownLayout = new QHBoxLayout(pDownWidget);

        pHighLayout->addWidget(highLabel);
        pHighLayout->addWidget(pHighBox);
        pHighLayout->addWidget(pHighColBtn);
        pLowLayout->addWidget(lowLabel);
        pLowLayout->addWidget(pLowBox);
        pLowLayout->addWidget(pLowColBtn);
        pMeanLayout->addWidget(meanLabel);
        pMeanLayout->addWidget(pMeanBox);
        pMeanLayout->addWidget(pMeanColBtn);
        pDownLayout->addWidget(pSave);
        pDownLayout->addWidget(pCancel);

        pGroupLayout->addWidget(pHighWidget);
        pGroupLayout->addWidget(pMeanWidget);
        pGroupLayout->addWidget(pLowWidget);

        mainLayout->addWidget(pGroupBox);
        mainLayout->addWidget(pDownWidget);

        pHighBox -> setRange(-1E37, 1E37);
        pLowBox -> setRange(-1E37, 1E37);
        pMeanBox -> setRange(-1E37, 1E37);
        pHighBox -> setSingleStep(0.1);
        pLowBox -> setSingleStep(0.1);
        pMeanBox -> setSingleStep(0.1);

        connect(pHighColBtn, &ColorChoosePushButton::clicked,
            this, &KinshipRangeValColSettingWidget::setHighColor);
        connect(pLowColBtn, &ColorChoosePushButton::clicked,
            this, &KinshipRangeValColSettingWidget::setLowColor);
        connect(pMeanColBtn, &ColorChoosePushButton::clicked,
            this, &KinshipRangeValColSettingWidget::setMeanColor);
        connect(pSave, &QPushButton::clicked,
            this, &KinshipRangeValColSettingWidget::saveTrigger);
        connect(pCancel, &QPushButton::clicked,
            this, &KinshipRangeValColSettingWidget::close);
    }
    explicit KinshipRangeValColSettingWidget(const KinshipRangeValColParam& param, QWidget* parent = nullptr)
        : KinshipRangeValColSettingWidget(parent)
    {
        setParam(param);
    }
    void setParam(const KinshipRangeValColParam& param)
    {
        pHighColBtn -> setChooseColor(param.colorHigh);
        pLowColBtn -> setChooseColor(param.colorLow);
        pMeanColBtn -> setChooseColor(param.colorMean);
        pHighBox -> setValue(param.valueHigh);
        pLowBox -> setValue(param.valueLow);
        pMeanBox -> setValue(param.valueMean);
    }
public slots:
    void saveTrigger()
    {
        KinshipRangeValColParam param;
        param.colorHigh = pHighColBtn -> getChooseColor();
        param.colorMean = pMeanColBtn -> getChooseColor();
        param.colorLow = pLowColBtn -> getChooseColor();
        param.valueHigh = pHighBox -> value();
        param.valueMean = pMeanBox -> value();
        param.valueLow = pLowBox -> value();
        emit sendParam(param);
        close();
    }
    void setHighColor()
    {
        QColor color = QColorDialog::getColor(pHighColBtn -> getChooseColor(), this, "Choose High Color");
        if (color.isValid()) {
            pHighColBtn -> setChooseColor(color);
        }
    }
    void setLowColor()
    {
        QColor color = QColorDialog::getColor(pLowColBtn -> getChooseColor(), this, "Choose Low Color");
        if (color.isValid()) {
            pLowColBtn -> setChooseColor(color);
        }
    }
    void setMeanColor()
    {
        QColor color = QColorDialog::getColor(pMeanColBtn -> getChooseColor(), this, "Choose Mean Color");
        if (color.isValid()) {
            pMeanColBtn -> setChooseColor(color);
        }
    }
signals:
    void sendParam(KinshipRangeValColParam param);
};

#endif // KINSHIPRANGEVALCOLSETTINGWIDGET_H

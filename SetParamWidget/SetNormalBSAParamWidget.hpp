#ifndef _SETPARAMWIDGET_SETNORMALBSAPARAMWIDGET_HPP_
#define _SETPARAMWIDGET_SETNORMALBSAPARAMWIDGET_HPP_

#include "SetParamWidgetHeader.hpp"

class SetNormalBSAParamWidget : public QWidget
{
    Q_OBJECT
private:
    QPushButton*     continueBtn;
    QPushButton*     cancelBtn;
    QLineEdit*       editWinLen;
    QLineEdit*       editStride;
    QLabel*          labelNHP;
    QLabel*          labelNLP;
    QLabel*          labelDepth;
    QLabel*          labelLowThre;
    QLabel*          labelHighThre;
    QLabel*          labelWinLen;
    QLabel*          labelStride;
    QLabel*          labelPvalue;
    QSpinBox*        nHPBox;
    QSpinBox*        nLPBox;
    QSpinBox*        depthBox;
    QDoubleSpinBox*  lowThreBox;
    QDoubleSpinBox*  highThreBox;
    QDoubleSpinBox*  pvalueBox;
    FD::BSAProject*  project;
    size_t           id;
    explicit SetNormalBSAParamWidget(QWidget *parent = nullptr);
public:
    SetNormalBSAParamWidget(size_t inId,
                            FD::BSAProject* inProject,
                            QWidget *parent = nullptr);
    ~SetNormalBSAParamWidget();
signals:
    void startNormalBSA(size_t id,int nThread);
public slots:
    void dealwithContinue();
};

#endif

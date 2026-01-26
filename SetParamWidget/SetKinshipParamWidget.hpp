#ifndef _SETPARAMWIDGET_SETKINSHIPPARAMWIDGET_HPP_
#define _SETPARAMWIDGET_SETKINSHIPPARAMWIDGET_HPP_

#include "SetParamWidgetHeader.hpp"

class SetKinshipParamWidget : public QWidget
{
    Q_OBJECT
protected:
    uint64_t id;
    MML::KinshipParam* para;
    QComboBox* typeBox;
    QPushButton* continueBtn;
    QPushButton* cancelBtn;
protected:
    explicit SetKinshipParamWidget(QWidget *parent = nullptr);
public:
    SetKinshipParamWidget(uint64_t inId, MML::KinshipParam* inPara, QWidget *parent = nullptr);

signals:
    void startKinship(uint64_t id,int nThread);

public slots:
    void dealwithContinue();
};

#endif // SETKINSHIPPARAMWIDGET_HPP

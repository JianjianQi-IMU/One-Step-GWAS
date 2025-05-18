#ifndef _SETPARAMWIDGET_SETKINSHIPPARAMWIDGET_HPP_
#define _SETPARAMWIDGET_SETKINSHIPPARAMWIDGET_HPP_

#include "SetParamWidgetHeader.hpp"

class SetKinshipParamWidget : public QWidget
{
    Q_OBJECT
protected:
    size_t id;
    MML::KinshipParam* para;
    QComboBox* typeBox;
    QPushButton* continueBtn;
    QPushButton* cancelBtn;
protected:
    explicit SetKinshipParamWidget(QWidget *parent = nullptr);
public:
    SetKinshipParamWidget(size_t inId, MML::KinshipParam* inPara, QWidget *parent = nullptr);

signals:
    void startKinship(size_t id,int nThread);

public slots:
    void dealwithContinue();
};

#endif // SETKINSHIPPARAMWIDGET_HPP

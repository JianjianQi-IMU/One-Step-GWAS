#ifndef _SETPARAMWIDGET_SETFASTSTRUCTUREPARAMWIDGET_HPP_
#define _SETPARAMWIDGET_SETFASTSTRUCTUREPARAMWIDGET_HPP_

#include "SetParamWidgetHeader.hpp"

class SetFastStructureParamWidget : public QWidget
{
    Q_OBJECT
private:
    size_t id;
    MML::FastStructureParam* para;
    QSpinBox* popBox;
    QPushButton* continueBtn;
    QPushButton* cancelBtn;
    explicit SetFastStructureParamWidget(QWidget *parent = nullptr);
public:
    SetFastStructureParamWidget(size_t inId, MML::FastStructureParam* inPara, QWidget *parent = nullptr);
    ~SetFastStructureParamWidget();
signals:
    void startFastStructure(size_t id,int nThread);
public slots:
    void dealwithContinue();
};

#endif

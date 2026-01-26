#ifndef _SETPARAMWIDGET_SETSTRUCTUREPARAMWIDGET_HPP_
#define _SETPARAMWIDGET_SETSTRUCTUREPARAMWIDGET_HPP_

#include "SetParamWidgetHeader.hpp"

class SetStructureParamWidget : public QWidget
{
    Q_OBJECT
private:
    uint64_t id;
    MML::StructureParam* para;
    QSpinBox* burinBox;
    QSpinBox* recordBox;
    QSpinBox* popBox;
    QPushButton* continueBtn;
    QPushButton* cancelBtn;
    explicit SetStructureParamWidget(QWidget *parent = nullptr);
public:
    SetStructureParamWidget(uint64_t inId, MML::StructureParam* inPara, QWidget *parent = nullptr);
    ~SetStructureParamWidget();
signals:
    void startStructure(uint64_t id,int nThread);
public slots:
    void dealwithContinue();
};

#endif

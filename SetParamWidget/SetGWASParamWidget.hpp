#ifndef _SETPARAMWIDGET_SETGWASPARAMWIDGET_HPP_
#define _SETPARAMWIDGET_SETGWASPARAMWIDGET_HPP_

#include "SetParamWidgetHeader.hpp"

class SetGWASParamWidget : public QWidget
{
    Q_OBJECT
private:
    uint64_t id;
    ThreadRun::AnalyzeClass info;
    QSpinBox* nThreadBox;
    QComboBox* binFormBox;
    QPushButton* continueBtn;
    QPushButton* cancelBtn;
    explicit SetGWASParamWidget(QWidget *parent = nullptr);
public:
    SetGWASParamWidget(uint64_t inID,ThreadRun::AnalyzeClass inInfo,QWidget *parent = nullptr);
    ~SetGWASParamWidget();
signals:
    void startGWAS(uint64_t id,ThreadRun::AnalyzeClass info,int nThread);
public slots:
    void dealwithContinue();
};

#endif

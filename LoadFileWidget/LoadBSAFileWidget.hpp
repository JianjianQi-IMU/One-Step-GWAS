#ifndef _LOADFILEWIDGET_LOADBSAFILEWIDGET_HPP_
#define _LOADFILEWIDGET_LOADBSAFILEWIDGET_HPP_

#include "LoadFileWidgetHeader.hpp"

class LoadBSAFileWidget : public QWidget
{
    Q_OBJECT
public:
    QPushButton*     continueBtn;
    QPushButton*     cancelBtn;
    QPushButton*     fileFormBtn;
    QPushButton*     openMarkerBtn;
    QPushButton*     openChrLenBtn;
    QComboBox*       fileFormChooseBox;
    QLineEdit*       editMarker;
    QLineEdit*       editChrLen;
    QLabel*          labelMarker;
    QLabel*          labelChrLen;
    AnalyzeUnitInfo* pUnitInfo;
    QList<FD::BaseProject*>* pProjectList;
    int row;
    explicit LoadBSAFileWidget(QWidget* parent = nullptr);
    LoadBSAFileWidget(AnalyzeUnitInfo* inUnitInfo,
                      QList<FD::BaseProject*>* inProjectList,
                      int inRow,
                      QWidget* parent = nullptr);
    ~LoadBSAFileWidget();
    void setUnitInfo(AnalyzeUnitInfo* inUnitInfo);
    void loadMarkerFile();
    void loadChrLenFile();
    void viewFileForm();
    void save();
signals:
    void loadOK(int row);
};

#endif

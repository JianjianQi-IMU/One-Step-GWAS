#ifndef _LOADFILEWIDGET_LOADGWASFILEWIDGET_HPP_
#define _LOADFILEWIDGET_LOADGWASFILEWIDGET_HPP_

#include "LoadFileWidgetHeader.hpp"
#include "FileDeal/ProjectFilesRecord.hpp"

class LoadGWASFileWidget : public QWidget
{
    Q_OBJECT
public:
    QPushButton* continueBtn;
    QPushButton* cancelBtn;
    QPushButton* openPlinkBtn;
    QPushButton* openPheBtn;
    QPushButton* openKinBtn;
    QPushButton* openCovBtn;
    QPushButton* fileFormBtn;
    QComboBox*   fileFormChooseBox;
    QLineEdit*   editFile1;
    QLineEdit*   editFile2;
    QLineEdit*   editPhe;
    QLineEdit*   editKin;
    QLineEdit*   editCov;
    QLabel*      labelPlink;
    QLabel*      labelPhe;
    QLabel*      labelKin;
    QLabel*      labelCov;
    AnalyzeUnitInfo* pUnitInfo;
    QList<FD::BaseProject*>* pProjectList;
    int row;
    explicit LoadGWASFileWidget(QWidget* parent = nullptr);
    LoadGWASFileWidget(AnalyzeUnitInfo* inUnitInfo,
                       QList<FD::BaseProject*>* inProjectList,
                       int inRow,
                       QWidget* parent = nullptr);
    ~LoadGWASFileWidget();
    void setFileSet(FD::BaseProjectFilesSet* inProject);
    void changeGenoFileInputMode(int id);
    void setUnitInfo(AnalyzeUnitInfo* inUnitInfo);
    void loadGenoFile();
    void loadCov();
    void loadPhe();
    void loadKin();
    void save();
    void viewFileFormat();
signals:
    void loadOK(int row);
};

#endif

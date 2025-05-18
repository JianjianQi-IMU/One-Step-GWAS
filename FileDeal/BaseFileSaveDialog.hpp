#ifndef BASEFILESAVEDIALOG_HPP
#define BASEFILESAVEDIALOG_HPP

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

#include <QDebug>

#include "FileFormClass.hpp"
#include "StyleChoose/RectSizeDisplayWidget.hpp"

namespace FD {

class SaveFileInfo
{
private:
    BaseFileFormInfoClass *pInfo;

public:
    SaveFileInfo()
    {
        pInfo = nullptr;
    }
    SaveFileInfo(const SaveFileInfo& inInfo)
        : SaveFileInfo()
    {
        *this = inInfo;
    }
    SaveFileInfo(SaveFileInfo&& inInfo)
        : SaveFileInfo()
    {
        *this = inInfo;
    }
    ~SaveFileInfo()
    {
        if (pInfo)
        {
            delete pInfo;
        }
        pInfo = nullptr;
    }
    template<typename FILEFORMINFO>
    void createFileInfo()
    {
        pInfo = new FILEFORMINFO;
    }
    void setFileForm(BaseFileFormInfoClass *inInfo)
    {
        if (inInfo == nullptr)
        {
            delete pInfo;
            pInfo = nullptr;
        } else {
            inInfo->copyTo(pInfo);
        }
    }
    FileFormClass getIDForm() const
    {
        return pInfo -> getIDForm();
    }
    BaseFileFormInfoClass* getFileInfo()
    {
        return pInfo;
    }

    SaveFileInfo& operator=(const SaveFileInfo& inInfo)
    {
        if (inInfo.pInfo == nullptr)
        {
            delete pInfo;
            pInfo = nullptr;
        } else {
            inInfo.pInfo->copyTo(pInfo);
        }
        return *this;
    }
    SaveFileInfo& operator=(SaveFileInfo&& inInfo)
    {
        if (pInfo)
        {
            delete pInfo;
        }
        pInfo = inInfo.pInfo;
        inInfo.pInfo = nullptr;
        return *this;
    }
};

class BaseFileSaveDialog : public QWidget
{
    Q_OBJECT
protected:
    QPushButton* chooseSavePathBtn;
    QPushButton* saveBtn;
    QPushButton* cancelBtn;
    QPushButton* fileFormLook;
    QLineEdit*   savePathEdit;
    QLineEdit*   widthEdit;
    QLineEdit*   heightEdit;
    QComboBox*   fileFormBox;
    RectSizeDisplayWidget* sizeDisplayWidget;
    QRegExpValidator* sizeValidator;

public:
    explicit BaseFileSaveDialog(QWidget *parent = nullptr);

signals:
    void sendSaveFile(SaveFileInfo info);
protected:
    // saveBtn & cancelBtn
    QWidget* init_SaveAndCancel();
    // savePathEdit & chooseSavePathBtn
    QWidget* init_SavePath();
    // fileFormBox & fileFormLook
    QWidget* init_FileFormChoose();
    // widthEdit & heightEdit
    QWidget* init_SizeSettings(bool flag = false);
    // sizeDisplayWidget
    void init_SizeDisplay();


private slots:
    virtual void do_Save() = 0;
    virtual void do_OpenSavePath() = 0;
    virtual void do_LookFileForm() = 0;
    virtual void do_SizeChanged() = 0;
    virtual void do_FileFormChanged() = 0;
};

class PlotFileSaveDialog : public BaseFileSaveDialog
{
    Q_OBJECT
private:
    QSize getSize();
    QSizeF getSizeF();
public:
    explicit PlotFileSaveDialog(QWidget *parent = nullptr);

    void setSize(QSize inSize);

private slots:
    void do_Save() override;
    void do_OpenSavePath() override;
    void do_LookFileForm() override;
    void do_SizeChanged() override;
    void do_FileFormChanged() override;
};

}

#endif // BASEFILESAVEDIALOG_HPP

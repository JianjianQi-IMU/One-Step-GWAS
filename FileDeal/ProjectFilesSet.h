#ifndef PROJECTFILESSET_H
#define PROJECTFILESSET_H

#include "BaseProject.hpp"
#include "FileFormClass.hpp"
#include <QString>

namespace FD {

class BaseProjectFilesSet
{
protected:
    ProjectInfo info;
public:
    BaseProjectFilesSet();
    virtual ~BaseProjectFilesSet();
    virtual QString getName() const
    {
        return QString();
    };

    ProjectInfo getProjectInfo() const
    {
        return info;
    }
}; // class BaseProjectFilesSet

class GWASProjectFilesSet : public BaseProjectFilesSet
{
protected:
    FileFormClass genoFileForm;
    QString genoFile;
    QString bimFile;
    QString covFile;
    QString pheFile;
    QString kinFile;
public:
    GWASProjectFilesSet();
    const QString& getGenoFile() const
    {
        return genoFile;
    }
    const QString& getBimFile() const
    {
        return bimFile;
    }
    const QString& getCovFile() const
    {
        return covFile;
    }
    const QString& getPheFile() const
    {
        return pheFile;
    }
    const QString& getKinFile() const
    {
        return kinFile;
    }
    FileFormClass getGenoFileForm() const
    {
        return genoFileForm;
    }
    void setGenoFile(const QString& Name)
    {
        genoFile = Name;
    }
    void setBimFile(const QString& Name)
    {
        bimFile = Name;
    }
    void setCovFile(const QString& Name)
    {
        covFile = Name;
    }
    void setPheFile(const QString& Name)
    {
        pheFile = Name;
    }
    void setKinFile(const QString& Name)
    {
        kinFile = Name;
    }
    void setGenoFileForm(FileFormClass form)
    {
        genoFileForm = form;
    }
    QString getName() const
    {
        return genoFile;
    }

    static const QString genoName;
    static const QString bimName;
    static const QString covName;
    static const QString kinName;
    static const QString pheName;
    static const QString GWASName;
};

}; // namespace FD

#endif // PROJECTFILESSET_H

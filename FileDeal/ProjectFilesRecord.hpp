#ifndef PROJECTFILESRECORD_HPP
#define PROJECTFILESRECORD_HPP

#include <QDomDocument>
#include <QQueue>

#include "ProjectFilesSet.h"

namespace FD {

class ProjectFilesRecordReader
{
protected:
    bool getGWASProject(QDomElement& elm, QQueue<BaseProjectFilesSet*>& outQueue);
    bool getBSAProject(QDomElement& elm, QQueue<BaseProjectFilesSet*>& outQueue);
public:
    ProjectFilesRecordReader();
    bool getProjects(const QString& hisFile, QQueue<BaseProjectFilesSet*>& outQueue);
};

class ProjectFilesRecordWriter
{
protected:
    QDomDocument outDoc;
    QDomElement outRootElm;

    void addProject(const BaseProjectFilesSet* inProject);
    void addGWASProject(const BaseProjectFilesSet* inProject);
    void addBSAProject(const BaseProjectFilesSet* inProject);
public:
    ProjectFilesRecordWriter();
    void write(const QString& outputFile);
    void addProjects(const QQueue<BaseProjectFilesSet*>& inProjects);
    void clear();
};

}; // namespace FD

#endif // PROJECTFILESRECORD_HPP

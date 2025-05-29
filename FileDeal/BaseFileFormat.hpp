#ifndef BASEFILEFORMAT_HPP
#define BASEFILEFORMAT_HPP

#include <QString>
#include <QVector>
#include <QMap>
#include <QDomDocument>

#include "FileFormClass.hpp"

namespace FD
{

class BaseFileFormat
{
public:
    FileFormClass idType;
    QString idStr;
    QString name;
    QString abbreviation;
    QString description;
    QString example;
    BaseFileFormat();
    void clear();
};

class FileFormatSet
{
protected:
    static QMap<FileFormClass, int> formFileMap;
    static QVector<BaseFileFormat> formFileVec;


    explicit FileFormatSet();
public:
    static BaseFileFormat getFileFormat(FileFormClass id);

protected:
    static void initInstance();
    static void readOneFileInfo(QDomNode& inNode, BaseFileFormat& outFormat);
};

}


#endif // BASEFILEFORMAT_HPP

#include "BaseFileFormat.hpp"

#include <QFile>

namespace FD
{

BaseFileFormat::BaseFileFormat()
{
    idType = FILEFORM_NONE;
}

void BaseFileFormat::clear()
{
    idType = FILEFORM_NONE;
    idStr.clear();
    name.clear();
    abbreviation.clear();
    description.clear();
    example.clear();
}

QMap<FileFormClass, int> FileFormatSet::formFileMap;

QVector<BaseFileFormat> FileFormatSet::formFileVec;

FileFormatSet::FileFormatSet()
{

}

BaseFileFormat FileFormatSet::getFileFormat(FileFormClass id)
{
    if (formFileVec.empty() || formFileMap.empty()) {
        initInstance();
    }
    if (formFileMap.find(id) == formFileMap.end()) {
        return BaseFileFormat();
    }
    return formFileVec[formFileMap[id]];
}

void FileFormatSet::initInstance()
{
    formFileMap.clear();
    formFileVec.clear();

    BaseFileFormat tmpFile;
    int iFile;

    QFile xmlFile(":/FileFormatInfo/FileFormatInfo.xml");
    if (!xmlFile.exists()) {
        return;
    }
    if (!xmlFile.open(QIODevice::ReadOnly))
    {
        return ;
    }
    QDomDocument doc;
    if (!doc.setContent(&xmlFile))
    {
        xmlFile.close();
        return ;
    }
    QDomElement root = doc.documentElement();
    if (root.tagName() != "FileFormatInfo") {
        return ;
    }
    QDomNodeList listFileFormat = root.elementsByTagName("FileFormat");
    for (iFile = 0; iFile < listFileFormat.size(); ++iFile) {
        QDomNode tmpNode = listFileFormat.at(iFile);
        readOneFileInfo(tmpNode, tmpFile);
        if (tmpFile.idType != FILEFORM_NONE) {
            formFileMap[tmpFile.idType] = formFileVec.size();
            formFileVec.append(tmpFile);
        }
    }
}

void FileFormatSet::readOneFileInfo(QDomNode &inNode, BaseFileFormat &outFormat)
{
    int iAttr;
    int iInfo;
    if (inNode.isElement()) {
        QDomNamedNodeMap tmpAttrs = inNode.attributes();
        outFormat.clear();
        if (tmpAttrs.isEmpty()) {
            return;
        }
        for (iAttr = 0; iAttr < tmpAttrs.size(); ++iAttr) {
            QDomAttr tmpAttr = tmpAttrs.item(iAttr).toAttr();
            QString tmpName = tmpAttr.name();
            QString tmpVal = tmpAttr.value();
            if (tmpName == "ID") {
                outFormat.idType = (FileFormClass)tmpVal.toInt();
            } else if (tmpName == "IDS") {
                outFormat.idStr = tmpVal;
            }
        }
        if (outFormat.idType == FILEFORM_NONE) {
            return;
        }
        QDomNodeList listFileInfo = inNode.childNodes();
        for (iInfo = 0; iInfo < listFileInfo.size(); ++iInfo) {
            QDomNode tmpInfoNode = listFileInfo.at(iInfo);
            if (tmpInfoNode.isCDATASection()) {
                QDomCDATASection tmpSec = tmpInfoNode.toCDATASection();
                if (tmpSec.nodeName() == "name") {
                    outFormat.name = tmpSec.data();
                } else if (tmpSec.nodeName() == "abbreviation") {
                    outFormat.abbreviation = tmpSec.data();
                } else if (tmpSec.nodeName() == "description") {
                    outFormat.description = tmpSec.data();
                } else if (tmpSec.nodeName() == "example") {
                    outFormat.example = tmpSec.data();
                }
            } else {
                QDomElement tmpElem = tmpInfoNode.toElement();
                if (tmpElem.tagName() == "name") {
                    outFormat.name = tmpElem.text();
                } else if (tmpElem.tagName() == "abbreviation") {
                    outFormat.abbreviation = tmpElem.text();
                } else if (tmpElem.tagName() == "description") {
                    outFormat.description = tmpElem.text();
                } else if (tmpElem.tagName() == "example") {
                    outFormat.example = tmpElem.text();
                }
            }
        }
    }
}

}

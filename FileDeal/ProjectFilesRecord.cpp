#include "ProjectFilesRecord.hpp"

#include <QFile>
#include <QTextStream>

namespace FD {

bool ProjectFilesRecordReader::getGWASProject(QDomElement &elm, QQueue<BaseProjectFilesSet*>& outQueue)
{
    QDomNodeList fileList = elm.elementsByTagName("File");
    int fileLen = fileList.size();
    GWASProjectFilesSet tSet;
    if(fileLen == 0) {
        return false;
    }
    for(int i = 0;i < fileLen; ++i)
    {
        QDomElement tElm = fileList.at(i).toElement();
        if(tElm.attribute("TypeID") == GWASProjectFilesSet::genoName) {
            if(!tElm.attribute("FormID").isEmpty()) {
                FileFormClass tForm = FileFormClass(tElm.attribute("FormID").toInt());
                tSet.setGenoFileForm(tForm);
            }
            tSet.setGenoFile(tElm.text());
        } else if(tElm.attribute("TypeID") == GWASProjectFilesSet::bimName) {
            tSet.setBimFile(tElm.text());
        } else if(tElm.attribute("TypeID") == GWASProjectFilesSet::pheName) {
            tSet.setPheFile(tElm.text());
        } else if(tElm.attribute("TypeID") == GWASProjectFilesSet::covName) {
            tSet.setCovFile(tElm.text());
        } else if(tElm.attribute("TypeID") == GWASProjectFilesSet::kinName) {
            tSet.setKinFile(tElm.text());
        }
    }
    GWASProjectFilesSet* outSet = new GWASProjectFilesSet;
    *outSet = tSet;
    outQueue.push_back(outSet);
    return true;
}

bool ProjectFilesRecordReader::getBSAProject(QDomElement &elm, QQueue<BaseProjectFilesSet*>& outQueue)
{
    return false;
}

ProjectFilesRecordReader::ProjectFilesRecordReader()
{

}

bool ProjectFilesRecordReader::getProjects(const QString &hisFile, QQueue<BaseProjectFilesSet *> &outQueue)
{
    QFile his(hisFile);
    if(!his.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QDomDocument doc;
    if(!doc.setContent(&his))
    {
        his.close();
        return false;
    }
    QDomElement root = doc.documentElement();
    QDomNodeList proList = root.elementsByTagName("Project");
    int proLen = proList.size();
    for(int i = 0;i < proLen;i++)
    {
        QDomElement tElm = proList.at(i).toElement();
        QString proTypeID = tElm.attribute("TypeID");
        if(proTypeID == GWASProjectFilesSet::GWASName)
        {
            getGWASProject(tElm,outQueue);
        }
    }
}

void ProjectFilesRecordWriter::addProject(const BaseProjectFilesSet *inProject)
{
    switch (inProject->getProjectInfo()) {
    case PROJECT_GWAS:
        addGWASProject(inProject);
        break;
    case PROJECT_BSA:
        addBSAProject(inProject);
        break;
    default:
        break;
    }
}

void ProjectFilesRecordWriter::addGWASProject(const BaseProjectFilesSet *inProject)
{
    const GWASProjectFilesSet* projectSet = dynamic_cast<const GWASProjectFilesSet*>(inProject);
    QDomElement proElm = outDoc.createElement("Project");
    QDomElement genoFile = outDoc.createElement("File");
    QDomElement bimFile = outDoc.createElement("File");
    QDomElement pheFile = outDoc.createElement("File");
    QDomElement covFile = outDoc.createElement("File");
    QDomElement kinFile = outDoc.createElement("File");

    genoFile.setTagName("File");
    genoFile.setAttribute("TypeID", GWASProjectFilesSet::genoName);
    genoFile.setAttribute("FormID", int(projectSet->getGenoFileForm()));
    genoFile.appendChild(outDoc.createTextNode(projectSet->getGenoFile()));

    bimFile.setTagName("File");
    bimFile.setAttribute("TypeID", GWASProjectFilesSet::bimName);
    bimFile.setAttribute("FormID", int(FILEFORM_BIM));
    bimFile.appendChild(outDoc.createTextNode(projectSet->getBimFile()));

    pheFile.setTagName("File");
    pheFile.setAttribute("TypeID", GWASProjectFilesSet::pheName);
    pheFile.setAttribute("FormID", int(FILEFORM_MY_PHE));
    pheFile.appendChild(outDoc.createTextNode(projectSet->getPheFile()));

    covFile.setTagName("File");
    covFile.setAttribute("TypeID", GWASProjectFilesSet::covName);
    covFile.setAttribute("FormID", int(FILEFORM_MY_COV));
    covFile.appendChild(outDoc.createTextNode(projectSet->getCovFile()));

    kinFile.setTagName("File");
    kinFile.setAttribute("TypeID", GWASProjectFilesSet::kinName);
    kinFile.setAttribute("FormID", int(FILEFORM_MY_KIN));
    kinFile.appendChild(outDoc.createTextNode(projectSet->getKinFile()));

    proElm.appendChild(genoFile);
    proElm.appendChild(bimFile);
    proElm.appendChild(pheFile);
    proElm.appendChild(covFile);
    proElm.appendChild(kinFile);
    proElm.setAttribute("TypeID", GWASProjectFilesSet::GWASName);

    outRootElm.appendChild(proElm);
}

void ProjectFilesRecordWriter::addBSAProject(const BaseProjectFilesSet *inProject)
{

}

FD::ProjectFilesRecordWriter::ProjectFilesRecordWriter()
{
    outRootElm = outDoc.createElement("Projects");
    outDoc.appendChild(outRootElm);
}

void ProjectFilesRecordWriter::write(const QString &outputFile)
{
    QFile outFile(outputFile);
    if(!outFile.open(QIODevice::WriteOnly))
    {
        return;
    }
    QTextStream outStream(&outFile);
    outDoc.save(outStream, 4);
}

void ProjectFilesRecordWriter::addProjects(const QQueue<BaseProjectFilesSet*> &inProjects)
{
    foreach(const BaseProjectFilesSet* project, inProjects)
    {
        addProject(project);
    }
}

void ProjectFilesRecordWriter::clear()
{
    outRootElm.clear();
}

}; // namespace FD

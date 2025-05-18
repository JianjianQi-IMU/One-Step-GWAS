#ifndef FILEFORMCLASS_HPP
#define FILEFORMCLASS_HPP

#include <QSizeF>
#include <QString>

namespace FD {

enum FileFormClass
{
    FILEFORM_NONE = 0,
    FILEFORM_TXT,
    FILEFORM_PDF,
    FILEFORM_SVG,
    FILEFORM_PNG,
    FILEFORM_JPEG,
    FILEFORM_JPG,
    FILEFORM_BMP,
    FILEFORM_VCF,
    FILEFORM_BED,
    FILEFORM_BIM,
    FILEFORM_PED,
    FILEFORM_PPED,
    FILEFORM_MY_COV,
    FILEFORM_MY_KIN,
    FILEFORM_MY_PHE,
    FILEFORM_MAXLEN,
};

class BaseFileFormInfoClass
{
protected:
    FileFormClass  idType;
public:
    QString filePath;

    BaseFileFormInfoClass()
    {
        idType = FILEFORM_NONE;
    }
    virtual ~BaseFileFormInfoClass()
    {

    }
    virtual FileFormClass getIDForm() const
    {
        return idType;
    }
    virtual void copyTo(BaseFileFormInfoClass*& inInfo) const
    {
        if (inInfo && inInfo -> getIDForm() == FILEFORM_NONE) {
            return ;
        }
        if (inInfo) {
            delete inInfo;
        }
        inInfo = new BaseFileFormInfoClass;
    }
};

class PDFFileFormInfo : public BaseFileFormInfoClass
{
public:
    QSizeF saveSize;

    PDFFileFormInfo()
    {
        idType = FILEFORM_PDF;
    }

    void copyTo(BaseFileFormInfoClass*& inInfo) const
    {
        if (inInfo && inInfo -> getIDForm() == FILEFORM_PDF) {
            *dynamic_cast<PDFFileFormInfo*>(inInfo) = *this;
            return ;
        }
        if (inInfo) {
            delete inInfo;
        }
        PDFFileFormInfo* newInfo = new PDFFileFormInfo;
        *newInfo = *this;
        inInfo = newInfo;
    }
};

class SVGFileFormInfo : public BaseFileFormInfoClass
{
public:
    QSizeF saveSize;

    SVGFileFormInfo()
    {
        idType = FILEFORM_SVG;
    }

    void copyTo(BaseFileFormInfoClass*& inInfo) const
    {
        if (inInfo && inInfo -> getIDForm() == FILEFORM_PDF) {
            *dynamic_cast<SVGFileFormInfo*>(inInfo) = *this;
            return ;
        }
        if (inInfo) {
            delete inInfo;
        }
        SVGFileFormInfo* newInfo = new SVGFileFormInfo;
        *newInfo = *this;
        inInfo = newInfo;
    }
};

class PNGFileFormInfo : public BaseFileFormInfoClass
{
public:
    QSize saveSize;

    PNGFileFormInfo()
    {
        idType = FILEFORM_PNG;
    }

    void copyTo(BaseFileFormInfoClass*& inInfo) const
    {
        if (inInfo && inInfo -> getIDForm() == FILEFORM_PNG) {
            *dynamic_cast<PNGFileFormInfo*>(inInfo) = *this;
            return ;
        }
        if (inInfo) {
            delete inInfo;
        }
        PNGFileFormInfo* newInfo = new PNGFileFormInfo;
        *newInfo = *this;
        inInfo = newInfo;
    }
};

}

#endif // FILEFORMCLASS_HPP

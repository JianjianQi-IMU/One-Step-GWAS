#ifndef FILE_DEAL_HPP
#define FILE_DEAL_HPP

#include "DataManager/DataManager.hpp"
#include "GenomeAnnotation/GenomeAnnotation.hpp"
#include "FileDeal/BaseProject.hpp"
#include "BSAanalysis/BSAProject.hpp"


namespace FD{

enum ChrLenReadTsvState
{
    CHRLENREADTSV_DEFAULT = 0,
    CHRLENREADTSV_OK = 1,
    CHRLENREADTSV_FILEPATHNULL = 2,
    CHRLENREADTSV_REPEATCHR = 3,
    CHRLENREADTSV_ERRORCOLNUM = 4,
};

class BIMNode
{
public:
    std::string name;
    int idChr;
    size_t pos;
    BIMNode() : pos(0), idChr(-1) {}
    BIMNode(const std::string& inName, int inIdChr, size_t inPos)
        : pos(inPos), idChr(inIdChr)
    {
        name = inName;
    }
    ~BIMNode() {}
};

class BIMLogPNode : public BIMNode
{
public:
    double statValue;
    double betax0;
    double PValue;
    double log10PValue;
    explicit BIMLogPNode()
        : statValue(0), betax0(0), PValue(0), log10PValue(0)
    {
        pos = 0;
        idChr= -1;
    }
    ~BIMLogPNode() {}
};

class BIMData
{
public:
    size_t       nMarker;
    BIMNode*     data;
    char**       chrName;
    int          nChr;
    bool         isValid;
    BIMData();
    ~BIMData();
    void clear();
    char** getChrName();
    int getNChr();
};

class BIMLogPData
{
public:
    size_t       nMarker;
    BIMLogPNode* data;
    char**       chrName;
    int          nChr;
    bool         isValid;
    BIMLogPData();
    ~BIMLogPData();
    void clear();
    char** getChrName();
    int getNChr();
};

class BEDread : public BaseFileRead
{
public:
    short fileTest;
    BEDread();
    ~BEDread();
    BEDread(const char* inFile);
    bool load(const char* inFile);
    bool statNum(size_t inNSample, size_t& outNum);
    bool readAll(size_t inNSample, BedData& out);

    static const size_t ReadScaleSize = 100000;
};

class TPolyPedread : public BaseFileRead
{
public:
    TPolyPedread();
    ~TPolyPedread();
    TPolyPedread(const char* inFile);
    bool readAll(PolyPedData& pData);
};

class BIMLogPread : public BaseFileRead
{
public:
    BIMLogPread();
    ~BIMLogPread();
    BIMLogPread(const char* inFile);
    bool readAll(BIMLogPData& out);
};

class BIMread : public BaseFileRead
{
public:
    explicit BIMread();
    explicit BIMread(const char* inFile);
    ~BIMread();
    bool readAll(BIMData& out);
};

class FAMread : public BaseFileRead
{
public:
    long long       sampleN;
    char**          data;
    long long*      popIndex;
    char**          popName;
    long long       nPop;
    FAMread();
    ~FAMread();
    FAMread(const char* inFile);
    bool clear();
    bool readAll();
    char* getSampleName(long long index);
};

class VCFread : public BaseFileRead
{
public:
    VCFread();
    ~VCFread();
    VCFread(const char* inFile);
    bool readAll(PolyPedData& pData, BIMLogPData& bimData);
};

class genomeRead
{
public:
    std::string geneTypeLabel;
    std::string geneIdKey;
    std::string geneAnnoKey;
    std::string CDSTypeLabel;
    std::string UTR5TypeLabel;
    std::string UTR3TypeLabel;

    std::fstream file;
    GenomeAnnotation* genome;
    std::vector<std::string> chrName;
    int nChr;
    std::map<std::string,int> idChrMap;
    bool isValid;
    explicit genomeRead();
    genomeRead(const char* inFile, char** inChrName, int inNchr);
    ~genomeRead();
    bool open(const char* inFile, char** inChrName, int inNchr);
    bool open(const char* inFile, const std::vector<std::string>& inChrName);
    bool readTsv();
    bool readGff();
    GenomeAnnotation* getGenome();
    void setGffLabel(const char* inGeneTypeLabel,
                     const char* inGeneIdKey,
                     const char* inGeneAnnoKey,
                     const char* inCDSTypeLabel = nullptr,
                     const char* inUTR5TypeLabel = nullptr,
                     const char* inUTR3TypeLabel = nullptr);
    void setGffLabelDefault();
};


bool pheRead(const char* inFile, MML::Mat& out);
bool kinRead(const char* inFile, MML::Mat& out);
bool covRead(const char* inFile, MML::Mat& out);
ChrLenReadTsvState chrLenReadTsv(const char *inFile,
                                 std::vector<std::string> &outChr,
                                 std::vector<long long>& outLen);
bool genomeSeqRead(const char* inFile, char** inChrName, long long inNChr,
                   const long long* inChrLen, GenomeSequence& genomeSeq);
bool PCAOutread(const char* inFile, MML::Mat& out, int nPC);

class GWASOutread
{
public:
    int logPCol;
    double* data;
    std::fstream file;
    bool isValid;
    size_t nMarkers;
    explicit GWASOutread();
    explicit GWASOutread(const char* inFile);
    ~GWASOutread();
    bool loadFile(const char* inFile);
    bool setLogCol(int iCol);
    bool readAll();
    double getValueAt(size_t id);
    const double* getValues();
};

class GWASProject : public BaseProject
{
public:
    BedData bed;
    BIMLogPData bim;
    PolyPedData pData;
    MML::Mat phe;
    MML::Mat cov;
    MML::Mat kin;
    bool isValid;
    bool isPolyploid;
    bool isKinValid;
    ReadState state;
    GWASProject();
    size_t getNSample() const;
    size_t getNMarker() const;
    size_t getNFactor() const;
    size_t getNPolyploid() const;
    ReadState read(const char* nameBed,
              const char* nameBim,
              const char* namePhe,
              const char* nameCov="\0",
              const char* nameKin="\0");
    ReadState readVCF(const char* nameVCF,
                      const char* namePhe,
                      const char* nameCov="\0",
                      const char* nameKin="\0");
    ReadState readTPolyPed(const char* nameTPed,
                       const char* nameBim,
                       const char* namePhe,
                       const char* nameCov="\0",
                       const char* nameKin="\0");
    bool saveOutput(const char* outFile) override;
    char** getChrName();
    int getNChr();
};

class BIMLogPNodeLessWithLogP
{
public:
    bool operator ()(const BIMLogPNode* a,const BIMLogPNode* b);
};

void deleteProject(const BaseProject* p);

};

#endif // FILE_DEAL_HPP

#ifndef GENOMEANNOTATION_HPP
#define GENOMEANNOTATION_HPP

#include <algorithm>
#include <list>
#include <vector>

//#include <QDebug>
//#include <QString>
#include <fstream>

class GeneAnnotation
{
public:
    char*                   name;
    int                     idChr;
    long long               start;
    long long               stop;
    char                    strand;
    char*                   annotation;
    std::list<long long>    CDS;
    /*
     * CDS [cds1start,cds1stop,
     *      cds2start,cds2stop,
     *      ...
     *      cdsNstart,cdsNstop]
*/
    long long            startUTR5;
    long long            stopUTR5;
    long long            startUTR3;
    long long            stopUTR3;
    explicit GeneAnnotation();
    GeneAnnotation(GeneAnnotation&& inGene);
    GeneAnnotation(const GeneAnnotation& inGene);
    GeneAnnotation(const char* inName,
                   int inIdChr,
                   long long inStart,
                   long long inStop,
                   char inStrand,
                   const char* inAnnotation = "",
                   const std::list<long long>& inCDS = std::list<long long>(),
                   long long inStartUTR5 = 0,
                   long long inStopUTR5 = 0,
                   long long inStartUTR3 = 0,
                   long long inStopUTR3 = 0);
    ~GeneAnnotation();
    bool contain(int inIdChr, long long inPos);
    bool addCDS(long long inStart, long long inStop);
    bool set5UTR(long long inStartUTR5, long long inStopUTR5);
    bool set3UTR(long long inStartUTR3, long long inStopUTR3);
    std::list<long long>* getCDSList();
    bool operator<(const GeneAnnotation& B) const;
    GeneAnnotation& operator=(const GeneAnnotation& inGene);
    GeneAnnotation& operator=(GeneAnnotation&& inGene);
    static int charMaxLen;
};

class GenomeAnnotation
{
private:
    char**             chrName;
    GeneAnnotation*    genes;
    long long*         firstI;
    long long*         lastI;
    short*             isNull;
    long long          nGenes;
    int                nChr;
    bool               isGeneModel;
    bool               isValid;
public:
    explicit GenomeAnnotation();
    GenomeAnnotation(char** inChrName, int inNChr,
                     GeneAnnotation* inGenes, long long inNGenes);
    GenomeAnnotation(const std::vector<std::string>& inChrName,
                     GeneAnnotation* inGenes, long long inNGenes);
    GenomeAnnotation(char** inChrName,int inNChr, long long inNGenes);
    GenomeAnnotation(const std::vector<std::string>& inChrName, long long inNGenes);
    ~GenomeAnnotation();
    bool loadChromosome(char** inChrName, int inNChr);
    bool loadChromosome(const std::vector<std::string>& inChrName);
    bool loadGenes(GeneAnnotation* inGenes, long long inNGenes);
    bool initGenes(long long inNGenes);
    bool sortGenes();
    void setValid(bool boolValue);
    bool getValid();
    long long getNGenes();
    const char* getChrName(int id) const;
    bool isEmptyChr(long long idChr);
    long long getChrFirsti(long long idChr);
    long long getChrLasti(long long idChr);
    GeneAnnotation* getGene(long long i);
    long long search(const char* inKeywords) const;
    long long search(const char* inKeywords, long long* outListI, long long& outNum, long long outMaxN) const;
    long long search(int inChrI, long double position, unsigned short mode = GA_START) const;   // find the first gene whose position
    long long search(int inChrI, long long position, unsigned short mode = GA_START) const;        // is bigger than or equal to
    long long searchLower(int inChrI, long double position, unsigned short mode = GA_START) const; // find the last gene whose position
    long long searchLower(int inChrI, long long position, unsigned short mode = GA_START) const;      // is smaller than or equal to
    bool haveGeneModel() const;
    void setGeneModel(bool flag);
    void clear();
    GenomeAnnotation& operator=(const GenomeAnnotation& inGenome);
    static const unsigned short GA_START = 0;
    static const unsigned short GA_STOP = 1;

    static int chrNameMaxLen;

//    void print();
};

class GenomeSequence
{
private:
    char**  chrName;
    char**  chrSeq;
    int     nChr;
    long long* chrLen;

public:
    explicit GenomeSequence();
    explicit GenomeSequence(char** inChrName, int inNChr, const long long* inChrLen);
    ~GenomeSequence();
    void setInitialization(char** inChrName, int inNChr, const long long* inChrLen);
    bool insertSeq(int iChr, long long iPosition, const char* inSeq);
    char* getChrSeq(int iChr); // iChr -> [0,nChr-1]
    char* getSeq(int iChr, long long iPosition); // iPosition -> [1,chrLen]
    char* getSeq(int iChr, long double iPosition);
    void clear();
    static int chrNameMaxLen;
    static short Base2Value(char  inBase);
    static char  Value2Base(short inValue);

    void write(const char* file, long long ip = 1);
};

#endif // GENOMEANNOTATION_HPP

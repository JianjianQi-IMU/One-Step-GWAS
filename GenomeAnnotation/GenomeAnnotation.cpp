#include "GenomeAnnotation.hpp"

int GeneAnnotation::charMaxLen = 4096;

GeneAnnotation::GeneAnnotation()
{
    name=nullptr;
    idChr=0;
    start=0;
    stop=0;
    strand=0;
    annotation=nullptr;
    startUTR5 = 0;
    stopUTR5  = 0;
    startUTR3 = 0;
    stopUTR3  = 0;
}

GeneAnnotation::GeneAnnotation(GeneAnnotation &&inGene)
    :GeneAnnotation()
{
    if(inGene.name){
        name=inGene.name;
        inGene.name=nullptr;
    }
    else name=nullptr;
    if(inGene.annotation){
        annotation=inGene.annotation;
        inGene.annotation=nullptr;
    }
    idChr     = inGene.idChr    ;
    start     = inGene.start    ;
    stop      = inGene.stop     ;
    strand    = inGene.strand   ;
    CDS       = inGene.CDS      ;
    startUTR5 = inGene.startUTR5;
    stopUTR5  = inGene.stopUTR5 ;
    startUTR3 = inGene.startUTR3;
    stopUTR3  = inGene.stopUTR3 ;
}

GeneAnnotation::GeneAnnotation(const GeneAnnotation &inGene)
    :GeneAnnotation()
{
    if(!name) name=new char[charMaxLen];
    if(!annotation) annotation=new char[charMaxLen];
    int i;
    for(i=0;i<charMaxLen-1&&inGene.name[i];++i){
        name[i]=inGene.name[i];
    }
    name[i]='\0';
    for(i=0;i<charMaxLen-1&&inGene.annotation[i];++i){
        annotation[i]=inGene.annotation[i];
    }
    annotation[i]='\0';
    idChr     = inGene.idChr    ;
    start     = inGene.start    ;
    stop      = inGene.stop     ;
    strand    = inGene.strand   ;
    CDS       = inGene.CDS      ;
    startUTR5 = inGene.startUTR5;
    stopUTR5  = inGene.stopUTR5 ;
    startUTR3 = inGene.startUTR3;
    stopUTR3  = inGene.stopUTR3 ;
}

GeneAnnotation::GeneAnnotation(const char* inName,
                               int inIdChr,
                               long long inStart,
                               long long inStop,
                               char inStrand,
                               const char* inAnnotation,
                               const std::list<long long>& inCDS,
                               long long inStartUTR5,
                               long long inStopUTR5,
                               long long inStartUTR3,
                               long long inStopUTR3)
    :idChr(inIdChr),start(inStart),stop(inStop),strand(inStrand),
      startUTR5(inStartUTR5),stopUTR5(inStopUTR5),
      startUTR3(inStartUTR3),stopUTR3(inStopUTR3)
{
    name=new char[charMaxLen];
    int i;
    for(i=0;i<charMaxLen-1&&inName[i];++i){
        name[i]=inName[i];
    }
    name[i]='\0';
    if(inAnnotation){
        annotation=new char[charMaxLen];
        for(i=0;i<charMaxLen-1&&inAnnotation[i];++i){
            annotation[i]=inAnnotation[i];
        }
        annotation[i]='\0';
    }
    CDS = inCDS;
}

GeneAnnotation::~GeneAnnotation()
{
    if(name) delete [] name;
    if(annotation) delete [] annotation;
}

bool GeneAnnotation::contain(int inIdChr, long long inPos)
{
    if(inIdChr==idChr){
        if(inPos>=start&&inPos<=stop) return true;
    }
    return false;
}

bool GeneAnnotation::addCDS(long long inStart, long long inStop)
{
    if(inStart<start||inStart>stop||
       inStop<start||inStop>stop) return false;
    CDS.push_back(inStart);
    CDS.push_back(inStop);
    return true;
}

bool GeneAnnotation::set5UTR(long long inStartUTR5, long long inStopUTR5)
{
    if(inStartUTR5<start||inStartUTR5>stop||
       inStopUTR5<start||inStopUTR5>stop) return false;
    startUTR5=inStartUTR5;
    stopUTR5=inStopUTR5;
    return true;
}

bool GeneAnnotation::set3UTR(long long inStartUTR3, long long inStopUTR3)
{
    if(inStartUTR3<start||inStartUTR3>stop||
       inStopUTR3<start||inStopUTR3>stop) return false;
    startUTR3=inStartUTR3;
    stopUTR3=inStopUTR3;
    return true;
}

std::list<long long>* GeneAnnotation::getCDSList()
{
    return &CDS;
}

bool GeneAnnotation::operator<(const GeneAnnotation &B) const
{
    if(idChr!=B.idChr) return idChr<B.idChr;
    return start<B.start;
}

GeneAnnotation &GeneAnnotation::operator=(const GeneAnnotation &inGene)
{
    if(!name) name=new char[charMaxLen];
    if(!annotation) annotation=new char[charMaxLen];
    int i;
    for(i=0;i<charMaxLen-1&&inGene.name[i];++i){
        name[i]=inGene.name[i];
    }
    name[i]='\0';
    for(i=0;i<charMaxLen-1&&inGene.annotation[i];++i){
        annotation[i]=inGene.annotation[i];
    }
    annotation[i]='\0';
    idChr     = inGene.idChr    ;
    start     = inGene.start    ;
    stop      = inGene.stop     ;
    strand    = inGene.strand   ;
    CDS       = inGene.CDS      ;
    startUTR5 = inGene.startUTR5;
    stopUTR5  = inGene.stopUTR5 ;
    startUTR3 = inGene.startUTR3;
    stopUTR3  = inGene.stopUTR3 ;
    return *this;
}

GeneAnnotation &GeneAnnotation::operator=(GeneAnnotation&& inGene){
    if(name) delete [] name;
    if(annotation) delete [] annotation;
    if(inGene.name){
        name=inGene.name;
        inGene.name=nullptr;
    }
    else name=nullptr;
    if(inGene.annotation){
        annotation=inGene.annotation;
        inGene.annotation=nullptr;
    }
    idChr     = inGene.idChr    ;
    start     = inGene.start    ;
    stop      = inGene.stop     ;
    strand    = inGene.strand   ;
    CDS       = inGene.CDS      ;
    startUTR5 = inGene.startUTR5;
    stopUTR5  = inGene.stopUTR5 ;
    startUTR3 = inGene.startUTR3;
    stopUTR3  = inGene.stopUTR3 ;
    return *this;
}

int GenomeAnnotation::chrNameMaxLen = 256;

GenomeAnnotation::GenomeAnnotation()
{
    chrName=nullptr;
    genes=nullptr;
    firstI=nullptr;
    lastI=nullptr;
    isNull=nullptr;
    nChr=0;
    nGenes=0;
    isGeneModel=false;
    isValid=false;
}

GenomeAnnotation::GenomeAnnotation(char **inChrName, int inNChr, GeneAnnotation *inGenes, long long inNGenes)
    :GenomeAnnotation()
{
    if(!loadChromosome(inChrName,inNChr)){
        clear();
        return;
    }
    if(!loadGenes(inGenes,inNGenes)){
        return;
    }
    isValid=true;
}

GenomeAnnotation::GenomeAnnotation(const std::vector<std::string> &inChrName, GeneAnnotation *inGenes, long long inNGenes)
    :GenomeAnnotation()
{
    if(!loadChromosome(inChrName)){
        clear();
        return;
    }
    if(!loadGenes(inGenes,inNGenes)){
        return;
    }
    isValid=true;
}

GenomeAnnotation::GenomeAnnotation(char **inChrName, int inNChr, long long inNGenes)
    :GenomeAnnotation()
{
    if(!loadChromosome(inChrName,inNChr)){
        clear();
        return;
    }
    if(!initGenes(inNGenes)){
        return;
    }
}

GenomeAnnotation::GenomeAnnotation(const std::vector<std::string> &inChrName, long long inNGenes)
    :GenomeAnnotation()
{
    if(!loadChromosome(inChrName)){
        clear();
        return;
    }
    if(!initGenes(inNGenes)){
        return;
    }
}

GenomeAnnotation::~GenomeAnnotation()
{
    clear();
}

bool GenomeAnnotation::loadChromosome(char **inChrName, int inNChr)
{
    if(!inChrName||!inNChr) return false;
    unsigned int i,j;
    if(inNChr!=nChr){
        if(chrName){
            for(i=0;(int)i<nChr;++i) delete [] chrName[i];
            delete [] chrName;
        }
        if(firstI) delete [] firstI;
        if(lastI) delete [] lastI;
        if(isNull) delete [] isNull;
        chrName=new char*[inNChr];
        firstI=new long long[inNChr];
        lastI=new long long[inNChr];
        isNull=new short[inNChr];
        for(i=0;(int)i<inNChr;++i) chrName[i]=new char[chrNameMaxLen];
    }
    nChr=inNChr;
    for(i=0;(int)i<inNChr;++i){
        for(j=0;(int)j<chrNameMaxLen-1&&inChrName[i][j];++j){
            chrName[i][j]=inChrName[i][j];
        }
        chrName[i][j]='\0';
    }
    return true;
}

bool GenomeAnnotation::loadChromosome(const std::vector<std::string> &inChrName)
{
    int inNChr = inChrName.size();
    int i,j;
    if(inNChr <= 0) return false;
    if(inNChr!=nChr){
        if(chrName){
            for(i=0;(int)i<nChr;++i) delete [] chrName[i];
            delete [] chrName;
        }
        if(firstI) delete [] firstI;
        if(lastI) delete [] lastI;
        if(isNull) delete [] isNull;
        chrName=new char*[inNChr];
        firstI=new long long[inNChr];
        lastI=new long long[inNChr];
        isNull=new short[inNChr];
        for(i=0;(int)i<inNChr;++i) chrName[i]=new char[chrNameMaxLen];
    }
    nChr=inNChr;
    for(i=0;(int)i<inNChr;++i){
        for(j=0;(int)j<chrNameMaxLen-1&&j<inChrName[i].size();++j){
            chrName[i][j]=inChrName[i][j];
        }
        chrName[i][j]='\0';
    }
    return true;
}

bool GenomeAnnotation::loadGenes(GeneAnnotation *inGenes, long long inNGenes)
{
    if(!chrName) return false;
    if(!inGenes||!inNGenes) return false;
    if(inNGenes!=nGenes){
        if(genes) delete [] genes;
        genes=new GeneAnnotation[inNGenes+1];
    }
    nGenes=inNGenes;
    for(long long i=0;i<nGenes;++i){
        genes[i]=inGenes[i];
    }
    std::sort(genes,&(genes[nGenes]));
    for(long long i=0;i<nChr;++i) firstI[i]=nGenes,lastI[i]=0;
    for(long long i=0;i<nGenes;++i){
        firstI[genes[i].idChr]=std::min(firstI[genes[i].idChr],i);
        lastI[genes[i].idChr]=std::max(lastI[genes[i].idChr],i);
    }
    for(long long i=0;i<nChr;++i){
        if(firstI[i]==nGenes) isNull[i]=1;
        else isNull[i]=0;
    }
    return true;
}

bool GenomeAnnotation::initGenes(long long inNGenes)
{
    if(!chrName||!inNGenes) return false;
    genes = new GeneAnnotation[inNGenes+1];
    nGenes=inNGenes;
    return true;
}

bool GenomeAnnotation::sortGenes()
{
    if(!genes||nGenes==0) return false;
    std::sort(genes,&(genes[nGenes]));
    for(long long i=0;i<nChr;++i) firstI[i]=nGenes,lastI[i]=0;
    for(long long i=0;i<nGenes;++i){
        firstI[genes[i].idChr]=std::min(firstI[genes[i].idChr],i);
        lastI[genes[i].idChr]=std::max(lastI[genes[i].idChr],i);
    }
    for(long long i=0;i<nChr;++i){
        if(firstI[i]==nGenes) isNull[i]=1;
        else isNull[i]=0;
    }
    isValid=true;
    return true;
}

void GenomeAnnotation::setValid(bool boolValue)
{
    isValid=boolValue;
}

bool GenomeAnnotation::getValid()
{
    return isValid;
}

long long GenomeAnnotation::getNGenes()
{
    return nGenes;
}

const char *GenomeAnnotation::getChrName(int id) const
{
    return chrName[id];
}

bool GenomeAnnotation::isEmptyChr(long long idChr)
{
    return bool(isNull[idChr]);
}

long long GenomeAnnotation::getChrFirsti(long long idChr)
{
    return firstI[idChr];
}

long long GenomeAnnotation::getChrLasti(long long idChr)
{
    return lastI[idChr];
}

GeneAnnotation *GenomeAnnotation::getGene(long long i)
{
    if(i>=nGenes) return nullptr;
    return &(genes[i]);
}

long long GenomeAnnotation::search(const char *inKeywords) const
{
    if(!isValid) return -1;
    long long iGenes;
    char *source;
    int i,j;
    int sLen,kLen;
    const int maxCharLim=256;
    for(i=0;inKeywords[i];++i) ;
    kLen=i;
    int *badChar=new int[maxCharLim];
    int *goodPos=new int[kLen+1];
    int *goodShift=new int[kLen+1];
    for(i=0; i<maxCharLim; i++) badChar[i] = -1;
    for(i=0; i<kLen; i++) badChar[int(inKeywords[i])] = i;
    for(i=0;i<kLen+1;i++) goodShift[i]=0;
    i=kLen,j=kLen+1;
    goodPos[i]=j;
    while(i>0){
        while(j<=kLen&&inKeywords[i-1]!=inKeywords[j-1]){
            if(goodShift[j]==0) goodShift[j]=j-i;
            j=goodPos[j];
        }
        i--;j--;
        goodPos[i]=j;
    }
    j=goodPos[0];
    for(i=0;i<=kLen;i++){
        if(goodShift[i]==0) goodShift[i]=j;
        if (i==j) j=goodPos[j];
    }
    for(iGenes=0;iGenes<(long long)(nGenes);++iGenes){
        source=genes[iGenes].name;
        for(i=0;source[i];++i) ;
        sLen=i;
        i=0;
        while(i<=(sLen-kLen)){
            j=kLen-1;
            while(j>=0&&inKeywords[j]==source[i+j]) j--;
            if(j<0) break;
            else i+=std::max(goodShift[j+1],std::max(1, j-badChar[int(source[i+j])]));
        }
        if(i<=(sLen-kLen)) break;
    }
    delete [] goodPos;
    delete [] goodShift;
    delete [] badChar;
    if(iGenes<(long long)(nGenes)) return iGenes;
    return -1;
}

long long GenomeAnnotation::search(const char *inKeywords, long long *outListI, long long& outNum, long long outMaxN) const
{
    if(!isValid) return -1;
    if(outMaxN==0) return -1;
    long long iGenes;
    char *source;
    int i,j;
    int sLen,kLen;
    outNum=0;
    const int maxCharLim=256;
    for(i=0;inKeywords[i];++i) ;
    kLen=i;
    int *badChar=new int[maxCharLim];
    int *goodPos=new int[kLen+1];
    int *goodShift=new int[kLen+1];
    for(i=0; i<maxCharLim; i++) badChar[i] = -1;
    for(i=0; i<kLen; i++) badChar[int(inKeywords[i])] = i;
    for(i=0;i<kLen+1;i++) goodShift[i]=0;
    i=kLen,j=kLen+1;
    goodPos[i]=j;
    while(i>0){
        while(j<=kLen&&inKeywords[i-1]!=inKeywords[j-1]){
            if(goodShift[j]==0) goodShift[j]=j-i;
            j=goodPos[j];
        }
        i--;j--;
        goodPos[i]=j;
    }
    j=goodPos[0];
    for(i=0;i<=kLen;i++){
        if(goodShift[i]==0) goodShift[i]=j;
        if (i==j) j=goodPos[j];
    }
    for(iGenes=0;iGenes<(long long)(nGenes);++iGenes){
        source=genes[iGenes].name;
        for(i=0;source[i];++i) ;
        sLen=i;
        i=0;
        while(i<=(sLen-kLen)){
            j=kLen-1;
            while(j>=0&&inKeywords[j]==source[i+j]) j--;
            if(j<0) break;
            else i+=std::max(goodShift[j+1],std::max(1, j-badChar[int(source[i+j])]));
        }
        if(i<=(sLen-kLen)){
            outListI[outNum]=iGenes;
            outNum++;
        }
        if(outNum>=outMaxN) break;
    }
    delete [] goodPos;
    delete [] goodShift;
    delete [] badChar;
    if(outNum) return outListI[outNum-1];
    return -1;
}

long long GenomeAnnotation::search(int inChrI, long double position, unsigned short mode) const
{
    if(!isValid) return -1;
    if(isNull[inChrI]) return -1;
    long long i=firstI[inChrI],j=lastI[inChrI],m;
    while(i<=j){
        m=(i+j)/2;
        if(mode==GA_START){
            if((long double)(genes[m].start)>=position) j=m-1;
            else i=m+1;
        }
        else{
            if((long double)(genes[m].stop)>=position) j=m-1;
            else i=m+1;
        }
    }
    if(i>(long long)(lastI[inChrI])) return -1;
    return i;
}

long long GenomeAnnotation::search(int inChrI, long long position, unsigned short mode) const
{
    if(!isValid) return -1;
    if(isNull[inChrI]) return -1;
    long long i=firstI[inChrI],j=lastI[inChrI],m;
    while(i<=j){
        m=(i+j)/2;
        if(mode==GA_START){
            if(genes[m].start>=position) j=m-1;
            else i=m+1;
        }
        else{
            if(genes[m].stop>=position) j=m-1;
            else i=m+1;
        }
    }
    if(i>(long long)(lastI[inChrI])) return -1;
    return i;
}

long long GenomeAnnotation::searchLower(int inChrI, long double position, unsigned short mode) const
{
    if(!isValid) return -1;
    if(isNull[inChrI]) return -1;
    long long i=firstI[inChrI],j=lastI[inChrI],m;
    while(i<=j){
        m=(i+j)/2;
        if(mode==GA_START){
            if((long double)(genes[m].start)>position) j=m-1;
            else i=m+1;
        }
        else{
            if((long double)(genes[m].stop)>position) j=m-1;
            else i=m+1;
        }
    }
    if(j<0) return -1;
    return j;
}

long long GenomeAnnotation::searchLower(int inChrI, long long position, unsigned short mode) const
{
    if(!isValid) return -1;
    if(isNull[inChrI]) return -1;
    long long i=firstI[inChrI],j=lastI[inChrI],m;
    while(i<=j){
        m=(i+j)/2;
        if(mode==GA_START){
            if(genes[m].start>position) j=m-1;
            else i=m+1;
        }
        else{
            if(genes[m].stop>position) j=m-1;
            else i=m+1;
        }
    }
    if(j<0) return -1;
    return j;
}

bool GenomeAnnotation::haveGeneModel() const
{
    return isGeneModel;
}

void GenomeAnnotation::setGeneModel(bool flag)
{
    isGeneModel=flag;
}

void GenomeAnnotation::clear()
{
    size_t i;
    if(chrName){
        for(i=0;(int)i<nChr;++i) delete [] chrName[i];
        delete [] chrName;
    }
    if(genes) delete [] genes;
    if(firstI) delete [] firstI;
    if(lastI) delete [] lastI;
    if(isNull) delete [] isNull;
    chrName=nullptr;
    genes=nullptr;
    firstI=nullptr;
    lastI=nullptr;
    isNull=nullptr;
    nChr=0;
    nGenes=0;
    isValid=false;
}

GenomeAnnotation &GenomeAnnotation::operator=(const GenomeAnnotation &inGenome)
{
    long long i,j;
    if(inGenome.nChr!=nChr){
        if(chrName){
            for(i=0;i<nChr;++i) delete [] chrName[i];
            delete [] chrName;
        }
        if(firstI) delete [] firstI;
        if(lastI) delete [] lastI;
        if(isNull) delete [] isNull;
        chrName=new char*[inGenome.nChr];
        firstI=new long long[inGenome.nChr];
        lastI=new long long[inGenome.nChr];
        isNull=new short[inGenome.nChr];
        for(i=0;i<inGenome.nChr;++i) chrName[i]=new char[chrNameMaxLen];
    }
    nChr=inGenome.nChr;
    if(nGenes!=inGenome.nGenes){
        if(genes) delete [] genes;
        genes=new GeneAnnotation[inGenome.nGenes+1];
    }
    nGenes=inGenome.nGenes;
    for(i=0;i<nChr;++i){
        for(j=0;j<chrNameMaxLen-1&&(inGenome.chrName[j]);++j){
            chrName[i][j]=(inGenome.chrName)[i][j];
        }
        chrName[i][j]='\0';
        firstI[i]=(inGenome.firstI)[i];
        lastI[i]=(inGenome.lastI)[i];
        isNull[i]=(inGenome.isNull)[i];
    }
    for(i=0;i<nGenes;++i){
        genes[i]=(inGenome.genes)[i];
    }
    isValid=inGenome.isValid;
    isGeneModel=inGenome.isGeneModel;
    return *this;
}

//void GenomeAnnotation::print()
//{
//    size_t i,j,k;
//    QString seq;
//    for(i=0;i<nChr;++i) {seq+=chrName[i];seq+=' ';}
//    qDebug() << "chr name:" <<seq;
//    seq.clear();
//    for(i=0;i<nGenes;i++){
//        seq+=genes[i].name;
//        seq+=' ';
//        seq+=QString::number(genes[i].idChr);
//        seq+=' ';
//        seq+=QString::number(genes[i].start);
//        seq+=' ';
//        seq+=QString::number(genes[i].stop);
//        seq+=' ';
//        seq+=genes[i].strand;
//        seq+=' ';
//        seq+=genes[i].annotation;
//        qDebug() << seq;
//        seq.clear();
//    }
//    seq+="a_b:";
//    for(i=0;i<nChr;++i){
//        seq+=' ';
//        seq+=QString::number(firstI[i])+'_'+QString::number(lastI[i]);
//    }
//    qDebug() << seq;
//    seq.clear();
//    seq+="is null:";
//    for(i=0;i<nChr;++i){
//        seq+=' ';
//        seq+=QString::number(isNull[i]);
//    }
//    qDebug() << seq;
//}

int GenomeSequence::chrNameMaxLen = 256;

GenomeSequence::GenomeSequence()
{
    chrName = nullptr;
    chrSeq  = nullptr;
    chrLen  = nullptr;
}

GenomeSequence::GenomeSequence(char** inChrName,
                               int inNChr,
                               const long long* inChrLen)
    :GenomeSequence()
{
    setInitialization(inChrName,inNChr,inChrLen);
}

GenomeSequence::~GenomeSequence()
{
    clear();
}

void GenomeSequence::setInitialization(char** inChrName,
                                       int inNChr,
                                       const long long* inChrLen)
{
    long long i,j;
    clear();
    nChr=inNChr;
    chrName = new char*[nChr];
    chrSeq  = new char*[nChr];
    chrLen  = new long long[nChr];
    for(i=0;i<inNChr;++i){
        chrName[i] = new char[chrNameMaxLen];
        chrSeq[i]  = new char[(inChrLen[i]+1)/2+1];
        for(j=0;(int)j<chrNameMaxLen-1&&inChrName[i][j];++j){
            chrName[i][j]=inChrName[i][j];
        }
        chrName[i][j]='\0';
        chrLen[i]=inChrLen[i];
    }
}

bool GenomeSequence::insertSeq(int iChr,long long iPosition,const char* inSeq)
{
    if(iChr>=nChr||iPosition<0) return false;
    if(chrLen[iChr]<iPosition) return false;
    long long i=0,i2=0;
    short flag;
    if((iPosition+1)%2==0) flag=0;
    else flag=1;
    while(inSeq[i]&&(i+iPosition)<=chrLen[iChr]){
        if(!flag){
            chrSeq[iChr][(iPosition-1)/2+i2]=(Base2Value(inSeq[i])<<4)|(chrSeq[iChr][(iPosition-1)/2+i2]&char(0b1111));

        }
        else{
            chrSeq[iChr][(iPosition-1)/2+i2]=Base2Value(inSeq[i])|(chrSeq[iChr][(iPosition-1)/2+i2]&char(0b11110000));
            ++i2;
        }
        flag=(flag+1)%2;
        ++i;
    }
    return true;
}

char *GenomeSequence::getChrSeq(int iChr)
{
    if(iChr<nChr){
        return chrSeq[iChr];
    }
    return nullptr;
}

char *GenomeSequence::getSeq(int iChr, long long iPosition)
{
    if(iChr>=nChr) return nullptr;
    if(chrLen[iChr]<iPosition) return nullptr;
    return &(chrSeq[iChr][(iPosition-1)/2]);
}

char *GenomeSequence::getSeq(int iChr, long double iPosition)
{
    if(iChr>=nChr||iPosition<0) return nullptr;
    long long iPos=std::abs(iPosition-0.5);
    if(chrLen[iChr]<iPos) return nullptr;
    return (chrSeq[iChr]+iPos/2);
}

void GenomeSequence::clear()
{
    long long i;
    if(chrName){
        for(i=0;i<nChr;++i){
            delete [] chrName[i];
        }
        delete [] chrName;
    }
    if(chrSeq){
        for(i=0;i<nChr;++i){
            delete [] chrSeq[i];
        }
        delete [] chrSeq;
    }
    if(chrLen) delete [] chrLen;
}

short GenomeSequence::Base2Value(char inBase)
{
    switch(inBase){
    case 'a':
    case 'A':
        return 1;
    case 't':
    case 'T':
    case 'u':
    case 'U':
        return 2;
    case 'c':
    case 'C':
        return 4;
    case 'g':
    case 'G':
        return 8;
    case 'r':
    case 'R':
        return 9;
    case 'y':
    case 'Y':
        return 6;
    case 'm':
    case 'M':
        return 5;
    case 'k':
    case 'K':
        return 10;
    case 's':
    case 'S':
        return 12;
    case 'w':
    case 'W':
        return 3;
    case 'h':
    case 'H':
        return 7;
    case 'b':
    case 'B':
        return 14;
    case 'v':
    case 'V':
        return 13;
    case 'd':
    case 'D':
        return 11;
    case 'n':
    case 'N':
        return 15;
    case '-':
        return 0;
    default:
        return -1;
    }
}

char GenomeSequence::Value2Base(short inValue)
{
    switch (inValue) {
    case 1:
        return 'A';
    case 2:
        return 'T';
    case 3:
        return 'W';
    case 4:
        return 'C';
    case 5:
        return 'M';
    case 6:
        return 'Y';
    case 7:
        return 'H';
    case 8:
        return 'G';
    case 9:
        return 'R';
    case 10:
        return 'K';
    case 11:
        return 'D';
    case 12:
        return 'S';
    case 13:
        return 'V';
    case 14:
        return 'B';
    case 15:
        return 'N';
    case 0:
        return '-';
    default:
        return '.';
    }
}

void GenomeSequence::write(const char *file,long long ip)
{
    std::fstream oF(file,std::ios::out);
    std::string outLine;
    char* tC;
    short tmp,b1,b2;
    for(long long i=0;i<nChr;++i){
        oF << chrName[i] << '\n';
        outLine.clear();
        for(long long j=ip;j<=chrLen[i];++j){
            tC=(getSeq(i,(long double)j+0.6));
            if(!tC) continue;
            tmp=*tC;
            b2=tmp&0b1111;
            tmp>>=4;
            b1=tmp&0b1111;
            if(j%2==0) outLine+=Value2Base(b1);
            else outLine+=Value2Base(b2);
        }
        oF << outLine << '\n';
    }
    oF.close();
}


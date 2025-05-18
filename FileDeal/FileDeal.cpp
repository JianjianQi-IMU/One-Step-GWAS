#include "FileDeal.hpp"

FD::BIMData::BIMData()
{
    nMarker = 0;
    data    = nullptr;
    chrName = nullptr;
    nChr    = 0;
    isValid = false;
}

FD::BIMData::~BIMData()
{
    clear();
}

void FD::BIMData::clear()
{
    if(data) delete [] data;
    if(chrName){
        for(int iChr=0;iChr<nChr;++iChr) delete [] chrName[iChr];
        delete [] chrName;
    }
    nMarker = 0;
    data    = nullptr;
    chrName = nullptr;
    nChr    = 0;
    isValid = false;
}

char **FD::BIMData::getChrName()
{
    return chrName;
}

int FD::BIMData::getNChr()
{
    return nChr;
}

FD::BIMLogPData::BIMLogPData()
{
    nMarker = 0;
    data    = nullptr;
    chrName = nullptr;
    nChr    = 0;
    isValid = false;
}

FD::BIMLogPData::~BIMLogPData()
{
    clear();
}

void FD::BIMLogPData::clear()
{
    if(data) delete [] data;
    if(chrName){
        for(int iChr=0;iChr<nChr;++iChr) delete [] chrName[iChr];
        delete [] chrName;
    }
    nMarker = 0;
    data    = nullptr;
    chrName = nullptr;
    nChr    = 0;
    isValid = false;
}

char **FD::BIMLogPData::getChrName()
{
    return chrName;
}

int FD::BIMLogPData::getNChr()
{
    return nChr;
}

FD::BEDread::BEDread():BaseFileRead()
{
    fileTest=-1;
}

FD::BEDread::~BEDread(){}

FD::BEDread::BEDread(const char* inFile):BEDread()
{
    load(inFile);
}

bool FD::BEDread::load(const char* inFile)
{
    char tilte[3];
    file.open(inFile,std::ios::in|std::ios::binary);
    isValid=true;
    file.read(tilte,3);
    if((tilte[0]^0b01101100)!=0||
       (tilte[1]^0b00011011)!=0||
       short(tilte[2])!=1)
    {
        fileTest=0;
    }
    else fileTest=1;

    return isValid;
}

bool FD::BEDread::statNum(size_t inNSample,size_t &outNum)
{
    if(!inNSample) return false;
    size_t endPos=0;
    if(!isValid) return false;
    file.clear();
    file.seekg(0,std::ios_base::end);
    endPos=static_cast<size_t>(file.tellg());
    file.clear();
    file.seekg(3,std::ios_base::beg);
    outNum=(endPos-3)/((inNSample+3)/4);
    return true;
}

bool FD::BEDread::readAll(size_t inNSample, BedData& out)
{
    if(!isValid || fileTest != 1 || !file.is_open()) return false;
    size_t nMarker = 0, nSample = inNSample, dataSize;
    if(!statNum(nSample, nMarker)) {
        return false;
    }
    dataSize=((nSample + 3) / 4) * nMarker;
    if(out.data) {
        delete [] out.data;
    }
    out.isValid = true;

    out.data = new (std::nothrow) char[dataSize];
    if (out.data == nullptr) {
        return false;
    }
    out.resetReadPoint();
    file.clear();
    file.seekg(3, std::ios_base::beg);

    // read bed data
    char* pData = out.data;
    size_t remainSize = dataSize;
    while (remainSize > ReadScaleSize) {
        file.read(pData, ReadScaleSize);
        pData += ReadScaleSize;
        remainSize -= ReadScaleSize;
        if (file.eof()) {
            delete [] out.data;
            out.data = nullptr;
            return false;
        }
    }
    file.read(pData, remainSize);

    out.dataSize = dataSize;
    out.nSample  = nSample;

    out.nMarker = nMarker;
    file.seekg(3, std::ios_base::beg);
    return true;
}


FD::TPolyPedread::TPolyPedread():BaseFileRead(){}

FD::TPolyPedread::~TPolyPedread(){}

FD::TPolyPedread::TPolyPedread(const char *inFile)
    :TPolyPedread()
{
    load(inFile);
}

bool FD::TPolyPedread::readAll(PolyPedData &pData)
{
    if (!isValid) {
        return false;
    }
    std::vector<std::string> lineList;
    std::string line;
    file.clear();
    file.seekg(0, std::ios::beg);
    size_t nMarker = 0, iMarker = 0;
    size_t nSample = 0, iSample = 0;
    size_t nPloid = 0, iPloid = 0;
    short* data = nullptr;
    short  tGeno;
    bool   isMissing = false;
    pData.clear();
    while (std::getline(file, line)) {
        strip(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        ++nMarker;
        split(lineList, line, "\t ");
        if (nSample == 0) {
            nSample = lineList.size();
        }
        if (nSample != lineList.size()) {
            return false;
        }
        for (iSample = 0; iSample < nSample; ++iSample) {
            if (lineList[iSample].size() == 2) {
                if (!(lineList[iSample] == "NA" ||
                    lineList[iSample] == "nA" ||
                    lineList[iSample] == "na" ||
                    lineList[iSample] == "Na")) {
                    if (nPloid != 0 && lineList[iSample].size() != nPloid) {
                        return false;
                    }
                    if (nPloid == 0) {
                        nPloid = 2;
                    }
                }
            } else {
                if(nPloid != 0 && lineList[iSample].size() != nPloid){
                    return false;
                }
                if(nPloid == 0) {
                    nPloid = lineList[iSample].size();
                }
            }
        }
    }
    file.clear();
    file.seekg(0, std::ios::beg);
    pData.data = new short[nSample * nMarker];
    data = pData.data;
    while (std::getline(file, line)){
        strip(line);
        if (line.empty() || line[0]=='#') {
            continue;
        }
        split(lineList, line,"\t ");
        for (iSample = 0; iSample < nSample; ++iSample){
            if (lineList[iSample] == "NA" ||
               lineList[iSample] == "nA" ||
               lineList[iSample] == "na" ||
               lineList[iSample] == "Na"){
                data[iMarker * nSample + iSample] = MML::UNASSIGNED;
            } else {
                isMissing = false;
                tGeno = 0;
                for (iPloid = 0; iPloid < nPloid; ++iPloid){
                    switch (lineList[iSample][iPloid]) {
                        case '0':
                            break;
                        case '1':
                            ++tGeno;
                            break;
                        default:
                            isMissing = true;
                            break;
                    }
                    if (isMissing) {
                        break;
                    }
                }
                if (isMissing) {
                    data[iMarker * nSample + iSample] = MML::UNASSIGNED;
                } else {
                    data[iMarker * nSample + iSample] = tGeno;
                }
            }
        }
        ++iMarker;
    }
    pData.nSample = nSample;
    pData.nMarker = nMarker;
    pData.nPloid  = nPloid;
    pData.isValid = true;
    pData.resetReadPoint();
    return true;
}

FD::BIMLogPread::BIMLogPread() : BaseFileRead(){}

FD::BIMLogPread::~BIMLogPread(){}

FD::BIMLogPread::BIMLogPread(const char *inFile):BIMLogPread()
{
    load(inFile);
}

bool FD::BIMLogPread::readAll(BIMLogPData& out)
{
    if(!isValid) return false;
    char line[MAXLINELEN];
    char* lineList[6];
    size_t i=0,imarkers=0;
    for(i=0;i<6;++i) lineList[i]=new char[MAXELEMLEN];
    std::set<std::string> chrSet;
    std::map<std::string,int> chrMap;
    if(out.chrName){
        for(int i=0;i<out.nChr;++i) delete [] out.chrName[i];
        delete [] out.chrName;
    }
    out.chrName=nullptr;
    if(out.data) delete [] out.data;
    out.data=nullptr;
    out.isValid=false;
    size_t nMarker=0,nChr=0;
    bool errorFlag=false;
    int nLineList=0;
    while(!(file.eof()))
    {
        file.getline(line,MAXLINELEN,'\n');
        pstrip(line);
        if(line[0]=='\0') continue;
        nLineList=split(line,lineList,"\t ",6);
        if(nLineList<6){
            errorFlag=true;
            break;
        }
        chrSet.insert(lineList[0]);
        ++nMarker;
    }
    if(errorFlag){
        for(i=0;i<6;++i) delete [] lineList[i];
        return false;
    }
    nChr=chrSet.size();
    out.chrName=new char*[nChr];
    auto iter=chrSet.begin();
    for(i=0;i<nChr;++i,++iter){
        (out.chrName)[i]=new char[MAXELEMLEN];
        charCopy((out.chrName)[i],iter->c_str());
        chrMap[(out.chrName)[i]]=i;
    }
    out.data=new BIMLogPNode[nMarker];
    file.clear();
    file.seekg(0,std::ios::beg);
    while(!(file.eof()))
    {
        file.getline(line,MAXLINELEN,'\n');
        pstrip(line);
        if(line[0]=='\0') continue;
        split(line,lineList,"\t ",6);
        (out.data)[imarkers]=BIMLogPNode();
        (out.data)[imarkers].name=lineList[1];
        (out.data)[imarkers].pos=atoll(lineList[3]);
        (out.data)[imarkers].idChr=chrMap[lineList[0]];
        ++imarkers;
    }
    out.nChr=nChr;
    out.nMarker=nMarker;
    out.isValid=true;
    for(i=0;i<6;++i) delete [] lineList[i];
    return true;
}

FD::BIMread::BIMread() : BaseFileRead(){}

FD::BIMread::BIMread(const char *inFile):BIMread()
{
    load(inFile);
}

FD::BIMread::~BIMread(){}

bool FD::BIMread::readAll(BIMData& out)
{
    if(!isValid) return false;
    char line[MAXLINELEN];
    char* lineList[6];
    size_t i=0,imarkers=0;
    if(out.chrName){
        for(int j=0;j<out.nChr;++j){
            delete [] out.chrName[j];
        }
        delete [] out.chrName;
    }
    if(out.data) delete [] out.data;
    out.data=nullptr;
    out.chrName=nullptr;
    out.isValid = false;
    for(i=0;i<6;++i) lineList[i]=new char[MAXELEMLEN];
    std::set<std::string> chrSet;
    std::map<std::string,int> chrMap;
    size_t nMarker=0;
    int nChr=0;
    bool errorFlag=false;
    int nLineList=0;
    while(!(file.eof()))
    {
        file.getline(line,MAXLINELEN,'\n');
        pstrip(line);
        if(line[0]=='\0') continue;
        nLineList=split(line,lineList,"\t ",6);
        if(nLineList<6){
            errorFlag=true;
            break;
        }
        chrSet.insert(lineList[0]);
        ++nMarker;
    }
    if(errorFlag){
        for(i=0;i<6;++i) delete [] lineList[i];
        return false;
    }
    nChr=chrSet.size();
    out.nChr=nChr;
    out.chrName=new char*[nChr];
    auto iter=chrSet.begin();
    for(i=0;i<nChr;++i,++iter){
        (out.chrName)[i]=new char[MAXELEMLEN];
        charCopy((out.chrName)[i],iter->c_str());
        chrMap[(out.chrName)[i]]=i;
    }
    out.data = new BIMNode[nMarker];
    file.clear();
    file.seekg(0,std::ios::beg);
    while(!(file.eof()))
    {
        file.getline(line,MAXLINELEN,'\n');
        pstrip(line);
        if(line[0]=='\0') continue;
        split(line,lineList,"\t ",6);
        (out.data)[imarkers].idChr=chrMap[lineList[0]];
        (out.data)[imarkers].pos=atoll(lineList[3]);
        (out.data)[imarkers].name=lineList[1];
        ++imarkers;
    }
    out.nMarker = nMarker;
    out.isValid = true;
    for(i=0;i<6;++i) delete [] lineList[i];
    return true;
}

FD::FAMread::FAMread() : BaseFileRead()
{
    sampleN  = 0;
    data     = nullptr;
    popIndex = nullptr;
    popName  = nullptr;
    isValid  = false;
}

FD::FAMread::~FAMread()
{
    clear();
}

FD::FAMread::FAMread(const char *inFile)
    :FAMread()
{
    load(inFile);
}

bool FD::FAMread::clear()
{
    if(!close()) return false;
    if(data){
        for(long long i=0;i<6*sampleN;++i) delete [] data[i];
        delete [] data;
    }
    if(popIndex) delete [] popIndex;
    if(popName){
        for(long long i=0;i<nPop;++i) delete [] popName[i];
        delete [] popName;
    }
    isValid=false;
    data = nullptr;
    popName = nullptr;
    popIndex = nullptr;
    sampleN = 0;
    nPop = 0;
    return true;
}

bool FD::FAMread::readAll()
{
    if(!isValid) return false;
    long long i=0,isamples=0;
    if(data    ){
        for(i=0;i<6*sampleN;++i) delete [] data[i];
        delete [] data;
    }
    if(popIndex) delete [] popIndex;
    if(popName ){
        for(i=0;i<nPop;++i) delete [] popName[i];
        delete [] popName;
    }
    char line[MAXLINELEN];
    char* lineList[6];
    std::set<std::string> popSet;
    std::map<std::string,int> popMap;
    for(i=0;i<6;++i) lineList[i]=new char[MAXELEMLEN];
    sampleN=0;
    while(!(file.eof()))
    {
        file.getline(line,MAXLINELEN,'\n');
        pstrip(line);
        if(line[0]=='\0') continue;
        split(line,lineList,"\t ",6);
        popSet.insert(lineList[0]);
        ++sampleN;
    }
    nPop=popSet.size();
    popName=new char*[nPop];
    auto iter=popSet.begin();
    for(i=0;i<nPop;++i,++iter){
        popName[i]=new char[MAXELEMLEN];
        charCopy(popName[i],iter->c_str());
        popMap[popName[i]]=i;
    }
    data=new char*[sampleN*6];
    for(i=0;i<sampleN*6;++i) data[i]=new char[MAXELEMLEN];
    popIndex=new long long[sampleN];
    file.clear();
    file.seekg(0,std::ios::beg);
    while(!(file.eof()))
    {
        file.getline(line,MAXLINELEN,'\n');
        pstrip(line);
        if(line[0]=='\0') continue;
        split(line,lineList,"\t ",6);
        for(i=0;i<6;++i){
            charCopy(data[isamples*6+i],lineList[i]);
        }

        popIndex[isamples]=popMap[lineList[0]];
        ++isamples;
    }
    for(i=0;i<6;++i) delete [] lineList[i];
    return true;
}

char *FD::FAMread::getSampleName(long long index)
{
    if(index>=sampleN) return nullptr;
    return data[6*index+1];
}

FD::VCFread::VCFread() : BaseFileRead(){}

FD::VCFread::~VCFread()
{
    if(file.is_open()) file.close();
}

FD::VCFread::VCFread(const char *inFile) : VCFread()
{
    load(inFile);
}

bool FD::VCFread::readAll(PolyPedData& pData,BIMLogPData& bimData)
{
    if(!isValid) return false;
    std::vector<std::string> lineList;
    std::vector<std::string> formatList;
    std::vector<std::string> indList;
    std::vector<std::string> genoList;
    std::set<std::string> chrSet;
    std::map<std::string,int> chrMap;
    std::string line,GTStr;
    file.clear();
    file.seekg(0,std::ios::beg);
    size_t iFormat=0,idGT=0;
    size_t nMarker=0,iMarker=0;
    size_t nSample=0,iSample=0;
    size_t nPloid=0,iPloid=0;
    size_t nChr=0,iChr=0;
    short* data=nullptr;
    short  tGeno,tGenoOne;
    pData.clear();
    bimData.clear();
    while(std::getline(file,line)){
        strip(line);
        if(line.empty()||line[0]=='#') continue;
        split(lineList,line,'\t');
        if(lineList.size() <= 9){
            return false;
        }
        if(!nSample){
            nSample = lineList.size()-9;
        }
        if(nSample != lineList.size()-9){
            return false;
        }
        chrSet.insert(lineList[0]);
        ++nMarker;
    }
    file.clear();
    file.seekg(0,std::ios::beg);
    pData.data = new short[nSample*nMarker];
    data = pData.data;
    nChr=chrSet.size();
    bimData.chrName=new char*[nChr];
    auto iter=chrSet.begin();
    for(iChr=0;iChr<nChr;++iChr,++iter){
        (bimData.chrName)[iChr]=new char[MAXELEMLEN];
        charCopy((bimData.chrName)[iChr],iter->c_str());
        chrMap[(bimData.chrName)[iChr]]=iChr;
    }
    bimData.data=new BIMLogPNode[nMarker];
    while(std::getline(file,line)){
        strip(line);
        if(line.empty()||line[0]=='#') continue;
        split(lineList,line,'\t');
        split(formatList,lineList[8],':');
        (bimData.data)[iMarker]=BIMLogPNode();
        (bimData.data)[iMarker].name=lineList[0]+'_'+lineList[1];
        (bimData.data)[iMarker].pos=std::stoll(lineList[1]);
        (bimData.data)[iMarker].idChr=chrMap[lineList[0]];
        for(iFormat=0;iFormat<formatList.size();++iFormat){
            if(formatList[iFormat] == "GT"){
                idGT = iFormat;
            }
        }
        for(iSample=0;iSample<nSample;++iSample){
            split(indList,lineList[iSample+9],':');
            GTStr = indList[idGT];
            tGeno = 0;
            if(GTStr.find('.') != std::string::npos){
                data[iMarker*nSample+iSample]=MML::UNASSIGNED;
                continue;
            }
            if(GTStr.find('|') != std::string::npos){
                split(genoList,GTStr,'|');
            }
            else{
                split(genoList,GTStr,'/');
            }
            if(!nPloid) nPloid = genoList.size();
            if(nPloid != genoList.size()) return false;
            for(iPloid = 0;iPloid < nPloid;++iPloid){
                tGenoOne = std::stoi(genoList[iPloid]);
                if(tGenoOne) tGeno += 1;
            }
            data[iMarker*nSample+iSample] = tGeno;
        }
        ++iMarker;
    }
    if(!nPloid) return false;
    pData.nSample = nSample;
    pData.nMarker = nMarker;
    pData.nPloid  = nPloid;
    pData.isValid = true;
    pData.resetReadPoint();
    bimData.nChr    = nChr;
    bimData.nMarker = nMarker;
    bimData.isValid = true;
    return true;
}

FD::genomeRead::genomeRead()
{
    setGffLabelDefault();
    genome=nullptr;
    isValid=false;

    geneTypeLabel = "mRNA";
    geneIdKey     = "ID";
    geneAnnoKey   = "Anno";
    CDSTypeLabel  = "CDS";
    UTR5TypeLabel = "five_prime_UTR";
    UTR3TypeLabel = "three_prime_UTR";
}

FD::genomeRead::genomeRead(const char *inFile,char** inChrName,int inNchr)
    :genomeRead()
{
    if(open(inFile,inChrName,inNchr)) isValid=true;
}

FD::genomeRead::~genomeRead()
{
    if(genome) delete genome;
    if(file.is_open()) file.close();
}

bool FD::genomeRead::open(const char *inFile,char** inChrName,int inNchr)
{
    if(!inChrName||!inNchr) return false;
    chrName.clear();
    for(int i = 0 ;i < inNchr ; ++i){
        chrName.push_back(inChrName[i]);
    }
    nChr=inNchr;
    if(file.is_open()) file.close();
    file.open(inFile,std::ios::in);
    if(!file.is_open()) return false;
    idChrMap.clear();
    for(int i=0;i<nChr;++i){
        idChrMap[chrName[i]]=i;
    }
    isValid=true;
    return true;
}

bool FD::genomeRead::open(const char *inFile, const std::vector<std::string> &inChrName)
{
    if(inChrName.empty()) return false;
    chrName.clear();
    chrName=inChrName;
    nChr=chrName.size();
    if(file.is_open()) file.close();
    file.open(inFile,std::ios::in);
    if(!file.is_open()) return false;
    idChrMap.clear();
    for(int i=0;i<nChr;++i){
        idChrMap[chrName[i]]=i;
    }
    isValid=true;
    return true;
}

bool FD::genomeRead::readTsv()
{
    if(!isValid) return false;
    if(genome) delete genome;
    bool errorFlag=false;
    size_t i,iGenes=0,nGenes=0;
    long long nList;
    char line[MAXLINELEN];
    char** lineList=new char*[6];
    for(i=0;i<6;++i) lineList[i]=new char[MAXELEMLEN];
    while(!(file.eof()))
    {
        file.getline(line,MAXLINELEN,'\n');
        strip(line);
        if(line[0]=='\0'||line[0]=='#') continue;
        nList = split(line,lineList,'\t',6);
        if(nList<5){
            errorFlag = true;
            break;
        }
        if(idChrMap.find(lineList[1])==idChrMap.end()) continue;
        ++nGenes;
    }
    if(!errorFlag){
        genome=new GenomeAnnotation(chrName,nGenes);
        file.clear();
        file.seekg(0,std::ios::beg);
        while(!(file.eof()))
        {
            file.getline(line,MAXLINELEN,'\n');
            strip(line);
            if(line[0]=='\0'||line[0]=='#') continue;
            nList = split(line,lineList,'\t',6);
            if(nList<5){
                errorFlag = true;
                break;
            }
            if(idChrMap.find(lineList[1])==idChrMap.end()) continue;
            if(nList==6){
                *(genome->getGene(iGenes))=GeneAnnotation(
                            lineList[0],
                            idChrMap[lineList[1]],
                            std::stoull(lineList[2]),
                            std::stoull(lineList[3]),
                            lineList[4][0],
                            lineList[5]
                            );
            }
            else if(nList==5){
                *(genome->getGene(iGenes))=GeneAnnotation(
                            lineList[0],
                            idChrMap[lineList[1]],
                            std::stoull(lineList[2]),
                            std::stoull(lineList[3]),
                            lineList[4][0],
                            "NA"
                            );
            }
            ++iGenes;
        }
    }
    for(i=0;i<6;++i) delete [] lineList[i];
    delete [] lineList;
    if(errorFlag) return false;
    else{
        genome->sortGenes();
        if(!genome->getValid()) return false;
    }
    return true;
}

bool FD::genomeRead::readGff()
{
    if(!isValid) return false;
    if(genome) delete genome;
    bool errorFlag=false,readBegin=false;
    long long i,iGenes=0,nGenes=0;
    long long tStart5UTR,tStop5UTR,tStart3UTR,tStop3UTR;
    long long nList;
    long long tStart,tStop;
    char line[MAXLINELEN];
    std::list<long long> tCDSList;
    std::vector<std::string> pairList,attriList;
    std::string tGeneId,tGeneAnno,tChr;
    char tStrand;
    char** lineList=new char*[9];
    for(i=0;i<9;++i) lineList[i]=new char[MAXELEMLEN];
    while(!(file.eof()))
    {
        file.getline(line,MAXLINELEN,'\n');
        strip(line);
        if(line[0]=='\0'||line[0]=='#') continue;
        split(line,lineList,'\t',9);
        if(idChrMap.find(lineList[0])!=idChrMap.end()&&
           geneTypeLabel==lineList[2]) ++nGenes;
    }
    genome=new GenomeAnnotation(chrName,nGenes);
    file.clear();
    file.seekg(0,std::ios::beg);
    while(!(file.eof()))
    {
        file.getline(line,MAXLINELEN,'\n');
        strip(line);
        if(line[0]=='\0'||line[0]=='#') continue;
        nList = split(line,lineList,'\t',9);
        if(nList<9){
            errorFlag = true;
            break;
        }
        if(idChrMap.find(lineList[0])==idChrMap.end()) continue;
        if(geneTypeLabel==lineList[2]){
            if(readBegin){
                *(genome->getGene(iGenes))=GeneAnnotation(
                            tGeneId.c_str(),
                            idChrMap[tChr],
                            tStart,
                            tStop,
                            tStrand,
                            tGeneAnno.c_str(),
                            tCDSList,
                            tStart5UTR,
                            tStop5UTR,
                            tStart3UTR,
                            tStop3UTR);
                ++iGenes;
            }

            tGeneId="NA",tGeneAnno="NA";
            tCDSList.clear();
            tStart5UTR=0,tStop5UTR=0,tStart3UTR=0,tStop3UTR=0;
            tStart=std::stoull(lineList[3]),tStop=std::stoull(lineList[4]);
            tChr=lineList[0],tStrand=lineList[6][0];
            split(attriList,std::string(lineList[8]),';');
            for(auto& pair:attriList){
                if(!pair.empty()){
                    split(pairList,pair,'=');
                    if(pairList[0]==geneIdKey) tGeneId=pairList[1];
                    else if(pairList[0]==geneAnnoKey) tGeneAnno=pairList[1];
                }
            }
            readBegin=true;
        }
        else if(CDSTypeLabel==lineList[2]){
            tCDSList.push_back(std::stoull(lineList[3]));
            tCDSList.push_back(std::stoull(lineList[4]));
        }
        else if(UTR5TypeLabel==lineList[2]){
            tStart5UTR=std::stoull(lineList[3]);
            tStop5UTR=std::stoull(lineList[4]);
        }
        else if(UTR3TypeLabel==lineList[2]){
            tStart3UTR=std::stoull(lineList[3]);
            tStop3UTR=std::stoull(lineList[4]);
        }
    }

    // the last one
    if(iGenes>0&&(!errorFlag)){
        *(genome->getGene(iGenes))=GeneAnnotation(
                    tGeneId.c_str(),
                    idChrMap[tChr],
                    tStart,
                    tStop,
                    tStrand,
                    tGeneAnno.c_str(),
                    tCDSList,
                    tStart5UTR,
                    tStop5UTR,
                    tStart3UTR,
                    tStop3UTR);

        ++iGenes;
    }
    genome->setGeneModel(true);

    for(i=0;i<9;++i) delete [] lineList[i];
    delete [] lineList;
    if(errorFlag) return false;
    else{
        genome->sortGenes();
        if(!genome->getValid()) return false;
    }
    return true;
}

GenomeAnnotation *FD::genomeRead::getGenome()
{
    return genome;
}

void FD::genomeRead::setGffLabel(const char* inGeneTypeLabel,
                                 const char* inGeneIdKey,
                                 const char* inGeneAnnoKey,
                                 const char* inCDSTypeLabel ,
                                 const char* inUTR5TypeLabel,
                                 const char* inUTR3TypeLabel)
{
    if(inGeneTypeLabel) geneTypeLabel = inGeneTypeLabel;
    if(inGeneIdKey    ) geneIdKey     = inGeneIdKey;
    if(inGeneAnnoKey  ) geneAnnoKey   = inGeneAnnoKey;
    if(inCDSTypeLabel ) CDSTypeLabel  = inCDSTypeLabel ;
    if(inUTR5TypeLabel) UTR5TypeLabel = inUTR5TypeLabel;
    if(inUTR3TypeLabel) UTR3TypeLabel = inUTR3TypeLabel;
}

void FD::genomeRead::setGffLabelDefault()
{
    geneIdKey="ID";
    geneTypeLabel="mRNA";
    geneAnnoKey="anno";
    CDSTypeLabel="CDS";
    UTR3TypeLabel="three_prime_UTR";
    UTR5TypeLabel="five_prime_UTR";
}


bool FD::pheRead(const char *inFile, MML::Mat &out)
{
    std::ifstream iF(inFile,std::ios::in);
    char line[MAXLINELEN];
    size_t nSam=0,iSam=0;
    while(!(iF.eof()))
    {
        iF.getline(line,MAXLINELEN,'\n');
        if(line[0]=='\0') continue;
        ++nSam;
    }
    out.resize(nSam,1);
    out.toColVec();
    iF.clear();
    iF.seekg(0,std::ios::beg);

    while(!(iF.eof()))
    {
        iF.getline(line,MAXLINELEN,'\n');
//        qDebug() << line;
        if(line[0]=='\0') continue;

        if(compare(line,"NA\0")||compare(line,"na\0")) out(iSam)=MML::DATA_NA;
        else out(iSam)=atof(line);
        ++iSam;
    }
    iF.close();
    return true;
}

bool FD::kinRead(const char *inFile, MML::Mat &out)
{
    std::ifstream iF(inFile,std::ios::in);
    char line[MAXLINELEN];
    char** lineList;
    size_t nSam=0,iSam=0,i=0;
    while(!(iF.eof()))
    {
        iF.getline(line,MAXLINELEN,'\n');
        if(line[0]=='\0') continue;
        ++nSam;
    }
    out.resize(nSam,nSam);
    lineList=new char*[nSam];
    for(i=0;i<nSam;++i) lineList[i]=new char[MAXELEMLEN];
    iF.clear();
    iF.seekg(0,std::ios::beg);
    while(!(iF.eof()))
    {
        iF.getline(line,MAXLINELEN,'\n');
        if(line[0]=='\0') continue;
        split(line,lineList,'\t',nSam);
        for(i=0;i<nSam;++i) out(iSam,i)=atof(lineList[i]);
        ++iSam;
    }
    for(i=0;i<nSam;++i) delete [] lineList[i];
    delete [] lineList;
    iF.close();
    return true;
}

bool FD::covRead(const char *inFile, MML::Mat &out)
{
    std::ifstream iF(inFile,std::ios::in);
    char line[MAXLINELEN];
    char** lineList;
    size_t nSam=0,iSam=0,nDim=0,i=0;
    while(!(iF.eof()))
    {
        iF.getline(line,MAXLINELEN,'\n');
        if(line[0]=='\0') continue;
        if(nDim==0)
        {
            for(i=0;line[i]!='\0';++i)
            {
                if(line[i]=='\t') ++nDim;
            }
            ++nDim;
        }
        ++nSam;
    }
    out.resize(nSam,nDim);
    lineList=new char*[nDim];
    for(i=0;i<nDim;++i) lineList[i]=new char[MAXELEMLEN];
    iF.clear();
    iF.seekg(0,std::ios::beg);
    while(!(iF.eof()))
    {
        iF.getline(line,MAXLINELEN,'\n');
        if(line[0]=='\0') continue;
        split(line,lineList,'\t',nDim);
        for(i=0;i<nDim;++i) out(iSam,i)=atof(lineList[i]);
        ++iSam;
    }
    for(i=0;i<nDim;++i) delete [] lineList[i];
    delete [] lineList;
    iF.close();
    return true;
}

FD::ChrLenReadTsvState FD::chrLenReadTsv(const char *inFile,
                                         std::vector<std::string> &outChr,
                                         std::vector<long long>& outLen)
{
    std::fstream file(inFile,std::ios::in);
    outChr.clear();
    outLen.clear();
    std::set<std::string> tSet;
    ChrLenReadTsvState errorFlag=CHRLENREADTSV_OK;
    if(!file.is_open()){
        return CHRLENREADTSV_FILEPATHNULL;
    }
    size_t i,nList;
    char line[MAXLINELEN];
    char** lineList=new char*[2];
    for(i=0;i<2;++i) lineList[i]=new char[MAXELEMLEN];
    i = 0;
    while(!(file.eof()))
    {
        file.getline(line,MAXLINELEN,'\n');
        strip(line);
        if(line[0]=='\0') continue;
        nList = split(line,lineList,'\t',2);
        if(nList<2){
            errorFlag=CHRLENREADTSV_ERRORCOLNUM;
            break;
        }
        if(tSet.find(lineList[0]) != tSet.end()){
            errorFlag=CHRLENREADTSV_REPEATCHR;
            break;
        }
        tSet.insert(lineList[0]);
        outChr.push_back(lineList[0]);
        outLen.push_back(std::stoll(lineList[1]));
    }
    file.close();
    for(i=0;i<2;++i) delete [] lineList[i];
    delete [] lineList;
    return errorFlag;
}

bool FD::genomeSeqRead(const char* inFile,
                       char** inChrName,
                       long long inNChr,
                       const long long* inChrLen,
                       GenomeSequence& genomeSeq)
{
    if(!inFile||!inChrName||!inChrLen) return false;
    long long i,iPosition=1,iChr=0;
    bool flag=false;
    std::fstream file(inFile,std::ios::in);
    char line[MAXLINELEN],tChr[MAXELEMLEN];
    std::map<std::string,long long> chrMap,chrLen;
    for(i=0;i<inNChr;++i){
        chrMap[inChrName[i]]=i;
    }
    while(!(file.eof())){
        file.getline(line,MAXLINELEN,'\n');
        strip(line);
        if(line[0]=='\0'||line[0]=='#') continue;
        if(line[0]=='>'){
            i=1;
            while(i<MAXLINELEN-1&&
                  line[i]!='\0'&&
                  line[i]!=' '&&
                  line[i]!='\t'&&
                  line[i]!='\n'&&
                  line[i]!='\r'){
                tChr[i-1]=line[i];
                ++i;
            }
            tChr[i-1]='\0';
            if(chrMap.find(tChr)==chrMap.end()) flag=false;
            else{
                flag=true;
                chrLen[tChr]=0;
            }
        }
        else if(flag){
            chrLen[tChr]+=charLen(line);
        }
    }
    if(chrLen.size()!=inNChr){
        file.close();
        return false;
    }
    for(i=0;i<inNChr;++i){
        if(inChrLen[i]!=chrLen[inChrName[i]]){
            file.close();
            return false;
        }
    }
    genomeSeq.setInitialization(inChrName,inNChr,inChrLen);
    file.clear();
    file.seekg(0,std::ios::beg);
    while(!(file.eof())){
        file.getline(line,MAXLINELEN,'\n');
        strip(line);
        if(line[0]=='\0'||line[0]=='#') continue;
        if(line[0]=='>'){
            i=1;
            while(i<MAXLINELEN-1&&
                  line[i]!='\0'&&
                  line[i]!=' '&&
                  line[i]!='\t'&&
                  line[i]!='\n'&&
                  line[i]!='\r'){
                tChr[i-1]=line[i];
                ++i;
            }
            tChr[i-1]='\0';
            if(chrMap.find(tChr)==chrMap.end()) flag=false;
            else{
                flag=true;
                iChr=chrMap[tChr];
            }
            iPosition=1;
        }
        else if(flag){
            genomeSeq.insertSeq(iChr,iPosition,line);
            iPosition+=charLen(line);
        }
    }
    file.close();
    return true;
}

bool FD::PCAOutread(const char *inFile, MML::Mat &out, int nPC)
{
    std::ifstream iF(inFile,std::ios::in);
    char line[MAXLINELEN];
    char** lineList;
    size_t nSam=0,iSam=0,i=0;
    int nLineList;
    bool errorFlag=false;
    while(!(iF.eof()))
    {
        iF.getline(line,MAXLINELEN,'\n');
        if(line[0]=='\0') continue;
        ++nSam;
    }
    out.resize(nSam,nPC);
    lineList=new char*[nPC];
    for(i=0;i<nPC;++i) lineList[i]=new char[MAXELEMLEN];
    iF.clear();
    iF.seekg(0,std::ios::beg);
    while(!(iF.eof()))
    {
        iF.getline(line,MAXLINELEN,'\n');
        if(line[0]=='\0') continue;
        nLineList=split(line,lineList,'\t',nPC);
        if(nLineList<nPC){
            errorFlag=true;
            break;
        }
        for(i=0;i<nPC;++i) out(iSam,i)=atof(lineList[i]);
        ++iSam;
    }
    for(i=0;i<nPC;++i) delete [] lineList[i];
    delete [] lineList;
    iF.close();
    if(errorFlag) return false;
    return true;
}

FD::GWASOutread::GWASOutread()
{
    data=nullptr;
    logPCol=3;
    isValid=false;
    nMarkers=0;
}

FD::GWASOutread::GWASOutread(const char *inFile)
    :GWASOutread()
{
    file.open(inFile,std::ios::in);
    if(file.is_open()) isValid=true;
}

FD::GWASOutread::~GWASOutread()
{
    if(file.is_open()) file.close();
    if(data) delete [] data;
}

bool FD::GWASOutread::loadFile(const char *inFile)
{
    if(file.is_open()) file.close();
    file.open(inFile);
    if(file.is_open()) isValid=true;
    else isValid=false;
    return isValid;
}

bool FD::GWASOutread::setLogCol(int iCol)
{
    if(iCol<0) return false;
    logPCol=iCol;
    return true;
}

bool FD::GWASOutread::readAll()
{
    char line[MAXLINELEN];
    char** lineList = nullptr;
    size_t i,iMarkers=0;
    int nCol=0;
    bool errorFlag=false;
    if(!file.is_open()||!isValid) return false;
    if(data) delete [] data;
    nMarkers=0;
    lineList = new char*[logPCol+1];
    for(i=0;i<logPCol+1;++i) lineList[i]=new char[MAXELEMLEN];
    while(!(file.eof())){
        file.getline(line,MAXLINELEN,'\n');
        strip(line);
        if(line[0]=='\0'||line[0]=='#') continue;
        ++nMarkers;
    }
    file.clear();
    file.seekg(0,std::ios::beg);
    data = new double[nMarkers];
    while(!(file.eof())){
        file.getline(line,MAXLINELEN,'\n');
        strip(line);
        if(line[0]=='\0'||line[0]=='#') continue;
        nCol = split(line,lineList," \t",logPCol+1);
        if(nCol<logPCol+1){
            errorFlag=true;
            break;
        }
        if(compare(lineList[logPCol],"nan")||
           compare(lineList[logPCol],"NA")||
           compare(lineList[logPCol],"na")){
            data[iMarkers]=1;
        }
        else data[iMarkers]=atof(lineList[logPCol]);
        if(data[iMarkers]==0.0) data[iMarkers]=1;
        ++iMarkers;
    }
    for(i=0;i<logPCol+1;++i) delete [] lineList[i];
    delete [] lineList;
    if(errorFlag) return false;
    return true;
}

double FD::GWASOutread::getValueAt(size_t id)
{
    if(isValid&&id<nMarkers) return data[id];
    return -1.0;
}

const double *FD::GWASOutread::getValues()
{
    if(isValid) return data;
    return nullptr;
}

FD::GWASProject::GWASProject()
{
    isValid=false;
    isKinValid=false;
    isPolyploid=false;
    proInfo=PROJECT_GWAS;
}

size_t FD::GWASProject::getNSample() const
{
    if(isPolyploid) return pData.nSample;
    return bed.nSample;
}

size_t FD::GWASProject::getNMarker() const
{
    return bim.nMarker;
}

size_t FD::GWASProject::getNFactor() const
{
    return cov.getNCol();
}

size_t FD::GWASProject::getNPolyploid() const
{
    if (isPolyploid) {
        return pData.nPloid;
    } else {
        return 2;
    }
    return 2;
}

FD::ReadState FD::GWASProject::read(const char* nameBed, const char* nameBim, const char* namePhe, const char* nameCov, const char* nameKin)
{
    BEDread bedR;
    BIMLogPread bimR;
    size_t BedNum=0,BimNum=0;
    isValid=false;
    isKinValid=false;
    isPolyploid=false;
    if(namePhe[0]=='\0') return state=FD_READINGERROR;
    else
    {
        if(!pheRead(namePhe,phe)) return state=FD_READINGERROR;
        if(phe.getNRow()<=0) return state=FD_N_SAMPLEERROR;
    }
    if(nameCov[0]=='\0')
    {
        cov.setData(phe.getNRow(),1,1,MML::_colvec);
    }
    else
    {
        if(!covRead(nameCov,cov)) return state=FD_READINGERROR;
        if(cov.getNRow()!=phe.getNRow()) return state=FD_N_SAMPLEERROR;
        if(cov.getNCol()<=0) return state=FD_N_FACTORERROR;
    }
    if(nameKin[0]=='\0') isKinValid=false;
    else
    {
        if(!kinRead(nameKin,kin))
        {
            isKinValid=false;
            return state=FD_READINGERROR;
        }
        if(kin.getNCol()!=kin.getNRow()&&kin.getNRow()!=phe.getNRow())
        {
            isKinValid=false;
            return state=FD_N_SAMPLEERROR;
        }
        isKinValid=true;
    }
    if(nameBim[0]=='\0') return state=FD_READINGERROR;
    else
    {
        if(!bimR.load(nameBim))
        {
            return state=FD_READINGERROR;
        }
        if(!bimR.readAll(bim))
        {
            bim.clear();
            return state=FD_READINGERROR;
        }
        BimNum=bim.nMarker;
    }
    if(nameBed[0]=='\0') return state=FD_READINGERROR;
    else
    {
        if(!bedR.load(nameBed))
        {
            bim.clear();
            return state=FD_READINGERROR;
        }
        if(!bedR.statNum(phe.getNRow(),BedNum))
        {
            bim.clear();
            return state=FD_READINGERROR;
        }
        if(!bedR.readAll(phe.getNRow(),bed)){
            bim.clear();
            return state=FD_READINGERROR;
        }
        if(BedNum!=BimNum){
            bed.clear();
            bim.clear();
            return state=FD_N_MARKERERROR;
        }
    }
    isValid=true;
    return state=FD_ENABLED;
}

FD::ReadState FD::GWASProject::readVCF(const char *nameVCF, const char *namePhe, const char *nameCov, const char *nameKin)
{
    BIMLogPread bimR;
    isValid=false;
    isKinValid=false;
    VCFread reader;
    if(namePhe[0]=='\0') return state=FD_READINGERROR;
    else
    {
        if(!pheRead(namePhe,phe)) return state=FD_READINGERROR;
        if(phe.getNRow()<=0) return state=FD_N_SAMPLEERROR;
    }
    if(nameCov[0]=='\0')
    {
        cov.setData(phe.getNRow(),1,1,MML::_colvec);
    }
    else
    {
        if(!covRead(nameCov,cov)) return state=FD_READINGERROR;
        if(cov.getNRow()!=phe.getNRow()) return state=FD_N_SAMPLEERROR;
        if(cov.getNCol()<=0) return state=FD_N_FACTORERROR;
    }
    if(nameKin[0]=='\0') isKinValid=false;
    else
    {
        if(!kinRead(nameKin,kin))
        {
            isKinValid=false;
            return state=FD_READINGERROR;
        }
        if(kin.getNCol()!=kin.getNRow()&&kin.getNRow()!=phe.getNRow())
        {
            isKinValid=false;
            return state=FD_N_SAMPLEERROR;
        }
        isKinValid=true;
    }
    if(nameVCF[0]=='\0') return state=FD_READINGERROR;
    else
    {
        if(!reader.load(nameVCF)){
            bim.clear();
            return state=FD_READINGERROR;
        }
        if(!reader.readAll(pData,bim)){
            bim.clear();
            return state=FD_READINGERROR;
        }
        if(phe.getNRow()!=pData.nSample){
            pData.clear();
            bim.clear();
            return state=FD_N_SAMPLEERROR;
        }
    }
    isValid=true;
    isPolyploid=true;
    return state=FD_ENABLED;
}

FD::ReadState FD::GWASProject::readTPolyPed(const char *nameTPed, const char *nameBim, const char *namePhe, const char *nameCov, const char *nameKin)
{

    BIMLogPread bimR;
    size_t BimNum=0;
    isValid=false;
    isKinValid=false;
    TPolyPedread reader;
    if(namePhe[0]=='\0') return state=FD_READINGERROR;
    else
    {
        if(!pheRead(namePhe,phe)) return state=FD_READINGERROR;
        if(phe.getNRow()<=0) return state=FD_N_SAMPLEERROR;
    }
    if(nameCov[0]=='\0')
    {
        cov.setData(phe.getNRow(),1,1,MML::_colvec);
    }
    else
    {
        if(!covRead(nameCov,cov)) return state=FD_READINGERROR;
        if(cov.getNRow()!=phe.getNRow()) return state=FD_N_SAMPLEERROR;
        if(cov.getNCol()<=0) return state=FD_N_FACTORERROR;
    }
    if(nameKin[0]=='\0') isKinValid=false;
    else
    {
        if(!kinRead(nameKin,kin))
        {
            isKinValid=false;
            return state=FD_READINGERROR;
        }
        if(kin.getNCol()!=kin.getNRow()&&kin.getNRow()!=phe.getNRow())
        {
            isKinValid=false;
            return state=FD_N_SAMPLEERROR;
        }
        isKinValid=true;
    }
    if(nameBim[0]=='\0') return state=FD_READINGERROR;
    else
    {
        if(!bimR.load(nameBim))
        {
            return state=FD_READINGERROR;
        }
        if(!bimR.readAll(bim))
        {
            bim.clear();
            return state=FD_READINGERROR;
        }
        BimNum=bim.nMarker;
    }
    if(nameTPed[0]=='\0') return state=FD_READINGERROR;
    else
    {
        if(!reader.load(nameTPed)){
            bim.clear();
            return state=FD_READINGERROR;
        }
        if(!reader.readAll(pData)){
            bim.clear();
            return state=FD_READINGERROR;
        }
        if(pData.nMarker!=BimNum){
            pData.clear();
            bim.clear();
            return state=FD_N_MARKERERROR;
        }
        if(phe.getNRow()!=pData.nSample){
            pData.clear();
            bim.clear();
            return state=FD_N_SAMPLEERROR;
        }
    }
    isValid=true;
    isPolyploid=true;
    return state=FD_ENABLED;
}

bool FD::GWASProject::saveOutput(const char *outFile)
{
    if(!isValid) return false;
    char outLine[MAXLINELEN];
    std::ofstream oF(outFile,std::ios::out);
    size_t i=0,nMarkers=bim.nMarker;
    const BIMLogPNode* pList=bim.data;
    for(i=0;i<nMarkers;++i)
    {
        if(pList[i].statValue==MML::DATA_NA)
        {
            sprintf(outLine,"%s\tNA\tNA\t1\n",pList[i].name.data());
        }
        else
        {
            sprintf(outLine,"%s\t%e\t%e\t%e\n",pList[i].name.data(),pList[i].betax0,pList[i].statValue,pList[i].PValue);
        }
        oF << outLine;
    }
    oF << std::flush;
    oF.close();
    return true;
}

char **FD::GWASProject::getChrName()
{
    if(isValid) return bim.getChrName();
    return nullptr;
}

int FD::GWASProject::getNChr()
{
    if(isValid) return bim.getNChr();
    return -1;
}


bool FD::BIMLogPNodeLessWithLogP::operator ()(const BIMLogPNode *a, const BIMLogPNode *b)
{
    return a->log10PValue<b->log10PValue;
}

void FD::deleteProject(const BaseProject *p)
{
    switch (p->proInfo) {
    case PROJECT_GWAS:
        delete dynamic_cast<const GWASProject*>(p);
        break;
    case PROJECT_BSA:
        delete dynamic_cast<const BSAProject*>(p);
        break;
    }
}




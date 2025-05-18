#include "BSAanalysis/BSAProject.hpp"

namespace FD{

BSAInterVal::BSAInterVal()
{

}

bool BSAInterVal::operator<(const BSAInterVal &B) const
{
    if(idChr!=B.idChr) return idChr<B.idChr;
    return start<B.start;
}

double BSAInterVal::getData(short info)
{
    double out=MML::DATA_NA;
    switch (info) {
    case 0: out=indexH; break;
    case 1: out=indexL; break;
    case 2: out=val; break;
    case 3: out=thre; break;
    case 4: out=-thre; break;
    default: break;
    }
    return out;
}

BSAVCFRead::BSAVCFRead()
{

}

BSAVCFRead::BSAVCFRead(const char *inFile) : BSAVCFRead()
{
    load(inFile);
}

bool BSAVCFRead::load(const char *inFile)
{
    close();
    file.open(inFile,std::ios::in);
    if(file.is_open()) isValid = true;
    return isValid;
}

bool BSAVCFRead::close()
{
    if(file.is_open()) file.close();
    isValid = false;
    return true;
}

bool BSAVCFRead::readAll(MML::Mat &data, std::vector<std::string> &poolName, ValPoints2 *&points, std::vector<std::string> &chrName)
{
    if(!isValid) return false;
    if(points){
        delete [] points;
        points = nullptr;
    }
    data.clear();
    poolName.clear();
    chrName.clear();
    file.clear();
    file.seekg(0,std::ios::beg);
    std::map<std::string,size_t> idChrMap;
    std::vector<std::string> lineList,adList,formatList,infoList,gtList;
    std::string line;
    bool header=false;
    size_t iMarker=0,idChr=0,tAD1,tAD2,i;
    int iAD=-1,iGT=-1,iFormat;
    while(std::getline(file,line)){
        strip(line);
        if(line.empty()) continue;
        split(lineList,line,'\t');
        if(line[0] == '#'){
            if(line.size()>=2&&line[1] != '#'){
                if(header) continue;
                if(lineList.size()<11) return false;
                header = true;
                poolName.push_back(lineList[9]);
                poolName.push_back(lineList[10]);
            }
        }
        else{
            if(lineList.size()<11) return false;
            if(idChrMap.find(lineList[0])==idChrMap.end()){
                idChrMap[lineList[0]]=idChr;
                chrName.push_back(lineList[0]);
                ++idChr;
            }
            ++iMarker;
        }
    }
    points = new ValPoints2[iMarker+1];
    data.resize(iMarker,4);
    file.clear();
    file.seekg(0,std::ios::beg);
    iMarker=0;
    while(std::getline(file,line)){
        strip(line);
        if(line.empty()) continue;
        split(lineList,line,'\t');
        if(line[0] != '#'){
            points[iMarker].idchr=idChrMap[lineList[0]];
            points[iMarker].pos=std::stoull(lineList[1]);
            split(formatList,lineList[8],':');
            iGT=-1,iAD=-1;
            for(iFormat=0;iFormat<formatList.size();++iFormat){
                if(formatList[iFormat]=="AD") iAD=iFormat;
                else if(formatList[iFormat]=="GT") iGT=iFormat;
            }
            if(iAD<0||iGT<0){
                data(iMarker,0) = MML::DATA_NA;
                data(iMarker,1) = MML::DATA_NA;
                data(iMarker,2) = MML::DATA_NA;
                data(iMarker,3) = MML::DATA_NA;
            }
            else{
                split(infoList,lineList[9],':');
                if(infoList[iGT].find('.')!=std::string::npos||
                   infoList[iAD].find('.')!=std::string::npos){
                    data(iMarker,0) = MML::DATA_NA;
                    data(iMarker,1) = MML::DATA_NA;
                }
                else{
                    split(gtList,infoList[iGT],"/|");
                    split(adList,infoList[iAD],',');
                    tAD1=0,tAD2=0;
                    for(i=0;i<gtList.size();++i){
                        if(gtList[i]=="0") tAD1+=std::stoull(adList[i]);
                        else tAD2+=std::stoull(adList[i]);
                    }
                    data(iMarker,0) = tAD1;
                    data(iMarker,1) = tAD2;
                }

                split(infoList,lineList[10],':');
                if(infoList[iGT].find('.')!=std::string::npos||
                   infoList[iAD].find('.')!=std::string::npos){
                    data(iMarker,2) = MML::DATA_NA;
                    data(iMarker,3) = MML::DATA_NA;
                }
                else{
                    split(gtList,infoList[iGT],"/|");
                    split(adList,infoList[iAD],',');
                    tAD1=0,tAD2=0;
                    for(i=0;i<gtList.size();++i){
                        if(gtList[i]=="0") tAD1+=std::stoull(adList[i]);
                        else tAD2+=std::stoull(adList[i]);
                    }
                    data(iMarker,2) = tAD1;
                    data(iMarker,3) = tAD2;
                }

            }
            ++iMarker;
        }
    }
    return true;
}

BSASpecificRead1::BSASpecificRead1()
{

}

BSASpecificRead1::BSASpecificRead1(const char *inFile)
    :BSASpecificRead1()
{
    load(inFile);
}

bool BSASpecificRead1::load(const char *inFile)
{
    close();
    file.open(inFile,std::ios::in);
    if(file.is_open()) isValid = true;
    return isValid;
}

bool BSASpecificRead1::close()
{
    if(file.is_open()) file.close();
    isValid = false;
    return true;
}

bool BSASpecificRead1::readAll(MML::Mat &data,
                               std::vector<std::string> &poolName,
                               ValPoints2 *&points,
                               std::vector<std::string>& chrName)
{
    if(!isValid) return false;
    if(points){
        delete [] points;
        points = nullptr;
    }
    data.clear();
    poolName.clear();
    chrName.clear();
    file.clear();
    file.seekg(0,std::ios::beg);
    std::map<std::string,size_t> idChrMap;
    std::vector<std::string> lineList,tList;
    std::string line;
    bool header=false;
    size_t iMarker=0,idChr=0;
    while(std::getline(file,line)){
        strip(line);
        if(line.empty()) continue;
        split(lineList,line,'\t');
        if(lineList.size()!=4){
            return false;
        }
        if(line[0] == '#'){
            if(header) continue;
            header = true;
            poolName.push_back(lineList[2]);
            poolName.push_back(lineList[3]);
        }
        else{
            if(idChrMap.find(lineList[0])==idChrMap.end()){
                idChrMap[lineList[0]]=idChr;
                chrName.push_back(lineList[0]);
                ++idChr;
            }
            ++iMarker;
        }
    }
    points = new ValPoints2[iMarker+1];
    data.resize(iMarker,4);
    file.clear();
    file.seekg(0,std::ios::beg);
    iMarker=0;
    while(std::getline(file,line)){
        strip(line);
        if(line.empty()) continue;
        split(lineList,line,'\t');
        if(line[0] != '#'){
            points[iMarker].idchr=idChrMap[lineList[0]];
            points[iMarker].pos=std::stoull(lineList[1]);
            split(tList,lineList[2],',');
            data(iMarker,0)=std::stod(tList[0]);
            data(iMarker,1)=std::stod(tList[1]);
            split(tList,lineList[3],',');
            data(iMarker,2)=std::stod(tList[0]);
            data(iMarker,3)=std::stod(tList[1]);
            ++iMarker;
        }
    }
    return true;
}

BSAProject::BSAProject()
{
    points = nullptr;
    charChrName = nullptr;

    threDeep = 7;
    threLow  = 0.3;
    threHigh = 0.7;
    threQUAL = 20;

    winLength    = 1000000;
    strideLength = 10000;

    nHP = 0;
    nLP = 0;

    isValid = false;

    proInfo = PROJECT_BSA;
}

BSAProject::~BSAProject()
{
    if(points) delete [] points;
    if(charChrName){
        for(size_t i=0;i<nCharChrName;++i){
            delete [] charChrName[i];
        }
        delete [] charChrName;
    }
}

bool BSAProject::setThreDeep(double inVal)
{
    if(inVal < 0) threDeep = -1;
    else threDeep = inVal;
    return true;
}

bool BSAProject::setThreLow(double inVal)
{
    if(inVal < 0) threLow = -1;
    else if(inVal > 1) threLow = 1.5;
    else threLow = inVal;
    return true;
}

bool BSAProject::setThreHigh(double inVal)
{
    if(inVal < 0) threHigh = -1;
    else if(inVal > 1) threHigh = 1.5;
    else threHigh = inVal;
    return true;
}

bool BSAProject::setThreQUAL(double inVal)
{
    threQUAL = inVal;
    return true;
}

bool BSAProject::setPValue(double inVal)
{
    pvalue = inVal;
    return true;
}

bool BSAProject::setWinLength(size_t inVal)
{
    winLength = inVal;
    return true;
}

bool BSAProject::setStrideLength(size_t inVal)
{
    strideLength = inVal;
    return true;
}

bool BSAProject::setNHighPool(size_t inVal)
{
    nHP = inVal;
    return true;
}

bool BSAProject::setNLowPool(size_t inVal)
{
    nLP = inVal;
    return true;
}

size_t BSAProject::searchPointIndex(int inIDChr, size_t inPos) const
{
    if(inIDChr >= chrName.size()||inIDChr<0) return SIZE_MAX;
    size_t chrStartIndex = chrStartId[inIDChr];
    size_t chrEndIndex = chrStartId[inIDChr+1];
    ValPoints2* iter = std::lower_bound(&(points[chrStartIndex]),
                                 &(points[chrEndIndex]),
                                 inPos);
    return std::distance(&points[0],iter);
}

size_t BSAProject::getMeanSamples() const
{
    return size_t((nHP+nLP)/2);
}

long BSAProject::getMaxDepth() const
{
    return maxDepth;
}

double BSAProject::getPValue() const
{
    return pvalue;
}

char **BSAProject::getChrName()
{
    if(!charChrName){
        if(chrName.empty()) return nullptr;
        nCharChrName = chrName.size();
        charChrName = new char*[nCharChrName];
        for(size_t i=0;i<nCharChrName;++i){
            charChrName[i] = new char[FD::MAXELEMLEN];
            FD::charCopy(charChrName[i],chrName[i].data());
        }
    }
    return charChrName;
}

bool BSAProject::sortPoints()
{
    sortData = false;
    if(!readPoints) return false;
    size_t* idList = new size_t[data.getNRow()+1];
    std::iota(&(idList[0]),&(idList[data.getNRow()]),0);
    std::sort(&(idList[0]),&(idList[data.getNRow()]),[&](size_t a,size_t b){return points[a]<points[b];});
    data.setRows(data.getNRow(),idList);
    delete [] idList;
    chrStartId.resize(chrName.size()+1);
    int preChrId=chrName.size();
    for(size_t iMarker=0;iMarker<data.getNRow();++iMarker){
        if(points[iMarker].idchr!=preChrId){
            preChrId=points[iMarker].idchr;
            chrStartId[preChrId]=iMarker;
        }
    }
    chrStartId[chrName.size()]=data.getNRow();
    sortData = true;
    if(sortData&&readChrLen) isValid = true;
    return true;
}

bool BSAProject::readChrFile(const char *inFile)
{
    readChrLen = false;
    if(!inFile) return false;
    std::ifstream file(inFile,std::ios::in);
    std::vector<std::string> lineList;
    std::string line;
    std::map<std::string,size_t> idChrMap;
    std::set<std::string> chrSet;
    for(size_t i=0;i<chrName.size();++i){
        idChrMap[chrName[i]]=i;
    }
    chrLen.resize(chrName.size());
    while(std::getline(file,line)){
        strip(line);
        if(line.empty()) continue;
        split(lineList,line,'\t');
        if(idChrMap.find(lineList[0])==idChrMap.end()) continue;
        chrLen[idChrMap[lineList[0]]]=std::stoull(lineList[1]);
        chrSet.insert(lineList[0]);
    }
    if(chrSet.size() != chrName.size()) return false;
    readChrLen = true;
    if(sortData&&readChrLen) isValid = true;
    return true;
}

bool BSAProject::readFileVCF(const char *inFile)
{
    isValid = false;
    sortData = false;
    readPoints = false;
    if(!(readerVCF.load(inFile))) return false;
    if(!(readerVCF.readAll(data,poolName,points,chrName))) return false;
    maxDepth = 0;
    for(size_t iMarker=0;iMarker<data.getNRow();++iMarker){
        maxDepth = std::max(std::lround(data(iMarker,0)+data(iMarker,1)),maxDepth);
        maxDepth = std::max(std::lround(data(iMarker,2)+data(iMarker,3)),maxDepth);
    }
//    qDebug() << "maxDep:" << maxDepth;
    readPoints = true;
    return true;
}

bool BSAProject::readFile1(const char *inFile)
{
    isValid = false;
    sortData = false;
    readPoints = false;
    if(!(reader1.load(inFile))) return false;
    if(!(reader1.readAll(data,poolName,points,chrName))) return false;
    maxDepth = 0;
    for(size_t iMarker=0;iMarker<data.getNRow();++iMarker){
        maxDepth = std::max(std::lround(data(iMarker,0)+data(iMarker,1)),maxDepth);
        maxDepth = std::max(std::lround(data(iMarker,2)+data(iMarker,3)),maxDepth);
    }
//    qDebug() << "maxDep:" << maxDepth;
    readPoints = true;
    return true;
}

bool BSAProject::saveOutput(const char *outFile)
{
    return true;
}


};

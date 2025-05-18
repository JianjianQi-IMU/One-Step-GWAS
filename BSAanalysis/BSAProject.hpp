#ifndef BSAPROJECT_HPP
#define BSAPROJECT_HPP

#include "DataManager/ValPoints.hpp"
#include "DataManager/DataManager.hpp"
#include "FileDeal/BaseProject.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

#include <QDebug>

namespace FD{

class BSAInterVal
{
public:
    int    idChr;
    size_t start;
    size_t stop;
    size_t n;
    double indexH;
    double indexL;
    double val;
    double thre;

    BSAInterVal();
    bool operator<(const BSAInterVal& B) const;
    double getData(short info);

};

class BSAVCFRead
{
private:
    bool isValid;
    std::fstream file;
public:
    explicit BSAVCFRead();
    explicit BSAVCFRead(const char* inFile);
    bool load(const char* inFile);
    bool close();
    bool readAll(MML::Mat& data,
                 std::vector<std::string>& poolName,
                 ValPoints2*& points,
                 std::vector<std::string>& chrName);
};

class BSASpecificRead1
{
private:
    bool isValid;
    std::fstream file;
public:
    explicit BSASpecificRead1();
    explicit BSASpecificRead1(const char* inFile);
    bool load(const char* inFile);
    bool close();
    bool readAll(MML::Mat& data,
                 std::vector<std::string>& poolName,
                 ValPoints2*& points,
                 std::vector<std::string>& chrName);
};

class BSAProject : public BaseProject
{
private:
    bool isValid;
    bool readPoints;
    bool sortData;
    bool readChrLen;

    BSASpecificRead1 reader1;
    BSAVCFRead       readerVCF;
    char** charChrName;
    size_t nCharChrName;

public:

    MML::Mat data; // nMarker * 4 --> nMarker * (A_HP, a_HP, A_LP, a_LP;)
                   //HP:High Pool; LP:Low Pool
    std::vector<std::string> poolName; // poolName[0]:HP; poolName[1]:LP

    ValPoints2* points; // the length is nMarkers+1
    std::vector<size_t> chrStartId; // the length is nChr+1

    std::vector<std::string> chrName;

    std::vector<size_t> chrLen;

    long maxDepth;

    double threDeep;
    double threLow;
    double threHigh;
    double threQUAL;
    double pvalue;

    size_t winLength;    // windows length
    size_t strideLength; // stride length

    size_t nHP; //the number of samples of HP
    size_t nLP; //the number of samples of LP

    explicit BSAProject();
    ~BSAProject();


    bool setThreDeep(double inVal);
    bool setThreLow (double inVal);
    bool setThreHigh(double inVal);
    bool setThreQUAL(double inVal);
    bool setPValue(double inVal);
    bool setWinLength(size_t inVal);
    bool setStrideLength(size_t inVal);
    bool setNHighPool(size_t inVal);
    bool setNLowPool(size_t inVal);

    size_t searchPointIndex(int inIDChr,size_t inPos) const;

    size_t getMeanSamples() const;
    long getMaxDepth() const;
    double getPValue() const;
    char** getChrName();

    bool sortPoints();

    bool readChrFile(const char* inFile);

    bool readFileVCF(const char* inFile);
    bool readFile1(const char* inFile);

    bool saveOutput(const char* outFile) override;
};

};

#endif

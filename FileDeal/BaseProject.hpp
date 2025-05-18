#ifndef BASEPROJECT_HPP
#define BASEPROJECT_HPP

#include <fstream>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <string>

#include "DataManager/DataManager.hpp"

namespace FD {

const long long MAXLINELEN = 1 << 20;
const long long MAXELEMLEN = 1 << 10;

enum ProjectInfo
{
    PROJECT_NULL = 0,
    PROJECT_GWAS,
    PROJECT_BSA,
    PROJECT_MAXLEN,
};

enum ReadState
{
    FD_ENABLED = 0,
    FD_N_SAMPLEERROR = 1,
    FD_N_MARKERERROR = 2,
    FD_N_FACTORERROR = 3,
    FD_READINGERROR = 4,
    FD_UNKNOWN = 5
};

bool compare(const char* s1, const char* s2);
bool pstrip(char* str);
bool strip(std::string& str);
bool strip(char* str);
long long split(const char* str, char** outList, char sep, long long max);
long long split(const char* str, char** outList, const char* sep, long long max);
bool split(std::vector<std::string>& list, const std::string& line, char sep);
bool split(std::vector<std::string>& list, const std::string& line, const std::string& sep);
bool split(std::vector<std::string>& list, const std::string& line, char sep,long long max);
bool split(std::vector<std::string>& list, const std::string& line, const std::string& sep, long long max);
void charCopy(char* object,const char* source);
void charCopy(char* object,const char* source, size_t len);
size_t charLen(const char* str);
bool charLess(const char* s1, const char* s2);

class BasePopGenoData
{
protected:
    size_t readPoint;

public:
    bool isValid;
    BasePopGenoData();
    virtual ~BasePopGenoData();
    virtual bool read2(double* out, size_t nMarkers = 1) = 0;
    virtual bool resetReadPoint();
    virtual void clear() = 0;
};

class BaseFileRead
{
protected:
    std::fstream file;
public:
    bool isValid;
    BaseFileRead();
    virtual ~BaseFileRead();
    virtual bool load(const char* inFile);
    virtual bool close();
    virtual bool clear();
};

class BedData : public BasePopGenoData
{
public:
    char* data;
    size_t dataSize;
    size_t nSample;
    size_t nMarker;
    explicit BedData();
    ~BedData();
    bool read(short* out, size_t nMarkers = 1);
    bool read2(double* out, size_t nMarkers = 1);
    void clear();
};

class PolyPedData : public BasePopGenoData
{
public:
    size_t nPloid;
    short* data;
    size_t nSample;
    size_t nMarker;
    PolyPedData();
    ~PolyPedData();
    bool read(short* out, size_t outNMarkers = 1);
    bool read2(double* out, size_t outNMarkers = 1);
    void clear();
};

class BaseProject
{
public:
    ProjectInfo proInfo;
    std::string logInfo;
    virtual bool saveOutput(const char* outFile) = 0;
    explicit BaseProject();
};

};

#endif // BASEPROJECT_HPP

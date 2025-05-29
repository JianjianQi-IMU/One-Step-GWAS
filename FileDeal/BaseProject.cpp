#include "BaseProject.hpp"

bool FD::compare(const char *s1, const char *s2)
{
    size_t i=0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) return false;
        ++i;
    }
    if (s1[i] != s2[i]) return false;
    return true;
}

bool FD::pstrip(char *str)
{
    size_t i, pi, len;
    bool f = true;
    for (i = 0, pi = 0; str[i] != '\0'; ++i) {
        if ((str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\r') && f) {
            ++pi;
        } else {
            f = false;
        }
    }
    len = i;
    if (pi == 0) return true;
    for (i = 0; i + pi <= len; ++i) {
        str[i] = str[pi + i];
    }
    return true;
}

bool FD::strip(std::string &line)
{
    while ((!line.empty()) && (line.back() == '\r' || line.back() == '\t' || line.back() == ' ' || line.back() == '\n')) {
        line.pop_back();
    }
    return true;
}


bool FD::strip(char *str)
{
    long long i = 0;
    for (; str[i]; ++i) {
        ;
    }
    --i;
    while (i >= 0 && (str[i] == '\r' || str[i] == '\t' || str[i] == ' ' || str[i] == '\n')) {
        --i;
    }
    str[i + 1] = '\0';
    return true;
}

long long FD::split(const char *str, char **outList, char sep, long long max = 0)
{
    long long i = 0, pi = 0, ni = 0, j = 0;
    bool f = false;
    for (i = 0; i < MAXLINELEN && str[i] != '\0'; ++i) {
        if (max != 0 && ni >= max) break;
        if (f && str[i] == sep) {
            f = false;
            for (j = pi; j < i && j - pi < MAXELEMLEN; ++j) {
                outList[ni][j - pi] = str[j];
            }
            outList[ni][j - pi] = '\0';
            ++ni;
        } else if ((!f) && str[i] != sep) {
            pi = i;
            f = true;
        }
    }
    if ((max == 0 || ni < max) && f) {
        for (j = pi; j < i && j - pi < MAXELEMLEN; ++j) {
            outList[ni][j - pi] = str[j];
        }
        outList[ni][j - pi] = '\0';
        ni++;
    }
    return ni;
}

long long FD::split(const char *str, char **outList, const char *sep, long long max)
{
    long long i = 0, pi = 0, ni = 0, j = 0, si = 0;
    bool f = false, ff = false;
    for (i = 0; i < MAXLINELEN && str[i] != '\0'; ++i) {
        if (max != 0 && ni >= max) break;
        si = 0;
        ff = false;
        while (sep[si]) {
            if (sep[si] == str[i]) ff = true;
            ++si;
        }
        if (f && ff) {
            f = false;
            for (j = pi; j < i && j - pi < MAXELEMLEN; ++j) {
                outList[ni][j - pi] = str[j];
            }
            outList[ni][j - pi] = '\0';
            ++ni;
        } else if ((!f) && (!ff)) {
            pi = i;
            f = true;
        }
    }
    if ((max == 0 || ni < max) && f) {
        for (j = pi; j < i && j - pi < MAXELEMLEN; ++j) {
            outList[ni][j - pi] = str[j];
        }
        outList[ni][j - pi] = '\0';
        ni++;
    }
    return ni;
}

bool FD::split(std::vector<std::string> &list, const std::string &line, char sep)
{
    list.clear();
    int i = 0, pi = 0, len = line.length();
    bool f = false;
    for (i = 0; i < len; i++) {
        if (f && line[i] == sep) {
            f = false;
            list.emplace_back(line.substr(pi, i - pi));
        } else if ((!f) && line[i] != sep) {
            f = true;
            pi = i;
        }
    }
    if (f) list.emplace_back(line.substr(pi, i - pi));
    return true;
}

bool FD::split(std::vector<std::string> &list, const std::string &line, const std::string &sep)
{
    list.clear();
    int i = 0, pi = 0, len = line.length();
    bool f = false, ff = false;
    for (i = 0; i < len; i++) {
        ff = false;
        for (char ty : sep) {
            ff = (ff || ty == line[i]);
        }
        if (f && ff) {
            f = false;
            list.emplace_back(line.substr(pi, i - pi));
        } else if ((!f) && (!ff)) {
            f = true;
            pi = i;
        }
    }
    if (f) list.emplace_back(line.substr(pi, i - pi));
    return true;
}

bool FD::split(std::vector<std::string> &list, const std::string &line, char sep, long long max)
{
    list.clear();
    int i = 0, pi = 0, len = line.length();
    bool f = false;
    for (i = 0; i < len; i++) {
        if (f && line[i] == sep) {
            f = false;
            list.emplace_back(line.substr(pi, i - pi));
        } else if ((!f) && line[i] != sep) {
            f = true;
            pi = i;
        }
        if (max > 0 && list.size() >= max) break;
    }
    if (f && !(max > 0 && list.size() >= max)) {
        list.emplace_back(line.substr(pi, i - pi));
    }
    return true;
}

bool FD::split(std::vector<std::string> &list, const std::string &line, const std::string &sep, long long max)
{
    list.clear();
    int i = 0, pi = 0, len = line.length();
    bool f = false, ff = false;
    for (i = 0; i < len; i++) {
        ff = false;
        for (char ty : sep) {
            ff = (ff || ty == line[i]);
        }
        if (f && ff) {
            f = false;
            list.emplace_back(line.substr(pi, i - pi));
        } else if ((!f) && (!ff)) {
            f = true;
            pi = i;
        }
        if (max > 0 && list.size() >= max) break;
    }
    if (f && !(max > 0 && list.size() >= max)) {
        list.emplace_back(line.substr(pi, i - pi));
    }
    return true;
}

bool FD::charLess(const char *s1, const char *s2)
{
    size_t i = 0;
    while (s1[i] != '\0' || s2[i] != '\0') {
        if (s1[i] > s2[i]) return false;
        else if (s1[i] < s2[i]) return true;
        ++i;
    }
    return false;
}

FD::BasePopGenoData::BasePopGenoData()
{

}

FD::BasePopGenoData::~BasePopGenoData(){}

bool FD::BasePopGenoData::resetReadPoint()
{
    if (!isValid) return false;
    readPoint = 0;
    return true;
}

FD::BaseFileRead::BaseFileRead()
{
    isValid = false;
}

FD::BaseFileRead::~BaseFileRead()
{
    clear();
}

bool FD::BaseFileRead::load(const char *inFile)
{
    if (!clear()) return false;
    file.open(inFile, std::ios::in);
    if (file.is_open()) isValid = true;
    else isValid = false;
    return isValid;
}

bool FD::BaseFileRead::close()
{
    if (file.is_open()) {
        file.close();
        if (file.is_open()) return false;
    }
    return true;
}

bool FD::BaseFileRead::clear()
{
    return close();
}

FD::BedData::BedData()
{
    data = nullptr;
    nSample = 0;
    readPoint = 0;
    dataSize = 0;
    nMarker = 0;
    isValid = false;
}

FD::BedData::~BedData()
{
    clear();
}
// out[isam + iMarkers * nSample] --> 0, 1, 2 : AA, Aa, aa
bool FD::BedData::read(short *out, size_t nMarkers)
{
    if (!isValid || !data || !out) {
        return false;
    }
    size_t nByte = ((nSample + 3) / 4) * nMarkers;
    size_t i = 0;
    size_t isam = 0;
    size_t j = 0;
    size_t iMarkers = 0;
    unsigned short tByte = 0, tc = 3;
    char* pByte = new (std::nothrow) char [nByte];
    if (pByte == nullptr) {
        return false;
    }
    if (readPoint + nByte <= dataSize) {
        charCopy(pByte, &data[readPoint], nByte);
        readPoint += nByte;
    }

    for (i = 0; i < nByte; ++i)
    {
        tByte = (unsigned short)(pByte[i]);
        for (j = 0; j < 4 && isam < nSample; ++j, ++isam)
        {
            switch(tByte & tc){
            case 0:
                out[isam + iMarkers * nSample] = 0;
                break;
            case 3:
                out[isam + iMarkers * nSample] = 2;
                break;
            case 2:
                out[isam + iMarkers * nSample] = 1;
                break;
            default:
                out[isam + iMarkers * nSample] = MML::UNASSIGNED;
                break;
            }
            tByte = tByte >> 2;
        }
        if (isam >= nSample) {
            ++iMarkers;
            isam = 0;
        }
    }

    if (pByte != nullptr) {
        delete [] pByte;
        pByte = nullptr;
    }

    return true;
}

// out[isam + iMarkers * nSample] --> 0, 0.5, 1.0 : AA, Aa, aa
bool FD::BedData::read2(double *out, size_t nMarkers)
{
    if(!isValid || !data || !out) {
        return false;
    }
    size_t nByte=((nSample + 3) / 4) * nMarkers;
    size_t i = 0;
    size_t isam = 0;
    size_t j = 0;
    size_t iMarkers = 0;
    unsigned short tByte = 0, tc = 3;
    char* pByte= new (std::nothrow) char [nByte];
    if (pByte == nullptr) {
        return false;
    }
    if (readPoint + nByte <= dataSize){
        charCopy(pByte, &data[readPoint], nByte);
        readPoint += nByte;
    }

    for(i = 0; i < nByte; ++i)
    {
        tByte = (unsigned short)(pByte[i]);
        for(j = 0; j < 4 && isam < nSample; ++j, ++isam)
        {
            switch(tByte & tc){
                case 0:
                    out[isam + iMarkers * nSample] = 0;
                    break;
                case 3:
                    out[isam + iMarkers * nSample] = 1;
                    break;
                case 2:
                    out[isam + iMarkers * nSample] = 0.5;
                    break;
                default:
                    out[isam + iMarkers * nSample] = MML::DATA_NA;
                    break;
            }
            tByte = tByte >> 2;
        }
        if(isam >= nSample) {
            ++iMarkers;
            isam = 0;
        }
    }

    if (pByte != nullptr) {
        delete [] pByte;
        pByte = nullptr;
    }

    return true;
}


void FD::BedData::clear()
{
    nSample = 0;
    readPoint = 0;
    if (data) delete [] data;
    dataSize = 0;
    nMarker = 0;
    isValid = false;
    data = nullptr;
}

FD::PolyPedData::PolyPedData()
{
    data = nullptr;
    nPloid = 0;
    nSample = 0;
    nMarker = 0;
    readPoint = 0;
    isValid = false;
}

FD::PolyPedData::~PolyPedData()
{
    if (data) delete [] data;
}

bool FD::PolyPedData::read(short *out, size_t outNMarkers)
{
    if (!isValid || !data || !outNMarkers || !out) {
        return false;
    }
    if (readPoint + outNMarkers > nMarker) {
        return false;
    }
    for (size_t i = 0; i < outNMarkers; ++i) {
        for (size_t iSample = 0; iSample < nSample; ++iSample) {
            if (data[readPoint * nSample + iSample] != MML::UNASSIGNED) {
                out[i * nSample + iSample] = data[readPoint * nSample + iSample];
            } else {
                out[i * nSample + iSample] = MML::UNASSIGNED;
            }
        }
        ++readPoint;
    }
    return true;
}

bool FD::PolyPedData::read2(double *out, size_t outNMarkers)
{
    if (!isValid || !data || !outNMarkers || !out) {
        return false;
    }
    if (readPoint + outNMarkers > nMarker) {
        return false;
    }
    for (size_t i = 0; i < outNMarkers; ++i){
        for (size_t iSample = 0; iSample < nSample; ++iSample) {
            if (data[readPoint * nSample + iSample] != MML::UNASSIGNED) {
                out[i * nSample + iSample] = data[readPoint * nSample + iSample] / double(nPloid);
            } else{
                out[i * nSample + iSample] = MML::DATA_NA;
            }
        }
        ++readPoint;
    }
    return true;
}



void FD::PolyPedData::clear()
{
    if (data) delete [] data;
    data = nullptr;
    nPloid = 0;
    nSample = 0;
    nMarker = 0;
    readPoint = 0;
    isValid = false;
}

void FD::charCopy(char* object, const char* source){
    size_t i = 0;
    while (source[i]) {
        object[i] = source[i];
        ++i;
    }
    object[i] = '\0';
}

void FD::charCopy(char *object, const char *source, size_t len)
{
    size_t i = 0;
    while (i < len) {
        object[i] = source[i];
        ++i;
    }
}

size_t FD::charLen(const char *str)
{
    size_t i = 0;
    while (str[i]) {
        ++i;
    }
    return i;
}

FD::BaseProject::BaseProject()
{

}





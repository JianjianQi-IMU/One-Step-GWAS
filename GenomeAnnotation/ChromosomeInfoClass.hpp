#ifndef CHROMOSOMEINFOCLASS_HPP
#define CHROMOSOMEINFOCLASS_HPP

#include <vector>
#include <string>

#include "DataManager/MatrixLib.hpp"
#include "FileDeal/FileDeal.hpp"

class ChromosomeInfoClass
{
public:
    static const int chrNameMaxLen = 256;
private:
    long long *chrLen; // Chromosome
    long long *cumChrLen; // Chromosome
    char **chrName; // Chromosome
    int nChr; // Chromosome
    long long maxCumPos; // Chromosome

    int currentMinChri;
    int currentMaxChri;

public:
    ChromosomeInfoClass():
        chrLen(nullptr),
        cumChrLen(nullptr),
        chrName(nullptr),
        nChr(0),
        maxCumPos(0),
        currentMinChri(0),
        currentMaxChri(0)
    {}
    ~ChromosomeInfoClass()
    {
        if (chrLen) {
            delete [] chrLen;
        }
        if (cumChrLen) {
            delete [] cumChrLen;
        }
        if (chrName) {
            for (int i = 0; i < nChr; ++i) {
                delete [] chrName[i];
            }
            delete [] chrName;
        }

        chrLen    = nullptr;
        cumChrLen = nullptr;
        chrName   = nullptr;
        nChr      = 0;
        maxCumPos = 0;

        currentMinChri = 0;
        currentMaxChri = 0;
    }
    bool loadChromosome(long long *inChrLen, char** inChrName, int inNChr)
    {
        if (!inChrLen || !inChrName || !inNChr) return false;
        int i;
        if (nChr != inNChr) {
            if (chrLen) delete [] chrLen;
            if (cumChrLen) delete [] cumChrLen;
            if (chrName) {
                for (i = 0; i < nChr; ++i) delete [] chrName[i];
                delete [] chrName;
            }
            chrLen = new long long[inNChr];
            chrName = new char*[inNChr];
            cumChrLen = new long long[inNChr + 1];
        }
        nChr = inNChr;
        for (i = 0; i < nChr; ++i) {
            chrLen[i] = inChrLen[i];
            chrName[i] = new char[chrNameMaxLen];
            FD::charCopy(chrName[i], inChrName[i]);
            if (i == 0) cumChrLen[i] = 0;
            else cumChrLen[i] = cumChrLen[i - 1] + chrLen[i - 1];
        }
        maxCumPos = cumChrLen[nChr - 1] + chrLen[nChr - 1];
        cumChrLen[nChr] = maxCumPos;
        currentMaxChri = 0;
        currentMinChri = nChr - 1;
        return true;
    }
    bool loadChromosome(const std::vector<long long>& inChrLen,
                        const std::vector<std::string>& inChrName){
        if (inChrLen.empty() || inChrName.empty() ||
           inChrName.size() != inChrLen.size()) {
            return false;
        }
        int i;
        int inNChr = inChrName.size();
        if (nChr != inNChr) {
            if (chrLen) delete [] chrLen;
            if (cumChrLen) delete [] cumChrLen;
            if (chrName) {
                for (i = 0; i < nChr; ++i) delete [] chrName[i];
                delete [] chrName;
            }
            chrLen = new long long[inNChr];
            chrName = new char*[inNChr];
            cumChrLen = new long long[inNChr + 1];
        }
        nChr = inNChr;
        for (i = 0; i < nChr; ++i) {
            chrLen[i] = inChrLen[i];
            chrName[i] = new char[chrNameMaxLen];
            FD::charCopy(chrName[i], inChrName[i].c_str());
            if (i == 0) cumChrLen[i] = 0;
            else cumChrLen[i] = cumChrLen[i - 1] + chrLen[i - 1];
        }
        maxCumPos = cumChrLen[nChr - 1] + chrLen[nChr - 1];
        cumChrLen[nChr] = maxCumPos;
        currentMaxChri = 0;
        currentMinChri = nChr - 1;
        return true;
    }
    void preAdjustPaintChri(long double leftPosF, long double rightPosF)
    {
        int a = 0, b = nChr, m = 0;
        while (a <= b) {
            m = (a + b) / 2;
            if ((long double)(cumChrLen[m]) >= leftPosF) b = m - 1;
            else a = m + 1;
        }
        currentMinChri = b;
        a = 0, b = nChr;
        while (a <= b) {
            m = (a + b) / 2;
            if ((long double)cumChrLen[m] >= rightPosF) b = m - 1;
            else a = m + 1;
        }
        currentMaxChri = b;
    }
    void setCurrentChri(int minChri, int maxChri)
    {
        currentMinChri = minChri;
        currentMaxChri = maxChri;
    }
    void getCurrentChri(int &outMinChri, int &outMaxChri)
    {
        outMinChri = currentMinChri;
        outMaxChri = currentMaxChri;
    }
    char** getChrName()
    {
        return chrName;
    }
    long long* getChrLen()
    {
        return chrLen;
    }
    long long* getCumChrLen()
    {
        return cumChrLen;
    }
    int getNChr()
    {
        return nChr;
    }
    long long getMaxCumPos()
    {
        return maxCumPos;
    }
};

//int ChromosomeInfoClass::chrNameMaxLen = 256;

#endif // CHROMOSOMEINFOCLASS_HPP

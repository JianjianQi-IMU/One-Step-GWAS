#ifndef FILEITERATOR_HPP
#define FILEITERATOR_HPP

#include "FileDeal.hpp"

namespace FD {

class BaseMarkerIterator
{
protected:
    size_t readPoint;
    size_t readNum;
    bool isValid;
public:
    BaseMarkerIterator();
    explicit BaseMarkerIterator(const BaseMarkerIterator& object);
    virtual ~BaseMarkerIterator();
    virtual bool next();
    virtual bool isEnd();
    void copy(const BaseMarkerIterator& object);
    void operator=(const BaseMarkerIterator& object);
    void operator=(BaseMarkerIterator&& object);
};

class BedDataIterator : public BaseMarkerIterator
{
private:
    char* data;
    size_t nSample;
public:
    BedDataIterator();
    BedDataIterator(BedData& inBedData, size_t start = 0, size_t stop = 0);
    bool loadData(BedData& inBedData, size_t start = 0, size_t stop = 0);
    bool read(short* out);
    bool read2(double* out);
    void operator=(const BedDataIterator& object);
};

class PolyPedDataIterator : public BaseMarkerIterator
{
private:
    size_t nPloid;
    short* data;
    size_t nSample;
public:
    PolyPedDataIterator();
    PolyPedDataIterator(PolyPedData& inPolyPedData, size_t start = 0, size_t stop = 0);
    bool loadData(PolyPedData& inPolyPedData, size_t start = 0, size_t stop = 0);
    bool read(short* out);
    bool read2(double* out);
    void operator=(const PolyPedDataIterator& object);
};

class BIMDataIterator : public BaseMarkerIterator
{
private:
    BIMNode* data;
public:
    BIMDataIterator();
    BIMDataIterator(BIMData& inBIMData, size_t start = 0, size_t stop = 0);
    bool loadData(BIMData& inBIMData, size_t start = 0, size_t stop = 0);
    BIMNode* read();
    void operator=(const BIMDataIterator& object);
};

class BIMLogPDataIterator : public BaseMarkerIterator
{
private:
    BIMLogPNode* data;
public:
    BIMLogPDataIterator();
    BIMLogPDataIterator(BIMLogPData& inBIMLogPData, size_t start = 0, size_t stop = 0);
    bool loadData(BIMLogPData& inBIMLogPData, size_t start = 0, size_t stop = 0);
    BIMLogPNode* read();
    void operator=(const BIMLogPDataIterator& object);
};

class PedDataIteratorSet
{
public:
    int info;
    double dataNA;
    short  unassigned;
    size_t nSample;
    size_t nScale;
    BedDataIterator bedIter;
    PolyPedDataIterator ppedIter;
    PedDataIteratorSet();
    PedDataIteratorSet(const PedDataIteratorSet& object);
    void setBedIter(const BedDataIterator& iter);
    void setPPedIter(const PolyPedDataIterator& iter);
    bool read2(double* out);
    bool next();
    bool isEnd();
    void copy(const PedDataIteratorSet& object);
    void operator=(const PedDataIteratorSet& object);
};

}

#endif // FILEITERATOR_HPP

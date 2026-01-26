#ifndef FILEITERATOR_HPP
#define FILEITERATOR_HPP

#include "FileDeal.hpp"

namespace FD {

enum FileIterPedClass : int32_t
{
    FILEITER_UNVALID = -1,
    FILEITER_BED = 0,
    FILEITER_PPED,
    FILEITER_NUM
};

class BaseMarkerIterator
{
protected:
    uint64_t readPoint;
    uint64_t readNum;
    bool isValid;
public:
    BaseMarkerIterator();
    explicit BaseMarkerIterator(const BaseMarkerIterator& object);
    virtual ~BaseMarkerIterator();
    virtual bool next();
    virtual bool isEnd();
    virtual bool reset();
    void copy(const BaseMarkerIterator& object);
    void operator=(const BaseMarkerIterator& object);
    void operator=(BaseMarkerIterator&& object);
};

class BedDataIterator : public BaseMarkerIterator
{
private:
    char* data;
    uint64_t nSample;
public:
    BedDataIterator();
    BedDataIterator(BedData& inBedData, uint64_t start = 0, uint64_t stop = 0);
    bool loadData(BedData& inBedData, uint64_t start = 0, uint64_t stop = 0);
    bool read(int16_t* out) const;
    bool read2(double* out) const;
    bool readAt(int16_t* out, int64_t idxMarker) const;
    bool read2At(double* out, int64_t idxMarker) const;
    uint64_t getNSample() const;
    uint64_t getNMarker() const;
    void operator=(const BedDataIterator& object);
};

class PolyPedDataIterator : public BaseMarkerIterator
{
private:
    uint64_t nPloid;
    int16_t* data;
    uint64_t nSample;
public:
    PolyPedDataIterator();
    PolyPedDataIterator(PolyPedData& inPolyPedData, uint64_t start = 0, uint64_t stop = 0);
    bool loadData(PolyPedData& inPolyPedData, uint64_t start = 0, uint64_t stop = 0);
    bool read(int16_t* out) const;
    bool read2(double* out) const;
    bool readAt(int16_t* out, int64_t idxMarker) const;
    bool read2At(double* out, int64_t idxMarker) const;
    uint64_t getNSample() const;
    uint64_t getNMarker() const;
    void operator=(const PolyPedDataIterator& object);
};

class BIMDataIterator : public BaseMarkerIterator
{
private:
    BIMNode* data;
public:
    BIMDataIterator();
    BIMDataIterator(BIMData& inBIMData, uint64_t start = 0, uint64_t stop = 0);
    bool loadData(BIMData& inBIMData, uint64_t start = 0, uint64_t stop = 0);
    BIMNode* read();
    void operator=(const BIMDataIterator& object);
};

class BIMLogPDataIterator : public BaseMarkerIterator
{
private:
    BIMLogPNode* data;
public:
    BIMLogPDataIterator();
    BIMLogPDataIterator(BIMLogPData& inBIMLogPData, uint64_t start = 0, uint64_t stop = 0);
    bool loadData(BIMLogPData& inBIMLogPData, uint64_t start = 0, uint64_t stop = 0);
    BIMLogPNode* read();
    BIMLogPNode* readAt(uint64_t idx);
    void operator=(const BIMLogPDataIterator& object);
};

class PedDataIteratorSet
{
public:
    FileIterPedClass info;
    double dataNA;
    int16_t  unassigned;
    uint64_t nSample;
    uint64_t nScale;
    BedDataIterator bedIter;
    PolyPedDataIterator ppedIter;
    PedDataIteratorSet();
    PedDataIteratorSet(const PedDataIteratorSet& object);
    void setBedIter(const BedDataIterator& iter);
    void setPPedIter(const PolyPedDataIterator& iter);
    uint64_t getNSample() const;
    uint64_t getNMarker() const;
    bool read(int16_t* out) const;
    bool read2(double* out) const;
    bool readAt(int16_t* out, int64_t idxMarker) const;
    bool read2At(double* out, int64_t idxMarker) const;
    bool next();
    bool isEnd();
    void copy(const PedDataIteratorSet& object);
    void operator=(const PedDataIteratorSet& object);
};

}

#endif // FILEITERATOR_HPP

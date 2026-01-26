#include "FileIterator.hpp"
#include <cstdint>

namespace FD {

BaseMarkerIterator::BaseMarkerIterator()
{
    readPoint = 0;
    readNum = 0;
    isValid = false;
}

BaseMarkerIterator::BaseMarkerIterator(const BaseMarkerIterator &object)
{
    copy(object);
}

BaseMarkerIterator::~BaseMarkerIterator()
{

}

bool BaseMarkerIterator::next()
{
    if (!isValid) return false;
    if (readPoint == readNum) return false;
    ++readPoint;
    return true;
}

bool BaseMarkerIterator::isEnd()
{
    if (!isValid) return false;
    if (readPoint == readNum) return true;
    return false;
}

bool BaseMarkerIterator::reset()
{
    if (!isValid) return false;
    readPoint = 0;
    return true;
}

void BaseMarkerIterator::copy(const BaseMarkerIterator &object)
{
    readPoint = object.readPoint;
    readNum   = object.readNum;
    isValid   = object.isValid;
}

void BaseMarkerIterator::operator=(const BaseMarkerIterator &object)
{
    copy(object);
}

void BaseMarkerIterator::operator=(BaseMarkerIterator &&object)
{
    copy(object);
}

BedDataIterator::BedDataIterator() : BaseMarkerIterator()
{
    isValid = false;
    data = nullptr;
}

BedDataIterator::BedDataIterator(BedData &inBedData, uint64_t start, uint64_t stop)
    :BedDataIterator()
{
    loadData(inBedData,start,stop);
}

bool BedDataIterator::loadData(BedData &inBedData, uint64_t start, uint64_t stop)
{
    if (!inBedData.isValid) return false;
    if (!stop) stop = inBedData.nMarker;
    if (start >= stop) return false;
    uint64_t nByte = 0;
    nSample = inBedData.nSample;
    nByte = (nSample + 3) / 4;
    readNum = stop - start;
    data = (inBedData.data + start * nByte);
    readPoint = 0;
    isValid = true;
    return true;
}

bool BedDataIterator::read(int16_t *out) const
{
    if (!isValid || !data || !out) {
        return false;
    }
    uint64_t nByte = (nSample + 3) / 4, i = 0, isam = 0, j = 0;
    uint16_t tByte = 0, tc = 3;
    char* pByte = nullptr;
    if (readPoint >= readNum) {
        return false;
    }
    pByte = new char [nByte];
    charCopy(pByte, &data[readPoint * nByte], nByte);
    for (i = 0; i < nByte; ++i)
    {
        tByte = (uint16_t)(pByte[i]);
        for (j = 0; j < 4 && isam < nSample; ++j, ++isam)
        {
            switch (tByte & tc) {
            case 0:
                out[isam] = 0;
                break;
            case 3:
                out[isam] = 2;
                break;
            case 2:
                out[isam] = 1;
                break;
            default:
                out[isam] = MML::UNASSIGNED;
                break;
            }
            tByte = (tByte >> 2);
        }
    }
    delete [] pByte;
    return true;
}

bool BedDataIterator::read2(double *out) const
{
    if (!isValid || !data || !out) {
        return false;
    }
    uint64_t nByte = (nSample + 3) / 4, i = 0, isam = 0, j = 0;
    uint16_t tByte = 0, tc = 3;
    char* pByte = nullptr;
    if (readPoint >= readNum) {
        return false;
    }
    pByte = new char [nByte];
    charCopy(pByte, &data[readPoint * nByte], nByte);
    for (i = 0; i < nByte; ++i)
    {
        tByte = (uint16_t)(pByte[i]);
        for (j = 0; j < 4 && isam < nSample; ++j, ++isam)
        {
            switch (tByte & tc) {
            case 0:
                out[isam] = 0;
                break;
            case 3:
                out[isam] = 1;
                break;
            case 2:
                out[isam] = 0.5;
                break;
            default:
                out[isam] = MML::DATA_NA;
                break;
            }
            tByte = (tByte >> 2);
        }
    }
    delete [] pByte;
    return true;
}

/*
* aa, Aa, AA = 0, 1, 2
*/
bool BedDataIterator::readAt(int16_t* out, int64_t idxMarker) const
{
    if (!isValid || !data || !out) {
        return false;
    }
    uint64_t nByte = (nSample + 3) / 4, i = 0, isam = 0, j = 0;
    uint16_t tByte = 0, tc = 3;
    char* pByte = nullptr;
    if (idxMarker >= readNum || idxMarker < 0) {
        return false;
    }
    pByte = new char [nByte];
    charCopy(pByte, &data[idxMarker * nByte], nByte);
    for (i = 0; i < nByte; ++i)
    {
        tByte = (uint16_t)(pByte[i]);
        for (j = 0; j < 4 && isam < nSample; ++j, ++isam)
        {
            switch (tByte & tc) {
            case 0:
                out[isam] = 0;
                break;
            case 3:
                out[isam] = 2;
                break;
            case 2:
                out[isam] = 1;
                break;
            default:
                out[isam] = MML::UNASSIGNED;
                break;
            }
            tByte = (tByte >> 2);
        }
    }
    delete [] pByte;
    return true;
}

bool BedDataIterator::read2At(double *out, int64_t idxMarker) const
{
    if (!isValid || !data || !out) {
        return false;
    }
    uint64_t nByte = (nSample + 3) / 4, i = 0, isam = 0, j = 0;
    uint16_t tByte = 0, tc = 3;
    char* pByte = nullptr;
    if (idxMarker >= readNum || idxMarker < 0) {
        return false;
    }
    pByte = new char [nByte];
    charCopy(pByte, &data[idxMarker * nByte], nByte);
    for (i = 0; i < nByte; ++i)
    {
        tByte = (uint16_t)(pByte[i]);
        for (j = 0; j < 4 && isam < nSample; ++j, ++isam)
        {
            switch (tByte & tc) {
            case 0:
                out[isam] = 0;
                break;
            case 3:
                out[isam] = 1;
                break;
            case 2:
                out[isam] = 0.5;
                break;
            default:
                out[isam] = MML::DATA_NA;
                break;
            }
            tByte = (tByte >> 2);
        }
    }
    delete [] pByte;
    return true;
}

uint64_t BedDataIterator::getNSample() const
{
    return nSample;
}

uint64_t BedDataIterator::getNMarker() const
{
    return readNum;
}

void BedDataIterator::operator=(const BedDataIterator &object)
{
    copy(object);
    data = object.data;
    nSample = object.nSample;
}

PolyPedDataIterator::PolyPedDataIterator() : BaseMarkerIterator()
{
    isValid = false;
    data = nullptr;
}

PolyPedDataIterator::PolyPedDataIterator(PolyPedData &inPolyPedData, uint64_t start, uint64_t stop)
    :PolyPedDataIterator()
{
    loadData(inPolyPedData,start,stop);
}

bool PolyPedDataIterator::loadData(PolyPedData &inPolyPedData, uint64_t start, uint64_t stop)
{
    if (!inPolyPedData.isValid) return false;
    if (!stop) stop = inPolyPedData.nMarker;
    if (start >= stop) return false;
    nPloid = inPolyPedData.nPloid;
    nSample = inPolyPedData.nSample;
    readNum = stop - start;
    data = (inPolyPedData.data + start * nSample);
    readPoint = 0;
    isValid = true;
    return true;
}

/*
* aaaa, Aaaa, AAaa, AAAa, AAAA = 0, 1, 2, 3, 4
*/
bool PolyPedDataIterator::read(int16_t *out) const
{
    if (!isValid || !data || !out) {
        return false;
    }
    if (readPoint >= readNum) {
        return false;
    }
    for (uint64_t iSample = 0; iSample < nSample; ++iSample) {
        if (data[readPoint * nSample + iSample] != MML::UNASSIGNED) {
            out[iSample] = data[readPoint * nSample + iSample];
        } else {
            out[iSample] = MML::UNASSIGNED;
        }
    }
    return true;
}

bool PolyPedDataIterator::read2(double *out) const
{
    if (!isValid || !data || !out) {
        return false;
    }
    if (readPoint >= readNum) {
        return false;
    }
    for (uint64_t iSample = 0; iSample < nSample; ++iSample) {
        if (data[readPoint * nSample + iSample] != MML::UNASSIGNED) {
            out[iSample] = data[readPoint * nSample + iSample] / double(nPloid);
        } else {
            out[iSample] = MML::DATA_NA;
        }
    }
    return true;
}

bool PolyPedDataIterator::readAt(int16_t* out, int64_t idxMarker) const
{
    if (!isValid || !data || !out) {
        return false;
    }
    if (idxMarker >= readNum || idxMarker < 0) {
        return false;
    }
    for (uint64_t iSample = 0; iSample < nSample; ++iSample) {
        if (data[idxMarker * nSample + iSample] != MML::UNASSIGNED) {
            out[iSample] = data[idxMarker * nSample + iSample];
        } else {
            out[iSample] = MML::UNASSIGNED;
        }
    }
    return true;
}

bool PolyPedDataIterator::read2At(double* out, int64_t idxMarker) const
{
    if (!isValid || !data || !out) {
        return false;
    }
    if (idxMarker >= readNum || idxMarker < 0) {
        return false;
    }
    for (uint64_t iSample = 0; iSample < nSample; ++iSample) {
        if (data[idxMarker * nSample + iSample] != MML::UNASSIGNED) {
            out[iSample] = data[idxMarker * nSample + iSample] / double(nPloid);
        } else {
            out[iSample] = MML::DATA_NA;
        }
    }
    return true;
}

uint64_t PolyPedDataIterator::getNSample() const
{
    return nSample;
}

uint64_t PolyPedDataIterator::getNMarker() const
{
    return readNum;
}

void PolyPedDataIterator::operator=(const PolyPedDataIterator &object)
{
    copy(object);
    nPloid  = object.nPloid;
    data    = object.data;
    nSample = object.nSample;
}

BIMDataIterator::BIMDataIterator() : BaseMarkerIterator()
{
    isValid = false;
    data = nullptr;
}

BIMDataIterator::BIMDataIterator(BIMData &inBIMData, uint64_t start, uint64_t stop)
    :BIMDataIterator()
{
    loadData(inBIMData,start,stop);
}

bool BIMDataIterator::loadData(BIMData &inBIMData, uint64_t start, uint64_t stop)
{
    if (!inBIMData.isValid) return false;
    if (!stop) stop = inBIMData.nMarker;
    if (start >= stop) return false;
    readNum = stop-start;
    data = (inBIMData.data + start);
    readPoint = 0;
    isValid = true;
    return true;
}

BIMNode *BIMDataIterator::read()
{
    if (!isValid || !data || readPoint >= readNum) return nullptr;
    return data+readPoint;
}

void BIMDataIterator::operator=(const BIMDataIterator &object)
{
    copy(object);
    data = object.data;
}

BIMLogPDataIterator::BIMLogPDataIterator() : BaseMarkerIterator()
{
    isValid = false;
    data = nullptr;
}

BIMLogPDataIterator::BIMLogPDataIterator(BIMLogPData &inBIMLogPData, uint64_t start, uint64_t stop)
    :BIMLogPDataIterator()
{
    loadData(inBIMLogPData,start,stop);
}

bool BIMLogPDataIterator::loadData(BIMLogPData &inBIMLogPData, uint64_t start, uint64_t stop)
{
    if (!inBIMLogPData.isValid) return false;
    if (!stop) stop = inBIMLogPData.nMarker;
    if (start >= stop) return false;
    readNum = stop - start;
    data = (inBIMLogPData.data + start);
    readPoint = 0;
    isValid = true;
    return true;
}

BIMLogPNode *BIMLogPDataIterator::read()
{
    if (!isValid || !data || readPoint >= readNum) return nullptr;
    return data + readPoint;
}

BIMLogPNode *BIMLogPDataIterator::readAt(uint64_t idx)
{
    if (!isValid || !data) return nullptr;
    return data + idx;
}

void BIMLogPDataIterator::operator=(const BIMLogPDataIterator &object)
{
    copy(object);
    data = object.data;
}

PedDataIteratorSet::PedDataIteratorSet()
{
    info = FILEITER_UNVALID;
    dataNA = MML::DATA_NA;
    unassigned = MML::UNASSIGNED;
}

PedDataIteratorSet::PedDataIteratorSet(const PedDataIteratorSet &object)
{
    copy(object);
}

void PedDataIteratorSet::setBedIter(const BedDataIterator &iter)
{
    bedIter = iter;
    info = FILEITER_BED;
}

void PedDataIteratorSet::setPPedIter(const PolyPedDataIterator &iter)
{
    ppedIter = iter;
    info = FILEITER_PPED;
}

uint64_t PedDataIteratorSet::getNSample() const
{
    switch (info) {
    case FILEITER_BED:
        return bedIter.getNSample();
    case FILEITER_PPED:
        return ppedIter.getNSample();
    default:
        return 0;
    }
    return 0;
}

uint64_t PedDataIteratorSet::getNMarker() const
{
    switch (info) {
    case FILEITER_BED:
        return bedIter.getNMarker();
    case FILEITER_PPED:
        return ppedIter.getNMarker();
    default:
        return 0;
    }
    return 0;
}

bool PedDataIteratorSet::read(int16_t* out) const
{
    switch (info) {
    case FILEITER_BED:
        return bedIter.read(out);
    case FILEITER_PPED:
        return ppedIter.read(out);
    default:
        return false;
    }
    return false;
}

bool PedDataIteratorSet::read2(double *out) const
{
    switch (info) {
    case FILEITER_BED:
        return bedIter.read2(out);
    case FILEITER_PPED:
        return ppedIter.read2(out);
    default:
        return false;
    }
    return false;
}

bool PedDataIteratorSet::readAt(int16_t* out, int64_t idxMarker) const
{
    switch (info) {
    case FILEITER_BED:
        return bedIter.readAt(out, idxMarker);
    case FILEITER_PPED:
        return ppedIter.readAt(out, idxMarker);
    default:
        return false;
    }
    return false;
}

bool PedDataIteratorSet::read2At(double* out, int64_t idxMarker) const
{
    switch (info) {
    case FILEITER_BED:
        return bedIter.read2At(out, idxMarker);
    case FILEITER_PPED:
        return ppedIter.read2At(out, idxMarker);
    default:
        return false;
    }
    return false;
}

bool PedDataIteratorSet::next()
{
    switch (info) {
    case FILEITER_BED:
        return bedIter.next();
    case FILEITER_PPED:
        return ppedIter.next();
    default:
        return false;
    }
    return false;
}

bool PedDataIteratorSet::isEnd()
{
    switch (info) {
    case FILEITER_BED:
        return bedIter.isEnd();
    case FILEITER_PPED:
        return ppedIter.isEnd();
    default:
        return false;
    }
    return false;
}

void PedDataIteratorSet::copy(const PedDataIteratorSet &object)
{
    info = object.info;
    nSample = object.nSample;
    nScale = object.nScale;
    bedIter = object.bedIter;
    ppedIter = object.ppedIter;
}

void PedDataIteratorSet::operator=(const PedDataIteratorSet &object)
{
    copy(object);
}

}

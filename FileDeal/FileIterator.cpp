#include "FileIterator.hpp"


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
    if(!isValid) return false;
    if(readPoint == readNum) return false;
    ++readPoint;
    return true;
}

bool BaseMarkerIterator::isEnd()
{
    if(!isValid) return false;
    if(readPoint == readNum) return true;
    return false;
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

BedDataIterator::BedDataIterator(BedData &inBedData, size_t start,size_t stop)
    :BedDataIterator()
{
    loadData(inBedData,start,stop);
}

bool BedDataIterator::loadData(BedData &inBedData, size_t start,size_t stop)
{
    if(!inBedData.isValid) return false;
    if(!stop) stop = inBedData.nMarker;
    if(start>=stop) return false;
    size_t nByte=0;
    nSample = inBedData.nSample;
    nByte=(nSample+3)/4;
    readNum = stop-start;
    data = (inBedData.data + start*nByte);
    readPoint = 0;
    isValid = true;
    return true;
}

bool BedDataIterator::read(short *out)
{
    if (!isValid || !data || !out) {
        return false;
    }
    size_t nByte = (nSample + 3) / 4, i = 0, isam = 0, j = 0;
    unsigned short tByte = 0, tc = 3;
    char* pByte = nullptr;
    if (readPoint >= readNum) {
        return false;
    }
    pByte = new char [nByte];
    charCopy(pByte, &data[readPoint * nByte], nByte);
    for (i = 0; i < nByte; ++i)
    {
        tByte = (unsigned short)(pByte[i]);
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

bool BedDataIterator::read2(double *out)
{
    if (!isValid || !data || !out) {
        return false;
    }
    size_t nByte = (nSample + 3) / 4, i = 0, isam = 0, j = 0;
    unsigned short tByte = 0, tc = 3;
    char* pByte = nullptr;
    if (readPoint >= readNum) {
        return false;
    }
    pByte = new char [nByte];
    charCopy(pByte, &data[readPoint * nByte], nByte);
    for (i = 0; i < nByte; ++i)
    {
        tByte = (unsigned short)(pByte[i]);
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

PolyPedDataIterator::PolyPedDataIterator(PolyPedData &inPolyPedData, size_t start, size_t stop)
    :PolyPedDataIterator()
{
    loadData(inPolyPedData,start,stop);
}

bool PolyPedDataIterator::loadData(PolyPedData &inPolyPedData, size_t start, size_t stop)
{
    if(!inPolyPedData.isValid) return false;
    if(!stop) stop = inPolyPedData.nMarker;
    if(start>=stop) return false;
    nPloid = inPolyPedData.nPloid;
    nSample = inPolyPedData.nSample;
    readNum = stop-start;
    data = (inPolyPedData.data + start*nSample);
    readPoint = 0;
    isValid = true;
    return true;
}

bool PolyPedDataIterator::read(short *out)
{
    if (!isValid || !data || !out) {
        return false;
    }
    if (readPoint >= readNum) {
        return false;
    }
    for (size_t iSample = 0; iSample < nSample; ++iSample) {
        if (data[readPoint * nSample + iSample] != MML::UNASSIGNED) {
            out[iSample] = data[readPoint * nSample + iSample];
        } else {
            out[iSample] = MML::UNASSIGNED;
        }
    }
    return true;
}

bool PolyPedDataIterator::read2(double *out)
{
    if (!isValid || !data || !out) {
        return false;
    }
    if (readPoint >= readNum) {
        return false;
    }
    for (size_t iSample = 0; iSample < nSample; ++iSample) {
        if (data[readPoint * nSample + iSample] != MML::UNASSIGNED) {
            out[iSample] = data[readPoint * nSample + iSample] / double(nPloid);
        } else {
            out[iSample] = MML::DATA_NA;
        }
    }
    return true;
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

BIMDataIterator::BIMDataIterator(BIMData &inBIMData, size_t start, size_t stop)
    :BIMDataIterator()
{
    loadData(inBIMData,start,stop);
}

bool BIMDataIterator::loadData(BIMData &inBIMData, size_t start, size_t stop)
{
    if(!inBIMData.isValid) return false;
    if(!stop) stop = inBIMData.nMarker;
    if(start>=stop) return false;
    readNum = stop-start;
    data = (inBIMData.data + start);
    readPoint = 0;
    isValid = true;
    return true;
}

BIMNode *BIMDataIterator::read()
{
    if(!isValid||!data||readPoint >= readNum) return nullptr;
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

BIMLogPDataIterator::BIMLogPDataIterator(BIMLogPData &inBIMLogPData, size_t start, size_t stop)
    :BIMLogPDataIterator()
{
    loadData(inBIMLogPData,start,stop);
}

bool BIMLogPDataIterator::loadData(BIMLogPData &inBIMLogPData, size_t start, size_t stop)
{
    if(!inBIMLogPData.isValid) return false;
    if(!stop) stop = inBIMLogPData.nMarker;
    if(start>=stop) return false;
    readNum = stop-start;
    data = (inBIMLogPData.data + start);
    readPoint = 0;
    isValid = true;
    return true;
}

BIMLogPNode *BIMLogPDataIterator::read()
{
    if(!isValid||!data||readPoint >= readNum) return nullptr;
    return data+readPoint;
}

void BIMLogPDataIterator::operator=(const BIMLogPDataIterator &object)
{
    copy(object);
    data = object.data;
}

PedDataIteratorSet::PedDataIteratorSet()
{
    info = -1;
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
    info = 0;
}

void PedDataIteratorSet::setPPedIter(const PolyPedDataIterator &iter)
{
    ppedIter = iter;
    info = 1;
}

bool PedDataIteratorSet::read2(double *out)
{
    switch (info) {
    case 0:
        return bedIter.read2(out);
    case 1:
        return ppedIter.read2(out);
    default:
        return false;
    }
    return false;
}

bool PedDataIteratorSet::next()
{
    switch (info) {
    case 0:
        return bedIter.next();
    case 1:
        return ppedIter.next();
    default:
        return false;
    }
    return false;
}

bool PedDataIteratorSet::isEnd()
{
    switch (info) {
    case 0:
        return bedIter.isEnd();
    case 1:
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

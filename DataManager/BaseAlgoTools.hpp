#ifndef DATAMANAGER_BASE_ALGO_TOOLS_HPP
#define DATAMANAGER_BASE_ALGO_TOOLS_HPP

#include <vector>

namespace MML
{

template<typename IDXCLASS, typename VALCLASS>
class VecHeapSort
{
private:
    std::vector<IDXCLASS> *idx;
    std::vector<VALCLASS> *val;
    void once(IDXCLASS id, IDXCLASS lim)
    {
        IDXCLASS tmpId = (*idx)[id];
        while (true) {
            IDXCLASS lc = id * 2 + 1;
            IDXCLASS rc = lc + 1;
            IDXCLASS tc = lc;
            if (lc >= lim) {
                break;
            }
            if (rc < lim && (*val)[(*idx)[tc]] < (*val)[(*idx)[rc]]) {
                tc = rc;
            }
            if ((*val)[tmpId] < (*val)[(*idx)[tc]]) {
                (*idx)[id] = (*idx)[tc];
                id = tc;
            } else {
                break;
            }
        }
        (*idx)[id] = tmpId;
    }

public:
    VecHeapSort()
    {
        idx = nullptr;
        val = nullptr;
    }

    VecHeapSort(std::vector<IDXCLASS> *inIdxVec, std::vector<VALCLASS> *inValVec)
        : VecHeapSort()
    {
        set(inIdxVec, inValVec);
    }

    void set(std::vector<IDXCLASS> *inIdxVec, std::vector<VALCLASS> *inValVec)
    {
        idx = inIdxVec;
        val = inValVec;
    }

    void sort()
    {
        if (!(idx && val)) {
            return;
        }
        IDXCLASS i, j, n = idx->size();
        IDXCLASS tmpId;
        for (i = n / 2 - 1; i >= 0; --i) {
            once(i, n - 1);
        }

        tmpId = (*idx)[n - 1];
        (*idx)[n - 1] = (*idx)[0];
        (*idx)[0] = tmpId;
        for (i = n - 2; i > 0; --i) {
            once(0, i);
            tmpId = (*idx)[i];
            (*idx)[i] = (*idx)[0];
            (*idx)[0] = tmpId;
        }
    }

};

};

#endif

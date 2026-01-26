#ifndef MML_STRUCTURE_HPP
#define MML_STRUCTURE_HPP

#include "DataManager/DataManager.hpp"

namespace MML {

class StructureParam
{
public:
    explicit StructureParam();
    uint64_t nBurnIn;
    uint64_t nRecord;
    uint64_t nPopulation;
    void setNBurnIn(uint64_t n);
    void setNRecord(uint64_t n);
    void setNPopulation(uint64_t n);
};

class Structure
{
public:
    uint64_t nMarker;
    uint64_t nPop;
    uint64_t nAllele;
    uint64_t nSample;
    uint64_t nPloid;
    Distribution dis;
    int16_t*  G;
    double* P;
    double* Q;
    int*    Z;
    double* lambda;
    double* alpha;
    double* sumQ;
    bool isRecord;
    std::default_random_engine generator;
    explicit Structure();
    Structure(uint64_t inNMarker,
              uint64_t inNPop,
              uint64_t inNAllele,
              uint64_t inNSample,
              uint64_t inNPloid  );
    ~Structure();
    bool setParameter(uint64_t inNMarker,
                      uint64_t inNPop,
                      uint64_t inNAllele,
                      uint64_t inNSample,
                      uint64_t inNPloid  );
    bool initialize();
    bool randomizeZ();
    bool updateP();
    bool updateQ();
    bool updateZ();
    int16_t* genotype(uint64_t iMarker,
                    uint64_t iSample,
                    uint64_t iPloid);
    double getSumQ(uint64_t iSample,uint64_t iPop);
    void setRecord(bool state);
    void clear();
    void onlyResult();
};

} // namespace MML

#endif // MML_STRUCTURE_HPP

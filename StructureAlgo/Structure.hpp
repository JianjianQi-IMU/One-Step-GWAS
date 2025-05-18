#ifndef MML_STRUCTURE_HPP
#define MML_STRUCTURE_HPP

#include "DataManager/DataManager.hpp"

namespace MML {

class StructureParam
{
public:
    explicit StructureParam();
    size_t nBurnIn;
    size_t nRecord;
    size_t nPopulation;
    void setNBurnIn(size_t n);
    void setNRecord(size_t n);
    void setNPopulation(size_t n);
};

class Structure
{
public:
    size_t nMarker;
    size_t nPop;
    size_t nAllele;
    size_t nSample;
    size_t nPloid;
    Distribution dis;
    short*  G;
    double* P;
    double* Q;
    int*    Z;
    double* lambda;
    double* alpha;
    double* sumQ;
    bool isRecord;
    std::default_random_engine generator;
    explicit Structure();
    Structure(size_t inNMarker,
              size_t inNPop,
              size_t inNAllele,
              size_t inNSample,
              size_t inNPloid  );
    ~Structure();
    bool setParameter(size_t inNMarker,
                      size_t inNPop,
                      size_t inNAllele,
                      size_t inNSample,
                      size_t inNPloid  );
    bool initialize();
    bool randomizeZ();
    bool updateP();
    bool updateQ();
    bool updateZ();
    short* genotype(size_t iMarker,
                    size_t iSample,
                    size_t iPloid);
    double getSumQ(size_t iSample,size_t iPop);
    void setRecord(bool state);
    void clear();
    void onlyResult();
};

} // namespace MML

#endif // MML_STRUCTURE_HPP

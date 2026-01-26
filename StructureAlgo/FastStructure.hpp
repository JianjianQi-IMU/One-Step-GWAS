#ifndef FASTSTRUCTURE_HPP
#define FASTSTRUCTURE_HPP

#include "DataManager/DataManager.hpp"

namespace MML {

enum FastStructurePrior
{
    FS_SIMPLE   = 0,
    FS_LOGISTIC = 1
};

class FastStructureParam
{
public:
    uint64_t nMarker;
    uint64_t nPop;
    uint64_t nAllele;
    uint64_t nSample;
    uint64_t nPloid;
    FastStructurePrior prior;
    explicit FastStructureParam();
};

class FastStructurePData
{
public:
    Mat PBeta;
    Mat PGamma;
    Mat PMu;
    Mat PLambda;
    Mat PVarBeta;
    Mat PVarGamma;
    Mat PZetaBeta;
    Mat PZetaGamma;
    explicit FastStructurePData();
    void initialize(const FastStructureParam& inParam);
};

class FastStructureQData
{
public:
    Mat Q;
    Mat QXi;
    Mat alpha;
    explicit FastStructureQData();
    void initialize(const FastStructureParam& inParam);
};

class FastStructure
{
private:
    uint64_t nMarker;
    uint64_t nPop;
    uint64_t nAllele;
    uint64_t nSample;
    uint64_t nPloid;

    FastStructurePrior prior;

    int16_t* G;
    Mat PBeta;
    Mat PGamma;
    Mat PMu;
    Mat PLambda;
    Mat PVarBeta;
    Mat PVarGamma;
    Mat PZetaBeta;
    Mat PZetaGamma;
    Mat Q;
    Mat QXi;
    Mat alpha;
    Distribution dis;
    std::default_random_engine generator;

    double* tmpVarBeta;
    double* tmpVarGamma;

    double tolThre;

    void updatePSimple();
    void updatePLogistic();

public:
    explicit FastStructure();
    ~FastStructure();
    void resetP();
    void resetQ();
    void initialize();
    void saveParam(FastStructureParam& outParam);
    void loadParam(const FastStructureParam& inParam);
    void savePData(FastStructurePData& outP);
    void saveQData(FastStructureQData& outQ);
    void loadPData(const FastStructurePData& inP);
    void loadQData(const FastStructureQData& inQ);
    void updateP();
    void updateP2();
    void updateQ();
    void updateQ2();
    void updatePHyper(bool noLambda);
    double marginalLikelihood();
    int16_t* genotype(uint64_t iMarker,
                    uint64_t iSample,
                    uint64_t iPloid);
    void batchInit(FastStructurePData& outP,
                   FastStructureQData& outQ);
    void getQ(MML::Mat &outQ);
    const int16_t* getG() const;
    void clear();

    static double expectGenotype(const FastStructurePData& P,
                                 const FastStructureQData& Q,
                                 const FastStructureParam& Para,
                                 uint64_t n,
                                 uint64_t l);
    static void calcuCV(const int16_t *G,
                        const FastStructurePData &P,
                        const FastStructureQData &Q,
                        const FastStructureParam &Para,
                        std::vector<double> &outMeanDeviance,
                        uint64_t nCV);
};

}

#endif // FASTSTRUCTURE_HPP

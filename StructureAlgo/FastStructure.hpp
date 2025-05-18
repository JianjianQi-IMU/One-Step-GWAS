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
    size_t nMarker;
    size_t nPop;
    size_t nAllele;
    size_t nSample;
    size_t nPloid;
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
    size_t nMarker;
    size_t nPop;
    size_t nAllele;
    size_t nSample;
    size_t nPloid;

    FastStructurePrior prior;

    short* G;
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
    short* genotype(size_t iMarker,
                    size_t iSample,
                    size_t iPloid);
    void batchInit(FastStructurePData& outP,
                   FastStructureQData& outQ);
    void getQ(MML::Mat &outQ);
    const short* getG() const;
    void clear();

    static double expectGenotype(const FastStructurePData& P,
                                 const FastStructureQData& Q,
                                 const FastStructureParam& Para,
                                 size_t n,
                                 size_t l);
    static void calcuCV(const short *G,
                        const FastStructurePData &P,
                        const FastStructureQData &Q,
                        const FastStructureParam &Para,
                        std::vector<double> &outMeanDeviance,
                        size_t nCV);
};

}

#endif // FASTSTRUCTURE_HPP

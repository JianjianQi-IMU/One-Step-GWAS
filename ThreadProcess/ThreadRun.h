#ifndef THREADRUN_H
#define THREADRUN_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <cmath>
#include <string>
#include <iomanip>
#include "FileDeal/FileIterator.hpp"
#include "EMMAXAlgo/EMMAX.hpp"
#include "GEMMAAlgo/GEMMA.hpp"
#include "PCAAlgo/FastPCA.hpp"
#include "StructureAlgo/Structure.hpp"
#include "KinshipCalcuAlgo/KinshipCalculator.hpp"
#include "StructureAlgo/FastStructure.hpp"
#include "BSAanalysis/BSAProject.hpp"
#include "BSAanalysis/BSAThreshold.hpp"
#include "FileDeal/FileDeal.hpp"
#include "FarmCPU/BlinkRaw.hpp"

#include <QDebug>

namespace  ThreadRun
{

enum AnalyzeClass
{
    GLM_GEMMA         = 1,
    MLM_GEMMA         = 2,
    MLM_EMMAX         = 3,
    PCA_PCA           = 4,
    PCA_FASTPCA       = 5,
    STR_STRUCTURE     = 6,
    STR_FASTSTRUCTURE = 7,
    KIN_KINSHIP       = 8,
    GLM_BLINK         = 9,
    BSA_NORMALBSA     = 10
};

class BaseThread
{
protected:
    BaseThread();
public:
    std::mutex mux;
    std::condition_variable conv;
    bool isValid;
    bool isThreadStart;
    std::atomic<bool> isPause;
    std::atomic<bool> isStop;
    std::atomic<bool> isFinished;
    AnalyzeClass info;
    std::atomic<long long> dealedNum;
    std::chrono::system_clock::time_point startTP;
    int nSubThread;
    std::atomic<std::atomic<bool>*> isPauseList;
    std::atomic<std::atomic<bool>*> isFinishedList;
    std::atomic<std::mutex*> muxList;
    bool infoOutput;
    long long getDealedNum();
    void pause();
    void stop();
    bool finished();
    void toContinue();
    void createFlagList();
    void checkFinishedList();
    void clearFlagList();
    std::string timePointToString(const std::chrono::system_clock::time_point& tp);
    virtual ~BaseThread()=0;
    virtual bool saveOutput(const char*)=0;
    virtual void run()=0;
    virtual bool makeThread()=0;
};

class EMMAXThread : public BaseThread
{
protected:
    EMMAXThread();
public:
    MML::Phenotype phe;
    FD::GWASProject* project;
    EMMAX::Betas betas;
    MML::Distribution dis;
    explicit EMMAXThread(FD::GWASProject* inProject);
//    EMMAXThread(const char* nameBed,
//                const char* nameBim,
//                const char* namePhe,
//                const char* nameCov,
//                const char* nameKin);
    ~EMMAXThread();
    bool saveOutput(const char* outFile) override;
    void run() override;
    bool makeThread() override;
};

class GEMMAThread : public BaseThread
{
protected:
    GEMMAThread();
public:
    FD::GWASProject *project;
    MML::Phenotype phe;
    GEMMA::MLM mlm;
    MML::Distribution dis;
    explicit GEMMAThread(FD::GWASProject* inProject);
//    GEMMAThread(const char* nameBed,
//                const char* nameBim,
//                const char* namePhe,
//                const char* nameCov,
//                const char* nameKin);
    ~GEMMAThread();
    bool saveOutput(const char* outFile) override;
    void run() override;
    bool makeThread() override;
};

class GLMThread : public BaseThread
{
protected:
    GLMThread();
public:
    FD::GWASProject *project;
    MML::Phenotype phe;
    GEMMA::GLM glm;
    MML::Distribution dis;
    explicit GLMThread(FD::GWASProject* inProject);
//    GLMThread(const char* nameBed,
//              const char* nameBim,
//              const char* namePhe,
//              const char* nameCov);
    ~GLMThread();
    bool saveOutput(const char* outFile) override;
    void run() override;
    bool makeThread() override;
};

class GLMBlinkThread : public BaseThread
{
protected:
    GLMBlinkThread();
public:
    FD::GWASProject *project;
    FD::PedDataIteratorSet dataForRun;
    FD::BIMLogPDataIterator bimForRun;
    MML::Phenotype phe;
    MML::Covariates cov;
    FARMCPU::BlinkParam param;
    MML::Distribution dis;
    FARMCPU::BlinkRaw blinkRaw;
    explicit GLMBlinkThread(FD::GWASProject* inProject);
    ~GLMBlinkThread();
    bool saveOutput(const char* outFile) override;
    void run() override;
    bool makeThread() override;
};

class PCAThread : public BaseThread
{
protected:
    PCAThread();
public:
    FD::GWASProject *project;
    MML::FastPCA pca;
    MML::Mat CovMat;
    MML::Mat PCs;
    MML::Mat EVal;
    uint64_t PCn;
    double sumEVal;
    explicit PCAThread(FD::GWASProject* inProject);
    ~PCAThread();
    void setPCn(uint64_t n);
    bool saveOutput(const char* outFile) override;
    void run() override;
    bool makeThread() override;
    static uint64_t READSCALE;
};


class FastPCAThread : public PCAThread
{
protected:
    FastPCAThread();
public:
    MML::Mat Y;
    MML::Mat S;
    explicit FastPCAThread(FD::GWASProject* inProject);
    ~FastPCAThread();
    void setPCn(uint64_t n);
    bool saveOutput(const char* outFile) override;
    void run() override;
    bool makeThread() override;
    static uint64_t READSCALE;
};

class StructureThread : public BaseThread
{
protected:
    StructureThread();
public:
    FD::GWASProject *project;
    MML::Structure structure;
    uint64_t nBurnIn;
    uint64_t nRecord;
    uint64_t nPopulation;
    MML::Mat out;
    explicit StructureThread(FD::GWASProject* inProject);
    ~StructureThread();
    void setRep(uint64_t inNBurnIn,uint64_t inNRecord);
    void setNPopulation(uint64_t inNPop);
    void setPara(const MML::StructureParam& inPara);
    bool saveOutput(const char* outFile) override;
    void run() override;
    bool makeThread() override;
};

class FastStructureThread : public BaseThread
{
protected:
    FastStructureThread();
public:
    FD::GWASProject *project;
    MML::FastStructureParam param;
    MML::FastStructure structure;
    MML::FastStructurePrior prior;
    uint64_t nPopulation;
    uint64_t nCV;
    MML::Mat out;
    std::vector<double> meanDev;
    explicit FastStructureThread(FD::GWASProject* inProject);
    ~FastStructureThread();
    void setNPopulation(uint64_t inNPop);
    bool saveOutput(const char* outFile) override;
    void run() override;
    bool makeThread() override;
};

class KinshipThread : public BaseThread
{
protected:
    KinshipThread();
public:
    FD::GWASProject *project;
    MML::KinshipCalculator *kinCalculator;
    MML::Mat outKin;
    explicit KinshipThread(FD::GWASProject* inProject,
                           MML::KinshipMode inMode = MML::REL_TYPE);
    ~KinshipThread();
    void clear();
    void setMode(MML::KinshipMode inMode);
    bool saveOutput(const char* outFile) override;
    void run() override;
    bool makeThread() override;
    static uint64_t READSCALE;
};

class NormalBSAThread : public BaseThread
{
protected:
    NormalBSAThread();
public:
    FD::BSAProject *project;
    BSAThresholdCalculatorF2 calculatorF2;
    FD::BSAInterVal* inter;
    uint64_t nInter;
    double* markerThre;
    explicit NormalBSAThread(FD::BSAProject* inProject);
    ~NormalBSAThread();
    bool saveOutput(const char* outFile) override;
    void run() override;
    bool makeThread() override;
    static uint64_t READSCALE;
};

void deleteThreadRun(const BaseThread* p);
bool saveOutput(BaseThread* p,const char* outFile);
FD::BIMLogPData* getBIMInfo(BaseThread* p);

};



#endif // THREADRUN_H

#include "ThreadRun.h"

ThreadRun::BaseThread::BaseThread()
{
    isValid=false;
    isFinished.store(false);
    isPause.store(false);
    isStop.store(false);
    isThreadStart=false;
    dealedNum.store(0);

    isPauseList = nullptr;
    isFinishedList = nullptr;
    muxList = nullptr;
    nSubThread = 0;
}

long long ThreadRun::BaseThread::getDealedNum()
{
    return dealedNum.load();
}

void ThreadRun::BaseThread::pause()
{
    if(!nSubThread){
        isPause.store(true);
    }
    else{
        for(int i=0;i<nSubThread;++i){
            isPauseList[i].store(true);
        }
    }
}

void ThreadRun::BaseThread::stop()
{
    if(!nSubThread){
        isStop.store(true);
        toContinue();
        while(true)
        {
            if(isFinished.load()) break;
        }
        isFinished.store(false);
    }
    else{
        isStop.store(true);
        toContinue();
        while(true)
        {
            checkFinishedList();
            if(isFinished.load()) break;
        }
        for(int i=0;i<nSubThread;++i){
            isFinishedList[i].store(false);
        }
    }
}

bool ThreadRun::BaseThread::finished()
{
    return isFinished.load();
}

void ThreadRun::BaseThread::toContinue()
{
    if(!nSubThread){
        isPause.store(false);
    }
    else{
        for(int i=0;i<nSubThread;++i){
            isPauseList[i].store(false);
        }
    }
    conv.notify_all();
}

void ThreadRun::BaseThread::createFlagList()
{
    if(isPauseList) delete [] isPauseList.load();
    if(isFinishedList) delete [] isFinishedList.load();
    if(muxList) delete [] muxList.load();
    isPauseList = new std::atomic<bool>[nSubThread];
    isFinishedList = new std::atomic<bool>[nSubThread];
    muxList = new std::mutex[nSubThread];
    for(int i=0;i<nSubThread;++i){
        isPauseList[i].store(false);
        isFinishedList[i].store(false);
    }
}

void ThreadRun::BaseThread::checkFinishedList()
{
    if(!nSubThread) return;
    bool f = true;
    for(int i=0;i<nSubThread;++i){
        if(!isFinishedList[i].load()){
            f = false;
            break;
        }
    }
    isFinished.store(f);
}

void ThreadRun::BaseThread::clearFlagList()
{
    if(isPauseList) delete [] isPauseList.load();
    if(isFinishedList) delete [] isFinishedList.load();
    if(muxList) delete [] muxList.load();
    isPauseList = nullptr;
    isFinishedList = nullptr;
    muxList = nullptr;
    nSubThread = 0;
}

ThreadRun::BaseThread::~BaseThread()
{
    clearFlagList();
}

ThreadRun::EMMAXThread::EMMAXThread()
    :BaseThread()
{
    info=MLM_EMMAX;
}

ThreadRun::EMMAXThread::EMMAXThread(FD::GWASProject *inProject)
    :EMMAXThread()
{
    if(!inProject) return;
    project=inProject;
    if(project->isValid&&project->isKinValid) isValid=true;
}
//ThreadRun::EMMAXThread::EMMAXThread(const char* nameBed, const char* nameBim, const char* namePhe, const char* nameCov, const char* nameKin)
//    :BaseThread()
//{
//    FD::ReadState theState=project.read(nameBed,nameBim,namePhe,nameCov,nameKin);
//    if(theState==FD::FD_ENABLED) isValid=true;
//    state=theState;
//    info=MLM_EMMAX;
//}

ThreadRun::EMMAXThread::~EMMAXThread()
{
    if(isThreadStart) stop();
}

bool ThreadRun::EMMAXThread::saveOutput(const char *outFile)
{
    // qDebug()<< "x";
    if(isValid&&isThreadStart&&isFinished.load())
    {
        project->saveOutput(outFile);
        return true;
    }
    return false;
}

void ThreadRun::EMMAXThread::run()
{
    /* removed */
}

bool ThreadRun::EMMAXThread::makeThread()
{
    /* removed */
    return false;
}

ThreadRun::GEMMAThread::GEMMAThread()
    :BaseThread()
{
    info=MLM_GEMMA;
}

ThreadRun::GEMMAThread::GEMMAThread(FD::GWASProject *inProject)
    :GEMMAThread()
{
    if(!inProject) return;
    project=inProject;
    if(project->isValid&&project->isKinValid) isValid=true;
}

//ThreadRun::GEMMAThread::GEMMAThread(const char *nameBed, const char *nameBim, const char *namePhe, const char *nameCov, const char *nameKin)
//    :BaseThread()
//{
//    FD::ReadState theState=project.read(nameBed,nameBim,namePhe,nameCov,nameKin);
//    if(theState==FD::FD_ENABLED) isValid=true;
//    state=theState;
//    info=MLM_GEMMA;
//}

ThreadRun::GEMMAThread::~GEMMAThread()
{
    if(isThreadStart) stop();
}

bool ThreadRun::GEMMAThread::saveOutput(const char *outFile)
{
    if(isValid&&isThreadStart&&isFinished.load())
    {
        project->saveOutput(outFile);
        return true;
    }
    return false;
}

void ThreadRun::GEMMAThread::run()
{
    /* removed */
}

bool ThreadRun::GEMMAThread::makeThread()
{
    /* removed */
    return true;
}

ThreadRun::GLMThread::GLMThread()
    :BaseThread()
{
    info=GLM_GEMMA;
}

ThreadRun::GLMThread::GLMThread(FD::GWASProject *inProject)
    :GLMThread()
{
    if(!inProject) return;
    project=inProject;
    if(project->isValid) isValid=true;

}

ThreadRun::GLMThread::~GLMThread()
{

}

bool ThreadRun::GLMThread::saveOutput(const char *outFile)
{
    if(isValid&&isThreadStart&&isFinished.load())
    {
        project->saveOutput(outFile);
        return true;
    }
    return false;
}

void ThreadRun::GLMThread::run()
{
    /* removed */
}

bool ThreadRun::GLMThread::makeThread()
{
    /* removed */
    return true;
}

size_t ThreadRun::PCAThread::READSCALE = 200;

ThreadRun::PCAThread::PCAThread()
    :BaseThread()
{
    info=PCA_PCA;
}

ThreadRun::PCAThread::PCAThread(FD::GWASProject *inProject)
    :PCAThread()
{
    if(!inProject) return;
    project=inProject;
    if(project->isValid) isValid=true;
    PCn=20;
}

ThreadRun::PCAThread::~PCAThread()
{

}

void ThreadRun::PCAThread::setPCn(size_t n)
{
    PCn=n;
}

bool ThreadRun::PCAThread::saveOutput(const char *outFile)
{
    std::fstream oF(outFile,std::ios::out);
    char outLine[FD::MAXLINELEN];
    size_t i=0,j=0,iByte=0;
    for(i=0;i<PCs.getNRow();++i)
    {
        iByte=0;
        for(j=0;j<PCn;++j){
            iByte+=sprintf(outLine+iByte,"%f\t",PCs(i,j));
        }
        *(outLine+iByte-1)='\n';
        oF << outLine;
    }
    oF << std::flush;
    oF.close();
    return true;
}

void ThreadRun::PCAThread::run()
{

    /* removed */
}

bool ThreadRun::PCAThread::makeThread()
{
    /* removed */
    return true;
}

size_t ThreadRun::FastPCAThread::READSCALE = 200;

ThreadRun::FastPCAThread::FastPCAThread()
    :PCAThread()
{
    info=PCA_FASTPCA;
}

ThreadRun::FastPCAThread::FastPCAThread(FD::GWASProject *inProject)
    :FastPCAThread()
{
    if(!inProject) return;
    project=inProject;
    if(project->isValid) isValid=true;
    PCn=10;
}

ThreadRun::FastPCAThread::~FastPCAThread()
{

}

void ThreadRun::FastPCAThread::setPCn(size_t n)
{
    PCn=n;
}

bool ThreadRun::FastPCAThread::saveOutput(const char *outFile)
{
    std::fstream oF(outFile,std::ios::out);
    char outLine[FD::MAXLINELEN];
    size_t i=0,j=0,iByte=0;
    for(i=0;i<PCs.getNRow();++i)
    {
        iByte=0;
        for(j=0;j<PCn;++j){
            iByte+=sprintf(outLine+iByte,"%f\t",PCs(i,j));
        }
        *(outLine+iByte-1)='\n';
        oF << outLine;
    }
    oF << std::flush;
    oF.close();
    return true;
}

void ThreadRun::FastPCAThread::run()
{
    /* removed */
}

bool ThreadRun::FastPCAThread::makeThread()
{
    /* removed */
    return true;
}

ThreadRun::StructureThread::StructureThread()
    :BaseThread()
{
    nBurnIn=40000;
    nRecord=15000;
//    nBurnIn=500;
//    nRecord=100;
    nPopulation=4;
    info=STR_STRUCTURE;
}

ThreadRun::StructureThread::StructureThread(FD::GWASProject *inProject)
    :StructureThread()
{
    if(!inProject) return;
    project=inProject;
    if(project->isValid) isValid=true;
}

ThreadRun::StructureThread::~StructureThread()
{

}

void ThreadRun::StructureThread::setRep(size_t inNBurnIn, size_t inNRecord)
{
    nBurnIn=inNBurnIn;
    nRecord=inNRecord;
}

void ThreadRun::StructureThread::setNPopulation(size_t inNPop)
{
    if(!inNPop){
        nPopulation=inNPop;
    }
}

void ThreadRun::StructureThread::setPara(const MML::StructureParam &inPara)
{
    nBurnIn=inPara.nBurnIn;
    nRecord=inPara.nRecord;
    nPopulation=inPara.nPopulation;
}

bool ThreadRun::StructureThread::saveOutput(const char *outFile)
{
    std::fstream oF(outFile,std::ios::out);
    char outLine[FD::MAXLINELEN];
    size_t i=0,j=0,iByte=0;
    size_t nSample=project->phe.getNRow();
    for(i=0;i<nSample;++i)
    {
        iByte=0;
        for(j=0;j<nPopulation;++j){
            iByte+=sprintf(outLine+iByte,"%f\t",out(i,j));
        }
        *(outLine+iByte-1)='\n';
        oF << outLine;
    }
    oF << std::flush;
    oF.close();
    return true;
}

void ThreadRun::StructureThread::run()
{
    /* removed */
}

bool ThreadRun::StructureThread::makeThread()
{
    /* removed */
    return true;
}

ThreadRun::FastStructureThread::FastStructureThread()
{
    info=STR_FASTSTRUCTURE;
    prior=MML::FS_SIMPLE;
    nPopulation = 3;
    nCV = 1;
}

ThreadRun::FastStructureThread::FastStructureThread(FD::GWASProject *inProject)
    :FastStructureThread()
{
    if(!inProject) return;
    project=inProject;
    if(project->isValid) isValid=true;
}

ThreadRun::FastStructureThread::~FastStructureThread()
{

}

void ThreadRun::FastStructureThread::setNPopulation(size_t inNPop)
{
    nPopulation = inNPop;
}

bool ThreadRun::FastStructureThread::saveOutput(const char *outFile)
{
    std::fstream oF(outFile,std::ios::out);
    char outLine[FD::MAXLINELEN];
    size_t i=0,j=0,iByte=0;
    size_t nSample=project->phe.getNRow();
    for(i=0;i<nSample;++i)
    {
        iByte=0;
        for(j=0;j<param.nPop;++j){
            iByte+=sprintf(outLine+iByte,"%f\t",out(i,j));
        }
        *(outLine+iByte-1)='\n';
        oF << outLine;
    }
    oF << std::flush;
    oF.close();
    return true;
}

void ThreadRun::FastStructureThread::run()
{
    /* removed */
}

bool ThreadRun::FastStructureThread::makeThread()
{
    /* removed */
    return true;
}

size_t ThreadRun::KinshipThread::READSCALE = 200;

ThreadRun::KinshipThread::KinshipThread()
    :BaseThread()
{
    info=KIN_KINSHIP;
    kinCalculator = nullptr;
}

ThreadRun::KinshipThread::KinshipThread(FD::GWASProject *inProject, MML::KinshipMode inMode)
    :KinshipThread()
{
    if(!inProject) {
        return;
    }
    project = inProject;
    if (inMode == MML::IBS_TYPE) {
        kinCalculator = new (std::nothrow) MML::KinshipIBSMatCalculator;
    } else {
        kinCalculator = new (std::nothrow) MML::KinshipCalculator;
    }
    if (project->isValid && kinCalculator != nullptr) {
        isValid = true;
    }
    kinCalculator->setNSamples(inProject->phe.getNRow());
    kinCalculator->setMode(inMode);
}

ThreadRun::KinshipThread::~KinshipThread()
{
    clear();
}

void ThreadRun::KinshipThread::clear()
{
    if (kinCalculator != nullptr) {
        delete kinCalculator;
        kinCalculator = nullptr;
    }
}

void ThreadRun::KinshipThread::setMode(MML::KinshipMode inMode)
{
    if ((kinCalculator != nullptr && kinCalculator->getMode() != inMode) ||
        kinCalculator == nullptr) {
        if (kinCalculator != nullptr) {
            delete kinCalculator;
            kinCalculator = nullptr;
        }
        if (inMode == MML::IBS_TYPE) {
            kinCalculator = new (std::nothrow) MML::KinshipIBSMatCalculator;
        } else {
            kinCalculator = new (std::nothrow) MML::KinshipCalculator;
        }
        if (kinCalculator == nullptr) {
            return;
        }
    }
    kinCalculator->setMode(inMode);
}

bool ThreadRun::KinshipThread::saveOutput(const char *outFile)
{
    std::fstream oF(outFile,std::ios::out);
    char outLine[FD::MAXLINELEN];
    size_t i=0,j=0,iByte=0;
    size_t nSample=project->phe.getNRow();
    for(i=0;i<nSample;++i)
    {
        iByte=0;
        for(j=0;j<nSample;++j){
            iByte+=sprintf(outLine+iByte,"%f\t",outKin(i,j));
        }
        *(outLine+iByte-1)='\n';
        oF << outLine;
    }
    oF << std::flush;
    oF.close();
    return true;
}

void ThreadRun::KinshipThread::run()
{
    /* removed */
}

bool ThreadRun::KinshipThread::makeThread()
{
    /* removed */
    return true;
}

ThreadRun::NormalBSAThread::NormalBSAThread()
    :BaseThread()
{
    inter = nullptr;
    markerThre = nullptr;
    project = nullptr;
    info = BSA_NORMALBSA;
}

ThreadRun::NormalBSAThread::NormalBSAThread(FD::BSAProject *inProject)
    :NormalBSAThread()
{
    if(!inProject) return;
    project=inProject;
    calculatorF2.setPara(project->getMeanSamples(),project->getMaxDepth(),project->getPValue());
}

ThreadRun::NormalBSAThread::~NormalBSAThread()
{
    if(inter) delete [] inter;
    if(markerThre) delete [] markerThre;
}

bool ThreadRun::NormalBSAThread::saveOutput(const char *outFile)
{
    std::fstream oF(outFile,std::ios::out);
    size_t i;
    oF << std::setprecision(6);
    oF << "#chr\tstart\tstop\tn\t";
    if(!(project->poolName.empty())){
        oF << project->poolName[0]+'\t';
        oF << project->poolName[1];
    }
    else{
        oF << "pool1\tpool2";
    }
    oF << "\tdeltaIndex\tthreshold\n";
    for(i=0;i<nInter;++i)
    {
        oF << project->chrName[inter[i].idChr];
        oF << '\t' << inter[i].start;
        oF << '\t' << inter[i].stop;
        oF << '\t' << inter[i].n;
        if(!(inter[i].n)||
           inter[i].indexH==MML::DATA_NA||
           inter[i].indexL==MML::DATA_NA||
           inter[i].val==MML::DATA_NA||
           inter[i].thre==MML::DATA_NA){
            oF << "\tNA\tNA\tNA\tNA\n";
        }
        else{
            oF << '\t' << inter[i].indexH;
            oF << '\t' << inter[i].indexL;
            oF << '\t' << inter[i].val;
            oF << '\t' << inter[i].thre << '\n';
        }
    }
    oF << std::flush;
    oF.close();
    return true;
}

void ThreadRun::NormalBSAThread::run()
{
    /* removed */
}

bool ThreadRun::NormalBSAThread::makeThread()
{
    /* removed */
    return true;
}

void ThreadRun::deleteThreadRun(const BaseThread *p)
{
    if(!p) return ;
    switch (p->info) {
    case GLM_GEMMA:
        delete dynamic_cast<const GLMThread*>(p);
        break;
    case MLM_GEMMA:
        delete dynamic_cast<const GEMMAThread*>(p);
        break;
    case MLM_EMMAX:
        delete dynamic_cast<const EMMAXThread*>(p);
        break;
    case PCA_PCA:
        delete dynamic_cast<const PCAThread*>(p);
        break;
    case PCA_FASTPCA:
        delete dynamic_cast<const FastPCAThread*>(p);
        break;
    case STR_STRUCTURE:
        delete dynamic_cast<const StructureThread*>(p);
        break;
    case STR_FASTSTRUCTURE:
        delete dynamic_cast<const FastStructureThread*>(p);
        break;
    case KIN_KINSHIP:
        delete dynamic_cast<const KinshipThread*>(p);
        break;
    case BSA_NORMALBSA:
        delete dynamic_cast<const NormalBSAThread*>(p);
        break;
    default: break;
    }
}

bool ThreadRun::saveOutput(BaseThread *p,const char* outFile)
{
    if(!p) return false;
    switch (p->info) {
    case GLM_GEMMA:
        return (dynamic_cast<GLMThread*>(p))->saveOutput(outFile);
        break;
    case MLM_GEMMA:
        return (dynamic_cast<GEMMAThread*>(p))->saveOutput(outFile);
        break;
    case MLM_EMMAX:
        return (dynamic_cast<EMMAXThread*>(p))->saveOutput(outFile);
        break;
    case PCA_PCA:
        return (dynamic_cast<PCAThread*>(p))->saveOutput(outFile);
        break;
    case PCA_FASTPCA:
        return (dynamic_cast<FastPCAThread*>(p))->saveOutput(outFile);
        break;
    case STR_STRUCTURE:
        return (dynamic_cast<StructureThread*>(p))->saveOutput(outFile);
        break;
    case STR_FASTSTRUCTURE:
        return (dynamic_cast<FastStructureThread*>(p))->saveOutput(outFile);
        break;
    case KIN_KINSHIP:
        return (dynamic_cast<KinshipThread*>(p))->saveOutput(outFile);
        break;
    case BSA_NORMALBSA:
        return (dynamic_cast<NormalBSAThread*>(p))->saveOutput(outFile);
        break;
    default: break;
    }
    return false;
}

FD::BIMLogPData *ThreadRun::getBIMInfo(BaseThread *p)
{
    if(!p) return nullptr;
    switch (p->info) {
    case GLM_GEMMA:
        return &((dynamic_cast<GLMThread*>(p))->project->bim);
        break;
    case MLM_GEMMA:
        return &((dynamic_cast<GEMMAThread*>(p))->project->bim);
        break;
    case MLM_EMMAX:
        return &((dynamic_cast<EMMAXThread*>(p))->project->bim);
        break;
    case PCA_PCA:
        return &((dynamic_cast<PCAThread*>(p))->project->bim);
        break;
    case PCA_FASTPCA:
        return &((dynamic_cast<FastPCAThread*>(p))->project->bim);
        break;
    case STR_STRUCTURE:
        return &((dynamic_cast<StructureThread*>(p))->project->bim);
        break;
    case STR_FASTSTRUCTURE:
        return &((dynamic_cast<FastStructureThread*>(p))->project->bim);
        break;
    case KIN_KINSHIP:
        return &((dynamic_cast<KinshipThread*>(p))->project->bim);
        break;
    default: break;
    }
    return nullptr;
}


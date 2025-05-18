#include "QThreadRun.hpp"

#include <QDebug>

namespace ThreadRun
{

QEMMAXThread::QEMMAXThread(QObject *parent) : QObject(parent)
{

}

QEMMAXThread::QEMMAXThread(FD::GWASProject *inProject, QObject *parent)
    :EMMAXThread(inProject),QObject(parent)
{

}

void QEMMAXThread::run()
{
    MML::Mat tMarkers,fMarkers;
    tMarkers.resize(project->phe.getNRow(),1);
    tMarkers.toColVec();
    long long i=0,MarkersNum=project->bim.nMarker;
    double betax0,Tstat,df=betas.filterN-betas.factorN-1,tP;
    for(;i<MarkersNum;++i)
    {
        if(i%16==0)
        {
            dealedNum.store(i);
            std::unique_lock<std::mutex> ul(mux);
            conv.wait(ul,[this]{
                return !isPause.load();
            });
            if(isStop.load()) break;
        }
        if(project->isPolyploid) project->pData.read2(tMarkers.data);
        else project->bed.read2(tMarkers.data);
        phe.filterMarkers(tMarkers,fMarkers);
        if(betas.calculateTstat(fMarkers,betax0,Tstat)){
            tP=dis.tPvalue2(Tstat,df);
            (project->bim.data)[i].PValue=tP;
            (project->bim.data)[i].statValue=Tstat;
            (project->bim.data)[i].betax0=betax0;
            (project->bim.data)[i].log10PValue=-std::log10(tP);
        }
        else{
            (project->bim.data)[i].PValue=1;
            (project->bim.data)[i].statValue=MML::DATA_NA;
            (project->bim.data)[i].betax0=MML::DATA_NA;
            (project->bim.data)[i].log10PValue=0;
        }
        if(isStop.load()) break;
    }
//    qDebug() << dealedNum.load();
    isFinished.store(true);
//    logInfoMux.lock();
    if(!isStop.load()) project->logInfo+="Done ";
//    logInfoMux.unlock();
    const auto theTP = std::chrono::system_clock::now();
    const auto theDura = std::chrono::duration_cast<std::chrono::seconds>(theTP-startTP);
    const auto hrs = std::chrono::duration_cast<std::chrono::hours>(theDura);
    const auto mins = std::chrono::duration_cast<std::chrono::minutes>(theDura - hrs);
    const auto secs = std::chrono::duration_cast<std::chrono::seconds>(theDura - hrs - mins);
    project->logInfo+='['+std::to_string(hrs.count())+" hours:"+std::to_string(mins.count())+" mins:"+std::to_string(secs.count())+" secs]\n";
    //    emit logUpdate();
    if(!isStop.load()) emit taskFinished();
}

bool QEMMAXThread::makeThread()
{
    if((!isValid)||(!project->isKinValid)) return false;
    clearFlagList();
    startTP = std::chrono::system_clock::now();
    phe.read(project->phe.getNRow(),project->phe.data);
    MML::Kinship kin(phe.rawN,project->kin.data,phe.getFilterIndex(),phe.getFilterNum());
//    logInfoMux.lock();
    project->logInfo+="### start EMMAX ###\n";
    project->logInfo+="samples number = "+std::to_string(project->phe.getNRow())+"\n";
    project->logInfo+="markers number = "+std::to_string(project->bim.nMarker)+"\n";
    if(!kin.eigen()){
        project->logInfo+="Error in eigenvalue decomposition with kinship matrix\n";
        emit logUpdate();
        return false;
    }
    MML::Covariates cov(phe.rawN,project->cov.getNCol(),project->cov.data,phe.getFilterIndex(),phe.getFilterNum());
    project->logInfo+="factor number  = "+std::to_string(project->cov.getNCol())+"\n";
    EMMAX::REML reml(kin,cov);
    double delta=0,dll=0,ll=0;
    if(!reml.getDelta(phe,delta,dll)){
        project->logInfo+="Error in solving REML\n";
        emit logUpdate();
        return false;
    }
    if(!betas.read(phe,kin,cov,delta)){
        project->logInfo+="Error in preparing betas initialization\n";
        emit logUpdate();
        return false;
    }
    reml.getLL(phe,delta,ll);
    project->logInfo+="delta          = "+std::to_string(delta)+"\n";
    project->logInfo+="REML           = "+std::to_string(ll)+"\n";
    emit logUpdate();
    isStop.store(false),isPause.store(false);
    std::thread theThread(&QEMMAXThread::run,this);
    theThread.detach();
    isThreadStart=true;
    return true;
}

void QEMMAXThread::runPart(int idThread,
                           FD::PedDataIteratorSet pedSet,
                           FD::BIMLogPDataIterator bimIter,
                           MML::Phenotype inPhe,
                           EMMAX::Betas inBetas,
                           MML::Distribution inDis)
{
    MML::Mat tMarkers,fMarkers;
    tMarkers.resize(pedSet.nSample,1);
    tMarkers.toColVec();
    long long i=0,ti=0,tSum;
    double betax0,Tstat,df=inBetas.filterN-inBetas.factorN-1,tP;

    std::mutex* pMux=muxList.load();
    for(;!pedSet.isEnd();pedSet.next(),bimIter.next(),++i)
    {
        if(i%16==0)
        {
            tSum=dealedNum.load();
            dealedNum.store(i-ti+tSum);
            ti=i;
            std::unique_lock<std::mutex> ul(pMux[idThread]);
            conv.wait(ul,[this,idThread](){
                return !((isPauseList.load())[idThread]).load();
            });
            if(isStop.load()) break;
        }
        pedSet.read2(tMarkers.data);
        inPhe.filterMarkers(tMarkers,fMarkers);
        if(inBetas.calculateTstat(fMarkers,betax0,Tstat)){
            tP=inDis.tPvalue2(Tstat,df);
            (bimIter.read())->PValue=tP;
            (bimIter.read())->statValue=Tstat;
            (bimIter.read())->betax0=betax0;
            (bimIter.read())->log10PValue=-std::log10(tP);
        }
        else{
            (bimIter.read())->PValue=1;
            (bimIter.read())->statValue=pedSet.dataNA;
            (bimIter.read())->betax0=pedSet.dataNA;
            (bimIter.read())->log10PValue=0;
        }
        if(isStop.load()) break;
    }
    isFinishedList[idThread].store(true);
    checkFinishedList();
    if(isFinished.load()){
        mux.lock();
        if(!infoOutput){
            infoOutput=true;
            if(!isStop.load()) project->logInfo+="Done ";
            const auto theTP = std::chrono::system_clock::now();
            const auto theDura = std::chrono::duration_cast<std::chrono::seconds>(theTP-startTP);
            const auto hrs = std::chrono::duration_cast<std::chrono::hours>(theDura);
            const auto mins = std::chrono::duration_cast<std::chrono::minutes>(theDura - hrs);
            const auto secs = std::chrono::duration_cast<std::chrono::seconds>(theDura - hrs - mins);
            project->logInfo+='['+std::to_string(hrs.count())+" hours:"+std::to_string(mins.count())+" mins:"+std::to_string(secs.count())+" secs]\n";
            if(!isStop.load()) emit taskFinished();

        }
        mux.unlock();
    }
}

bool QEMMAXThread::makeThreadMuti(int nThread)
{
    if(nThread<=0) return false;
    if(nThread==1||
       project->bim.nMarker<100||
       project->bim.nMarker/nThread<=1) return makeThread();
    if((!isValid)||(!project->isKinValid)) return false;
    clearFlagList();
    startTP = std::chrono::system_clock::now();
    phe.read(project->phe.getNRow(),project->phe.data);
    MML::Kinship kin(phe.rawN,project->kin.data,phe.getFilterIndex(),phe.getFilterNum());
    project->logInfo+="### start EMMAX ###\n";
    project->logInfo+="samples number = "+std::to_string(project->phe.getNRow())+"\n";
    project->logInfo+="markers number = "+std::to_string(project->bim.nMarker)+"\n";
    if(!kin.eigen()){
        project->logInfo+="Error in eigenvalue decomposition with kinship matrix\n";
        emit logUpdate();
        return false;
    }
    MML::Covariates cov(phe.rawN,project->cov.getNCol(),project->cov.data,phe.getFilterIndex(),phe.getFilterNum());
    project->logInfo+="factor number  = "+std::to_string(project->cov.getNCol())+"\n";
    EMMAX::REML reml(kin,cov);
    double delta=0,dll=0,ll=0;
    if(!reml.getDelta(phe,delta,dll)){
        project->logInfo+="Error in solving REML\n";
        emit logUpdate();
        return false;
    }
    if(!betas.read(phe,kin,cov,delta)){
        project->logInfo+="Error in preparing betas initialization\n";
        emit logUpdate();
        return false;
    }
    reml.getLL(phe,delta,ll);
    project->logInfo+="delta          = "+std::to_string(delta)+"\n";
    project->logInfo+="REML           = "+std::to_string(ll)+"\n";
    emit logUpdate();

    qDebug() << "pre-initialize";

    long long MarkersNum = project->bim.nMarker;
    long long scaleSize = MarkersNum/nThread;
    long long remainSize;
    if(MarkersNum%nThread){
        ++scaleSize;
        remainSize=MarkersNum-scaleSize*(nThread-1);
    }
    else remainSize=scaleSize;
    nSubThread = nThread;
    createFlagList();
    infoOutput=false;
    isStop=false;
    dealedNum.store(0);
    for(int iThread=0;iThread<nThread;++iThread){
        FD::PedDataIteratorSet tSet;
        FD::PolyPedDataIterator tPPed;
        FD::BedDataIterator tBed;
        FD::BIMLogPDataIterator tBim;
        tSet.nSample=project->phe.getNRow();
//        qDebug() << "1";
        if(iThread==nThread-1){
            tSet.nScale=remainSize;
            if(project->isPolyploid){
                tPPed.loadData(project->pData,iThread*scaleSize,MarkersNum);
                tSet.setPPedIter(tPPed);
            }
            else{
                tBed.loadData(project->bed,iThread*scaleSize,MarkersNum);
                tSet.setBedIter(tBed);
            }
            tBim.loadData(project->bim,iThread*scaleSize,MarkersNum);
        }
        else{
//            qDebug() << "2";
            tSet.nScale=scaleSize;
            if(project->isPolyploid){
                tPPed.loadData(project->pData,iThread*scaleSize,(iThread+1)*scaleSize);
                tSet.setPPedIter(tPPed);
            }
            else{
                tBed.loadData(project->bed,iThread*scaleSize,(iThread+1)*scaleSize);
                tSet.setBedIter(tBed);
            }
            tBim.loadData(project->bim,iThread*scaleSize,(iThread+1)*scaleSize);
        }
//        qDebug() << "3";
        std::thread theThread(&QEMMAXThread::runPart,this,
                              iThread,
                              tSet,
                              tBim,
                              phe,
                              betas,
                              dis);
        theThread.detach();
    }
//    qDebug() << "Thread Start";
    isThreadStart=true;
    return true;
}

QGEMMAThread::QGEMMAThread(QObject *parent) : QObject(parent)
{

}

QGEMMAThread::QGEMMAThread(FD::GWASProject *inProject, QObject *parent)
    :GEMMAThread(inProject),QObject(parent)
{

}

void QGEMMAThread::run()
{
    MML::Mat tMarkers,fMarkers,tUtx;
    tMarkers.resize(project->phe.getNRow(),1);
    tMarkers.toColVec();
    long long i=0,MarkersNum=project->bim.nMarker;
    double betax0,Fstat,df=mlm.filterN-mlm.factorN-1,tP;
    for(;i<MarkersNum;++i)
    {
        if(i%16==0)
        {
            dealedNum.store(i);
            std::unique_lock<std::mutex> ul(mux);
            conv.wait(ul,[this]{
                return !isPause.load();
            });
            if(isStop.load()) break;
        }
        if(project->isPolyploid) project->pData.read2(tMarkers.data);
        else project->bed.read2(tMarkers.data);
        phe.filterMarkers(tMarkers,fMarkers);
        MML::Mat::XVmul(mlm.Ut,fMarkers,tUtx);
        if(mlm.analyze(tUtx,betax0,Fstat))
        {
            tP=1-dis.fcdf(Fstat,1,df);
            (project->bim.data)[i].PValue=tP;
            (project->bim.data)[i].statValue=Fstat;
            (project->bim.data)[i].betax0=betax0;
            (project->bim.data)[i].log10PValue=-std::log10(tP);
        }
        else
        {
            (project->bim.data)[i].PValue=1;
            (project->bim.data)[i].statValue=MML::DATA_NA;
            (project->bim.data)[i].betax0=MML::DATA_NA;
            (project->bim.data)[i].log10PValue=0;
        }
        if(isStop.load()) break;
    }
//    qDebug() << dealedNum.load();
    isFinished.store(true);
    if(!isStop.load()) project->logInfo+="Done ";
//    logInfoMux.unlock();
    const auto theTP = std::chrono::system_clock::now();
    const auto theDura = std::chrono::duration_cast<std::chrono::seconds>(theTP-startTP);
    const auto hrs = std::chrono::duration_cast<std::chrono::hours>(theDura);
    const auto mins = std::chrono::duration_cast<std::chrono::minutes>(theDura - hrs);
    const auto secs = std::chrono::duration_cast<std::chrono::seconds>(theDura - hrs - mins);
    project->logInfo+='['+std::to_string(hrs.count())+" hours:"+std::to_string(mins.count())+" mins:"+std::to_string(secs.count())+" secs]\n";
    //    emit logUpdate();
    if(!isStop.load()) emit taskFinished();
}

bool QGEMMAThread::makeThread()
{
    if((!isValid)||(!project->isKinValid)) return false;
    startTP = std::chrono::system_clock::now();
    phe.read(project->phe.getNRow(),project->phe.data);
    MML::Kinship kin(phe.rawN,project->kin.data,phe.getFilterIndex(),phe.getFilterNum());
    project->logInfo+="### start GEMMA ###\n";
    project->logInfo+="samples number = "+std::to_string(project->phe.getNRow())+"\n";
    project->logInfo+="markers number = "+std::to_string(project->bim.nMarker)+"\n";
    MML::Covariates cov(phe.rawN,project->cov.getNCol(),project->cov.data,phe.getFilterIndex(),phe.getFilterNum());
    if(!mlm.read(phe,kin,cov)){
        project->logInfo+="Error in preparing MLM initialization\n";
        return false;
    }
    emit logUpdate();
    isStop.store(false),isPause.store(false);
    std::thread theThread(&QGEMMAThread::run,this);
    theThread.detach();
    isThreadStart=true;
    return true;
}

void QGEMMAThread::runPart(int idThread, FD::PedDataIteratorSet pedSet, FD::BIMLogPDataIterator bimIter, MML::Phenotype inPhe, GEMMA::MLM inMLM, MML::Distribution inDis)
{
    MML::Mat tMarkers,fMarkers,tUtx;
    tMarkers.resize(pedSet.nSample,1);
    tMarkers.toColVec();
    long long i=0,ti=0,tSum;
    double betax0,Fstat,df=inMLM.filterN-inMLM.factorN-1,tP;
    std::mutex* pMux=muxList.load();
    for(;!pedSet.isEnd();pedSet.next(),bimIter.next(),++i)
    {
        if(i%16==0)
        {
            tSum=dealedNum.load();
            dealedNum.store(i-ti+tSum);
            ti=i;
            std::unique_lock<std::mutex> ul(pMux[idThread]);
            conv.wait(ul,[this,idThread](){
                return !((isPauseList.load())[idThread]).load();
            });
            if(isStop.load()) break;
        }
        pedSet.read2(tMarkers.data);
        inPhe.filterMarkers(tMarkers,fMarkers);
        MML::Mat::XVmul(inMLM.Ut,fMarkers,tUtx);
        if(inMLM.analyze(tUtx,betax0,Fstat))
        {
            tP=1-dis.fcdf(Fstat,1,df);
            (bimIter.read())->PValue=tP;
            (bimIter.read())->statValue=Fstat;
            (bimIter.read())->betax0=betax0;
            (bimIter.read())->log10PValue=-std::log10(tP);
        }
        else
        {
            (bimIter.read())->PValue=1;
            (bimIter.read())->statValue=pedSet.dataNA;
            (bimIter.read())->betax0=pedSet.dataNA;
            (bimIter.read())->log10PValue=0;
        }
        if(isStop.load()) break;
    }
    isFinishedList[idThread].store(true);
    checkFinishedList();
    if(isFinished.load()){
        mux.lock();
        if(!infoOutput){
            infoOutput=true;
            if(!isStop.load()) project->logInfo+="Done ";
            const auto theTP = std::chrono::system_clock::now();
            const auto theDura = std::chrono::duration_cast<std::chrono::seconds>(theTP-startTP);
            const auto hrs = std::chrono::duration_cast<std::chrono::hours>(theDura);
            const auto mins = std::chrono::duration_cast<std::chrono::minutes>(theDura - hrs);
            const auto secs = std::chrono::duration_cast<std::chrono::seconds>(theDura - hrs - mins);
            project->logInfo+='['+std::to_string(hrs.count())+" hours:"+std::to_string(mins.count())+" mins:"+std::to_string(secs.count())+" secs]\n";
            if(!isStop.load()) emit taskFinished();

        }
        mux.unlock();
    }
}

bool QGEMMAThread::makeThreadMuti(int nThread)
{
    if(nThread<=0) return false;
    if(nThread==1||
       project->bim.nMarker<100||
       project->bim.nMarker/nThread<=1) return makeThread();
    if((!isValid)||(!project->isKinValid)) return false;
    clearFlagList();
    startTP = std::chrono::system_clock::now();
    phe.read(project->phe.getNRow(),project->phe.data);
    MML::Kinship kin(phe.rawN,project->kin.data,phe.getFilterIndex(),phe.getFilterNum());
    project->logInfo+="### start GEMMA ###\n";
    project->logInfo+="samples number = "+std::to_string(project->phe.getNRow())+"\n";
    project->logInfo+="samples number(filtered) = "+std::to_string(phe.getFilterNum())+"\n";
    project->logInfo+="markers number = "+std::to_string(project->bim.nMarker)+"\n";
    MML::Covariates cov(phe.rawN,project->cov.getNCol(),project->cov.data,phe.getFilterIndex(),phe.getFilterNum());
    if(!mlm.read(phe,kin,cov)){
        project->logInfo+="Error in preparing MLM initialization\n";
        return false;
    }
    emit logUpdate();

    long long MarkersNum = project->bim.nMarker;
    long long scaleSize = MarkersNum/nThread;
    long long remainSize;
    if(MarkersNum%nThread){
        ++scaleSize;
        remainSize=MarkersNum-scaleSize*(nThread-1);
    }
    else remainSize=scaleSize;
    nSubThread = nThread;
    createFlagList();
    infoOutput=false;
    isStop=false;
    dealedNum.store(0);
    for(int iThread=0;iThread<nThread;++iThread){
        FD::PedDataIteratorSet tSet;
        FD::PolyPedDataIterator tPPed;
        FD::BedDataIterator tBed;
        FD::BIMLogPDataIterator tBim;
        tSet.nSample=project->phe.getNRow();
//        qDebug() << "1";
        if(iThread==nThread-1){
            tSet.nScale=remainSize;
            if(project->isPolyploid){
                tPPed.loadData(project->pData,iThread*scaleSize,MarkersNum);
                tSet.setPPedIter(tPPed);
            }
            else{
                tBed.loadData(project->bed,iThread*scaleSize,MarkersNum);
                tSet.setBedIter(tBed);
            }
            tBim.loadData(project->bim,iThread*scaleSize,MarkersNum);
        }
        else{
//            qDebug() << "2";
            tSet.nScale=scaleSize;
            if(project->isPolyploid){
                tPPed.loadData(project->pData,iThread*scaleSize,(iThread+1)*scaleSize);
                tSet.setPPedIter(tPPed);
            }
            else{
                tBed.loadData(project->bed,iThread*scaleSize,(iThread+1)*scaleSize);
                tSet.setBedIter(tBed);
            }
            tBim.loadData(project->bim,iThread*scaleSize,(iThread+1)*scaleSize);
        }
//        qDebug() << "3";
        std::thread theThread(&QGEMMAThread::runPart,this,
                              iThread,
                              tSet,
                              tBim,
                              phe,
                              mlm,
                              dis);
        theThread.detach();
    }
//    qDebug() << "Thread Start";
    isThreadStart=true;
    return true;
}

QGLMThread::QGLMThread(QObject *parent) : QObject(parent)
{

}

QGLMThread::QGLMThread(FD::GWASProject *inProject, QObject *parent)
    :GLMThread(inProject),QObject(parent)
{

}

void QGLMThread::run()
{
    MML::Mat tMarkers,fMarkers;
    tMarkers.resize(project->phe.getNRow(),1);
    tMarkers.toColVec();
    long long i,MarkersNum=project->bim.nMarker;
    double betax0,Fstat,df=glm.filterN-glm.factorN-1,tP;
    for(i=0;i<MarkersNum;++i)
    {
        if(i%16==0)
        {
            dealedNum.store(i);
            std::unique_lock<std::mutex> ul(mux);
            conv.wait(ul,[this]{
                return !isPause.load();
            });
            if(isStop.load()) break;
        }
        if(project->isPolyploid) project->pData.read2(tMarkers.data);
        else project->bed.read2(tMarkers.data);
        if(!phe.filterMarkers(tMarkers,fMarkers)){
            (project->bim.data)[i].PValue=1;
            (project->bim.data)[i].betax0=MML::DATA_NA;
            (project->bim.data)[i].statValue=MML::DATA_NA;
            (project->bim.data)[i].log10PValue=0;
        }
        else if(glm.analyze(fMarkers,betax0,Fstat))
        {
            tP=dis.fcdfTri(Fstat,1,df);
            (project->bim.data)[i].PValue=tP;
            (project->bim.data)[i].betax0=betax0;
            (project->bim.data)[i].statValue=Fstat;
            (project->bim.data)[i].log10PValue=-std::log10(tP);
        }
        else
        {
            (project->bim.data)[i].PValue=1;
            (project->bim.data)[i].betax0=MML::DATA_NA;
            (project->bim.data)[i].statValue=MML::DATA_NA;
            (project->bim.data)[i].log10PValue=0;
        }
        if(isStop.load()) break;
    }
//    qDebug() << dealedNum.load();
    isFinished.store(true);
    if(!isStop.load()) project->logInfo+="Done ";
//    logInfoMux.unlock();
    const auto theTP = std::chrono::system_clock::now();
    const auto theDura = std::chrono::duration_cast<std::chrono::seconds>(theTP-startTP);
    const auto hrs = std::chrono::duration_cast<std::chrono::hours>(theDura);
    const auto mins = std::chrono::duration_cast<std::chrono::minutes>(theDura - hrs);
    const auto secs = std::chrono::duration_cast<std::chrono::seconds>(theDura - hrs - mins);
    project->logInfo+='['+std::to_string(hrs.count())+" hours:"+std::to_string(mins.count())+" mins:"+std::to_string(secs.count())+" secs]\n";
    //    emit logUpdate();
    if(!isStop.load()) emit taskFinished();
}

bool QGLMThread::makeThread()
{

    if(!isValid) return false;
    phe.read(project->phe.getNRow(),project->phe.data);
    startTP = std::chrono::system_clock::now();
    project->logInfo+="### start GLM ###\n";
    MML::Covariates cov(phe.rawN,project->cov.getNCol(),project->cov.data,phe.getFilterIndex(),phe.getFilterNum());
    if(!glm.read(phe,cov)){
        project->logInfo+="Error in preparing GLM initialization\n";
        emit logUpdate();
        return false;
    }
    project->logInfo+="samples number = "+std::to_string(project->phe.getNRow())+"\n";
    project->logInfo+="samples number(filtered) = "+std::to_string(phe.getFilterNum())+"\n";
    project->logInfo+="markers number = "+std::to_string(project->bim.nMarker)+"\n";
    emit logUpdate();
    isStop.store(false),isPause.store(false);
    std::thread theThread(&QGLMThread::run,this);
    theThread.detach();
    isThreadStart=true;
    return true;
}

void QGLMThread::runPart(int idThread, FD::PedDataIteratorSet pedSet, FD::BIMLogPDataIterator bimIter, MML::Phenotype inPhe, GEMMA::GLM inGLM, MML::Distribution inDis)
{
    MML::Mat tMarkers,fMarkers;
    tMarkers.resize(project->phe.getNRow(),1);
    tMarkers.toColVec();
    long long i=0,ti=0,tSum;
    double betax0,Fstat,df=glm.filterN-glm.factorN-1,tP;
    std::mutex* pMux=muxList.load();
    for(;!pedSet.isEnd();pedSet.next(),bimIter.next(),++i)
    {
        if(i%16==0)
        {
            tSum=dealedNum.load();
            dealedNum.store(i-ti+tSum);
            ti=i;
            std::unique_lock<std::mutex> ul(pMux[idThread]);
            conv.wait(ul,[this,idThread](){
                return !((isPauseList.load())[idThread]).load();
            });
            if(isStop.load()) break;
        }
        pedSet.read2(tMarkers.data);
        if(!inPhe.filterMarkers(tMarkers,fMarkers)){
            (bimIter.read())->PValue=1;
            (bimIter.read())->statValue=pedSet.dataNA;
            (bimIter.read())->betax0=pedSet.dataNA;
            (bimIter.read())->log10PValue=0;
        }
        else if(inGLM.analyze(fMarkers,betax0,Fstat))
        {
            tP=inDis.fcdfTri(Fstat,1,df);
            (bimIter.read())->PValue=tP;
            (bimIter.read())->statValue=Fstat;
            (bimIter.read())->betax0=betax0;
            (bimIter.read())->log10PValue=-std::log10(tP);
        }
        else
        {
            (bimIter.read())->PValue=1;
            (bimIter.read())->statValue=pedSet.dataNA;
            (bimIter.read())->betax0=pedSet.dataNA;
            (bimIter.read())->log10PValue=0;
        }
        if(isStop.load()) break;
    }
    isFinishedList[idThread].store(true);
    checkFinishedList();
    if(isFinished.load()){
        mux.lock();
        if(!infoOutput){
            infoOutput=true;
            if(!isStop.load()) project->logInfo+="Done ";
            const auto theTP = std::chrono::system_clock::now();
            const auto theDura = std::chrono::duration_cast<std::chrono::seconds>(theTP-startTP);
            const auto hrs = std::chrono::duration_cast<std::chrono::hours>(theDura);
            const auto mins = std::chrono::duration_cast<std::chrono::minutes>(theDura - hrs);
            const auto secs = std::chrono::duration_cast<std::chrono::seconds>(theDura - hrs - mins);
            project->logInfo+='['+std::to_string(hrs.count())+" hours:"+std::to_string(mins.count())+" mins:"+std::to_string(secs.count())+" secs]\n";
            if(!isStop.load()) emit taskFinished();

        }
        mux.unlock();
    }
}

bool QGLMThread::makeThreadMuti(int nThread)
{
    if(!isValid) return false;
    phe.read(project->phe.getNRow(),project->phe.data);
    startTP = std::chrono::system_clock::now();
    project->logInfo+="### start GLM ###\n";
    MML::Covariates cov(phe.rawN,project->cov.getNCol(),project->cov.data,phe.getFilterIndex(),phe.getFilterNum());
    if(!glm.read(phe,cov)){
        project->logInfo+="Error in preparing GLM initialization\n";
        emit logUpdate();
        return false;
    }
    project->logInfo+="samples number = "+std::to_string(project->phe.getNRow())+"\n";
    project->logInfo+="samples number(filtered) = "+std::to_string(phe.getFilterNum())+"\n";
    project->logInfo+="markers number = "+std::to_string(project->bim.nMarker)+"\n";
    emit logUpdate();
    long long MarkersNum = project->bim.nMarker;
    long long scaleSize = MarkersNum/nThread;
    long long remainSize;
    if(MarkersNum%nThread){
        ++scaleSize;
        remainSize=MarkersNum-scaleSize*(nThread-1);
    }
    else remainSize=scaleSize;
    nSubThread = nThread;
    createFlagList();
    infoOutput=false;
    isStop=false;
    dealedNum.store(0);
    for(int iThread=0;iThread<nThread;++iThread){
        FD::PedDataIteratorSet tSet;
        FD::PolyPedDataIterator tPPed;
        FD::BedDataIterator tBed;
        FD::BIMLogPDataIterator tBim;
        tSet.nSample=project->phe.getNRow();
        //        qDebug() << "1";
        if(iThread==nThread-1){
            tSet.nScale=remainSize;
            if(project->isPolyploid){
                tPPed.loadData(project->pData,iThread*scaleSize,MarkersNum);
                tSet.setPPedIter(tPPed);
            }
            else{
                tBed.loadData(project->bed,iThread*scaleSize,MarkersNum);
                tSet.setBedIter(tBed);
            }
            tBim.loadData(project->bim,iThread*scaleSize,MarkersNum);
        }
        else{
            //            qDebug() << "2";
            tSet.nScale=scaleSize;
            if(project->isPolyploid){
                tPPed.loadData(project->pData,iThread*scaleSize,(iThread+1)*scaleSize);
                tSet.setPPedIter(tPPed);
            }
            else{
                tBed.loadData(project->bed,iThread*scaleSize,(iThread+1)*scaleSize);
                tSet.setBedIter(tBed);
            }
            tBim.loadData(project->bim,iThread*scaleSize,(iThread+1)*scaleSize);
        }
        //        qDebug() << "3";
        std::thread theThread(&QGLMThread::runPart,this,
                              iThread,
                              tSet,
                              tBim,
                              phe,
                              glm,
                              dis);
        theThread.detach();
    }
    //    qDebug() << "Thread Start";
    isThreadStart=true;
    return true;
}

QFastPCAThread::QFastPCAThread(QObject *parent) : QObject(parent)
{

}

QFastPCAThread::QFastPCAThread(FD::GWASProject *inProject, QObject *parent)
    :FastPCAThread(inProject),QObject(parent)
{

}

void QFastPCAThread::run()
{

}

bool QFastPCAThread::makeThread()
{

}

QPCAThread::QPCAThread(QObject *parent) : QObject(parent)
{

}

QPCAThread::QPCAThread(FD::GWASProject *inProject, QObject *parent)
    :PCAThread(inProject),QObject(parent)
{

}

void QPCAThread::run()
{
    MML::Mat tX,CY,Q,R;
    size_t n=project->phe.getNRow(),pi=0,i;
    size_t p=project->bim.nMarker;
    while(pi+READSCALE<p){
        tX.resize(READSCALE,n);
        if(project->isPolyploid) project->pData.read2(tX.data,READSCALE);
        else project->bed.read2(tX.data,READSCALE);
        tX.t();

        pca.standardizeCol(tX,1.0);
        pca.addCovMat(tX,CovMat);
        pi+=READSCALE;
        dealedNum.store(pi*100/p);
        std::unique_lock<std::mutex> ul(mux);
        conv.wait(ul,[this]{
            return !isPause.load();
        });
        if(isStop.load()) break;
    }
    if(isStop.load()){
        isFinished.store(true);
        return;
    }
    tX.resize(p-pi,n);
    if(project->isPolyploid) project->pData.read2(tX.data,p-pi);
    else project->bed.read2(tX.data,p-pi);
    tX.t();
    pca.standardizeCol(tX,1.0);
    pca.addCovMat(tX,CovMat);
    CovMat.toSym('L');
    MML::Mat CovMatEVec;
    if(CovMat.symEig(CovMatEVec,EVal)){
        MML::quickSortEigen(CovMatEVec,EVal,false);
        PCs=CovMatEVec(0,0,n-1,PCn-1);
    }
    sumEVal = 0.0;
    for(i=0;i<n;++i) sumEVal+=EVal(i);
    isFinished.store(true);
    if(!isStop.load()) project->logInfo+="Done ";
//    logInfoMux.unlock();
    const auto theTP = std::chrono::system_clock::now();
    const auto theDura = std::chrono::duration_cast<std::chrono::seconds>(theTP-startTP);
    const auto hrs = std::chrono::duration_cast<std::chrono::hours>(theDura);
    const auto mins = std::chrono::duration_cast<std::chrono::minutes>(theDura - hrs);
    const auto secs = std::chrono::duration_cast<std::chrono::seconds>(theDura - hrs - mins);
    project->logInfo+='['+std::to_string(hrs.count())+" hours:"+std::to_string(mins.count())+" mins:"+std::to_string(secs.count())+" secs]\n";
    //    emit logUpdate();
    if(!isStop.load()) emit taskFinished();
}

bool QPCAThread::makeThread()
{
    size_t n=project->phe.getNRow();
    startTP = std::chrono::system_clock::now();
    PCn=std::min(n,PCn);
    CovMat.setData(n,n,0.0);
    isStop.store(false),isPause.store(false);
    std::thread theThread(&QPCAThread::run,this);
    theThread.detach();
    isThreadStart=true;
    return true;
}

QStructureThread::QStructureThread(QObject *parent) : QObject(parent)
{

}

QStructureThread::QStructureThread(FD::GWASProject *inProject, QObject *parent)
    :StructureThread(inProject),QObject(parent)
{

}

void QStructureThread::run()
{
    size_t iRep=0,nRep=nBurnIn+nRecord;
//    qDebug() << iRep;
    for(iRep=0;iRep<nRep;++iRep){
        if(iRep==nBurnIn) structure.setRecord(true);
        if(iRep%4==0){
            dealedNum.store((iRep+1)*100/nRep);
            std::unique_lock<std::mutex> ul(mux);
            conv.wait(ul,[this]{
                return !isPause.load();
            });
        }
//        qDebug() << iRep;
        structure.updateP();
//        qDebug() << iRep;
        structure.updateQ();
//        qDebug() << iRep;
        structure.updateZ();
        if(isStop.load()) break;
    }
    structure.onlyResult();
    out.resize(structure.nSample,structure.nPop);
    for(size_t i=0;i<structure.nSample;++i){
        for(size_t j=0;j<structure.nPop;++j){
            out(i,j)=structure.getSumQ(i,j)/nRecord;
        }
    }
    isFinished.store(true);
    if(!isStop.load()) project->logInfo+="Done ";
//    logInfoMux.unlock();
    const auto theTP = std::chrono::system_clock::now();
    const auto theDura = std::chrono::duration_cast<std::chrono::seconds>(theTP-startTP);
    const auto hrs = std::chrono::duration_cast<std::chrono::hours>(theDura);
    const auto mins = std::chrono::duration_cast<std::chrono::minutes>(theDura - hrs);
    const auto secs = std::chrono::duration_cast<std::chrono::seconds>(theDura - hrs - mins);
    project->logInfo+='['+std::to_string(hrs.count())+" hours:"+std::to_string(mins.count())+" mins:"+std::to_string(secs.count())+" secs]\n";
    //    emit logUpdate();
    if(!isStop.load()) emit taskFinished();
}

bool QStructureThread::makeThread()
{
    size_t nMarker = project->bim.nMarker, iMarker;
    size_t nSample = project->phe.getNRow(), iSample;
    size_t nPloid = project->pData.nPloid, iPloid;
    startTP = std::chrono::system_clock::now();
    short *tMarkers = nullptr;
    short *tPMarkers = nullptr;
    short tc;
    if (project->isPolyploid) {
        tPMarkers = new short[nSample];
        project->pData.resetReadPoint();
        structure.setParameter(nMarker, nPopulation, 2, nSample, nPloid);
        structure.initialize();
        for (iMarker = 0; iMarker < nMarker; ++iMarker) {
            project->pData.read(tPMarkers);
            for (iSample = 0; iSample < nSample; ++iSample) {
                tc = tPMarkers[iSample];
                if (tc == MML::UNASSIGNED) {
                    for (iPloid = 0; iPloid < nPloid; ++iPloid) {
                        *(structure.genotype(iMarker, iSample, iPloid)) = MML::UNASSIGNED;
                    }
                } else {
                    iPloid=0;
                    for (; iPloid<nPloid-tc; ++iPloid) {
                        *(structure.genotype(iMarker, iSample, iPloid)) = 0;
                    }
                    for (; iPloid<nPloid; ++iPloid) {
                        *(structure.genotype(iMarker, iSample, iPloid)) = 1;
                    }
                }
            }
        }
        project->pData.resetReadPoint();
    } else {
        structure.setParameter(nMarker, nPopulation, 2, nSample, 2);
        structure.initialize();
        tMarkers = new short[nSample];
        project->bed.resetReadPoint();
        for (iMarker = 0; iMarker < nMarker; ++iMarker){
            project->bed.read(tMarkers);
            for (iSample = 0; iSample < nSample; ++iSample){
                switch (tMarkers[iSample]) {
                case 0:
                    *(structure.genotype(iMarker, iSample, 0)) = 0;
                    *(structure.genotype(iMarker, iSample, 1)) = 0;
                    break;
                case 2:
                    *(structure.genotype(iMarker, iSample, 0)) = 1;
                    *(structure.genotype(iMarker, iSample, 1)) = 1;
                    break;
                case 1:
                    *(structure.genotype(iMarker, iSample, 0)) = 0;
                    *(structure.genotype(iMarker, iSample, 1)) = 1;
                    break;
                default:
                    *(structure.genotype(iMarker, iSample, 0)) = MML::UNASSIGNED;
                    *(structure.genotype(iMarker, iSample, 1)) = MML::UNASSIGNED;
                    break;
                }
            }
        }
        project->bed.resetReadPoint();
    }
    if (tMarkers) {
        delete [] tMarkers;
    }
    if (tPMarkers) {
        delete [] tPMarkers;
    }
    structure.randomizeZ();
    isStop.store(false), isPause.store(false);
    std::thread theThread(&QStructureThread::run,this);
    theThread.detach();
    isThreadStart = true;
    return true;
}

QFastStructureThread::QFastStructureThread(QObject *parent) : QObject(parent)
{

}

QFastStructureThread::QFastStructureThread(FD::GWASProject *inProject, QObject *parent)
    :FastStructureThread(inProject),QObject(parent)
{

}

void QFastStructureThread::run()
{
    MML::FastStructurePData tP,iP;
    MML::FastStructureQData tQ,iQ;
    MML::FastStructureParam initParam;
    MML::FastStructure initStru;
    MML::Mat iPLambda;
    std::vector<size_t> randomList;
    double tLL=-1e300,LL=-1e300,iterTol=1e300;
    size_t batchSize = std::min(param.nMarker,(unsigned long long)(1000000)/param.nSample);
    size_t nMarker=param.nMarker,nSample=param.nSample,nPloid=param.nPloid;
    size_t i,j,iMarker,iSample,iPloid,iter=0;
    meanDev.clear();
    if(nMarker <= batchSize){
        tP.initialize(param);
        tQ.initialize(param);
    }
    else{
        randomList.resize(nMarker);
        initParam = param;
        initParam.nMarker = batchSize;
        initStru.loadParam(initParam);
        initStru.initialize();
        for(i=0;i<5;++i){
            std::iota(randomList.begin(),randomList.end(),0);
            std::random_shuffle(randomList.begin(),randomList.end());
            for(j=0;j<batchSize;++j){
                iMarker = randomList[j];
                for(iSample=0;iSample<nSample;++iSample){
                    for(iPloid=0;iPloid<nPloid;++iPloid){
                        *(initStru.genotype(j,iSample,iPloid)) =
                            *(structure.genotype(iMarker,iSample,iPloid));
                    }
                }
            }

            initStru.batchInit(iP,iQ);
            initStru.resetP();
            initStru.resetQ();
            iPLambda=iP.PLambda;
            iP.initialize(param);
            if(prior == MML::FS_LOGISTIC){
                iP.PLambda=iPLambda;
            }
            structure.loadPData(iP);
            structure.loadQData(iQ);
            for(j=0;j<5;++j){
                structure.updateP2();
                structure.updatePHyper(true);
            }
            structure.updateQ();
            structure.updateP();
            tLL=structure.marginalLikelihood();
            project->logInfo+="init "+std::to_string(i+1)+" | Marginal Likelihood: "+std::to_string(tLL)+"\n";
            emit logUpdate();
            if(LL < tLL){
                structure.savePData(tP);
                structure.saveQData(tQ);
                LL = tLL;
            }
        }
    }
    structure.loadPData(tP);
    structure.loadQData(tQ);
    while(std::fabs(iterTol)>MML::EPS){
        std::unique_lock<std::mutex> ul(mux);
        conv.wait(ul,[this]{
            return !isPause.load();
        });
        structure.updateQ2();
        structure.updateQ();
        structure.updateP2();
        structure.updateP();
        if((iter+1)%10 == 0){
            tLL=structure.marginalLikelihood();
            iterTol = tLL - LL;
            LL=tLL;
            structure.updatePHyper(false);
            project->logInfo+=std::to_string(iter+1)+" | Marginal Likelihood: "+std::to_string(tLL)+"\n";
            emit logUpdate();
        }
        ++iter;
    }
    structure.getQ(out);
    if(nCV){
        project->logInfo+="calculing CV error ...\n";
        emit logUpdate();
        structure.savePData(tP);
        structure.saveQData(tQ);
        MML::FastStructure::calcuCV(structure.getG(),tP,tQ,param,meanDev,nCV);
        for(i=0;i<nCV;++i){
            project->logInfo+="mean deviance "+std::to_string(i+1)+" : "+std::to_string(meanDev[i])+"\n";
        }
        emit logUpdate();
    }
    isFinished.store(true);
    if(!isStop.load()) project->logInfo+="Done ";
    const auto theTP = std::chrono::system_clock::now();
    const auto theDura = std::chrono::duration_cast<std::chrono::seconds>(theTP-startTP);
    const auto hrs = std::chrono::duration_cast<std::chrono::hours>(theDura);
    const auto mins = std::chrono::duration_cast<std::chrono::minutes>(theDura - hrs);
    const auto secs = std::chrono::duration_cast<std::chrono::seconds>(theDura - hrs - mins);
    project->logInfo+='['+std::to_string(hrs.count())+" hours:"+std::to_string(mins.count())+" mins:"+std::to_string(secs.count())+" secs]\n";
    if(!isStop.load()) emit taskFinished();
}

bool QFastStructureThread::makeThread()
{
    size_t nMarker = project->bim.nMarker, iMarker;
    size_t nSample = project->phe.getNRow(), iSample;
    size_t nPloid = project->pData.nPloid, iPloid;
    short tc;
    short *tMarkers = nullptr;
    short *tPMarkers = nullptr;
    startTP = std::chrono::system_clock::now();

    param.nAllele = 2;
    param.nMarker = nMarker;
    param.nPop = nPopulation;
    param.nSample = nSample;
    param.prior = prior;

    if (project->isPolyploid) {

        /* no realization now */
        return false;


        param.nPloid = nPloid;
        tPMarkers = new short[nSample];
        project->pData.resetReadPoint();
        structure.loadParam(param);
        structure.initialize();
        for (iMarker = 0; iMarker < nMarker; ++iMarker) {
            project->pData.read(tPMarkers);
            for (iSample = 0; iSample < nSample; ++iSample) {
                tc = tPMarkers[iSample];
                if (tc == MML::UNASSIGNED) {
                    for (iPloid = 0; iPloid < nPloid; ++iPloid) {
                        *(structure.genotype(iMarker, iSample, iPloid)) = MML::UNASSIGNED;
                    }
                } else {
                    iPloid = 0;
                    for (; iPloid < nPloid - tc; ++iPloid) {
                        *(structure.genotype(iMarker,iSample,iPloid)) = 0;
                    }
                    for (; iPloid < nPloid; ++iPloid) {
                        *(structure.genotype(iMarker, iSample, iPloid)) = 1;
                    }
                }
            }
        }
        project->pData.resetReadPoint();
    } else {
        param.nPloid = 2;
        structure.loadParam(param);
        structure.initialize();
        tMarkers = new short[nSample];
        project->bed.resetReadPoint();
        for (iMarker = 0; iMarker < nMarker; ++iMarker) {
            project->bed.read(tMarkers);
            for (iSample = 0; iSample < nSample; ++iSample) {
                switch (tMarkers[iSample]) {
                case 0:
                    *(structure.genotype(iMarker, iSample, 0)) = 0;
                    *(structure.genotype(iMarker, iSample, 1)) = 0;
                    break;
                case 2:
                    *(structure.genotype(iMarker, iSample, 0)) = 1;
                    *(structure.genotype(iMarker, iSample, 1)) = 1;
                    break;
                case 1:
                    *(structure.genotype(iMarker, iSample, 0)) = 0;
                    *(structure.genotype(iMarker, iSample, 1)) = 1;
                    break;
                default:
                    *(structure.genotype(iMarker, iSample, 0)) = MML::UNASSIGNED;
                    *(structure.genotype(iMarker, iSample, 1)) = MML::UNASSIGNED;
                    break;
                }
            }
        }
        project->bed.resetReadPoint();
    }

    if (tMarkers) {
        delete [] tMarkers;
    }
    if (tPMarkers) {
        delete [] tPMarkers;
    }
    isStop.store(false), isPause.store(false);
    std::thread theThread(&QFastStructureThread::run, this);
    theThread.detach();
    isThreadStart = true;
    return true;
}

QKinshipThread::QKinshipThread(QObject *parent) : QObject(parent)
{

}

QKinshipThread::QKinshipThread(FD::GWASProject *inProject, MML::KinshipMode inMode, QObject *parent)
    :KinshipThread(inProject, inMode),QObject(parent)
{

}

void QKinshipThread::run()
{
    MML::IMat tX;
    size_t n = project->phe.getNRow(), pi = 0;
    size_t p = project->bim.nMarker;
    while (pi + READSCALE < p) {
        tX.resize(READSCALE, n);
        if (project->isPolyploid) {
            project->pData.read(tX.data, READSCALE);
        } else {
            project->bed.read(tX.data, READSCALE);
        }
        tX.t();
        kinCalculator->addMat(tX);
        pi += READSCALE;
        dealedNum.store(pi * 100 / p);
        std::unique_lock<std::mutex> ul(mux);
        conv.wait(ul, [this] {
            return !isPause.load();
        } );
        if (isStop.load()) {
            break;
        }
    }
    tX.resize(p - pi, n);
    if (project->isPolyploid) {
        project->pData.read(tX.data, p - pi);
    } else {
        project->bed.read(tX.data, p - pi);
    }
    tX.t();
    kinCalculator->addMat(tX);
    kinCalculator->getKinship(outKin);
    kinCalculator->clear();
    isFinished.store(true);

    if (!isStop.load()) {
        project->logInfo += "Done ";
    }

    const auto theTP = std::chrono::system_clock::now();
    const auto theDura = std::chrono::duration_cast<std::chrono::seconds>(theTP-startTP);
    const auto hrs = std::chrono::duration_cast<std::chrono::hours>(theDura);
    const auto mins = std::chrono::duration_cast<std::chrono::minutes>(theDura - hrs);
    const auto secs = std::chrono::duration_cast<std::chrono::seconds>(theDura - hrs - mins);
    project->logInfo+='['+std::to_string(hrs.count())+" hours:"+std::to_string(mins.count())+" mins:"+std::to_string(secs.count())+" secs]\n";

    if (!isStop.load()) {
        emit taskFinished();
    }
}

bool QKinshipThread::makeThread()
{
    isStop.store(false), isPause.store(false);
    kinCalculator->setIsPolyploid(project->isPolyploid);
    kinCalculator->setNPloid(project->getNPolyploid());
    startTP = std::chrono::system_clock::now();
    std::thread theThread(&QKinshipThread::run,this);
    theThread.detach();
    isThreadStart = true;
    return true;
}

QNormalBSAThread::QNormalBSAThread(QObject *parent) : QObject(parent)
{

}

QNormalBSAThread::QNormalBSAThread(FD::BSAProject *inProject, QObject *parent)
    :NormalBSAThread(inProject),QObject(parent)
{

}

void QNormalBSAThread::run()
{
    size_t iChr,nChr=(project->chrLen).size();
    size_t iInter = 0,iTotalInter = 0,iMarker,nInterMarker;
    size_t start,stop;
    size_t winLength=project->winLength;
    size_t strideLength=project->strideLength;
    std::vector<size_t> nChrInter(nChr);
    std::vector<double> threList;
    ValPoints2* points=project->points;
    size_t nMarkers=(project->data).getNRow(),nValidMarkers=0;
    double t1,t2,t3,s1,s2,s3;
    nInter = 0;

    if(markerThre) delete [] markerThre;
    markerThre=new double[nMarkers];
    for(iChr=0;iChr<nChr;++iChr){
        nChrInter[iChr] = std::lround(((project->chrLen)[iChr]-winLength)
                                      /(double)(strideLength));
        nInter += nChrInter[iChr];
    }
    calculatorF2.setPara(project->getMeanSamples(),project->getMaxDepth(),project->getPValue());
    calculatorF2.calcuThreshold(threList);
    if(inter) delete [] inter;
    inter = new FD::BSAInterVal[nInter];
    for(iMarker=0;iMarker<nMarkers;++iMarker){

        if(iTotalInter%8==0){
            std::unique_lock<std::mutex> ul(mux);
            conv.wait(ul,[this]{
                return !isPause.load();
            });
            dealedNum.store(iMarker*50/nMarkers);
            if(isStop.load()) break;
        }

        points[iMarker].val1=MML::DATA_NA;
        points[iMarker].val2=MML::DATA_NA;
        if((project->data)(iMarker,0)==MML::DATA_NA||
           (project->data)(iMarker,1)==MML::DATA_NA||
           (project->data)(iMarker,2)==MML::DATA_NA||
           (project->data)(iMarker,3)==MML::DATA_NA) continue;
        s1=(project->data)(iMarker,0)+(project->data)(iMarker,1);
        s2=(project->data)(iMarker,2)+(project->data)(iMarker,3);
        if(std::lround(s1)<project->threDeep) continue;
        if(std::lround(s2)<project->threDeep) continue;
        t1=(project->data)(iMarker,1)/s1;
        t2=(project->data)(iMarker,3)/s2;
        if((t1<project->threLow&&t2<project->threLow)||
           (t1>project->threHigh&&t2>project->threHigh)) continue;
        points[iMarker].val1=t1;
        points[iMarker].val2=t2;
        markerThre[iMarker]=threList[(std::lround(s1)+std::lround(s2))/2];
    }
    for(iChr=0;iChr<nChr;++iChr){
        start=1,stop=winLength;
        for(iInter=0;iInter<nChrInter[iChr];++iInter){

            if(iTotalInter%8==0){
                std::unique_lock<std::mutex> ul(mux);
                conv.wait(ul,[this]{
                    return !isPause.load();
                });
                dealedNum.store(iTotalInter*50/nInter+50);
                if(isStop.load()) break;
            }

            if(iInter==nChrInter[iChr]-1) stop = (project->chrLen)[iChr];
            iMarker = project->searchPointIndex(iChr,start);
            s1=0,s2=0,s3=0,t1=0,t2=0,t3=0;
            nInterMarker = 0;
            while(iMarker<nMarkers&&
                  points[iMarker].idchr==iChr&&
                  points[iMarker].pos<=stop){
                if(points[iMarker].val1!=MML::DATA_NA){
                    s1+=points[iMarker].val1;
                    s2+=points[iMarker].val2;
                    s3+=markerThre[iMarker];
                    ++nInterMarker;
                }
                ++iMarker;
            }
            inter[iTotalInter].n=nInterMarker;
            inter[iTotalInter].idChr=iChr;
            inter[iTotalInter].start=start;
            inter[iTotalInter].stop=stop;
            if(nInterMarker){
                inter[iTotalInter].indexH=s1/nInterMarker;
                inter[iTotalInter].indexL=s2/nInterMarker;
                inter[iTotalInter].val   =
                        inter[iTotalInter].indexH - inter[iTotalInter].indexL;
                inter[iTotalInter].thre  =s3/nInterMarker;
            }
            else{
                inter[iTotalInter].indexH=MML::DATA_NA;
                inter[iTotalInter].indexL=MML::DATA_NA;
                inter[iTotalInter].val   =MML::DATA_NA;
                inter[iTotalInter].thre  =MML::DATA_NA;
            }
            ++iTotalInter;
            start+=strideLength;
            stop+=strideLength;
        }
    }
    isFinished.store(true);
    if(!isStop.load()) project->logInfo+="Done ";
//    logInfoMux.unlock();
    const auto theTP = std::chrono::system_clock::now();
    const auto theDura = std::chrono::duration_cast<std::chrono::seconds>(theTP-startTP);
    const auto hrs = std::chrono::duration_cast<std::chrono::hours>(theDura);
    const auto mins = std::chrono::duration_cast<std::chrono::minutes>(theDura - hrs);
    const auto secs = std::chrono::duration_cast<std::chrono::seconds>(theDura - hrs - mins);
    project->logInfo+='['+std::to_string(hrs.count())+" hours:"+std::to_string(mins.count())+" mins:"+std::to_string(secs.count())+" secs]\n";
    //    emit logUpdate();
    if(!isStop.load()) emit taskFinished();
}

bool QNormalBSAThread::makeThread()
{
    qDebug() << "start";
    isStop.store(false),isPause.store(false);
    startTP = std::chrono::system_clock::now();
    std::thread theThread(&QNormalBSAThread::run,this);
    theThread.detach();
    isThreadStart=true;
    return true;
}

void qDeleteThreadRun(const BaseThread *p)
{
    if(!p) return ;
    switch (p->info) {
    case GLM_GEMMA:
        delete dynamic_cast<const QGLMThread*>(p);
        break;
    case MLM_GEMMA:
        delete dynamic_cast<const QGEMMAThread*>(p);
        break;
    case MLM_EMMAX:
        delete dynamic_cast<const QEMMAXThread*>(p);
        break;
    case PCA_PCA:
        delete dynamic_cast<const QPCAThread*>(p);
        break;
    case PCA_FASTPCA:
        delete dynamic_cast<const QFastPCAThread*>(p);
        break;
    case STR_STRUCTURE:
        delete dynamic_cast<const QStructureThread*>(p);
        break;
    case STR_FASTSTRUCTURE:
        delete dynamic_cast<const QFastStructureThread*>(p);
        break;
    case KIN_KINSHIP:
        delete dynamic_cast<const QKinshipThread*>(p);
        break;
    case BSA_NORMALBSA:
        delete dynamic_cast<const QNormalBSAThread*>(p);
        break;
    default: break;
    }
}

bool qSaveOutput(BaseThread *p, const char *outFile)
{
    if(!p) return false;
    switch (p->info) {
    case GLM_GEMMA:
        return (dynamic_cast<QGLMThread*>(p))->saveOutput(outFile);
        break;
    case MLM_GEMMA:
        return (dynamic_cast<QGEMMAThread*>(p))->saveOutput(outFile);
        break;
    case MLM_EMMAX:
        return (dynamic_cast<QEMMAXThread*>(p))->saveOutput(outFile);
        break;
    case PCA_PCA:
        return (dynamic_cast<QPCAThread*>(p))->saveOutput(outFile);
        break;
    case PCA_FASTPCA:
        return (dynamic_cast<QFastPCAThread*>(p))->saveOutput(outFile);
        break;
    case STR_STRUCTURE:
        return (dynamic_cast<QStructureThread*>(p))->saveOutput(outFile);
        break;
    case STR_FASTSTRUCTURE:
        return (dynamic_cast<QFastStructureThread*>(p))->saveOutput(outFile);
        break;
    case KIN_KINSHIP:
        return (dynamic_cast<QKinshipThread*>(p))->saveOutput(outFile);
        break;
    case BSA_NORMALBSA:
        return (dynamic_cast<QNormalBSAThread*>(p))->saveOutput(outFile);
        break;
    default: break;
    }
    return false;
}

FD::BIMLogPData *qGetBIMInfo(BaseThread *p)
{
    if(!p) return nullptr;
    switch (p->info) {
    case GLM_GEMMA:
        return &((dynamic_cast<QGLMThread*>(p))->project->bim);
        break;
    case MLM_GEMMA:
        return &((dynamic_cast<QGEMMAThread*>(p))->project->bim);
        break;
    case MLM_EMMAX:
        return &((dynamic_cast<QEMMAXThread*>(p))->project->bim);
        break;
    case PCA_PCA:
        return &((dynamic_cast<QPCAThread*>(p))->project->bim);
        break;
    case PCA_FASTPCA:
        return &((dynamic_cast<QFastPCAThread*>(p))->project->bim);
        break;
    case STR_STRUCTURE:
        return &((dynamic_cast<QStructureThread*>(p))->project->bim);
        break;
    case STR_FASTSTRUCTURE:
        return &((dynamic_cast<QFastStructureThread*>(p))->project->bim);
        break;
    case KIN_KINSHIP:
        return &((dynamic_cast<QKinshipThread*>(p))->project->bim);
        break;
    default: break;
    }
    return nullptr;
}






};

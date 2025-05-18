#ifndef QBASETHREAD_HPP
#define QBASETHREAD_HPP

#include <QObject>
#include "ThreadRun.h"
#include "FileDeal/FileIterator.hpp"

namespace ThreadRun
{

class QEMMAXThread : public QObject,public EMMAXThread
{
    Q_OBJECT
private:
    explicit QEMMAXThread(QObject *parent = nullptr);
public:
    explicit QEMMAXThread(FD::GWASProject* inProject,QObject *parent = nullptr);

    void run() override;
    bool makeThread() override;
    void runPart(int idThread,
                 FD::PedDataIteratorSet pedSet,
                 FD::BIMLogPDataIterator bimIter,
                 MML::Phenotype inPhe,
                 EMMAX::Betas inBetas,
                 MML::Distribution inDis);
    bool makeThreadMuti(int nThread);
signals:
    void logUpdate();
    void taskFinished();

};

class QGEMMAThread : public QObject,public GEMMAThread
{
    Q_OBJECT
private:
    explicit QGEMMAThread(QObject *parent = nullptr);
public:
    explicit QGEMMAThread(FD::GWASProject* inProject,QObject *parent = nullptr);

    void run() override;
    bool makeThread() override;
    void runPart(int idThread,
                 FD::PedDataIteratorSet pedSet,
                 FD::BIMLogPDataIterator bimIter,
                 MML::Phenotype inPhe,
                 GEMMA::MLM inMLM,
                 MML::Distribution inDis);
    bool makeThreadMuti(int nThread);
signals:
    void logUpdate();
    void taskFinished();

};

class QGLMThread : public QObject,public GLMThread
{
    Q_OBJECT
private:
    explicit QGLMThread(QObject *parent = nullptr);
public:
    explicit QGLMThread(FD::GWASProject* inProject,QObject *parent = nullptr);

    void run() override;
    bool makeThread() override;
    void runPart(int idThread,
                 FD::PedDataIteratorSet pedSet,
                 FD::BIMLogPDataIterator bimIter,
                 MML::Phenotype inPhe,
                 GEMMA::GLM inGLM,
                 MML::Distribution inDis);
    bool makeThreadMuti(int nThread);
signals:
    void logUpdate();
    void taskFinished();

};

class QPCAThread : public QObject,public PCAThread
{
    Q_OBJECT
private:
    explicit QPCAThread(QObject *parent = nullptr);
public:
    explicit QPCAThread(FD::GWASProject* inProject,QObject *parent = nullptr);

    void run() override;
    bool makeThread() override;
signals:
    void logUpdate();
    void taskFinished();

};

class QFastPCAThread : public QObject,public FastPCAThread
{
    Q_OBJECT
private:
    explicit QFastPCAThread(QObject *parent = nullptr);
public:
    explicit QFastPCAThread(FD::GWASProject* inProject,QObject *parent = nullptr);

    void run() override;
    bool makeThread() override;
signals:
    void logUpdate();
    void taskFinished();

};

class QStructureThread : public QObject,public StructureThread
{
    Q_OBJECT
private:
    explicit QStructureThread(QObject *parent = nullptr);
public:
    explicit QStructureThread(FD::GWASProject* inProject,QObject *parent = nullptr);

    void run() override;
    bool makeThread() override;
signals:
    void logUpdate();
    void taskFinished();

};

class QFastStructureThread : public QObject,public FastStructureThread
{
    Q_OBJECT
private:
    explicit QFastStructureThread(QObject *parent = nullptr);
public:
    explicit QFastStructureThread(FD::GWASProject* inProject,QObject *parent = nullptr);

    void run() override;
    bool makeThread() override;
signals:
    void logUpdate();
    void taskFinished();

};

class QKinshipThread : public QObject,public KinshipThread
{
    Q_OBJECT
private:
    explicit QKinshipThread(QObject *parent = nullptr);
public:
    explicit QKinshipThread(FD::GWASProject* inProject,
                            MML::KinshipMode inMode = MML::REL_TYPE,
                            QObject *parent = nullptr);

    void run() override;
    bool makeThread() override;
signals:
    void logUpdate();
    void taskFinished();

};

class QNormalBSAThread : public QObject,public NormalBSAThread
{
    Q_OBJECT
private:
    explicit QNormalBSAThread(QObject *parent = nullptr);
public:
    explicit QNormalBSAThread(FD::BSAProject* inProject,
                              QObject *parent = nullptr);

    void run() override;
    bool makeThread() override;
signals:
    void logUpdate();
    void taskFinished();

};

void qDeleteThreadRun(const BaseThread* p);
bool qSaveOutput(BaseThread* p,const char* outFile);
FD::BIMLogPData* qGetBIMInfo(BaseThread* p);

};

#endif // QBASETHREAD_HPP

#ifndef ANALYZELISTVIEW_H
#define ANALYZELISTVIEW_H

#include <QWidget>
#include <QEvent>
#include <QPushButton>
#include <QProgressBar>
#include <QMenu>
#include <QListView>
#include <QStyledItemDelegate>

#include "ThreadProcess/QThreadRun.hpp"
#include "FileDeal/ProjectFilesRecord.hpp"

enum runState
{
    RUN_UNKNOWN = 0,
    RUN_DEFAULT = 1,
    RUN_UNHANDLED = 2,
    RUN_PAUSE = 3,
    RUN_RUNNING = 4,
    RUN_FINISHED = 5
};

class AnalyzeUnitInfo
{
public:
    QVector<size_t>  ldataList;
    QVector<QString> ldataName;
    QVector<QString> ldataIcon;

    QVector<QString> sdataList;
    QVector<QString> sdataName;
    QVector<QString> sdataIcon;

    runState state;
    AnalyzeUnitInfo();
    AnalyzeUnitInfo(const QString& inPhe,
                    const QString& inBed,
                    const QString& inBim,
                    const QString& inCov,
                    const QString& inKin);
    AnalyzeUnitInfo(const QString& inPhe,
                    const QString& inBed,
                    const QString& inBim,
                    const QString& inCov);
    void setNFile(int n);
    void setNNumber(int n);
    void setRunState(runState inState);

    int getNFile() const;
    int getNNumber() const;
    QString getIFile(int id) const;
    QString getIFileName(int id) const;
    QString getINumberName(int id) const;
    size_t  getINumber(int id) const;
    QString getINumberIcon(int id) const;
    QString getIFileIcon(int id) const;
    QString& getIFile(int id);
    QString& getIFileName(int id);
    QString& getINumberName(int id);
    size_t&  getINumber(int id);
    QString& getINumberIcon(int id);
    QString& getIFileIcon(int id);
    void addFile(const QString& file, const QString& name, const QString& icon);
    void addNumber(size_t n, const QString& name, const QString& icon);
    void clear();
};

Q_DECLARE_METATYPE(AnalyzeUnitInfo);

class AnalyzeWidget : public QWidget
{
    Q_OBJECT
private:

public:
    const static QString startButtonStyle1;
    const static QString startButtonStyle2;
    const static QString runningBarStyle;
    const static int startButtonSize;
    const static int runningBarHeight;
    QPushButton* startButton;
    QProgressBar* runningBar;
    ThreadRun::BaseThread* pThread;
    AnalyzeUnitInfo* pUnit;
    QWidget * p;
    int id;
    explicit AnalyzeWidget(QWidget *parent = nullptr);
    ~AnalyzeWidget();
    bool setThread(ThreadRun::BaseThread* inThread);
    bool setAnalyzeUnit(AnalyzeUnitInfo* inUnit);
    void setID(int i);
    void init();
    void clear();
signals:
    void signalProjectStateChanged(int i, runState state);
    void signalProjectFinished(int i);
    void signalProjectLogUpdate(int i);
public slots:
    void startButtonTrigger();
    void projectFinished();
    void projectLogUpdate();
};

class AnalyzeItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    int orderFrameWidth;
    int infoFrameLeftMargin;
    int infoFrameRightMargin;
    int numberInfoLength;

    QList<AnalyzeUnitInfo>* pUnitList;
    QList<AnalyzeWidget*>*   pWidgetList;
    QList<ThreadRun::BaseThread*>* pThreadList;
    QList<FD::BaseProject*>* pProjectList;
    explicit AnalyzeItemDelegate(QWidget *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setUnitList(QList<AnalyzeUnitInfo>* inUnitList);
    void setWidgetList(QList<AnalyzeWidget*>* inWidgetList);
    void setThreadList(QList<ThreadRun::BaseThread*>* inThreadList);
    void setProjectList(QList<FD::BaseProject*>* inProjectList);

    static int minWidth;
signals:
};

class AnalyzeListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit AnalyzeListModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void setUnitList(QList<AnalyzeUnitInfo>* inUnitList);
private:
    QList<AnalyzeUnitInfo>* pUnitList;
};

class MenuActionSet
{
public:
    QAction* removeItem;
    QAction* removeAll;
    QAction* loadFileGWAS;
    QAction* loadFileBSA;
    QAction* editPhe;
    QAction* editCov;
    QAction* editKin;
    QAction* addNewItem;
    QAction* saveOutput;
    QAction* visualize;
    MenuActionSet();
    ~MenuActionSet();
    void init(QMenu* parent);
    void setRunDisabled();
    void setEnabled();
};

class AnalyzeListViewMenu : public QMenu
{
    Q_OBJECT
public:
    MenuActionSet* pActions;
    explicit AnalyzeListViewMenu(QWidget* parent = nullptr);
    void connectAnalyzeListView(QListView* pView);
    ~AnalyzeListViewMenu();
};


class AnalyzeListView : public QListView
{
    Q_OBJECT
public:
    MML::StructureParam struPara;
    MML::FastStructureParam fastStruPara;
    MML::KinshipParam kinshipPara;
    AnalyzeItemDelegate* pDelegate;
    QList<AnalyzeUnitInfo>* pUnitList;
    QList<AnalyzeWidget*>*   pWidgetList;
    QList<ThreadRun::BaseThread*>* pThreadList;
    QList<FD::BaseProject*>* pProjectList;
    AnalyzeListModel* pModel;
    AnalyzeListViewMenu* pMenu;
    explicit AnalyzeListView(QWidget *parent = nullptr);
    ~AnalyzeListView();
protected:
    void closeEvent(QCloseEvent* e) override;
    bool pro_loadFileAt(int id, FD::ProjectInfo info);
    bool pro_loadFileAt(int id, FD::BaseProjectFilesSet inSet);
    bool pro_addNew();
    bool pro_clearAt(int row);
    bool pro_removeAt(int row);
    bool pro_stopAt(int row);
    bool pro_AnalyzeAt(int row, ThreadRun::AnalyzeClass info, int nThread);
    bool pro_widgetCliked(const QModelIndex& index);
    bool pro_saveResultAt(int row);
    bool pro_editPheAt(int row);
    bool pro_editCovAt(int row);
    bool pro_editKinAt(int row);
    bool pro_startVisualizationAt(int row);
public:
    bool clearAt(int row); //pro_clearAt(int row)
    bool addNew(); //pro_addNew()
    bool removeOne(int row); //pro_removeAt(int row)
    bool stopOne(int row); //pro_stopAt(int row)
    bool startLoad(int row, FD::ProjectInfo info); //pro_loadFileAt
signals:
    void windowsClose();
    void projectChanged(int row);
    void projectRemoved(int row);
    void projectSelected(int row);
    void projectSelectedState(runState state);
    void projectLogUpdate(int row);
    void projectAdd();
    void allRemoved();
    void projectInserted(int row);

public slots:
    void runEMMAX();
    void runGEMMA();
    void runGLM();
    void runPCA();
    void runFastPCA();
    void runStructure();
    void runFastStructure();
    void runKinship();
    void runNormalBSA();
    void removeCurrent();//pro_removeAt
    void stopCurrent();//pro_stopAt
    void removeAll(bool isWarning = true); //pro_removeAt
    void saveCurrent(); //pro_saveResultAt
    void loadFile(FD::ProjectInfo info = FD::PROJECT_GWAS); // pro_loadFileAt
    void loadFileAt(int id, FD::ProjectInfo info = FD::PROJECT_GWAS); // pro_loadFileAt
    void loadFile(FD::BaseProjectFilesSet inSet);
    void loadFileGWAS();
    void loadFileBSA();
    void editPhe();
    void editCov();
    void editKin();
    void startVisualization();
    void startGWAS(size_t id, ThreadRun::AnalyzeClass info, int nThread);
    void startStructure(size_t id, int nThread);
    void startFastStructure(size_t id, int nThread);
    void startKinship(size_t id, int nThread);
    void startNormalBSA(size_t id, int nThread);
    void selectProject(int id);
    void pauseOrContinueSelectedProject();
    void receiveProjectStateChanged(int id, runState state);
    void receiveProjectFinished(int id);
    void receiveProjectLogUpdate(int id);
    void menuExec(const QPoint& pos);
    void widgetCliked(const QModelIndex& index);
};

#endif // ANALYZEITEMDELEGATE_H

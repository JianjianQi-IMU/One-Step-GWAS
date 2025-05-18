#include "AnalyzeListView.h"

#include <QPainter>
#include <QPainterPath>
#include <QMessageBox>
#include <QFileDialog>
#include <QFontMetricsF>

#include "PreVisualizationWidget/PreBSAVisualizeWidget.hpp"
#include "PreVisualizationWidget/PreKinshipVisualizeWidget.hpp"
#include "PreVisualizationWidget/PrePCAVisualizeWidget.hpp"
#include "PreVisualizationWidget/PreStrucVisualizeWidget.hpp"
#include "PreVisualizationWidget/PreVisualizeWidget.hpp"

#include "LoadFileWidget/LoadBSAFileWidget.hpp"
#include "LoadFileWidget/LoadGWASFileWidget.hpp"

#include "SetParamWidget/SetFastStructureParamWidget.hpp"
#include "SetParamWidget/SetGWASParamWidget.hpp"
#include "SetParamWidget/SetNormalBSAParamWidget.hpp"
#include "SetParamWidget/SetStructureParamWidget.hpp"
#include "SetParamWidget/SetKinshipParamWidget.hpp"

#include "DataInteractTable/DataInteractTableWidget.hpp"

#include <QDebug>

AnalyzeUnitInfo::AnalyzeUnitInfo()
{
    state = RUN_UNKNOWN;
}

AnalyzeUnitInfo::AnalyzeUnitInfo(const QString &inPhe, const QString &inBed, const QString &inBim,
                                 const QString &inCov, const QString &inKin)
{
    state = RUN_DEFAULT;
    sdataList.append(inBed);
    sdataList.append(inBim);
    sdataList.append(inPhe);
    sdataList.append(inCov);
    sdataList.append(inKin);
    sdataName.append("bed");
    sdataName.append("bim");
    sdataName.append("phe");
    sdataName.append("cov");
    sdataName.append("kin");
    sdataIcon.append(":/icon/bedFile.png");
    sdataIcon.append(":/icon/bimFile.png");
    sdataIcon.append(":/icon/pheFile.png");
    sdataIcon.append(":/icon/covFile.png");
    sdataIcon.append(":/icon/kinFile.png");

    ldataList.append(0);
    ldataList.append(0);
    ldataList.append(0);
    sdataName.append("Samples");
    sdataName.append("Markers");
    sdataName.append("Factors");
    sdataIcon.append(":/icon/n_phenotypeIcon.png");
    sdataIcon.append(":/icon/n_markersIcon.png"  );
    sdataIcon.append(":/icon/n_covariateIcon.png");
}

AnalyzeUnitInfo::AnalyzeUnitInfo(const QString &inPhe,
                                 const QString &inBed,
                                 const QString &inBim,
                                 const QString &inCov)
{
    state = RUN_DEFAULT;
    sdataList.append(inBed);
    sdataList.append(inBim);
    sdataList.append(inPhe);
    sdataList.append(inCov);
    sdataName.append("bed");
    sdataName.append("bim");
    sdataName.append("phe");
    sdataName.append("cov");

    ldataList.append(0);
    ldataList.append(0);
    ldataList.append(0);
    sdataName.append("Samples");
    sdataName.append("Markers");
    sdataName.append("Factors");
}

void AnalyzeUnitInfo::setNFile(int n)
{
    sdataList.clear();
    sdataName.clear();
    sdataIcon.clear();
    sdataList.resize(n);
    sdataName.resize(n);
    sdataIcon.resize(n);
}

void AnalyzeUnitInfo::setNNumber(int n)
{
    ldataList.clear();
    ldataName.clear();
    ldataIcon.clear();
    ldataList.resize(n);
    ldataName.resize(n);
    ldataIcon.resize(n);
}

void AnalyzeUnitInfo::setRunState(runState inState)
{
    state = inState;
}

int AnalyzeUnitInfo::getNFile() const
{
    return sdataList.size();
}

int AnalyzeUnitInfo::getNNumber() const
{
    return ldataList.size();
}

QString AnalyzeUnitInfo::getIFile(int id) const
{
    if (id >= sdataList.size() || id < 0) {
        return QString();
    }
    return sdataName[id];
}

QString AnalyzeUnitInfo::getIFileName(int id) const
{
    if (id >= sdataList.size() || id < 0) {
        return QString();
    }
    return sdataList[id];
}

QString AnalyzeUnitInfo::getINumberName(int id) const
{
    if (id >= ldataList.size() || id < 0) return QString();
    return ldataName[id];
}

size_t AnalyzeUnitInfo::getINumber(int id) const
{
    if (id >= ldataList.size() || id < 0) return SIZE_MAX;
    return ldataList[id];
}

QString AnalyzeUnitInfo::getINumberIcon(int id) const
{
    if (id >= ldataList.size() || id < 0) return QString();
    return ldataIcon[id];
}

QString AnalyzeUnitInfo::getIFileIcon(int id) const
{
    if (id >= sdataList.size() || id < 0) return QString();
    return sdataIcon[id];
}

QString &AnalyzeUnitInfo::getIFile(int id)
{
    return sdataName[id];
}

QString &AnalyzeUnitInfo::getIFileName(int id)
{
    return sdataList[id];
}

QString &AnalyzeUnitInfo::getINumberName(int id)
{
    return ldataName[id];
}

size_t &AnalyzeUnitInfo::getINumber(int id)
{
    return ldataList[id];
}

QString &AnalyzeUnitInfo::getINumberIcon(int id)
{
    return ldataIcon[id];
}

QString &AnalyzeUnitInfo::getIFileIcon(int id)
{
    return sdataIcon[id];
}

void AnalyzeUnitInfo::addFile(const QString &file, const QString &name, const QString &icon)
{
    sdataList.append(file);
    sdataName.append(name);
    sdataIcon.append(icon);
}

void AnalyzeUnitInfo::addNumber(size_t n, const QString &name, const QString &icon)
{
    ldataList.append(n);
    ldataName.append(name);
    ldataIcon.append(icon);
}

void AnalyzeUnitInfo::clear()
{
    state=RUN_UNKNOWN;
    sdataList.clear();
    sdataName.clear();
    sdataIcon.clear();
    ldataList.clear();
    ldataName.clear();
    ldataIcon.clear();
}


const QString AnalyzeWidget::startButtonStyle1 = QString(
"QPushButton{ \
border-style: solid; \
border-top-color: transparent; \
border-right-color: transparent; \
border-left-color: transparent; \
border-bottom-color: transparent; \
border-width: 1px; \
border-style: solid; \
background-image: url(:/icon/startIcon.png); \
padding: 2px; \
background-color: transparent; \
} \
QPushButton:hover{ \
border-style: solid; \
border-top-color: transparent; \
border-right-color: transparent; \
border-left-color: transparent; \
border-bottom-color: transparent; \
border-width: 1px; \
border-style: solid; \
background-image: url(:/icon/startIcon3.png); \
padding: 2px; \
background-color: transparent; \
} \
QPushButton:pressed{ \
border-style: solid; \
border-top-color: transparent; \
border-right-color: transparent; \
border-left-color: transparent; \
border-bottom-color: transparent; \
border-width: 1px; \
border-style: solid; \
background-image: url(:/icon/startIcon3.png); \
padding: 2px; \
background-color: #cfcfcf; \
} \
QPushButton:disabled{ \
border-style: solid; \
border-top-color: transparent; \
border-right-color: transparent; \
border-left-color: transparent; \
border-bottom-color: transparent; \
border-width: 1px; \
border-style: solid; \
background-image: url(:/icon/startIcon2); \
padding: 2px; \
background-color: transparent;}"
);

const QString AnalyzeWidget::startButtonStyle2 = QString (
"QPushButton{ \
border-style: solid; \
border-top-color: transparent; \
border-right-color: transparent; \
border-left-color: transparent; \
border-bottom-color: transparent; \
border-width: 1px; \
border-style: solid; \
background-image: url(:/icon/startIcon4.png); \
padding: 2px; \
background-color: transparent; \
} \
QPushButton:hover{ \
border-style: solid; \
border-top-color: transparent; \
border-right-color: transparent; \
border-left-color: transparent; \
border-bottom-color: transparent; \
border-width: 1px; \
border-style: solid; \
background-image: url(:/icon/startIcon5.png); \
padding: 2px; \
background-color: transparent; \
} \
QPushButton:pressed{ \
border-style: solid; \
border-top-color: transparent; \
border-right-color: transparent; \
border-left-color: transparent; \
border-bottom-color: transparent; \
border-width: 1px; \
border-style: solid; \
background-image: url(:/icon/startIcon5.png); \
padding: 2px; \
background-color: #cfcfcf; \
} \
QPushButton:disabled{ \
border-style: solid; \
border-top-color: transparent; \
border-right-color: transparent; \
border-left-color: transparent; \
border-bottom-color: transparent; \
border-width: 1px; \
border-style: solid; \
background-image: url(:/icon/startIcon6); \
padding: 2px; \
background-color: transparent;}"
);

const QString AnalyzeWidget::runningBarStyle = QString(
"QProgressBar { \
text-align: center; \
color: rgb(240, 240, 240); \
border-width: 2px; \
border-color: rgb(58, 58, 58); \
border-style: inset; \
background-color:#1e1d23; \
} \
QProgressBar::chunk {background-color: #00dddd;}"
);

const int AnalyzeWidget::startButtonSize = 64;
const int AnalyzeWidget::runningBarHeight = 20;

AnalyzeWidget::AnalyzeWidget(QWidget *parent) : QWidget(parent)
{
    p = parent;
    startButton = nullptr;
    runningBar = nullptr;
    id = 0;
}

AnalyzeWidget::~AnalyzeWidget()
{
    if (startButton) delete startButton;
    if (runningBar) delete runningBar;
}

bool AnalyzeWidget::setThread(ThreadRun::BaseThread *inThread)
{
    if (!inThread) return false;
    pThread = inThread;
    if (pThread->info == ThreadRun::STR_FASTSTRUCTURE) {
        runningBar->setRange(0, 0);
    }
    return true;
}

bool AnalyzeWidget::setAnalyzeUnit(AnalyzeUnitInfo *inUnit)
{
    if (!inUnit) return false;
    pUnit = inUnit;
    return true;
}

void AnalyzeWidget::setID(int i)
{
    if (i >= 0) id = i;
}

void AnalyzeWidget::init()
{

    if (!startButton) startButton = new QPushButton(p);
    if (!runningBar) runningBar = new QProgressBar(p);
    startButton->resize(startButtonSize, startButtonSize);
    startButton->setStyleSheet(startButtonStyle1);
    runningBar->setStyleSheet(runningBarStyle);
    startButton->setHidden(true);
    runningBar->setHidden(true);

    connect(startButton, &QPushButton::clicked, this, &AnalyzeWidget::startButtonTrigger);
}

void AnalyzeWidget::clear()
{
    pThread = nullptr;
    pUnit = nullptr;
}

void AnalyzeWidget::startButtonTrigger()
{
    if (pThread && pUnit && pThread->isThreadStart && (!pThread->isFinished.load())) {
        if (pUnit->state == RUN_RUNNING) {
            pUnit->state = RUN_PAUSE;
            pThread->pause();
            emit signalProjectStateChanged(id, RUN_PAUSE);
        } else if (pUnit->state == RUN_PAUSE) {
            pUnit->state = RUN_RUNNING;
            pThread->toContinue();
            emit signalProjectStateChanged(id, RUN_RUNNING);
        }
    }
}

void AnalyzeWidget::projectFinished()
{
    if (pUnit) pUnit->state = RUN_FINISHED;
    emit signalProjectFinished(id);
}

void AnalyzeWidget::projectLogUpdate()
{
    emit signalProjectLogUpdate(id);
}

int AnalyzeItemDelegate::minWidth = 600;

AnalyzeItemDelegate::AnalyzeItemDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{
    orderFrameWidth = 80;
    infoFrameLeftMargin = 20;
    infoFrameRightMargin = 20;
    numberInfoLength = 150;
}

void AnalyzeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);
    long x0 = option.rect.left(), y0 = option.rect.top();
    long width0 = option.rect.width(), height0 = option.rect.height();
    long infoFrameWidth = width0 - orderFrameWidth;
    long runningBarx0 = x0 + orderFrameWidth + infoFrameLeftMargin;
    long runningBarcy0 = y0 + height0 * 0.65;
    long numberInfocy0 = y0 + height0 * 0.35;
    long runningBarWid = width0 - runningBarx0 - 74 - infoFrameRightMargin;
    long startBtnx0 = runningBarx0 + runningBarWid + 10, startBtncy0 = runningBarcy0;
    long startBtnSize = 64;
    QRectF rect1(x0 + 1, y0 + 1, width0 - 2, height0 - 2);
    QPainterPath ppath1;
    size_t id = index.row();
    painter->setPen(QColor(125, 162, 206));

    // paint selected state
    if (option.state.testFlag(QStyle::State_Selected)) {
        ppath1.addRoundedRect(rect1, 2, 2);
        painter->fillPath(ppath1, QBrush(QColor(230, 230, 255)));
        ppath1.clear();
        ppath1.addRect(QRect(x0 + 1, y0 + 1, 10, height0 - 2));
        painter->fillPath(ppath1,QBrush(QColor("#5e5d53")));
    }

    // paint framework
    painter->setPen(QColor(Qt::black));
    painter->setFont(QFont("Arial", 8));
    painter->drawText(x0, y0, orderFrameWidth, height0, Qt::AlignCenter, QString::number(id + 1));
    painter->setPen(QPen(QColor(208, 208, 208), 1));
    painter->drawLine(x0 + orderFrameWidth, y0 + 5, x0 + orderFrameWidth, y0 + height0 - 5);
    painter->drawLine(x0, y0, x0 + width0, y0);
    painter->drawLine(x0, y0 + height0, x0 + width0, y0 + height0);

    // paint number text
    painter->setPen(QPen(Qt::black, 8));
    size_t iNumber = 0, nNumber = (pUnitList->at(id)).getNNumber(), tmpNumberVal;
    QString tmpNumText;
    QPixmap tmpNumPixmap;
    for (iNumber = 0; iNumber < nNumber; ++iNumber) {
        tmpNumberVal = (pUnitList->at(id)).getINumber(iNumber);
        tmpNumText = (pUnitList->at(id)).getINumberName(iNumber) + " " + QString::number(tmpNumberVal);
        tmpNumPixmap = QPixmap((pUnitList->at(id)).getINumberIcon(iNumber));
        painter->drawPixmap(runningBarx0 + iNumber * numberInfoLength, numberInfocy0 - 8, tmpNumPixmap);
        painter->drawText(runningBarx0 + iNumber * numberInfoLength + 20,numberInfocy0 - 20, numberInfoLength - 20, 40, Qt::AlignVCenter, tmpNumText);

    }

    // paint widget
    if((pUnitList->at(id)).state == RUN_PAUSE || (pUnitList->at(id)).state == RUN_RUNNING)
    {
        (*pWidgetList)[id]->startButton->setHidden(false);
        (*pWidgetList)[id]->startButton->move(startBtnx0, startBtncy0 - startBtnSize / 2);
        if ((pUnitList->at(id)).state == RUN_RUNNING) {
            (*pWidgetList)[id]->startButton->setStyleSheet(AnalyzeWidget::startButtonStyle2);
        } else {
            (*pWidgetList)[id]->startButton->setStyleSheet(AnalyzeWidget::startButtonStyle1);

        }
        if ((pUnitList->at(id)).state == RUN_PAUSE || (pUnitList->at(id)).state == RUN_RUNNING) {
            (*pWidgetList)[id]->runningBar->setHidden(false);
            (*pWidgetList)[id]->runningBar->resize(runningBarWid, AnalyzeWidget::runningBarHeight);
            (*pWidgetList)[id]->runningBar->move(runningBarx0, runningBarcy0 - AnalyzeWidget::runningBarHeight / 2);
            if((*pThreadList)[id]) {
                if((*pThreadList)[id]->info==ThreadRun::PCA_FASTPCA  ||
                   (*pThreadList)[id]->info==ThreadRun::PCA_PCA      ||
                   (*pThreadList)[id]->info==ThreadRun::STR_STRUCTURE||
                   (*pThreadList)[id]->info==ThreadRun::KIN_KINSHIP  ||
                   (*pThreadList)[id]->info==ThreadRun::BSA_NORMALBSA) {
                    (*pWidgetList)[id]->runningBar->setValue(int((*pThreadList)[id]->getDealedNum()));
                } else if((*pThreadList)[id]->info==ThreadRun::GLM_GEMMA || (*pThreadList)[id]->info==ThreadRun::MLM_GEMMA ||
                        (*pThreadList)[id]->info==ThreadRun::MLM_EMMAX) {
                    (*pWidgetList)[id]->runningBar->setValue(int(double((*pThreadList)[id]->getDealedNum()) / (pUnitList->at(id)).getINumber(1) * 100));
                }
            }
        } else {
            (*pWidgetList)[id]->runningBar->setHidden(true);
        }
    } else {
        (*pWidgetList)[id]->startButton->setHidden(true);
        (*pWidgetList)[id]->runningBar->setHidden(true);
    }

    int lenFinishedStr;
    QFont finishedFont("Arial", 16);
    QFontMetricsF finishedFontMetricsF(finishedFont);
    QString finishedStr;
    int higFinishedStr = finishedFontMetricsF.capHeight();
    int gapFinishedStr = 8;
    QPixmap mapFinished;
    if ((pUnitList->at(id)).state == RUN_FINISHED) {
        if ((*pThreadList)[id]->info == ThreadRun::PCA_FASTPCA) {
            mapFinished = QPixmap(":/icon/PCAIcon.png");
            finishedStr = "Fast PCA";
        } else if ((*pThreadList)[id]->info == ThreadRun::PCA_PCA) {
            mapFinished = QPixmap(":/icon/PCAIcon.png");
            finishedStr = "PCA";
        } else if ((*pThreadList)[id]->info == ThreadRun::STR_STRUCTURE) {
            mapFinished = QPixmap(":/icon/structureIcon.png");
            finishedStr = "STRUCTURE";
        } else if ((*pThreadList)[id]->info == ThreadRun::STR_FASTSTRUCTURE) {
            mapFinished = QPixmap(":/icon/structureIcon.png");
            finishedStr = "Fast Structure";
        } else if ((*pThreadList)[id]->info == ThreadRun::KIN_KINSHIP) {
            mapFinished = QPixmap(":/icon/KinshipIcon.png");
            finishedStr = "Kinship";
        } else if ((*pThreadList)[id]->info == ThreadRun::GLM_GEMMA) {
            mapFinished = QPixmap(":/icon/GWASIcon.png");
            finishedStr = "GLM";
        } else if ((*pThreadList)[id]->info == ThreadRun::MLM_GEMMA) {
            mapFinished = QPixmap(":/icon/GWASIcon.png");
            finishedStr = "GEMMA";
        } else if ((*pThreadList)[id]->info == ThreadRun::MLM_EMMAX) {
            mapFinished = QPixmap(":/icon/GWASIcon.png");
            finishedStr = "EMMAX";
        }
        lenFinishedStr = finishedFontMetricsF.horizontalAdvance(finishedStr);
        painter->setFont(finishedFont);
        painter->drawPixmap(startBtnx0 - lenFinishedStr - startBtnSize - 2 * gapFinishedStr,
                            startBtncy0 - startBtnSize / 2, mapFinished);
        painter->drawText(QPoint(startBtnx0 - lenFinishedStr - gapFinishedStr, startBtncy0 + higFinishedStr / 2),
                          finishedStr);
        painter->drawPixmap(startBtnx0, startBtncy0-startBtnSize / 2, QPixmap(":/icon/OKIcon.png"));
    }

    // restore
    painter->restore();
}


QSize AnalyzeItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QSize(option.rect.width(), 100);
}

void AnalyzeItemDelegate::setUnitList(QList<AnalyzeUnitInfo> *inUnitList)
{
    pUnitList = inUnitList;
}

void AnalyzeItemDelegate::setWidgetList(QList<AnalyzeWidget*> *inWidgetList)
{
    pWidgetList = inWidgetList;
}

void AnalyzeItemDelegate::setThreadList(QList<ThreadRun::BaseThread *> *inThreadList)
{
    pThreadList = inThreadList;
}

void AnalyzeItemDelegate::setProjectList(QList<FD::BaseProject *> *inProjectList)
{
    pProjectList = inProjectList;
}



AnalyzeListModel::AnalyzeListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

int AnalyzeListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    Q_UNUSED(parent);
    return pUnitList->size();
    // FIXME: Implement me!
}

QVariant AnalyzeListModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);
    if (!index.isValid()) return QVariant();
    if (index.row() < pUnitList->size()) return QVariant::fromValue(pUnitList->at(index.row()));
    // FIXME: Implement me!
    return QVariant();
}

bool AnalyzeListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!

        if (index.row()<pUnitList->size()) {
            (*pUnitList)[index.row()] = value.value<AnalyzeUnitInfo>();
            emit dataChanged(index, index, QVector<int>() << role);
            return true;
        }
    }
    return false;
}

bool AnalyzeListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    for (int i = 0; i < count; ++i) pUnitList->insert(row, AnalyzeUnitInfo());
    endInsertRows();
    return true;
}

bool AnalyzeListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row + count - 1 >= pUnitList->size()) return false;
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    for (int i = 0; i < count; ++i) pUnitList->removeAt(row);
    endRemoveRows();
    return true;
}

void AnalyzeListModel::setUnitList(QList<AnalyzeUnitInfo> *inUnitList)
{
    pUnitList = inUnitList;
}

MenuActionSet::MenuActionSet()
{
    removeItem = nullptr;
    loadFileGWAS = nullptr;
    loadFileBSA = nullptr;
    editPhe = nullptr;
    editCov = nullptr;
    editKin = nullptr;
    addNewItem = nullptr;
    removeAll = nullptr;
    saveOutput = nullptr;
    visualize = nullptr;
}

MenuActionSet::~MenuActionSet()
{
    if (removeItem) delete removeItem;
    if (loadFileGWAS) delete loadFileGWAS;
    if (loadFileBSA) delete loadFileBSA;
    if (editPhe) delete editPhe;
    if (editCov) delete editCov;
    if (editKin) delete editKin;
    if (addNewItem) delete addNewItem;
    if (removeAll) delete removeAll;
    if (saveOutput) delete saveOutput;
    if (visualize) delete visualize;
}

void MenuActionSet::init(QMenu *parent)
{
    if (!removeItem) removeItem = new QAction("Remove", parent);
    if (!loadFileGWAS) loadFileGWAS = new QAction("Load GWAS File", parent);
    if (!loadFileBSA) loadFileBSA = new QAction("Load BSA File", parent);
    if (!editPhe) editPhe = new QAction("Edit Phenotype Data", parent);
    if (!editCov) editCov = new QAction("Edit Covariate Data", parent);
    if (!editKin) editKin = new QAction("Edit Kinship Data", parent);
    if (!addNewItem) addNewItem = new QAction("Create New Project", parent);
    if (!removeAll) removeAll = new QAction("Remove All", parent);
    if (!saveOutput) saveOutput = new QAction("Save Result", parent);
    if (!visualize) visualize = new QAction("Result Visualization", parent);

    removeAll->setIcon(QIcon(":/icon/removeAll.png"));
    removeItem->setIcon(QIcon(":/icon/removeProjectIcon.png"));
    loadFileGWAS->setIcon(QIcon(":/icon/openIcon.png"));
    loadFileBSA->setIcon(QIcon(":/icon/openIcon.png"));
    addNewItem->setIcon(QIcon(":/icon/newProjectIcon.png"));
    saveOutput->setIcon(QIcon(":/icon/saveIcon.png"));
    editPhe->setIcon(QIcon(":/icon/editFile.png"));
    editCov->setIcon(QIcon(":/icon/editFile.png"));
    editKin->setIcon(QIcon(":/icon/editFile.png"));
    visualize->setIcon(QIcon(":/icon/visualizeIcon.png"));

    parent->addAction(loadFileGWAS);
    // parent->addAction(loadFileBSA);
    parent->addSeparator();
    parent->addAction(editPhe);
    parent->addAction(editCov);
    parent->addAction(editKin);
    parent->addSeparator();
    parent->addAction(visualize);
    parent->addSeparator();
    parent->addAction(removeItem);
    parent->addAction(removeAll);
    parent->addSeparator();
    parent->addAction(saveOutput);
    parent->addSeparator();
    parent->addAction(addNewItem);
}


void MenuActionSet::setRunDisabled()
{
    loadFileGWAS->setDisabled(true);
    loadFileBSA->setDisabled(true);
}

void MenuActionSet::setEnabled()
{
    removeItem->setEnabled(true);
    addNewItem->setEnabled(true);
    removeAll->setEnabled(true);
}

AnalyzeListViewMenu::AnalyzeListViewMenu(QWidget *parent)
    :QMenu(parent)
{
    pActions = new MenuActionSet;
    pActions->init(this);
}

void AnalyzeListViewMenu::connectAnalyzeListView(QListView *pView)
{
    AnalyzeListView* pAnalyzeView = dynamic_cast<AnalyzeListView*>(pView);
    connect(pActions->removeItem, &QAction::triggered,pAnalyzeView, &AnalyzeListView::removeCurrent);
    connect(pActions->loadFileGWAS, &QAction::triggered,pAnalyzeView, &AnalyzeListView::loadFileGWAS);
    connect(pActions->editPhe, &QAction::triggered,pAnalyzeView, &AnalyzeListView::editPhe);
    connect(pActions->editCov, &QAction::triggered,pAnalyzeView, &AnalyzeListView::editCov);
    connect(pActions->editKin, &QAction::triggered,pAnalyzeView, &AnalyzeListView::editKin);
    connect(pActions->removeAll, &QAction::triggered,pAnalyzeView, &AnalyzeListView::removeAll);
    connect(pActions->addNewItem, &QAction::triggered,pAnalyzeView, &AnalyzeListView::addNew);
    connect(pActions->saveOutput, &QAction::triggered,pAnalyzeView, &AnalyzeListView::saveCurrent);
    connect(pActions->visualize, &QAction::triggered,pAnalyzeView, &AnalyzeListView::startVisualization);

}

AnalyzeListViewMenu::~AnalyzeListViewMenu()
{
    delete pActions;
}

AnalyzeListView::AnalyzeListView(QWidget *parent):QListView(parent)
{
    pUnitList=new QList<AnalyzeUnitInfo>();
    pDelegate=new AnalyzeItemDelegate(this);
    pWidgetList=new QList<AnalyzeWidget*>();
    pThreadList=new QList<ThreadRun::BaseThread*>();
    pProjectList=new QList<FD::BaseProject*>();
    setItemDelegate(pDelegate);
    pDelegate->setUnitList(pUnitList);
    pDelegate->setWidgetList(pWidgetList);
    pDelegate->setThreadList(pThreadList);
    pDelegate->setProjectList(pProjectList);
    pModel = new AnalyzeListModel(this);
    pModel->setUnitList(pUnitList);
    setModel(pModel);
    setMinimumWidth(AnalyzeItemDelegate::minWidth);

    // Menu
    pMenu = new AnalyzeListViewMenu(this);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &AnalyzeListView::customContextMenuRequested, this, &AnalyzeListView::menuExec);
    pMenu->connectAnalyzeListView(this);

    connect(this, &AnalyzeListView::clicked, this, &AnalyzeListView::widgetCliked);
}

AnalyzeListView::~AnalyzeListView()
{
    removeAll(false);
    if (pUnitList) delete pUnitList;
    if (pWidgetList) delete pWidgetList;
    if (pThreadList) delete pThreadList;
    if (pProjectList) delete pProjectList;
}

void AnalyzeListView::closeEvent(QCloseEvent *e)
{
    emit windowsClose();
    e->accept();
}

bool AnalyzeListView::pro_loadFileAt(int id, FD::ProjectInfo info)
{
    int currentCount = pModel->rowCount();
    if (id < 0 || id >= currentCount) return false;
    if (pProjectList->at(id) && !(((*pUnitList)[id]).state == RUN_PAUSE || ((*pUnitList)[id]).state == RUN_RUNNING)) {
           if(QMessageBox::question(this, "Is replace?", "The project has been initiated.Is replace?",
                QMessageBox::Yes, QMessageBox::No) == QMessageBox::No) return false;
    }
    if (((*pUnitList)[id]).state == RUN_PAUSE || ((*pUnitList)[id]).state == RUN_RUNNING) {
        if(QMessageBox::warning(this, "remove", "This project is incomplete. Will you continue?",
            QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel) return false;
    }
    if (((*pUnitList)[id]).state != RUN_UNKNOWN) {
        pro_clearAt(id);
    }
    if (info == FD::PROJECT_GWAS) {
        LoadGWASFileWidget* loadWidget = new LoadGWASFileWidget(&((*pUnitList)[id]), pProjectList, id);
        loadWidget->show();
        connect(loadWidget, &LoadGWASFileWidget::loadOK, this, &AnalyzeListView::projectChanged);
    } else if (info == FD::PROJECT_BSA) {
        LoadBSAFileWidget* loadWidget = new LoadBSAFileWidget(&((*pUnitList)[id]), pProjectList, id);
        // loadWidget->setAttribute(Qt::WA_ShowModal,true);
        loadWidget->show();
        connect(loadWidget, &LoadBSAFileWidget::loadOK, this, &AnalyzeListView::projectChanged);
    }
    return true;
}

bool AnalyzeListView::pro_loadFileAt(int id, FD::BaseProjectFilesSet inSet)
{
    int currentCount = pModel->rowCount();
    if (id < 0 || id >= currentCount) return false;
    if (pProjectList->at(id) && !(((*pUnitList)[id]).state == RUN_PAUSE || ((*pUnitList)[id]).state == RUN_RUNNING)) {
        if(QMessageBox::question(this, "Is replace?", "The project has been initiated.Is replace?",
            QMessageBox::Yes, QMessageBox::No) == QMessageBox::No) return false;
    }
    if (((*pUnitList)[id]).state == RUN_PAUSE || ((*pUnitList)[id]).state == RUN_RUNNING) {
        if (QMessageBox::warning(this, "remove", "This project is incomplete. Will you continue?",
            QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel) return false;
    }
    if (((*pUnitList)[id]).state != RUN_UNKNOWN) {
        pro_clearAt(id);
    }
    FD::ProjectInfo info = inSet.getProjectInfo();
    if (info == FD::PROJECT_GWAS) {
        LoadGWASFileWidget* loadWidget = new LoadGWASFileWidget(&((*pUnitList)[id]), pProjectList, id);
        loadWidget->setAttribute(Qt::WA_ShowModal, true);
        loadWidget->setFileSet(&inSet);
        loadWidget->show();
        connect(loadWidget,&LoadGWASFileWidget::loadOK, this, &AnalyzeListView::projectChanged);
    } else if (info == FD::PROJECT_BSA) {
        LoadBSAFileWidget* loadWidget = new LoadBSAFileWidget(&((*pUnitList)[id]), pProjectList, id);
        loadWidget->setAttribute(Qt::WA_ShowModal, true);
        loadWidget->show();
        connect(loadWidget, &LoadBSAFileWidget::loadOK, this, &AnalyzeListView::projectChanged);
    }
    return true;
}

bool AnalyzeListView::pro_addNew()
{
    int currentCount = pModel->rowCount();
    pModel->insertRows(currentCount, 1);
    pModel->setData(pModel->index(currentCount, 0), QVariant::fromValue(AnalyzeUnitInfo()));
    pWidgetList->push_back(nullptr);
    (*pWidgetList)[currentCount] = new AnalyzeWidget(this);
    (*pWidgetList)[currentCount]->setID(currentCount);
    (*pWidgetList)[currentCount]->init();
    (*pWidgetList)[currentCount]->setAnalyzeUnit(&((*pUnitList)[currentCount]));
    pThreadList->push_back(nullptr);
    pProjectList->push_back(nullptr);
    emit projectInserted(currentCount);
    emit projectAdd();
    return true;
}

bool AnalyzeListView::pro_clearAt(int row)
{
    (*pUnitList)[row].clear();
    (*pWidgetList)[row]->clear();
    (*pWidgetList)[row]->setAnalyzeUnit(&((*pUnitList)[row]));
    if ((*pThreadList)[row]) ThreadRun::qDeleteThreadRun((*pThreadList)[row]);
    (*pThreadList)[row] = nullptr;
    if (pProjectList->at(row)) FD::deleteProject(pProjectList->at(row));
    (*pProjectList)[row] = nullptr;
    return true;
}

bool AnalyzeListView::pro_removeAt(int row)
{
    int currentCount;
    ThreadRun::qDeleteThreadRun(pThreadList->at(row));
    if (pProjectList->at(row)) FD::deleteProject(pProjectList->at(row));
    pProjectList->removeAt(row);
    pThreadList->removeAt(row);
    pModel->removeRows(row, 1);
    delete pWidgetList->at(row);
    pWidgetList->removeAt(row);
    currentCount = pModel->rowCount();
    for (int i = row; i < currentCount; ++i) {
        (*pWidgetList)[i]->setID(i);
    }
    emit projectRemoved(row);
    pro_widgetCliked(currentIndex());
    return true;
}

bool AnalyzeListView::pro_stopAt(int row)
{
    int currentCount = pModel->rowCount();
    if (row < 0 || row >= currentCount) return false;
    if (pUnitList->at(row).state == RUN_RUNNING || pUnitList->at(row).state == RUN_PAUSE) {
        if (QMessageBox::warning(this, "stop", "This project is incomplete. Will you continue?",
            QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel) return false;
        ThreadRun::qDeleteThreadRun(pThreadList->at(row));
        (*pThreadList)[row] = nullptr;
        (*pUnitList)[row].state = RUN_UNHANDLED;
        emit projectSelectedState(RUN_UNHANDLED);
        return true;
    }
    return false;
}

bool AnalyzeListView::pro_AnalyzeAt(int row, ThreadRun::AnalyzeClass info, int nThread)
{
    FD::GWASProject* tproject = nullptr;
    FD::BSAProject*  bproject = nullptr;
    if (((*pUnitList)[row]).state == RUN_UNKNOWN || ((*pUnitList)[row]).state == RUN_DEFAULT) {
        QMessageBox::critical(this, "error", "This project has not been initiatied.");
        return false;
    }
    if (((*pUnitList)[row]).state == RUN_PAUSE || ((*pUnitList)[row]).state == RUN_RUNNING) {
        QMessageBox::critical(this, "error", "This project is being analyzed.");
        return false;
    }
    if (((*pUnitList)[row]).state == RUN_FINISHED) {
        if (QMessageBox::warning(this, "warning","This project will be restarted. Will you continue?",
            QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel) return false;
    }
    if (pThreadList->at(row)) {
        ThreadRun::qDeleteThreadRun(pThreadList->at(row));
        (*pThreadList)[row] = nullptr;
    }

    if ((*pProjectList)[row]->proInfo == FD::PROJECT_GWAS) {
        tproject = dynamic_cast<FD::GWASProject*>((*pProjectList)[row]);
    } else if ((*pProjectList)[row]->proInfo == FD::PROJECT_BSA) {
        bproject = dynamic_cast<FD::BSAProject*>((*pProjectList)[row]);
    } else {
        QMessageBox::critical(this, "error", "The type of project error");
        return false;
    }

    ThreadRun::BaseThread* tmp = nullptr;
    if (row < 0 || row >= pUnitList->size()) return false;

    connect((*pWidgetList)[row], &AnalyzeWidget::signalProjectFinished,
            this, &AnalyzeListView::receiveProjectFinished);
    connect((*pWidgetList)[row], &AnalyzeWidget::signalProjectStateChanged,
            this, &AnalyzeListView::receiveProjectStateChanged);
    connect((*pWidgetList)[row], &AnalyzeWidget::signalProjectLogUpdate,
            this, &AnalyzeListView::receiveProjectLogUpdate);

    if (info == ThreadRun::MLM_EMMAX) {
        if (!tproject->isKinValid) {
            QMessageBox::critical(this, "error", "There is no kinship data");
            return false;
        }
        tmp = new ThreadRun::QEMMAXThread(tproject);
        connect(dynamic_cast<ThreadRun::QEMMAXThread*>(tmp),
                &ThreadRun::QEMMAXThread::taskFinished,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectFinished);
        connect(dynamic_cast<ThreadRun::QEMMAXThread*>(tmp),
                &ThreadRun::QEMMAXThread::logUpdate,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectLogUpdate);
        if (!dynamic_cast<ThreadRun::QEMMAXThread*>(tmp)->makeThreadMuti(nThread)) {
            QMessageBox::critical(this, "error", "Thread start failed.");
            delete dynamic_cast<ThreadRun::QEMMAXThread*>(tmp);
            return false;
        }
        (*pThreadList)[row] = tmp;
    } else if (info==ThreadRun::MLM_GEMMA) {
        if (!tproject->isKinValid) {
            QMessageBox::critical(this, "error", "There is no kinship data");
            return false;
        }
        tmp = new ThreadRun::QGEMMAThread(tproject);
        connect(dynamic_cast<ThreadRun::QGEMMAThread*>(tmp),
                &ThreadRun::QGEMMAThread::taskFinished,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectFinished);
        connect(dynamic_cast<ThreadRun::QGEMMAThread*>(tmp),
                &ThreadRun::QGEMMAThread::logUpdate,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectLogUpdate);
        if (!dynamic_cast<ThreadRun::QGEMMAThread*>(tmp)->makeThreadMuti(nThread)) {
            QMessageBox::critical(this, "error", "Thread start failed.");
            delete dynamic_cast<ThreadRun::QGEMMAThread*>(tmp);
            return false;
        }
        (*pThreadList)[row] = tmp;
    } else if (info == ThreadRun::GLM_GEMMA) {
        tmp = new ThreadRun::QGLMThread(tproject);
        connect(dynamic_cast<ThreadRun::QGLMThread*>(tmp),
                &ThreadRun::QGLMThread::taskFinished,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectFinished);
        connect(dynamic_cast<ThreadRun::QGLMThread*>(tmp),
                &ThreadRun::QGLMThread::logUpdate,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectLogUpdate);
        if (!dynamic_cast<ThreadRun::QGLMThread*>(tmp)->makeThreadMuti(nThread)) {
            QMessageBox::critical(this,"error","Thread start failed.");
            delete dynamic_cast<ThreadRun::QGLMThread*>(tmp);
            return false;
        }
        (*pThreadList)[row] = tmp;
    } else if (info == ThreadRun::PCA_FASTPCA) {
        tmp=new ThreadRun::QFastPCAThread(tproject);
        connect(dynamic_cast<ThreadRun::QFastPCAThread*>(tmp),
                &ThreadRun::QFastPCAThread::taskFinished,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectFinished);
        connect(dynamic_cast<ThreadRun::QFastPCAThread*>(tmp),
                &ThreadRun::QFastPCAThread::logUpdate,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectLogUpdate);
        if (!tmp->makeThread()) {
            QMessageBox::critical(this, "error", "Thread start failed.");
            delete dynamic_cast<ThreadRun::QFastPCAThread*>(tmp);
            return false;
        }
        (*pThreadList)[row] = tmp;
    } else if (info == ThreadRun::PCA_PCA) {
        tmp = new ThreadRun::QPCAThread(tproject);
        connect(dynamic_cast<ThreadRun::QPCAThread*>(tmp),
                &ThreadRun::QPCAThread::taskFinished,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectFinished);
        connect(dynamic_cast<ThreadRun::QPCAThread*>(tmp),
                &ThreadRun::QPCAThread::logUpdate,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectLogUpdate);
        if (!tmp->makeThread()) {
            QMessageBox::critical(this, "error", "Thread start failed.");
            delete dynamic_cast<ThreadRun::QPCAThread*>(tmp);
            return false;
        }
        (*pThreadList)[row] = tmp;
    } else if (info == ThreadRun::STR_STRUCTURE) {
        tmp=new ThreadRun::QStructureThread(tproject);
        connect(dynamic_cast<ThreadRun::QStructureThread*>(tmp),
                &ThreadRun::QStructureThread::taskFinished,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectFinished);
        connect(dynamic_cast<ThreadRun::QStructureThread*>(tmp),
                &ThreadRun::QStructureThread::logUpdate,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectLogUpdate);
        dynamic_cast<ThreadRun::QStructureThread*>(tmp)->setPara(struPara);
        if (!tmp->makeThread()) {
            QMessageBox::critical(this, "error", "Thread start failed.");
            delete dynamic_cast<ThreadRun::QStructureThread*>(tmp);
            return false;
        }
        (*pThreadList)[row] = tmp;
    } else if (info==ThreadRun::STR_FASTSTRUCTURE) {
        tmp = new ThreadRun::QFastStructureThread(tproject);
        connect(dynamic_cast<ThreadRun::QFastStructureThread*>(tmp),
                &ThreadRun::QFastStructureThread::taskFinished,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectFinished);
        connect(dynamic_cast<ThreadRun::QFastStructureThread*>(tmp),
                &ThreadRun::QFastStructureThread::logUpdate,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectLogUpdate);
        dynamic_cast<ThreadRun::QFastStructureThread*>(tmp)->setNPopulation(fastStruPara.nPop);
        if (!tmp->makeThread()) {
            QMessageBox::critical(this, "error", "Thread start failed.");
            delete dynamic_cast<ThreadRun::QFastStructureThread*>(tmp);
            return false;
        }
        (*pThreadList)[row] = tmp;
    } else if (info==ThreadRun::KIN_KINSHIP) {
        tmp = new ThreadRun::QKinshipThread(tproject, kinshipPara.mode);
        connect(dynamic_cast<ThreadRun::QKinshipThread*>(tmp),
                &ThreadRun::QKinshipThread::taskFinished,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectFinished);
        connect(dynamic_cast<ThreadRun::QKinshipThread*>(tmp),
                &ThreadRun::QKinshipThread::logUpdate,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectLogUpdate);
        if (!tmp->makeThread()) {
            QMessageBox::critical(this, "error", "Thread start failed.");
            delete dynamic_cast<ThreadRun::QKinshipThread*>(tmp);
            return false;
        }
        (*pThreadList)[row] = tmp;
    } else if (info==ThreadRun::BSA_NORMALBSA) {
        tmp = new ThreadRun::QNormalBSAThread(bproject);
        connect(dynamic_cast<ThreadRun::QNormalBSAThread*>(tmp),
                &ThreadRun::QNormalBSAThread::taskFinished,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectFinished);
        connect(dynamic_cast<ThreadRun::QNormalBSAThread*>(tmp),
                &ThreadRun::QNormalBSAThread::logUpdate,
                (*pWidgetList)[row],
                &AnalyzeWidget::projectLogUpdate);
        if (!tmp->makeThread()) {
            QMessageBox::critical(this, "error", "Thread start failed.");
            delete dynamic_cast<ThreadRun::QNormalBSAThread*>(tmp);
            return false;
        }
        (*pThreadList)[row] = tmp;
    }
    else return false;


    (*pUnitList)[row].state = RUN_RUNNING;
    (*pWidgetList)[row]->setThread(tmp);
    emit projectSelectedState(RUN_RUNNING);
    return true;
}

bool AnalyzeListView::pro_widgetCliked(const QModelIndex &index)
{
    if (!index.isValid()) {
        if (pModel->rowCount() == 0) {
            emit projectSelectedState(RUN_UNKNOWN);
        }
        return false;
    }
    int row = index.row();
    if (row < 0 || row >= pUnitList->size()) return false;
    emit projectSelected(row);
    emit projectSelectedState((*pUnitList)[row].state);
    return true;
}

bool AnalyzeListView::pro_saveResultAt(int row)
{
    int currentCount = pModel->rowCount();
    if (row < 0 || row >= currentCount || !(pThreadList->at(row))) {
        QMessageBox::warning(this, "write null", "There is no result in the project.");
        return false;
    }
    if (!(pThreadList->at(row))->finished()) {
        QMessageBox::warning(this, "write null", "The project is incompleted.");
        return false;
    }
    QString outFile = QFileDialog::getSaveFileName(this);
    if (outFile.isEmpty()) return false;
    if (!ThreadRun::qSaveOutput((*pThreadList)[row], outFile.toLatin1().data())) {
        QMessageBox::warning(this, "write failed", "The output writing failed.");
    }
    return true;
}

bool AnalyzeListView::pro_editPheAt(int row)
{
    int currentCount = pModel->rowCount();
    FD::GWASProject* tproject = nullptr;
    if (row < 0 || row >= currentCount) return false;
    if (((*pUnitList)[row]).state == RUN_UNKNOWN || ((*pUnitList)[row]).state == RUN_DEFAULT) {
        QMessageBox::critical(this, "error", "This project has not been initiatied.");
        return false;
    }
    if ((*pProjectList)[row]->proInfo != FD::PROJECT_GWAS) {
        QMessageBox::critical(this, "error", "This project is not GWAS project.");
        return false;
    }
    if (((*pUnitList)[row]).state == RUN_PAUSE || ((*pUnitList)[row]).state == RUN_RUNNING) {
        QMessageBox::critical(this, "error", "This project is being analyzed.");
        return false;
    }
    tproject = dynamic_cast<FD::GWASProject*>((*pProjectList)[row]);
    DataInteractTableWidget* pWidget = new DataInteractTableWidget(nullptr, true);
    pWidget->load(tproject->phe.data,tproject->phe.getNRow(), 1);
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
    return true;
}

bool AnalyzeListView::pro_editCovAt(int row)
{
    int currentCount = pModel->rowCount();
    FD::GWASProject* tproject = nullptr;
    if (row < 0 || row >= currentCount) return false;
    if (((*pUnitList)[row]).state == RUN_UNKNOWN || ((*pUnitList)[row]).state == RUN_DEFAULT) {
        QMessageBox::critical(this, "error", "This project has not been initiatied.");
        return false;
    }
    if ((*pProjectList)[row]->proInfo != FD::PROJECT_GWAS) {
        QMessageBox::critical(this, "error", "This project is not GWAS project.");
        return false;
    }
    if (((*pUnitList)[row]).state == RUN_PAUSE || ((*pUnitList)[row]).state == RUN_RUNNING) {
        QMessageBox::critical(this, "error", "This project is being analyzed.");
        return false;
    }
    tproject = dynamic_cast<FD::GWASProject*>((*pProjectList)[row]);
    DataInteractTableWidget* pWidget = new DataInteractTableWidget(nullptr, true);
    pWidget->load(tproject->cov.data, tproject->phe.getNRow(), tproject->cov.getNCol());
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
    return true;
}

bool AnalyzeListView::pro_editKinAt(int row)
{
    int currentCount = pModel->rowCount();
    FD::GWASProject* tproject = nullptr;
    if (row < 0 || row >= currentCount) return false;
    if (((*pUnitList)[row]).state == RUN_UNKNOWN || ((*pUnitList)[row]).state == RUN_DEFAULT) {
        QMessageBox::critical(this, "error", "This project has not been initiatied.");
        return false;
    }
    if ((*pProjectList)[row]->proInfo != FD::PROJECT_GWAS) {
        QMessageBox::critical(this, "error", "This project is not GWAS project.");
        return false;
    }
    tproject = dynamic_cast<FD::GWASProject*>((*pProjectList)[row]);
    if (tproject && !tproject->isKinValid) {
        QMessageBox::critical(this, "error", "This project has not kinship data.");
        return false;
    }
    if (((*pUnitList)[row]).state == RUN_PAUSE || ((*pUnitList)[row]).state == RUN_RUNNING) {
        QMessageBox::critical(this, "error", "This project is being analyzed.");
        return false;
    }
    DataInteractTableWidget* pWidget = new DataInteractTableWidget(nullptr, true);
    pWidget->load(tproject->kin.data, tproject->phe.getNRow(), tproject->phe.getNRow());
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
    return true;
}

bool AnalyzeListView::pro_startVisualizationAt(int row)
{
    FD::BIMLogPData* bimData = nullptr;
    PreVisualizeWidget* pPreVisualWidget = nullptr;
    PrePCAVisualizeWidget* pPrePCAVisualWidget = nullptr;
    ThreadRun::QPCAThread* pPCAThread = nullptr;
    PreStrucVisualizeWidget* pPreStrucVisualizeWidget = nullptr;
    ThreadRun::QStructureThread* pStructureThread = nullptr;
    ThreadRun::QFastStructureThread* pFastStructureThread = nullptr;
    PreKinshipVisualizeWidget* pPreKinshipVisualizeWidget = nullptr;
    ThreadRun::QKinshipThread* pKinshipThread = nullptr;
    PreBSAVisualizeWidget* pPreBSAVisualizeWidget = nullptr;
    ThreadRun::QNormalBSAThread* pNormalBSAThread = nullptr;
    int currentCount = pModel->rowCount();
    if (row < 0 || row >= pUnitList->size()) return false;
    if (!((*pThreadList)[row])) {
        QMessageBox::critical(this, "incomplete", "This project is incomplete.");
        return false;
    }
    ThreadRun::AnalyzeClass analyzeClass = (*pThreadList)[row]->info;
    if (row < 0 || row >= currentCount) return false;
    if (pUnitList->at(row).state == RUN_RUNNING || pUnitList->at(row).state == RUN_PAUSE)
    {
        QMessageBox::critical(this, "incomplete", "This project is incomplete.");
        return false;
    }
    if (pUnitList->at(row).state != RUN_FINISHED) {
        QMessageBox::critical(this, "incomplete", "This project is incomplete.");
        return false;
    }
    if (analyzeClass == ThreadRun::GLM_GEMMA || analyzeClass == ThreadRun::MLM_EMMAX ||
        analyzeClass == ThreadRun::MLM_GEMMA) {
        bimData = ThreadRun::getBIMInfo(pThreadList->at(row));
        pPreVisualWidget = new PreVisualizeWidget(bimData);
        pPreVisualWidget->setAttribute(Qt::WA_ShowModal, true);
        connect(this, &AnalyzeListView::windowsClose, pPreVisualWidget, &PreVisualizeWidget::close);
        pPreVisualWidget->show();
    } else if (analyzeClass==ThreadRun::PCA_FASTPCA|| analyzeClass == ThreadRun::PCA_PCA) {
        pPCAThread = dynamic_cast<ThreadRun::QPCAThread*>((*pThreadList)[row]);
        pPrePCAVisualWidget = new PrePCAVisualizeWidget(&(pPCAThread->PCs));
        pPrePCAVisualWidget->setAttribute(Qt::WA_ShowModal, true);
        pPrePCAVisualWidget->setVarRatio(pPCAThread->EVal(0) / pPCAThread->sumEVal,
                                         pPCAThread->EVal(1) / pPCAThread->sumEVal,
                                         pPCAThread->EVal(2) / pPCAThread->sumEVal);
        connect(this, &AnalyzeListView::windowsClose, pPrePCAVisualWidget, &PrePCAVisualizeWidget::close);
        pPrePCAVisualWidget->show();
    } else if (analyzeClass == ThreadRun::STR_STRUCTURE) {
        pStructureThread = dynamic_cast<ThreadRun::QStructureThread*>((*pThreadList)[row]);
        pPreStrucVisualizeWidget = new PreStrucVisualizeWidget(pStructureThread->out);
        pPreStrucVisualizeWidget->setAttribute(Qt::WA_ShowModal, true);
        connect(this, &AnalyzeListView::windowsClose, pPreStrucVisualizeWidget, &PreStrucVisualizeWidget::close);
        pPreStrucVisualizeWidget->show();
    } else if (analyzeClass == ThreadRun::STR_FASTSTRUCTURE) {
        pFastStructureThread = dynamic_cast<ThreadRun::QFastStructureThread*>((*pThreadList)[row]);
        pPreStrucVisualizeWidget = new PreStrucVisualizeWidget(pFastStructureThread->out);
        pPreStrucVisualizeWidget->setAttribute(Qt::WA_ShowModal, true);
        connect(this, &AnalyzeListView::windowsClose, pPreStrucVisualizeWidget, &PreStrucVisualizeWidget::close);
        pPreStrucVisualizeWidget->show();
    } else if (analyzeClass == ThreadRun::KIN_KINSHIP) {
        pKinshipThread = dynamic_cast<ThreadRun::QKinshipThread*>((*pThreadList)[row]);
        pPreKinshipVisualizeWidget = new PreKinshipVisualizeWidget(pKinshipThread->outKin);
        pPreKinshipVisualizeWidget -> setAttribute(Qt::WA_ShowModal, true);
        connect(this,&AnalyzeListView::windowsClose, pPreKinshipVisualizeWidget, &PreKinshipVisualizeWidget::close);
        pPreKinshipVisualizeWidget -> show();
    } else if (analyzeClass==ThreadRun::BSA_NORMALBSA) {
        pNormalBSAThread = dynamic_cast<ThreadRun::QNormalBSAThread*>((*pThreadList)[row]);
        pPreBSAVisualizeWidget = new PreBSAVisualizeWidget(pNormalBSAThread);
        pPreBSAVisualizeWidget -> setAttribute(Qt::WA_ShowModal, true);
        connect(this,&AnalyzeListView::windowsClose, pPreBSAVisualizeWidget, &PreBSAVisualizeWidget::close);
        pPreBSAVisualizeWidget -> show();
    }
    return true;
}

bool AnalyzeListView::clearAt(int row)
{
    return pro_clearAt(row);
}

bool AnalyzeListView::addNew()
{
    return pro_addNew();
}

bool AnalyzeListView::removeOne(int row)
{
    int currentCount = pModel->rowCount();
    if (row<0||row>=currentCount) return false;
    if ((pUnitList->at(row).state == RUN_RUNNING || pUnitList->at(row).state == RUN_PAUSE))
    {
        if(QMessageBox::warning(this, "remove", "This project is incomplete. Will you continue?",
            QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel) return false;
    }
    return pro_removeAt(row);
}

bool AnalyzeListView::stopOne(int row)
{
    return pro_stopAt(row);
}

bool AnalyzeListView::startLoad(int row, FD::ProjectInfo info)
{
    return pro_loadFileAt(row,info);
}

void AnalyzeListView::runEMMAX()
{
    QModelIndex currentID = currentIndex();
    if (!currentID.isValid()) {
        QMessageBox::critical(this, "warning", "the project is incorrect.");
        return ;
    }
    int row = currentID.row();
    if (!(*pProjectList)[row]) {
        QMessageBox::critical(this, "error", "This project is null.");
        return;
    }
    if ((*pProjectList)[row]->proInfo != FD::PROJECT_GWAS) {
        QMessageBox::critical(this, "error", "This project is not GWAS project.");
        return;
    }
    SetGWASParamWidget* pWidget = new SetGWASParamWidget(row, ThreadRun::MLM_EMMAX);
    connect(pWidget, &SetGWASParamWidget::startGWAS, this, &AnalyzeListView::startGWAS);
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
}

void AnalyzeListView::runGEMMA()
{
    QModelIndex currentID = currentIndex();
    if (!currentID.isValid()) {
        QMessageBox::critical(this, "warning", "the project is incorrect.");
        return ;
    }
    int row = currentID.row();
    if (!(*pProjectList)[row]) {
        QMessageBox::critical(this, "error", "This project is null.");
        return;
    }
    if ((*pProjectList)[row]->proInfo != FD::PROJECT_GWAS) {
        QMessageBox::critical(this, "error", "This project is not GWAS project.");
        return;
    }
    SetGWASParamWidget* pWidget = new SetGWASParamWidget(row, ThreadRun::MLM_GEMMA);
    connect(pWidget, &SetGWASParamWidget::startGWAS, this, &AnalyzeListView::startGWAS);
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
}

void AnalyzeListView::runGLM()
{
    QModelIndex currentID = currentIndex();
    if(!currentID.isValid()) {
        QMessageBox::critical(this, "warning", "the project is incorrect.");
        return ;
    }
    int row = currentID.row();
    if (!(*pProjectList)[row]) {
        QMessageBox::critical(this, "error", "This project is null.");
        return;
    }
    if ((*pProjectList)[row]->proInfo != FD::PROJECT_GWAS) {
        QMessageBox::critical(this, "error", "This project is not GWAS project.");
        return;
    }
    SetGWASParamWidget* pWidget = new SetGWASParamWidget(row, ThreadRun::GLM_GEMMA);
    connect(pWidget, &SetGWASParamWidget::startGWAS, this, &AnalyzeListView::startGWAS);
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
}

void AnalyzeListView::runPCA()
{
    QModelIndex currentID=currentIndex();
    if (!currentID.isValid()) {
        QMessageBox::critical(this, "warning", "the project is incorrect.");
        return ;
    }
    int row = currentID.row();
    if (!(*pProjectList)[row]) {
        QMessageBox::critical(this, "error", "This project is null.");
        return;
    }
    if ((*pProjectList)[row]->proInfo != FD::PROJECT_GWAS) {
        QMessageBox::critical(this, "error", "This project is not GWAS project.");
        return;
    }
    pro_AnalyzeAt(currentID.row(), ThreadRun::PCA_PCA, 1);
}

void AnalyzeListView::runFastPCA()
{
    QModelIndex currentID = currentIndex();
    if (!currentID.isValid())
    {
        QMessageBox::critical(this, "warning", "the project is incorrect.");
        return ;
    }
    int row = currentID.row();
    if (!(*pProjectList)[row]) {
        QMessageBox::critical(this, "error", "This project is null.");
        return;
    }
    if ((*pProjectList)[row]->proInfo != FD::PROJECT_GWAS) {
        QMessageBox::critical(this, "error", "This project is not GWAS project.");
        return;
    }
    pro_AnalyzeAt(currentID.row(), ThreadRun::PCA_FASTPCA, 1);
}

void AnalyzeListView::runStructure()
{
    QModelIndex currentID = currentIndex();
    if (!currentID.isValid())
    {
        QMessageBox::critical(this, "warning", "the project is incorrect.");
        return ;
    }
    int row = currentID.row();
    if (!(*pProjectList)[row]) {
        QMessageBox::critical(this, "error", "This project is null.");
        return;
    }
    if ((*pProjectList)[row]->proInfo != FD::PROJECT_GWAS) {
        QMessageBox::critical(this, "error", "This project is not GWAS project.");
        return;
    }
    SetStructureParamWidget* pWidget = new SetStructureParamWidget(currentID.row(), &struPara);
    connect(pWidget, &SetStructureParamWidget::startStructure,
            this, &AnalyzeListView::startStructure);
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
}

void AnalyzeListView::runFastStructure()
{
    QModelIndex currentID = currentIndex();
    if (!currentID.isValid())
    {
        QMessageBox::critical(this, "warning", "the project is incorrect.");
        return ;
    }
    int row = currentID.row();
    if (!(*pProjectList)[row]) {
        QMessageBox::critical(this, "error", "This project is null.");
        return;
    }
    if ((*pProjectList)[row]->proInfo != FD::PROJECT_GWAS) {
        QMessageBox::critical(this, "error", "This project is not GWAS project.");
        return;
    }
    SetFastStructureParamWidget* pWidget = new SetFastStructureParamWidget(currentID.row(), &fastStruPara);
    connect(pWidget, &SetFastStructureParamWidget::startFastStructure,
            this, &AnalyzeListView::startFastStructure);
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
}

void AnalyzeListView::runKinship()
{
    QModelIndex currentID = currentIndex();
    if (!currentID.isValid())
    {
        QMessageBox::critical(this, "warning", "the project is incorrect.");
        return ;
    }
    int row = currentID.row();
    if (!(*pProjectList)[row]) {
        QMessageBox::critical(this, "error", "This project is null.");
        return;
    }
    if ((*pProjectList)[row]->proInfo != FD::PROJECT_GWAS) {
        QMessageBox::critical(this, "error", "This project is not GWAS project.");
        return;
    }
    FD::GWASProject* project = dynamic_cast<FD::GWASProject*>((*pProjectList)[row]);
    if (project->isPolyploid) {
        kinshipPara.nPloid = project->pData.nPloid;
    } else {
        kinshipPara.nPloid = 2;
    }
    SetKinshipParamWidget* pWidget = new SetKinshipParamWidget(currentID.row(), &kinshipPara);
    connect(pWidget, &SetKinshipParamWidget::startKinship,
            this, &AnalyzeListView::startKinship);
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
}

void AnalyzeListView::runNormalBSA()
{
    QModelIndex currentID = currentIndex();
    if (!currentID.isValid())
    {
        QMessageBox::critical(this, "warning", "the project is incorrect.");
        return ;
    }
    int row = currentID.row();
    if (!(*pProjectList)[row]) {
        QMessageBox::critical(this, "error", "This project is null.");
        return;
    }
    if ((*pProjectList)[row]->proInfo != FD::PROJECT_BSA) {
        QMessageBox::critical(this, "error", "This project is not BSA project.");
        return;
    }
    FD::BSAProject* tproject = dynamic_cast<FD::BSAProject*>((*pProjectList)[row]);
    SetNormalBSAParamWidget* pWidget = new SetNormalBSAParamWidget(currentID.row(),tproject);
    connect(pWidget, &SetNormalBSAParamWidget::startNormalBSA,
            this, &AnalyzeListView::startNormalBSA);
    pWidget->setAttribute(Qt::WA_ShowModal, true);
    pWidget->show();
}

void AnalyzeListView::removeCurrent()
{
    QModelIndex currentID = currentIndex();
    if (!currentID.isValid()) return ;
    int row = currentID.row(), len = pModel->rowCount();
    if (currentID.row() < len - 1) {
        pro_widgetCliked(pModel->index(row, 0));
    } else if (currentID.row() > 0) {
        pro_widgetCliked(pModel->index(row - 1, 0));
    }
    pro_removeAt(row);
}

void AnalyzeListView::stopCurrent()
{
    QModelIndex currentID = currentIndex();
    if (!currentID.isValid()) return ;
    int row = currentID.row();
    pro_stopAt(row);
}

void AnalyzeListView::removeAll(bool isWarning)
{
    int currentCount = pModel->rowCount(),row;
    if (!currentCount) return;
    if (isWarning){
        if (QMessageBox::warning(this, "remove all", "All project will be removed. Will you continue?",
            QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel) return ;
    }
    for (row = currentCount - 1; row >= 0; --row) {
        pro_removeAt(row);
    }
    emit allRemoved();
}

void AnalyzeListView::saveCurrent()
{
    QModelIndex currentID = currentIndex();
    pro_saveResultAt(currentID.row());
}

void AnalyzeListView::loadFile(FD::ProjectInfo info)
{
    int currentCount = pModel->rowCount();
    QModelIndex currentID = currentIndex();
    if (currentID.row() < 0 || currentID.row() >= currentCount) return;
    pro_loadFileAt(currentID.row(), info);
}

void AnalyzeListView::loadFileAt(int id, FD::ProjectInfo info)
{
    pro_loadFileAt(id, info);
}

void AnalyzeListView::loadFile(FD::BaseProjectFilesSet inSet)
{
    int currentCount = pModel->rowCount();
    pro_addNew();
    pro_loadFileAt(currentCount, inSet);
}

void AnalyzeListView::loadFileGWAS()
{
    loadFile(FD::PROJECT_GWAS);
}

void AnalyzeListView::loadFileBSA()
{
    loadFile(FD::PROJECT_BSA);
}

void AnalyzeListView::editPhe()
{
    QModelIndex currentID = currentIndex();
    pro_editPheAt(currentID.row());
}

void AnalyzeListView::editCov()
{
    QModelIndex currentID = currentIndex();
    pro_editCovAt(currentID.row());
}

void AnalyzeListView::editKin()
{
    QModelIndex currentID = currentIndex();
    pro_editKinAt(currentID.row());
}

void AnalyzeListView::startVisualization()
{
    QModelIndex currentID = currentIndex();
    pro_startVisualizationAt(currentID.row());
}

void AnalyzeListView::startGWAS(size_t id, ThreadRun::AnalyzeClass info, int nThread)
{
    pro_AnalyzeAt(id, info, nThread);
}

void AnalyzeListView::startStructure(size_t id, int nThread)
{
    pro_AnalyzeAt(id, ThreadRun::STR_STRUCTURE, nThread);
}

void AnalyzeListView::startFastStructure(size_t id, int nThread)
{
    pro_AnalyzeAt(id, ThreadRun::STR_FASTSTRUCTURE, nThread);
}

void AnalyzeListView::startKinship(size_t id, int nThread)
{
    pro_AnalyzeAt(id, ThreadRun::KIN_KINSHIP, nThread);
}

void AnalyzeListView::startNormalBSA(size_t id, int nThread)
{
    pro_AnalyzeAt(id, ThreadRun::BSA_NORMALBSA, nThread);
}

void AnalyzeListView::selectProject(int id)
{
    if (id < 0 || id >= pUnitList->size()) return;
    setCurrentIndex(pModel->index(id, 0));
}

void AnalyzeListView::pauseOrContinueSelectedProject()
{
    QModelIndex currentID = currentIndex();
    if (!currentID.isValid()) return;
    int row = currentID.row();
    if (row < 0 || row >= pUnitList->size()) return;
    (*pWidgetList)[row]->startButtonTrigger();
}

void AnalyzeListView::receiveProjectStateChanged(int id, runState state)
{
    QModelIndex currentID = currentIndex();
    if (!currentID.isValid()) return;
    int row = currentID.row();
    if (row < 0 || row >= pUnitList->size()) return;
    if (id == row) emit projectSelectedState(state);
}

void AnalyzeListView::receiveProjectFinished(int id)
{
    QModelIndex currentID = currentIndex();
    if (!currentID.isValid()) return;
    int row = currentID.row();
    if (row < 0 || row >= pUnitList->size()) return;
    if (id == row) {
        emit projectSelectedState(RUN_FINISHED);
        emit projectLogUpdate(id);
    }
}

void AnalyzeListView::receiveProjectLogUpdate(int id)
{
    emit projectLogUpdate(id);
}

void AnalyzeListView::menuExec(const QPoint &pos)
{
    pMenu->exec(this->mapToGlobal(pos));
}

void AnalyzeListView::widgetCliked(const QModelIndex &index)
{
    pro_widgetCliked(index);
}


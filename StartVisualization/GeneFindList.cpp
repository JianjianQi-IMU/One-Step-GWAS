#include "GeneFindList.hpp"
#include "ui_GeneFindList.h"

GeneFindListView::GeneFindListView(QWidget *parent)
    :QListView(parent)
{
    genome=nullptr;
    pMenu=new QMenu(this);
    pDelegate=new StringFindListItemDelegate(this);
    pModel=new QStringListModel(this);
    setModel(pModel);
    setItemDelegate(pDelegate);
    nGene=0;

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,&GeneFindListView::customContextMenuRequested,this,&GeneFindListView::menuExec);

    QAction* actLook = new QAction("Look with chart",this);
    QAction* actRemove = new QAction("Remove",this);
    QAction* actRemovaAll = new QAction("Remove all",this);

    pMenu->addAction(actLook);
    pMenu->addSeparator();
    pMenu->addAction(actRemove);
    pMenu->addAction(actRemovaAll);

    connect(actLook,&QAction::triggered,this,&GeneFindListView::lookGenei);
    connect(actRemove,&QAction::triggered,this,&GeneFindListView::removeGenei);
    connect(actRemovaAll,&QAction::triggered,this,&GeneFindListView::clearAll);
}

GeneFindListView::GeneFindListView(GenomeAnnotation *inGenome, QWidget *parent)
    :GeneFindListView(parent)
{
    genome=inGenome;
}

GeneFindListView::~GeneFindListView()
{

}

void GeneFindListView::setGenome(GenomeAnnotation *inGenome)
{
    genome=inGenome;
}

bool GeneFindListView::removeOne(int row)
{
    if(row>=0&&row<nGene){
        iGeneList.removeAt(row);
        pModel->removeRow(row);
        --nGene;
        return true;
    }
    return false;
}

void GeneFindListView::insertGeneid(long long *inList, long long n)
{
    if(n==0) return ;
    if(nGene){
        iGeneList.clear();
        pModel->removeRows(0,nGene);
    }
    pModel->insertRows(0,n);
    GeneAnnotation* pGene=nullptr;
    for(long long i=0;i<n;++i){
        pGene=genome->getGene(inList[i]);
        iGeneList.push_back(inList[i]);
        pModel->setData(pModel->index(i),pGene->name);
    }
    nGene=n;
}

void GeneFindListView::clearAll()
{
    if(nGene){
        pModel->removeRows(0,nGene);
        iGeneList.clear();
        nGene=0;
    }
}

void GeneFindListView::lookGenei()
{
    int id=currentIndex().row();
    emit sendLookGenei(iGeneList[id]);
}

void GeneFindListView::removeGenei()
{
    removeOne(currentIndex().row());
}

void GeneFindListView::menuExec(const QPoint &pos)
{
    pMenu->exec(this->mapToGlobal(pos));
}

GeneFindList::GeneFindList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneFindList)
{
    ui->setupUi(this);

    connect(ui->SearchOutListView,&GeneFindListView::sendLookGenei,this,&GeneFindList::getLookGenei);
}

GeneFindList::~GeneFindList()
{
    delete ui;
}

void GeneFindList::setGenome(GenomeAnnotation *inGenome)
{
    ui->SearchOutListView->setGenome(inGenome);
}

void GeneFindList::getLookGenei(long long i)
{
    emit sendLookGenei(i);
}

void GeneFindList::on_searchBtn_clicked()
{
    if(ui->searhTextEdit->text().isEmpty()){
        QMessageBox::critical(this,"input error","Keywords is empty.");
        return;
    }
    QString keywords=ui->searhTextEdit->text();
    long long iGeneList[searchDisplayMaxNum];
    long long nOut;
    ui->SearchOutListView->genome->search(keywords.toLatin1().data(),iGeneList,nOut,searchDisplayMaxNum);
    if(!nOut){
        QMessageBox::warning(this,"can not find","There is no result by searching.");
        return;
    }
    ui->SearchOutListView->insertGeneid(iGeneList,nOut);

}


void GeneFindList::on_clearBtn_clicked()
{
    ui->SearchOutListView->clearAll();
}



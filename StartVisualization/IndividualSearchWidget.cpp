#include "IndividualSearchWidget.hpp"

IndividualSearchOutListModel::IndividualSearchOutListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int IndividualSearchOutListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;
    return idDisplayList.size();
    // FIXME: Implement me!
}

QVariant IndividualSearchOutListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    int row=index.row();
    if(row<0||row>=idDisplayList.size()) return QVariant();
    if(idDisplayList[index.row()]<0||idDisplayList[index.row()]>=nameList.size()) return QVariant();
    if(role==Qt::DisplayRole){
        return nameList[idDisplayList[index.row()]];
    }
//    else if(role==Qt::DecorationRole){
//        return icon;
//    }
    // FIXME: Implement me!
    return QVariant();
}

bool IndividualSearchOutListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if(row<0||row>idDisplayList.size()||count<=0) return false;
    beginInsertRows(parent, row, row + count - 1);
    idDisplayList.insert(row,count,-1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool IndividualSearchOutListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(row<0||row+count>idDisplayList.size()||count<=0) return false;
    beginRemoveRows(parent, row, row + count - 1);
    idDisplayList.erase(idDisplayList.begin()+row,idDisplayList.begin()+row+count);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

bool IndividualSearchOutListModel::loadNameList(char **inNameList, size_t n)
{
    if(!inNameList||!n) return false;
    nameList.clear();
    for(size_t i=0;i<n;++i) nameList.append(inNameList[i]);
    return true;
}

bool IndividualSearchOutListModel::setIDList(const QModelIndex &index, size_t id, int role)
{
    if (!index.isValid()) return false;
    int row=index.row();
    if(row<0||row>=idDisplayList.size()||id<0||id>=nameList.size()) return false;
    idDisplayList[row]=id;
    emit dataChanged(index, index, QVector<int>() << role);
    return true;
}

size_t IndividualSearchOutListModel::getIDSample(int row) const
{
    if(row<0||row>=idDisplayList.size()) return SIZE_MAX;
    return idDisplayList[row];
}

IndividualSearchOutListView::IndividualSearchOutListView(QWidget *parent)
    :QListView(parent)
{
    pModel = new IndividualSearchOutListModel(this);
    pDelegate = new StringFindListItemDelegate(this);

    setModel(pModel);
    setItemDelegate(pDelegate);

    connect(this,&IndividualSearchOutListView::clicked,
            this,&IndividualSearchOutListView::doSelectedChanged);
}

IndividualSearchOutListView::~IndividualSearchOutListView()
{

}

bool IndividualSearchOutListView::loadNameList(char **inNameList, size_t n)
{
    return pModel -> loadNameList(inNameList,n);
}

void IndividualSearchOutListView::clearList()
{
    pModel -> removeRows(0,pModel->rowCount());
}

void IndividualSearchOutListView::setIDDisplayList(const QVector<size_t> &inList)
{
    clearList();
    if(!(inList.size())) return ;
    pModel -> insertRows(0,inList.size());
    for(int i=0;i<inList.size();++i){
        pModel->setIDList(pModel->index(i),inList[i]);
    }
}

void IndividualSearchOutListView::doSelectedChanged(const QModelIndex &index)
{
    if(!(index.isValid())) return ;
    int row = index.row();
    if(row<0||row>=pModel->rowCount()) return ;
    emit sendSelectedChanged(pModel->getIDSample(row));
}


IndividualSearchWidget::IndividualSearchWidget(QWidget *parent)
    :QWidget(parent)
{
    nameList = nullptr;
    nSample=0;

    // top input widget setting
    pEdit = new QLineEdit(this);
    pSearchBtn = new QPushButton(this);
    pSearchBtn->setIcon(QIcon(":/icon/searchIcon3.png"));
    pSearchBtn->setFixedSize(25,25);
    pEdit->setMaximumWidth(9999);
    QWidget* topInputWidget = new QWidget(this);
    QHBoxLayout* topInputLayout = new QHBoxLayout(topInputWidget);
    topInputLayout->addWidget(pEdit);
    topInputLayout->addWidget(pSearchBtn);
    topInputLayout->addWidget(new QWidget(this));
    topInputLayout->setMargin(0);
    connect(pSearchBtn,&QPushButton::clicked,this,&IndividualSearchWidget::doSearchSample);

    //listview setting
    pList = new IndividualSearchOutListView(this);
    connect(pList,&IndividualSearchOutListView::sendSelectedChanged,
            this,&IndividualSearchWidget::sendSelectedChanged);

    //clear button setting
    pClearBtn = new QPushButton("clear",this);
    QWidget* clearListWidget = new QWidget(this);
    QWidget* nullWidget = new QWidget(this);
    QHBoxLayout* clearListLayout = new QHBoxLayout(clearListWidget);
    clearListLayout->addWidget(nullWidget);
    clearListLayout->addWidget(pClearBtn);
    pClearBtn->setFixedWidth(100);
    connect(pClearBtn,&QPushButton::clicked,
            this,&IndividualSearchWidget::clearList);

    //main widget setting
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout -> addWidget(topInputWidget);
    mainLayout -> addWidget(pList);
    mainLayout -> addWidget(clearListWidget);
}

IndividualSearchWidget::~IndividualSearchWidget()
{
    if(nameList){
        for(size_t i=0;i<nSample;++i) delete [] nameList[i];
        delete [] nameList;
    }
}

bool IndividualSearchWidget::loadNameList(char **inNameList, size_t n)
{
    if(!inNameList||!n) return false;
    if(nameList){
        for(size_t i=0;i<nSample;++i) delete [] nameList[i];
        delete [] nameList;
    }
    nSample = n;
    nameList = new char*[nSample];
    for(size_t i=0;i<nSample;++i){
        nameList[i] = new char[FD::MAXELEMLEN];
        FD::charCopy(nameList[i],inNameList[i]);
    }
    pList -> loadNameList(inNameList,n);
    return true;
}

void IndividualSearchWidget::searchSample(const char *inKeywords, QVector<size_t> &outList,long long outMaxN) const
{
    outList.clear();
    if(!inKeywords) return ;
    if(outMaxN<=0) return ;
    char *source;
    long long iSample;
    int i,j;
    int sLen,kLen;
    int outNum=0;
    const int maxCharLim=256;
    for(i=0;inKeywords[i];++i) ;
    kLen=i;
    int *badChar=new int[maxCharLim];
    int *goodPos=new int[kLen+1];
    int *goodShift=new int[kLen+1];
    for(i=0; i<maxCharLim; i++) badChar[i] = -1;
    for(i=0; i<kLen; i++) badChar[int(inKeywords[i])] = i;
    for(i=0;i<kLen+1;i++) goodShift[i]=0;
    i=kLen,j=kLen+1;
    goodPos[i]=j;
    while(i>0){
        while(j<=kLen&&inKeywords[i-1]!=inKeywords[j-1]){
            if(goodShift[j]==0) goodShift[j]=j-i;
            j=goodPos[j];
        }
        i--;j--;
        goodPos[i]=j;
    }
    j=goodPos[0];
    for(i=0;i<=kLen;i++){
        if(goodShift[i]==0) goodShift[i]=j;
        if (i==j) j=goodPos[j];
    }
    for(iSample=0;iSample<nSample;++iSample){
        source=nameList[iSample];
        for(i=0;source[i];++i) ;
        sLen=i;
        i=0;
        while(i<=(sLen-kLen)){
            j=kLen-1;
            while(j>=0&&inKeywords[j]==source[i+j]) j--;
            if(j<0) break;
            else i+=std::max(goodShift[j+1],std::max(1, j-badChar[int(source[i+j])]));
        }
        if(i<=(sLen-kLen)){
            outList.append(iSample);
            outNum++;
        }
        if(outNum>=outMaxN) break;
    }
    delete [] goodPos;
    delete [] goodShift;
    delete [] badChar;
}

void IndividualSearchWidget::clearList()
{
    pList->clearList();
    emit sendSelectedChanged(-1);
}

void IndividualSearchWidget::doSearchSample()
{
    QString name=pEdit->text();
    if(name.isEmpty()) return ;
    QVector<size_t> tList;
    searchSample(name.toLatin1().data(),tList,10000);
    pList->setIDDisplayList(tList);
}

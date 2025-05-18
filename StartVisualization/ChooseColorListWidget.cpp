#include "ChooseColorListWidget.hpp"

ChooseColorListIndex::ChooseColorListIndex(QObject *parent)
    :QObject(parent)
{

}

ChooseColorListIndex::ChooseColorListIndex(int index, QObject *parent)
    :ChooseColorListIndex(parent)
{
    id=index;
}

void ChooseColorListIndex::setIndex(int index)
{
    id=index;
}

void ChooseColorListIndex::removeItem()
{
    emit sendRemove(id);
}

void ChooseColorListIndex::chooseColor()
{
    emit sendChoose(id);
}

int ChooseColorListItemDelegate::buttonSize = 28;

int ChooseColorListItemDelegate::rowHeight = 32;

ChooseColorListItemDelegate::ChooseColorListItemDelegate(QWidget *parent)
    :QStyledItemDelegate(parent)
{

    chooseBtnList = nullptr;
    removeBtnList = nullptr;
    colorList     = nullptr;
}

void ChooseColorListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);
    long x0=option.rect.left(),y0=option.rect.top();
    long width0=option.rect.width(),height0=option.rect.height();
    long btnx0=x0+width0-2*buttonSize-10,btnx1=x0+width0-buttonSize-5,btny0=y0+(height0-buttonSize)/2;
    QRectF rect(x0+21,y0+1,width0-22,height0-2);
    QRectF rect2(x0,y0,2,height0);
    size_t id=index.row();
    QColor rectColor=colorList->at(id);
    bool isMouseOver=false;
    painter->fillRect(rect,rectColor);
    painter->drawText(x0,y0,20,height0,Qt::AlignHCenter|Qt::AlignVCenter,QString::number(id));
    if(option.state.testFlag(QStyle::State_Selected)){
        painter->fillRect(rect2,QBrush(QColor("#5e5d53")));
    }
    (*chooseBtnList)[id]->move(btnx0,btny0);
    (*removeBtnList)[id]->move(btnx1,btny0);
    (*chooseBtnList)[id]->show();
    (*removeBtnList)[id]->show();
    painter->restore();
}

QSize ChooseColorListItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QSize(option.rect.width(),rowHeight);
}

void ChooseColorListItemDelegate::setChooseBtnList(QVector<QPushButton *> *inList)
{
    chooseBtnList=inList;
}

void ChooseColorListItemDelegate::setRemoveBtnList(QVector<QPushButton *> *inList)
{
    removeBtnList=inList;
}

void ChooseColorListItemDelegate::setColorList(QVector<QColor> *inList)
{
    colorList=inList;
}

ChooseColorListModel::ChooseColorListModel(QWidget *parent)
    :QAbstractListModel(parent)
{
    chooseBtnList = new QVector<QPushButton*>;
    removeBtnList = new QVector<QPushButton*>;
    indexList     = new QVector<ChooseColorListIndex*>;
    colorList     = new QVector<QColor>;
    theParent     = parent;
}

ChooseColorListModel::~ChooseColorListModel()
{
    if(chooseBtnList) delete chooseBtnList;
    if(removeBtnList) delete removeBtnList;
    if(indexList    ) delete indexList;
    if(colorList    ) delete colorList;
}

int ChooseColorListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return colorList->size();
}

QVariant ChooseColorListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) return QVariant();
    return (*colorList)[index.row()].name();
}

bool ChooseColorListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    int i;
    QPushButton* pBtn=nullptr;
    ChooseColorListIndex* pIndex=nullptr;
    for(i=0;i<count;++i){
        pIndex=new ChooseColorListIndex(row,theParent);
        connect(pIndex,&ChooseColorListIndex::sendChoose,this,&ChooseColorListModel::chooseColorAt);
        connect(pIndex,&ChooseColorListIndex::sendRemove,this,&ChooseColorListModel::removeAt);
        indexList->insert(row,pIndex);
        pBtn=new QPushButton(theParent);
//        pBtn->setHidden(true);
        pBtn->setFixedSize(ChooseColorListItemDelegate::buttonSize,ChooseColorListItemDelegate::buttonSize);
        pBtn->setIcon(QIcon(":/icon/chooseColorIcon2.png"));
        connect(pBtn,&QPushButton::clicked,pIndex,&ChooseColorListIndex::chooseColor);
        chooseBtnList->insert(row,pBtn);
        pBtn=new QPushButton(theParent);
//        pBtn->setHidden(true);
        pBtn->setFixedSize(ChooseColorListItemDelegate::buttonSize,ChooseColorListItemDelegate::buttonSize);
        pBtn->setIcon(QIcon(":/icon/deleteIcon.png"));
        connect(pBtn,&QPushButton::clicked,pIndex,&ChooseColorListIndex::removeItem);
        removeBtnList->insert(row,pBtn);
        colorList->insert(row,QColor(0,0,0));
    }
    for(i=row;i<colorList->size();++i){
        (*indexList)[i]->setIndex(i);
    }
    endInsertRows();
    return true;
}

bool ChooseColorListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(row+count-1>=colorList->size()) return false;
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    int i;
    for(int i=0;i<count;++i){
        delete (*indexList)[row];
        indexList->removeAt(row);
        delete (*chooseBtnList)[row];
        chooseBtnList->removeAt(row);
        delete (*removeBtnList)[row];
        removeBtnList->removeAt(row);
        colorList->removeAt(row);
    }
    for(i=row;i<colorList->size();++i){
        (*indexList)[i]->setIndex(i);
    }
    endRemoveRows();
    return true;
}

QVector<QPushButton *> *ChooseColorListModel::getChooseBtnList()
{
    return chooseBtnList;
}

QVector<QPushButton *> *ChooseColorListModel::getRemoveBtnList()
{
    return removeBtnList;
}

QVector<QColor> *ChooseColorListModel::getColorList()
{
    return colorList;
}

void ChooseColorListModel::removeAt(int row)
{
    removeRows(row,1);
}

void ChooseColorListModel::chooseColorAt(int row)
{
   QColor color = QColorDialog::getColor(colorList->at(row),theParent);
   if(color.isValid()){
       (*colorList)[row]=color;
   }
}

void ChooseColorListModel::removeAll(int minVal)
{
    if(colorList->isEmpty()) return;
    if(colorList->length()<=minVal) return;
    removeRows(0,colorList->size()-minVal);
}

void ChooseColorListModel::addOne()
{
    insertRows(colorList->size(),1);
}

void ChooseColorListModel::setColorList(const QVector<QColor> *inColorList)
{
    removeAll();
    int n=inColorList->size();
    if(!n) return;
    insertRows(0,n);
    for(int i=0;i<n;++i){
        (*colorList)[i]=inColorList->at(i);
    }
}

const QVector<QColor> *ChooseColorListModel::outputColorList()
{
    return colorList;
}


ChooseColorListView::ChooseColorListView(QWidget *parent)
    :QListView(parent)
{
    pDelegate = new ChooseColorListItemDelegate(this);
    pModel    = new ChooseColorListModel(this);
    pMenu     = new QMenu(this);
    pDelegate->setChooseBtnList(pModel->getChooseBtnList());
    pDelegate->setColorList(pModel->getColorList());
    pDelegate->setRemoveBtnList(pModel->getRemoveBtnList());
    setModel(pModel);
    setItemDelegate(pDelegate);
    maxNum = 128;
    minNum = 1;
    isSilentTouchMaximun = true;
    QAction* actChooseColor = new QAction("select color",pMenu);
    QAction* actRemove      = new QAction("remove",pMenu);
    QAction* actRemoveAll   = new QAction("remove all",pMenu);
    QAction* actAddOne      = new QAction("add new",pMenu);
//    actChooseColor -> setIcon();
//    actRemove      -> setIcon();
//    actRemoveAll   -> setIcon();
//    actAddOne      -> setIcon();
    pMenu -> addAction(actChooseColor);
    pMenu -> addAction(actRemove);
    pMenu -> addAction(actRemoveAll);
    pMenu -> addAction(actAddOne);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,&ChooseColorListView::customContextMenuRequested,this,&ChooseColorListView::menuExec);

    connect(actChooseColor,&QAction::triggered,this,&ChooseColorListView::chooseColorOne);
    connect(actRemove,&QAction::triggered,this,&ChooseColorListView::removeOne);
    connect(actRemoveAll,&QAction::triggered,this,&ChooseColorListView::removeAll);
    connect(actAddOne,&QAction::triggered,this,&ChooseColorListView::addOne);
}

ChooseColorListView::~ChooseColorListView()
{

}

void ChooseColorListView::setMaxNum(int maxVal)
{
    maxNum = maxVal;
}

void ChooseColorListView::setMinNum(int minVal)
{
    minNum = minVal;
}

void ChooseColorListView::setIsSilentTouchMaximun(bool flag)
{
    isSilentTouchMaximun = flag;
}

void ChooseColorListView::menuExec(const QPoint &pos)
{
    pMenu->exec(this->mapToGlobal(pos));
}

void ChooseColorListView::removeAt(int row)
{
    pModel->removeAt(row);
    update();
}

void ChooseColorListView::chooseColorAt(int row)
{
    pModel->chooseColorAt(row);
    update();
}

void ChooseColorListView::removeOne()
{
    if(pModel->rowCount() <= minNum){
        if(isSilentTouchMaximun){
            QMessageBox::warning(this,"warning","Having touched the minimum.");
        }
        return ;
    }
    QModelIndex index=currentIndex();
    if(!index.isValid()) return;
    int row=index.row();
    if(row>=pModel->rowCount()) return;
    pModel->removeAt(row);
    update();
}

void ChooseColorListView::chooseColorOne()
{
    QModelIndex index=currentIndex();
    if(!index.isValid()) return;
    int row=index.row();
    if(row>=pModel->rowCount()) return;
    pModel->chooseColorAt(row);
    update();
}

void ChooseColorListView::removeAll()
{
    pModel->removeAll(minNum);
    update();
}

void ChooseColorListView::addOne()
{
    if(pModel->rowCount() >= maxNum){
        if(isSilentTouchMaximun){
            QMessageBox::warning(this,"warning","Having touched the maximum.");
        }
        return ;
    }
    pModel->addOne();
    update();
}

void ChooseColorListView::setColorList(const QVector<QColor> *inColorList)
{
    pModel->setColorList(inColorList);
    update();
}

const QVector<QColor> *ChooseColorListView::outputColorList()
{
    return pModel->outputColorList();
}

ChooseColorListWidget::ChooseColorListWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose,true);
    setAttribute(Qt::WA_ShowModal,true);

    theParent=parent;

    pListWidget=new ChooseColorListView(this);
    QLabel* listLabel=new QLabel("color list:",this);
    okBtn     = new QPushButton("OK",this);
    cancelBtn = new QPushButton("cancel",this);
    addBtn    = new QPushButton(this);
    resetBtn  = new QPushButton(this);
    QWidget* upWidget=new QWidget(this);
    QWidget* okWidget=new QWidget(this);
    QHBoxLayout* upLayout=new QHBoxLayout(upWidget);
    QVBoxLayout* mainLayout=new QVBoxLayout(this);
    QHBoxLayout* okLayout=new QHBoxLayout(okWidget);
    resetBtn->setToolTip("reset color list");
    resetBtn->setFixedSize(25,25);
    resetBtn->setIcon(QIcon(":/icon/restoreIcon.png"));
    addBtn  ->setToolTip("add color");
    addBtn  ->setFixedSize(25,25);
    addBtn  ->setIcon(QIcon(":/icon/addIcon.png"));
    upLayout->addWidget(listLabel);
    upLayout->addWidget(resetBtn);
    upLayout->addWidget(addBtn);
    okLayout->addWidget(okBtn);
    okLayout->addWidget(cancelBtn);
    mainLayout->addWidget(upWidget);
    mainLayout->addWidget(pListWidget);
    mainLayout->addWidget(okWidget);

    connect(okBtn,&QPushButton::clicked,this,&ChooseColorListWidget::saveColorList);
    connect(cancelBtn,&QPushButton::clicked,this,&ChooseColorListWidget::close);
    connect(resetBtn,&QPushButton::clicked,this,&ChooseColorListWidget::resetColorList);
    connect(addBtn,&QPushButton::clicked,pListWidget,&ChooseColorListView::addOne);
}

ChooseColorListWidget::ChooseColorListWidget(const QVector<QColor>& pList,
                                             int maxVal,
                                             int minVal,
                                             QWidget *parent)
    :ChooseColorListWidget(parent)
{
    pListWidget->setMaxNum(maxVal);
    pListWidget->setMinNum(minVal);
    oldColorList = pList;
    pListWidget->setColorList(&oldColorList);
}

void ChooseColorListWidget::setIsSilentTouchMaximun(bool flag)
{
    pListWidget -> setIsSilentTouchMaximun(flag);
}

void ChooseColorListWidget::saveColorList()
{
    emit sendColor(*(pListWidget->outputColorList()));
    close();
}

void ChooseColorListWidget::resetColorList()
{
    pListWidget->setColorList(&oldColorList);
}

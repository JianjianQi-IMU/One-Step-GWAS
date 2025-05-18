#ifndef CHOOSECOLORLISTWIDGET_HPP
#define CHOOSECOLORLISTWIDGET_HPP

#include <QWidget>
#include <QVector>
#include <QColor>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QListView>
#include <QPushButton>
#include <QColorDialog>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

#include <QDebug>

class ChooseColorListIndex : public QObject
{
    Q_OBJECT
private:
    int id;
public:
    explicit ChooseColorListIndex(QObject* parent = nullptr);
    explicit ChooseColorListIndex(int index,
                                  QObject* parent = nullptr);

    void setIndex(int index);
signals:
    void sendRemove(int i);
    void sendChoose(int i);
public slots:
    void removeItem();
    void chooseColor();
};

class ChooseColorListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ChooseColorListItemDelegate(QWidget *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    //custom
    void setChooseBtnList(QVector<QPushButton*>* inList);
    void setRemoveBtnList(QVector<QPushButton*>* inList);
    void setColorList(QVector<QColor>* inList);

    static int buttonSize;
    static int rowHeight;

signals:

private:
    QVector<QPushButton*>* chooseBtnList;
    QVector<QPushButton*>* removeBtnList;
    QVector<QColor>*       colorList;
};

class ChooseColorListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ChooseColorListModel(QWidget *parent = nullptr);
    ~ChooseColorListModel();

    // Header:
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

//    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
//    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

//    bool canFetchMore(const QModelIndex &parent) const override;
//    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
//    bool setData(const QModelIndex &index, const QVariant &value,
//                 int role = Qt::EditRole) override;

//    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // custom
    QVector<QPushButton*>* getChooseBtnList();
    QVector<QPushButton*>* getRemoveBtnList();
    QVector<QColor>*       getColorList();
    void removeAt(int row);
    void chooseColorAt(int row);
    void removeAll(int minVal = 0);
    void addOne();
    void setColorList(const QVector<QColor>* inColorList);
    const QVector<QColor>* outputColorList();

private:
    QWidget* theParent;
    QVector<QPushButton*>* chooseBtnList;
    QVector<QPushButton*>* removeBtnList;
    QVector<ChooseColorListIndex*>* indexList;
    QVector<QColor>*       colorList;
};

class ChooseColorListView : public QListView
{
    Q_OBJECT
private:
    ChooseColorListItemDelegate* pDelegate;
    ChooseColorListModel*        pModel;
    QMenu* pMenu;
    int maxNum;
    int minNum;

    bool isSilentTouchMaximun;
public:
    explicit ChooseColorListView(QWidget *parent = nullptr);
    ~ChooseColorListView();
    void setMaxNum(int maxVal);
    void setMinNum(int minVal);
    void setIsSilentTouchMaximun(bool flag);
signals:
//    void sendSelectedId(AnalyzeUnitInfo* outUnit,QString name);

public slots:
    void menuExec(const QPoint& pos);
    void removeAt(int row);
    void chooseColorAt(int row);
    void removeOne();
    void chooseColorOne();
    void removeAll();
    void addOne();
    void setColorList(const QVector<QColor>* inColorList);
    const QVector<QColor>* outputColorList();
};


class ChooseColorListWidget : public QWidget
{
    Q_OBJECT
private:
    QWidget* theParent;
    ChooseColorListView* pListWidget;
    QPushButton* resetBtn;
    QPushButton* okBtn;
    QPushButton* cancelBtn;
    QPushButton* addBtn;
    QVector<QColor> oldColorList;

    explicit ChooseColorListWidget(QWidget *parent = nullptr);
public:
    ChooseColorListWidget(const QVector<QColor>& pList,
                          int maxVal = 128,
                          int minVal = 0,
                          QWidget *parent = nullptr);


    void setIsSilentTouchMaximun(bool flag);
signals:
    void sendColor(QVector<QColor> outColor);
public slots:
    void saveColorList();
    void resetColorList();
};

#endif // CHOOSECOLORLISTWIDGET_HPP

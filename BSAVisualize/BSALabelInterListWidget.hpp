#ifndef BSALABELINTERLISTWIDGET_HPP
#define BSALABELINTERLISTWIDGET_HPP

#include <QWidget>
#include <QVBoxLayout>
#include "BSAVisualize/BSALabelInterListView.hpp"

class BSALabelInterListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BSALabelInterListWidget(QWidget *parent = nullptr);
    ~BSALabelInterListWidget();
    void setInter(FD::BSAInterVal* inInter,size_t inNInter);
    bool setMaxLabelInterNum(int n);
    bool setChrName(char** pName);


private:
    FD::BSAInterVal* inter;
    size_t nInter;

    BSALabelInterListView* pLabelView;
signals:
    void removeLabelInter(int row);
    void lookLabelInter(int row);
    void removeAllLabelInter();
public slots:
    bool addInter(BSAPointID id);
    bool removeInter(int row);

};

#endif // BSALABELINTERLISTWIDGET_HPP

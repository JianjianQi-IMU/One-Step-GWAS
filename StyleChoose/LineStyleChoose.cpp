#include "LineStyleChoose.hpp"

#include <QHBoxLayout>

LineStyleDisplayWidget::LineStyleDisplayWidget(const LineStyleParam &inParam,
                                               QWidget* parent)
    : QWidget(parent)
{
    param = inParam;
}

void LineStyleDisplayWidget::setLineParam(const LineStyleParam &inParam)
{
    param = inParam;
}

LineStyleParam LineStyleDisplayWidget::getLineParam() const
{
    return param;
}

void LineStyleDisplayWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    QSize tsize = size();
    QPen  tpen;
    tpen.setColor(param.lineColor);
    tpen.setStyle(param.lineStyle);
    tpen.setWidth(param.lineSize);
    painter.setPen(tpen);
    painter.drawLine(0,tsize.height()/2,
                     tsize.width(),tsize.height()/2);
}

LineStyleDisplayItemWidget::LineStyleDisplayItemWidget(const QString &inStr, const LineStyleParam &inParam, QWidget *parent)
    : QLineEdit(parent)
{
    pLabel = new QLabel(inStr,this);
    pStyleWidget = new LineStyleDisplayWidget(inParam,this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout -> addWidget(pLabel);
    layout -> addWidget(pStyleWidget);
    layout -> setContentsMargins(4,0,0,0);
    pLabel -> setFixedSize(32,16);
    setReadOnly(true);
}

void LineStyleDisplayItemWidget::setIndex(int id)
{
    index = id;
}

void LineStyleDisplayItemWidget::setItem(const QString &inStr, const LineStyleParam &inParam)
{
    pLabel -> setText(inStr);
    pStyleWidget -> setLineParam(inParam);
}

LineStyleParam LineStyleDisplayItemWidget::getLineParam() const
{
    return pStyleWidget -> getLineParam();
}

QString LineStyleDisplayItemWidget::getLabelText() const
{
    return pLabel -> text();
}

void LineStyleDisplayItemWidget::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    emit sendClicked(index);
    QLineEdit::mousePressEvent(e);

}

LineStyleDisplayComboBox::LineStyleDisplayComboBox(QWidget *parent)
    :QComboBox(parent)
{
    pListWidget = new QListWidget(this);
    currentItem = new LineStyleDisplayItemWidget("",LineStyleParam(),this);
    setContextMenuPolicy(Qt::NoContextMenu);
    setLineEdit(currentItem);
    setModel(pListWidget->model());
    setView(pListWidget);
}

void LineStyleDisplayComboBox::setCurrentItem(int id)
{
    currentItem -> setItem(vec[id]->getLabelText(),vec[id]->getLineParam());
    hidePopup();
    currentItem -> update();
}

LineStyleParam LineStyleDisplayComboBox::getCurrentParam() const
{
    return currentItem -> getLineParam();
}

void LineStyleDisplayComboBox::addLineItem(const QString &inStr, const LineStyleParam &inParam)
{
    LineStyleDisplayItemWidget* pAdd = new LineStyleDisplayItemWidget(inStr,inParam,this);
    QListWidgetItem* pItem = new QListWidgetItem(pListWidget);
    pListWidget -> addItem(pItem);
    pListWidget -> setItemWidget(pItem,pAdd);
    pAdd->setIndex(vec.size());
    vec.append(pAdd);
    connect(pAdd,&LineStyleDisplayItemWidget::sendClicked,
            this,&LineStyleDisplayComboBox::setCurrentItem);
}

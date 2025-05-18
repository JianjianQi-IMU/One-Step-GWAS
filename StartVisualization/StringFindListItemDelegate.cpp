#include "StringFindListItemDelegate.hpp"

#include <QPainter>

StringFindListItemDelegate::StringFindListItemDelegate(QWidget *parent)
    :QStyledItemDelegate(parent)
{

}

void StringFindListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    long x0=option.rect.left(),y0=option.rect.top();
    long width0=option.rect.width(),height0=option.rect.height();
    QRectF choosedRect(x0+1,y0+1,width0-2,height0-2);
    QRectF textRect(x0+marginLeftText,y0,width0-marginLeftText,height0);
    QRectF selectedRect(x0,float(height0-seletedLabelheight)/2+y0,seletedLabelWidth,seletedLabelheight);
    if(option.state.testFlag(QStyle::State_Selected))
    {
        painter->setPen(Qt::transparent);
        painter->setBrush(QBrush(QColor(230, 230, 255)));
        painter->drawRoundedRect(choosedRect,1,1);
        painter->fillRect(selectedRect,QColor("#353b48"));
    }
    painter->setPen(QColor(Qt::black));
    painter->drawText(textRect,Qt::AlignVCenter|Qt::AlignLeft,index.data().toString());
    painter->restore();
}

QSize StringFindListItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QSize(option.rect.width(),itemRectHeight);
}

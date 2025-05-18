#ifndef STRINGFINDLISTITEMDELEGATE_HPP
#define STRINGFINDLISTITEMDELEGATE_HPP

#include <QStyledItemDelegate>

class StringFindListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit StringFindListItemDelegate(QWidget *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    static const int itemRectHeight {40};
    static const int marginLeftText {10};
    static const int seletedLabelWidth {5};
    static const int seletedLabelheight {30};
signals:

};

#endif // STRINGFINDLISTITEMDELEGATE_HPP

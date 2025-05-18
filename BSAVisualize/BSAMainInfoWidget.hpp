#ifndef BSAMAININFOWIDGET_HPP
#define BSAMAININFOWIDGET_HPP

#include <QWidget>

namespace Ui {
class BSAMainInfoWidget;
}

class BSAMainInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BSAMainInfoWidget(QWidget *parent = nullptr);
    ~BSAMainInfoWidget();

private:
    Ui::BSAMainInfoWidget *ui;
};

#endif // BSAMAININFOWIDGET_HPP

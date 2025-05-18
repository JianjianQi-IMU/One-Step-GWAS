#ifndef GWASQQMAININFOWIDGET_H
#define GWASQQMAININFOWIDGET_H

#include <QWidget>

namespace Ui {
class GWASQQMainInfoWidget;
}

class GWASQQMainInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GWASQQMainInfoWidget(QWidget *parent = nullptr);
    ~GWASQQMainInfoWidget();
    void setNumber(const QString& text);
    void setMax(const QString& text);

private:
    Ui::GWASQQMainInfoWidget *ui;
};

#endif // GWASQQMAININFOWIDGET_H

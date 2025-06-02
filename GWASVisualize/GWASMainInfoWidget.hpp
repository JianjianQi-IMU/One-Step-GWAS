#ifndef GWASMAININFOWIDGET_HPP
#define GWASMAININFOWIDGET_HPP

#include <QWidget>

namespace Ui
{
class GWASMainInfoWidget;
}

class GWASMainInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GWASMainInfoWidget(QWidget *parent = nullptr);
    ~GWASMainInfoWidget();
    void setThreshold(const QString& text);
    void setNumber(const QString& text);
    void setMax(const QString& text);


private:
    Ui::GWASMainInfoWidget *ui;
};

#endif // GWASMAININFOWIDGET_HPP

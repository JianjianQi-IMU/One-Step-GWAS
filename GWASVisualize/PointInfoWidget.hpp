#ifndef POINTINFOWIDGET_HPP
#define POINTINFOWIDGET_HPP

#include <QWidget>

namespace Ui
{
class PointInfoWidget;
}

class PointInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PointInfoWidget(QWidget *parent = nullptr);
    ~PointInfoWidget();

    void setLogP(const QString& text);
    void setChr(const QString& text);
    void setPos(const QString& text);
    void setGene1(const QString& text);
    void setGene2(const QString& text);
    void setInfoNull();
private:
    Ui::PointInfoWidget *ui;
};

#endif // POINTINFOWIDGET_HPP

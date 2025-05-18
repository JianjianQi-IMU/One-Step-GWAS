#ifndef PCAMAININFOWIDGET_HPP
#define PCAMAININFOWIDGET_HPP

#include <QWidget>

namespace Ui {
class PCAMainInfoWidget;
}

class PCAMainInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PCAMainInfoWidget(QWidget *parent = nullptr);
    ~PCAMainInfoWidget();
    void setNSample(const QString& text);
    void setNPop(const QString& text);
    void setPC1(const QString& text);
    void setPC2(const QString& text);
    void setPC3(const QString& text);

private:
    Ui::PCAMainInfoWidget *ui;
};

#endif // PCAMAININFOWIDGET_HPP

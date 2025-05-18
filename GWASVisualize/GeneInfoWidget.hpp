#ifndef GENEINFOWIDGET_HPP
#define GENEINFOWIDGET_HPP

#include <QWidget>

namespace Ui {
class GeneInfoWidget;
}

class GeneInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeneInfoWidget(QWidget *parent = nullptr);
    ~GeneInfoWidget();

    void setName(const QString& text);
    void setType(const QString& text);
    void setStrand(const QString& text);
    void setPos(const QString& text);
    void setAnno(const QString& text);
    void setLen(const QString& text);
    void setChr(const QString& text);
    void setInfoNull();

private:
    Ui::GeneInfoWidget *ui;
};

#endif // GENEINFOWIDGET_HPP

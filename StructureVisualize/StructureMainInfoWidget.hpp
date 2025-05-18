#ifndef STRUCTUREMAININFOWIDGET_HPP
#define STRUCTUREMAININFOWIDGET_HPP

#include <QWidget>

namespace Ui {
class StructureMainInfoWidget;
}

class StructureMainInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StructureMainInfoWidget(QWidget *parent = nullptr);
    ~StructureMainInfoWidget();
    void setNSamples(const QString inText);
    void setNPops(const QString inText);

private:
    Ui::StructureMainInfoWidget *ui;
};

#endif // STRUCTUREMAININFOWIDGET_HPP

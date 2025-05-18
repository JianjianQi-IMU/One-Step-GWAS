#ifndef SETPOINTSIZEWIDGET_H
#define SETPOINTSIZEWIDGET_H

#include <QWidget>
#include <QSpinBox>
#include <QPushButton>

class SetPointSizeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SetPointSizeWidget(QWidget *parent = nullptr);
    SetPointSizeWidget(int defaultSize, QWidget *parent = nullptr);
public slots:
    void savePointSize();
private:
    QSpinBox*    valBox;
    QPushButton* okBtn;
    QPushButton* cancelBtn;

signals:
    void sendPointSize(float val);
};

#endif // SETPOINTSIZEWIDGET_H

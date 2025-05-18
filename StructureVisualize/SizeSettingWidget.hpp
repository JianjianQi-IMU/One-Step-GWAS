#ifndef SIZESETTINGWIDGET_HPP
#define SIZESETTINGWIDGET_HPP

#include <QWidget>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

class SizeSettingWidget : public QWidget
{
    Q_OBJECT
private:
    QSize setSize;
    QSpinBox* widthValBox;
    QSpinBox* heightValBox;
    QPushButton* setBtn;
    QPushButton* cancelBtn;
    explicit SizeSettingWidget(QWidget* parent = nullptr) : QWidget(parent)
    {
        setAttribute(Qt::WA_DeleteOnClose,true);
        setAttribute(Qt::WA_ShowModal,true);

        widthValBox = new QSpinBox(this);
        heightValBox = new QSpinBox(this);
        QLabel* WLabel = new QLabel("Width",this);
        QLabel* HLabel = new QLabel("Height",this);
        setBtn    = new QPushButton("set",this);
        cancelBtn = new QPushButton("cancel",this);

        QWidget* downWidth = new QWidget(this);
        QWidget* wWidth = new QWidget(this);
        QWidget* hWidth = new QWidget(this);
        QHBoxLayout* wLayout = new QHBoxLayout(wWidth);
        QHBoxLayout* hLayout = new QHBoxLayout(hWidth);
        QHBoxLayout* downLayout = new QHBoxLayout(downWidth);
        QVBoxLayout* mainLayout = new QVBoxLayout(this);

        wLayout -> addWidget(WLabel);
        wLayout -> addWidget(widthValBox);
        hLayout -> addWidget(HLabel);
        hLayout -> addWidget(heightValBox);

        downLayout -> addWidget(setBtn);
        downLayout -> addWidget(cancelBtn);

        mainLayout -> addWidget(wWidth);
        mainLayout -> addWidget(hWidth);
        mainLayout -> addWidget(downWidth);

        widthValBox -> setMaximum(10000);
        widthValBox -> setMinimum(1);
        heightValBox -> setMaximum(10000);
        heightValBox -> setMinimum(1);

        connect(setBtn,&QPushButton::clicked,
                this,&SizeSettingWidget::setBtnClicked);
        connect(cancelBtn,&QPushButton::clicked,
                this,&SizeSettingWidget::close);
    }
public:
    SizeSettingWidget(const QSize& inSize,QWidget* parent = nullptr)
        : SizeSettingWidget(parent)
    {
        setSize = inSize;
    }

public slots:
    void setBtnClicked()
    {
        int W = widthValBox->value();
        int H = heightValBox->value();
        if(W <= 0 || H <= 0){
            QMessageBox::critical(this,"error","this value of size is error");
            return;
        }
        emit sendSizeVal(QSize(W,H));
        close();
    }
signals:
    void sendSizeVal(QSize size);

};

#endif // SIZESETTINGWIDGET_HPP

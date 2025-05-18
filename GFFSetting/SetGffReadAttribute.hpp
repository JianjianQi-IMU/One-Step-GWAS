#ifndef SETGFFREADATTRIBUTE_HPP
#define SETGFFREADATTRIBUTE_HPP

#include <QWidget>
#include <FileDeal/FileDeal.hpp>

namespace Ui {
class SetGffReadAttribute;
}

class SetGffReadAttribute : public QWidget
{
    Q_OBJECT
private:
    explicit SetGffReadAttribute(QWidget *parent = nullptr);
    FD::genomeRead *pGenomeReader;
public:
    SetGffReadAttribute(FD::genomeRead *genomeReader,QWidget *parent = nullptr);
    ~SetGffReadAttribute();
    void displayDemo();
    void saveSetting();
    void setDefault();
    QString getDemoText();

private:
    Ui::SetGffReadAttribute *ui;
};

#endif // SETGFFREADATTRIBUTE_HPP

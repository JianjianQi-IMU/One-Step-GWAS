#ifndef LOADGENOMESEQUENCEWIDGET_HPP
#define LOADGENOMESEQUENCEWIDGET_HPP

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>

class LoadGenomeSequenceWidget : public QWidget
{
    Q_OBJECT
private:
    QPushButton*    continueBtn;
    QPushButton*    cancelBtn;
    QPushButton*    openBtn;
    QLineEdit*      filePathEdit;

public:
    explicit LoadGenomeSequenceWidget(QWidget* parent = nullptr);
    ~LoadGenomeSequenceWidget();
public slots:
    void loadGenomeSeqFile();
    void openFile();
signals:
    void sendFilePath(QString filePath);
};

#endif // LOADGENOMESEQUENCEWIDGET_HPP

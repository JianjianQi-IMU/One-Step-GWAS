#ifndef STARTPCAVISUALIZATIONWIDGET_HPP
#define STARTPCAVISUALIZATIONWIDGET_HPP


#include "FileDeal/FileDeal.hpp"
#include "PCAVisualize/PCA2DVisualizationWidget.hpp"
#include "PCAVisualize/PCA3DVisualizationWidget.hpp"
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>

class StartPCAVisualizationWidget : public QWidget
{
    Q_OBJECT
private:
    QLineEdit*    filePathEdit;
    QLineEdit*    PCAPathEdit;
    QPushButton*  continueBtn;
    QPushButton*  cancelBtn;
    QRadioButton* D2Radio;
    QRadioButton* D3Radio;
    MML::Mat PCs;
public:
    explicit StartPCAVisualizationWidget(QWidget *parent = nullptr);
    ~StartPCAVisualizationWidget();
public slots:
    void openFile();
    void openPCA();
    void createVisualize();
};

#endif // STARTPCAVISUALIZATIONWIDGET_HPP

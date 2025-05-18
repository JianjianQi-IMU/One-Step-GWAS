#ifndef VIEWFILEFORMATWIDGET_HPP
#define VIEWFILEFORMATWIDGET_HPP

#include <QWidget>
#include <QLabel>
#include <QTextBrowser>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFont>
#include <QTextDocument>
#include <QTabWidget>
#include "BaseFileFormat.hpp"

class ViewFileFormatWidget : public QWidget
{
    Q_OBJECT
private:
    QGroupBox*   nameBox;
    QTextBrowser* browser;
    QTextDocument* doc;
    FD::BaseFileFormat format;

    QString exchangeNewLineChar(const QString& inStr);

public:
    explicit ViewFileFormatWidget(QWidget *parent = nullptr);
    explicit ViewFileFormatWidget(const FD::BaseFileFormat& inFormat, QWidget *parent = nullptr);
    void load(const FD::BaseFileFormat& inFormat);
    void load(FD::FileFormClass inFormat);
signals:

};

class ViewFileFormatTable : public QWidget
{
    Q_OBJECT
private:
    QTabWidget* tabWidget;
    bool pri_addNewTab(FD::FileFormClass inFormat);
public:
    explicit ViewFileFormatTable(QWidget *parent = nullptr);
    explicit ViewFileFormatTable(const QVector<FD::FileFormClass>& inVec, QWidget *parent = nullptr);
    void load(const QVector<FD::FileFormClass>& inVec);
    bool addNewTab(FD::FileFormClass inFormat);
signals:

};

#endif // VIEWFILEFORMATWIDGET_HPP

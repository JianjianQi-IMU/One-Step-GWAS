#ifndef STYLECHOOSE_LINESTYLECHOOSE_HPP
#define STYLECHOOSE_LINESTYLECHOOSE_HPP

#include <QComboBox>
#include <QPainter>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QVector>
#include "LineStyleParam.hpp"

class LineStyleDisplayWidget : public QWidget
{
    Q_OBJECT
private:
    LineStyleParam param;
public:
    explicit LineStyleDisplayWidget(const LineStyleParam& inParam = LineStyleParam(),
                                    QWidget* parent = nullptr);
    void setLineParam(const LineStyleParam& inParam);
    LineStyleParam getLineParam() const;
private:
    void paintEvent(QPaintEvent* e) override;
};

class LineStyleDisplayItemWidget : public QLineEdit
{
    Q_OBJECT
private:
    QLabel*                    pLabel;
    LineStyleDisplayWidget*    pStyleWidget;
    int                        index;
public:
    explicit LineStyleDisplayItemWidget(const QString& inStr = QString(),
                                        const LineStyleParam& inParam = LineStyleParam(),
                                        QWidget* parent = nullptr);
    void setIndex(int id);
    void setItem(const QString& inStr = QString(),
                 const LineStyleParam& inParam = LineStyleParam());
    LineStyleParam getLineParam() const;
    QString getLabelText() const;
signals:
    void sendClicked(int id);
protected:
    void mousePressEvent(QMouseEvent* e) override;
};

class LineStyleDisplayComboBox : public QComboBox
{
    Q_OBJECT
private:
    QVector<LineStyleDisplayItemWidget*>    vec;
    QListWidget*                            pListWidget;
    LineStyleDisplayItemWidget*             currentItem;
public:
    explicit LineStyleDisplayComboBox(QWidget* parent = nullptr);
    void setCurrentItem(int id);
    LineStyleParam getCurrentParam() const;
    void addLineItem(const QString& inStr = QString(),
                     const LineStyleParam& inParam = LineStyleParam());
};

#endif

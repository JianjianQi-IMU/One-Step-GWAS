#ifndef WIDGETUTIL_HPP
#define WIDGETUTIL_HPP

#include <QObject>
#include <QString>

class WidgetUtil : QObject
{
    Q_OBJECT

public:
    static int GetQLabelDefWidth(const char *inStr, const char *inFamily,
                                 int inSize);
    static int GetQLabelDefWidth(const QString &inStr, const char *inFamily,
                                 int inSize);
};

#endif

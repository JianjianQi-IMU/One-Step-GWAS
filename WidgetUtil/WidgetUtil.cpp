
#include "WidgetUtil.hpp"

#include <QFontMetrics>
#include <QFont>

int WidgetUtil::GetQLabelDefWidth(const char *inStr, const char *inFamily, int inSize)
{
    QString str(inStr);
    return GetQLabelDefWidth(str, inFamily, inSize);
}

int WidgetUtil::GetQLabelDefWidth(const QString &inStr, const char *inFamily, int inSize)
{
    QFont font(inFamily, inSize);
    QFontMetrics metrics(font);
    return metrics.horizontalAdvance(inStr);
}

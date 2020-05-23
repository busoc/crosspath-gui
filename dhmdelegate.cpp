#include "dhmdelegate.h"
#include "points.h"

DMSDelegate::DMSDelegate(QObject* parent): QStyledItemDelegate(parent)
{

}

QString DMSDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(locale);
    return toDMS(value.toDouble());
}

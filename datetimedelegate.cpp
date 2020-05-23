#include <QDateTime>
#include "datetimedelegate.h"

DateTimeDelegate::DateTimeDelegate(QObject* parent): QStyledItemDelegate(parent)
{

}

QString DateTimeDelegate::displayText(const QVariant &value, const QLocale &locale) const {
    Q_UNUSED(locale);
    return value.toDateTime().toString(Qt::ISODateWithMs);
}

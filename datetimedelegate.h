#ifndef DATETIMEDELEGATE_H
#define DATETIMEDELEGATE_H

#include <QStyledItemDelegate>

class DateTimeDelegate: public QStyledItemDelegate
{
public:
    DateTimeDelegate(QObject* parent = nullptr);
    virtual QString displayText(const QVariant& value, const QLocale& locale) const;
};

#endif // DATETIMEDELEGATE_H

#ifndef DHMDELEGATE_H
#define DHMDELEGATE_H

#include <QStyledItemDelegate>

class DMSDelegate: public QStyledItemDelegate
{
public:
    DMSDelegate(QObject* parent = nullptr);
    virtual QString displayText(const QVariant& value, const QLocale& locale) const;
};

#endif // DHMDELEGATE_H

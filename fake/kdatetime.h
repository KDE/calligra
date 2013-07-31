#ifndef FAKE_KDATETIME_H
#define FAKE_KDATETIME_H

#include <Qt>
#include <QDate>
#include <QTime>
#include <QDateTime>

#include "kofake_export.h"

class KDateTime : public QDateTime
{
public:

    enum TimeFormat
    {
        ISODate = Qt::ISODate,
        RFCDate = Qt::ISODate,
        RFCDateDay = Qt::ISODate,
        QtTextDate = Qt::TextDate,
        LocalDate = Qt::LocaleDate,
        RFC3339Date = Qt::ISODate
    };

    KDateTime(const QDateTime &dt) : QDateTime(dt) {}

    QString toString(TimeFormat tf = ISODate) const
    {
        return QDateTime::toString( (Qt::DateFormat)tf );
    }
    QString toString(Qt::DateFormat df) const
    {
        return QDateTime::toString(df);
    }

    static KDateTime currentLocalDateTime()
    {
        QDateTime dt = QDateTime::currentDateTime();
        return KDateTime(dt);
    }
};

#endif

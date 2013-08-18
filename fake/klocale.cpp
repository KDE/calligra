#include "klocale.h"
#include "kcalendarsystem.h"


KLocale::KLocale(const QByteArray &name)
    : m_calendar(new KCalendarSystem(this))
{
}

KLocale::KLocale(KCalendarSystem *calendar, const QByteArray &name)
    : m_calendar(calendar)
{
}

KLocale::~KLocale()
{
    delete m_calendar;
}

void KLocale::splitLocale(const QString &aLocale, QString &language, QString &country,
                          QString &modifier, QString &charset)
{
    QString locale = aLocale;

    language.clear();
    country.clear();
    modifier.clear();
    charset.clear();

    // In case there are several concatenated locale specifications,
    // truncate all but first.
    int f = locale.indexOf(QLatin1Char(':'));
    if (f >= 0) {
        locale.truncate(f);
    }

    f = locale.indexOf(QLatin1Char('.'));
    if (f >= 0) {
        charset = locale.mid(f + 1);
        locale.truncate(f);
    }

    f = locale.indexOf(QLatin1Char('@'));
    if (f >= 0) {
        modifier = locale.mid(f + 1);
        locale.truncate(f);
    }

    f = locale.indexOf(QLatin1Char('_'));
    if (f >= 0) {
        country = locale.mid(f + 1);
        locale.truncate(f);
    }

    language = locale;
}

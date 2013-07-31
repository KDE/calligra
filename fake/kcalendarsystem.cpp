#include "kcalendarsystem.h"

#include "klocale.h"
#include "kglobal.h"

KCalendarSystem::KCalendarSystem(const KLocale *locale)
    : m_locale(locale ? locale : new KLocale(this))
{
}

KCalendarSystem::~KCalendarSystem()
{
    if (m_locale->calendar() == this)
        delete m_locale;
}

QString KCalendarSystem::formatDate(const QDate &fromDate, KLocale::DateFormat toFormat) const
{
    return m_locale->formatDate(fromDate, toFormat);
}

QString KCalendarSystem::formatDate(const QDate &fromDate, const QString &toFormat, KLocale::DateTimeFormatStandard formatStandard) const
{
    return m_locale->formatDate(fromDate, toFormat);
}

QString KCalendarSystem::formatDate(const QDate &fromDate, const QString &toFormat, KLocale::DigitSet digitSet, KLocale::DateTimeFormatStandard formatStandard) const
{
    return m_locale->formatDate(fromDate, toFormat);
}

QString KCalendarSystem::formatDate(const QDate &date, KLocale::DateTimeComponent component, KLocale::DateTimeComponentFormat format, KLocale::WeekNumberSystem weekNumberSystem) const
{
    return m_locale->formatDate(date, component);
}

QDate KCalendarSystem::readDate(const QString &str, bool *ok) const
{
    return m_locale->readDate(str, ok);
}

QDate KCalendarSystem::readDate(const QString &str, KLocale::ReadDateFlags flags, bool *ok) const
{
    return m_locale->readDate(str, ok);
}

QDate KCalendarSystem::readDate(const QString &dateString, const QString &dateFormat, bool *ok) const
{
    return m_locale->readDate(dateString, dateFormat, ok);
}

QDate KCalendarSystem::readDate(const QString &dateString, const QString &dateFormat, bool *ok, KLocale::DateTimeFormatStandard formatStandard) const
{
    return m_locale->readDate(dateString, dateFormat, ok);
}

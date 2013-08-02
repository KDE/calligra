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

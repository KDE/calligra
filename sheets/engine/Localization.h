/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_LOCALIZATION
#define CALLIGRA_SHEETS_LOCALIZATION

#include <QDebug>
#include <QLocale>

#include <KLocalizedString>
#ifndef Q_OS_WIN
#include <QSharedDataPointer>
#endif

#include "Format.h"

#include "sheets_engine_export.h"

namespace Calligra
{
namespace Sheets
{
class Time;
/**
 * This is a wrapper around QLocale. Matches the old KLocale API.
 * Eventually we'll want to have support for per-document locales, but for now we just use the system one.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT Localization
{
public:
    Localization();
#ifdef _MSC_VER
    Localization(const Localization &other);
#endif
    ~Localization();

    void setDefaultLocale();
    void setLanguage(const QString &name);
    void setLanguage(QLocale::Language language, QLocale::Script script = QLocale::AnyScript, QLocale::Country country = QLocale::AnyCountry);
    QString name() const;

    QString decimalSymbol() const;
    QString negativeSign() const;
    QString positiveSign() const;
    QString thousandsSeparator() const;

    double readNumber(const QString &str, bool *ok) const;
    QDateTime readDateTime(const QString &str, bool *ok) const;
    QDateTime readDateTime(const QString &str, const QString &format, bool *ok) const;
    QDate readDate(const QString &str, bool *ok) const;
    QDate readDate(const QString &str, const QString &format, bool *ok) const;
    Time readTime(const QString &str, bool *ok) const;
    Time readTime(const QString &str, const QString &format, bool *ok) const;

    /**
     * Uses these date/time formats:
     * d = day as number without a leading zero
     * dd = day as number including a leading zero
     * ddd = appreviated name of day (e.g. 'Mon' to 'Sun')
     * dddd = full name of day (e.g. 'Monday' to 'Sunday')
     * M = month as number without a leading zero
     * MM = month as number including a leading zero
     * MMM = appreviated name of month (e.g. 'Jan' to 'Dec')
     * MMMM = full name of month (e.g. 'January' to 'December')
     * yy = two-digit year
     * yyyy = four-digit year
     *
     * h = hour without a leading zero, 0-23 format, or 1-12 if AM/PM display
     * hh = hour with a leading zero, 0-23 format, or 1-12 if AM/PM display
     * H = hour without a leading zero, always 0-23 format
     * HH = hour with a leading zero, always 0-23 format
     * m = minute as number without a leading zero
     * mm = minute as number including a leading zero
     * s = second as number without a leading zero
     * ss = second as number including a leading zero
     * AP/A = uppercase AM/PM
     * ap/a = lowercase am/pm
     * t = timezone
     */
    QString dateTimeFormat(bool longFormat) const;
    QString dateFormat(bool longFormat) const;
    QString timeFormat(bool longFormat) const;
    QString dateTimeFormat(Format::Type type) const;
    QString dateFormat(Format::Type type) const;
    QString timeFormat(Format::Type type) const;

    QString currencySymbol() const;

    QString timeSeparator() const;
    bool timeWithAMPM() const;
    QString dateSeparator(bool longDate) const;
    int firstDayOfWeek() const;
    QString dayName(int day, bool longFormat = true) const;
    /** Name of month, 1-12 */
    QString monthName(int month, bool longFormat = true) const;

    QString formatBool(bool val) const;

    QString formatNumber(double num, int precision = -1) const;
    QString formatCurrency(double num, const QString &currencySymbol, int precision = -1) const;

    /** Formats using the correct decimal point, but no thousands separators */
    QString formatDoubleNoSep(double val) const;
    QString formatDateTime(const QDateTime &datetime, bool longFormat = true) const;
    QString formatDateTime(const QDateTime &datetime, const QString &format) const;
    QString formatDate(const QDate &date, bool longFormat = true) const;
    QString formatDate(const QDate &date, const QString &format) const;
    QString formatTime(const Time &time, bool longFormat = true) const;
    QString formatTime(const Time &time, const QString &format) const;

    QString translateString(KLocalizedString str) const;

    QString toUpper(const QString &str) const;
    QString toLower(const QString &str) const;

    QString languageName(bool full) const;

private:
    class Private;
#ifndef _MSC_VER
    QSharedDataPointer<Private> d;
#else
    // HACK to get this to compile on msvc
    Private *pd;
    class PrivateData;
    PrivateData *d = nullptr;
#endif
    void updateDateTimeFormats();
    void updateDateFormats();
    void updateTimeFormats();
    void setLocale(const QLocale &l);
    QString timeToRegExp(const QString &format, bool neg = false) const;
};

} // namespace Sheets
} // namespace Calligra

CALLIGRA_SHEETS_ENGINE_EXPORT QDebug operator<<(QDebug dbg, const Calligra::Sheets::Localization *l);
CALLIGRA_SHEETS_ENGINE_EXPORT QDebug operator<<(QDebug dbg, const Calligra::Sheets::Localization &l);

#endif // CALLIGRA_SHEETS_LOCALIZATION

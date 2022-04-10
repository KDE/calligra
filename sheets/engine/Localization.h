/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_LOCALIZATION
#define CALLIGRA_SHEETS_LOCALIZATION

#include <QLocale>

#include <KLocalizedString>

#include "sheets_engine_export.h"

namespace Calligra
{
namespace Sheets
{

/**
 * This is a wrapper around QLocale. Matches the old KLocale API.
 * Eventually we'll want to have support for per-document locales, but for now we just use the system one.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT Localization
{
public:
    Localization();

    void defaultSystemConfig();

    QString decimalSymbol() const;
    QString negativeSign() const;
    QString positiveSign() const;
    QString thousandsSeparator() const;

    double readNumber(const QString &str, bool *ok) const;
    QDateTime readDateTime(const QString &str, bool *ok) const;
    QDateTime readDateTime(const QString &str, const QString &format, bool *ok) const;
    QDate readDate(const QString &str, bool *ok) const;
    QDate readDate(const QString &str, const QString &format, bool *ok) const;
    QTime readTime(const QString &str, bool *ok) const;
    QTime readTime(const QString &str, const QString &format, bool *ok) const;

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
    QString dateFormat(int type) const;

    QString currencySymbol() const;

    QString timeSeparator() const { return timeSep; }
    bool timeWithAMPM() const { return includesAMPM; }
    QString dateSeparator(bool longDate) const { return longDate ? dateSepLong : dateSepShort; }
    int firstDayOfWeek() const;
    QString dayName(int day) const;
    /** Name of month, 1-12 */
    QString monthName(int month) const;

    QString formatBool(bool val) const;

    QString formatNumber(double num, int precision = -1) const;
    QString formatCurrency(double num, const QString &currencySymbol, int precision = -1) const;

    /** Formats using the correct decimal point, but no thousands separators */
    QString formatDoubleNoSep(double val) const;
    QString formatDateTime(const QDateTime &datetime, bool longFormat = true) const;
    QString formatDateTime(const QDateTime &datetime, const QString &format) const;
    QString formatDate(const QDate &date, bool longFormat = true) const;
    QString formatDate(const QDate &date, const QString &format) const;
    QString formatTime(const QTime &time, bool longFormat = true) const;
    QString formatTime(const QTime &time, const QString &format) const;

    QString translateString(KLocalizedString str) const;

    QString toUpper(const QString &str) const;
    QString toLower(const QString &str) const;

    QString languageName(bool full) const;
private:
    void setLocale(const QLocale &l);

    QLocale locale;
    QString timeSep, dateSepShort, dateSepLong;
    bool includesAMPM;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LOCALIZATION

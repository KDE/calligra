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
    QString thousandsSeparator() const;

    QDateTime readDateTime(const QString &str, bool *ok);
    QDateTime readDateTime(const QString &str, const QString &format, bool *ok);
    QDate readDate(const QString &str, bool *ok);
    QDate readDate(const QString &str, const QString &format, bool *ok);
    QTime readTime(const QString &str, bool *ok);
    QTime readTime(const QString &str, const QString &format, bool *ok);

    QString dateTimeFormat(bool longFormat);
    QString dateFormat(bool longFormat);
    QString timeFormat(bool longFormat);

    QString formatBool(bool val) const;
    /** Formats using the correct decimal point, but no thousands separators */
    QString formatDoubleNoSep(double val) const;
    QString formatDateTime(const QDateTime &datetime, bool longFormat = true);
    QString formatDateTime(const QDateTime &datetime, const QString &format);
    QString formatDate(const QDate &date, bool longFormat = true);
    QString formatDate(const QDate &date, const QString &format);
    QString formatTime(const QTime &time, bool longFormat = true);
    QString formatTime(const QTime &time, const QString &format);

    QString translateString(KLocalizedString str) const;

    QString toUpper(const QString &str) const;
    QString toLower(const QString &str) const;
private:
    QLocale locale;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LOCALIZATION

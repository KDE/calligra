/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003-2006 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2006 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2009-2010 Nokia Corporation and /or its subsidiary(-ies).
   Contact: Manikandaprasad Chandrasekar <manikandaprasad.chandrasekar@nokia.com>
   SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "ImportUtils.h"

#include <NumberFormatParser.h>
#include <QRegularExpression>

namespace XlsUtils
{

Q_LOGGING_CATEGORY(lcExcelImport, "calligra.filter.xls2ods")

QString removeEscaped(const QString &text, bool removeOnlyEscapeChar)
{
    QString s(text);
    int pos = 0;
    while (true) {
        pos = s.indexOf('\\', pos);
        if (pos < 0)
            break;
        if (removeOnlyEscapeChar) {
            s = s.left(pos) + s.mid(pos + 1);
            pos++;
        } else {
            s = s.left(pos) + s.mid(pos + 2);
        }
    }
    return s;
}

QString extractLocale(QString &time)
{
    QString locale;
    if (time.startsWith("[$-")) {
        int pos = time.indexOf(']');
        if (pos > 3) {
            locale = time.mid(3, pos - 3);
            time.remove(0, pos + 1);
            pos = time.lastIndexOf(';');
            if (pos >= 0) {
                time = time.left(pos);
            }
        }
    }
    return locale;
}

bool isPercentageFormat(const QString &valueFormat)
{
    int length = valueFormat.length();
    if (length < 1)
        return false;
    return valueFormat[length - 1] == QChar('%');
}

bool isTimeFormat(const QString &valueFormat)
{
    QString vf = valueFormat;
    QString locale = extractLocale(vf);
    Q_UNUSED(locale);
    vf = removeEscaped(vf);

    // if there is still a time formatting picture item that was not escaped
    // and therefore removed above, then we have a time format here.
    QRegularExpression ex("(h|H|m|s)");
    return vf.indexOf(ex) >= 0;
}

bool isFractionFormat(const QString &valueFormat)
{
    QRegularExpression ex("^#[?]+/[0-9?]+$");
    QString vf = removeEscaped(valueFormat);
    return vf.indexOf(ex) >= 0;
}

bool isDateFormat(const QString &valueFormat)
{
    return NumberFormatParser::isDateFormat(valueFormat);
}

CellFormatKey::CellFormatKey(const Swinder::Format *format, const QString &formula)
    : format(format)
    , isGeneral(format->valueFormat() == "General")
    , decimalCount(-1)
{
    if (!isGeneral) {
        if (formula.startsWith(QLatin1String("msoxl:="))) { // special cases
            QRegularExpression roundRegExp("^msoxl:=ROUND[A-Z]*\\(.*;[\\s]*([0-9]+)[\\s]*\\)$");
            QRegularExpressionMatch match;
            if (formula.indexOf(roundRegExp, 0, &match) >= 0) {
                bool ok = false;
                int decimals = match.captured(1).trimmed().toInt(&ok);
                if (ok) {
                    decimalCount = decimals;
                }
            }
        } else if (formula.startsWith(QLatin1String("msoxl:=RAND("))) {
            decimalCount = 9;
        }
    }
}

bool CellFormatKey::operator==(const CellFormatKey &b) const
{
    return format == b.format && isGeneral == b.isGeneral && decimalCount == b.decimalCount;
}

}

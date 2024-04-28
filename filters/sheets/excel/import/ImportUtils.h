/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003-2006 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2006 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2009-2010 Nokia Corporation and /or its subsidiary(-ies).
   Contact: Manikandaprasad Chandrasekar <manikandaprasad.chandrasekar@nokia.com>
   SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef IMPORTUTILS_H
#define IMPORTUTILS_H

#include <QDebug>
#include <QHash>
#include <QLoggingCategory>
#include <QString>

#include "../sidewinder/format.h"
#include "../sidewinder/value.h"

namespace XlsUtils
{

/// Remove via the "\" char escaped characters from the string.
QString removeEscaped(const QString &text, bool removeOnlyEscapeChar = false);
/// extract and return locale and remove locale from time string.
QString extractLocale(QString &time);

bool isPercentageFormat(const QString &valueFormat);
bool isTimeFormat(const QString &valueFormat);
bool isFractionFormat(const QString &valueFormat);
bool isDateFormat(const QString &valueFormat);

struct CellFormatKey {
    const Swinder::Format *format;
    bool isGeneral;
    int decimalCount;
    CellFormatKey(const Swinder::Format *format, const QString &formula);
    bool operator==(const CellFormatKey &b) const;
};

static inline uint qHash(const CellFormatKey &key)
{
    return ::qHash(key.format) ^ ::qHash(key.decimalCount);
}

Q_DECLARE_LOGGING_CATEGORY(lcExcelImport);

}

#endif // IMPORTUTILS_H

/* This file is part of the KDE project
   Copyright (C) 2003-2006 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
   Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
   Contact: Manikandaprasad Chandrasekar <manikandaprasad.chandrasekar@nokia.com>
   Copyright (c) 2010 Carlos Licea <carlos@kdab.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef IMPORTUTILS_H
#define IMPORTUTILS_H

#include <QString>
#include <QHash>

#include "../sidewinder/format.h"
#include "../sidewinder/value.h"

namespace XlsUtils {

/// Remove via the "\" char escaped characters from the string.
QString removeEscaped(const QString &text, bool removeOnlyEscapeChar = false);
/// extract and return locale and remove locale from time string.
QString extractLocale(QString &time);

bool isPercentageFormat(const QString& valueFormat);
bool isTimeFormat(const Swinder::Value &value, const QString& valueFormat);
bool isFractionFormat(const QString& valueFormat);
bool isDateFormat(const Swinder::Value &value, const QString& valueFormat);

struct CellFormatKey {
    const Swinder::Format* format;
    bool isGeneral;
    int decimalCount;
    CellFormatKey(const Swinder::Format* format, const QString& formula);
    bool operator==(const CellFormatKey& b) const;
};

static inline uint qHash(const CellFormatKey& key)
{
    return ::qHash(key.format) ^ ::qHash(key.decimalCount);
}

}

#endif // IMPORTUTILS_H

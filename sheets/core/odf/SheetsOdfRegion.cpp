/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2016 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2016 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2005-2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2002 Phillip Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1999-2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Region.h"
#include "SheetsOdf.h"
#include "SheetsOdfPrivate.h"

// This file contains functionality to load/save regions

namespace Calligra
{
namespace Sheets
{

static void append(const QChar *from, const QChar *to, QChar **dest)
{
    while (from < to) {
        **dest = *from;
        ++from;
        ++*dest;
    }
}

void Odf::loadRegion(const QChar *&data, const QChar *&end, QChar *&out)
{
    enum {
        Start,
        InQuotes
    } state = Start;

    if (*data == QChar('$', 0)) {
        ++data;
    }

    bool isRange = false;

    const QChar *pos = data;
    while (data < end) {
        switch (state) {
        case Start:
            switch (data->unicode()) {
            case '\'': // quoted sheet name or named area
                state = InQuotes;
                break;
            case '.': // sheet name separator
                if (pos != data && !isRange) {
                    append(pos, data, &out);
                    *out = QChar('!', 0);
                    ++out;
                }
                pos = data;
                ++pos;
                break;
            case ':': { // cell separator
                isRange = true;
                append(pos, data, &out);
                *out = *data; // append :
                ++out;
                const QChar *next = data + 1;
                if (!next->isNull()) {
                    const QChar *nextnext = next + 1;
                    if (!nextnext->isNull() && *next == QChar('$', 0) && *nextnext != QChar('.', 0)) {
                        ++data;
                    }
                }
                pos = data + 1;
            } break;
            case ' ': // range separator
                append(pos, data, &out);
                *out = QChar(';', 0);
                ++out;
                pos = data;
                break;
            default:
                break;
            }
            break;
        case InQuotes:
            if (data->unicode() == '\'') {
                // an escaped apostrophe?
                // As long as Calligra Sheets does not support fixed sheets eat the dollar sign.
                const QChar *next = data + 1;
                if (!next->isNull() && *next == QChar('\'', 0)) {
                    ++data;
                } else { // the end
                    state = Start;
                }
            }
            break;
        }
        ++data;
    }
    append(pos, data, &out);
}

QString Odf::loadRegion(const QString &expression)
{
    QString result;
    QString temp;
    bool isRange = false;
    enum {
        Start,
        InQuotes
    } state = Start;
    int i = 0;
    // NOTE Stefan: As long as Calligra Sheets does not support fixed sheets eat the dollar sign.
    if (expression[i] == '$')
        ++i;
    while (i < expression.count()) {
        switch (state) {
        case Start: {
            if (expression[i] == '\'') { // quoted sheet name or named area
                temp.append(expression[i]);
                state = InQuotes;
            } else if (expression[i] == '.') { // sheet name separator
                // was there already a sheet name?
                if (!temp.isEmpty() && !isRange) {
                    result.append(temp);
                    result.append('!');
                }
                temp.clear();
            } else if (expression[i] == ':') { // cell separator
                isRange = true;
                result.append(temp);
                result.append(':');
                temp.clear();
                // NOTE Stefan: As long as Calligra Sheets does not support fixed sheets eat the dollar sign.
                if (i + 2 < expression.count() && expression[i + 1] == '$' && expression[i + 2] != '.')
                    ++i;
            } else if (expression[i] == ' ') { // range separator
                result.append(temp);
                result.append(';');
                temp.clear();
            } else
                temp.append(expression[i]);
            ++i;
            break;
        }
        case InQuotes: {
            temp.append(expression[i]);
            if (expression[i] == '\'') {
                // an escaped apostrophe?
                if (i + 1 < expression.count() && expression[i + 1] == '\'')
                    ++i; // eat it
                else // the end
                    state = Start;
            }
            ++i;
            break;
        }
        }
    }
    return result + temp;
}

QString Odf::saveRegion(const QString &expression)
{
    QString result;
    QString sheetName;
    QString temp;
    enum {
        Start,
        InQuotes
    } state = Start;
    int i = 0;
    while (i < expression.count()) {
        switch (state) {
        case Start: {
            if (expression[i] == '\'') {
                temp.append(expression[i]);
                state = InQuotes;
            } else if (expression[i] == '!') { // sheet name separator
                // There has to be a sheet name.
                if (temp.isEmpty())
                    return expression; // error
                if (temp.count() > 2 && (temp[0] != '\'' && temp[temp.count() - 1] != '\'')) {
                    temp.replace('\'', "''");
                    if (temp.contains(' ') || temp.contains('.') || temp.contains(';') || temp.contains('!') || temp.contains('$') || temp.contains(']'))
                        temp = '\'' + temp + '\'';
                }
                sheetName = temp;
                result.append(temp);
                result.append('.');
                temp.clear();
            } else if (expression[i] == ':') { // cell separator
                if (result.isEmpty())
                    result = '.';
                result.append(temp);
                result.append(':');
                result.append(sheetName);
                result.append('.');
                temp.clear();
            } else if (expression[i] == ';') { // range separator
                result.append(temp);
                result.append(' ');
                temp.clear();
            } else
                temp.append(expression[i]);
            ++i;
            break;
        }
        case InQuotes: {
            temp.append(expression[i]);
            if (expression[i] == '\'') {
                // an escaped apostrophe?
                if (i + 1 < expression.count() && expression[i + 1] == '\'')
                    ++i; // eat it
                else // the end
                    state = Start;
            }
            ++i;
            break;
        }
        }
    }
    if (result.isEmpty())
        result = '.';
    return result + temp;
}

QString Odf::saveRegion(Region *region)
{
    return saveRegion(region->name());
}

} // Sheets
} // Calligra

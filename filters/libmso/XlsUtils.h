/*
 *  SPDX-FileCopyrightText: 2010 Sebastian Sauer <sebsauer@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef XLSUTILS_H
#define XLSUTILS_H

#include <QDebug>
#include <QPoint>
#include <QRegularExpression>
#include <math.h>

// translate the range-character to a number
inline int rangeCharToInt(char c)
{
    return (c >= 'A' && c <= 'Z') ? (c - 'A' + 1) : -1;
}

// translates the range-string into a number
inline int rangeStringToInt(const QString &string)
{
    int result = 0;
    const int size = string.size();
    for (int i = 0; i < size; i++)
        result += rangeCharToInt(string[i].toLatin1()) * pow(10.0, (size - i - 1));
    return result;
}

// splits a given cellrange like Sheet1.D2:Sheet1.F2, Sheet1.D2:F2, D2:F2 or D2 into its parts
inline std::pair<QString, QRect> splitCellRange(QString range)
{
    range.remove('$'); // remove "fixed" character
    // remove []
    if (range.startsWith(QLatin1Char('[')) && range.endsWith(QLatin1Char(']'))) {
        range.remove(0, 1).chop(1);
    }
    std::pair<QString, QRect> result;
    const bool isPoint = !range.contains(':');
    QRegularExpression regEx =
        isPoint ? QRegularExpression("(.*)(\\.|\\!)([A-Z]+)([0-9]+)") : QRegularExpression("(.*)(\\.|\\!)([A-Z]+)([0-9]+)\\:(|.*\\.)([A-Z]+)([0-9]+)");
    QRegularExpressionMatch match;
    if (range.indexOf(regEx, 0, &match) >= 0) {
        const QString sheetName = match.captured(1);
        QPoint topLeft(rangeStringToInt(match.captured(3)), match.captured(4).toInt());
        if (isPoint) {
            result = std::pair<QString, QRect>(sheetName, QRect(topLeft, QSize(1, 1)));
        } else {
            const QPoint bottomRight(rangeStringToInt(match.captured(6)), match.captured(7).toInt());
            result = std::pair<QString, QRect>(sheetName, QRect(topLeft, bottomRight));
        }
    }
    return result;
}

#endif

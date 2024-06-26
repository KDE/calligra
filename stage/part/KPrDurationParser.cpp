/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPrDurationParser.h"

#include <QGlobalStatic>
#include <QRegularExpression>
#include <QStringList>

// define the regex used as statics
static const QRegularExpression clockRegEx("^(?:(\\d+):)?(\\d{2}):(\\d{2}(?:\\.\\d+)?)$");
static const QRegularExpression timecountRegEx("^(\\d+(?:\\.\\d+)?)(h|min|s|ms)?$");

int KPrDurationParser::durationMs(const QString &duration)
{
    int ms = -1;
    QRegularExpressionMatch match;
    if (duration.indexOf(clockRegEx, 0, &match) != -1) {
        QStringList result(match.capturedTexts());
        bool ok = true;
        ms = qreal(result[1].toInt(&ok, 10) * 3600 + result[2].toInt(&ok, 10) * 60 + result[3].toDouble(&ok)) * 1000;
    } else if (duration.indexOf(timecountRegEx, 0, &match) != -1) {
        QStringList result(match.capturedTexts());
        qreal factor = 1000;
        // the default (no metric) is s
        if (result.length() > 2) {
            QString metric(result[2]);
            if (metric != "s") {
                if (metric == "min") {
                    factor = 60 * 1000;
                } else if (metric == "ms") {
                    factor = 1;
                } else if (metric == "h") {
                    factor = 3600 * 1000;
                }
            }
        }
        bool ok = true;
        ms = qRound(qreal(result[1].toDouble(&ok) * factor));
    }
    return ms;
}

QString KPrDurationParser::msToString(const int ms)
{
    QString result;
    if (ms % 1000 == 0) {
        result = QString("%1s").arg(ms / 1000);
    } else {
        result = QString("%1ms").arg(ms);
    }
    return result;
}

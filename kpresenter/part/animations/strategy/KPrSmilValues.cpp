/* This file is part of the KDE project
 * Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
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

#include "KPrSmilValues.h"
#include <QStringList>
#include "KoXmlWriter.h"
#include "kdebug.h"

KPrSmilValues::KPrSmilValues(KoShape *shape) : KPrAnimationValue(shape)
{
}

qreal KPrSmilValues::value(qreal time) const
{
    qreal value, value1, value2 = 0.0;
    for (int i = 0; i < m_values.size(); i++) {
        if (time > m_times.at(i) && (m_times.at(i+1) - m_times.at(i))) {
            value1 = m_values.at(i).eval(m_cache);
            value2 = m_values.at(i+1).eval(m_cache);
            value = (time - m_times.at(i)) * (value2 - value1);
            value = value / (m_times.at(i+1) - m_times.at(i));
            value += value1;
        }
        else if (time == m_times.at(i)){
            value = m_values.at(i).eval(m_cache);;
        }
    }
    return value;
}

qreal KPrSmilValues::startValue() const
{
    return m_values.at(0).eval(m_cache);
}

qreal KPrSmilValues::endValue() const
{
    return m_values.at(m_values.size() - 1).eval(m_cache);
}

bool KPrSmilValues::loadValues(QString values, QString keyTimes, QString keySplines, SmilCalcMode calcMode)
{
    m_calcMode = calcMode;

    QStringList valuesList = values.split(";");
    if (valuesList.size() < 2) {
        return false;
    }

    foreach (QString value, valuesList) {
        KPrValueParser parser(value, m_shape);
        if (!parser.valid()) {
            return false;
        }
        m_values.append(parser);
    }

    // keyTimes
    if (keyTimes.isEmpty()) {
        for (int i = 0; i < m_values.size(); i++) {
            m_times.append(qreal(i)/qreal(m_values.size()));
        }
    }
    else {
        QStringList keyTimesList = keyTimes.split(";");
        if (valuesList.size() != keyTimesList.size()) {
            return false;
        }
        for (int i = 0; i < keyTimesList.size(); i++) {
            QString timeString = keyTimesList.at(i);
            qreal time = timeString.toDouble();
            if ((i == 0 && time != 0) || (i == keyTimesList.size() - 1 && time != 1)) {
                return false;
            }
            m_times.append(time);
        }
    }

    // keySplines
    if (m_calcMode ==  KPrAnimationValue::spline) {
        kWarning(33003) << "keySpline not yes supported";
        QStringList keySplinesList = keySplines.split(";");
    }
    return true;
}

bool KPrSmilValues::saveOdf(KoPASavingContext &paContext) const
{
    KoXmlWriter &writer = paContext.xmlWriter();
    // values
    QString values;
    foreach (KPrValueParser valueParser, m_values) {
        if (values.isEmpty()) {
            values = QString("%1").arg(valueParser.formula());
        } else {
            values.append(QString(";%1").arg(valueParser.formula()));
        }
    }
    writer.addAttribute("smil:values", values);
    // keyTimes
    QString keyTimes;
    foreach (qreal time, m_times) {
        if (keyTimes.isEmpty()) {
            keyTimes = QString("%1").arg(time);
        } else {
            keyTimes.append(QString(";%1").arg(time));
        }
    }
    writer.addAttribute("smil:keyTimes", keyTimes);
    return true;
}

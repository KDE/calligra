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
#include "kdebug.h"

KPrSmilValues::KPrSmilValues()
{
}

qreal KPrSmilValues::value(qreal time) const{
    qreal value;
    for(int i =0; i < m_values.size(); i++ != 0){
        if (time > m_times.at(i) && (m_times.at(i+1) - m_times.at(i))) {
            value = (time - m_times.at(i)) * (m_values.at(i+1) - m_values.at(i));
            value = value / (m_times.at(i+1) - m_times.at(i));
            value += m_values.at(i);
        }
        else if (time == m_times.at(i)){
            value = m_values.at(i);
        }
    }
    return value;
}

qreal KPrSmilValues::startValue() const
{
    return value(0);
}

qreal KPrSmilValues::endValue() const
{
    return value(1);
}

bool KPrSmilValues::loadValues(QString values, QString keyTimes, QString keySplines, SmilCalcMode calcMode)
{
    m_calcMode = calcMode;

    QStringList valuesList = values.split(";");
    if (valuesList.size() < 2) {
        return false;
    }
    // TODO parse value
    m_values.append(0);
    m_values.append(500);

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

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

#include "KPrShapeAnimations.h"
#include "KPrFormulaParser.h"
#include "StageDebug.h"

#include "KoXmlWriter.h"

#include <QStringList>

KPrSmilValues::KPrSmilValues(KPrShapeAnimation *shapeAnimation)
    : KPrAnimationValue(shapeAnimation)
    , m_formulaParser(0)
{
}

KPrSmilValues::~KPrSmilValues()
{
    delete m_formulaParser;
}

qreal KPrSmilValues::value(qreal time) const
{
    qreal value = 0.0;
    qreal value1 = 0.0;
    qreal value2 = 0.0;

    if (m_formulaParser) {
        value = m_formulaParser->eval(m_cache, time);
    }
    else {
        for (int i = 0; i < m_values.size(); i++) {
            if (time > m_times.at(i) && (m_times.at(i + 1) - m_times.at(i))) {
                value1 = m_values.at(i).eval(m_cache);
                value2 = m_values.at(i + 1).eval(m_cache);
                value = (time - m_times.at(i)) * (value2 - value1);
                value = value / (m_times.at(i + 1) - m_times.at(i));
                value += value1;
            }
            else if (time == m_times.at(i)){
                value = m_values.at(i).eval(m_cache);
            }
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

bool KPrSmilValues::loadValues(const QString &values, const QString &keyTimes, const QString &keySplines, SmilCalcMode calcMode)
{
    Q_UNUSED(keySplines);
    m_calcMode = calcMode;
    QStringList valuesList = values.split(QLatin1Char(';'));
    if (valuesList.size() < 2) {
        return false;
    }

    foreach (const QString &value, valuesList) {
        KPrFormulaParser parser(value, m_shape, m_textBlockData, KPrFormulaParser::Values);
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
        QStringList keyTimesList = keyTimes.split(QLatin1Char(';'));
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
        warnStageAnimation << "keySpline not yes supported";
//         QStringList keySplinesList = keySplines.split(QLatin1Char(';'));
    }
    return true;
}

bool KPrSmilValues::loadFormula(const QString &values, const QString &keyTimes, const QString &keySplines, KPrAnimationValue::SmilCalcMode calcMode, const QString &formula)
{
    bool retval = loadValues(values, keyTimes, keySplines, calcMode);

    //formula
    if (!formula.isEmpty()) {
        m_formulaParser = new KPrFormulaParser(formula, m_shape, m_textBlockData, KPrFormulaParser::Formula);
        if (!m_formulaParser->valid()) {
            m_formulaParser = 0;
            return false;
        }
    } else {
        return false;
    }
    return retval;
}

bool KPrSmilValues::saveOdf(KoPASavingContext &paContext) const
{
    KoXmlWriter &writer = paContext.xmlWriter();
    // values
    QString values;
    foreach (const KPrFormulaParser &valueParser, m_values) {
        if (values.isEmpty()) {
            values = QString("%1").arg(valueParser.formula());
        } else {
            values.append(QString(";%1").arg(valueParser.formula()));
        }
    }
    writer.addAttribute("smil:values", values);
    //Formula
    if (m_formulaParser) {
        QString formula = m_formulaParser->formula();
        if (!formula.isEmpty()) {
            writer.addAttribute("anim:formula", formula);
        }
    }
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

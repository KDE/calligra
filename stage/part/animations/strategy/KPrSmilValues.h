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

#ifndef KPRSMILVALUES_H
#define KPRSMILVALUES_H

#include "KPrAnimationValue.h"
#include <QList>
#include <QPair>
class KPrFormulaParser;

class KPrShapeAnimation;


class KPrSmilValues : public KPrAnimationValue
{
public:
    explicit KPrSmilValues(KPrShapeAnimation *shapeAnimation);
    ~KPrSmilValues() override;

    qreal value(qreal time) const override;
    qreal endValue() const override;
    qreal startValue() const override;
    bool loadValues(const QString &values, const QString &keyTimes, const QString &keySplines, SmilCalcMode calcMode);
    bool loadFormula(const QString &values, const QString &keyTimes, const QString &keySplines, SmilCalcMode calcMode, const QString &formula);
    bool saveOdf(KoPASavingContext &paContext) const override;
protected:
    QList<KPrFormulaParser> m_values;
    QList<qreal> m_times;
    QList<qreal> m_splines;
    KPrFormulaParser *m_formulaParser;
};

#endif // KPRSMILVALUES_H

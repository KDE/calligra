/* This file is part of the KDE project
 * Copyright ( C ) 2010 Casper Boemann <cbo@boemannn.dk>
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
#include "KPrAnimationCache.h"

#include <QMap>
#include <QString>
#include <QVariant>

#include <KoShape.h>
#include <KoTextBlockData.h>

KPrAnimationCache::KPrAnimationCache()
{
}

KPrAnimationCache::~KPrAnimationCache()
{
}


bool KPrAnimationCache::hasValue(KoShape *shape, const QString &id)
{
    if (m_shapeValues.contains(shape))
        return m_shapeValues.value(shape).contains(id);
    return false;
}

bool KPrAnimationCache::hasValue(KoTextBlockData *textBlockData, const QString &id)
{
    if (m_textBlockDataValues.contains(textBlockData))
        return m_textBlockDataValues.value(textBlockData).contains(id);
    return false;
}

void KPrAnimationCache::setValue(KoShape *shape, const QString &id, const QVariant &value)
{
    m_shapeValues[shape][id] = value;
}

void KPrAnimationCache::setValue(KoTextBlockData *textBlockData, const QString &id, const QVariant &value)
{
    m_textBlockDataValues[textBlockData][id] = value;
}

QVariant KPrAnimationCache::value(KoShape *shape, const QString &id, const QVariant &defaultValue)
{
    if (m_shapeValues.contains(shape))
        return m_shapeValues.value(shape).value(id, defaultValue);
    return defaultValue;
}

QVariant KPrAnimationCache::value(KoTextBlockData *textBlockData, const QString &id, const QVariant &defaultValue)
{
    if (m_textBlockDataValues.contains(textBlockData))
        return m_textBlockDataValues.value(textBlockData).value(id, defaultValue);
    return defaultValue;
}

void KPrAnimationCache::init(int step, KoShape *shape, const QString &id, const QVariant &value, const QVariant &backValue)
{
    for (int i = m_shapeValuesStack.size(); i <= step; ++i) {
        // copy previous values
        if (i > 0) {
            m_shapeValuesStack.append(m_shapeValuesStack[i-1]);
        }
        else {
            m_shapeValuesStack.append(QMap<KoShape *, QMap<QString, QVariant> >());
        }
    }
    // check if value is valid
    if (value.isValid()) {
        m_shapeValuesStack[step][shape][id] = value;
    }
    else {
        m_shapeValuesStack[step][shape].remove(id);
    }
    if (backValue.isValid()) {
        bool isSet = false;
        for (int i = 0; i < step; ++i) {
            if (hasValue(i, shape, id)) {
                isSet = true;
                break;
            }
        }
        if (!isSet) {
            for (int i = 0; i < step; ++i) {
                m_shapeValuesStack[i][shape][id];
            }
        }
    }
}

void KPrAnimationCache::update(int step, KoShape *shape, const QString &id, const QVariant &value)
{
    // TODO check if step is the currently selected step?
    // should we pass the step at all?
    // we need special handling for some types of value but that is for later
    m_shapeValues[shape][id] = value;
}

void KPrAnimationCache::startStep(int step)
{
    // TODO check if step exist
    m_shapeValues = m_shapeValuesStack.value(step);
}

QVariant KPrAnimationCache::value(int step, KoShape *shape, const QString &id, const QVariant &defaultValue)
{
    // TODO is it needed?
}

bool KPrAnimationCache::hasValue(int step, KoShape *shape, const QString &id)
{
    // TODO optimize as using first contains and then value is a waste of resources
    // add test if step exists
    if (m_shapeValuesStack[step].contains(shape))
        return m_shapeValuesStack[step].value(shape).contains(id);
    return false;
}

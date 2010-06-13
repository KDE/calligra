/* This file is part of the KDE project
 * Copyright ( C ) 2010 Casper Boemann <cbo@boemannn.dk>
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
#include "KPrAnimationCache.h"
#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>

#include <KoShape.h>
#include <KoTextBlockData.h>

#include "kdebug.h"

KPrAnimationCache::KPrAnimationCache()
{
}

KPrAnimationCache::~KPrAnimationCache()
{
}


bool KPrAnimationCache::hasValue(KoShape *shape, const QString &id)
{
    if (m_shapeValuesStack[m_step].contains(shape))
        return m_shapeValuesStack[m_step].value(shape).contains(id);
    return false;
}

bool KPrAnimationCache::hasValue(KoTextBlockData *textBlockData, const QString &id)
{
    if (m_textBlockDataValuesStack[m_step].contains(textBlockData))
        return m_textBlockDataValuesStack[m_step].value(textBlockData).contains(id);
    return false;
}

void KPrAnimationCache::setValue(int step, KoShape *shape, const QString &id, const QVariant &value)
{
    m_shapeValuesStack[step][shape][id] = value;
}

void KPrAnimationCache::setValue(KoTextBlockData *textBlockData, const QString &id, const QVariant &value)
{
    m_textBlockDataValuesStack[m_step][textBlockData][id] = value;
}

QVariant KPrAnimationCache::value(KoShape *shape, const QString &id, const QVariant &defaultValue)
{
    if (m_currentShapeValues.contains(shape))
        return m_currentShapeValues.value(shape).value(id, defaultValue);
    return defaultValue;
    /*if (m_shapeValuesStack[m_step].contains(shape))
        return m_shapeValuesStack[m_step].value(shape).value(id, defaultValue);
    return defaultValue;*/
}

QVariant KPrAnimationCache::value(int step, KoShape *shape, const QString &id)
{
    if (m_shapeValuesStack[step].contains(shape))
        return m_shapeValuesStack[step].value(shape).value(id);
    return QVariant();
}


QVariant KPrAnimationCache::value(KoTextBlockData *textBlockData, const QString &id, const QVariant &defaultValue)
{
    if (m_textBlockDataValuesStack[m_step].contains(textBlockData))
        return m_textBlockDataValuesStack[m_step].value(textBlockData).value(id, defaultValue);
    return defaultValue;
}

void KPrAnimationCache::init(int step, KoShape *shape, const QString &id, const QVariant &value)
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

    // Check visibility
    if(id == "visibility")
    {
        for(int i = step - 1; i >= 0; i--)
        {
            if(!this->hasValue(i, shape, id)){
                this->setValue(i, shape, id, value.toBool());
            }
        }
    }
}

void KPrAnimationCache::update(KoShape *shape, const QString &id, const QVariant &value)
{
    // we need special handling for some types of value but that is for later
    m_currentShapeValues[shape][id] = value;
}

void KPrAnimationCache::startStep(int step)
{
    m_step = step;
    if(m_shapeValuesStack.size() > step)
        m_currentShapeValues = m_shapeValuesStack[m_step];
}

void KPrAnimationCache::endStep(int step)
{
    m_step = step;
    if(m_shapeValuesStack.size() > step)
        m_currentShapeValues = m_shapeValuesStack[m_step+1];
}


bool KPrAnimationCache::hasValue(int step, KoShape *shape, const QString &id)
{
    // TODO optimize as using first contains and then value is a waste of resources
    // add test if step exists
    if (m_shapeValuesStack[step].contains(shape))
        return m_shapeValuesStack[step].value(shape).contains(id);
    return false;
}

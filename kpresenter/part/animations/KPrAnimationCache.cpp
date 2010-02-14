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

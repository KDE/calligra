/* This file is part of the KDE project
 * Copyright (C) 2011 C. Boemann <cbo@boemann.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#include "MockRootAreaProvider.h"

#include "KoTextLayoutRootArea.h"

#include <QDebug>

MockRootAreaProvider::MockRootAreaProvider()
    : maxPosition(0)
    , m_suggestedRect(QRectF(100, 100, 200, 1000))
    , m_askedForMoreThenOneArea(false)
{
}

KoTextLayoutRootArea *MockRootAreaProvider::provide(KoTextDocumentLayout *documentLayout, const RootAreaConstraint &constraint, int requestedPosition, bool *isNewRootArea)
{
    if (maxPosition > 0 && requestedPosition > maxPosition) {
        qInfo()<<"To many area requests:"<<maxPosition<<requestedPosition;
        return 0; // guard against loop
    }
    m_askedForMoreThenOneArea |= (m_areas.count() > 1);
    *isNewRootArea = !m_areas.contains(requestedPosition);
    if (!m_areas.contains(requestedPosition)) {
        m_areas.insert(requestedPosition, new KoTextLayoutRootArea(documentLayout));
        qInfo()<<"New area provided:"<<requestedPosition;
    }
    return m_areas.value(requestedPosition);
}

void MockRootAreaProvider::doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea)
{
    Q_UNUSED(rootArea);
    Q_UNUSED(isNewRootArea);
}

void MockRootAreaProvider::updateAll()
{
}

void MockRootAreaProvider::releaseAllAfter(KoTextLayoutRootArea *afterThis)
{
    Q_UNUSED(afterThis);
}

QRectF MockRootAreaProvider::suggestRect(KoTextLayoutRootArea */*rootArea*/)
{
    return m_suggestedRect;
}

void MockRootAreaProvider::setSuggestedRect(QRectF rect)
{
    m_suggestedRect = rect;
}

QList<KoTextLayoutObstruction *> MockRootAreaProvider::relevantObstructions(KoTextLayoutRootArea *rootArea)
{
    Q_UNUSED(rootArea);
    QList<KoTextLayoutObstruction*> obstructions;
    return obstructions;
}

KoTextLayoutRootArea *MockRootAreaProvider::area(int pos) const
{
    return m_areas.value(pos);
}

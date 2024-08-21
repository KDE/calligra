/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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

KoTextLayoutRootArea *
MockRootAreaProvider::provide(KoTextDocumentLayout *documentLayout, const RootAreaConstraint &constraint, int requestedPosition, bool *isNewRootArea)
{
    Q_UNUSED(constraint);
    if (maxPosition > 0 && requestedPosition > maxPosition) {
        qInfo() << "To many area requests:" << maxPosition << requestedPosition;
        return nullptr; // guard against loop
    }
    m_askedForMoreThenOneArea |= (m_areas.count() > 1);
    *isNewRootArea = !m_areas.contains(requestedPosition);
    if (!m_areas.contains(requestedPosition)) {
        m_areas.insert(requestedPosition, new KoTextLayoutRootArea(documentLayout));
        qInfo() << "New area provided:" << requestedPosition;
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

QRectF MockRootAreaProvider::suggestRect(KoTextLayoutRootArea *rootArea)
{
    Q_UNUSED(rootArea)
    return m_suggestedRect;
}

void MockRootAreaProvider::setSuggestedRect(QRectF rect)
{
    m_suggestedRect = rect;
}

QList<KoTextLayoutObstruction *> MockRootAreaProvider::relevantObstructions(KoTextLayoutRootArea *rootArea)
{
    Q_UNUSED(rootArea);
    QList<KoTextLayoutObstruction *> obstructions;
    return obstructions;
}

KoTextLayoutRootArea *MockRootAreaProvider::area(int pos) const
{
    return m_areas.value(pos);
}

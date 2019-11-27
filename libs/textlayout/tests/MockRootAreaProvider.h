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

#ifndef MOCKROOTAREAPROVIDER_H
#define MOCKROOTAREAPROVIDER_H

#include "KoTextLayoutRootAreaProvider.h"

#include <QRectF>
#include <QMap>

class MockRootAreaProvider : public KoTextLayoutRootAreaProvider
{
public:
    MockRootAreaProvider();

    /// reimplemented
    KoTextLayoutRootArea *provide(KoTextDocumentLayout *documentLayout, const RootAreaConstraint &constraints, int requestedPosition, bool *isNewArea) override;
    void releaseAllAfter(KoTextLayoutRootArea *afterThis) override;
    void doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea) override;
    QRectF suggestRect(KoTextLayoutRootArea *rootArea) override;
    QList<KoTextLayoutObstruction *> relevantObstructions(KoTextLayoutRootArea *rootArea) override;
    void updateAll() override;

    void setSuggestedRect(QRectF rect);

    KoTextLayoutRootArea *area(int pos = 0) const;

    int maxPosition;
    QMap<int, KoTextLayoutRootArea*> m_areas;
    QRectF m_suggestedRect;
    bool m_askedForMoreThenOneArea;
};

#endif

/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef MOCKROOTAREAPROVIDER_H
#define MOCKROOTAREAPROVIDER_H

#include "KoTextLayoutRootAreaProvider.h"

#include <QMap>
#include <QRectF>

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
    QMap<int, KoTextLayoutRootArea *> m_areas;
    QRectF m_suggestedRect;
    bool m_askedForMoreThenOneArea;
};

#endif

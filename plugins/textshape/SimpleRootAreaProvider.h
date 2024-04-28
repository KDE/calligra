/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SIMPLEROOTAREAPROVIDER_H
#define SIMPLEROOTAREAPROVIDER_H

#include "KoTextLayoutRootAreaProvider.h"

class TextShape;
class KoTextShapeData;

class SimpleRootAreaProvider : public KoTextLayoutRootAreaProvider
{
public:
    SimpleRootAreaProvider(KoTextShapeData *data, TextShape *textshape);

    /// reimplemented
    KoTextLayoutRootArea *
    provide(KoTextDocumentLayout *documentLayout, const RootAreaConstraint &constraints, int requestedPosition, bool *isNewRootArea) override;

    void releaseAllAfter(KoTextLayoutRootArea *afterThis) override;

    void doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea) override;

    void updateAll() override;

    QRectF suggestRect(KoTextLayoutRootArea *rootArea) override;

    QList<KoTextLayoutObstruction *> relevantObstructions(KoTextLayoutRootArea *rootArea) override;

    TextShape *m_textShape;

    KoTextLayoutRootArea *m_area;
    KoTextShapeData *m_textShapeData;
    bool m_fixAutogrow;
};

#endif

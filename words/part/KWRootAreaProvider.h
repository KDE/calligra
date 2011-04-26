/* This file is part of the KDE project
 * Copyright (C) 2011 Casper Boemann <cbo@boemann.dk>
 * Copyright (C) 2011 Sebastian Sauer <sebastian.sauer@kdab.com>
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

#ifndef KWROOTAREAPROVIDER_H
#define KWROOTAREAPROVIDER_H

#include <KoTextLayoutRootAreaProvider.h>

#include <QMap>

class KoShape;
class KoTextShapeData;
class KWTextFrameSet;
class KWFrame;

class KWRootAreaProvider : public KoTextLayoutRootAreaProvider
{
public:
    //KWRootAreaProvider(KWTextFrameSet *textFrameSet, KoShape *shape, KoTextShapeData *data);
    explicit KWRootAreaProvider(KWTextFrameSet *textFrameSet);
    virtual ~KWRootAreaProvider();

    /// reimplemented
    virtual KoTextLayoutRootArea *provide(KoTextDocumentLayout *documentLayout);
    virtual void releaseAllAfter(KoTextLayoutRootArea *afterThis);
    virtual void doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea);
    virtual bool suggestPageBreak(KoTextLayoutRootArea *beforeThis);
    virtual QSizeF suggestSize(KoTextLayoutRootArea *rootArea);
    virtual QList<KoTextLayoutObstruction *> relevantObstructions(const QRectF &rect, const QList<KoTextLayoutObstruction *> &excludingThese);
private:
    KWTextFrameSet *m_textFrameSet;
    QList<KoTextLayoutRootArea *> m_pages;
};

#endif
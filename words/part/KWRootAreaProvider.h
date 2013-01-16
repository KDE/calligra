/* This file is part of the KDE project
 * Copyright (C) 2011 C. Boemann <cbo@boemann.dk>
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

#include "KWPage.h"
#include <KoTextLayoutRootAreaProvider.h>

#include <QMap>
#include <QPair>

class KoShape;
class KoTextShapeData;
class KWTextFrameSet;
class KWFrame;
class KWPage;

class KWRootAreaPage
{
public:
    KWRootAreaPage() {};
    ~KWRootAreaPage() {};
    KWPage page;
	QList<KoTextLayoutRootArea *> rootAreas;
    explicit KWRootAreaPage(const KWPage &p) : page(p) {}
};

class KWRootAreaProvider : public KoTextLayoutRootAreaProvider
{
public:
    //KWRootAreaProvider(KWTextFrameSet *textFrameSet, KoShape *shape, KoTextShapeData *data);
    explicit KWRootAreaProvider(KWTextFrameSet *textFrameSet);
    virtual ~KWRootAreaProvider();

    void clearPages(int pageNumber);
    QList<KWRootAreaPage *> pages() const { return m_pages; }

    /// reimplemented
    virtual KoTextLayoutRootArea *provide(KoTextDocumentLayout *documentLayout);
    virtual void releaseAllAfter(KoTextLayoutRootArea *afterThis);
    virtual void doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea);
    virtual QSizeF suggestSize(KoTextLayoutRootArea *rootArea);
    virtual QList<KoTextLayoutObstruction *> relevantObstructions(KoTextLayoutRootArea *rootArea);
private:
    KWTextFrameSet *m_textFrameSet;
    QList<KWRootAreaPage *> m_pages;
    QHash<KoTextLayoutRootArea*, KWRootAreaPage *> m_pageHash;
    QList<QPair<KWRootAreaProvider *, int> > m_dependentProviders;

    KoTextLayoutRootArea* provideNext(KoTextDocumentLayout *documentLayout);
    void addDependentProvider(KWRootAreaProvider *provider, int pageNumber);
    void handleDependentProviders(int pageNumber);
};

#endif

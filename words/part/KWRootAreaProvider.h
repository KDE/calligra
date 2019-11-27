/* This file is part of the KDE project
 * Copyright (C) 2011-2015 C. Boemann <cbo@boemann.dk>
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
#include <KWRootAreaProviderBase.h>

#include <QMap>
#include <QPair>

class KoShape;
class KoTextShapeData;
class KWTextFrameSet;
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

class KWRootAreaProvider : public KWRootAreaProviderBase
{
public:
    //KWRootAreaProvider(KWTextFrameSet *textFrameSet, KoShape *shape, KoTextShapeData *data);
    explicit KWRootAreaProvider(KWTextFrameSet *textFrameSet);
    ~KWRootAreaProvider() override;

    void addDependentProvider(KWRootAreaProviderBase *provider, int pageNumber);

    void clearPages(int pageNumber) override;
    void setPageDirty(int pageNumber) override;

    /// reimplemented
    KoTextLayoutRootArea *provide(KoTextDocumentLayout *documentLayout, const RootAreaConstraint &constraints, int requestedPosition, bool *isNewArea) override;
    void releaseAllAfter(KoTextLayoutRootArea *afterThis) override;
    void doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea) override;

private:
    QList<KWRootAreaPage *> m_pages;
    QHash<KoTextLayoutRootArea*, KWRootAreaPage *> m_pageHash;
    QList<KoTextLayoutRootArea*> m_rootAreaCache;
    QList<QPair<KWRootAreaProviderBase *, int> > m_dependentProviders;

    QList<KWRootAreaPage *> pages() const { return m_pages; }
    KoTextLayoutRootArea* provideNext(KoTextDocumentLayout *documentLayout, const RootAreaConstraint &constraints);
    void handleDependentProviders(int pageNumber);
};

#endif

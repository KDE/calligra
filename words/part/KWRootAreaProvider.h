/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011-2015 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2011 Sebastian Sauer <sebastian.sauer@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    KWRootAreaPage() = default;
    ~KWRootAreaPage() = default;
    KWPage page;
    QList<KoTextLayoutRootArea *> rootAreas;
    explicit KWRootAreaPage(const KWPage &p)
        : page(p)
    {
    }
};

class KWRootAreaProvider : public KWRootAreaProviderBase
{
public:
    // KWRootAreaProvider(KWTextFrameSet *textFrameSet, KoShape *shape, KoTextShapeData *data);
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
    QHash<KoTextLayoutRootArea *, KWRootAreaPage *> m_pageHash;
    QList<KoTextLayoutRootArea *> m_rootAreaCache;
    QList<QPair<KWRootAreaProviderBase *, int>> m_dependentProviders;

    QList<KWRootAreaPage *> pages() const
    {
        return m_pages;
    }
    KoTextLayoutRootArea *provideNext(KoTextDocumentLayout *documentLayout, const RootAreaConstraint &constraints);
    void handleDependentProviders(int pageNumber);
};

#endif

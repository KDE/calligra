/* This file is part of the KDE project
 * Copyright (C) 2011 Casper Boemann <cbo@boemann.dk>
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

#include "KWRootAreaProvider.h"
#include "KWPageManager.h"
#include "KWDocument.h"
#include "KWPage.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWFrameLayout.h"

#include <KoTextLayoutRootArea.h>
#include <KoShape.h>
#include <KoTextShapeData.h>
#include <KoTextDocumentLayout.h>

#include <kdebug.h>

KWRootAreaProvider::KWRootAreaProvider(KWTextFrameSet *textFrameSet, KoShape *shape, KoTextShapeData *data)
    : KoTextLayoutRootAreaProvider()
    , m_textFrameSet(textFrameSet)
    , m_shape(shape)
    , m_data(data)
    , m_area(0)
{
}

KoTextLayoutRootArea *KWRootAreaProvider::provide(KoTextDocumentLayout *documentLayout)
{
    Q_ASSERT(m_textFrameSet->pageManager());
    if (m_textFrameSet->pageManager()->pageCount() > 0) { //FIXME remove this workaround

        const KWPageStyle pageStyle;
        KWPage page = m_textFrameSet->pageManager()->appendPage(pageStyle);

        kDebug() << "pageNumber=" << page.pageNumber();
        Q_ASSERT(page.isValid());

        KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->kwordDocument());
        KWFrameLayout *frlay = kwdoc->frameLayout();
        frlay->createNewFramesForPage(page.pageNumber());
    }

    //KWPage page = m_pageManager->page(pageNumber);
    //if(m_area) return 0;

    m_area = new KoTextLayoutRootArea(documentLayout);
    m_area->setAssociatedShape(m_shape);
    //m_area->setReferenceRect(0, m_shape->size().width(), 0, m_shape->size().height());
    m_data->setRootArea(m_area);
    return m_area;
}

void KWRootAreaProvider::releaseAllAfter(KoTextLayoutRootArea *afterThis)
{
    kDebug();
}

void KWRootAreaProvider::doPostLayout(KoTextLayoutRootArea *rootArea)
{
    kDebug();

    //rootArea->setTop(rootArea->top() + rootArea->associatedShape()->size().height());
    //rootArea->setBottom(rootArea->top() + rootArea->associatedShape()->size().height());
    //rootArea->setReferenceRect(0, rootArea->associatedShape()->size().width(), 0, rootArea->associatedShape()->size().height());

    rootArea->associatedShape()->update();
}

QSizeF KWRootAreaProvider::suggestSize(KoTextLayoutRootArea *rootArea)
{
    return m_shape->size();
}

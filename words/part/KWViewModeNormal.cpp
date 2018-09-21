/* This file is part of the KDE project
 * Copyright (C) 2006, 2009 Thomas Zander <zander@kde.org>
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

#include "KWViewModeNormal.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include "KWView.h"
#include <KoViewConverter.h>

#include <WordsDebug.h>

#define GAP 20

KWViewModeNormal::KWViewModeNormal()
    : m_pageSpreadMode(false)
{
}

QVector<KWViewMode::ViewMap> KWViewModeNormal::mapExposedRects(const QRectF &viewRect, KoViewConverter *viewConverter) const
{
    QVector<ViewMap> answer;
    if (!viewConverter) return answer;

#if 1
    if (m_pageTops.isEmpty())
        return answer;
    KWPage page  = m_pageManager->begin();
    const int pageOffset = page.pageNumber();

    // Perform a binary search for page-index using our m_pageTops cache.
    int begin = 0;
    int end = m_pageTops.count() - 1;
    int index = 0;
    const qreal value = viewConverter->viewToDocument(viewRect.topLeft()).y();
    if (m_pageTops.value(end) <= value) { // check extremes. Only end is needed since begin is zero.
        begin = end;
        index = end;
    }
    while (end - begin > 1) {
        index = begin + (end - begin) / 2;
        qreal diff = m_pageTops.value(index) - value;
        if (diff < 0)
            begin = index;
        else if (diff > 0)
            end = index;
        else
            break;
    }
    // index is now the number of the first possible page that can
    // contain the viewRect.  The next step is to find the
    // corresponding Page.  Since we have no way to get to the Nth
    // page directly we have to enumerate them from the beginning.
    //
    // We use 1 since we might hit a pagespread in the binary search,
    // so start one page early.
    while (index > 1) { 
        page = page.next();
        --index;
    }

    // From here we loop through some more pages after the found one
    // and see if they intersect with the page in question.  When we
    // have two pages in row that don't intersect we break the loop.
    qreal offsetX = 0.0;
    int emptyPages = 0;
    for(; page.isValid(); page = page.next()) {
        Q_ASSERT_X(page.pageNumber()-pageOffset < m_pageTops.count(), __FUNCTION__,
                   QString("Pagemanager has more pages than viewmode (%1>%2 with pageOffset=%3 and pageNumber=%4 and pageCount=%5). Make sure you add pages via the document!")
                   .arg(page.pageNumber()-pageOffset).arg(m_pageTops.count())
                   .arg(pageOffset).arg(page.pageNumber()).arg(m_pageManager->pageCount()).toLocal8Bit());


        // Some invariants
        const QRectF pageRect = page.rect();
        const qreal offsetY = m_pageTops[page.pageNumber() - pageOffset] - pageRect.top();

        bool pageIntersects = false;

        // 1. First handle the page itself.
        const QRectF zoomedPage = viewConverter->documentToView(pageRect);
        ViewMap vm;
        vm.page = page;
        //kDebug(32003) <<"page" << page.pageNumber();
        vm.distance = viewConverter->documentToView(QPointF(offsetX, offsetY));

        const QRectF targetPage(zoomedPage.x() + vm.distance.x(), zoomedPage.y() + vm.distance.y(),
                                zoomedPage.width(), zoomedPage.height());
        QRectF intersection = targetPage.intersected(viewRect);
        if (! intersection.isEmpty()) {
            intersection.moveTopLeft(intersection.topLeft() - vm.distance);
            vm.clipRect = intersection.toRect();
            answer.append(vm);
            pageIntersects = true;
        }

        // 2. Then handle the annotation area if annotations are active.
        //
        //    The reason we don't do them together with the page
        //    itself is because the pages have a gap between them, but
        //    the annotation area should be unbroken.
        //
        // NOTE: 'annotation' below means the annotation area.
        //
        // FIXME: We should only do this if the annotation area is
        //        actually shown. How can we inside the KWViewMode
        //        know if annotations are active?
        if (1 /* annotations are shown */) {
            const QRectF annotationRect = pageRect.adjusted(page.width(), 0,
                                                            KWCanvasBase::AnnotationAreaWidth, GAP);
            const QRectF zoomedAnnotation = viewConverter->documentToView(annotationRect);
            ViewMap vm2;
            vm2.page = page;
            vm2.distance = viewConverter->documentToView(QPointF(offsetX, offsetY));

            const QRectF targetAnnotation(zoomedAnnotation.x() + vm2.distance.x(),
                                          zoomedAnnotation.y() + vm2.distance.y(),
                                          zoomedAnnotation.width(), zoomedAnnotation.height());
            intersection = targetAnnotation.intersected(viewRect);
            if (! intersection.isEmpty()) {
                intersection.moveTopLeft(intersection.topLeft() - vm2.distance);
                vm2.clipRect = intersection.toRect();
                answer.append(vm2);
                pageIntersects = true;
            }
        }

        // Record whether this page had an intersection with the view rect.
        if (pageIntersects) {
            emptyPages = 0;
        } else {
            ++emptyPages;
        }
        if (emptyPages > 2) // Since we show at max 2 pages side by side this is an easy rule
            break;

        if (m_pageSpreadMode) {
            if (page.pageSide() == KWPage::Left)
                offsetX = page.width() + GAP;
            else
                offsetX = 0.0;
        }
    }
#else
    KWPage page  = m_pageManager->begin();
    Q_ASSERT(page.isValid());
    qreal offsetX = 0.0;
    const int pageOffset = page.pageNumber();
    for(; page.isValid(); page = page.next()) {
        const QRectF pageRect = page.rect();
        const QRectF zoomedPage = viewConverter->documentToView(pageRect);
        ViewMap vm;
        vm.page = page;

        const qreal offsetY = m_pageTops[page.pageNumber() - pageOffset] - pageRect.top();
        vm.distance = viewConverter->documentToView(QPointF(offsetX, offsetY));
#if 0
        const QRectF targetPage(zoomedPage.x() + vm.distance.x(), zoomedPage.y() + vm.distance.y(), zoomedPage.width() , zoomedPage.height());
        QRectF intersection = targetPage.intersected(viewRect);
        if (! intersection.isEmpty()) {
            intersection.moveTopLeft(intersection.topLeft() - vm.distance);
            vm.clipRect = intersection.toRect();
            answer.append(vm);
        }
#else
        const QRectF targetPage(zoomedPage.x() + vm.distance.x(), zoomedPage.y() + vm.distance.y(), zoomedPage.width() , zoomedPage.height());
        vm.clipRect = targetPage.toRect();
        answer.append(vm);
#endif
    }
#endif
    return answer;
}

void KWViewModeNormal::updatePageCache()
{
    if (!m_pageManager) {
        warnWords << "Error detected while running KWViewModeNormal::updatePageCache: PageManager not set";
        return;
    }

    m_pageSpreadMode = false;
    foreach (const KWPage &page, m_pageManager->pages()) {
        Q_UNUSED(page);
    }
    m_pageTops.clear();
    qreal width = 0.0, bottom = 0.0;
    if (m_pageSpreadMode) { // two pages next to each other per row
        qreal top = 0.0, last = 0.0, halfWidth = 0.0;
        foreach (const KWPage &page, m_pageManager->pages()) {
            switch (page.pageSide()) {
            case KWPage::Left:
                m_pageTops.append(top);
                last = page.height();
                halfWidth = page.width() + GAP;
                width = qMax(width, halfWidth);
                bottom = top + last;
                break;
            case KWPage::Right:
                m_pageTops.append(top);
                top += qMax(page.height(), last);
                last = 0.0;
                width = qMax(width, halfWidth + page.width());
                halfWidth = 0.0;
                bottom = top;
                top += GAP;
                break;
            default:
                Q_ASSERT(false);
                break;
            }
        }
    } else { // each page on a row
        qreal top = 0.0;
        foreach (const KWPage &page, m_pageManager->pages()) {
            m_pageTops.append(top);
            top += page.height() + GAP;
            width = qMax(width, page.width());
        }
        bottom = top;
    }
    if (bottom > GAP)
        bottom -= GAP; // remove one too many added
    m_contents = QSizeF(width, bottom);
}

QPointF KWViewModeNormal::documentToView(const QPointF & point, KoViewConverter *viewConverter) const
{
    Q_ASSERT(viewConverter);

    KWPage page = m_pageManager->page(point);
    if (! page.isValid())
        page = m_pageManager->last();
    if (! page.isValid())
        return QPointF();
    int pageIndex = page.pageNumber() - m_pageManager->begin().pageNumber();
    qreal x = 0;
    if (m_pageSpreadMode && page.pageSide() == KWPage::Right) {
        KWPage prevPage = m_pageManager->page(page.pageNumber() - 1);
        if (prevPage.isValid())
            x = prevPage.width();
    }

    QPointF offsetInPage(point.x(),  + point.y() - page.offsetInDocument());
    Q_ASSERT(pageIndex >= 0);
    Q_ASSERT(pageIndex < m_pageTops.count());
    QPointF translated(x, m_pageTops[pageIndex]);
    return viewConverter->documentToView(translated + offsetInPage);
}

QPointF KWViewModeNormal::viewToDocument(const QPointF & point, KoViewConverter *viewConverter) const
{
    Q_ASSERT(viewConverter);

    QPointF clippedPoint(qMax(qreal(0.0), point.x()), qMax(qreal(0.0), point.y()));
    QPointF translated = viewConverter->viewToDocument(clippedPoint);
    int pageNumber = 0;
    foreach (qreal top, m_pageTops) {
        if (translated.y() < top)
            break;
        pageNumber++;
    }
    translated = viewConverter->viewToDocument(point);
    KWPage page = m_pageManager->page(pageNumber - 1 + m_pageManager->begin().pageNumber());
    qreal xOffset = translated.x();

    if (page.isValid() && m_pageSpreadMode && page.pageSide() == KWPage::Right && page != m_pageManager->begin()) {
        // there is a page displayed left of this one.
        KWPage prevPage = page.previous();
        if (xOffset <= prevPage.width()) // was left page instead of right :)
            page = prevPage;
        else
            xOffset -= prevPage.width();
    }

    if (! page.isValid()) // below doc or right of last page
        return QPointF(m_contents.width(), m_contents.height());

    qreal yOffset = translated.y();
    if (pageNumber >= 0)
        yOffset -= m_pageTops[pageNumber -1];

    return QPointF(xOffset, page.offsetInDocument() + yOffset);
}

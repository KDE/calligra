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
#include "KWCanvas.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include <KoViewConverter.h>

#include <kdebug.h>

#define GAP 5

KWViewModeNormal::KWViewModeNormal()
    : m_pageSpreadMode(false)
{
}

QList<KWViewMode::ViewMap> KWViewModeNormal::clipRectToDocument(const QRect &viewRect) const
{
    QList<ViewMap> answer;
    if (m_pageTops.isEmpty())
        return answer;
    KWPage page  = m_pageManager->begin();
    qreal offsetX = 0.0;
    const int pageOffset = page.pageNumber();

    int begin = 0;
    int end = m_pageTops.count() - 1;
    int index = 0;
    const qreal value = m_viewConverter->viewToDocument(viewRect.topLeft()).y();
    if (m_pageTops.value(end) <= value) { // check extremes. Only end is needed since begin is zero.
        begin = end;
        index = end;
    }
    while (end - begin > 1) { // binary search for page-index using our m_pageTops cache.
        index = begin + (end - begin) / 2;
        qreal diff = m_pageTops.value(index) - value;
        if (diff < 0)
            begin = index;
        else if (diff > 0)
            end = index;
        else
            break;
    }
    while (index > 1) { // 1 since we might hit a pagespread in the binary search, so start one page early
        page = page.next();
        --index;
        if (page.pageSide() == KWPage::PageSpread)
            --index;
    }

    int emptyPages = 0;
    while (page.isValid()) {
#ifndef NDEBUG
        if (page.pageNumber() - pageOffset >= m_pageTops.count()) {
            kWarning(32003) << "ERROR; pagemanager has more pages than viewmode ("
                << m_pageManager->pageCount() << ">" << m_pageTops.count()
                << "). Make sure you add pages via the document!";
            break;
        }
#endif
        const QRectF pageRect = page.rect();
        const QRectF zoomedPage = m_viewConverter->documentToView(pageRect);
        ViewMap vm;
        vm.page = page;
        //kDebug(32003) <<"page" << page.pageNumber();
        const qreal offsetY = m_pageTops[page.pageNumber() - pageOffset] - pageRect.top();
        vm.distance = m_viewConverter->documentToView(QPointF(offsetX, offsetY));

        const QRectF targetPage(zoomedPage.x() + vm.distance.x(), zoomedPage.y() + vm.distance.y(),
                zoomedPage.width() , zoomedPage.height());
        QRectF intersection = targetPage.intersect(viewRect);
        if (! intersection.isEmpty()) {
            intersection.moveTopLeft(intersection.topLeft() - vm.distance);
            vm.clipRect = intersection.toRect();
            answer.append(vm);
            emptyPages = 0;
        } else {
            emptyPages++;
        }
        if (emptyPages > 2) // Since we show at max 2 pages side by side this is an easy rule
            break;
        if (m_pageSpreadMode) {
            if (page.pageSide() == KWPage::Left)
                offsetX = page.width() + GAP;
            else
                offsetX = 0.0;
        }
        page = page.next();
    }

    return answer;
}

void KWViewModeNormal::updatePageCache()
{
    if (!m_pageManager) {
        kWarning(31002) << "Error detected while running KWViewModeNormal::updatePageCache: PageManager not set";
        return;
    }

    m_pageSpreadMode = false;
    foreach (const KWPage &page, m_pageManager->pages()) {
        if (page.pageSide() == KWPage::PageSpread) {
            m_pageSpreadMode = true;
            break;
        }
    }
    m_pageTops.clear();
    qreal width = 0.0, bottom = 0.0;
    if (m_pageSpreadMode) { // two pages next to each other per row
        qreal top = 0.0, last = 0.0, halfWidth = 0.0;
        foreach (const KWPage &page, m_pageManager->pages()) {
            switch (page.pageSide()) {
            case KWPage::PageSpread:
                if (last > 0)
                    top += last + GAP;
                m_pageTops.append(top);
                m_pageTops.append(top);
                top += page.height() + GAP;
                width = qMax(width, page.width());
                halfWidth = 0.0;
                last = 0.0;
                bottom = top;
                break;
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

QPointF KWViewModeNormal::documentToView(const QPointF & point) const
{
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
    Q_ASSERT(pageIndex >= 0 && pageIndex < m_pageTops.count());
    QPointF translated(x, m_pageTops[pageIndex]);
    Q_ASSERT(m_viewConverter);
    return m_viewConverter->documentToView(translated + offsetInPage);
}

QPointF KWViewModeNormal::viewToDocument(const QPointF & point) const
{
    QPointF clippedPoint(qMax(qreal(0.0), point.x()), qMax(qreal(0.0), point.y()));
    QPointF translated = m_viewConverter->viewToDocument(clippedPoint);
    int pageNumber = 0;
    foreach (qreal top, m_pageTops) {
        if (translated.y() < top)
            break;
        pageNumber++;
    }
    translated = m_viewConverter->viewToDocument(point);
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

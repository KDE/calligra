/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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

#include <kdebug.h>

#define GAP 5

KWViewModeNormal::KWViewModeNormal( KWCanvas* canvas )
    : KWViewMode( canvas )
{
    updatePageCache();
}

QList<KWViewMode::ViewMap> KWViewModeNormal::clipRectToDocument(const QRect &viewRect) const {
    const KWPageManager *pageManager = canvas()->document()->pageManager();
    QList<ViewMap> answer;
    const int pageOffset = pageManager->startPage();
    double offsetX = 0.0;
    foreach(KWPage *page, pageManager->pages()) {
        if(m_pageTops.count() <= page->pageNumber() - pageOffset) {
           kWarning(32003) << "KWViewModeNormal ERROR; pagemanager has more pages than viewmode (" <<
               pageManager->pageCount() << ">" << m_pageTops.count() <<
               "). Make sure you add pages via the document!" << endl;
            break;
        }
        QRectF zoomedPage = canvas()->viewConverter()->documentToView(page->rect());
        ViewMap vm;
//kDebug() << "page[" << page->pageNumber() << "] uses pagetops: " << m_pageTops[page->pageNumber() - pageOffset] << endl;
        vm.distance = canvas()->viewConverter()->documentToView(
                QPointF(offsetX, m_pageTops[page->pageNumber() - pageOffset] - page->offsetInDocument()));

        QRectF targetPage = QRectF(zoomedPage.topLeft() + vm.distance, zoomedPage.size());
        QRectF intersection = targetPage.intersect(viewRect);
        if(! intersection.isEmpty()) {
            intersection.moveTopLeft(intersection.topLeft() - vm.distance);
            vm.clipRect = intersection.toRect();
            answer.append(vm );
        }
        if(m_pageSpreadMode) {
            if(page->pageSide() == KWPage::Left)
                offsetX = page->width() + GAP;
            else
                offsetX = 0.0;
        }
    }

    return answer;
}

void KWViewModeNormal::updatePageCache() {
    m_pageSpreadMode = false;
    foreach(KWPage *page, canvas()->document()->pageManager()->pages()) {
        if(page->pageSide() == KWPage::PageSpread) {
            m_pageSpreadMode = true;
            break;
        }
    }
    m_pageTops.clear();
    double width = 0.0, bottom = 0.0;
    if(m_pageSpreadMode) { // two pages next to each other per row
        double top = 0.0, last = 0.0, halfWidth = 0.0;
        foreach(KWPage *page, canvas()->document()->pageManager()->pages()) {
            switch(page->pageSide()) {
                case KWPage::PageSpread:
                    if(last > 0)
                        top += last + GAP;
                    m_pageTops.append(top);
                    m_pageTops.append(top);
                    top += page->height() + GAP;
                    width = qMax(width, page->width());
                    halfWidth = 0.0;
                    last = 0.0;
                    bottom = top;
                    break;
                case KWPage::Left:
                    m_pageTops.append(top);
                    last = page->height();
                    halfWidth = page->width() + GAP;
                    width = qMax(width, halfWidth);
                    bottom = top + last;
                    break;
                case KWPage::Right:
                    m_pageTops.append(top);
                    top += qMax(page->height(), last);
                    last = 0.0;
                    width = qMax(width, halfWidth + page->width());
                    halfWidth = 0.0;
                    bottom = top;
                    top += GAP;
                    break;
            }
        }
    }
    else { // each page on a row
        double top = 0.0;
        foreach(KWPage *page, canvas()->document()->pageManager()->pages()) {
            m_pageTops.append(top);
            top += page->height() + GAP;
            width = qMax(width, page->width());
        }
        bottom = top;
    }
    m_contents = QSizeF(width, bottom);
}

QSize KWViewModeNormal::contentsSize() {
    QPointF size = canvas()->viewConverter()->documentToView(QPointF(m_contents.width(), m_contents.height()));
    return QSize(qRound(size.x()), qRound(size.y()));
}

QPointF KWViewModeNormal::documentToView( const QPointF & point ) const {
    const KWPageManager *pageManager = canvas()->document()->pageManager();
    KWPage *page = pageManager->page(point);
    if(! page)
        page = pageManager->page(pageManager->lastPageNumber());
    Q_ASSERT(page);
    int pageIndex = page->pageNumber() - canvas()->document()->startPage();
    double x = 0;
    if(m_pageSpreadMode && page->pageSide() == KWPage::Right) {
        KWPage *prevPage = canvas()->document()->pageManager()->page(page->pageNumber() - 1);
        if(prevPage)
            x = prevPage->width();
    }

    QPointF offsetInPage(point.x(),  + point.y() - page->offsetInDocument());
    QPointF translated(x, m_pageTops[pageIndex]);
    return canvas()->viewConverter()->documentToView(translated + offsetInPage);
}

QPointF KWViewModeNormal::viewToDocument( const QPointF & point ) const {
    const KWPageManager *pageManager = canvas()->document()->pageManager();
    QPointF clippedPoint(qMax(0.0, point.x()), qMax(0.0, point.y()));
    QPointF translated = canvas()->viewConverter()->viewToDocument(clippedPoint);
    int pageNumber = 0;
    foreach(double top, m_pageTops) {
        if(translated.y() < top)
            break;
        pageNumber++;
    }
    KWPage *page = pageManager->page(pageNumber -1 + pageManager->startPage());
    double xOffset = translated.x();
    if(page && m_pageSpreadMode && page->pageSide() == KWPage::Right &&
            pageManager->startPage() < page->pageNumber()) {
        // there is a page displayed left of this one.
        KWPage *prevPage = pageManager->page(page->pageNumber() - 1);
        if(xOffset <= prevPage->width()) // was left page instead of right :)
            page = prevPage;
        else
            xOffset -= prevPage->width();
    }
    if(! page) // below doc or right of last page
        return QPointF(m_contents.width(), m_contents.height());

    double yOffset = translated.y();
    if(pageNumber > 0)
        yOffset -= m_pageTops[pageNumber-1];

    return QPointF(xOffset, page->offsetInDocument() + yOffset);
}

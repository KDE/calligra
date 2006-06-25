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
    // it is important to note that between the input and output there will be
    // no difference in zoom levels; all we are talking about are translations.
// TODO make KWViewModeNormal::clipRectToDocument actually use the viewRect

#if 0
    // First: unzoom to figure out which pages are intersected
    QRectF docRect = QRectF(viewRect.topLeft(), viewRect.size());
    docRect = viewToDocument(docRect);

    KWPageManager *pageManager = canvas()->document()->pageManager();

    int pageNumber=pageManager->pageNumber(docRect.topLeft());
    KWPage *page = pageManager->page(pageNumber);
    

#endif
    const KWPageManager *pageManager = canvas()->document()->pageManager();
    QList<ViewMap> answer;
    int pageOffset = pageManager->startPage();
    double offsetX = 0.0;
    foreach(KWPage *page, pageManager->pages()) {
        QRectF zoomedPage = canvas()->viewConverter()->documentToView(page->rect());
        ViewMap vm;
        vm.clipRect = zoomedPage.toRect();
//kDebug() << "page[" << page->pageNumber() << "] uses pagetops: " << m_pageTops[page->pageNumber() - pageOffset] << endl;
        vm.distance = canvas()->viewConverter()->documentToView(
                QPointF(offsetX, m_pageTops[page->pageNumber() - pageOffset] - page->offsetInDocument()));
        answer.append(vm );
        if(page->pageSide() == KWPage::Left)
            offsetX = page->width() + GAP;
        else
            offsetX = 0.0;
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
            top += page->height();
            width = qMax(width, page->width());
            bottom = top;
        }
    }
    m_contents = QSizeF(width, bottom);
kDebug() << "  contents: " << width << "x" << bottom << endl;
}

QSize KWViewModeNormal::contentsSize() {
    QPointF size = canvas()->viewConverter()->documentToView(QPointF(m_contents.width(), m_contents.height()));
    return QSize(qRound(size.x()), qRound(size.y()));
}

QPointF KWViewModeNormal::documentToView( const QPointF & point ) {
    KWPage *page = canvas()->document()->pageManager()->page(point);
    int pageIndex = page->pageNumber() - canvas()->document()->startPage();
    double x = 0;
    if(page->pageSide() == KWPage::Right) {
        KWPage *prevPage = canvas()->document()->pageManager()->page(page->pageNumber() - 1);
        if(prevPage)
            x = prevPage->width();
    }

    QPointF translated(x, m_pageTops[pageIndex]);
    return canvas()->viewConverter()->documentToView(translated);
}

QPointF KWViewModeNormal::viewToDocument( const QPointF & point ) {
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

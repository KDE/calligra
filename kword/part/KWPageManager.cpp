/* This file is part of the KOffice project
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.

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
#include "KWPageManager.h"
#include "KWPage.h"

#include <KoShape.h>
#include <KoUnit.h>

//#define DEBUG_PAGES

KWPageManager::KWPageManager() {
    m_firstPage = 1;
    m_onlyAllowAppend = false;
    m_defaultPageLayout = KoPageLayout::standardLayout();
}

KWPageManager::~KWPageManager() {
    qDeleteAll(m_pageList);
}

int KWPageManager::pageNumber(const QPointF &point) const {
    int pageNumber=m_firstPage;
    double startOfpage = 0.0;
    foreach(KWPage *page, m_pageList) {
        if(startOfpage >= point.y())
            break;
        startOfpage += page->height();
        pageNumber++;
    }
    return pageNumber-1;
}

int KWPageManager::pageNumber(const KoShape *shape) const {
    return pageNumber(shape->absolutePosition());
}
int KWPageManager::pageNumber(const double ptY) const {
    return pageNumber(QPointF(0, ptY));
}
int KWPageManager::pageCount() const {
    if(m_pageList.isEmpty())
        return 0;
    KWPage *first = m_pageList.first();
    KWPage *last = m_pageList.last();
    return 1 + last->pageNumber() + (last->pageSide() == KWPage::PageSpread?1:0) - first->pageNumber();
}

KWPage* KWPageManager::page(int pageNum) const {
    foreach(KWPage *page, m_pageList) {
        if(page->pageNumber() == pageNum ||
                page->pageSide() == KWPage::PageSpread && page->pageNumber()+1 == pageNum)
            return page;
    }
#ifdef DEBUG_PAGES
    kWarning(31001) << "KWPageManager::page(" << pageNum << ") failed; Requested page does not exist ["<< m_firstPage << "-"<< lastPageNumber() << "]"<< endl;
    kDebug(31001) << kBacktrace();
#endif
    return 0;
}
KWPage* KWPageManager::page(const KoShape *shape) const {
    return page(pageNumber(shape));
}
KWPage* KWPageManager::page(const QPointF &point) const {
    return page(pageNumber(point));
}
KWPage* KWPageManager::page(double ptY) const {
    return page(pageNumber(ptY));
}

void KWPageManager::setStartPage(int startPage) {
    int offset = startPage - m_firstPage;
    bool switchSides = startPage % 2 != m_firstPage % 2;
    foreach(KWPage *page, m_pageList) {
        page->m_pageNum = page->m_pageNum + offset;
        if(switchSides && page->pageSide() != KWPage::PageSpread)
            page->m_pageSide = page->m_pageSide == KWPage::Left ? KWPage::Right : KWPage::Left;
    }
    m_firstPage = startPage;
}

int KWPageManager::lastPageNumber() const {
    return pageCount() + m_firstPage - 1;
}

KWPage* KWPageManager::insertPage(int index) {
    if(m_onlyAllowAppend)
        return appendPage();
    // increase the pagenumbers of pages following the index
    foreach(KWPage *page, m_pageList) {
        if(page->pageNumber() >= index)
            page->m_pageNum++;
    }
    KWPage *page = new KWPage(this, qMin( qMax(index, m_firstPage), lastPageNumber()+1 ));
    m_pageList.append(page);
    qSort(m_pageList.begin(), m_pageList.end(), compareItems);
    return page;
}

KWPage* KWPageManager::appendPage() {
    KWPage *page = new KWPage(this, lastPageNumber() + 1);
    m_pageList.append(page);
    return page;
}

const KoPageLayout KWPageManager::pageLayout(int pageNumber) const {
    KoPageLayout lay = m_defaultPageLayout;
    if(pageNumber >= m_firstPage && pageNumber <= lastPageNumber()) {
        KWPage *page = this->page(pageNumber);
        lay.ptHeight = page->height();
        lay.ptWidth = page->width();
        lay.ptTop = page->topMargin();
        lay.ptLeft = page->leftMargin();
        lay.ptBottom = page->bottomMargin();
        lay.ptRight = page->rightMargin();
    }
    return lay;
}

double KWPageManager::topOfPage(int pageNum) const {
    return pageOffset(pageNum, false);
}
double KWPageManager::bottomOfPage(int pageNum) const {
    return pageOffset(pageNum, true);
}

double KWPageManager::pageOffset(int pageNum, bool bottom) const {
    if(pageNum < m_firstPage)
        return 0;
    double offset = 0.0;
    foreach(KWPage *page, m_pageList) {
        if(page->pageNumber() == pageNum) {
            if(bottom)
                offset += page->height();
            break;
        }
        offset += page->height() + m_padding.top + m_padding.bottom;
    }
    return offset;
}

void KWPageManager::removePage(int pageNumber) {
    removePage(page(pageNumber));
}
void KWPageManager::removePage(KWPage *page) {
    if(!page)
        return;
    const int count = page->pageSide() == KWPage::PageSpread ? 2 : 1;
    foreach(KWPage *p, m_pageList) {
        if(p->m_pageNum > page->m_pageNum)
            p->m_pageNum -= count;
    }

    m_pageList.removeAll(page);
    // TODO make the deletion of object page occur in a single shot
    delete page;
}

void KWPageManager::setDefaultPage(const KoPageLayout &layout) {
    m_defaultPageLayout = layout;
    // make sure we have 1 default, either pageBound or left/right bound.
    if(m_defaultPageLayout.ptLeft < 0 || m_defaultPageLayout.ptRight < 0) {
        m_defaultPageLayout.ptLeft = -1;
        m_defaultPageLayout.ptRight = -1;
    } else {
        m_defaultPageLayout.ptPageEdge = -1;
        m_defaultPageLayout.ptBindingSide = -1;
        m_defaultPageLayout.ptLeft = qMax(m_defaultPageLayout.ptLeft, 0.0);
        m_defaultPageLayout.ptRight = qMax(m_defaultPageLayout.ptRight, 0.0);
    }
    //kDebug() << "setDefaultPage l:" << m_defaultPageLayout.ptLeft << ", r: " << m_defaultPageLayout.ptRight << ", a: " << m_defaultPageLayout.ptPageEdge << ", b: " << m_defaultPageLayout.ptBindingSide << endl;
}

QPointF KWPageManager::clipToDocument(const QPointF &point) {
    int page=m_firstPage;
    double startOfpage = 0.0;

    foreach (KWPage *p, m_pageList) {
        startOfpage += p->height();
        if(startOfpage >= point.y())
            break;
        page++;
    }
    page = qMin(page, lastPageNumber());
    QRectF rect = this->page(page)->rect();
    if(rect.contains(point))
        return point;

    QPointF rc(point);
    if(rect.top() > rc.y())
        rc.setY(rect.top());
    else if(rect.bottom() < rc.y())
        rc.setY(rect.bottom());

    if(rect.left() > rc.x())
        rc.setX(rect.left());
    else if(rect.right() < rc.x())
        rc.setX(rect.right());
    return rc;
}

QList<KWPage*> KWPageManager::pages() const {
    return QList<KWPage*> (m_pageList);
}

// **** PageList ****
int KWPageManager::compareItems(KWPage *a, KWPage *b)
{
    return b->pageNumber() - a->pageNumber() > 0;
}

/* This file is part of the KOffice project
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
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
#include "koRect.h"

KWPageManager::KWPageManager() {
    m_firstPage = 0;
    m_onlyAllowAppend = false;
    m_pageList.setAutoDelete(true);
}

int KWPageManager::pageNumber(const KoRect &frame) const {
    int page=m_firstPage;
    double startOfpage = 0.0;
    QPtrListIterator<KWPage> pages(m_pageList);
    while(pages.current() && startOfpage < frame.top()) {
        startOfpage += pages.current()->height();
        if(startOfpage > frame.top())
            break;
        page++;
        ++pages;
    }
    if(!pages.current() || frame.right() > pages.current()->width() ||
            frame.top() > pages.current()->height() + startOfpage) {
#ifdef DEBUG_PAGES
    if(!pages.current())
        kdDebug() << "  KWPageManager::pageNumber of "<< frame << " is too high, no page there\n";
    else if(frame.right() > pages.current()->width())
        kdDebug() << "  KWPageManager::pageNumber right of "<< frame << " is out of bounds\n";
    else if(frame.top() > pages.current()->height() + startOfpage)
        kdDebug() << "  KWPageManager::pageNumber "<< frame << " spans multiple pages\n";
#endif
        return -1; // not inside the page...
    }
    return page;
}
int KWPageManager::pageNumber(const KoPoint &point) const {
    return pageNumber(KoRect(point, point));
}
int KWPageManager::pageNumber(const double ptY) const {
    return pageNumber(KoRect(0, ptY, 0, 0));
}
int KWPageManager::pageNumber(const KoRect *rect) const {
    KoRect tmp(*rect);
    return pageNumber(tmp);
}

int KWPageManager::pageCount() const {
    return m_pageList.count();
}

KWPage* KWPageManager::page(int pageNum) const {
    QPtrListIterator<KWPage> pages(m_pageList);
    while(pages.current()) {
        if(pages.current()->pageNumber() == pageNum)
            return pages.current();
        ++pages;
    }
    kdWarning(31001) << "KWPageManager::page(" << pageNum << ") failed; Requested page does not exist"<< endl;
    return 0;
}
KWPage* KWPageManager::page(const KoRect &frame) const {
    return page(pageNumber(frame));
}
KWPage* KWPageManager::page(const KoPoint &point) const {
    return page(pageNumber(point));
}
KWPage* KWPageManager::page(double ptY) const {
    return page(pageNumber(ptY));
}
KWPage* KWPageManager::page(const KoRect *rect) const {
    KoRect tmp(*rect);
    return page(tmp);
}

void KWPageManager::setStartPage(int startPage) {
    int offset = startPage - m_firstPage;
    bool switchSides = startPage % 2 != m_firstPage % 2;
    for(QPtrListIterator<KWPage> pages(m_pageList); pages.current(); ++pages) {
        KWPage *page = pages.current();
        page->m_pageNum = page->m_pageNum + offset;
        if(switchSides)
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
    KWPage *page = new KWPage(this, QMIN( QMAX(index, m_firstPage), lastPageNumber()+1 ));
    QPtrListIterator<KWPage> pages(m_pageList);
    while(pages.current() && pages.current()->pageNumber() < index)
        ++pages;
    while(pages.current()) {
        pages.current()->m_pageNum++;
        ++pages;
    }
    m_pageList.inSort(page);
    return page;
}

KWPage* KWPageManager::appendPage() {
    KWPage *page = new KWPage(this, lastPageNumber() + 1);
    m_pageList.append(page);
    return page;
}

const KoPageLayout KWPageManager::pageLayout(int pageNumber) const {
    KoPageLayout lay = m_defaultPageLayout;
    KWPage *page = this->page(pageNumber);
    if(page) {
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
    QPtrListIterator<KWPage> pages(m_pageList);
    double offset = 0.0;
    while(pages.current()) {
        if(pages.current()->pageNumber() == pageNum) {
            if(bottom)
                offset += pages.current()->height();
            break;
        }
        offset += pages.current()->height();
        ++pages;
    }
    return offset;
}

void KWPageManager::removePage(int pageNumber) {
    removePage(page(pageNumber));
}
void KWPageManager::removePage(KWPage *page) {
    QPtrListIterator<KWPage> pages(m_pageList);
    while(pages.current() && pages.current()->pageNumber() <= page->pageNumber())
        ++pages;
    while(pages.current()) {
        pages.current()->m_pageNum--;
        ++pages;
    }
    // TODO make the deletion of object page occur in a single shot
    m_pageList.remove(page);
}

void KWPageManager::setDefaultPageSize(double width, double height) {
    m_defaultPageLayout.ptWidth = width;
    m_defaultPageLayout.ptHeight = height;
}

void KWPageManager::setDefaultPageMargins(double top, double left, double bottom, double right) {
    m_defaultPageLayout.ptTop = top;
    m_defaultPageLayout.ptLeft = left;
    m_defaultPageLayout.ptBottom = bottom;
    m_defaultPageLayout.ptRight = right;
}

// **** PageList ****
int KWPageManager::PageList::compareItems(QPtrCollection::Item a, QPtrCollection::Item b)
{
    int pa = ((KWPage *)a)->pageNumber();
    int pb = ((KWPage *)b)->pageNumber();
    if (pa == pb) return 0;
    if (pa < pb) return -1;
    return 1;
}

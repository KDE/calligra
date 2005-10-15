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
#include "kwframe.h"

KWPageManager::KWPageManager() {
    m_firstPage = 1;
    m_pageList.append( new KWPage(1) );
}

int KWPageManager::pageNumber(const KWFrame *frame) const {
    int page=m_firstPage;
    double startOfpage = 0.0;
    QPtrListIterator<KWPage> pages(m_pageList);
    while(pages.current() && startOfpage < frame->top()) {
        KWPage* page = pages.current();
        startOfpage += page->size().height();
        page++;
        ++pages;
    }
    return page;
}

int KWPageManager::numPages() const {
    return m_pageList.count();
}

KWPage* KWPageManager::pageAt(int pageNum) const {
    int effectivePage = pageNum - m_firstPage;
    Q_ASSERT(effectivePage > 0);
    if(effectivePage < 0)
        return 0;

    QPtrListIterator<KWPage> pages(m_pageList);
    while(pages.current()) {
        if(effectivePage == 0)
            return pages.current();
        effectivePage--;
        ++pages;
    }
    Q_ASSERT(effectivePage == 0);
    return 0;
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
    return numPages() + m_firstPage - 1;
}

KWPage* KWPageManager::createPage() {
    KWPage *page = new KWPage(lastPageNumber() + 1);
    m_pageList.append(page);
    return page;
}

int KWPageManager::PageList::compareItems(QPtrCollection::Item a, QPtrCollection::Item b)
{
    // TODO
    return 0;
}

/* This file is part of the KOffice project
 * Copyright (C) 2005-2006, 2008 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 * Copyright (C) 2008 Sebastian Sauer <mail@dipe.org>
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
#include "KWPageManager.h"
#include "KWPageManager_p.h"
#include "KWPage.h"
#include "KWDocument.h"

#include <KoShape.h>
#include <KoUnit.h>

#include <KDebug>

//#define DEBUG_PAGES

KWPageManagerPrivate::KWPageManagerPrivate()
    : lastId(0),
    preferPageSpread(false),
    defaultPageStyle("Standard")
{
    pageStyles.insert(defaultPageStyle.name(), defaultPageStyle);
}

qreal KWPageManagerPrivate::pageOffset(int pageNum, bool bottom) const
{
    Q_ASSERT(pageNum >= 0);
    qreal offset = 0.0;

    QMap<int, int>::const_iterator iter = pageNumbers.begin();
    for (;iter != pageNumbers.end(); ++iter) {
        const KWPageManagerPrivate::Page page = pages[iter.value()];
        if (page.pageSide == KWPage::PageSpread && iter.key()%2 == 1)
            continue;
        if (iter.key() == pageNum) {
            if (bottom)
                offset += page.style.pageLayout().height;
            break;
        }
        offset += page.style.pageLayout().height + padding.top + padding.bottom;
    }
    return offset;
}

void KWPageManagerPrivate::setPageNumberForId(int pageId, int newPageNumber)
{
    if (pageNumbers.isEmpty() || ! pages.contains(pageId))
        return;

    const int oldPageNumber = pages[pageId].pageNumber;
    int diff = newPageNumber - oldPageNumber;
    int from = oldPageNumber;
    int to = newPageNumber;
    if (from > to)
        qSwap(from, to);

    QMap<int, int> oldPageNumbers = pageNumbers; // backup
    QHash<int, Page> oldPages = pages; // backup

    pageNumbers.clear();
    pages.clear();

    foreach (int id, oldPages.keys()) {
        Page page = oldPages[id];
        if (diff < 0 && page.pageNumber >= from && page.pageNumber < to) {
            kWarning() << "you requested to change the page number to a number that already exist, all will end soon";
            return;
        }
        const int oldPageNumber = page.pageNumber; // debug only
        if (page.pageNumber >= from)
            page.pageNumber += diff;
        kDebug(32001) << "adjusting page number from" << oldPageNumber << "to" << page.pageNumber << "side" << page.pageSide;
        if (page.pageSide == KWPage::PageSpread) {
            if (page.pageNumber % 2 == 1) { // pagespreads can only be on even pageNumbers
                page.pageNumber++;
                diff++;
            }
            pageNumbers.insert(page.pageNumber + 1, id);
        }
        else {
            page.pageSide = page.pageNumber % 2 == 0 ? KWPage::Left : KWPage::Right;
        }
        pageNumbers.insert(page.pageNumber, id);
        pages.insert(id, page);
    }

    Q_ASSERT(pages.count() == oldPages.count()); // don't loose anything :)
}

///////////

KWPageManager::KWPageManager()
    : d (new KWPageManagerPrivate())
{
}

KWPageManager::~KWPageManager()
{
    delete d;
}

int KWPageManager::pageNumber(const QPointF &point) const
{
    qreal startOfpage = 0.0;
    int answer = -1;
    QMap<int, int>::const_iterator iter = d->pageNumbers.begin();
    for (;iter != d->pageNumbers.end(); ++iter) {
        const KWPageManagerPrivate::Page page = d->pages[iter.value()];
        if (page.pageSide == KWPage::PageSpread && iter.key()%2 == 1)
            continue;
        startOfpage += page.style.pageLayout().height + d->padding.top + d->padding.bottom;
        answer = iter.key();
        if (startOfpage >= point.y())
            break;
    }
    return answer;
}

int KWPageManager::pageNumber(const KoShape *shape) const
{
    return pageNumber(shape->absolutePosition());
}

int KWPageManager::pageNumber(const qreal y) const
{
    return pageNumber(QPointF(0, y));
}

int KWPageManager::pageCount() const
{
    int count = 0;
    QHash<int,KWPageManagerPrivate::Page>::const_iterator iter = d->pages.constBegin();
    while (iter != d->pages.constEnd()) {
        if (iter.value().pageSide == KWPage::PageSpread)
            ++count;
        ++count;
        ++iter;
    }
    return count;
}

KWPage KWPageManager::page(int pageNum) const
{
    if (d->pageNumbers.contains(pageNum))
        return KWPage(d, d->pageNumbers.value(pageNum));

#ifdef DEBUG_PAGES
    kWarning(32001) << "KWPageManager::page(" << pageNum << ") failed; Requested page does not exist";
#endif
    return KWPage();
}

KWPage KWPageManager::page(const KoShape *shape) const
{
    return page(pageNumber(shape));
}

KWPage KWPageManager::page(const QPointF &point) const
{
    return page(pageNumber(point));
}

KWPage KWPageManager::page(qreal y) const
{
    return page(pageNumber(y));
}

KWPage KWPageManager::insertPage(int pageNumber, const KWPageStyle &pageStyle)
{
    if (pageNumber <= 0 || (!d->pages.isEmpty() && last().pageNumber() < pageNumber))
        return appendPage(pageStyle);

    KWPageManagerPrivate::Page newPage;
    newPage.style = pageStyle;

    KWPage prevPage = page(pageNumber-1);
    if (prevPage.isValid()) {
        if (pageNumber % 2 == 1 && prevPage.pageSide() == KWPage::PageSpread)
            pageNumber++; // don't insert in the middle of a pageSpread.
        if (! newPage.style.isValid())
            newPage.style = prevPage.pageStyle();
    }
    else {
        // TODO create a new page style that starts with the wanted page number
    }

    if (! newPage.style.isValid())
        newPage.style = defaultPageStyle();
    newPage.pageNumber = pageNumber;

    // increase the pagenumbers of pages following the pageNumber
    QMap<int, int> pageNumbers = d->pageNumbers;
    if (pageNumbers.count()) {
        QMap<int, int>::iterator iter = pageNumbers.end();
        do {
            --iter;

            if (iter.key() < pageNumber)
                break;
            KWPageManagerPrivate::Page page = d->pages[iter.value()];
            d->pageNumbers.remove(page.pageNumber);
            page.pageNumber++;
            d->pages.insert(iter.value(), page);
            d->pageNumbers.insert(page.pageNumber, iter.value());
        } while (iter != pageNumbers.begin());
    }

    newPage.pageSide = newPage.pageNumber % 2 == 0 ? KWPage::Left : KWPage::Right;
    d->pages.insert(++d->lastId, newPage);
    Q_ASSERT(! d->pageNumbers.contains(newPage.pageNumber));
    d->pageNumbers.insert(newPage.pageNumber, d->lastId);
    kDebug(32001) << "pageNumber=" << pageNumber << "pageCount=" << pageCount();

    return KWPage(d, d->lastId);
}

KWPage KWPageManager::insertPage(const KWPage &page)
{
#if 0
    Q_ASSERT(page.pageNumber() <= pageCount());
    Q_ASSERT(page.pageNumber() == pageCount() || page != this->page(page->pageNumber()));
    for (int i = page->pageNumber(); i < m_pageList.count(); ++i) { // increase the pagenumbers of pages following the pageNumber
        m_pageList[i]->m_pageNum = m_pageList[i]->m_pageNum + 1;
    }
    if (page->pageNumber() < pageCount()) {
        m_pageList.insert(page->pageNumber(), page);
    } else {
        m_pageList.append(page);
    }
    kDebug(32001) << "pageNumber=" << page->pageNumber() << "pageCount=" << pageCount();
#endif
    return page;
}

KWPage KWPageManager::appendPage(const KWPageStyle &pageStyle)
{
    KWPageManagerPrivate::Page page;

    if (! d->pages.isEmpty()) {
        QMap<int, int>::iterator end = d->pageNumbers.end();
        --end; // last one is one before the imaginary 'end'
        KWPageManagerPrivate::Page lastPage = d->pages[end.value()];
        page = lastPage;
        ++page.pageNumber;
        if (lastPage.pageSide == KWPage::PageSpread)
            ++page.pageNumber;
    }
    else {
        page.pageNumber = 1;
    }
    page.pageSide = page.pageNumber % 2 == 0 ? KWPage::Left : KWPage::Right;

    page.style = pageStyle;
    if (! page.style.isValid())
        page.style = defaultPageStyle();

    d->pages.insert(++d->lastId, page);
    d->pageNumbers.insert(page.pageNumber, d->lastId);
    kDebug(32001) << "pageNumber=" << page.pageNumber << "pageCount=" << pageCount();

    return KWPage(d, d->lastId);
}

qreal KWPageManager::topOfPage(int pageNum) const
{
    return d->pageOffset(pageNum, false);
}

qreal KWPageManager::bottomOfPage(int pageNum) const
{
    return d->pageOffset(pageNum, true);
}

void KWPageManager::removePage(int pageNumber)
{
    removePage(page(pageNumber));
}

void KWPageManager::removePage(const KWPage &page)
{
    if (!page.isValid())
        return;
    const int removedPageNumber = page.pageNumber();
    d->pages.remove(d->pageNumbers[page.pageNumber()]);

    // decrease the pagenumbers of pages following the pageNumber
    QMap<int, int> pageNumbers = d->pageNumbers;
    QMap<int, int>::iterator iter = pageNumbers.begin();
    while(iter != pageNumbers.end()) {
        if (iter.key() > removedPageNumber) {
            KWPageManagerPrivate::Page page = d->pages[iter.value()];
            d->pageNumbers.remove(page.pageNumber);
            page.pageNumber--;
            d->pages.insert(iter.value(), page);
            d->pageNumbers.insert(page.pageNumber, iter.value());
        }
        ++iter;
    }
    kDebug(32001) << "pageNumber=" << removedPageNumber << "pageCount=" << pageCount();
}

QPointF KWPageManager::clipToDocument(const QPointF &point) const
{
    qreal startOfpage = 0.0;

    KWPage page;
    // decrease the pagenumbers of pages following the pageNumber
    QMap<int, int>::const_iterator iter = d->pageNumbers.begin();
    while(iter != d->pageNumbers.end()) {
        const KWPageManagerPrivate::Page p = d->pages[iter.value()];
        startOfpage += p.style.pageLayout().height + d->padding.top + d->padding.bottom;
        if (startOfpage >= point.y()) {
            page = KWPage(d, iter.value());
            break;
        }
        ++iter;
    }
    if (! page.isValid())
        page = last();

    QRectF rect = page.rect();
    if (rect.contains(point))
        return point;

    QPointF rc(point);
    if (rect.top() > rc.y())
        rc.setY(rect.top());
    else if (rect.bottom() < rc.y())
        rc.setY(rect.bottom());

    if (rect.left() > rc.x())
        rc.setX(rect.left());
    else if (rect.right() < rc.x())
        rc.setX(rect.right());
    return rc;
}

QList<KWPage> KWPageManager::pages() const
{
    QList<KWPage> answer;
    foreach(int key, d->pages.keys()) {
        answer << KWPage(d, key);
    }
    return answer;
}

// **** PageList ****

QHash<QString, KWPageStyle> KWPageManager::pageStyles() const
{
    return d->pageStyles;
}

KWPageStyle KWPageManager::pageStyle(const QString &name) const
{
    if (d->pageStyles.contains(name))
        return d->pageStyles[name];
    return KWPageStyle();
}

void KWPageManager::addPageStyle(const KWPageStyle &pageStyle)
{
    Q_ASSERT(! pageStyle.name().isEmpty());
    d->pageStyles.insert(pageStyle.name(), pageStyle);
}

KWPageStyle KWPageManager::addPageStyle(const QString &name)
{
    if (d->pageStyles.contains(name))
        return d->pageStyles[name];
    KWPageStyle pagestyle(name);
    addPageStyle(pagestyle);
    return pagestyle;
}

KWPageStyle KWPageManager::defaultPageStyle() const
{
    return d->defaultPageStyle;
}

void KWPageManager::clearPageStyles()
{
    d->pageStyles.clear();
    d->defaultPageStyle = addPageStyle("Standard");
}

const KWPage KWPageManager::begin() const
{
    if (d->pages.isEmpty())
        return KWPage();
    return KWPage(d, d->pageNumbers.begin().value());
}

const KWPage KWPageManager::last() const
{
    if (d->pages.isEmpty())
        return KWPage();
    QMap<int, int>::iterator end = d->pageNumbers.end();
    --end; // last one is one before the imaginary 'end'
    return KWPage(d, end.value());
}

KWPage KWPageManager::begin()
{
    if (d->pages.isEmpty())
        return KWPage();
    return KWPage(d, d->pageNumbers.begin().value());
}

KWPage KWPageManager::last()
{
    if (d->pages.isEmpty())
        return KWPage();
    QMap<int, int>::iterator end = d->pageNumbers.end();
    --end; // last one is one before the imaginary 'end'
    return KWPage(d, end.value());
}

KoInsets KWPageManager::padding() const
{
    return d->padding;
}

void KWPageManager::setPadding(const KoInsets &padding)
{
    d->padding = padding;
}

bool KWPageManager::preferPageSpread() const
{
    return d->preferPageSpread;
}

void KWPageManager::setPreferPageSpread(bool on)
{
    d->preferPageSpread = on;
}

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
    preferPageSpread(false)
{
}

qreal KWPageManagerPrivate::pageOffset(int pageNum, bool bottom) const
{
    Q_ASSERT(pageNum >= 0);
    qreal offset = 0.0;

    // decrease the pagenumbers of pages following the pageNumber
    QMap<int, int>::const_iterator iter = pageNumbers.begin();
    while(iter != pageNumbers.end()) {
        const KWPageManagerPrivate::Page page = pages[iter.value()];
        if (iter.key() == pageNum) {
            if (bottom)
                offset += page.style.pageLayout().height;
            break;
        }
        offset += page.style.pageLayout().height + padding.top + padding.bottom;
        ++iter;
    }
    return offset;
}

void KWPageManagerPrivate::setPageNumberForId(int pageId, int newPageNumber)
{
    if (pageNumbers.isEmpty() || ! pageNumbers.contains(pageId))
        return;
    Q_ASSERT(pages.contains(pageId));
//   if (pages[pageId].pageNumber == newPageNumber)
//       return;

    const int oldPageNumber = pages[pageId].pageNumber;
    const int diff = newPageNumber - oldPageNumber;
    int from = oldPageNumber;
    int to = newPageNumber;
    if (from > to)
        qSwap(from, to);

    QMap<int, int> oldPageNumbers = pageNumbers; // backup
    QHash<int, Page> oldPages = pages; // backup
    Q_ASSERT(oldPages.count() == oldPageNumbers.count());

    pageNumbers.clear();
    pages.clear();

    foreach (int id, oldPages.keys()) {
        Page page = oldPages[id];
        if (diff < 0 && page.pageNumber >= from && page.pageNumber < to) {
            kWarning() << "you requested to change the page number to a number that already exist, all will end soon";
            return;
        }
int oldPageNumber = page.pageNumber;
        if (page.pageNumber >= from)
            page.pageNumber += diff;
qDebug() << "adjusting page number from" << oldPageNumber << "to" << page.pageNumber;
        pageNumbers.insert(page.pageNumber, id);
        pages.insert(id, page);
    }

    Q_ASSERT(pageNumbers.count() == pages.count() == oldPages.count());
}

///////////

KWPageManager::KWPageManager()
    : d (new KWPageManagerPrivate())
{
    KWPageStyle defaultpagestyle("Standard");
    defaultpagestyle.setPageLayout(KoPageLayout::standardLayout());
    addPageStyle(defaultpagestyle);
}

KWPageManager::~KWPageManager()
{
    delete d;
}

int KWPageManager::pageNumber(const QPointF &point) const
{
    int pageNumber = -1;
    qreal startOfpage = 0.0;
    foreach (int pageId, d->pageNumbers.values()) {
        if (startOfpage >= point.y())
            break;
        KWPage page(d, pageId);
        startOfpage += page.height();
        pageNumber = page.pageNumber();
    }
#ifdef DEBUG_PAGES
    if (pageNumber < 0) {
        kWarning(32001) << "KWPageManager::pageNumber(" << point << ") failed; QPoint does not have a valid page";
        kDebug(32001) << kBacktrace();
    }
#endif
    return pageNumber;
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
    return d->pageNumbers.count();
}

KWPage KWPageManager::page(int pageNum) const
{
#if 0
    foreach(KWPage *page, m_pageList) {
        if (page->pageNumber() == pageNum ||
                (page->pageSide() == KWPage::PageSpread && page->pageNumber() + 1 == pageNum))
            return page;
    }
#ifdef DEBUG_PAGES
    kWarning(32001) << "KWPageManager::page(" << pageNum << ") failed; Requested page does not exist";
    kDebug(32001) << kBacktrace();
#endif
    return 0;
#endif
    if (d->pageNumbers.contains(pageNum))
        return KWPage(d, d->pageNumbers.value(pageNum));

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
    return page;
#endif
}

KWPage KWPageManager::appendPage(const KWPageStyle &pageStyle)
{
    KWPageManagerPrivate::Page page;
    page.style = pageStyle;

    KWPage lastPage = last();
    if (!page.style.isValid()) {
        if (lastPage.isValid())
            page.style = lastPage.pageStyle();
        else
            page.style = defaultPageStyle();
    }

    if (lastPage.isValid()) {
        page.pageNumber = lastPage.pageNumber() + 1;
        if (lastPage.pageSide() == KWPage::PageSpread)
            page.pageNumber++;
    }
    else
        page.pageNumber = 1;
    page.pageSide = page.pageNumber % 2 == 0 ? KWPage::Left : KWPage::Right;

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

QPointF KWPageManager::clipToDocument(const QPointF &point)
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
    foreach(int key, d->pageNumbers.values()) {
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
    Q_ASSERT(! pageStyle.masterName().isEmpty());
    Q_ASSERT(! d->pageStyles.contains(pageStyle.masterName())); // This should never occur...
    d->pageStyles.insert(pageStyle.masterName(), pageStyle);
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
    return d->pageStyles["Standard"];
}

void KWPageManager::clearPageStyle()
{
    d->pageStyles.clear();
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

const KoInsets &KWPageManager::padding() const
{
    return d->padding;
}

KoInsets &KWPageManager::padding()
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

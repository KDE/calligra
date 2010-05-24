/* This file is part of the KOffice project
 * Copyright (C) 2005-2010 Thomas Zander <zander@kde.org>
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
#include "KWPageStyle_p.h"
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
    const qreal totalPadding = padding.top + padding.bottom;

    QMap<int, int>::const_iterator iter = pageNumbers.constBegin();
    for (;iter != pageNumbers.constEnd(); ++iter) {
        const KWPageManagerPrivate::Page page = pages.value(iter.value());
        if (page.pageSide == KWPage::PageSpread && iter.key() % 2 == 1)
            continue;
        if (iter.key() == pageNum) {
            if (bottom)
                offset += page.style.pageLayout().height;
            break;
        }
        offset += page.style.priv()->pageLayout.height + totalPadding;
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
            kWarning(32001) << "you requested to change the page number to a number that already exist, all will end soon";
            return;
        }
#ifdef DEBUG_PAGES
        const int oldPageNumber = page.pageNumber; // debug only
#endif
        if (page.pageNumber >= from)
            page.pageNumber += diff;
#ifdef DEBUG_PAGES
        kDebug(32001) << "adjusting page number from" << oldPageNumber << "to" << page.pageNumber << "side" << page.pageSide;
#endif
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

void KWPageManagerPrivate::insertPage(const Page &newPage)
{
    // increase the pagenumbers of pages following the pageNumber
    if (!pageNumbers.isEmpty()) {
        QMap<int, int> numbers = pageNumbers;
        const int offset = newPage.pageSide == KWPage::PageSpread ? 2 : 1;
        QMap<int, int>::iterator iter = numbers.end();
        do {
            --iter;

            if (iter.key() < newPage.pageNumber)
                break;
            KWPageManagerPrivate::Page page = pages[iter.value()];
            pageNumbers.remove(page.pageNumber);
            page.pageNumber += offset;
            pages.insert(iter.value(), page);
            pageNumbers.insert(page.pageNumber, iter.value());
        } while (iter != numbers.begin());
    }

    pages.insert(++lastId, newPage);
    Q_ASSERT(! pageNumbers.contains(newPage.pageNumber));
    pageNumbers.insert(newPage.pageNumber, lastId);
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
    QMap<int, int>::const_iterator iter = d->pageNumbers.constBegin();
    for (;iter != d->pageNumbers.constEnd(); ++iter) {
        const KWPageManagerPrivate::Page page = d->pages[iter.value()];
        if (page.pageSide == KWPage::PageSpread && iter.key() % 2 == 1)
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
    if (pageNumber <= 0 || d->pages.isEmpty() || pageNumber > last().pageNumber())
        return appendPage(pageStyle);

    KWPageManagerPrivate::Page newPage;
    newPage.style = pageStyle;

    KWPage prevPage = page(pageNumber - 1);
    if (prevPage.isValid()) {
        if (pageNumber % 2 == 1 && prevPage.pageSide() == KWPage::PageSpread)
            pageNumber++; // don't insert in the middle of a pageSpread.
        if (! newPage.style.isValid())
            newPage.style = prevPage.pageStyle();
    }

    if (! newPage.style.isValid())
        newPage.style = defaultPageStyle();
    newPage.pageNumber = pageNumber;
    if (newPage.pageNumber % 2 == 0) {
        if (newPage.style.pageLayout().bindingSide >= 0) // pageSpread
            newPage.pageSide = KWPage::PageSpread;
        else
            newPage.pageSide = KWPage::Left;
    } else {
        newPage.pageSide = KWPage::Right;
    }

    d->insertPage(newPage);

#ifdef DEBUG_PAGES
    kDebug(32001) << "pageNumber=" << pageNumber << "pageCount=" << pageCount();
#endif

    return KWPage(d, d->lastId);
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
    } else {
        page.pageNumber = 1;
    }

    if (pageStyle.isValid()) {
        page.style = pageStyle;
    } else {
        if (page.style.isValid()) {
            QString nextPageMasterStyleName = page.style.nextStyleName();
            KWPageStyle nextPageMasterStyle = this->pageStyle(nextPageMasterStyleName);
            if (nextPageMasterStyle.isValid()) {
                page.style = nextPageMasterStyle;
            }
        }
    }
    if (!page.style.isValid()) {
        page.style = defaultPageStyle();
    }

    if (page.pageNumber % 2 == 0) {
        if (page.style.pageLayout().bindingSide >= 0) // pageSpread
            page.pageSide = KWPage::PageSpread;
        else
            page.pageSide = KWPage::Left;
    } else {
        page.pageSide = KWPage::Right;
    }

    d->pages.insert(++d->lastId, page);
    d->pageNumbers.insert(page.pageNumber, d->lastId);
#ifdef DEBUG_PAGES
    kDebug(32001) << "pageNumber=" << page.pageNumber << "pageCount=" << pageCount();
#endif

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
    d->pages.remove(d->pageNumbers[removedPageNumber]);

    // decrease the pagenumbers of pages following the pageNumber
    QMap<int, int> pageNumbers = d->pageNumbers;
    QMap<int, int>::iterator iter = pageNumbers.begin();
    while (iter != pageNumbers.end()) {
        if (iter.key() > removedPageNumber) {
            KWPageManagerPrivate::Page page = d->pages[iter.value()];
            d->pageNumbers.remove(page.pageNumber);
            page.pageNumber--;
            d->pages.insert(iter.value(), page);
            d->pageNumbers.insert(page.pageNumber, iter.value());
        }
        else if (iter.key() == removedPageNumber) {
            d->pageNumbers.remove(iter.key());
        }
        ++iter;
    }
#ifdef DEBUG_PAGES
    kDebug(32001) << "pageNumber=" << removedPageNumber << "pageCount=" << pageCount();
#endif
}

QPointF KWPageManager::clipToDocument(const QPointF &point) const
{
    qreal startOfpage = 0.0;

    KWPage page;
    // decrease the pagenumbers of pages following the pageNumber
    QMap<int, int>::const_iterator iter = d->pageNumbers.constBegin();
    while (iter != d->pageNumbers.constEnd()) {
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

QList<KWPage> KWPageManager::pages(const QString &pageStyle) const
{
    QList<KWPage> answer;
    const bool checkForStyle = !pageStyle.isEmpty();
    foreach (int key, d->pages.keys()) {
        if (checkForStyle && d->pages.value(key).style.name() != pageStyle)
            continue;
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
    Q_ASSERT(pageStyle.isValid());
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

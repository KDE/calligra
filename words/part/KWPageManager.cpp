/* This file is part of the Calligra project
 * Copyright (C) 2005-2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 * Copyright (C) 2008,2011 Sebastian Sauer <mail@dipe.org>
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

#include <WordsDebug.h>

#define DEBUG_PAGES

KWPageManagerPrivate::KWPageManagerPrivate()
        : lastId(0),
        defaultPageStyle(QLatin1String("Standard")) // don't translate, used as identifier!
{
}

qreal KWPageManagerPrivate::pageOffset(int pageNum/*, bool bottom*/) const
{
#if 0
    Q_ASSERT(pageNum >= 0);
    qreal offset = 0.0;
    const qreal totalPadding = padding.top + padding.bottom;

    QMap<int, int>::const_iterator iter = pageNumbers.constBegin();
    for (;iter != pageNumbers.constEnd(); ++iter) {
        const KWPageManagerPrivate::Page &page = pages[iter.value()];
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
#else
    //Q_ASSERT(pageOffsets.contains(pageNum));
    qreal offset = pageOffsets.value(pageNum);
    //debugWords << "pageNum=" << pageNum << "offset=" << offset;
    return offset;
#endif
}

void KWPageManagerPrivate::setPageOffset(int pageNum, qreal offset)
{
    pageOffsets[pageNum] = offset;
}

void KWPageManagerPrivate::setVisiblePageNumber(int pageId, int newPageNumber)
{
#if 0
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
            warnWords << "you requested to change the page number to a number that already exist, all will end soon";
            return;
        }
#ifdef DEBUG_PAGES
        const int oldPageNumber = page.pageNumber; // debug only
#endif
        if (page.pageNumber >= from)
            page.pageNumber += diff;
#ifdef DEBUG_PAGES
        debugWords << "adjusting page number from" << oldPageNumber << "to" << page.pageNumber << "side" << page.pageSide;
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
#else
    if (newPageNumber >= 0)
        visiblePageNumbers[pageId] = newPageNumber;
    else
        visiblePageNumbers.remove(pageId);
#endif
}

void KWPageManagerPrivate::insertPage(const Page &newPage)
{
#ifdef DEBUG_PAGES
    debugWords << "pageNumber=" << newPage.pageNumber;
#endif

    // increase the pagenumbers of pages following the pageNumber
    if (!pageNumbers.isEmpty()) {
        const QMap<int, int> numbers = pageNumbers;
        QMap<int, int>::ConstIterator iter = numbers.end();
        do {
            --iter;

            if (iter.key() < newPage.pageNumber)
                break;
            KWPageManagerPrivate::Page page = pages[iter.value()];
            pageNumbers.remove(iter.key());
            page.pageNumber += 1;
            pages.insert(iter.value(), page);
            pageNumbers.insert(iter.key() + 1, iter.value());
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
    addPageStyle(d->defaultPageStyle);
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
    warnWords << "KWPageManager::page(" << pageNum << ") failed; Requested page does not exist";
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

#ifdef DEBUG_PAGES
    debugWords << "pageNumber=" << pageNumber << "pageStyle=" << (pageStyle.isValid() ? pageStyle.name() : QString());
#endif

    KWPageManagerPrivate::Page newPage;
    newPage.style = pageStyle;

    KWPage prevPage = page(pageNumber - 1);
    if (prevPage.isValid()) {
        if (! newPage.style.isValid())
            newPage.style = prevPage.pageStyle();
    }

    if (! newPage.style.isValid())
        newPage.style = defaultPageStyle();
    newPage.pageNumber = pageNumber;
    if (newPage.pageNumber % 2 == 0) {
        newPage.pageSide = KWPage::Left;
    } else {
        newPage.pageSide = KWPage::Right;
    }

    d->insertPage(newPage);

    return KWPage(d, d->lastId);
}

KWPage KWPageManager::appendPage(const KWPageStyle &pageStyle)
{
    KWPageManagerPrivate::Page page;

    if (! d->pages.isEmpty()) {
        QMap<int, int>::ConstIterator end = d->pageNumbers.constEnd();
        --end; // last one is one before the imaginary 'end'
        KWPageManagerPrivate::Page lastPage = d->pages[end.value()];
        page = lastPage;
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
        page.pageSide = KWPage::Left;
    } else {
        page.pageSide = KWPage::Right;
    }

    d->pages.insert(++d->lastId, page);
    d->pageNumbers.insert(page.pageNumber, d->lastId);

#ifdef DEBUG_PAGES
    debugWords << "pageNumber=" << page.pageNumber << "pageCount=" << pageCount() << "pageStyle=" << (pageStyle.isValid() ? pageStyle.name() : QString());
#endif

    return KWPage(d, d->lastId);
}

qreal KWPageManager::topOfPage(int pageNum) const
{
    return d->pageOffset(pageNum);
}

qreal KWPageManager::bottomOfPage(int pageNum) const
{
    KWPage p = page(pageNum);
    Q_ASSERT(p.isValid());
    return d->pageOffset(pageNum) + p.height();
}

void KWPageManager::removePage(int pageNumber)
{
    removePage(page(pageNumber));
}

void KWPageManager::removePage(const KWPage &page)
{
    Q_ASSERT(page.isValid());
    debugWords << page.pageNumber();

    const int removedPageNumber = page.pageNumber();
    d->pages.remove(d->pageNumbers[removedPageNumber]);
    d->visiblePageNumbers.remove(removedPageNumber);

    // decrease the pagenumbers of pages following the pageNumber
    const QMap<int, int> pageNumbers = d->pageNumbers;
    QMap<int, int>::ConstIterator iter = pageNumbers.begin();
    while (iter != pageNumbers.end()) {
        if (iter.key() < removedPageNumber) {
            //  don't touch those
        } else if (iter.key() > removedPageNumber) {
            KWPageManagerPrivate::Page page = d->pages[iter.value()];
            d->pageNumbers.remove(iter.key());
            page.pageNumber--;
            d->pages.insert(iter.value(), page);
            d->pageNumbers.insert(page.pageNumber, iter.value());
        } else {
            d->pageNumbers.remove(iter.key());
        }
        ++iter;
    }

#ifdef DEBUG_PAGES
    debugWords << "pageNumber=" << removedPageNumber << "pageCount=" << pageCount();
#endif
}

QVector<KWPage> KWPageManager::pages(const QString &pageStyle) const
{
    QVector<KWPage> answer;
    const bool checkForStyle = !pageStyle.isEmpty();
    QHash<int, KWPageManagerPrivate::Page>::ConstIterator it = d->pages.constBegin();
    QHash<int, KWPageManagerPrivate::Page>::ConstIterator end = d->pages.constEnd();
    for(; it != end; ++it) {
        if (checkForStyle && it.value().style.name() != pageStyle)
            continue;
        answer << KWPage(d, it.key());
    }
    std::sort(answer.begin(), answer.end());
    return answer;
}

QHash<QString, KWPageStyle> KWPageManager::pageStyles() const
{
    return d->pageStyles;
}

KWPageStyle KWPageManager::pageStyle(const QString &name) const
{
    if (d->pageStyles.contains(name))
        return d->pageStyles[name];
    if (d->pageStyleNames.contains(name))
        return d->pageStyles[d->pageStyleNames[name]];
    return KWPageStyle();
}

void KWPageManager::addPageStyle(const KWPageStyle &pageStyle)
{
    Q_ASSERT(!pageStyle.name().isEmpty());
    Q_ASSERT(pageStyle.isValid());
    d->pageStyles.insert(pageStyle.name(), pageStyle);
    if (!pageStyle.displayName().isEmpty())
        d->pageStyleNames.insert(pageStyle.displayName(), pageStyle.name());
}

void KWPageManager::removePageStyle(const KWPageStyle &pageStyle)
{
    KWPageStyle style = d->pageStyles.value(pageStyle.name());
    Q_ASSERT(style == pageStyle);
    d->pageStyles.remove(pageStyle.name());
    Q_ASSERT(!d->pageStyleNames.contains(pageStyle.displayName()) || d->pageStyleNames[pageStyle.displayName()] == pageStyle.name());
    d->pageStyleNames.remove(pageStyle.displayName());
}

KWPageStyle KWPageManager::defaultPageStyle() const
{
    return d->defaultPageStyle;
}

void KWPageManager::clearPageStyles()
{
    d->pageStyles.clear();
    d->pageStyleNames.clear();
    d->defaultPageStyle = KWPageStyle(QLatin1String("Standard")); // don't translate, used as identifier!
    addPageStyle(d->defaultPageStyle);
}

const KWPage KWPageManager::begin() const
{
    if (d->pages.isEmpty() || d->pageNumbers.empty())
        return KWPage();
    return KWPage(d, d->pageNumbers.constBegin().value());
}

const KWPage KWPageManager::last() const
{
    if (d->pages.isEmpty() || d->pageNumbers.empty())
        return KWPage();
    QMap<int, int>::ConstIterator end = d->pageNumbers.constEnd();
    --end; // last one is one before the imaginary 'end'
    return KWPage(d, end.value());
}

KWPage KWPageManager::begin()
{
    if (d->pages.isEmpty() || d->pageNumbers.empty())
        return KWPage();
    return KWPage(d, d->pageNumbers.begin().value());
}

KWPage KWPageManager::last()
{
    if (d->pages.isEmpty() || d->pageNumbers.empty())
        return KWPage();
    QMap<int, int>::ConstIterator end = d->pageNumbers.constEnd();
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

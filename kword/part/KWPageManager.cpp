/* This file is part of the KOffice project
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
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
#include "KWPage.h"
#include "KWDocument.h"

#include <KoShape.h>
#include <KoUnit.h>

#include <KDebug>

//#define DEBUG_PAGES

KWPageManager::KWPageManager(KWDocument* document)
    : m_document(document),
    m_preferPageSpread(false)
{
    clearPageStyle(); // creates also a new default style.
}

KWPageManager::~KWPageManager() {
    qDeleteAll(m_pageStyle);
    qDeleteAll(m_pageList);
}

int KWPageManager::pageNumber(const QPointF &point) const {
    int pageNumber = -1;
    qreal startOfpage = 0.0;
    foreach(KWPage *page, m_pageList) {
        if(startOfpage >= point.y())
            break;
        startOfpage += page->height();
        pageNumber = page->pageNumber();
    }
#ifdef DEBUG_PAGES
    if(pageNumber < 0) {
        kWarning(31001) << "KWPageManager::pageNumber(" << point << ") failed; QPoint does not have a valid page";
        kDebug(32001) << kBacktrace();
    }
#endif
    return pageNumber;
}

int KWPageManager::pageNumber(const KoShape *shape) const {
    return pageNumber(shape->absolutePosition());
}
int KWPageManager::pageNumber(const qreal y) const {
    return pageNumber(QPointF(0, y));
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
                (page->pageSide() == KWPage::PageSpread && page->pageNumber()+1 == pageNum))
            return page;
    }
#ifdef DEBUG_PAGES
    kWarning(31001) << "KWPageManager::page(" << pageNum << ") failed; Requested page does not exist";
    kDebug(32001) << kBacktrace();
#endif
    return 0;
}
KWPage* KWPageManager::page(const KoShape *shape) const {
    return page(pageNumber(shape));
}
KWPage* KWPageManager::page(const QPointF &point) const {
    return page(pageNumber(point));
}
KWPage* KWPageManager::page(qreal y) const {
    return page(pageNumber(y));
}

KWPage* KWPageManager::insertPage(int pageNumber, KWPageStyle *pageStyle) {
    Q_ASSERT( pageNumber >= 0 && pageNumber <= pageCount() );
    if(pageNumber >= pageCount()) {
        return appendPage(pageStyle);
    }
    if (! pageStyle) {
        KWPage *p = (pageNumber > 0) ? this->page(pageNumber - 1) : 0;
        pageStyle = p ? p->pageStyle() : this->defaultPageStyle();
    }
    for(int i = pageNumber; i < m_pageList.count(); ++i) { // increase the pagenumbers of pages following the pageNumber
        m_pageList[i]->m_pageNum++;
    }
    KWPage *page = new KWPage(this, pageNumber, pageStyle);
    m_pageList.insert(pageNumber, page);
    kDebug(31001) << "pageNumber=" << pageNumber << "pageCount=" << pageCount();
    return page;
}

KWPage* KWPageManager::insertPage(KWPage *page) {
    Q_ASSERT( page->pageNumber() <= pageCount() );
    Q_ASSERT( page->pageNumber() == pageCount() || page != this->page(page->pageNumber()) );
    for(int i = page->pageNumber(); i < m_pageList.count(); ++i) { // increase the pagenumbers of pages following the pageNumber
        m_pageList[i]->m_pageNum++;
    }
    if(page->pageNumber() < pageCount()) {
        m_pageList.insert(page->pageNumber(), page);
    } else {
        m_pageList.append(page);
    }
    kDebug(31001) << "pageNumber=" << page->pageNumber() << "pageCount=" << pageCount();
    return page;
}

KWPage* KWPageManager::appendPage(KWPageStyle *pageStyle) {
    if (! pageStyle) {
        KWPage *p = this->page(m_pageList.count() - 1);
        pageStyle = p ? p->pageStyle() : this->defaultPageStyle();
    }
    KWPage *page = new KWPage(this, m_pageList.count(), pageStyle);
    m_pageList.append(page);
    kDebug(31001) << "pageNumber=" << page->pageNumber() << "pageCount=" << pageCount();
    return page;
}

qreal KWPageManager::topOfPage(int pageNum) const {
    return pageOffset(pageNum, false);
}
qreal KWPageManager::bottomOfPage(int pageNum) const {
    return pageOffset(pageNum, true);
}

qreal KWPageManager::pageOffset(int pageNum, bool bottom) const
{
    Q_ASSERT(pageNum >= 0);
    qreal offset = 0.0;
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
    delete page;
}

QPointF KWPageManager::clipToDocument(const QPointF &point) {
    int page = 0;
    qreal startOfpage = 0.0;

    foreach (KWPage *p, m_pageList) {
        startOfpage += p->height();
        if(startOfpage >= point.y())
            break;
        page++;
    }
    page = qMin(page, pageCount() - 1);
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

QHash<QString, KWPageStyle *> KWPageManager::pageStyles() const
{
    return m_pageStyle;
}

KWPageStyle *KWPageManager::pageStyle(const QString &name) const {
    if (m_pageStyle.contains(name))
        return m_pageStyle[name];
    return 0;
}

void KWPageManager::addPageStyle(KWPageStyle *pageStyle) {
    const QString masterpagename = pageStyle->masterName();
    Q_ASSERT(! masterpagename.isEmpty());
    Q_ASSERT(! m_pageStyle.contains(masterpagename)); // This should never occur...
    m_pageStyle[masterpagename] = pageStyle;
    QObject::connect(pageStyle, SIGNAL(relayout()), m_document, SLOT(relayout()));
}

KWPageStyle* KWPageManager::addPageStyle(const QString &name)
{
    if(m_pageStyle.contains(name))
        return m_pageStyle[name];
    KWPageStyle* pagestyle = new KWPageStyle(name);
    addPageStyle(pagestyle);
    return pagestyle;
}

KWPageStyle* KWPageManager::defaultPageStyle() const {
    Q_ASSERT(m_pageStyle.contains("Standard"));
    return m_pageStyle["Standard"];
}

void KWPageManager::clearPageStyle() {
    qDeleteAll(m_pageStyle);
    m_pageStyle.clear();

    KWPageStyle* defaultpagestyle = new KWPageStyle("Standard");
    defaultpagestyle->setPageLayout(KoPageLayout::standardLayout());
    addPageStyle(defaultpagestyle);
}

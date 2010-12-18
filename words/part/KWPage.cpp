/* This file is part of the KOffice project
 * Copyright (C) 2005, 2008, 2010 Thomas Zander <zander@kde.org>
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

#include "KWPage.h"
#include "KWPageManager_p.h"

#include <KDebug>

void KWPage::setPageNumber(int pageNumber)
{
    if (isValid())
        priv->setPageNumberForId(n, pageNumber);
}

bool KWPage::isValid() const
{
    return priv && priv->pages.contains(n);
}

int KWPage::pageNumber() const
{
    if (! isValid())
        return -1; // invalid
    return priv->pages[n].pageNumber;
}

KWPageStyle KWPage::pageStyle() const
{
    if (! isValid())
        return KWPageStyle(); // invalid

    return priv->pages[n].style;
}

void KWPage::setPageStyle(const KWPageStyle style)
{
    if (! isValid())
        return;
    if (! style.isValid()) {
        kWarning(32001) << "Passing invalid style to KWPage::setPageStyle()";
        return;
    }
    if (! priv->pageStyles.contains(style.name()))
        priv->pageStyles.insert(style.name(), style);

    KWPageManagerPrivate::Page page = priv->pages[n];
    page.style = style;
    priv->pages.insert(n, page);
}

void KWPage::setPageSide(PageSide ps)
{
    if (! isValid())
        return;
    KWPageManagerPrivate::Page page = priv->pages[n];

    if (page.pageSide == ps)
        return;

    const bool needsRenumbering = (page.pageSide == PageSpread && ps != PageSpread) || ps == PageSpread;
    page.pageSide = ps;
    priv->pages.insert(n, page);

    if (needsRenumbering)
        priv->setPageNumberForId(n, page.pageNumber);
}

KWPage::PageSide KWPage::pageSide() const
{
    if (! isValid())
        return KWPage::Left; // invalid

    return priv->pages[n].pageSide;
}

bool KWPage::operator==(const KWPage &other) const
{
    return other.n == n && other.priv == priv;
}

uint KWPage::hash() const
{
    return n + qHash(priv);
}

qreal KWPage::width() const
{
    if (! isValid())
        return 0;
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    return page.style.pageLayout().width * (page.pageSide == PageSpread ? 2 : 1);
}

qreal KWPage::height() const
{
    if (! isValid())
        return 0;
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    return page.style.pageLayout().height;
}

qreal KWPage::topMargin() const
{
    if (! isValid())
        return 0;
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    return page.style.pageLayout().topMargin;
}

qreal KWPage::bottomMargin() const
{
    if (! isValid())
        return 0;
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    return page.style.pageLayout().bottomMargin;
}

qreal KWPage::leftMargin() const
{
    if (! isValid())
        return 0;
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    qreal answer = page.pageSide == Left ? pageEdgeMargin() : marginClosestBinding();
    if (answer != -1)
        return answer;
    return page.style.pageLayout().leftMargin;
}

qreal KWPage::rightMargin() const
{
    if (! isValid())
        return 0;
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    qreal answer = page.pageSide == Right ? pageEdgeMargin() : marginClosestBinding();
    if (answer != -1)
        return answer;
    return page.style.pageLayout().rightMargin;
}

qreal KWPage::topPadding() const
{
    if (! isValid())
        return 0;
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    return page.style.pageLayout().topPadding;
}

qreal KWPage::bottomPadding() const
{
    if (! isValid())
        return 0;
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    return page.style.pageLayout().bottomPadding;
}

qreal KWPage::leftPadding() const
{
    if (! isValid())
        return 0;
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    return page.style.pageLayout().leftPadding;
}

qreal KWPage::rightPadding() const
{
    if (! isValid())
        return 0;
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    return page.style.pageLayout().rightPadding;
}

qreal KWPage::pageEdgeMargin() const
{
    if (! isValid())
        return 0;
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    return page.style.pageLayout().pageEdge;
}

qreal KWPage::marginClosestBinding() const
{
    if (! isValid())
        return 0;
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    return page.style.pageLayout().bindingSide;
}

qreal KWPage::offsetInDocument() const
{
    // the y coordinate
    return isValid() ? priv->pageOffset(priv->pages[n].pageNumber, false) : 0.;
}

QRectF KWPage::rect(int pageNumber) const
{
    if (! isValid())
        return QRectF();
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    if (pageNumber == page.pageNumber && page.pageSide == PageSpread) // left
        return QRectF(0, offsetInDocument(), width() / 2, height());
    if (pageNumber == page.pageNumber + 1 && page.pageSide == PageSpread) // right
        return QRectF(width() / 2, offsetInDocument(), width() / 2, height());
    return QRectF(0, offsetInDocument(), width(), height());
}

KoPageFormat::Orientation KWPage::orientationHint() const
{
    if (! isValid())
        return KoPageFormat::Landscape;
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    return page.orientation;
}

void KWPage::setOrientationHint(KoPageFormat::Orientation orientation)
{
    if (! isValid())
        return;
    KWPageManagerPrivate::Page page = priv->pages[n];
    page.orientation = orientation;
    priv->pages.insert(n, page);
}

const KWPage KWPage::previous() const
{
    if (! isValid())
        return KWPage();
    QMap<int,int>::const_iterator iter = priv->pageNumbers.constFind(pageNumber());
    if (iter == priv->pageNumbers.constBegin())
        return KWPage();
    --iter;
    return KWPage(priv, iter.value());
}

const KWPage KWPage::next() const
{
    if (! isValid())
        return KWPage();
    QMap<int,int>::const_iterator iter = priv->pageNumbers.constFind(pageNumber());
    Q_ASSERT(iter != priv->pageNumbers.constEnd());
    ++iter;
    if (priv->pages[n].pageSide == PageSpread) {// one more
        Q_ASSERT(iter != priv->pageNumbers.constEnd());
        ++iter;
    }
    if (iter == priv->pageNumbers.constEnd())
        return KWPage();
    return KWPage(priv, iter.value());
}

void KWPage::setDirectionHint(KoText::Direction direction)
{
    if (!isValid())
        return;
    KWPageManagerPrivate::Page page = priv->pages[n];
    page.textDirection = direction;
    priv->pages.insert(n, page);
}

KoText::Direction KWPage::directionHint() const
{
    if (! isValid())
        return KoText::AutoDirection;
    KWPageManagerPrivate::Page page = priv->pages[n];
    KoText::Direction dir = page.textDirection;
    if (dir != KoText::InheritDirection)
        return dir;
    return page.style.direction();
}

void KWPage::setAutoGenerated(bool on)
{
    if (!isValid())
        return;
    KWPageManagerPrivate::Page page = priv->pages[n];
    page.autoGenerated = on;
    priv->pages.insert(n, page);
}

bool KWPage::isAutoGenerated() const
{
    if (!isValid())
        return false;
    KWPageManagerPrivate::Page page = priv->pages[n];
    return page.autoGenerated;
}

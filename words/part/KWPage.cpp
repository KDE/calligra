/* This file is part of the Calligra project
 * Copyright (C) 2005, 2007-2008, 2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 * Copyright (C) 2005, 2007-2008, 2011 Sebastian Sauer <mail@dipe.org>
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
#include <KoShapeManager.h>
#include <KoZoomHandler.h>
#include <KoShape.h>
#include <QPainter>

#include <WordsDebug.h>

void KWPage::setVisiblePageNumber(int pageNumber)
{
    if (isValid())
        priv->setVisiblePageNumber(n, pageNumber);
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
        warnWords << "Passing invalid style to KWPage::setPageStyle()";
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

    page.pageSide = ps;
    priv->pages.insert(n, page);
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
    return page.style.pageLayout().width;
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
    return isValid() ? priv->pageOffset(priv->pages[n].pageNumber) : 0.;
}

void KWPage::setOffsetInDocument(qreal offset)
{
    priv->setPageOffset(priv->pages[n].pageNumber, offset);
}

QRectF KWPage::rect() const
{
    if (! isValid())
        return QRectF();
    return QRectF(0, offsetInDocument(), width(), height());
}

QRectF KWPage::contentRect() const
{
    if (! isValid())
        return QRectF();
    return priv->pages[n].contentRect;
}

void KWPage::setContentRect(const QRectF &rect)
{
    if (isValid()) {
        priv->pages[n].contentRect = rect;
    }
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
    const KWPageManagerPrivate::Page &page = priv->pages[n];
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
    const KWPageManagerPrivate::Page &page = priv->pages[n];
    return page.autoGenerated;
}


QImage KWPage::thumbnail(const QSize &size, KoShapeManager *shapeManager, bool asPrint) const
{
    KoZoomHandler zoomHandler;
    const qreal realWidth = zoomHandler.resolutionX() * width();
    const qreal realHeight = zoomHandler.resolutionX() * height();

    const qreal widthScale = size.width() / realWidth;
    const qreal heightScale = size.height() / realHeight;

    const qreal zoom = (widthScale > heightScale) ? heightScale : widthScale;
    // adapt thumbnailSize to match the rendered page
    QSize thumbnailSize(size);
    if (widthScale > heightScale) {
        const int thumbnailWidth = qMin(thumbnailSize.width(), qRound(realWidth*heightScale));
        thumbnailSize.setWidth(thumbnailWidth);
    } else {
        const int thumbnailHeight = qMin(thumbnailSize.height(), qRound(realHeight*widthScale));
        thumbnailSize.setHeight(thumbnailHeight);
    }

    zoomHandler.setZoom(zoom);

    foreach(KoShape* shape, shapeManager->shapes()) {
        shape->waitUntilReady(zoomHandler, false);
    }

    QImage img(thumbnailSize, QImage::Format_ARGB32);
    // paint white as default page background
    img.fill(QColor(Qt::white).rgb());
    QPainter gc(&img);
    gc.setRenderHint(QPainter::Antialiasing, true);
    gc.translate(0, -zoomHandler.documentToViewY(offsetInDocument()));
    gc.setClipRect(zoomHandler.documentToView(rect()));
    shapeManager->paint(gc, zoomHandler, asPrint);
    gc.end();

    return img;
}

int KWPage::visiblePageNumber(PageSelection select, int adjustment) const
{
    KWPage page = *(const_cast<KWPage*>(this));
    switch (select) {
    case KoTextPage::CurrentPage: break;
    case KoTextPage::PreviousPage:
        page = page.previous();
        break;
    case KoTextPage::NextPage:
        page = page.next();
        break;
    }

    if (! page.isValid())
        return -1;

    int pageNumber = 0;
    if (select != KoTextPage::CurrentPage) {
        pageNumber = page.visiblePageNumber();
    } else {
        int n = page.pageNumber();
        if (priv->visiblePageNumbers.contains(n))
            pageNumber = priv->visiblePageNumbers[n];
    }

    if (pageNumber == 0) {
        page = page.previous();
        pageNumber = page.isValid() ? qMax(1, page.visiblePageNumber() + 1) : 1;
    }

    if (adjustment != 0) {
        pageNumber += adjustment;
        if (page.priv && !page.priv->pageNumbers.contains(pageNumber))
            pageNumber = -1; // doesn't exist.
    }

    return pageNumber;
}

QString KWPage::masterPageName() const
{
    KWPageStyle pagestyle = pageStyle();
    if (pagestyle.isValid()) {
        QString name = pagestyle.name();
        if (!name.isEmpty())
            return name;
    }
    KWPage prevpage = previous();
    while (prevpage.isValid()) {
        KWPageStyle prevpagestyle = prevpage.pageStyle();
        if (prevpagestyle.isValid()) {
            if (!prevpagestyle.nextStyleName().isEmpty())
                return prevpagestyle.nextStyleName();
            if (!prevpagestyle.name().isEmpty())
                return prevpagestyle.name();
        }
    }
    return QString();
}

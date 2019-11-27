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
#ifndef KWPAGE_H
#define KWPAGE_H

#include "KWPageManager.h"
#include "KWPageStyle.h"
#include "words_export.h"

#include <KoText.h>
#include <KoTextPage.h>
#include <QRectF>
#include <QObject>

class KWPageManagerPrivate;
class KoShapeManager;

/**
 * This class represents a printed page of the document.  Each page is either left, right or
 * a pageSpread. See the PageSide for details.
 * The KWPage is created and maintained by the KWPageManager so you won't find a constructor
 * on this class.
 * Each KWPage is attached to a KWPageStyle representing the page master.
 */
class WORDS_EXPORT KWPage : public KoTextPage
{
public:
    inline KWPage() : priv(0), n(0) {}
    inline KWPage(KWPageManagerPrivate *manager, int index) : priv(manager), n(index) {}
    inline KWPage(const KWPage &o) : priv(o.priv), n(o.n) {}
    inline KWPage &operator=(const KWPage &o) { priv = o.priv; n = o.n; return *this; }

    /// An enum to define if this is a page that is printed to be a left or a right page
    enum PageSide {
        Left,       ///< A left page. Used for even-numbered pages
        Right       ///< A right page. Used for odd numbered pages
    };

    /// return the width of this page (in pt)
    qreal width() const;
    /// return the height of this page (in pt)
    qreal height() const;
    /// return the height of the margin at top (in pt);
    qreal topMargin() const;
    /// return the height of the margin at bottom (in pt);
    qreal bottomMargin() const;
    /// return the width of the margin at left (in pt);
    qreal leftMargin() const;
    /// return the width of the margin at right (in pt);
    qreal rightMargin() const;
    /// return the height of the padding at top (in pt);
    qreal topPadding() const;
    /// return the height of the padding at bottom (in pt);
    qreal bottomPadding() const;
    /// return the width of the padding at left (in pt);
    qreal leftPadding() const;
    /// return the width of the padding at right (in pt);
    qreal rightPadding() const;
    /// return width of the margin that is the closest to the page edge, i.e. left for even pages
    qreal pageEdgeMargin() const;
    /// return width of the margin that is the closest to the binding, i.e. right for even pages
    qreal marginClosestBinding() const;

    /**
     * return a rectangle outlining this page, using the offset in the document.
     */
    QRectF rect() const override;

    /**
     * return a rectangle outlining the main text on this page, using the offset in the document.
     */
    QRectF contentRect() const override;

    /* set the content rect valid on this page
     */
    void setContentRect(const QRectF &rect);

    // the y coordinate
    /**
     * All Pages are in a document-wide coordinate system that simply puts every page under
     * the next page and keeps the Y coordinate counting throughout the doc.  The X coordinate
     * off each page in the document is zero, i.e. aligned to the left.
     * This method returns the offset of this page in the whole document which means it will
     * add the height of all the pages that come before this one.
     */
    qreal offsetInDocument() const;
    /**
     * Set the offset of the page.
     */
    void setOffsetInDocument(qreal offset);

    /// Return the pageSide of this page, see the PageSide
    PageSide pageSide() const;
    /// set the pageSide of this page, see the PageSide
    void setPageSide(PageSide ps);

    /// reimplemented from KoTextPage
    int visiblePageNumber(PageSelection select = CurrentPage, int adjustment = 0) const override;

    /// reimplemented from KoTextPage
    int pageNumber() const override;

    /**
     * Adjusts the visible page number of this page.
     *
     * This implements hard-coded page numbers like those defined via @a KoParagraphStyle::PageNumber . If
     * the page number equals zero then the page has an auto page number. That means previous page number
     * plus one.
     *
     * @param pageNumber The visible page-number for this page.
     */
    void setVisiblePageNumber(int pageNumber);

    /// reimplemented from KoTextPage
    QString masterPageName() const override;

    /// returns the page style applied on this page
    KWPageStyle pageStyle() const;
    /// set the page style to apply on this page
    void setPageStyle(const KWPageStyle style);

    /**
     * Return the orientation property of the page.
     * Note that the orientation has no influence on the actual page or its size in any way. Its
     * just a property. There are dialogs that will be better at showing page sizes and at offering features
     * when this property is set properly, though.
     */
    KoPageFormat::Orientation orientationHint() const;
    void setOrientationHint(KoPageFormat::Orientation);

    /**
     * Set the text-direction that text on this page should be initialized with.
     * New text that is written on this page can use this to set the text direction for
     * the paragraph.
     * @param direction the direction.
     */
    void setDirectionHint(KoText::Direction direction);

    /**
     * return the text-direction that text on this page should be initialized with.
     * New text that is written on this page can use this to set the text direction for
     * the paragraph.
     */
    KoText::Direction directionHint() const;

    void setAutoGenerated(bool on);
    bool isAutoGenerated() const;

    /// returns the previous page.
    const KWPage previous() const;
    /// returns the next page.
    const KWPage next() const;

    bool isValid() const;

    /**
     * Create a thumbnail image for this page.
     * Note: if the page has not been laid out yet, the result may be empty!
     *
     * @param size: the size in pixels of the thumbnail
     * @param shapeManager the shape manager containing the page's shapes
     * @param asPrint set to @c true if the rendering should be done like for print,
     *                or @c false if with the current view settings
     * @returns the thumbnail
     */
    QImage thumbnail(const QSize &size, KoShapeManager *shapeManager, bool asPrint = false) const;

    bool operator==(const KWPage &other) const;
    inline bool operator!=(const KWPage &other) const { return ! operator==(other); }
    inline bool operator<(const KWPage &other) const { return n < other.n; }
    inline bool operator>(const KWPage &other) const { return n > other.n; }
    uint hash() const;

private:
    friend class KWPageTextInfo;
    KWPageManagerPrivate *priv;
    int n;
};

Q_DECLARE_TYPEINFO(KWPage, Q_MOVABLE_TYPE);

inline uint qHash(const KWPage &page)
{
    return page.hash();
}

#endif

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
#ifndef kw_page_h
#define kw_page_h

#include <KoRect.h>
#include "KWPageManager.h"
#include "KoPageLayout.h"

class KoZoomHandler;

/**
 * This class represents a printed page of the document.  Each page is either left, right or
 * a pageSpread. See the PageSideEnum for details.
 * The KWPage is created and maintained by the KWPageManager so you won't find a constructor
 * on this class.  Margins and sizes are inherited from the page managers default, setting the
 * size to the same size as the default will from then on update the size automatically.
 */
class KWPage {
public:
    /// An enum to define if this is a page that is printed to be a left or a right page
    enum PageSideEnum {
        Left,       ///< A left page. Used for even-numbered pages
        Right,      ///< A right page. Used for odd numbered pages
        PageSpread  ///< A page spread which is one KWPage instance, but represents 2 pagenumbers
    };

    /// set the width of the page in pt
    void setWidth(const double &x);
    /// set the height of the page in pt
    void setHeight(const double &y);
    /// set height of the top margin in pt
    void setTopMargin(const double &x);
    /// set height of the bottom margin in pt
    void setBottomMargin(const double &y);

    /**
     * set width of the margin that is the closest to the page edge, i.e. left for even pages.
     * Any page should either use valid left and right margins, or valid pageedge/binding ones,
     * setting this one will remove the left/right ones.
     */
    void setPageEdgeMargin(const double &x);
    /**
     * set width of the margin that is the closest to the binding, i.e. right for even pages.
     * Any page should either use valid left and right margins, or valid pageedge/binding ones,
     * setting this one will remove the left/right ones.
     */
    void setMarginClosestBinding(const double &x);
    /**
     * set width of the left margin.
     * Any page should either use valid left and right margins, or valid pageedge/binding ones,
     * setting this one will remove the pageedge/closest-binding ones.
     */
    void setLeftMargin(const double &l);
    /**
     * set width of the right margin.
     * Any page should either use valid left and right margins, or valid pageedge/binding ones,
     * setting this one will remove the pageedge/closest-binding ones.
     */
    void setRightMargin(const double &r);

    /// return the width of this page (in pt)
    double width() const;
    /// return the height of this page (in pt)
    double height() const;
    /// return the height of the margin at top (in pt);
    double topMargin() const;
    /// return the height of the margin at bottom (in pt);
    double bottomMargin() const;
    /// return the width of the margin at left (in pt);
    double leftMargin() const;
    /// return the width of the margin at left (in pt);
    double rightMargin() const;
    /// return width of the margin that is the closest to the page edge, i.e. left for even pages
    double pageEdgeMargin() const;
    /// return width of the margin that is the closest to the binding, i.e. right for even pages
    double marginClosestBinding() const;

    /**
     * return a rectangle outlining this page, using the offset in the document.
     * For page-spreads the page size will effectively be 2 pages unless the
     * pageNumber param is specified and the pagenumber of either the left or the
     * right page is passed.
     * @param pageNumber passing a pagenumber will return the rect of either the
     *  left or the right halve of a pageSpread.
     * Passing a pagenumber that is not represented by this object will have
     *  unpredictabe results.
     */
    QRectF rect(int pageNumber=-1) const;

    // the y coordinate
    /**
     * All Pages are in a document-wide coordinate system that simply puts every page under
     * the next page and keeps the Y coordinate counting throughout the doc.  The X coordinate
     * off each page in the document is zero, i.e. aligned to the left.
     * This method returns the offset of this page in the whole document which means it will
     * add the height of all the pages that come before this one.
     */
    double offsetInDocument() const;

    /// Return the pageSide of this page, see the PageSideEnum
    PageSideEnum pageSide() const { return m_pageSide; }
    /// set the pageSide of this page, see the PageSideEnum
    void setPageSide(PageSideEnum ps) { m_pageSide = ps; }

    /// returns the number of this page as it will be shown to the user.
    int pageNumber() const { return m_pageNum; }

private:
    /** private constructor, only for our friends
     * @param parent the KWPageManager that we belong to.
     * @param pageNum the number of the page as the user will see it.
     */
    KWPage(KWPageManager *parent, int pageNum);

    int m_pageNum;
    PageSideEnum m_pageSide;
    KoPageLayout m_pageLayout;

    KWPageManager *m_parent;

friend class KWPageManager;
};

#endif

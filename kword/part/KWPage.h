/* This file is part of the KOffice project
 * Copyright (C) 2005, 2007 Thomas Zander <zander@kde.org>
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
#ifndef kw_page_h
#define kw_page_h

#include "KWPageManager.h"
#include "KWPageSettings.h"
#include "KoPageLayout.h"
#include "kword_export.h"

#include <KoText.h>
#include <QRectF>
class KoZoomHandler;

/**
 * This class represents a printed page of the document.  Each page is either left, right or
 * a pageSpread. See the PageSide for details.
 * The KWPage is created and maintained by the KWPageManager so you won't find a constructor
 * on this class.  
 * Each KWPage is attached to a KWPageSettings representing the page master.
 */
class KWORD_EXPORT KWPage {
public:
    /// An enum to define if this is a page that is printed to be a left or a right page
    enum PageSide {
        Left,       ///< A left page. Used for even-numbered pages
        Right,      ///< A right page. Used for odd numbered pages
        PageSpread  ///< A page spread which is one KWPage instance, but represents 2 pagenumbers
    };

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
    /// return the width of the margin at right (in pt);
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

    /// Return the pageSide of this page, see the PageSide
    PageSide pageSide() const { return m_pageSide; }
    /// set the pageSide of this page, see the PageSide
    void setPageSide(PageSide ps) { m_pageSide = ps; }

    /// returns the number of this page as it will be shown to the user.
    int pageNumber() const { return m_pageNum; }

    /// returns the page settings applied on this page
    KWPageSettings *pageSettings() { return m_pageSettings; }
    /// set the page settings to apply on this page
    void setPageSettings (KWPageSettings *settings) { m_pageSettings = settings; }

    /**
     * Return the orientation property of the page.
     * Note that the orientation has no influence on the actual page or its size in any way. Its
     * just a property. There are dialogs that will be better at showing page sizes and at offering features
     * when this property is set properly, though.
     */
    KoPageFormat::Orientation orientationHint() const;

    /**
     * Set the text-direction that text on this page should be initialized with.
     * New text that is written on this page can use this to set the text direction for
     * the paragraph.
     * @param direction the direction.
     */
    void setDirectionHint(KoText::Direction direction) { m_textDirectionHint = direction; }

    /**
     * return the text-direction that text on this page should be initialized with.
     * New text that is written on this page can use this to set the text direction for
     * the paragraph.
     */
    KoText::Direction directionHint() const { return m_textDirectionHint; }

    KWPage *next();
    KWPage *previous();

private:
    /** private constructor, only for our friends
     * @param parent the KWPageManager that we belong to.
     * @param pageNum the number of the page as the user will see it.
     * @param pageSettings the page settings to use for the page
     */
    KWPage(KWPageManager *parent, int pageNum, KWPageSettings *pageSettings);

    int m_pageNum;
    PageSide m_pageSide;
    KoText::Direction m_textDirectionHint;

    KWPageManager *m_parent;
    KWPageSettings *m_pageSettings;

    friend class KWPageManager;
};

#endif

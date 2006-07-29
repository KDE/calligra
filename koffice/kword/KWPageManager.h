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
#ifndef kw_pagemanager_h
#define kw_pagemanager_h

#include "KoPageLayout.h"

#include <qptrlist.h>

class KWPage;
class KoPoint;
class KoRect;

/**
 * The Page Manager manages all the pages a document contains and separates all the frames
 * the document contains into actual printed pages.
 */
class KWPageManager {
public:
    KWPageManager();

    /// return pageNumber of @p frame, pagenumbers for a normal document start at 1.
    int pageNumber(const KoRect *rect) const;
    /// return pageNumber of @p rect, pagenumbers for a normal document start at 1.
    int pageNumber(const KoRect &rect) const;
    /// return pageNumber of @p point, pagenumbers for a normal document start at 1.
    int pageNumber(const KoPoint &point) const;
    /** return pageNumber of page with document-offset (in the Y direction) of @p ptY,
     *  pagenumbers for a normal document start at 1.
     */
    int pageNumber(double ptY) const;
    /// return total number of pages in this document.
    int pageCount() const;
    /// return the highest page number we have in this document.
    int lastPageNumber() const;
    /// return the KWPage of a specific page number. Returns 0 if page does not exist.
    KWPage* page(int pageNumber) const;
    /// return the KWPage instance where the rect is on. Returns 0 if page does not exist.
    KWPage* page(const KoRect &rect) const;
    /// return the KWPage instance where the rect or frame is on. Returns 0 if page does not exist.
    KWPage* page(const KoRect *frame) const;
    /// return the KWPage instance where the point is on. Returns 0 if page does not exist.
    KWPage* page(const KoPoint &point) const;
    /// return the KWPage instance of the y-coordinate in the document. Returns 0 if page does not exist.
    KWPage* page(double ptY) const;

    /**
     * Return the y-offset in this document of the top of page with @p pageNumber
     * Note that pageNumber is NOT an offset in the document, but the real number
     * of the page.
     * @see setStartPage(int)
     */
    double topOfPage(int pageNumber) const; // in pt
    /**
     * Return the y-offset in this document of the bottom of page with @p pageNumber
     * Note that pageNumber is NOT an offset in the document, but the real number
     * of the page.
     * @see setStartPage(int)
     */
    double bottomOfPage(int pageNumber) const; // in pt

    /// Set a new startpage for this document, renumbering all pages already added.
    void setStartPage(int startPage);

    /// return the first pagenumber of this document
    int startPage() const { return m_firstPage; }

    /**
     * Register if new pages can only be appended after the last one and not
     * somewhere in between.
     * @param appendOnly the new value
     */
    void setOnlyAllowAppend(bool appendOnly) { m_onlyAllowAppend = appendOnly; }
    /**
     * return if new pages can only be appended after the last one and not
     * somewhere in between.
     */
    bool onlyAllowAppend() { return m_onlyAllowAppend; }

    /**
     * Inserts a new page at the specified position in the document.
     * Shifts the page currently at that position (if any) and any subsequent pages after.
     * if onlyAllowAppend is set to true the pagenumber will be ignored and the new page
     * will always be appended.
     * @param pageNumber page number of the new page
     */
    KWPage* insertPage(int pageNumber);
    /// Append a new page at the end of the document
    KWPage* appendPage();

    /// Remove the page with @p pageNumber renumbering all pages after pages already added
    void removePage(int pageNumber);
    /// Remove @p page renumbering all pages after pages already added
    void removePage(KWPage *page);

    /// return the effective pageLayout of @p pageNumber combining the default and the page specific ones
    const KoPageLayout pageLayout(int pageNumber) const;

    void setDefaultPage(const KoPageLayout &layout);

    /**
     * Returns the argument point, with altered coordinats if the point happens to be
     * outside all pages.
     * The resulting point is the shortest distance from the argument inside the document.
     * @param point the point to test
     */
    KoPoint clipToDocument(const KoPoint &point);

private:
    /// helper method for the topOfPage and bottomOfPage
    double pageOffset(int pageNumber, bool bottom) const;
    /// sorter for numbered pages.
    class PageList : public QPtrList<KWPage> {
    protected:
        virtual int compareItems(QPtrCollection::Item a, QPtrCollection::Item b);
    };

    PageList m_pageList;
    int m_firstPage;
    bool m_onlyAllowAppend; // true for WP style documents.

    KoPageLayout m_defaultPageLayout;

friend class KWPage;
};

#endif

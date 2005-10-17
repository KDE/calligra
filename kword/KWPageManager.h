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

#include "koPageLayout.h"

#include <qptrlist.h>

class KWFrame;
class KWPage;
class KoPoint;
class KoRect;

/**
 * The Page Manger manages all the pages a document contains and separates the all the frames
 *  the document contains into actual printed pages.
 */
class KWPageManager {
public:
    KWPageManager();

    /// return pageNumber of @p frame, pagenumbers for a normal document start at 1.
    int pageNumber(const KoRect *frame) const;
    /// return pageNumber of @p point, pagenumbers for a normal document start at 1.
    int pageNumber(const KoPoint *point) const;
    int pageNumber(double ptY) const;
    /// return total number of pages in this document.
    int pageCount() const;
    /// return the highest page number we have in this document.
    int lastPageNumber() const;
    /// return the KWPage of a specific page number
    KWPage* page(int pageNumber) const;
    /// return the KWPage instance where the frame is on.
    KWPage* page(const KoRect *frame) const;
    /// return the KWPage instance where the point is on.
    KWPage* page(const KoPoint *point) const;
    /// return the KWPage instance of the y-coordinate in the document.
    KWPage* page(double ptY) const;

    double topOfPage(int pageNumber) const; // in pt
    double bottomOfPage(int pageNumber) const; // in pt

    /// Set a new startpage for this document, renumbering all pages.
    void setStartPage(int startPage);

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

    void removePage(int pageNumber);
    void removePage(KWPage *page);

    const KoPageLayout pageLayout(int pageNumber) const;

    void setDefaultPageSize(double width, double height);
    void setDefaultPageMargins(double top, double left, double bottom, double right);

private:
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

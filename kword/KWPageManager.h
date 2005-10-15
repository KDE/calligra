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

#include <qptrlist.h>
class KWFrame;
class KWPage;

/**
 * The Page Manger manages all the pages a document contains and separates the all the frames
 *  the document contains into actual printed pages.
 */
class KWPageManager {
public:
    KWPageManager();

    /// return pageNumber of @p frame, pagenumbers for a normal document start at 1.
    int pageNumber(const KWFrame *frame) const;
    /// return total number of pages in this document.
    int numPages() const;
    /// return the highest page number we have in this document.
    int lastPageNumber() const;
    /// return the KWPage representation of a page number.
    KWPage* pageAt(int pageNum) const;

    /// Set a new startpage for this document, renumbering all pages.
    void setStartPage(int startPage);

    KWPage* createPage();

private:
    /// sorter for numbered pages.
    class PageList : public QPtrList<KWPage> {
    protected:
        virtual int compareItems(QPtrCollection::Item a, QPtrCollection::Item b);
    };

    PageList m_pageList;
    int m_firstPage;
};

#endif


/* This file is part of the KOffice project
 * Copyright (C) 2008 Thomas Zander <zander@kde.org>
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
#ifndef KWPageManagerPrivate_H
#define KWPageManagerPrivate_H

#include "KWPageStyle.h"
#include "KWPage.h"

#include <QHash>
#include <QMap>

class KWPageManagerPrivate
{
public:
    struct Page
    {
        Page()
            : pageSide(KWPage::Right),
            pageNumber(1),
            orientation(KoPageFormat::Portrait),
            textDirection(KoText::InheritDirection)
        {
        }
        KWPageStyle style;
        KWPage::PageSide pageSide;
        int pageNumber; // set by the append-page and overwritten by the text-layout
        KoPageFormat::Orientation orientation;
        KoText::Direction textDirection;
    };

    KWPageManagerPrivate();

    qreal pageOffset(int pageNum, bool bottom) const;

    /**
     * Update the page number for the page related to the pageId and also update the
     * page number of all pages following the page.
     */
    void setPageNumberForId(int pageId, int newPageNumber);

    /// helper method for the commands.
    void insertPage(const Page &page);


    // use a sorted map to find page the identifier for page objects based on the page number.
    QMap<int, int> pageNumbers; // page number to pageId

    // use a fast access hash to store the page objects, sorted by their identifier
    QHash<int, Page> pages; // pageId to page struct

    int lastId; // pageIds are distributed serially,

    bool preferPageSpread;

    QHash <QString, KWPageStyle> pageStyles;
    KoInsets padding;
    KWPageStyle defaultPageStyle;
};

#endif

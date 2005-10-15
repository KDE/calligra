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

#include <koRect.h>
#include "KWPageManager.h"

/**
 * This class represents a printed page of the document.
 */
class KWPage {
public:
    /// An enum to define if this is a page that is printed to be a left or a right page
    enum PageSideEnum {Left, Right}; //future:, PageSpread };

    /// return the size of this page in pt, x and y are always 0
    const KoRect size() const { return m_size; }
    /// set the width of the page in pt
    void setWidth(const double &x) { m_size.setRight(x); }
    /// set the height of the page in pt
    void setHeight(const double &y) { m_size.setBottom(y); }

    /// Return the pageSide of this page, see the PageSideEnum
    PageSideEnum pageSide() const { return m_pageSide; }
    /// set the pageSide of this page, see the PageSideEnum
    void setPageSide(PageSideEnum ps) { m_pageSide = ps; }

    int pageNum() const { return m_pageNum; }

private:
    // private constructor, only for our friends
    KWPage(int pageNum);
    int m_pageNum; ///< first page is 1
    KoRect m_size;
    PageSideEnum m_pageSide;

friend class KWPageManager;
};

#endif

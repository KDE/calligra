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
#include "koPageLayout.h"

class KoZoomHandler;

/**
 * This class represents a printed page of the document.
 */
class KWPage {
public:
    /// An enum to define if this is a page that is printed to be a left or a right page
    enum PageSideEnum {Left, Right}; //future:, PageSpread };

    /// set the width of the page in pt
    void setWidth(const double &x);
    /// set the height of the page in pt
    void setHeight(const double &y);
    void setTopMargin(const double &x);
    void setBottomMargin(const double &y);
    void setLeftMargin(const double &x);
    void setRightMargin(const double &y);

    double width() const;
    double height() const;
    double topMargin() const;
    double bottomMargin() const;
    double leftMargin() const;
    double rightMargin() const;

    // the y coordinate
    double offsetInDocument() const;

    /// Return the pageSide of this page, see the PageSideEnum
    PageSideEnum pageSide() const { return m_pageSide; }
    /// set the pageSide of this page, see the PageSideEnum
    void setPageSide(PageSideEnum ps) { m_pageSide = ps; }

    int pageNumber() const { return m_pageNum; }

    QRect zoomedRect(KoZoomHandler *zoomHandler);
    const KoRect rect() const;

private:
    // private constructor, only for our friends
    KWPage(KWPageManager *parent, int pageNum);
    int m_pageNum;
    PageSideEnum m_pageSide;
    KoPageLayout m_pageLayout;

    KWPageManager *m_parent;

friend class KWPageManager;
};

#endif

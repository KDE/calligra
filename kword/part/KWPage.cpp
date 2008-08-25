/* This file is part of the KOffice project
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
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

#include <KoZoomHandler.h>
#include <KoInlineObject.h>

#include <QRect>

KWPage::KWPage(KWPageManager *parent, int pageNum, KWPageStyle *pageStyle)
{
    m_parent = parent;
    m_pageNum = pageNum;
    m_pageStyle = pageStyle;
    m_pageSide = pageNum%2==0 ? Left : Right;

    m_textDirectionHint = KoText::AutoDirection;
}

void KWPage::setPageNumber(int pageNum) {
    if(m_pageNum == pageNum)
        return;

    m_pageNum = pageNum;

    //TODO mark KoVariable page-number's as dirty
}

int KWPage::pageNumber() const {
    return m_pageNum;
}

int KWPage::pageNumber(KoInlineObject* inlineObject) {
    Q_ASSERT(inlineObject);

    //The text:select-page attribute varies according to the element where it appears as follows:
    //  <text:page-continuation> - specifies whether to check for a previous or next page and if the page exists, the continuation text is printed.
    //  <text:page-number> - specifies whether to display or not the number of the previous or the following page rather than the number of the current page.
    //Note: To display the current page number on all pages except the first or last page, use a combination of the text:select-page and text:page-adjust attributes.
    //Note: Obviously these two uses of text:select-page are very close but it still seems to me to be different. The first usage triggers the use of another attribute. 
    const QString selectpage = inlineObject->attribute("select-page", QString()).toString();

    //text:page-adjust
    //The text:page-adjust attribute specifies an adjustment of the value of a page number field , allowing the display of page numbers of following or preceding pages. When this attribute is used, the application:
    //  1. Adds the value of the attribute to the current page number.
    //  2. Checks to see if the resulting page exists.
    //  3. If the page exists, the number of that page is displayed.
    //  4. If the page does not exist, the value of the page number field remains empty and no number is displayed.
    //The text:page-adjus
    const int pageadjust = inlineObject->attribute("page-adjust", 0).toInt();

    int pagenum = -1;
    if (selectpage == "current" || selectpage.isEmpty())
        pagenum = m_pageNum;
    else if (selectpage == "previous")
        pagenum = previous() ? previous()->m_pageNum : -1;
    else if (selectpage == "next")
        pagenum = next() ? next()->m_pageNum : -1;

    if(pagenum >= 0) {
        pagenum += pageadjust;
        if( ! m_parent->page(pagenum) )
            pagenum = -1;
    }

    return pagenum; //"current"
}

qreal KWPage::width() const
{
    return m_pageStyle->pageLayout().width * (m_pageSide==PageSpread ? 2:1);
}

qreal KWPage::height() const
{
    return m_pageStyle->pageLayout().height;
}

qreal KWPage::topMargin() const
{
    return m_pageStyle->pageLayout().top;
}
qreal KWPage::bottomMargin() const
{
    return m_pageStyle->pageLayout().bottom;
}

qreal KWPage::leftMargin() const
{
    qreal answer = m_pageSide == Left ? pageEdgeMargin() : marginClosestBinding();
    if(answer != -1)
        return answer;
    return m_pageStyle->pageLayout().left;
}

qreal KWPage::rightMargin() const
{
    qreal answer = m_pageSide == Right ? pageEdgeMargin() : marginClosestBinding();
    if(answer != -1)
        return answer;
    return m_pageStyle->pageLayout().right;
}
qreal KWPage::pageEdgeMargin() const
{
    return m_pageStyle->pageLayout().pageEdge;
}
qreal KWPage::marginClosestBinding() const
{
    return m_pageStyle->pageLayout().bindingSide;
}

qreal KWPage::offsetInDocument() const
{ // the y coordinate
    return m_parent->topOfPage(m_pageNum);
}

QRectF KWPage::rect(int pageNumber) const
{
    if(pageNumber == m_pageNum && m_pageSide == PageSpread) // left
        return QRectF(0, offsetInDocument(), width()/2, height());
    if(pageNumber == m_pageNum+1 && m_pageSide == PageSpread) // right
        return QRectF(width()/2, offsetInDocument(), width()/2, height());
    return QRectF(0, offsetInDocument(), width(), height());
}

KoPageFormat::Orientation KWPage::orientationHint() const
{
    return m_pageStyle->pageLayout().orientation;
}

KWPage *KWPage::next()
{
    return m_parent->page(m_pageNum +1); //(m_pageNum + m_pageSide == PageSpread ? 2 : 1);
}

KWPage *KWPage::previous()
{
    return m_parent->page(m_pageNum -1);
}

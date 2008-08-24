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
#include "KoZoomHandler.h"

#include <QRect>

KWPage::KWPage(KWPageManager *parent, int pageNum, KWPageStyle *pageStyle)
{
    m_parent = parent;
    m_pageNum = pageNum;
    m_pageStyle = pageStyle;
    m_pageSide = pageNum%2==0 ? Left : Right;

    m_textDirectionHint = KoText::AutoDirection;
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
    return m_parent->page(m_pageNum + m_pageSide == PageSpread ? 2 : 1);
}

KWPage *KWPage::previous()
{
    return m_parent->page(m_pageNum -1);
}

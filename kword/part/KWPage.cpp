/* This file is part of the KOffice project
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
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

KWPage::KWPage(KWPageManager *parent, int pageNum) {
    m_parent = parent;
    m_pageNum = pageNum;
    m_pageLayout.width = -1.0;
    m_pageLayout.height = -1.0;
    m_pageLayout.left = -1.0;
    m_pageLayout.right = -1.0;
    m_pageLayout.bottom = -1.0;
    m_pageLayout.top = -1.0;
    m_pageLayout.pageEdge = -1.0;
    m_pageLayout.bindingSide = -1.0;
    m_pageSide = pageNum%2==0 ? Left : Right;
}

double KWPage::width() const {
    if(m_pageLayout.width != -1)
        return m_pageLayout.width;
    return m_parent->m_defaultPageLayout.width * (m_pageSide==PageSpread?2:1);
}

double KWPage::height() const {
    if(m_pageLayout.height != -1)
        return m_pageLayout.height;
    return m_parent->m_defaultPageLayout.height;
}

void KWPage::setWidth(const double &x) {
    m_pageLayout.width = x == m_parent->m_defaultPageLayout.width ? -1 : x;
}
void KWPage::setHeight(const double &y) {
    m_pageLayout.height = y == m_parent->m_defaultPageLayout.height ? -1 : y;
}
void KWPage::setTopMargin(const double &t) {
    m_pageLayout.top = t == m_parent->m_defaultPageLayout.top ? -1 : t;
}
void KWPage::setBottomMargin(const double &b) {
    m_pageLayout.bottom = b == m_parent->m_defaultPageLayout.bottom ? -1 : b;
}
void KWPage::setPageEdgeMargin(const double &m) {
    m_pageLayout.pageEdge = m == m_parent->m_defaultPageLayout.pageEdge ? -1 : m;
    m_pageLayout.left = -1;
    m_pageLayout.right = -1;
}
void KWPage::setMarginClosestBinding(const double &m) {
    m_pageLayout.bindingSide = m == m_parent->m_defaultPageLayout.bindingSide ? -1 : m;
    m_pageLayout.left = -1;
    m_pageLayout.right = -1;
}
void KWPage::setLeftMargin(const double &l) {
    m_pageLayout.left = l == m_parent->m_defaultPageLayout.left ? -1 : l;
    m_pageLayout.bindingSide = -1;
    m_pageLayout.pageEdge = -1;
    if(rightMargin() == -1)
        m_pageLayout.right = 0; // never leave this object in an illegal state
}
void KWPage::setRightMargin(const double &r) {
    m_pageLayout.right = r == m_parent->m_defaultPageLayout.right ? -1 : r;
    m_pageLayout.bindingSide = -1;
    m_pageLayout.pageEdge = -1;
    if(leftMargin() == -1)
        m_pageLayout.left = 0; // never leave this object in an illegal state
}

double KWPage::topMargin() const {
    if(m_pageLayout.top != -1)
        return m_pageLayout.top;
    return m_parent->m_defaultPageLayout.top;
}
double KWPage::bottomMargin() const {
    if(m_pageLayout.bottom != -1)
        return m_pageLayout.bottom;
    return m_parent->m_defaultPageLayout.bottom;
}
double KWPage::leftMargin() const {
    // first try local left.
    if(m_pageLayout.left != -1)
        return m_pageLayout.left;

    // then see if the margin is in use.
    double answer = m_pageSide == Right ? marginClosestBinding() : pageEdgeMargin();
    if(answer != -1)
        return answer;
    return m_parent->m_defaultPageLayout.left;
}
double KWPage::rightMargin() const {
    if(m_pageLayout.right != -1)
        return m_pageLayout.right;

    double answer = m_pageSide == Right ? pageEdgeMargin() : marginClosestBinding();
    if(answer != -1)
        return answer;
    return m_parent->m_defaultPageLayout.right;
}
double KWPage::pageEdgeMargin() const {
    if(m_pageLayout.pageEdge != -1)
        return m_pageLayout.pageEdge;
    return m_parent->m_defaultPageLayout.pageEdge;
}
double KWPage::marginClosestBinding() const {
    if(m_pageLayout.bindingSide != -1)
        return m_pageLayout.bindingSide;
    return m_parent->m_defaultPageLayout.bindingSide;
}

double KWPage::offsetInDocument() const { // the y coordinate
    return m_parent->topOfPage(m_pageNum);
}

QRectF KWPage::rect(int pageNumber) const {
    if(pageNumber == m_pageNum && m_pageSide == PageSpread) // left
        return QRectF(0, offsetInDocument(), width()/2, height());
    if(pageNumber == m_pageNum+1 && m_pageSide == PageSpread) // right
        return QRectF(width()/2, offsetInDocument(), width()/2, height());
    return QRectF(0, offsetInDocument(), width(), height());
}

const KoPageLayout KWPage::pageLayout() const {
    return m_parent->pageLayout(pageNumber());
}

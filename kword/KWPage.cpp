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

#include "KWPage.h"
#include "KoZoomHandler.h"

#include <QRect>

KWPage::KWPage(KWPageManager *parent, int pageNum) {
    m_parent = parent;
    m_pageNum = pageNum;
    m_pageLayout.ptWidth = -1.0;
    m_pageLayout.ptHeight = -1.0;
    m_pageLayout.ptLeft = -1.0;
    m_pageLayout.ptRight = -1.0;
    m_pageLayout.ptBottom = -1.0;
    m_pageLayout.ptTop = -1.0;
    m_pageLayout.ptPageEdge = -1.0;
    m_pageLayout.ptBindingSide = -1.0;
    m_pageSide = pageNum%2==0 ? Left : Right;
}

double KWPage::width() const {
    if(m_pageLayout.ptWidth != -1)
        return m_pageLayout.ptWidth;
    return m_parent->m_defaultPageLayout.ptWidth;
}

double KWPage::height() const {
    if(m_pageLayout.ptHeight != -1)
        return m_pageLayout.ptHeight;
    return m_parent->m_defaultPageLayout.ptHeight;
}

void KWPage::setWidth(const double &x) {
    m_pageLayout.ptWidth = x == m_parent->m_defaultPageLayout.ptWidth ? -1 : x;
}
void KWPage::setHeight(const double &y) {
    m_pageLayout.ptHeight = y == m_parent->m_defaultPageLayout.ptHeight ? -1 : y;
}
void KWPage::setTopMargin(const double &t) {
    m_pageLayout.ptTop = t == m_parent->m_defaultPageLayout.ptTop ? -1 : t;
}
void KWPage::setBottomMargin(const double &b) {
    m_pageLayout.ptBottom = b == m_parent->m_defaultPageLayout.ptBottom ? -1 : b;
}
void KWPage::setPageEdgeMargin(const double &m) {
    m_pageLayout.ptPageEdge = m == m_parent->m_defaultPageLayout.ptPageEdge ? -1 : m;
    m_pageLayout.ptLeft = -1;
    m_pageLayout.ptRight = -1;
}
void KWPage::setMarginClosestBinding(const double &m) {
    m_pageLayout.ptBindingSide = m == m_parent->m_defaultPageLayout.ptBindingSide ? -1 : m;
    m_pageLayout.ptLeft = -1;
    m_pageLayout.ptRight = -1;
}
void KWPage::setLeftMargin(const double &l) {
    m_pageLayout.ptLeft = l == m_parent->m_defaultPageLayout.ptLeft ? -1 : l;
    m_pageLayout.ptBindingSide = -1;
    m_pageLayout.ptPageEdge = -1;
    if(rightMargin() == -1)
        m_pageLayout.ptRight = 0; // never leave this object in an illegal state
}
void KWPage::setRightMargin(const double &r) {
    m_pageLayout.ptRight = r == m_parent->m_defaultPageLayout.ptRight ? -1 : r;
    m_pageLayout.ptBindingSide = -1;
    m_pageLayout.ptPageEdge = -1;
    if(leftMargin() == -1)
        m_pageLayout.ptLeft = 0; // never leave this object in an illegal state
}

double KWPage::topMargin() const {
    if(m_pageLayout.ptTop != -1)
        return m_pageLayout.ptTop;
    return m_parent->m_defaultPageLayout.ptTop;
}
double KWPage::bottomMargin() const {
    if(m_pageLayout.ptBottom != -1)
        return m_pageLayout.ptBottom;
    return m_parent->m_defaultPageLayout.ptBottom;
}
double KWPage::leftMargin() const {
    // first try local left.
    if(m_pageLayout.ptLeft != -1)
        return m_pageLayout.ptLeft;

    // then see if the margin is in use.
    double answer = m_pageSide == Right ? marginClosestBinding() : pageEdgeMargin();
    if(answer != -1)
        return answer;
    return m_parent->m_defaultPageLayout.ptLeft;
}
double KWPage::rightMargin() const {
    if(m_pageLayout.ptRight != -1)
        return m_pageLayout.ptRight;

    double answer = m_pageSide == Left ? marginClosestBinding() : pageEdgeMargin();
    if(answer != -1)
        return answer;
    return m_parent->m_defaultPageLayout.ptRight;
}
double KWPage::pageEdgeMargin() const {
    if(m_pageLayout.ptPageEdge != -1)
        return m_pageLayout.ptPageEdge;
    return m_parent->m_defaultPageLayout.ptPageEdge;
}
double KWPage::marginClosestBinding() const {
    if(m_pageLayout.ptBindingSide != -1)
        return m_pageLayout.ptBindingSide;
    return m_parent->m_defaultPageLayout.ptBindingSide;
}

double KWPage::offsetInDocument() const { // the y coordinate
    return m_parent->topOfPage(m_pageNum);
}

QRect KWPage::zoomedRect(KoZoomHandler *zoomHandler) {
    return QRect(0, zoomHandler->zoomItYOld(offsetInDocument()),
                 zoomHandler->zoomItXOld(width()), zoomHandler->zoomItYOld(height()));
}

const KoRect KWPage::rectOld() const {
    return KoRect(0, offsetInDocument(), width(), height());
}

const QRectF KWPage::rect() const {
    return QRectF(0, offsetInDocument(), width(), height());
}

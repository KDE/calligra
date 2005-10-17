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
#include "kozoomhandler.h"

#include <qrect.h>

KWPage::KWPage(KWPageManager *parent, int pageNum) {
    m_parent = parent;
    m_pageNum = pageNum;
    m_pageLayout.ptWidth = -1.0;
    m_pageLayout.ptHeight = -1.0;
    m_pageLayout.ptLeft = -1.0;
    m_pageLayout.ptRight = -1.0;
    m_pageLayout.ptBottom = -1.0;
    m_pageLayout.ptTop = -1.0;
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
void KWPage::setLeftMargin(const double &l) {
    m_pageLayout.ptLeft = l == m_parent->m_defaultPageLayout.ptLeft ? -1 : l;
}
void KWPage::setRightMargin(const double &r) {
    m_pageLayout.ptRight = r == m_parent->m_defaultPageLayout.ptRight ? -1 : r;
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
    if(m_pageLayout.ptLeft != -1)
        return m_pageLayout.ptLeft;
    return m_parent->m_defaultPageLayout.ptLeft;
}
double KWPage::rightMargin() const {
    if(m_pageLayout.ptRight != -1)
        return m_pageLayout.ptRight;
    return m_parent->m_defaultPageLayout.ptRight;
}

double KWPage::offsetInDocument() const { // the y coordinate
    return m_parent->topOfPage(m_pageNum);
}

QRect KWPage::zoomedRect(KoZoomHandler *zoomHandler) {
    QRect rect = QRect(0, zoomHandler->zoomItY(offsetInDocument()),
        zoomHandler->zoomItX(width()), zoomHandler->zoomItY(height()));
    return rect;
}

const KoRect KWPage::rect() const {
    KoRect rect = KoRect(0, offsetInDocument(), width(), height());
    return rect;
}

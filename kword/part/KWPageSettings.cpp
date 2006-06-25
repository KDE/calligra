/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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

#include "KWPageSettings.h"


KWPageSettings::KWPageSettings() {
    m_pageLayout = KoPageLayout::standardLayout();
    m_pageLayout.ptLeft = -1;
    m_pageLayout.ptRight = -1;
    m_pageLayout.ptPageEdge = 140;
    m_pageLayout.ptBindingSide = 10;
    m_mainFrame = true;
    m_headerDistance = 10;
    m_footerDistance = 9;
    m_footnoteDistance = 8;
    m_endNoteDistance = 7;
    m_firstHeader = KWord::HFTypeNone;
    m_firstFooter = KWord::HFTypeEvenOdd;
    m_headers = KWord::HFTypeEvenOdd;
    m_footers = KWord::HFTypeSameAsFirst;
}

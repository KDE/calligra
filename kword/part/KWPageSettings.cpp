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
    clear();
}
KWPageSettings::KWPageSettings(const KWPageSettings &other) {
    m_columns = other.m_columns;
    m_mainFrame = other.m_mainFrame;
    m_headerDistance = other.m_headerDistance;
    m_footerDistance = other.m_footerDistance;
    m_footNoteDistance = other.m_footNoteDistance;
    m_endNoteDistance = other.m_endNoteDistance;
    m_firstHeader = other.m_firstHeader;
    m_firstFooter = other.m_firstFooter;
    m_headers = other.m_headers;
    m_footers = other.m_footers;
    m_footNoteSeparatorLineWidth = other.m_footNoteSeparatorLineWidth;
    m_footNoteSeparatorLineLength = other.m_footNoteSeparatorLineLength;
    m_footNoteSeparatorLineType = other.m_footNoteSeparatorLineType;
    m_footNoteSeparatorLinePos = other.m_footNoteSeparatorLinePos;
}

void KWPageSettings::clear() {
    // defaults
    m_footNoteSeparatorLineLength = 20; // 20%, i.e. 1/5th
    m_footNoteSeparatorLineWidth = 0.5; // like in OOo
    m_footNoteSeparatorLineType = Qt::SolidLine;

    m_mainFrame = true;
    m_headerDistance = 10; // ~3mm
    m_footerDistance = 10;
    m_footNoteDistance = 10;
    m_endNoteDistance = 10;
    m_firstHeader = KWord::HFTypeNone;
    m_firstFooter = KWord::HFTypeEvenOdd;
    m_headers = KWord::HFTypeEvenOdd;
    m_footers = KWord::HFTypeSameAsFirst;
    m_columns.columns = 1;
    m_columns.ptColumnSpacing = 6;
}

/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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

#include "KWPageSettings.h"
#include <kdebug.h>

KWPageSettings::KWPageSettings(const QString& mastername)
    : QObject()
    , m_masterName(mastername)
{
    clear();
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
    m_headers = KWord::HFTypeNone;
    m_footers = KWord::HFTypeNone;
    m_columns.columns = 1;
    m_columns.columnSpacing = 17; // ~ 6mm
    m_hfFrameSets.clear();
    m_pageLayout = KoPageLayout::standardLayout();
}

void KWPageSettings::setFooterPolicy(KWord::HeaderFooterType p) {
    m_footers = p;
    emit relayout();
}

void KWPageSettings::setHeaderPolicy(KWord::HeaderFooterType p) {
    m_headers = p;
    emit relayout();
}

const KoPageLayout KWPageSettings::pageLayout() const {
    return m_pageLayout;
}

void KWPageSettings::setPageLayout (const KoPageLayout &pageLayout) {
    m_pageLayout = pageLayout;
}

#include "KWPageSettings.moc"

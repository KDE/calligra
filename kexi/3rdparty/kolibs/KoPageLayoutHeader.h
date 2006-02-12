/* This file is part of the KDE project
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef kopagelayoutheader_h
#define kopagelayoutheader_h

#include <KoUnit.h>
#include <KoPageLayout.h>
#include <KoPageLayoutHeaderBase.h>

class QWidget;
class KoUnitDoubleSpinBox;
class KoPagePreview;

class KoPageLayoutHeader : public KoPageLayoutHeaderBase {
    Q_OBJECT

public:
    KoPageLayoutHeader(QWidget *parent, KoUnit::Unit unit, const KoKWHeaderFooter &kwhf);
    const KoKWHeaderFooter& headerFooter();

private:
    KoUnitDoubleSpinBox *m_headerSpacing, *m_footerSpacing, *m_footnoteSpacing;

    KoKWHeaderFooter m_headerFooters;
};

#endif


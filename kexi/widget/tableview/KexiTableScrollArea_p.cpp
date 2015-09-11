/* This file is part of the KDE project
   Copyright (C) 2002 Till Busch <till@bux.at>
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and,or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#include "KexiTableScrollArea_p.h"
#include "kexitableedit.h"


KexiTableScrollArea::Private::Private(KexiTableScrollArea* t)
        : appearance(t)
{
    clearVariables();
    tv = t;
    editOnDoubleClick = true;
    disableDrawContents = false;
    navigatorEnabled = true;
    contextMenuEnabled = true;
    skipKeyPress = false;
    ensureCellVisibleOnShow = QPoint(-1, -1);
    internal_bottomMargin = 0;
    highlightedRecord = -1;
    moveCursorOnMouseRelease = false;
    horizontalHeaderVisible = true;
    recentCellWithToolTip = QPoint(-1, -1);
    dragIndicatorRubberBand = 0;
    insideResizeEvent = false;
    firstShowEvent = true;
    scrollAreaWidget = 0;
}

KexiTableScrollArea::Private::~Private()
{
    delete horizontalHeader;
    horizontalHeader = 0; //!< set because there may be pending events
    delete verticalHeader;
    verticalHeader = 0; //!< set because there may be pending events
    delete headerModel;
}

void KexiTableScrollArea::Private::clearVariables()
{
    // Initialize variables
}

void KexiTableScrollArea::Private::setSpreadSheetMode(bool set)
{
    tv->setBottomMarginInternal(set ? 0 : tv->recordHeight() / 2);
    //copy the m_navPanelEnabled flag
    Appearance a = appearance;
    a.navigatorEnabled = tv->m_navPanelEnabled;
    tv->setAppearance(a);
}

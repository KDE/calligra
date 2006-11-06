/* This file is part of the KDE project
   Copyright (C) 2002 Till Busch <till@bux.at>
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexitableview_p.h"
#include "kexitableedit.h"

#include <qlabel.h>

#include <kglobalsettings.h>


KexiTableViewPrivate::KexiTableViewPrivate(KexiTableView* t)
 : appearance(t)
{
	clearVariables();
	tv = t;
	editOnDoubleClick = true;
	pBufferPm = 0;
	disableDrawContents = false;
	navigatorEnabled = true;
	contextMenuEnabled = true;
	skipKeyPress = false;
//moved	vScrollBarValueChanged_enabled = true;
//moved	scrollbarToolTipsEnabled = true;
//moved	scrollBarTipTimerCnt = 0;
//moved	scrollBarTip = 0;
	ensureCellVisibleOnShow = QPoint(-1,-1);
	internal_bottomMargin = tv->horizontalScrollBar()->sizeHint().height()/2;
	highlightedRow = -1;
	moveCursorOnMouseRelease = false;
	horizontalHeaderVisible = true;
	recentCellWithToolTip = QPoint(-1,-1);
}

KexiTableViewPrivate::~KexiTableViewPrivate()
{
	delete pBufferPm;
//moved	delete scrollBarTip;
}

void KexiTableViewPrivate::clearVariables()
{
	// Initialize variables
}

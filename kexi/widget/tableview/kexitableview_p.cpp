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
//moved , autonumberIcon( qembed_findData("autonumber.png") )
{
	clearVariables();
	tv = t;
//moved	pInsertItem = 0;

	editOnDoubleClick = true;
	pBufferPm = 0;
//moved	deletionPolicy = KexiTableView::AskDelete;
	disableDrawContents = false;
//moved	readOnly = -1; //don't know
//moved	insertingEnabled = -1; //don't know
	
//moved	contentsMousePressEvent_dblClick = false;
//moved	isSortingEnabled = true;
	navigatorEnabled = true;
	contextMenuEnabled = true;
//moved	filteringEnabled = true;
//moved	navPanel = 0;
	skipKeyPress = false;
	vScrollBarValueChanged_enabled = true;
	scrollbarToolTipsEnabled = true;
	scrollBarTipTimerCnt = 0;
	scrollBarTip = 0;
//moved	inside_acceptEditor = false;
//moved	internal_acceptsRowEditAfterCellAccepting = false;
//moved	acceptsRowEditAfterCellAccepting = false;
//moved	emptyRowInsertingEnabled = false;
//moved	dragIndicatorLine = -1;
//moved		rowWillBeDeleted = -1;
//	dropsAtRowEnabled = false;
//moved	initDataContentsOnShow = false;
//moved	cursorPositionSetExplicityBeforeShow = false;
//moved	verticalHeaderAlreadyAdded = false;
	ensureCellVisibleOnShow = QPoint(-1,-1);
//moved	spreadSheetMode = false;
	internal_bottomMargin = tv->horizontalScrollBar()->sizeHint().height()/2;
	highlightedRow = -1;
	moveCursorOnMouseRelease = false;
}

KexiTableViewPrivate::~KexiTableViewPrivate()
{
	delete pBufferPm;
//moved	delete pInsertItem;
//moved	delete pRowEditBuffer;
	delete scrollBarTip;
}

void KexiTableViewPrivate::clearVariables()
{
	// Initialize variables
//moved	pEditor = 0;
//	numRows = 0;
//moved	curRow = -1;
//moved	curCol = -1;
//moved	pCurrentItem=0;
//moved	pRowEditBuffer=0;
//	pInsertItem = 0;
//moved	rowEditing = false;
//moved	newRowEditing = false;
//	sortedColumn = -1;
//	sortOrder = true;
//	recordIndicator = false;
}


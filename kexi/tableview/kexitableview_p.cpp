/* This file is part of the KDE project
   Copyright (C) 2002 Till Busch <till@bux.at>
   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#include "kexitableview_p.h"
#include <qlabel.h>

KexiTableViewPrivate::KexiTableViewPrivate()
{
	clearVariables();

//	numCols = 0;
	editOnDoubleClick = true;
	pBufferPm = 0;
	deletionPolicy = KexiTableView::NoDelete;
	disableDrawContents = false;
	readOnly = -1; //don't know
	insertingEnabled = -1; //don't know
	
	contentsMousePressEvent_dblClick = false;
	isSortingEnabled = true;
	navigationPanelEnabled = true;
	navPanel = 0;
	skipKeyPress = false;
	navRowNumber = 0;
	scrollBarTipTimerCnt = 0;
	scrollBarTip = 0;
}

KexiTableViewPrivate::~KexiTableViewPrivate()
{
	delete pBufferPm;
	delete pInsertItem;
	delete scrollBarTip;
}

void KexiTableViewPrivate::clearVariables() {
	// Initialize variables
	pEditor = 0;
//	numRows = 0;
	curRow = -1;
	curCol = -1;
	pCurrentItem=0;
	pInsertItem = 0;
	rowEditing = false;
	newRowEditing = false;
//	sortedColumn = -1;
//	sortOrder = true;
//	recordIndicator = false;
}


/* This file is part of the KDE project
   Copyright (C) 2002 Till Busch <till@bux.at>
   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#ifndef KEXITABLEVIEW_P_H
#define KEXITABLEVIEW_P_H

#include "kexitableview.h"
#include "kexitableheader.h"

#include <kexidb/roweditbuffer.h>

#include <qevent.h>
#include <qtimer.h>
#include <qvalidator.h>
#include <qasciidict.h>

#include <kpushbutton.h>
#include <ktoolbarbutton.h>
#include <klineedit.h>
#include <kpopupmenu.h>
#include <kaction.h>

class KexiTableItem;
class KexiTableRM;
class KexiTableEdit;
class QLabel;

class KexiTableViewPrivate 
{
	public:

	KexiTableViewPrivate();
	~KexiTableViewPrivate();
	void clearVariables();

	bool editOnDoubleClick : 1;
//	bool recordIndicator : 1;

	// cursor position
	int			curRow;
	int			curCol;
	KexiTableItem	*pCurrentItem;

	//! when (current or new) row is edited - changed field values are temporary stored here
	KexiDB::RowEditBuffer *pRowEditBuffer; 

	// foreign widgets
	QHeader			*pTopHeader;
	KexiTableRM	*pVerticalHeader;
//	KexiTableRM		*pRecordMarker;
	KexiTableEdit	*pEditor;

	//! editors: one for each column (indexed by KexiTableViewColumn)
	QPtrDict<KexiTableEdit> editors;

//	int numRows;
//	int numCols;
	int rowHeight;
//	int sortedColumn;
//	bool sortOrder;

//	KexiTableView::InsertionPolicy insertionPolicy;
	KexiTableView::DeletionPolicy deletionPolicy;

	QPixmap			*pBufferPm;
	QTimer			*pUpdateTimer;
	KPopupMenu		*pContextMenu;
	int menu_id_addRecord;
	int menu_id_removeRecord;

//	QMemArray<QVariant::Type> pColumnTypes;
//	QMemArray<int> pColumnModes;
//	QPtrList<QVariant> pColumnDefaults;

#if 0//(js) doesn't work!
	QTimer			*scrollTimer;
#endif
	KexiTableItem	*pInsertItem;
	
//	QStringList		dropFilters;

	KexiTableView::ScrollDirection scrollDirection;

	bool needAutoScroll : 1;

	bool bgAltering : 1;
	
	bool fullRowSelectionEnabled : 1;

	bool disableDrawContents : 1;

	/*! true if currently selected row is edited */
	bool rowEditing : 1;
	
	/*! true if currently selected, new row is edited;
	  implies: rowEditing==true. */
	bool newRowEditing : 1;

	/*! true if currently double click action was is performed 
	(so accept/cancel editor shoudn't be executed) */
	bool contentsMousePressEvent_dblClick : 1;

	//! 'sorting by column' availability flag for widget
	bool isSortingEnabled : 1;

	/*! true if the navigation panel is enabled (visible) for the view.
	 True by default. */
	bool navigatorEnabled : 1;

	/*! true if the context menu is enabled (visible) for the view.
	 True by default. */
	bool contextMenuEnabled : 1;

	/*! true if filtering is enabled for the view. */
	bool filteringEnabled : 1;
	
	/*! used to force single skip keyPress event. */
	bool skipKeyPress : 1;
	
	/*! Used to enable/disable execution of KexiTableView::vScrollBarValueChanged()
	 when users navigates rows using keyboard, so vscrollbar tooltips are not visible then. */
	bool vScrollBarValueChanged_enabled : 1;

	/*! True, if vscrollbar tooltips are enabled (true by default) */
	bool scrollbarToolTipsEnabled : 1;

	/*! Used in acceptEditor() to avoid infinite recursion, 
	 eg. when we're calling acceptRowEdit() during cell accepting phase. */
	bool inside_acceptEditor : 1;

	/*! Internal: if true, this table view automatically accepts 
	 row editing (using acceptRowEdit()) on accepting any cell's edit 
	 (i.e. after acceptEditor()). */
	bool internal_acceptsRowEditAfterCellAccepting : 1;

	/*! Public version of 'acceptsRowEditAfterCellAcceptin' flag (available for a user).
	 It's OR'es together with above flag.
	*/
	bool acceptsRowEditAfterCellAccepting : 1;

	/*! true, if inserting empty rows are enabled (false by default) */
	bool emptyRowInsertingEnabled : 1;

	/*! true, if this table accepts dropping data on the rows (false by default). */
	bool dropsAtRowEnabled : 1;

	/*! true, if initDataContents() should be called on show event. */
	bool initDataContentsOnShow : 1;

	/*! 1 if table view is readOnly, 0 if not; 
	 otherwise (-1 means "dont know") the 'readOnly' flag from table views' 
	 internal data structure (KexiTableViewData *KexiTableView::m_data) is reused. 
	 */
	int readOnly;

	/*! like for readOnly: 1 if inserting is enabled */
	int insertingEnabled;

	QColor emptyAreaColor;
	
	/*! Navigation widgets, used if navigationPanelEnabled is true. */
	QFrame *navPanel; //!< main navigation widget
	QToolButton *navBtnFirst, *navBtnPrev, *navBtnNext, *navBtnLast, *navBtnNew;
	KLineEdit *navRowNumber;
	QIntValidator *navRowNumberValidator;
	KLineEdit *navRowCount; //!< readonly counter
	int nav1DigitWidth;

	QLabel *scrollBarTip;
	QTimer scrollBarTipTimer;
	uint scrollBarTipTimerCnt; //!< helper for timeout counting
	
	//! row colors
	QColor baseColor, textColor, altColor, grayColor;
	QBrush diagonalGrayPattern;

	//! Actions pluged for this table view. \sa plugSharedAction()
	QAsciiDict<KAction> sharedActions;

	/*! Row number that over which user drags a mouse pointer.
	 Used to indicate dropping possibility for that row. 
	 Equal -1 if no indication is needed.
	*/
	int dragIndicatorLine;

	//! Used by delayed mode of maximizeColumnsWidth() 
	QValueList<int> maximizeColumnsWidthOnShow;
};

#endif

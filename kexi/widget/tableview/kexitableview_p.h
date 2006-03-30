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

#ifndef KEXITABLEVIEW_P_H
#define KEXITABLEVIEW_P_H

#include "kexitableview.h"
#include "kexitableheader.h"

#include <kexidb/roweditbuffer.h>
#include <widget/utils/kexidisplayutils.h>

#include <qevent.h>
#include <qtimer.h>
#include <qvalidator.h>
#include <q3asciidict.h>
//Added by qt3to4:
#include <QPixmap>
#include <QLabel>
#include <Q3ValueList>

#include <kpushbutton.h>

#include <klineedit.h>
#include <kmenu.h>
#include <kaction.h>

class KexiTableItem;
class KexiTableRM;
class KexiTableEdit;
class QLabel;
class TableViewHeader;

/*! KexiTableView internal data
 @internal */
class KexiTableViewPrivate 
{
	public:

	KexiTableViewPrivate(KexiTableView* t);
	~KexiTableViewPrivate();

	void clearVariables();

	KexiTableView *tv;

//moved	//! cursor position
//moved	int curRow;
//moved	int curCol;
//moved	KexiTableItem *pCurrentItem;

//moved	//! when (current or new) row is edited - changed field values are temporary stored here
//moved	KexiDB::RowEditBuffer *pRowEditBuffer; 

	// foreign widgets
	TableViewHeader *pTopHeader;
//moved	KexiTableRM *pVerticalHeader;
//moved	KexiTableEdit *pEditor;

	//! editors: one for each column (indexed by KexiTableViewColumn)
	Q3PtrDict<KexiTableEdit> editors;

	int rowHeight;

//moved	KexiTableView::DeletionPolicy deletionPolicy;

	QPixmap *pBufferPm;
	QTimer *pUpdateTimer;
//moved	KMenu *pContextMenu;
	int menu_id_addRecord;
	int menu_id_removeRecord;

//	QMemArray<QVariant::Type> pColumnTypes;
//	QMemArray<int> pColumnModes;
//	QPtrList<QVariant> pColumnDefaults;

#if 0//(js) doesn't work!
	QTimer *scrollTimer;
#endif
//moved	KexiTableItem *pInsertItem;
	
	KexiTableView::ScrollDirection scrollDirection;

	bool editOnDoubleClick : 1;

	bool needAutoScroll : 1;

	bool disableDrawContents : 1;

//moved	/*! true if currently selected row is edited */
//moved	bool rowEditing : 1;
	
//moved	/*! true if currently selected, new row is edited;
//moved	  implies: rowEditing==true. */
//moved	bool newRowEditing : 1;

//moved	/*! true if currently double click action was is performed 
//moved	(so accept/cancel editor shoudn't be executed) */
//moved	bool contentsMousePressEvent_dblClick : 1;

//moved	//! 'sorting by column' availability flag for widget
//	bool isSortingEnabled : 1;

	/*! true if the navigation panel is enabled (visible) for the view.
	 True by default. */
	bool navigatorEnabled : 1;

	/*! true if the context menu is enabled (visible) for the view.
	 True by default. */
	bool contextMenuEnabled : 1;

//moved	/*! true if filtering is enabled for the view. */
//moved	bool filteringEnabled : 1;
	
	/*! used to force single skip keyPress event. */
	bool skipKeyPress : 1;
	
	/*! Used to enable/disable execution of KexiTableView::vScrollBarValueChanged()
	 when users navigates rows using keyboard, so vscrollbar tooltips are not visible then. */
	bool vScrollBarValueChanged_enabled : 1;

	/*! True, if vscrollbar tooltips are enabled (true by default) */
	bool scrollbarToolTipsEnabled : 1;

//moved	/*! Used in acceptEditor() to avoid infinite recursion, 
//moved	 eg. when we're calling acceptRowEdit() during cell accepting phase. */
//moved	bool inside_acceptEditor : 1;

//moved	/*! @internal if true, this table view automatically accepts 
//moved	 row editing (using acceptRowEdit()) on accepting any cell's edit 
//moved	 (i.e. after acceptEditor()). */
//moved	bool internal_acceptsRowEditAfterCellAccepting : 1;

//moved	/*! Public version of 'acceptsRowEditAfterCellAcceptin' flag (available for a user).
//moved	 It's OR'es together with above flag.
//moved	*/
//moved	bool acceptsRowEditAfterCellAccepting : 1;

//moved	/*! true, if inserting empty rows are enabled (false by default) */
//moved	bool emptyRowInsertingEnabled : 1;

//moved	/*! true, if this table accepts dropping data on the rows (false by default). */
//moved	bool dropsAtRowEnabled : 1;

//moved	/*! true, if initDataContents() should be called on show event. */
//moved	bool initDataContentsOnShow : 1;

//moved	/*! Set to true in setCursor() to indicate that cursor position was set
//moved	 before show() and it shouldn't be changed on show(). 
//moved	 Only used if initDataContentsOnShow is true. */
//moved	bool cursorPositionSetExplicityBeforeShow : 1;

//moved	/*! true, if certical header shouldn't be increased in
//moved	 KexiTableView::slotRowInserted() because it was already done 
//moved	 in KexiTableView::createEditor(). */
//moved	bool verticalHeaderAlreadyAdded : 1;

//moved	/*! true if spreadSheetMode is enabled. False by default.
//moved	 @see KexiTableView::setSpreadSheetMode() */
//moved	bool spreadSheetMode : 1;

	/*! true if cursor should be moved on mouse release evenr rather than mouse press 
	 in handleContentsMousePressOrRelease().
	 False by default. Used by KeixComboBoxPopup. */
	bool moveCursorOnMouseRelease : 1;

//moved	/*! 1 if table view is readOnly, 0 if not; 
//moved	 otherwise (-1 means "dont know") the 'readOnly' flag from table views' 
//moved	 internal data structure (KexiTableViewData *KexiTableView::m_data) is reused. 
//moved	 */
//moved	int readOnly;

//moved	/*! like for readOnly: 1 if inserting is enabled */
//moved	int insertingEnabled;

	KexiTableView::Appearance appearance;
	
//moved	/*! Navigation widgets, used if navigationPanelEnabled is true. */
//moved	KexiRecordNavigator *navPanel; //!< main navigation widget
	//moved to KexiRecordNavigator
//	QToolButton *navBtnFirst, *navBtnPrev, *navBtnNext, *navBtnLast, *navBtnNew;
//	KLineEdit *navRowNumber;
//	QIntValidator *navRowNumberValidator;
//	KLineEdit *navRowCount; //!< readonly counter
//	int nav1DigitWidth;

	QLabel *scrollBarTip;
	QTimer scrollBarTipTimer;
	uint scrollBarTipTimerCnt; //!< helper for timeout counting
	
	//! brushes, fonts
//	QColor baseColor, textColor, altColor, grayColor;
	QBrush diagonalGrayPattern;
///moved	QFont autonumberFont;
//moved	int autonumberTextWidth;
//moved	QPixmap autonumberIcon;
	KexiDisplayUtils::DisplayParameters autonumberSignDisplayParameters;

//moved	//! Actions pluged for this table view. \sa plugSharedAction()
//moved	QAsciiDict<KAction> sharedActions;

//moved	/*! Row number that over which user drags a mouse pointer.
//moved	 Used to indicate dropping possibility for that row. 
//moved	 Equal -1 if no indication is needed.
//moved	*/
//moved	int dragIndicatorLine;

//moved		/*! Row number (>=0 or -1 == no row) that will be deleted in KexiTableViewData::deleteRow().
//moved		 It is set in slotAboutToDeleteRow(KexiTableItem&,KexiDB::ResultInfo*,bool)) slot
//moved		 received from KexiTableViewData member. 
//moved		 This value will be used in slotRowDeleted() after rowDeleted() signal 
//moved		 is received from KexiTableViewData member and the cleared (set to -1). */
//moved	int rowWillBeDeleted;

	//! Used by delayed mode of maximizeColumnsWidth() 
	Q3ValueList<int> maximizeColumnsWidthOnShow;

	/*! Used for delayed call of ensureCellVisible() after show().
	 It's equal to (-1,-1) if ensureCellVisible() shouldn't e called. */
	QPoint ensureCellVisibleOnShow;

	/*! @internal Changes bottom margin settings, in pixels. 
	 At this time, it's used by KexiComboBoxPopup to decrease margin for popup's table. */
	int internal_bottomMargin;

	/*! Helper for "highlighted row" effect. */
	int highlightedRow;

	/*! Id of context menu key (cached). */
	int contextMenuKey;
};

#endif

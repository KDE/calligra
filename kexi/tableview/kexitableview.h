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

#ifndef KEXITABLEVIEW_H
#define KEXITABLEVIEW_H

//we want to have kde-support:
#define USE_KDE

#include <qscrollview.h>
#include <qtimer.h>
#include <qvariant.h>
#include <qptrlist.h>

#include <kdebug.h>

#include "kexitablerm.h"
#include "kexitableviewdata.h"

class QHeader;
class QLineEdit;
class QPopupMenu;
class QTimer;

class KPrinter;

class KexiTableHeader;
class KexiTableRM;
class KexiTableItem;
class KexiTableEdit;
class KexiTableViewPrivate;
//class KexiTableList;

/*
	this class provides a tablewidget ;)
*/

class KEXIDATATABLE_EXPORT KexiTableView : public QScrollView
{
Q_OBJECT
public:
	
	KexiTableView(KexiTableViewData* data=0, QWidget* parent=0, const char* name=0);
	~KexiTableView();

	KexiTableViewData *data() const { return m_data; }

	/*! Sets data for this table view. if \a owner is true, the table view will own 
	 \a data and therefore will destoy when required, else: \a data is (possibly) shared and
	 not owned by the widget. 
	 If widget already has data assigned (and owns this data),
	 old data is destroyed before new assignment.
	 */
	void setData( KexiTableViewData *data, bool owner = true );

	/*! Clears internal table data and its visible representation. 
	 Does not clear columns information.
	 Does not destroy KexiTableViewData object (if present) but only clears its contents.
	 Repaints widget if \a repaint is true. */
	void clearData(bool repaint = true);

	/*! Clears columns information.and thus 
	 all internal table data and its visible representation (using clearData()).
	 Repaints widget if \a repaint is true.
	 */
	void clearColumns(bool repaint = true);

/*js	enum ColumnModes
	{
		ColumnReadOnly = 1,
		ColumnEditable,
		ColumnAutoIncrement
	};*/

//	virtual void addColumn(QString name, QVariant::Type type, bool editable, QVariant defaultValue = QVariant(""),
//		int width=100, bool autoinc=false);

	QString columnCaption(int colNum) const;
	
	bool isSortingEnabled() const;
	/*! \return sorted column number or -1 if no column is sorted */
	int sortedColumn();
	/*! \return true if ascending order for sorting. This not implies that 
	 any sorting has been performed. */
	bool sortingAscending() const;

	QVariant::Type columnType(int col) const;
	QVariant columnDefaultValue(int col) const;
	bool columnEditable(int col) const;
	inline KexiTableItem *itemAt(int row) const;
	
	/*! \return true if data represented by this table view 
	 is not editable using it (it can be editable with other ways although). */
	virtual bool isReadOnly() const;

	/*! Sets readOnly flag for this table view.
	 Unless the flag is set, the widget inherits readOnly flag from it's data
	 structure assigned with setData(). The default value if false.
	 
	 This method is useful when you need to switch on the flag indepentently 
	 from the data structure.
	 Note: it is not allowed to force readOnly off
	 when internal data is readOnly - in that case the method does nothing.
	 You can check internal data flag calling data()->readOnly().
	*/
	void setReadOnly(bool set);

	/*! \return true if data inserting is enabled (the default).
	*/
	bool isInsertingEnabled() const;

	/*! Sets insertEnabled flag. If true, empty row is available 
	 at the end of this widget for new entering new data. 
	 Unless the flag is set, the widget inherits insertingEnabled flag from it's data
	 structure assigned with setData(). The default value if false.

	 Note: it is not allowed to force insertingEnabled on when internal data 
	 has insertingEnabled set off - in that case the method does nothing.
 	 You can check internal data flag calling data()->insertingEnabled().
	 
	 \sa setReadOnly()
	*/
	void setInsertingEnabled(bool set);

	int currentColumn() const;
	int currentRow() const;

	KexiTableItem *selectedItem() const;

	int		rows() const;
	int		cols() const;

	QRect		cellGeometry(int row, int col) const;
	int		columnWidth(int col) const;
	int		rowHeight() const;
	int		columnPos(int col) const;
	int		rowPos(int row) const;
	int		columnAt(int pos) const;
	int		rowAt(int pos, bool ignoreEnd=false) const;

	/*! \return true if currently selected row is edited. */
	bool rowEditing() const;

	void		updateCell(int row, int col);
//	void		updateRow(int row);
//	void		remove(int row);

	// properties
	bool		backgroundAltering() const;
	void		setBackgroundAltering(bool altering);
//	bool		recordIndicator() const;
//	void		setRecordIndicator(bool indicator);
	bool		editableOnDoubleClick() const;
	void		setEditableOnDoubleClick(bool set);
	QColor		emptyAreaColor() const;
	void		setEmptyAreaColor(QColor c);

#ifndef KEXI_NO_PRINT
	// printing
//	void		setupPrinter(KPrinter &printer);
	void		print(KPrinter &printer);
#endif

	// reimplemented for internal reasons
	virtual QSizePolicy	sizePolicy() const;
	virtual QSize		sizeHint() const;
	virtual QSize		minimumSizeHint() const;
	void		setFont(const QFont &f);

	void		addDropFilter(const QString &filter);

//	void		inserted();

	void		emitSelected();

	KexiTableRM	*recordMarker() const;
	KexiTableRM *verticalHeader() const;
	
//	void		takeInsertItem();
//	void		setInsertItem(KexiTableItem *i);
//	KexiTableItem	*insertItem() const;

	enum AdditionPolicy
	{
		NoAdd,
		AutoAdd,
		SignalAdd
	};

	enum DeletionPolicy
	{
		NoDelete,
		AskDelete,
		ImmediateDelete,
		SignalDelete
	};

	virtual void	setAdditionPolicy(AdditionPolicy policy);
	AdditionPolicy	additionPolicy() const;

	virtual void	setDeletionPolicy(DeletionPolicy policy);
	DeletionPolicy	deletionPolicy() const;

	void triggerUpdate();

	typedef enum ScrollDirection
	{
		ScrollUp,
		ScrollDown,
		ScrollLeft,
		ScrollRight
	};

protected:
	// painting and layout
	void	drawContents(QPainter *p, int cx, int cy, int cw, int ch);
	void	createBuffer(int width, int height);
	void	paintCell(QPainter* p, KexiTableItem *item, int col, const QRect &cr, bool print=false);
	void	paintEmptyArea(QPainter *p, int cx, int cy, int cw, int ch);
	void	updateGeometries();
	virtual QSize tableSize() const;

	QPoint contentsToViewport2( const QPoint &p );
	void contentsToViewport2( int x, int y, int& vx, int& vy );
	QPoint viewportToContents2( const QPoint& vp );

	// event handling
	virtual void contentsMousePressEvent(QMouseEvent*);
	virtual void contentsMouseMoveEvent(QMouseEvent*);
	virtual void contentsMouseDoubleClickEvent(QMouseEvent *e);
	virtual void contentsMouseReleaseEvent(QMouseEvent *e);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void focusInEvent(QFocusEvent*);
	virtual void focusOutEvent(QFocusEvent*);
	virtual void resizeEvent(QResizeEvent *);
	virtual void viewportResizeEvent( QResizeEvent *e );//js
	virtual void showEvent(QShowEvent *e);
	virtual void contentsDragMoveEvent(QDragMoveEvent *e);
	virtual void contentsDropEvent(QDropEvent *ev);

	void	createEditor(int row, int col, QString addText = QString::null, bool backspace = false);
	bool	focusNextPrevChild(bool next);

	/*! Updates visibility/accesibility of popup menu items,
	returns false if no items are visible after update. */
	bool	updateContextMenu();
	
	/*! Shows context menu at \a pos for selected cell
	 if menu is configured,
	 else: contextMenuRequested() signal is emmited.
	 Method used in contentsMousePressEvent() (for right button)
	 and keyPressEvent() for Qt::Key_Menu key.
	 If \a pos is QPoint(-1,-1) (the default), menu is positioned below the current cell.
	*/
	void	showContextMenu( QPoint pos = QPoint(-1,-1) );

	/*! internal */
	inline void paintRow(KexiTableItem *item,
		QPainter *pb, int r, int rowp, int cx, int cy, 
		int colfirst, int collast, int maxwc,
		const QColor& baseCol, const QColor& altCol);

	void remove(KexiTableItem *item, bool moveCursor=true);

protected slots:
	void			columnWidthChanged( int col, int os, int ns );
	void			cancelEditor();
	virtual void	acceptEditor();
	virtual void	boolToggled();
	void			slotUpdate();
	void sortColumnInternal(int col);

	/*! Accepts row editing. All changes made to the editing 
	 row duing this current session will be accepted. */
	void acceptRowEdit();
	/*! Cancels row editing All changes made to the editing 
	 row duing this current session will be undone. */
	void cancelRowEdit();

	void			slotAutoScroll();

public slots:
	//! Sets sorting on column \a col, or (when \a col == -1) sets rows unsorted
	//! this will dont work if sorting is disabled with setSortingEnabled()
	void setSorting(int col, bool ascending=true);
	/*! Enables or disables sorting for this table view
		This method is different that setSorting() because it prevents both user
		and programmer from sorting by clicking a column's header or calling setSorting().
		By default sorting is enabled for table view.
	*/
	void setSortingEnabled(bool set);
	//! Sorts all rows by column selected with setSorting()
	void sort();
	void			setCursor(int row, int col = -1);
	void			selectRow(int row);
	void			selectNextRow();
	void			selectPrevRow();
	void			selectFirstRow();
	void			selectLastRow();
//	void			gotoNext();
//js	int			findString(const QString &string);
	
	/*! Removes currently selected record; does nothing if no record 
	 is currently selected. If record is in edit mode, editing 
	 is cancelled before removing.  */
	void removeCurrentRecord();
	
//	virtual void		addRecord();

signals:
	void			itemSelected(KexiTableItem *);
	void			itemReturnPressed(KexiTableItem *, int);
	void			itemDblClicked(KexiTableItem *, int);
	void			itemChanged(KexiTableItem *, int);
	void			itemChanged(KexiTableItem *, int,QVariant oldValue);
	void			itemRemoveRequest(KexiTableItem *);
	void			currentItemRemoveRequest();
	void			addRecordRequest();
	void			dropped(QDropEvent *);
	void			contextMenuRequested(KexiTableItem *, int col, const QPoint &);
	void			sortedColumnChanged(int col);

protected:
	KexiTableViewData *m_data;
	bool m_owner : 1;

	KexiTableViewPrivate *d;

	friend class KexiTableItem;
};

inline KexiTableItem *KexiTableView::itemAt(int row) const
{
	if (!m_data->at(row))
		kdDebug() << "KexiTableView::itemAt(" << row << "): NO ITEM!!" << endl;
	return m_data->at(row);
}
/*
inline int KexiTableView::currentRow()
{
	return m_curRow;
}

inline KexiTableItem *KexiTableView::selectedItem()
{
	return m_pCurrentItem;
}

inline QVariant::Type KexiTableView::columnType(int col)
{
	return m_pColumnTypes->at(col);
}

inline bool	KexiTableView::columnEditable(int col)
{
	return m_pColumnModes->at(col);
	if(m_pColumnModes->at(col) & ColumnEditable)
		return true;

	return false;
}

inline QVariant KexiTableView::columnDefault(int col)
{
	return *m_pColumnDefaults->at(col);
}
*/
#endif

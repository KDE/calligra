/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
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
*/

#ifndef KEXIRELATIONWIDGET_H
#define KEXIRELATIONWIDGET_H

#include <Q3CString>
#include <KexiView.h>
#include "kexirelationview.h"

class KComboBox;
class KPushButton;
class KMenu;
class KAction;
class Q3ListViewItem;

class KexiMainWindowIface;

namespace KexiDB
{
	class Connection;
	class TableSchema;
	class Reference;
}

class KEXIRELATIONSVIEW_EXPORT KexiRelationWidget : public KexiView
{
	Q_OBJECT

	public:
		KexiRelationWidget(QWidget *parent);
		virtual ~KexiRelationWidget();

		//! \return a dictionary of added tables
		TablesDict* tables() const;
		KexiRelationViewTableContainer* table(const QString& name) const;
		const ConnectionList* connections() const;

//		KexiRelationView	*relationView() const { return m_relationView; }
		void addTable(const QString& t);

//		void openTable(KexiDB::TableSchema* table, bool designMode);

		virtual QSize sizeHint() const;

		/*! Used to add newly created object information to the combo box. */
		void objectCreated(const Q3CString &mime, const Q3CString& name);
		void objectDeleted(const Q3CString &mime, const Q3CString& name);
		void objectRenamed(const Q3CString &mime, const Q3CString& name, const Q3CString& newName);

	signals:
		void tableAdded(KexiDB::TableSchema& t);
		void tableHidden(KexiDB::TableSchema& t);
		void tablePositionChanged(KexiRelationViewTableContainer*);
		void aboutConnectionRemove(KexiRelationViewConnection*);
		void tableFieldDoubleClicked( KexiDB::TableSchema* table, const QString& fieldName );
	
	public slots:
		/*! Adds a table \a t to the area. This changes only visual representation.
		 If \a rect is valid, table widget rgeometry will be initialized.
		 */
		void addTable(KexiDB::TableSchema *t, const QRect &rect = QRect());

		//! Adds a connection \a con to the area. This changes only visual representation.
		void addConnection(const SourceConnection& conn);

		void removeSelectedObject();

		/*! Removes all tables and coonections from the widget. */
		void clear();

		/*! Removes all coonections from the view. */
		void removeAllConnections();

		/*! Hides all tables except \a tables. */
		void hideAllTablesExcept( KexiDB::TableSchema::List* tables );

	protected slots:
		void slotAddTable();
		void tableViewGotFocus();
		void connectionViewGotFocus();
		void emptyAreaGotFocus();
		void tableContextMenuRequest(const QPoint& pos);
		void connectionContextMenuRequest(const QPoint& pos);
		void emptyAreaContextMenuRequest( const QPoint& pos );
		void openSelectedTable();
		void designSelectedTable();
		void slotTableHidden(KexiDB::TableSchema &table);
		void aboutToShowPopupMenu();
		void slotTableFieldDoubleClicked(Q3ListViewItem *i,const QPoint&,int);

	protected:
		/*! executes popup menu at \a pos, or, 
		 if \a pos not specified: at center of selected table view (if any selected),
		 or at center point of the relations view. */
		void executePopup( QPoint pos = QPoint(-1,-1) );

		//! Invalidates all actions availability.
		void invalidateActions();

		//! Fills table's combo box with all available table names.
		void fillTablesCombo();

	private:
		KComboBox *m_tableCombo;
		KPushButton *m_btnAdd;
		KexiRelationView *m_relationView;
		KexiDB::Connection *m_conn;

		KMenu *m_tableQueryPopup //over table/query
			, *m_connectionPopup //over connection
			, *m_areaPopup; //over outer area
		KAction *m_openSelectedTableAction, *m_designSelectedTableAction;
		QAction *m_tableQueryPopupTitle, *m_connectionPopupTitle;
};

#endif

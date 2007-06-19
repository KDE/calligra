/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2004, 2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIRELATIONVIEW_H
#define KEXIRELATIONVIEW_H

#include <qpointer.h>
#include <q3scrollview.h>
#include <q3ptrlist.h>
#include <q3dict.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <Q3Frame>
#include <QKeyEvent>

#include <kexidb/tableschema.h>

#include "kexirelationviewconnection.h"
#include "kexirelationviewtable.h"

class Q3Frame;

class KexiRelationViewTable;

namespace KexiDB
{
	class Connection;
}

typedef Q3Dict<KexiRelationViewTableContainer> TablesDict;
typedef Q3DictIterator<KexiRelationViewTableContainer> TablesDictIterator;
typedef Q3PtrList<KexiRelationViewConnection> ConnectionList;
typedef Q3PtrListIterator<KexiRelationViewConnection> ConnectionListIterator;

struct SourceConnection
{
	QString masterTable;
	QString detailsTable;
	QString masterField;
	QString detailsField;
};

/*! @short provides a view for displaying relations between database tables.

 It is currently used for two purposes:
 - displaying global database relations
 - displaying relations defined for a database query

 The class is for displaying only - retrieving data and updating data on the backend side is implemented 
 in KexiRelationWidget, and more specifically in: Kexi Relation Part and Kexi Query Part.
*/
class KEXIRELATIONSVIEW_EXPORT KexiRelationView : public Q3ScrollView
{
	Q_OBJECT

	public:
		KexiRelationView(QWidget *parent, const char *name=0);
		virtual ~KexiRelationView();

		//! \return a dictionary of added tables
		TablesDict* tables() { return &m_tables; }

		/*! Adds a table \a t to the area. This changes only visual representation.
		 If \a rect is valid, table widget geometry will be initialized.
		 \return added table container or 0 on failure.
		 */
		KexiRelationViewTableContainer* addTableContainer(KexiDB::TableSchema *t, 
			const QRect &rect = QRect());

		/*! \return table container for table \a t. */
		KexiRelationViewTableContainer * tableContainer(KexiDB::TableSchema *t) const;

		//! Adds a connection \a con to the area. This changes only visual representation.
		void addConnection(const SourceConnection& _conn /*, bool interactive=true*/);

		void setReadOnly(bool);

		inline KexiRelationViewConnection* selectedConnection() const { return m_selectedConnection; }

		inline KexiRelationViewTableContainer* focusedTableView() const { return m_focusedTableView; }

		virtual QSize sizeHint() const;

		const ConnectionList* connections() const { return &m_connectionViews; }

//		KexiRelationViewTableContainer* containerForTable(KexiDB::TableSchema* tableSchema);

	signals:
		void tableContextMenuRequest( const QPoint& pos );
		void connectionContextMenuRequest( const QPoint& pos );
		void emptyAreaContextMenuRequest( const QPoint& pos );
		void tableViewGotFocus();
		void connectionViewGotFocus();
		void emptyAreaGotFocus();
		void tableHidden(KexiDB::TableSchema& t);
		void tablePositionChanged(KexiRelationViewTableContainer*);
		void aboutConnectionRemove(KexiRelationViewConnection*);

	public slots:
		//! Clears current selection - table/query or connection
		void clearSelection();

		/*! Removes all tables and connections from the view. 
		 Does not emit signals like tableHidden(). */
		void clear();

		/*! Removes all coonections from the view. */
		void removeAllConnections();

		/*! Hides all tables except \a tables. */
		void hideAllTablesExcept( KexiDB::TableSchema::List* tables );

		void slotTableScrolling(const QString&);

		//! removes selected table or connection
		void removeSelectedObject();


	protected slots:
		void containerMoved(KexiRelationViewTableContainer *c);
		void slotListUpdate(QObject *s);
		void slotTableViewEndDrag();
		void slotTableViewGotFocus();

	protected:
//		/*! executes popup menu at \a pos, or, 
//		 if \a pos not specified: at center of selected table view (if any selected),
//		 or at center point of the relations view. */
//		void executePopup( QPoint pos = QPoint(-1,-1) );

		void drawContents(QPainter *p, int cx, int cy, int cw, int ch);
		void contentsMousePressEvent(QMouseEvent *ev);
		virtual void keyPressEvent(QKeyEvent *ev);

		void recalculateSize(int width, int height);
		void stretchExpandSize();
//		void		invalidateActions();
//		void clearTableSelection();
//		void clearConnSelection();

		void hideTable(KexiRelationViewTableContainer* tableView);
		void removeConnection(KexiRelationViewConnection *conn);

		TablesDict m_tables;
		bool m_readOnly;
		ConnectionList m_connectionViews;
		KexiRelationViewConnection* m_selectedConnection;
		QPointer<KexiRelationViewTableContainer> m_focusedTableView;
};

#endif

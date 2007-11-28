/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KexiRelationsScrollArea_H
#define KexiRelationsScrollArea_H

#include <QScrollArea>
#include <QSet>
#include <QMutableSetIterator>
#include <QHash>
#include <QMutableHashIterator>
#include <QMouseEvent>
#include <QKeyEvent>

#include <kexidb/tableschema.h>

#include "KexiRelationsConnection.h"
#include "KexiRelationsTableContainer.h"

class KexiRelationViewTable;

namespace KexiDB
{
	class Connection;
}

typedef QHash<QString, KexiRelationsTableContainer*> TablesHash;
typedef QMutableHashIterator<QString, KexiRelationsTableContainer*> TablesHashMutableIterator;
typedef QHash<QString, KexiRelationsTableContainer*>::ConstIterator TablesHashConstIterator;
typedef QSet<KexiRelationsConnection*> ConnectionSet;
typedef QMutableSetIterator<KexiRelationsConnection*> ConnectionSetMutableIterator;
typedef QSet<KexiRelationsConnection*>::ConstIterator ConnectionSetIterator;

//! A data structure describing connection
struct SourceConnection
{
	QString masterTable;
	QString detailsTable;
	QString masterField;
	QString detailsField;
};

/*! @short Provides a view for displaying relations between database tables.

 It is currently used for two purposes:
 - displaying global database relations
 - displaying relations defined for a database query

 The class is for displaying only - retrieving data and updating data on the backend side is implemented
 in KexiRelationWidget, and more specifically in: Kexi Relation Part and Kexi Query Part.
*/
class KEXIRELATIONSVIEW_EXPORT KexiRelationsScrollArea : public QScrollArea
{
	Q_OBJECT

	public:
		KexiRelationsScrollArea(QWidget *parent);
		virtual ~KexiRelationsScrollArea();

		//! \return a hash of added tables
		TablesHash* tables() const;

		/*! Adds a table \a t to the area. This changes only visual representation.
		 If \a rect is valid, table widget geometry will be initialized.
		 \return added table container or 0 on failure.
		 */
		KexiRelationsTableContainer* addTableContainer(KexiDB::TableSchema *t,
			const QRect &rect = QRect());

		/*! \return table container for table \a t. */
		KexiRelationsTableContainer * tableContainer(KexiDB::TableSchema *t) const;

		//! Adds a connection \a _conn to the area. This changes only visual representation.
		void addConnection(const SourceConnection& _conn /*, bool interactive=true*/);

		void setReadOnly(bool);

		KexiRelationsConnection* selectedConnection() const;

		KexiRelationsTableContainer* focusedTableContainer() const;

		virtual QSize sizeHint() const;

		const ConnectionSet* connections() const;

		//! @internal Handles mouse press event for area widget
		void handleMousePressEvent(QMouseEvent *ev);

		//! @internal Handles paint event for area widget
		void handlePaintEvent( QPaintEvent *event );

	signals:
		void tableContextMenuRequest( const QPoint& pos );
		void connectionContextMenuRequest( const QPoint& pos );
		void emptyAreaContextMenuRequest( const QPoint& pos );
		void tableViewGotFocus();
		void connectionViewGotFocus();
		void emptyAreaGotFocus();
		void tableHidden(KexiDB::TableSchema& t);
		void tablePositionChanged(KexiRelationsTableContainer*);
		void aboutConnectionRemove(KexiRelationsConnection*);

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

//unused		void slotTableScrolling(const QString&);

		//! removes selected table or connection
		void removeSelectedObject();

	protected slots:
		void containerMoved(KexiRelationsTableContainer *c);
		void slotListUpdate(QObject *s);
		void slotTableViewEndDrag();
		void slotTableViewGotFocus();
		void slotAutoScrollTimeout();

	protected:
		//! Reimplemented to draw connections.
	//	virtual void paintEvent( QPaintEvent *event );
//Qt 4		void drawContents(QPainter *p, int cx, int cy, int cw, int ch);
		void contentsMousePressEvent(QMouseEvent *ev);
		virtual void keyPressEvent(QKeyEvent *ev);
		virtual void contextMenuEvent(QContextMenuEvent* event);

//unused		void recalculateSize(int width, int height);
//unused		void stretchExpandSize();

		void hideTable(KexiRelationsTableContainer* tableView);
		void removeConnection(KexiRelationsConnection *conn);

		//! Removes current value of iterator \a it, also deleted the container object.
		void hideTableInternal(TablesHashMutableIterator& it);

		//! Removes current value of iterator \a it, also deleted the connection object.
		void removeConnectionInternal(ConnectionSetMutableIterator& it);

	private:
		class Private;
		Private* const d;
};

#endif

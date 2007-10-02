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

#ifndef KexiRelationsView_H
#define KexiRelationsView_H

#include <KexiView.h>
#include "KexiRelationsScrollArea.h"

class KComboBox;
class KPushButton;
class KMenu;
class KAction;
class Q3ListViewItem;


namespace KexiDB
{
	class Connection;
	class TableSchema;
	class TableOrQuerySchema;
}

//! A Kexi view for displaying relationships.
/*! It is used for within Query Designer
 Note: it will be also reused in Database Relationships view. */
class KEXIRELATIONSVIEW_EXPORT KexiRelationsView : public KexiView
{
	Q_OBJECT

	public:
		KexiRelationsView(QWidget *parent);
		virtual ~KexiRelationsView();

		//! \return a hash of added tables
		TablesHash* tables() const;
		
		KexiRelationsTableContainer* table(const QString& name) const;

		const ConnectionSet* connections() const;

		void addTable(const QString& t);

		virtual QSize sizeHint() const;

		/*! Used to add newly created object information to the combo box. */
		void objectCreated(const QString &mime, const QString& name);

		void objectDeleted(const QString &mime, const QString& name);
		
		void objectRenamed(const QString &mime, const QString& name, const QString& newName);

	signals:
		void tableAdded(KexiDB::TableSchema& t);
		void tableHidden(KexiDB::TableSchema& t);
		void tablePositionChanged(KexiRelationsTableContainer*);
		void aboutConnectionRemove(KexiRelationsConnection*);
		void appendFields( KexiDB::TableOrQuerySchema& tableOrQuery, const QStringList& fieldNames );
	
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
		void appendSelectedFields();
		void openSelectedTable();
		void designSelectedTable();
		void slotTableHidden(KexiDB::TableSchema &table);
		void aboutToShowPopupMenu();

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
		class Private;
		Private* const d;
};

#endif

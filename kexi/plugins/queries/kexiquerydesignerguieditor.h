/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIQUERYDESIGNERGUIEDITOR_H
#define KEXIQUERYDESIGNERGUIEDITOR_H

#include <qguardedptr.h>
#include <qsplitter.h>

#include <kexiviewbase.h>
#include <kexipropertybuffer.h>
#include <kexiquerypart.h>

class KexiMainWindow;
class KexiTableViewData;
class KexiDataTable;
class KexiTableItem;
class KexiRelationWidget;
class KexiSectionHeader;
class KexiTableViewPropertyBuffer;
class KexiRelationViewTableContainer;
class KexiRelationViewConnection;

namespace KexiPart
{
	class Item;
}

namespace KexiDB
{
	class Connection;
	class QuerySchema;
	class TableSchema;
	class ResultInfo;
};

class KexiQueryDesignerGuiEditor : public KexiViewBase
{
	Q_OBJECT

	public:
		KexiQueryDesignerGuiEditor(KexiMainWindow *mainWin, QWidget *parent, const char *name = 0);
		~KexiQueryDesignerGuiEditor();

//		KexiDB::QuerySchema	*schema();

		KexiRelationWidget *relationView() const;

		virtual QSize sizeHint() const;

	protected:
		void initTable();
		void addRow(const QString &tbl, const QString &field);
//		void			restore();
		virtual bool beforeSwitchTo(int mode, bool &cancelled, bool &dontStore);
		virtual bool afterSwitchFrom(int mode, bool &cancelled);

		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata);
		virtual bool storeData();

		/*! Updates data in columns depending on tables that are currently inserted.
		 Tabular Data in combo box popups is updated as well. */
		void updateColumsData();

		/*! \return property buffer associated with currently selected row (i.e. field)
		 or 0 if current row is empty. */
		virtual KexiPropertyBuffer *propertyBuffer();

		KexiPropertyBuffer* createPropertyBuffer( int row, 
			const QString& tableName, const QString& fieldName, bool newOne = false );

		/*! Builds query schema out of information provided by gui. 
		 The schema is stored in temp->query member. */
		void buildSchema();

		KexiQueryPart::TempData * tempData();

		/*! Helper: allocates and initializes new GUI table's row. Doesn't insert it, just returns. */
		KexiTableItem* createNewRow(const QString& tableName, const QString& fieldName) const;

	protected slots:
		void slotDragOverTableRow(KexiTableItem *item, int row, QDragMoveEvent* e);
		void slotDroppedAtRow(KexiTableItem *item, int row, 
			QDropEvent *ev, KexiTableItem*& newItem);
		void slotTableAdded(KexiDB::TableSchema &t);
		void slotTableHidden(KexiDB::TableSchema &t);

		//! Called before cell change in tableview.
		void slotBeforeCellChanged(KexiTableItem *item, int colnum, 
			QVariant newValue, KexiDB::ResultInfo* result);

		void slotRowInserted(KexiTableItem* item, uint row);
		void slotTablePositionChanged(KexiRelationViewTableContainer*);
		void slotAboutConnectionRemove(KexiRelationViewConnection*);
		void slotTableFieldDoubleClicked( KexiDB::TableSchema* table, const QString& fieldName );

		/*! Loads layout of relation GUI diagram. */
		bool loadLayout();

	private:
		KexiTableViewData *m_data;
		KexiDataTable *m_dataTable;
		QGuardedPtr<KexiDB::Connection> m_conn;

		KexiRelationWidget *m_relations;
		KexiSectionHeader *m_head;
		QSplitter *m_spl;

		//! used to remember in slotDroppedAtRow() what data was dropped, 
		//! so we can create appropriate prop. buffer in slotRowInserted()
		KexiTableViewData *m_fieldColumnData, *m_tablesColumnData;
		KexiTableViewPropertyBuffer* m_buffers;
		KexiTableItem *m_droppedNewItem;

		QString m_droppedNewTable, m_droppedNewField;
};

#endif


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
#include "kexiquerypart.h"

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

class KexiQueryDesignerGuiEditorPrivate;

class KexiQueryDesignerGuiEditor : public KexiViewBase
{
	Q_OBJECT

	public:
		KexiQueryDesignerGuiEditor(KexiMainWindow *mainWin, QWidget *parent, const char *name = 0);
		virtual ~KexiQueryDesignerGuiEditor();

//		KexiDB::QuerySchema	*schema();

		KexiRelationWidget *relationView() const;

		virtual QSize sizeHint() const;

	public slots:
		virtual void setFocus();

	protected:
		void initTableColumns(); //!< Called just once.
		void initTableRows(); //!< Called to have all rows empty.
		void addRow(const QString &tbl, const QString &field);
//		void			restore();
		virtual tristate beforeSwitchTo(int mode, bool &dontStore);
		virtual tristate afterSwitchFrom(int mode);

		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);
		virtual tristate storeData();

		/*! Updates data in columns depending on tables that are currently inserted.
		 Tabular Data in combo box popups is updated as well. */
		void updateColumnsData();

		/*! \return property buffer associated with currently selected row (i.e. field)
		 or 0 if current row is empty. */
		virtual KexiPropertyBuffer *propertyBuffer();

		KexiPropertyBuffer* createPropertyBuffer( int row, 
			const QString& tableName, const QString& fieldName, bool newOne = false );

		/*! Builds query schema out of information provided by gui. 
		 The schema is stored in temp->query member. 
		 \a errMsg is optional error message returned. 
		 \return true on proper schema creation. */
		bool buildSchema(QString *errMsg = 0);

		KexiQueryPart::TempData * tempData() const;

		/*! Helper: allocates and initializes new GUI table's row. Doesn't insert it, just returns. */
		KexiTableItem* createNewRow(const QString& tableName, const QString& fieldName) const;

		KexiDB::BaseExpr* parseCriteriaString(
			const QString& columnName, const QString& fullString);

	protected slots:
		void slotDragOverTableRow(KexiTableItem *item, int row, QDragMoveEvent* e);
		void slotDroppedAtRow(KexiTableItem *item, int row, 
			QDropEvent *ev, KexiTableItem*& newItem);
		void slotTableAdded(KexiDB::TableSchema &t);
		void slotTableHidden(KexiDB::TableSchema &t);

		//! Called before cell change in tableview.
		void slotBeforeCellChanged(KexiTableItem *item, int colnum, 
			QVariant newValue, KexiDB::ResultInfo* result);

		void slotRowInserted(KexiTableItem* item, uint row, bool repaint);
		void slotTablePositionChanged(KexiRelationViewTableContainer*);
		void slotAboutConnectionRemove(KexiRelationViewConnection*);
		void slotTableFieldDoubleClicked( KexiDB::TableSchema* table, const QString& fieldName );

		/*! Loads layout of relation GUI diagram. */
		bool loadLayout();

		/*! Stores layout of relation GUI diagram. */
		bool storeLayout();

		void showTablesAndConnectionsForQuery(KexiDB::QuerySchema *query);
		void showFieldsForQuery(KexiDB::QuerySchema *query);

		void slotPropertyChanged(KexiPropertyBuffer &buf, KexiProperty &property);

		void slotTableCreated(KexiDB::TableSchema& schema);

	private:
		KexiQueryDesignerGuiEditorPrivate *d;
};

#endif


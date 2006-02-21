/* This file is part of the KDE project
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIALTERTABLEDIALOG_H
#define KEXIALTERTABLEDIALOG_H

#include <kexidatatable.h>
#include "kexitablepart.h"

class KexiTableItem;
class KexiAlterTableDialogPrivate;

namespace KoProperty {
	class Set;
	class Property;
}

class KexiAlterTableDialog : public KexiDataTable
{
	Q_OBJECT

	public:
		/*! Creates a new alter table dialog. */
		KexiAlterTableDialog(KexiMainWindow *win, QWidget *parent,
			const char *name = 0);

		virtual ~KexiAlterTableDialog();

		KexiTablePart::TempData* tempData() const;

//		virtual QWidget* mainWidget();
//		KexiDataTableView* tableView() const { return m_view; }

//		virtual QSize minimumSizeHint() const;
//		virtual QSize sizeHint() const;

	protected slots:
		/*! Equivalent to updateActions(false). Called on row insert/delete
		 in a KexiDataAwarePropertySet. */
		void updateActions();

//		void slotCellSelected(int col, int row);
		virtual void slotUpdateRowActions(int row);

		//! Called before cell change in tableview.
		void slotBeforeCellChanged(KexiTableItem *item, int colnum,
			QVariant& newValue, KexiDB::ResultInfo* result);

		//! Called on row change in a tableview.
		void slotRowUpdated(KexiTableItem *item);

		//! Called before row inserting in tableview.
		void slotAboutToInsertRow(KexiTableItem* item, KexiDB::ResultInfo* result, bool repaint);

		//! Called before row deleting in tableview.
		void slotAboutToDeleteRow(KexiTableItem& item, KexiDB::ResultInfo* result, bool repaint);

		/*! Called after any property has been changed in the current property set,
		 to perform some actions (like updating other dependent properties) */
		void slotPropertyChanged(KoProperty::Set& set, KoProperty::Property& property);

		/*! Toggles primary key for currently selected field.
		 Does nothing for empty row. */
		void slotTogglePrimaryKey();

/*		//! Called before row updating in tableview.
		void slotAboutToUpdateRow(KexiTableItem* item,
			KexiDB::RowEditBuffer* buffer, KexiDB::ResultInfo* result);
*/
//		void slotEmptyRowInserted(KexiTableItem*, uint index);

	protected:
		virtual void updateActions(bool activated);

		//! called whenever data should be reloaded (on switching to this view mode)
		void initData();

		/*! Creates a new property set for \a field.
		 The property set will be asigned to \a row, and owned by this dialog.
		 If \a newOne is true, the property set will be marked as newly created.
		 \return newly created property set. */
		KoProperty::Set* createPropertySet( int row, KexiDB::Field *field, bool newOne = false );

		virtual tristate beforeSwitchTo(int mode, bool &dontStore);
		virtual tristate afterSwitchFrom(int mode);

		/*! \return property set associated with currently selected row (i.e. field)
		 or 0 if current row is empty. */
		virtual KoProperty::Set *propertySet();

		void removeCurrentPropertySet();

		/*! Reimplemented from KexiViewBase, because tables creation is more complex. */
		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);

		/*! Reimplemented from KexiViewBase, because table storage is more complex. */
		virtual tristate storeData(bool dontAsk = false);

		tristate buildSchema(KexiDB::TableSchema &schema);

		QString messageForSavingChanges(bool &emptyTable);

		/*! Helper, used for slotTogglePrimaryKey() and slotPropertyChanged().
		 Assigns primary key icon and value for property set \a propertySet, 
		 and deselects it from previous pkey's row. 
		 \a aWasPKey is internal. */
		void setPrimaryKey(KoProperty::Set &propertySet, bool set, bool aWasPKey = false);

		//! Gets subtype strings and names for type \a fieldType.
		void getSubTypeListData(KexiDB::Field::TypeGroup fieldTypeGroup, 
			QStringList& stringsList, QStringList& namesList);

	private:
		KexiAlterTableDialogPrivate *d;
};

#endif


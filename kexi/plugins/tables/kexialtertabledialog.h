/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIALTERTABLEDIALOG_H
#define KEXIALTERTABLEDIALOG_H

#include <qptrvector.h>
#include <kexidatatable.h>

class KPopupMenu;
class KexiMainWindow;
class KexiTableView;
class KexiTableItem;
class KexiPropertyEditor;
class KexiProperty;
class KexiPropertyBuffer;

namespace KexiDB
{
	class Field;
	class Cursor;
	class TableSchema;
	class RowEditBuffer;
	class ResultInfo;
}

class KexiAlterTableDialog : public KexiDataTable
{
	Q_OBJECT

	typedef QPtrVector<KexiPropertyBuffer> FieldsBuffer;

	public:
		/*! Creates alter table dialog. \a table may be NULL 
		 if new table should be composed instead of altering existing one. */
		KexiAlterTableDialog(KexiMainWindow *win, QWidget *parent, 
			KexiDB::TableSchema *table, const char *name = 0);

		~KexiAlterTableDialog();

//		virtual QWidget* mainWidget();
//		KexiDataTableView* tableView() const { return m_view; }

//		virtual QSize minimumSizeHint() const;
//		virtual QSize sizeHint() const;


	protected:
		void init();
		void initActions();

		/*! Creates a new property buffer for \a field. 
		 The buffer will be asigned to \a row, and owned by this dialog. 
		 \return newly created property buffer. */
		KexiPropertyBuffer * createPropertyBuffer( int row, KexiDB::Field *field );

		virtual bool beforeSwitchTo(int mode, bool &cancelled);

		/*! \return property buffer associated with currently selected row (i.e. field)
		 or 0 if current row is empty. */
		virtual KexiPropertyBuffer *propertyBuffer();

		void removeCurrentPropertyBuffer();

		/*! Reimplemented from KexiViewBase, because tables creation is more complex. */
		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata);

		/*! Reimplemented from KexiViewBase, because table storage is more complex. */
		virtual bool storeData();

	protected slots:
		void slotCellSelected(int col, int row);
//		void slotUpdateRowActions(int row);

		//! Called on property changes in property editor.
		void slotPropertyChanged(KexiPropertyBuffer& buf,KexiProperty& prop);

		//! Called before cell change in tableview.
		void slotBeforeCellChanged(KexiTableItem *item, int colnum, 
			QVariant newValue, KexiDB::ResultInfo* result);

		//! Called on row change in a tableview.
		void slotRowUpdated(KexiTableItem *item);

		//! Called before row inserting in tableview.
		void slotAboutToInsertRow(KexiTableItem* item, KexiDB::ResultInfo* result);

/*		//! Called before row updating in tableview.
		void slotAboutToUpdateRow(KexiTableItem* item, 
			KexiDB::RowEditBuffer* buffer, KexiDB::ResultInfo* result);
*/

		//! Called on row delete in a tableview.
		void slotRowDeleted();

		void slotEmptyRowInserted(KexiTableItem*, uint index);

	private:
//		KexiTableView *m_view;
		const KexiDB::TableSchema *m_table; //!< original table schema
		KexiDB::TableSchema *m_newTable; //!< new table schema
//		KexiPropertyEditor *m_properties;
		FieldsBuffer m_buffers; //!< buffer
//		QPtrDict<KexiDB::Field> m_newFields; //!< newly created fields 
//		                                     //!< assigned for property buffers
		int m_row; //!< used to know if a new row is selected in slotCellSelected()
		bool m_currentBufferCleared : 1;
};

#endif


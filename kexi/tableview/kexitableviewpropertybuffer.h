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

#ifndef KEXITABLEVIEWPROPERTYBUFFER_H
#define KEXITABLEVIEWPROPERTYBUFFER_H

#include <qguardedptr.h>
#include "kexipropertybuffer.h"

class KexiViewBase;
class KexiTableItem;
class KexiTableView;
class KexiTableViewData;

/*! This helper class handles data changes of a single KexiTableView object inside 
 of KexiViewBase object.

 It is currently used in KexiAlterTableDialog and KexiQueryDesignerGuiEditor, 
 and may be used for similar purposes, when each KexiTableView's row can be associated 
 with single KexiPropertyBuffer object, and given KexiTableView object has to inform 
 the world about currently selected row/buffer.

 Following functionality is built-in:
 - auto-initializing after resetting of table view's data
 - destroying single buffer that is associated with deleted row
 - inserting single buffer that and associating it with new row
 - setting view's 'dirty' flag when needed
 - signalling via KexiVieBase::propertyBufferSwitched() that current property 
   buffer has changed (e.g. on moving to other row)
*/
class KEXIDATATABLE_EXPORT KexiTableViewPropertyBuffer : public QObject
{
	Q_OBJECT

	public:
		/*! You can instantiate KexiTableViewPropertyBuffer object 
		 for existing \a tableView and \a view. \a tableView can have data assigned 
		 (KexiTableView::setData()) now but it can be done later as well 
		 (but assigning data is needed for proper functionality).
		 Any changed reassignments of table view's data will be handled automatically. */
		KexiTableViewPropertyBuffer(KexiViewBase *view, KexiTableView* tableView);

		virtual ~KexiTableViewPropertyBuffer();

		uint size() const;
		KexiPropertyBuffer* currentPropertyBuffer() const;
		KexiPropertyBuffer* at(uint row) const { return m_buffers[row]; }

	public slots:
		void currentRowDeleted();
		void removeCurrentPropertyBuffer();
		void clear(uint minimumSize = 0);
		/*! Inserts \a buf buffer at \a row position. 
		 If there was a buffer at this position before, it will be destroyed. 
		 If \a newOne is true, the property buffer will be marked as newly created, 
		 simply by adding "newrow" property. */
		void insert(uint row, KexiPropertyBuffer* buf, bool newOne = false);

		void remove(uint row);

	protected slots:
		/*! Handles table view's data source changes. */
		void slotDataSet( KexiTableViewData *data );

		//! Called on row delete in a tableview.
		void slotRowDeleted();

		//! Called on multiple rows delete in a tableview.
		void slotRowsDeleted( const QValueList<int> &rows );

		//! Called on \a row insertion in a tableview.
		void slotRowInserted(KexiTableItem* item, uint row);

		//! Called on selecting another cell in a tableview.
		void slotCellSelected(int, int row);

	protected:
		KexiPropertyBuffer::Vector m_buffers; //!< prop. buffers vector

		QGuardedPtr<KexiViewBase> m_view;
		QGuardedPtr<KexiTableView> m_tableView;
		QGuardedPtr<KexiTableViewData> m_currentTVData;

		int m_row; //!< used to know if a new row is selected in slotCellSelected()
};

#endif


/* This file is part of the KDE project
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDATAAWAREPROPERTYSET_H
#define KEXIDATAAWAREPROPERTYSET_H

#include <qpointer.h>
#include <q3ptrvector.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <koproperty/set.h>

typedef Q3PtrVector<KoProperty::Set> SetVector;

class KexiViewBase;
class KexiTableItem;
class KexiTableViewData;
class KexiDataAwareObjectInterface;

/*! This helper class handles data changes of a single
 object implementing KexiDataAwareObjectInterface (e.g. KexiTableView) inside
 a KexiViewBase container.

 It is currently used in KexiAlterTableDialog and KexiQueryDesignerGuiEditor,
 and may be used for similar purposes, when each KexiDataAwareObjectInterface's
 row can be associated with single KoProperty::Set object, and given
 KexiDataAwareObjectInterface object has to inform the world about currently
 selected row/property set.

 Following functionality is built-in:
 - auto-initializing after resetting of table view's data
 - destroying single property set that is associated with deleted row
 - inserting single property set that and associating it with new row
 - all property sets are cleared when view's data is cleared (using clear())
 - setting view's 'dirty' flag when needed
 - signalling via KexiViewBase::propertySetSwitched() that current property
   set has changed (e.g. on moving to other row)
*/
class KEXIDATATABLE_EXPORT KexiDataAwarePropertySet : public QObject
{
	Q_OBJECT

	public:
		/*! You can instantiate KexiDataAwarePropertySet object
		 for existing \a tableView and \a view. \a tableView can have data assigned
		 (KexiDataAwareObjectInterface::setData()) now but it can be done later as well
		 (but assigning data is needed for proper functionality).
		 Any changed reassignments of table view's data will be handled automatically. */
		KexiDataAwarePropertySet(KexiViewBase *view, KexiDataAwareObjectInterface* dataObject);

		virtual ~KexiDataAwarePropertySet();

		uint size() const;

		KoProperty::Set* currentPropertySet() const;

		uint currentRow() const;

		inline KoProperty::Set* at(uint row) const { return m_sets[row]; }

		/*! \return a pointer to property set assigned for \a item or null if \a item has no
		 property set assigned or it's not owned by assigned table view or
		 if assigned table view has no data set. */
		KoProperty::Set* findPropertySetForItem(KexiTableItem& item);

		/*! \return number of the first row containing \a propertyName property equal to \a value.
		 This is used e.g. in the Table Designer to find a row by field name. 
		 If no such row has been found, -1 is returned. */
		int findRowForPropertyValue(const QCString& propertyName, const QVariant& value);

	signals:
		/*! Emmited when row is deleted.
		 KexiDataAwareObjectInterface::rowDeleted() signal is usually used but when you're using
		 KexiDataAwarePropertySet, you never know if currentPropertySet() is updated.
		 So use this signal instead. */
		void rowDeleted();

		/*! Emmited when row is inserted.
		 Purpose of this signal is similar to rowDeleted() signal. */
		void rowInserted();

	public slots:
		void removeCurrentPropertySet();

		void clear(uint minimumSize = 0);

		/*! Inserts \a set property set at \a row position.
		 If there was a buffer at this position before, it will be destroyed.
		 If \a newOne is true, the property set will be marked as newly created,
		 simply by adding "newrow" property.

		 The property set \a set will be owned by this object, so you should not
		 delete this property set by hand but call removeCurrentPropertySet()
		 or remove(uint) instead.
		 Note that property set's parent (QObject::parent()) must be null
		 or qual to this KexiDataAwarePropertySet object, otherwise this method
		 will fail with a warning.
		*/
		void insert(uint row, KoProperty::Set* set, bool newOne = false);

		/*! Removes a property set at \a row position. */
		void remove(uint row);

	protected slots:
		/*! Handles table view's data source changes. */
		void slotDataSet( KexiTableViewData *data );

		//! Called on row delete in a tableview.
		void slotRowDeleted();

		//! Called on multiple rows delete in a tableview.
		void slotRowsDeleted( const Q3ValueList<int> &rows );

		//! Called on \a row insertion in a tableview.
		void slotRowInserted(KexiTableItem* item, uint row, bool repaint);

		//! Called on selecting another cell in a tableview.
		void slotCellSelected(int, int row);

		//! Called on clearing tableview's data: just clears all property sets.
		void slotReloadRequested();

	protected:
		SetVector m_sets; //!< prop. sets vector

		QPointer<KexiViewBase> m_view;
		KexiDataAwareObjectInterface* m_dataObject;
//		QPointer<KexiTableView> m_tableView;
		QPointer<KexiTableViewData> m_currentTVData;

		int m_row; //!< used to know if a new row is selected in slotCellSelected()
};

#endif


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

#include "kexidataawarepropertyset.h"
#include "kexitableviewdata.h"
#include "kexidataawareobjectiface.h"

#include <koproperty/property.h>
#include <kexiviewbase.h>

#define MAX_FIELDS 1009 //nice prime number (default prop. set vector size)

KexiDataAwarePropertySet::KexiDataAwarePropertySet(KexiViewBase *view,
	KexiDataAwareObjectInterface* dataObject)
 : QObject( view, QCString(view->name())+"KexiDataAwarePropertySet" )
 , m_view(view)
 , m_dataObject(dataObject)
 , m_row(-99)
{
	m_sets.setAutoDelete(true);

//	connect(m_dataObject, SIGNAL(dataSet(KexiTableViewData*)),
//		this, SLOT(slotDataSet(KexiTableViewData*)));
	m_dataObject->connectDataSetSignal(this, SLOT(slotDataSet(KexiTableViewData*)));
//	connect(m_dataObject, SIGNAL(cellSelected(int,int)),
//		this, SLOT(slotCellSelected(int,int)));
	m_dataObject->connectCellSelectedSignal(this, SLOT(slotCellSelected(int,int)));
//
	slotDataSet( m_dataObject->data() );
	const bool wasDirty = view->dirty();
	clear();
	if (!wasDirty)
		view->setDirty(false);
}

KexiDataAwarePropertySet::~KexiDataAwarePropertySet()
{
}

void KexiDataAwarePropertySet::slotDataSet( KexiTableViewData *data )
{
	if (!m_currentTVData.isNull()) {
		m_currentTVData->disconnect( this );
		clear();
	}
	m_currentTVData = data;
	if (!m_currentTVData.isNull()) {
		connect(m_currentTVData, SIGNAL(rowDeleted()), this, SLOT(slotRowDeleted()));
		connect(m_currentTVData, SIGNAL(rowsDeleted( const QValueList<int> & )),
			this, SLOT(slotRowsDeleted( const QValueList<int> & )));
		connect(m_currentTVData, SIGNAL(rowInserted(KexiTableItem*,uint,bool)),
			this, SLOT(slotRowInserted(KexiTableItem*,uint,bool)));
		connect(m_currentTVData, SIGNAL(reloadRequested()),
			this, SLOT(slotReloadRequested()));
	}
}

void KexiDataAwarePropertySet::removeCurrentPropertySet()
{
	remove( m_dataObject->currentRow() );
}

void KexiDataAwarePropertySet::remove(uint row)
{
	KoProperty::Set *set = m_sets.at(row);
	if (!set)
		return;
	set->debug();
	m_sets.remove(row);
	m_view->setDirty();
	m_view->propertySetSwitched();
}

uint KexiDataAwarePropertySet::size() const
{
	return m_sets.size();
}

void KexiDataAwarePropertySet::clear(uint minimumSize)
{
	m_sets.clear();
	m_sets.resize(QMAX(minimumSize, MAX_FIELDS));
	m_view->setDirty(true);
	m_view->propertySetSwitched();
}

void KexiDataAwarePropertySet::slotReloadRequested()
{
	clear();
}

void KexiDataAwarePropertySet::insert(uint row, KoProperty::Set* set, bool newOne)
{
	if (!set || row >= m_sets.size()) {
		kexiwarn << "KexiDataAwarePropertySet::insert() invalid args: rew="<< row<< " propertyset="<< set<< endl;
		return;
	}
	if (set->parent() && set->parent()!=this) {
		kexiwarn << "KexiDataAwarePropertySet::insert() propertyset's parent must be NULL or this KexiDataAwarePropertySet" << endl;
		return;
	}

	m_sets.insert(row, set);

	connect(set, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)), m_view, SLOT(setDirty()));

	if (newOne) {
		//add a special property indicating that this is brand new set,
		//not just changed
		KoProperty::Property* prop = new KoProperty::Property("newrow");
		prop->setVisible(false);
		set->addProperty( prop );
		m_view->setDirty();
	}
}

KoProperty::Set* KexiDataAwarePropertySet::currentPropertySet() const
{
	return (m_dataObject->currentRow() >= 0) ? m_sets.at( m_dataObject->currentRow() ) : 0;
}

uint KexiDataAwarePropertySet::currentRow() const
{
	return m_dataObject->currentRow();
}

void KexiDataAwarePropertySet::slotRowDeleted()
{
	m_view->setDirty();
	removeCurrentPropertySet();

	//let's move up all property sets that are below that deleted
	m_sets.setAutoDelete(false);//to avoid auto deleting in insert()
	const int r = m_dataObject->currentRow();
	for (int i=r;i<int(m_sets.size()-1);i++) {
		KoProperty::Set *set = m_sets[i+1];
		m_sets.insert( i , set );
	}
	m_sets.insert( m_sets.size()-1, 0 );
	m_sets.setAutoDelete(true);//revert the flag

	m_view->propertySetSwitched();
	emit rowDeleted();
}

void KexiDataAwarePropertySet::slotRowsDeleted( const QValueList<int> &rows )
{
	//let's move most property sets up & delete unwanted
	m_sets.setAutoDelete(false);//to avoid auto deleting in insert()
	const int orig_size = size();
	int prev_r = -1;
	int num_removed = 0, cur_r = -1;
	for (QValueList<int>::ConstIterator r_it = rows.constBegin(); r_it!=rows.constEnd() && *r_it < orig_size; ++r_it) {
		cur_r = *r_it;// - num_removed;
		if (prev_r>=0) {
//			kdDebug() << "move " << prev_r+num_removed-1 << ".." << cur_r-1 << " to " << prev_r+num_removed-1 << ".." << cur_r-2 << endl;
			int i=prev_r;
			KoProperty::Set *set = m_sets.take(i+num_removed);
			kdDebug() << "property set " << i+num_removed << " deleted" << endl;
			delete set;
			num_removed++;
			for (; (i+num_removed)<cur_r; i++) {
				m_sets.insert( i, m_sets[i+num_removed] );
				kdDebug() << i << " <- " << i+num_removed << endl;
			}
		}
		prev_r = cur_r - num_removed;
	}
	//move remaining property sets up
	if (cur_r>=0) {
		KoProperty::Set *set = m_sets.take(cur_r);
		kdDebug() << "property set " << cur_r << " deleted" << endl;
		delete set;
		num_removed++;
		for (int i=prev_r; (i+num_removed)<orig_size; i++) {
			m_sets.insert( i, m_sets[i+num_removed] );
			kdDebug() << i << " <- " << i+num_removed << endl;
		}
	}
	//finally: clear last rows
	for (int i=orig_size-num_removed; i<orig_size; i++) {
		kdDebug() << i << " <- zero" << endl;
		m_sets.insert( i, 0 );
	}
	m_sets.setAutoDelete(true);//revert the flag

	if (num_removed>0)
		m_view->setDirty();
	m_view->propertySetSwitched();
}

//void KexiDataAwarePropertySet::slotEmptyRowInserted(KexiTableItem*, uint /*index*/)
void KexiDataAwarePropertySet::slotRowInserted(KexiTableItem*, uint row, bool /*repaint*/)
{
	m_view->setDirty();

	//let's move down all property set that are below
	m_sets.setAutoDelete(false);//to avoid auto deleting in insert()
//	const int r = m_dataObject->currentRow();
	m_sets.resize(m_sets.size()+1);
	for (int i=int(m_sets.size())-1; i>(int)row; i--) {
		KoProperty::Set *set = m_sets[i-1];
		m_sets.insert( i , set );
	}
	m_sets.insert( row, 0 );
	m_sets.setAutoDelete(true);//revert the flag

	m_view->propertySetSwitched();

	emit rowInserted();
}

void KexiDataAwarePropertySet::slotCellSelected(int, int row)
{
	if(row == m_row)
		return;
	m_row = row;
	m_view->propertySetSwitched();
}

KoProperty::Set* KexiDataAwarePropertySet::findPropertySetForItem(KexiTableItem& item)
{
	if (m_currentTVData.isNull())
		return 0;
	int idx = m_currentTVData->findRef(&item);
	if (idx<0)
		return 0;
	return m_sets[idx];
}

int KexiDataAwarePropertySet::findRowForPropertyValue(const QCString& propertyName, const QVariant& value)
{
	const int size = m_sets.size();
	for (int i=0; i<size; i++) {
		KoProperty::Set *set = m_sets[i];
		if (!set || !set->contains(propertyName))
			continue;
		if (set->property(propertyName).value() == value)
			return i;
	}
	return -1;
}

#include "kexidataawarepropertyset.moc"

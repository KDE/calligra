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

#include <kexi_global.h>
#include <koproperty/property.h>
#include <KexiView.h>

#define MAX_FIELDS 101 //nice prime number (default prop. set vector size)

KexiDataAwarePropertySet::KexiDataAwarePropertySet(KexiView *view,
	KexiDataAwareObjectInterface* dataObject)
 : QObject( view )
 , m_view(view)
 , m_dataObject(dataObject)
 , m_row(-99)
{
	setObjectName(view->objectName()+"_KexiDataAwarePropertySet");

//	connect(m_dataObject, SIGNAL(dataSet(KexiTableViewData*)),
//		this, SLOT(slotDataSet(KexiTableViewData*)));
	m_dataObject->connectDataSetSignal(this, SLOT(slotDataSet(KexiTableViewData*)));
//	connect(m_dataObject, SIGNAL(cellSelected(int,int)),
//		this, SLOT(slotCellSelected(int,int)));
	m_dataObject->connectCellSelectedSignal(this, SLOT(slotCellSelected(int,int)));
//
	slotDataSet( m_dataObject->data() );
	const bool wasDirty = view->isDirty();
	clear();
	if (!wasDirty)
		view->setDirty(false);
}

KexiDataAwarePropertySet::~KexiDataAwarePropertySet()
{
	qDeleteAll(m_sets);
	m_sets.clear();
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
		connect(m_currentTVData, SIGNAL(rowsDeleted( const QList<int> & )),
			this, SLOT(slotRowsDeleted( const QList<int> & )));
		connect(m_currentTVData, SIGNAL(rowInserted(KexiDB::RecordData*,uint,bool)),
			this, SLOT(slotRowInserted(KexiDB::RecordData*,uint,bool)));
		connect(m_currentTVData, SIGNAL(reloadRequested()),
			this, SLOT(slotReloadRequested()));
	}
}

void KexiDataAwarePropertySet::eraseCurrentPropertySet()
{
	eraseAt( m_dataObject->currentRow() );
}

void KexiDataAwarePropertySet::eraseAt(uint row)
{
	KoProperty::Set *set = m_sets.at(row);
	if (!set)
		return;
	m_sets[row] = 0;
	set->debug();
	delete set;
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
	m_sets.resize(qMax(minimumSize, (uint)MAX_FIELDS));
	m_view->setDirty(true);
	m_view->propertySetSwitched();
}

void KexiDataAwarePropertySet::slotReloadRequested()
{
	clear();
}

void KexiDataAwarePropertySet::set(uint row, KoProperty::Set* set, bool newOne)
{
	if (!set || row >= (uint)m_sets.size()) {
		kexiwarn << "KexiDataAwarePropertySet::set() invalid args: rew="<< row<< " propertyset="<< set<< endl;
		return;
	}
	if (set->parent() && set->parent()!=this) {
		kexiwarn << "KexiDataAwarePropertySet::set() propertyset's parent must be NULL or this KexiDataAwarePropertySet" << endl;
		return;
	}

	m_sets[row] = set;

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

/* Qt4 : not needed
	removeCurrentPropertySet();

	//let's move up all property sets that are below that deleted
	const int r = m_dataObject->currentRow();
	const int size = int(m_sets.size()) - 1;
	for (int i = r; i<size; i++)
		m_sets[ i ] = m_sets.at(i+1);
	m_sets[ size ] = 0;*/

	// It's simplified in Qt4:
	m_sets.remove( m_dataObject->currentRow() );

	m_view->propertySetSwitched();
	emit rowDeleted();
}

void KexiDataAwarePropertySet::slotRowsDeleted( const QList<int> &rows )
{
	//let's move most property sets up & delete unwanted
	const int orig_size = size();
	int prev_r = -1;
	int num_removed = 0, cur_r = -1;
	for (QList<int>::ConstIterator r_it = rows.constBegin(); r_it!=rows.constEnd() && *r_it < orig_size; ++r_it) {
		cur_r = *r_it;
		if (prev_r>=0) {
//			kDebug() << "move " << prev_r+num_removed-1 << ".." << cur_r-1 << " to " << prev_r+num_removed-1 << ".." << cur_r-2 << endl;
			int i=prev_r;
			KoProperty::Set *set = m_sets.at(i+num_removed);
//Qt4			if (set)
			m_sets.remove(i+num_removed);
			kDebug() << "property set " << i+num_removed << " deleted" << endl;
			delete set;
			num_removed++;
/* Qt4 : not needed
			for (; (i+num_removed)<cur_r; i++) {
				m_sets[ i ] = m_sets.at(i+num_removed);
				kDebug() << i << " <- " << i+num_removed << endl;
			}*/
		}
		prev_r = cur_r - num_removed;
	}
/* Qt4 : not needed
	//move remaining buffers up
	if (cur_r>=0) {
		KoProperty::Set *set = m_sets.at(cur_r);
		if (set)
			m_sets.remove(cur_r);
		kDebug() << "property set " << cur_r << " deleted" << endl;
		delete set;
		num_removed++;
		for (int i=prev_r; (i+num_removed)<orig_size; i++) {
			m_sets[ i ] = m_sets.at(i+num_removed);
			kDebug() << i << " <- " << i+num_removed << endl;
		}
	}
	*/
/* Qt4 : not needed
	//finally: clear last rows
	for (int i=orig_size-num_removed; i<orig_size; i++) {
		kDebug() << i << " <- zero" << endl;
		m_sets[ i ] = 0;
	}
	for (int i=orig_size-num_removed; i<orig_size; i++) {
		kDebug() << i << " <- zero" << endl;
		m_sets[ i ] = 0;
	}*/
	//(Qt4) finally: add empty rows
	m_sets.insert( size(), num_removed, 0 );

	if (num_removed > 0)
		m_view->setDirty();
	m_view->propertySetSwitched();
}

void KexiDataAwarePropertySet::slotRowInserted(KexiDB::RecordData*, uint pos, bool /*repaint*/)
{
	m_view->setDirty();

	//let's move down all property set that are below
//	const int r = m_dataObject->currentRow();
/* Qt4 : not needed
	m_sets.resize(m_sets.size()+1);
	for (int i=int(m_sets.size())-1; i>(int)row; i--)
		m_sets[i] = m_sets.at(i-1);
	m_sets[row] = 0;*/

	// It's simplified in Qt4:
	m_sets.insert(pos, 0);
	
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

KoProperty::Set* KexiDataAwarePropertySet::findPropertySetForItem(KexiDB::RecordData& record)
{
	if (m_currentTVData.isNull())
		return 0;
	int idx = m_currentTVData->indexOf(&record);
	if (idx<0)
		return 0;
	return m_sets.at(idx);
}

int KexiDataAwarePropertySet::findRowForPropertyValue(
	const QByteArray& propertyName, const QVariant& value)
{
	const int size = m_sets.size();
	for (int i=0; i<size; i++) {
		KoProperty::Set *set = m_sets.at(i);
		if (!set || !set->contains(propertyName))
			continue;
		if (set->property(propertyName).value() == value)
			return i;
	}
	return -1;
}

#include "kexidataawarepropertyset.moc"

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexitableviewpropertybuffer.h"
#include "kexitableviewdata.h"
#include "kexidataawareobjectiface.h"

#include <kexiviewbase.h>

#define MAX_FIELDS 101 //nice prime number (default buffer vector size)

KexiDataAwarePropertyBuffer::KexiDataAwarePropertyBuffer(KexiViewBase *view, 
	KexiDataAwareObjectInterface* dataObject)
 : QObject( view, QCString(view->name())+"KexiDataAwarePropertyBuffer" )
 , m_view(view)
 , m_dataObject(dataObject)
 , m_row(-99)
{
	m_buffers.setAutoDelete(true);

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

KexiDataAwarePropertyBuffer::~KexiDataAwarePropertyBuffer()
{
}

void KexiDataAwarePropertyBuffer::slotDataSet( KexiTableViewData *data )
{
	if (!m_currentTVData.isNull()) {
		m_currentTVData->disconnect( this );
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

void KexiDataAwarePropertyBuffer::removeCurrentPropertyBuffer()
{
	remove( m_dataObject->currentRow() );
}

void KexiDataAwarePropertyBuffer::remove(uint row)
{
	KexiPropertyBuffer *buf = m_buffers.at(row);
	if (!buf)
		return;
	buf->debug();
	m_buffers.remove(row);
	m_view->setDirty();
	m_view->propertyBufferSwitched();
}

uint KexiDataAwarePropertyBuffer::size() const
{
	return m_buffers.size();
}

void KexiDataAwarePropertyBuffer::clear(uint minimumSize)
{
	m_buffers.clear();
	m_buffers.resize(QMAX(minimumSize, MAX_FIELDS));
	m_view->setDirty(true);
	m_view->propertyBufferSwitched();
}

void KexiDataAwarePropertyBuffer::slotReloadRequested()
{
	clear();
}

void KexiDataAwarePropertyBuffer::insert(uint row, KexiPropertyBuffer* buf, bool newOne)
{
	if (!buf || row >= m_buffers.size()) {
		kexiwarn << "KexiDataAwarePropertyBuffer::insert() invalid args: rew="<< row<< " buf="<< buf<< endl;
		return;
	}
	if (buf->parent() && buf->parent()!=this) {
		kexiwarn << "KexiDataAwarePropertyBuffer::insert() buffer's parent must be NULL or this KexiDataAwarePropertyBuffer" << endl;
		return;
	}

//	m_buffers.remove(row);//sanity

/*	//let's move down all buffers that are below
	m_buffers.setAutoDelete(false);//to avoid auto deleting in insert()
	m_buffers.resize(m_buffers.size()+1);
	for (int i=int(m_buffers.size()-1); i>(int)row; i--) {
		KexiPropertyBuffer *b = m_buffers[i-1];
		m_buffers.insert( i , b );
	}*/
	m_buffers.insert(row, buf);

//	m_buffers.setAutoDelete(true);//revert the flag

	connect(buf,SIGNAL(propertyChanged()), m_view, SLOT(setDirty()));

	if (newOne) {
		//add a special property indicating that this is brand new buffer, 
		//not just changed
		KexiProperty* prop = new KexiProperty("newrow", QVariant());
		prop->setVisible(false);
		buf->add( prop );
		m_view->setDirty();
	}
}

KexiPropertyBuffer* KexiDataAwarePropertyBuffer::currentPropertyBuffer() const
{
	return (m_dataObject->currentRow() >= 0) ? m_buffers.at( m_dataObject->currentRow() ) : 0;
}

void KexiDataAwarePropertyBuffer::slotRowDeleted()
{
	m_view->setDirty();
	removeCurrentPropertyBuffer();

	//let's move up all buffers that are below that deleted
	m_buffers.setAutoDelete(false);//to avoid auto deleting in insert()
	const int r = m_dataObject->currentRow();
	for (int i=r;i<int(m_buffers.size()-1);i++) {
		KexiPropertyBuffer *b = m_buffers[i+1];
		m_buffers.insert( i , b );
	}
	m_buffers.insert( m_buffers.size()-1, 0 );
	m_buffers.setAutoDelete(true);//revert the flag

	m_view->propertyBufferSwitched();
	emit rowDeleted();
}

void KexiDataAwarePropertyBuffer::slotRowsDeleted( const QValueList<int> &rows )
{
	//let's move most buffers up & delete unwanted
	m_buffers.setAutoDelete(false);//to avoid auto deleting in insert()
	const int orig_size = size();
	int prev_r = -1;
	int num_removed = 0, cur_r = -1;
	for (QValueList<int>::ConstIterator r_it = rows.constBegin(); r_it!=rows.constEnd() && *r_it < orig_size; ++r_it) {
		cur_r = *r_it;// - num_removed;
		if (prev_r>=0) {
//			kdDebug() << "move " << prev_r+num_removed-1 << ".." << cur_r-1 << " to " << prev_r+num_removed-1 << ".." << cur_r-2 << endl;
			int i=prev_r;
			KexiPropertyBuffer *b = m_buffers.take(i+num_removed);
			kdDebug() << "buffer " << i+num_removed << " deleted" << endl;
			delete b;
			num_removed++;
			for (; (i+num_removed)<cur_r; i++) {
				m_buffers.insert( i, m_buffers[i+num_removed] );
				kdDebug() << i << " <- " << i+num_removed << endl;
			}
		}
		prev_r = cur_r - num_removed;
	}
	//move remaining buffers up
	if (cur_r>=0) {
		KexiPropertyBuffer *b = m_buffers.take(cur_r);
		kdDebug() << "buffer " << cur_r << " deleted" << endl;
		delete b;
		num_removed++;
		for (int i=prev_r; (i+num_removed)<orig_size; i++) {
			m_buffers.insert( i, m_buffers[i+num_removed] );
			kdDebug() << i << " <- " << i+num_removed << endl;
		}
	}
	//finally: clear last rows
	for (int i=orig_size-num_removed; i<orig_size; i++) {
		kdDebug() << i << " <- zero" << endl;
		m_buffers.insert( i, 0 );
	}
	m_buffers.setAutoDelete(true);//revert the flag
	
	if (num_removed>0)
		m_view->setDirty();
	m_view->propertyBufferSwitched();
}

//void KexiDataAwarePropertyBuffer::slotEmptyRowInserted(KexiTableItem*, uint /*index*/)
void KexiDataAwarePropertyBuffer::slotRowInserted(KexiTableItem*, uint row, bool /*repaint*/)
{
	m_view->setDirty();

	//let's move down all buffers that are below
	m_buffers.setAutoDelete(false);//to avoid auto deleting in insert()
//	const int r = m_dataObject->currentRow();
	m_buffers.resize(m_buffers.size()+1);
	for (int i=int(m_buffers.size()); i>(int)row; i--) {
		KexiPropertyBuffer *b = m_buffers[i-1];
		m_buffers.insert( i , b );
	}
	m_buffers.insert( row, 0 );
	m_buffers.setAutoDelete(true);//revert the flag

	m_view->propertyBufferSwitched();

	emit rowInserted();
}

void KexiDataAwarePropertyBuffer::slotCellSelected(int, int row)
{
	if(row == m_row)
		return;
	m_row = row;
	m_view->propertyBufferSwitched();
}

KexiPropertyBuffer* KexiDataAwarePropertyBuffer::bufferForItem(KexiTableItem& item)
{
	if (m_currentTVData.isNull())
		return 0;
	int idx = m_currentTVData->findRef(&item);
	if (idx<0)
		return 0;
	return m_buffers[idx];
}

#include "kexitableviewpropertybuffer.moc"


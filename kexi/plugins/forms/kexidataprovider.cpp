/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexidataprovider.h"

#include <qwidget.h>
#include <qobjectlist.h>

#include <kdebug.h>
#include <klocale.h>

#include <tableview/kexitableitem.h>
#include <tableview/kexitableviewdata.h>

KexiDataProvider::KexiDataProvider()
 : KexiDataItemChangesListener()
 , m_mainWidget(0)
{
}

KexiDataProvider::~KexiDataProvider()
{
}

void KexiDataProvider::setMainWidget(QWidget* mainWidget)
{
	m_mainWidget = mainWidget;
	m_dataItems.clear();
	m_usedDataSources.clear();
	m_fieldNumbersForDataItems.clear();
	if (!m_mainWidget)
		return;

	//find widgets whose will work as data items
	QObjectList *l = m_mainWidget->queryList( "QWidget" );
	QObjectListIt it( *l );
	QObject *obj;
	QDict<char> tmpSources;
	for ( ; (obj = it.current()) != 0; ++it ) {
		if (dynamic_cast<KexiDataItemInterface*>(obj)) {
			QString dataSource( dynamic_cast<KexiDataItemInterface*>(obj)->dataSource().lower() );
			if (!dataSource.isEmpty()) {
				kexipluginsdbg << obj->name() << endl;
				m_dataItems.append( dynamic_cast<KexiDataItemInterface*>(obj) );
				dynamic_cast<KexiDataItemInterface*>(obj)->installListener( this );
				tmpSources.replace( dataSource, (char*)1 );
			}
		}
	}
	delete l;
	//we've got now a set (unique list) of field names in tmpSources
	//remember it in m_usedDataSources
	for (QDictIterator<char> it(tmpSources); it.current(); ++it) {
		m_usedDataSources += it.currentKey();
	}
	//fill m_fieldNumbersForDataItems mapping from data item to field number
	//(needed for fillDataItems)
	for (QPtrListIterator<KexiDataItemInterface> it(m_dataItems); it.current(); ++it) {
		m_fieldNumbersForDataItems.insert( it.current(), 
			m_usedDataSources.findIndex(it.current()->dataSource().lower()) );
	}
}

void KexiDataProvider::fillDataItems(KexiTableItem& row)//KexiDB::Cursor& cursor)
{
	kexidbg << "KexiDataProvider::fillDataItems() cnt=" << row.count() << endl;
 	for (KexiDataItemInterfaceToIntMap::ConstIterator it = m_fieldNumbersForDataItems.constBegin(); 
		it!=m_fieldNumbersForDataItems.constEnd(); ++it)
	{
		it.key()->setValue( row.at(it.data()) );
	}
}

void KexiDataProvider::valueChanged(KexiDataItemInterface* item)
{
}

void KexiDataProvider::invalidateDataSources( const QValueList<uint>& invalidSources )
{
	QValueVector<int> newIndices(m_dataItems.count());
	uint i = 0, number = 0;

	//in 'newIndices' let's collect new indices for every data source
	foreach(QValueList<uint>::ConstIterator, it, invalidSources) {
		for (; i < (*it); i++) {
			newIndices[i] = number++;
			kexidbg << "invalidateDataSources(): " << i << " -> " << number-1 << endl;
		}
		newIndices[i]=-1;
		kexidbg << "invalidateDataSources(): " << i << " -> " << -1 << endl;
		i++;
	}
	//fill remaining part of the vector
	for (; i < m_dataItems.count(); i++) {
		newIndices[i] = number++;
		kexidbg << "invalidateDataSources(): " << i << " -> " << number-1 << endl;
	}

	//recreate m_fieldNumbersForDataItems and mark widgets with invalid DS
	KexiDataItemInterfaceToIntMap newFieldNumbersForDataItems;
//	for (QMap<KexiDataItemInterface*,uint>::ConstIterator it = m_fieldNumbersForDataItems.constBegin();
//		it!=m_fieldNumbersForDataItems.constEnd();++it)
	foreach(KexiDataItemInterfaceToIntMap::ConstIterator, it, m_fieldNumbersForDataItems) {
		const uint newIndex = newIndices[ it.data() ];
		if (newIndex==-1) {
			kexidbg << "invalidateDataSources(): removing" << endl;
			m_dataItems.remove(it.key());
			it.key()->setInvalidState( QString::fromLatin1("#") + i18n("NAME") + QString::fromLatin1("?") );
		}
		else {
			kexidbg << "invalidateDataSources(): " << it.key()->dataSource() << ": " << it.data() << " -> " << newIndex << endl;
			newFieldNumbersForDataItems.replace(it.key(), newIndex);
		}
	}
	m_fieldNumbersForDataItems = newFieldNumbersForDataItems;

	//update data sources list (some of them may be removed)
	QDict<char> tmpUsedDataSources(101);

	foreach_list(QPtrListIterator<KexiDataItemInterface>, it, m_dataItems) {
		tmpUsedDataSources.replace( it.current()->dataSource().lower(), (char*)1 );
	}
	m_usedDataSources.clear();
	foreach_list(QDictIterator<char>, it, tmpUsedDataSources) {
		m_usedDataSources += it.currentKey();
	}
}

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
#include <kexidb/queryschema.h>

KexiFormDataProvider::KexiFormDataProvider()
 : KexiDataItemChangesListener()
 , m_mainWidget(0)
 , m_duplicatedItems(0)
{
}

KexiFormDataProvider::~KexiFormDataProvider()
{
	delete m_duplicatedItems;
}

void KexiFormDataProvider::setMainWidget(QWidget* mainWidget)
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
		if (dynamic_cast<KexiFormDataItemInterface*>(obj)) {
			QString dataSource( dynamic_cast<KexiFormDataItemInterface*>(obj)->dataSource().lower() );
			if (!dataSource.isEmpty()) {
				kexipluginsdbg << obj->name() << endl;
				m_dataItems.append( dynamic_cast<KexiFormDataItemInterface*>(obj) );
				dynamic_cast<KexiFormDataItemInterface*>(obj)->installListener( this );
				tmpSources.replace( dataSource, (char*)1 );
			}
		}
	}
	delete l;
	//now we've got a set (unique list) of field names in tmpSources
	//remember it in m_usedDataSources
	for (QDictIterator<char> it(tmpSources); it.current(); ++it) {
		m_usedDataSources += it.currentKey();
	}
	//fill m_fieldNumbersForDataItems mapping from data item to field number
	//(needed for fillDataItems)
	for (QPtrListIterator<KexiFormDataItemInterface> it(m_dataItems); it.current(); ++it) {
		m_fieldNumbersForDataItems.insert( it.current(), 
			m_usedDataSources.findIndex(it.current()->dataSource().lower()) );
	}
}

void KexiFormDataProvider::fillDataItems(KexiTableItem& row)
{
	kexidbg << "KexiFormDataProvider::fillDataItems() cnt=" << row.count() << endl;
 	for (KexiFormDataItemInterfaceToIntMap::ConstIterator it = m_fieldNumbersForDataItems.constBegin(); 
		it!=m_fieldNumbersForDataItems.constEnd(); ++it)
	{
		kdDebug() << "fill data of '" << it.key()->dataSource() <<  "' at idx=" << it.data() << endl;
		it.key()->setValue( row.at(it.data()) );
	}
}

void KexiFormDataProvider::fillDuplicatedDataItems(
	KexiFormDataItemInterface* item, const QVariant& value)
{
	if (!m_duplicatedItems) {
		//build (once) a set of duplicated data items (having the same data sources)
		//so we can later check if an item is duplicated with a cost of o(1)
		QMap<QString,int> tmpDuplicatedItems;
		QMapIterator<QString,int> it_dup;
		for (QPtrListIterator<KexiFormDataItemInterface> it(m_dataItems); it.current(); ++it) {
			it_dup = tmpDuplicatedItems.find( it.current()->dataSource().lower() );
			uint count;
			if (it_dup==tmpDuplicatedItems.end())
				count = 0;
			else
				count = it_dup.data();
			tmpDuplicatedItems.insert( it.current()->dataSource().lower(), ++count );
		}
		m_duplicatedItems = new QDict<char>(1013);
		for (it_dup = tmpDuplicatedItems.begin(); it_dup!=tmpDuplicatedItems.end(); ++it_dup) {
			if (it_dup.data() > 1) {
        m_duplicatedItems->insert( it_dup.key(), (char*)1 );
				kdDebug() << "duplicated item: " << it_dup.key() << " (" << it_dup.data() << " times)" << endl;
			}
		}
	}
	if (m_duplicatedItems->find( item->dataSource().lower() )) {
		QString dataSource( item->dataSource().lower() );
		for (QPtrListIterator<KexiFormDataItemInterface> it(m_dataItems); it.current(); ++it) {
			if (it.current()!=item && dataSource == it.current()->dataSource().lower()) {
				kdDebug() << "- setting value for item '" 
					<< dynamic_cast<QObject*>(it.current())->name() << " == " << value.toString() << endl;
				it.current()->setValue( value );
			}
		}
	}
}

void KexiFormDataProvider::valueChanged(KexiDataItemInterface* item)
{
}

void KexiFormDataProvider::invalidateDataSources( const QValueList<uint>& invalidSources,
 KexiDB::QuerySchema* query)
{
	QValueVector<int> newIndices(m_dataItems.count());
	uint i = 0, number = 0;

	//in 'newIndices' let's collect new indices for every data source
	foreach(QValueList<uint>::ConstIterator, it, invalidSources) {
		//all previous indices have corresponding DS
		for (; i < (*it); i++) {
			newIndices[i] = number++;
			kexidbg << "invalidateDataSources(): " << i << " -> " << number-1 << endl;
		}
		//this idex have no corresponding DS
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
	KexiFormDataItemInterfaceToIntMap newFieldNumbersForDataItems;
	foreach(KexiFormDataItemInterfaceToIntMap::ConstIterator, it, m_fieldNumbersForDataItems) {
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

	//update data sources set (some of them may be removed)
	QDict<char> tmpUsedDataSources(1013);

	if (query)
		query->debug();

	//if (query && m_dataItems.count()!=query->fieldCount()) {
	//	kdWarning() << "KexiFormDataProvider::invalidateDataSources(): m_dataItems.count()!=query->fieldCount() ("
	//	 << m_dataItems.count() << "," << query->fieldCount() << ")" << endl;
	//}
	//i = 0;
	foreach_list(QPtrListIterator<KexiFormDataItemInterface>, it, m_dataItems) {
		uint fieldNumber = m_fieldNumbersForDataItems[ it.current() ];
		if (query) {
//! @todo what about using QueryColumnInfo here?
			KexiDB::Field *f = query->fieldsExpanded().at(fieldNumber)->field;
			it.current()->setField(f);
			kdDebug() << "- item=" << dynamic_cast<QObject*>(it.current())->name() 
				<< " dataSource=" << it.current()->dataSource()
				<< " field=" << f->name() << endl;
		}
		tmpUsedDataSources.replace( it.current()->dataSource().lower(), (char*)1 );
//		i++;
	}
	m_usedDataSources.clear();
	foreach_list(QDictIterator<char>, it, tmpUsedDataSources) {
		m_usedDataSources += it.currentKey();
	}
}

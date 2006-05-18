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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexidataprovider.h"

#include <qwidget.h>
#include <qobjectlist.h>

#include <kdebug.h>
#include <klocale.h>

#include <tableview/kexitableitem.h>
#include <tableview/kexitableviewdata.h>
#include <kexidb/queryschema.h>
#include <kexiutils/utils.h>

#include "widgets/kexidbform.h"

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

void KexiFormDataProvider::setMainDataSourceWidget(QWidget* mainWidget)
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
		if (!dynamic_cast<KexiFormDataItemInterface*>(obj))
			continue;
#if 0 //! @todo reenable when subform is moved to KexiDBForm
		KexiDBForm *dbForm = KexiUtils::findParent<KexiDBForm>(obj, "KexiDBForm"); //form's surface...
		if (dbForm!=m_mainWidget) //only set data for this form's data items
			continue;
#else
		//tmp: reject widgets within subforms
		if (KexiUtils::findParent<KexiDBForm>(obj, "KexiDBSubForm"))
			continue;
#endif
		QString dataSource( dynamic_cast<KexiFormDataItemInterface*>(obj)->dataSource().lower() );
		if (dataSource.isEmpty())
			continue;
		kexipluginsdbg << obj->name() << endl;
		m_dataItems.append( dynamic_cast<KexiFormDataItemInterface*>(obj) );
		dynamic_cast<KexiFormDataItemInterface*>(obj)->installListener( this );
		tmpSources.replace( dataSource, (char*)1 );
	}
	delete l;
	//now we've got a set (unique list) of field names in tmpSources
	//remember it in m_usedDataSources
	for (QDictIterator<char> it(tmpSources); it.current(); ++it) {
		m_usedDataSources += it.currentKey();
	}
}

void KexiFormDataProvider::fillDataItems(KexiTableItem& row)
{
	kexidbg << "KexiFormDataProvider::fillDataItems() cnt=" << row.count() << endl;
 	for (KexiFormDataItemInterfaceToIntMap::ConstIterator it = m_fieldNumbersForDataItems.constBegin(); 
		it!=m_fieldNumbersForDataItems.constEnd(); ++it)
	{
		kexipluginsdbg << "fill data of '" << it.key()->dataSource() <<  "' at idx=" << it.data() 
			<< " data=" << row.at(it.data()) << endl;
		it.key()->setValue( row.at(it.data()) );
	}
}

void KexiFormDataProvider::fillDuplicatedDataItems(
	KexiFormDataItemInterface* item, const QVariant& value)
{
	if (!m_duplicatedItems) {
		//build (once) a set of duplicated data items (having the same fields assigned)
		//so we can later check if an item is duplicated with a cost of o(1)
		QMap<KexiDB::Field*,int> tmpDuplicatedItems;
		QMapIterator<KexiDB::Field*,int> it_dup;
		for (QPtrListIterator<KexiFormDataItemInterface> it(m_dataItems); it.current(); ++it) {
			it_dup = tmpDuplicatedItems.find( it.current()->columnInfo()->field );
			uint count;
			if (it_dup==tmpDuplicatedItems.end())
				count = 0;
			else
				count = it_dup.data();
			tmpDuplicatedItems.insert( it.current()->columnInfo()->field, ++count );
		}
		m_duplicatedItems = new QPtrDict<char>(101);
		for (it_dup = tmpDuplicatedItems.begin(); it_dup!=tmpDuplicatedItems.end(); ++it_dup) {
			if (it_dup.data() > 1) {
				m_duplicatedItems->insert( it_dup.key(), (char*)1 );
				kexipluginsdbg << "duplicated item: " << static_cast<KexiDB::Field*>(it_dup.key())->name() 
					<< " (" << it_dup.data() << " times)" << endl;
			}
		}
	}
	if (m_duplicatedItems->find( item->columnInfo()->field )) {
		for (QPtrListIterator<KexiFormDataItemInterface> it(m_dataItems); it.current(); ++it) {
			if (it.current()!=item && item->columnInfo()->field == it.current()->columnInfo()->field) {
				kexipluginsdbg << "- setting value for item '" 
					<< dynamic_cast<QObject*>(it.current())->name() << " == " << value.toString() << endl;
				it.current()->setValue( value );
			}
		}
	}
}

void KexiFormDataProvider::valueChanged(KexiDataItemInterface* item)
{
	Q_UNUSED( item );
}

bool KexiFormDataProvider::cursorAtNewRow()
{
	return false;
}

void KexiFormDataProvider::invalidateDataSources( const QValueList<uint>& invalidSources,
 KexiDB::QuerySchema* query)
{
	//fill m_fieldNumbersForDataItems mapping from data item to field number
	//(needed for fillDataItems)
	KexiDB::QueryColumnInfo::Vector fieldsExpanded;
	uint dataFieldsCount; // == fieldsExpanded.count() if query is available or else == m_dataItems.count()
	if (query) {
		fieldsExpanded = query->fieldsExpanded();
		dataFieldsCount = fieldsExpanded.count();
		QMap<KexiDB::QueryColumnInfo*,int> fieldsOrder( query->fieldsOrder() );
		for (QMapConstIterator<KexiDB::QueryColumnInfo*,int> it = fieldsOrder.constBegin(); it!=fieldsOrder.constEnd(); ++it) {
			kexipluginsdbg << "query->fieldsOrder()[ " << it.key()->field->name() << " ] = " << it.data() << endl;
		}
		for (QPtrListIterator<KexiFormDataItemInterface> it(m_dataItems); it.current(); ++it) {
			KexiFormDataItemInterface *item = it.current();
			KexiDB::QueryColumnInfo* ci = query->columnInfo( it.current()->dataSource() );
			int index = ci ? query->fieldsOrder()[ ci ] : -1;
			kexipluginsdbg << "query->fieldsOrder()[ " << (ci ? ci->field->name() : "") << " ] = " << index 
				<< " (dataSource: " << item->dataSource() << ", name=" << dynamic_cast<QObject*>(item)->name() << ")" << endl;
			if (index!=-1)
				m_fieldNumbersForDataItems.insert( item, index );
	//todo
	//WRONG: not only used data sources can be fetched!
	//			m_fieldNumbersForDataItems.insert( it.current(), 
	//				m_usedDataSources.findIndex(it.current()->dataSource().lower()) );
		}
	}
	else {//!query
		dataFieldsCount = m_dataItems.count();
	}

	//in 'newIndices' let's collect new indices for every data source
	foreach(QValueList<uint>::ConstIterator, it, invalidSources) {
		//all previous indices have corresponding data source
//		for (; i < (*it); i++) {
//			newIndices[i] = number++;
			//kexidbg << "invalidateDataSources(): " << i << " -> " << number-1 << endl;
//		}
		//this index have no corresponding data source
//		newIndices[i]=-1;
		KexiFormDataItemInterface *item = m_dataItems.at( *it );
		if (item)
			item->setInvalidState( QString::fromLatin1("#") + i18n("NAME") + QString::fromLatin1("?") );
		m_dataItems.remove(*it);
		kexidbg << "invalidateDataSources(): " << (*it) << " -> " << -1 << endl;
//		i++;
	}
	//fill remaining part of the vector
//	for (; i < dataFieldsCount; i++) { //m_dataItems.count(); i++) {
		//newIndices[i] = number++;
		//kexidbg << "invalidateDataSources(): " << i << " -> " << number-1 << endl;
	//}

#if 0
	//recreate m_fieldNumbersForDataItems and mark widgets with invalid data sources
	KexiFormDataItemInterfaceToIntMap newFieldNumbersForDataItems;
	foreach(KexiFormDataItemInterfaceToIntMap::ConstIterator, it, m_fieldNumbersForDataItems) {
		bool ok;
		const int newIndex = newIndices.at( it.data(), &ok );
		if (ok && newIndex!=-1) {
			kexidbg << "invalidateDataSources(): " << it.key()->dataSource() << ": " << it.data() << " -> " << newIndex << endl;
			newFieldNumbersForDataItems.replace(it.key(), newIndex);
		}
		else {
			kexidbg << "invalidateDataSources(): removing " << it.key()->dataSource() << endl;
			m_dataItems.remove(it.key());
			it.key()->setInvalidState( QString::fromLatin1("#") + i18n("NAME") + QString::fromLatin1("?") );
		}
	}
#endif
//	m_fieldNumbersForDataItems = newFieldNumbersForDataItems;

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
		KexiFormDataItemInterface * item = it.current();
		uint fieldNumber = m_fieldNumbersForDataItems[ item ];
		if (query) {
			KexiDB::QueryColumnInfo *ci = fieldsExpanded[fieldNumber];
//			KexiDB::Field *f = ci->field;
			it.current()->setColumnInfo(ci);
			kexipluginsdbg << "- item=" << dynamic_cast<QObject*>(it.current())->name() 
				<< " dataSource=" << it.current()->dataSource()
				<< " field=" << ci->field->name() << endl;
		}
		tmpUsedDataSources.replace( it.current()->dataSource().lower(), (char*)1 );
	}
	m_usedDataSources.clear();
	foreach_list(QDictIterator<char>, it, tmpUsedDataSources) {
		m_usedDataSources += it.currentKey();
	}
}

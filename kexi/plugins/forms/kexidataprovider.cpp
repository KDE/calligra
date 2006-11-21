/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jaroslaw Staniek <js@iidea.pl>

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
#include <qobject.h>
//Added by qt3to4:
#include <Q3ValueList>

#include <kdebug.h>
#include <klocale.h>

#include <widget/tableview/kexitableitem.h>
#include <widget/tableview/kexitableviewdata.h>
#include <widget/tableview/kexicomboboxbase.h>
#include <kexidb/queryschema.h>
#include <kexiutils/utils.h>

#include "widgets/kexidbform.h"

KexiFormDataProvider::KexiFormDataProvider()
 : KexiDataItemChangesListener()
 , m_mainWidget(0)
 , m_duplicatedItems(0)
 , m_disableFillDuplicatedDataItems(false)
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
	Q3Dict<char> tmpSources;
	for ( ; (obj = it.current()) != 0; ++it ) {
		KexiFormDataItemInterface* const formDataItem = dynamic_cast<KexiFormDataItemInterface*>(obj);
		if (!formDataItem)
			continue;
		if (formDataItem->parentInterface()) //item with parent interface: collect parent instead...
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
		QString dataSource( formDataItem->dataSource().lower() );
		if (dataSource.isEmpty())
			continue;
		kexipluginsdbg << obj->name() << endl;
		m_dataItems.append( formDataItem );
		formDataItem->installListener( this );
		tmpSources.replace( dataSource, (char*)1 );
	}
	delete l;
	//now we've got a set (unique list) of field names in tmpSources
	//remember it in m_usedDataSources
	for (Q3DictIterator<char> it(tmpSources); it.current(); ++it) {
		m_usedDataSources += it.currentKey();
	}
}

void KexiFormDataProvider::fillDataItems(KexiTableItem& row, bool cursorAtNewRow)
{
	kexipluginsdbg << "KexiFormDataProvider::fillDataItems() cnt=" << row.count() << endl;
	for (KexiFormDataItemInterfaceToIntMap::ConstIterator it = m_fieldNumbersForDataItems.constBegin(); 
		it!=m_fieldNumbersForDataItems.constEnd(); ++it)
	{
		KexiFormDataItemInterface *itemIface = it.key();
		if (!itemIface->columnInfo()) {
			kexipluginsdbg << "KexiFormDataProvider::fillDataItems(): itemIface->columnInfo() == 0" << endl;
			continue;
		}
		//1. Is this a value with a combo box (lookup)?
		int indexForVisibleLookupValue = itemIface->columnInfo()->indexForVisibleLookupValue();
		if (indexForVisibleLookupValue<0 && indexForVisibleLookupValue>=(int)row.count()) //sanity
			indexForVisibleLookupValue = -1; //no
		const QVariant value(row.at(it.data()));
		QVariant visibleLookupValue;
		if (indexForVisibleLookupValue!=-1 && (int)row.size()>indexForVisibleLookupValue)
			visibleLookupValue = row.at(indexForVisibleLookupValue);
		kexipluginsdbg << "fill data of '" << itemIface->dataSource() <<  "' at idx=" << it.data() 
			<< " data=" << value << (indexForVisibleLookupValue!=-1 
				? QString(" SPECIAL: indexForVisibleLookupValue=%1 visibleValue=%2")
					.arg(indexForVisibleLookupValue).arg(visibleLookupValue.toString())
				: QString::null)
			<< endl;
		const bool displayDefaultValue = cursorAtNewRow && (value.isNull() && visibleLookupValue.isNull())
			&& !itemIface->columnInfo()->field->defaultValue().isNull() 
			&& !itemIface->columnInfo()->field->isAutoIncrement(); //no value to set but there is default value defined
		itemIface->setValue( 
			displayDefaultValue ? itemIface->columnInfo()->field->defaultValue() : value,
			QVariant(), /*add*/
			/*!remove old*/false, 
			indexForVisibleLookupValue==-1 ? 0 : &visibleLookupValue //pass visible value if available
		);
		// now disable/enable "display default value" if needed (do it after setValue(), before setValue() turns it off)
		if (itemIface->hasDisplayedDefaultValue() != displayDefaultValue)
			itemIface->setDisplayDefaultValue( dynamic_cast<QWidget*>(itemIface), displayDefaultValue );
	}
}

void KexiFormDataProvider::fillDuplicatedDataItems(
	KexiFormDataItemInterface* item, const QVariant& value)
{
	if (m_disableFillDuplicatedDataItems)
		return;
	if (!m_duplicatedItems) {
		//build (once) a set of duplicated data items (having the same fields assigned)
		//so we can later check if an item is duplicated with a cost of o(1)
		QMap<KexiDB::Field*,int> tmpDuplicatedItems;
		QMapIterator<KexiDB::Field*,int> it_dup;
		for (Q3PtrListIterator<KexiFormDataItemInterface> it(m_dataItems); it.current(); ++it) {
			if (!it.current()->columnInfo() || !it.current()->columnInfo()->field)
				continue;
			kDebug() << " ** " << it.current()->columnInfo()->field->name() << endl;
			it_dup = tmpDuplicatedItems.find( it.current()->columnInfo()->field );
			uint count;
			if (it_dup==tmpDuplicatedItems.end())
				count = 0;
			else
				count = it_dup.data();
			tmpDuplicatedItems.insert( it.current()->columnInfo()->field, ++count );
		}
		m_duplicatedItems = new Q3PtrDict<char>(101);
		for (it_dup = tmpDuplicatedItems.begin(); it_dup!=tmpDuplicatedItems.end(); ++it_dup) {
			if (it_dup.data() > 1) {
				m_duplicatedItems->insert( it_dup.key(), (char*)1 );
				kexipluginsdbg << "duplicated item: " << static_cast<KexiDB::Field*>(it_dup.key())->name() 
					<< " (" << it_dup.data() << " times)" << endl;
			}
		}
	}
	if (item->columnInfo() && m_duplicatedItems->find( item->columnInfo()->field )) {
		for (Q3PtrListIterator<KexiFormDataItemInterface> it(m_dataItems); it.current(); ++it) {
			if (it.current()!=item && item->columnInfo()->field == it.current()->columnInfo()->field) {
				kexipluginsdbg << "- setting a copy of value for item '" 
					<< dynamic_cast<QObject*>(it.current())->name() << "' == " << value << endl;
				it.current()->setValue( value );
			}
		}
	}
}

void KexiFormDataProvider::valueChanged(KexiDataItemInterface* item)
{
	Q_UNUSED( item );
}

bool KexiFormDataProvider::cursorAtNewRow() const
{
	return false;
}

void KexiFormDataProvider::invalidateDataSources( const Q3Dict<char>& invalidSources,
 KexiDB::QuerySchema* query)
{
	//fill m_fieldNumbersForDataItems mapping from data item to field number
	//(needed for fillDataItems)
	KexiDB::QueryColumnInfo::Vector fieldsExpanded;
//	uint dataFieldsCount; // == fieldsExpanded.count() if query is available or else == m_dataItems.count()

	if (query) {
		fieldsExpanded = query->fieldsExpanded( KexiDB::QuerySchema::WithInternalFields );
//		dataFieldsCount = fieldsExpanded.count();
		QMap<KexiDB::QueryColumnInfo*,int> columnsOrder( query->columnsOrder() );
		for (QMapConstIterator<KexiDB::QueryColumnInfo*,int> it = columnsOrder.constBegin(); it!=columnsOrder.constEnd(); ++it) {
			kexipluginsdbg << "query->columnsOrder()[ " << it.key()->field->name() << " ] = " << it.data() << endl;
		}
		for (Q3PtrListIterator<KexiFormDataItemInterface> it(m_dataItems); it.current(); ++it) {
			KexiFormDataItemInterface *item = it.current();
			KexiDB::QueryColumnInfo* ci = query->columnInfo( it.current()->dataSource() );
			int index = ci ? columnsOrder[ ci ] : -1;
			kexipluginsdbg << "query->columnsOrder()[ " << (ci ? ci->field->name() : "") << " ] = " << index 
				<< " (dataSource: " << item->dataSource() << ", name=" << dynamic_cast<QObject*>(item)->name() << ")" << endl;
			if (index!=-1 && !m_fieldNumbersForDataItems[ item ])
				m_fieldNumbersForDataItems.insert( item, index );
	//todo
	//WRONG: not only used data sources can be fetched!
	//			m_fieldNumbersForDataItems.insert( it.current(), 
	//				m_usedDataSources.findIndex(it.current()->dataSource().lower()) );
		}
	}
	else {//!query
//		dataFieldsCount = m_dataItems.count();
	}

#if 0 //moved down
	//in 'newIndices' let's collect new indices for every data source
	foreach(Q3ValueList<uint>::ConstIterator, it, invalidSources) {
		//all previous indices have corresponding data source
//		for (; i < (*it); i++) {
//			newIndices[i] = number++;
			//kexipluginsdbg << "invalidateDataSources(): " << i << " -> " << number-1 << endl;
//		}
		//this index have no corresponding data source
//		newIndices[i]=-1;
		KexiFormDataItemInterface *item = m_dataItems.at( *it );
		if (item)
			item->setInvalidState( QString::fromLatin1("#") + i18n("NAME") + QString::fromLatin1("?") );
		m_dataItems.remove(*it);
		kexipluginsdbg << "invalidateDataSources(): " << (*it) << " -> " << -1 << endl;
//		i++;
	}
#endif
	//fill remaining part of the vector
//	for (; i < dataFieldsCount; i++) { //m_dataItems.count(); i++) {
		//newIndices[i] = number++;
		//kexipluginsdbg << "invalidateDataSources(): " << i << " -> " << number-1 << endl;
	//}

#if 0
	//recreate m_fieldNumbersForDataItems and mark widgets with invalid data sources
	KexiFormDataItemInterfaceToIntMap newFieldNumbersForDataItems;
	foreach(KexiFormDataItemInterfaceToIntMap::ConstIterator, it, m_fieldNumbersForDataItems) {
		bool ok;
		const int newIndex = newIndices.at( it.data(), &ok );
		if (ok && newIndex!=-1) {
			kexipluginsdbg << "invalidateDataSources(): " << it.key()->dataSource() << ": " << it.data() << " -> " << newIndex << endl;
			newFieldNumbersForDataItems.replace(it.key(), newIndex);
		}
		else {
			kexipluginsdbg << "invalidateDataSources(): removing " << it.key()->dataSource() << endl;
			m_dataItems.remove(it.key());
			it.key()->setInvalidState( QString::fromLatin1("#") + i18n("NAME") + QString::fromLatin1("?") );
		}
	}
#endif
//	m_fieldNumbersForDataItems = newFieldNumbersForDataItems;

	//update data sources set (some of them may be removed)
	Q3Dict<char> tmpUsedDataSources(1013);

	if (query)
		query->debug();

	//if (query && m_dataItems.count()!=query->fieldCount()) {
	//	kWarning() << "KexiFormDataProvider::invalidateDataSources(): m_dataItems.count()!=query->fieldCount() ("
	//	 << m_dataItems.count() << "," << query->fieldCount() << ")" << endl;
	//}
	//i = 0;
	m_disableFillDuplicatedDataItems = true; // temporary disable fillDuplicatedDataItems()
	                                         // because setColumnInfo() can activate it
	for (Q3PtrListIterator<KexiFormDataItemInterface> it(m_dataItems); it.current();) {
		KexiFormDataItemInterface * item = it.current();
		if (invalidSources[ item->dataSource().lower() ]) {
			item->setInvalidState( QString::fromLatin1("#") + i18n("NAME") + QString::fromLatin1("?") );
			m_dataItems.remove(item);
			continue;
		}
		uint fieldNumber = m_fieldNumbersForDataItems[ item ];
		if (query) {
			KexiDB::QueryColumnInfo *ci = fieldsExpanded[fieldNumber];
			item->setColumnInfo(ci);
			kexipluginsdbg << "- item=" << dynamic_cast<QObject*>(item)->name() 
				<< " dataSource=" << item->dataSource()
				<< " field=" << ci->field->name() << endl;
			const int indexForVisibleLookupValue = ci->indexForVisibleLookupValue();
			if (-1 != indexForVisibleLookupValue && indexForVisibleLookupValue < (int)fieldsExpanded.count()) {
				//there's lookup column defined: set visible column as well
				KexiDB::QueryColumnInfo *visibleColumnInfo = fieldsExpanded[ indexForVisibleLookupValue ];
				if (visibleColumnInfo) {
					item->setVisibleColumnInfo( visibleColumnInfo );
					if (dynamic_cast<KexiComboBoxBase*>(item) && m_mainWidget
						&& dynamic_cast<KexiComboBoxBase*>(item)->internalEditor()) {
						// m_mainWidget (dbform) should filter the (just created using setVisibleColumnInfo()) 
						// combo box' internal editor (actually, only if the combo is in 'editable' mode)
						dynamic_cast<KexiComboBoxBase*>(item)->internalEditor()->installEventFilter(m_mainWidget);
					}
					kexipluginsdbg << " ALSO SET visibleColumn=" << visibleColumnInfo->debugString() 
						<< "\n at position " << indexForVisibleLookupValue << endl;
				}
			}
		}
		tmpUsedDataSources.replace( item->dataSource().lower(), (char*)1 );
		++it;
	}
	m_disableFillDuplicatedDataItems = false;
	m_usedDataSources.clear();
	foreach_list(Q3DictIterator<char>, it, tmpUsedDataSources) {
		m_usedDataSources += it.currentKey();
	}
}

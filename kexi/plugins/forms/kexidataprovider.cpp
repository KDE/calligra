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
	for (QMapConstIterator<KexiDataItemInterface*,uint> it = m_fieldNumbersForDataItems.constBegin(); 
		it!=m_fieldNumbersForDataItems.constEnd(); ++it)
	{
		it.key()->setValue( row.at(it.data()) );
//		it.key()->setValue( cursor.value(it.data()) );
	}
}

void KexiDataProvider::valueChanged(KexiDataItemInterface* item)
{
}


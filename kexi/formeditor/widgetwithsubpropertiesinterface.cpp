/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "widgetwithsubpropertiesinterface.h"

#include <qmetaobject.h>
#include <qasciidict.h>

#include <kdebug.h>

using namespace KFormDesigner;

WidgetWithSubpropertiesInterface::WidgetWithSubpropertiesInterface()
{
}

WidgetWithSubpropertiesInterface::~WidgetWithSubpropertiesInterface()
{
}

void WidgetWithSubpropertiesInterface::setSubwidget(QWidget *widget)
{
	m_subwidget = widget;
	m_subproperies.clear();
	QAsciiDict<char> addedSubproperies(1024);
	if (m_subwidget) {
		//remember properties in the subwidget that are not present in the parent
		for( QMetaObject *metaObject = m_subwidget->metaObject(); metaObject; metaObject = metaObject->superClass()) {
			const int numProperties = metaObject->numProperties();
			for (int i = 0; i < numProperties; i++) {
				const char *propertyName = metaObject->property( i )->name();
				if (dynamic_cast<QObject*>(this)->metaObject()->findProperty( propertyName, true )==-1
						&& !addedSubproperies.find( propertyName ) )
				{
					m_subproperies.append( propertyName );
					addedSubproperies.insert( propertyName, (char*)1 );
					kdDebug() << propertyName << endl;
				}
			}
		}
		qHeapSort( m_subproperies );
	}
}

QWidget* WidgetWithSubpropertiesInterface::subwidget() const
{
	return m_subwidget;
}

QValueList<Q3CString> WidgetWithSubpropertiesInterface::subproperies() const
{
	return m_subproperies;
}

const QMetaProperty *WidgetWithSubpropertiesInterface::findMetaSubproperty(const char * name) const
{
	if (!m_subwidget || m_subproperies.find(name) == m_subproperies.constEnd()) {
		return 0;
	}
	const int index = m_subwidget->metaObject()->findProperty( name, true );
	if (index==-1)
		return 0;
	return m_subwidget->metaObject()->property( index, true );
}

QVariant WidgetWithSubpropertiesInterface::subproperty( const char * name, bool &ok ) const
{
	if (!m_subwidget || m_subproperies.find(name) == m_subproperies.constEnd()) {
		ok = false;
		return QVariant();
	}
	ok = true;
	return m_subwidget->property( name );
}

bool WidgetWithSubpropertiesInterface::setSubproperty( const char * name, const QVariant & value )
{
	if (!m_subwidget || m_subproperies.find(name) == m_subproperies.end()) {
		return false;
	}
	return m_subwidget->setProperty( name, value );
}

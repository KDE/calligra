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

#include <kexiutils/utils.h>
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
	QSet<QByteArray> addedSubproperies;
	if (m_subwidget) {
		//remember properties in the subwidget that are not present in the parent
		for( const QMetaObject *metaObject = m_subwidget->metaObject(); metaObject; 
			metaObject = metaObject->superClass())
		{
			QList<QMetaProperty> properties( 
				KexiUtils::propertiesForMetaObjectWithInherited(metaObject) );
			foreach (QMetaProperty property, properties) {
				if (-1!=KexiUtils::indexOfPropertyWithSuperclasses(
					dynamic_cast<QObject*>(this), property.name())
					&& !addedSubproperies.contains( property.name() ) )
				{
					m_subproperies.insert( property.name() );
					addedSubproperies.insert( property.name() );
					kDebug() << "WidgetWithSubpropertiesInterface::setSubwidget(): "
						"added subwidget's property that is not present in the parent: " 
						<< property.name() << endl;
				}
			}
		}
	}
}

QWidget* WidgetWithSubpropertiesInterface::subwidget() const
{
	return m_subwidget;
}

QSet<Q3CString> WidgetWithSubpropertiesInterface::subproperies() const
{
	return m_subproperies;
}

QMetaProperty WidgetWithSubpropertiesInterface::findMetaSubproperty(const char * name) const
{
	if (!m_subwidget || m_subproperies.contains(name))
		return QMetaProperty();
	return KexiUtils::findPropertyWithSuperclasses(m_subwidget, name);
}

QVariant WidgetWithSubpropertiesInterface::subproperty( const char * name, bool &ok ) const
{
	if (!m_subwidget || m_subproperies.contains(name)) {
		ok = false;
		return QVariant();
	}
	ok = true;
	return m_subwidget->property( name );
}

bool WidgetWithSubpropertiesInterface::setSubproperty( const char * name, const QVariant & value )
{
	if (!m_subwidget || m_subproperies.contains(name))
		return false;
	return m_subwidget->setProperty( name, value );
}

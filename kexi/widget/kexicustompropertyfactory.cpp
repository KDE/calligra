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

#include "kexicustompropertyfactory.h"
#include "kexicustompropertyfactory_p.h"
#include <koproperty/customproperty.h>

using namespace KoProperty;

class PixmapIdCustomProperty : public CustomProperty
{
	public:
		PixmapIdCustomProperty(Property *parent)
			: CustomProperty(parent) {
		}
		virtual ~PixmapIdCustomProperty() {};
		virtual void setValue(const QVariant &value, bool rememberOldValue) {}
		virtual QVariant value() const { return m_property->value(); }
		virtual bool handleValue() const {
			return false; //m_property->type()==KexiCustomPropertyFactory::PixmapData;
		}
};

//---------------

KexiCustomPropertyFactory::KexiCustomPropertyFactory(QObject* parent) 
: CustomPropertyFactory(parent)
{
}

KexiCustomPropertyFactory::~KexiCustomPropertyFactory()
{
}

CustomProperty* KexiCustomPropertyFactory::createCustomProperty(Property *parent)
{
	const int type = parent->type();
	if (type==(int)KexiCustomPropertyFactory::PixmapId)
		return new PixmapIdCustomProperty(parent);
	return 0;
}

Widget* KexiCustomPropertyFactory::createCustomWidget(Property *prop)
{
	return new KexiImagePropertyEdit(prop);
}


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
#include <kexiutils/identifier.h>

#include <koproperty/customproperty.h>
//Added by qt3to4:
#include <Q3ValueList>

using namespace KoProperty;

//! @internal
class PixmapIdCustomProperty : public CustomProperty
{
public:
    PixmapIdCustomProperty(Property *parent)
            : CustomProperty(parent) {
    }
    virtual ~PixmapIdCustomProperty() {}
    virtual void setValue(const QVariant &value, bool rememberOldValue) {
        Q_UNUSED(value);
        Q_UNUSED(rememberOldValue);
    }
    virtual QVariant value() const {
        return m_property->value();
    }
    virtual bool handleValue() const {
        return false; //m_property->type()==KexiCustomPropertyFactory::PixmapData;
    }
};

//! @internal
class IdentifierCustomProperty : public CustomProperty
{
public:
    IdentifierCustomProperty(Property *parent)
            : CustomProperty(parent) {
    }
    virtual ~IdentifierCustomProperty() {}
    virtual void setValue(const QVariant &value, bool rememberOldValue) {
        Q_UNUSED(rememberOldValue);
        if (!value.toString().isEmpty())
            m_value = KexiUtils::string2Identifier(value.toString()).toLower();
    }
    virtual QVariant value() const {
        return m_value;
    }
    virtual bool handleValue() const {
        return true;
    }
    QString m_value;
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
    if (type == (int)KexiCustomPropertyFactory::PixmapId)
        return new PixmapIdCustomProperty(parent);
    else if (type == (int)KexiCustomPropertyFactory::Identifier)
        return new IdentifierCustomProperty(parent);
    return 0;
}

Widget* KexiCustomPropertyFactory::createCustomWidget(Property *prop)
{
    const int type = prop->type();
    if (type == (int)KexiCustomPropertyFactory::PixmapId)
        return new KexiImagePropertyEdit(prop);
    else if (type == (int)KexiCustomPropertyFactory::Identifier)
        return new KexiIdentifierPropertyEdit(prop);

    return 0;
}

void KexiCustomPropertyFactory::init()
{
    if (KoProperty::FactoryManager::self()->factoryForEditorType(KexiCustomPropertyFactory::PixmapId))
        return; //already registered

    // register custom editors and properties
    KexiCustomPropertyFactory *factory = new KexiCustomPropertyFactory(KoProperty::FactoryManager::self());
    Q3ValueList<int> types;
    types << KexiCustomPropertyFactory::PixmapId << KexiCustomPropertyFactory::Identifier;
    KoProperty::FactoryManager::self()->registerFactoryForProperties(types, factory);
    KoProperty::FactoryManager::self()->registerFactoryForEditors(types, factory);
}

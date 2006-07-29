/***************************************************************************
 * kexidbfield.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/


#include "kexidbfield.h"

#include <api/variant.h>

using namespace Kross::KexiDB;

KexiDBField::KexiDBField(::KexiDB::Field* field)
    : Kross::Api::Class<KexiDBField>("KexiDBField")
    , m_field(field)
{
    this->addFunction0< Kross::Api::Variant >("type", this, &KexiDBField::type);
    this->addFunction1< void, Kross::Api::Variant >("setType", this, &KexiDBField::setType);

    this->addFunction0< Kross::Api::Variant >("subType", this, &KexiDBField::subType);
    this->addFunction1< void, Kross::Api::Variant >("setSubType", this, &KexiDBField::setSubType);

    this->addFunction0< Kross::Api::Variant >("variantType", this, &KexiDBField::variantType);
    this->addFunction0< Kross::Api::Variant >("typeGroup", this, &KexiDBField::typeGroup);

    this->addFunction0< Kross::Api::Variant >("isAutoInc", this, &KexiDBField::isAutoInc);
    this->addFunction1< void, Kross::Api::Variant >("setAutoInc", this, &KexiDBField::setAutoInc);

    this->addFunction0< Kross::Api::Variant >("isUniqueKey", this, &KexiDBField::isUniqueKey);
    this->addFunction1< void, Kross::Api::Variant >("setUniqueKey", this, &KexiDBField::setUniqueKey);

    this->addFunction0< Kross::Api::Variant >("isPrimaryKey", this, &KexiDBField::isPrimaryKey);
    this->addFunction1< void, Kross::Api::Variant >("setPrimaryKey", this, &KexiDBField::setPrimaryKey);

    this->addFunction0< Kross::Api::Variant >("isForeignKey", this, &KexiDBField::isForeignKey);
    this->addFunction1< void, Kross::Api::Variant >("setForeignKey", this, &KexiDBField::setForeignKey);

    this->addFunction0< Kross::Api::Variant >("isNotNull", this, &KexiDBField::isNotNull);
    this->addFunction1< void, Kross::Api::Variant >("setNotNull", this, &KexiDBField::setNotNull);

    this->addFunction0< Kross::Api::Variant >("isNotEmpty", this, &KexiDBField::isNotEmpty);
    this->addFunction1< void, Kross::Api::Variant >("setNotEmpty", this, &KexiDBField::setNotEmpty);

    this->addFunction0< Kross::Api::Variant >("isIndexed", this, &KexiDBField::isIndexed);
    this->addFunction1< void, Kross::Api::Variant >("setIndexed", this, &KexiDBField::setIndexed);

    this->addFunction0< Kross::Api::Variant >("isUnsigned", this, &KexiDBField::isUnsigned);
    this->addFunction1< void, Kross::Api::Variant >("setUnsigned", this, &KexiDBField::setUnsigned);

    this->addFunction0< Kross::Api::Variant >("name", this, &KexiDBField::name);
    this->addFunction1< void, Kross::Api::Variant >("setName", this, &KexiDBField::setName);

    this->addFunction0< Kross::Api::Variant >("caption", this, &KexiDBField::caption);
    this->addFunction1< void, Kross::Api::Variant >("setCaption", this, &KexiDBField::setCaption);

    this->addFunction0< Kross::Api::Variant >("description", this, &KexiDBField::description);
    this->addFunction1< void, Kross::Api::Variant >("setDescription", this, &KexiDBField::setDescription);

    this->addFunction0< Kross::Api::Variant >("length", this, &KexiDBField::length);
    this->addFunction1< void, Kross::Api::Variant >("setLength", this, &KexiDBField::setLength);

    this->addFunction0< Kross::Api::Variant >("precision", this, &KexiDBField::precision);
    this->addFunction1< void, Kross::Api::Variant >("setPrecision", this, &KexiDBField::setPrecision);

    this->addFunction0< Kross::Api::Variant >("width", this, &KexiDBField::width);
    this->addFunction1< void, Kross::Api::Variant >("setWidth", this, &KexiDBField::setWidth);

    this->addFunction0< Kross::Api::Variant >("defaultValue", this, &KexiDBField::defaultValue);
    this->addFunction1< void, Kross::Api::Variant >("setDefaultValue", this, &KexiDBField::setDefaultValue);
}

KexiDBField::~KexiDBField()
{
}

const QString KexiDBField::getClassName() const
{
    return "Kross::KexiDB::KexiDBField";
}

const QString KexiDBField::type() { return m_field->typeString(); }
void KexiDBField::setType(const QString type) { m_field->setType( ::KexiDB::Field::typeForString(type) ); }

const QString KexiDBField::subType() { return m_field->subType(); }
void KexiDBField::setSubType(const QString& subtype) { m_field->setSubType(subtype); }

const QString KexiDBField::variantType() { return QVariant::typeToName( m_field->variantType() ); }
const QString KexiDBField::typeGroup() { return m_field->typeGroupString(); }

bool KexiDBField::isAutoInc() { return m_field->isAutoIncrement(); }
void KexiDBField::setAutoInc(bool autoinc) { m_field->setAutoIncrement(autoinc); }

bool KexiDBField::isUniqueKey() { return m_field->isUniqueKey(); }
void KexiDBField::setUniqueKey(bool unique) { m_field->setUniqueKey(unique); }

bool KexiDBField::isPrimaryKey() { return m_field->isPrimaryKey(); }
void KexiDBField::setPrimaryKey(bool primary) { m_field->setPrimaryKey(primary); }

bool KexiDBField::isForeignKey() { return m_field->isForeignKey(); }
void KexiDBField::setForeignKey(bool foreign) { m_field->setForeignKey(foreign); }

bool KexiDBField::isNotNull() { return m_field->isNotNull(); }
void KexiDBField::setNotNull(bool notnull) { m_field->setNotNull(notnull); }

bool KexiDBField::isNotEmpty() { return m_field->isNotEmpty(); }
void KexiDBField::setNotEmpty(bool notempty) { m_field->setNotEmpty(notempty); }

bool KexiDBField::isIndexed() { return m_field->isIndexed(); }
void KexiDBField::setIndexed(bool indexed) { m_field->setIndexed(indexed); }

bool KexiDBField::isUnsigned() { return m_field->isUnsigned(); }
void KexiDBField::setUnsigned(bool isunsigned) { m_field->setUnsigned(isunsigned); }

const QString KexiDBField::name() { return m_field->name(); }
void KexiDBField::setName(const QString& name) { m_field->setName(name); }

const QString KexiDBField::caption() { return m_field->caption(); }
void KexiDBField::setCaption(const QString& caption) { m_field->setCaption(caption); }

const QString KexiDBField::description() { return m_field->description(); }
void KexiDBField::setDescription(const QString& desc) { m_field->setDescription(desc); }

uint KexiDBField::length() { return m_field->length(); }
void KexiDBField::setLength(uint length) { m_field->setLength(length); }

uint KexiDBField::precision() { return m_field->precision(); }
void KexiDBField::setPrecision(uint precision) { m_field->setPrecision(precision); }

uint KexiDBField::width() { return m_field->width(); }
void KexiDBField::setWidth(uint width) { m_field->setWidth(width); }

QVariant KexiDBField::defaultValue() { return m_field->defaultValue(); }
void KexiDBField::setDefaultValue(const QVariant& defaultvalue) { m_field->setDefaultValue(defaultvalue); }

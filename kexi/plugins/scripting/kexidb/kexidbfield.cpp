/***************************************************************************
 * kexidbfield.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2006 by Sebastian Sauer (mail@dipe.org)
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

using namespace Scripting;

KexiDBField::KexiDBField(QObject* parent, ::KexiDB::Field* field, bool owner)
        : QObject(parent)
        , m_field(field)
        , m_owner(owner)
{
    setObjectName("KexiDBField");
}

KexiDBField::~KexiDBField()
{
    if (m_owner)
        delete m_field;
}

const QString KexiDBField::type()
{
    return m_field->typeString();
}
void KexiDBField::setType(const QString type)
{
    m_field->setType(::KexiDB::Field::typeForString(type));
}

const QString KexiDBField::subType()
{
    return m_field->subType();
}
void KexiDBField::setSubType(const QString& subtype)
{
    m_field->setSubType(subtype);
}

const QString KexiDBField::variantType()
{
    return QVariant::typeToName(m_field->variantType());
}
const QString KexiDBField::typeGroup()
{
    return m_field->typeGroupString();
}

bool KexiDBField::isAutoInc()
{
    return m_field->isAutoIncrement();
}
void KexiDBField::setAutoInc(bool autoinc)
{
    m_field->setAutoIncrement(autoinc);
}

bool KexiDBField::isUniqueKey()
{
    return m_field->isUniqueKey();
}
void KexiDBField::setUniqueKey(bool unique)
{
    m_field->setUniqueKey(unique);
}

bool KexiDBField::isPrimaryKey()
{
    return m_field->isPrimaryKey();
}
void KexiDBField::setPrimaryKey(bool primary)
{
    m_field->setPrimaryKey(primary);
}

bool KexiDBField::isForeignKey()
{
    return m_field->isForeignKey();
}
void KexiDBField::setForeignKey(bool foreign)
{
    m_field->setForeignKey(foreign);
}

bool KexiDBField::isNotNull()
{
    return m_field->isNotNull();
}
void KexiDBField::setNotNull(bool notnull)
{
    m_field->setNotNull(notnull);
}

bool KexiDBField::isNotEmpty()
{
    return m_field->isNotEmpty();
}
void KexiDBField::setNotEmpty(bool notempty)
{
    m_field->setNotEmpty(notempty);
}

bool KexiDBField::isIndexed()
{
    return m_field->isIndexed();
}
void KexiDBField::setIndexed(bool indexed)
{
    m_field->setIndexed(indexed);
}

bool KexiDBField::isUnsigned()
{
    return m_field->isUnsigned();
}
void KexiDBField::setUnsigned(bool isunsigned)
{
    m_field->setUnsigned(isunsigned);
}

const QString KexiDBField::name()
{
    return m_field->name();
}
void KexiDBField::setName(const QString& name)
{
    m_field->setName(name);
}

const QString KexiDBField::caption()
{
    return m_field->caption();
}
void KexiDBField::setCaption(const QString& caption)
{
    m_field->setCaption(caption);
}

const QString KexiDBField::description()
{
    return m_field->description();
}
void KexiDBField::setDescription(const QString& desc)
{
    m_field->setDescription(desc);
}

uint KexiDBField::length()
{
    return m_field->length();
}
void KexiDBField::setLength(uint length)
{
    m_field->setLength(length);
}

uint KexiDBField::precision()
{
    return m_field->precision();
}
void KexiDBField::setPrecision(uint precision)
{
    m_field->setPrecision(precision);
}

uint KexiDBField::width()
{
    return m_field->width();
}
void KexiDBField::setWidth(uint width)
{
    m_field->setWidth(width);
}

QVariant KexiDBField::defaultValue()
{
    return m_field->defaultValue();
}
void KexiDBField::setDefaultValue(const QVariant& defaultvalue)
{
    m_field->setDefaultValue(defaultvalue);
}

#include "kexidbfield.moc"

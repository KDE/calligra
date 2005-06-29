/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2004  Jaroslaw Staniek <js@iidea.pl>

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

#include "property.h"
#include "customproperty.h"
#include "set.h"
#include "factory.h"

#ifndef QT_ONLY
#include <kdebug.h>
#endif

#include <qobject.h>
#include <qasciidict.h>

namespace KOProperty {

QT_STATIC_CONST_IMPL Property Property::null;

class PropertyPrivate
{
    public:
        PropertyPrivate()
        :  valueList(0), custom(0), useCustomProperty(true),
         parent(0), children(0), relatedProperties(0)
        {}

        ~PropertyPrivate()
        {
            delete children;
            delete relatedProperties;
            delete custom;
        }

    int type;
    QCString name;
    QString  caption;
    QString description;
    QVariant value;
    QVariant oldValue;
    /*! The string-to-value correspondence list of the property.*/
    QMap<QString, QVariant> *valueList;
    QString icon;

    bool changed : 1;
    bool storable : 1;
    bool readOnly : 1;
    bool visible : 1;
    int autosync;
    QMap<QCString, QVariant> options;

    CustomProperty *custom;
    //! Flag used to allow CustomProperty to use setValue()
    bool useCustomProperty;

    QValueList<Set*>  lists;
    Property  *parent;
    QValueList<Property*>  *children;
    //! list of properties with the same name (when intersecting buffers)
    QValueList<Property*>  *relatedProperties;
};


QMap<QString, QVariant>
createValueListFromStringLists(const QStringList &keys, const QStringList &values)
{
	QMap<QString, QVariant> map;
	if(keys.count() != values.count())
		return map;

	QStringList::ConstIterator valueIt = values.begin();
	QStringList::ConstIterator endIt = keys.constEnd();
	for(QStringList::ConstIterator it = keys.begin(); it != endIt; ++it, ++valueIt)
		map.insert( *it, *valueIt);

	return map;
}



Property::Property(const QCString &name, const QString &caption, const QString &desc,
        const QVariant &value, int type)
{
    d = new PropertyPrivate();

    d->name = name;
    d->caption = caption;
    d->description = desc;
    d->value = value;
    d->visible = true;
    d->storable = true;
    d->readOnly = false;
    d->changed = false;
    d->autosync = -1;

    if(type == Auto)
        d->type = value.type();
    else
        d->type = type;

    d->custom = Factory::getInstance()->customPropertyForProperty(this);
}

Property::Property(const QCString &name, const QString &caption, const QString &desc,
        const QMap<QString, QVariant> &valueList, const QVariant &value, int type)
{
    d = new PropertyPrivate();

    d->name = name;
    d->caption = caption;
    d->description = desc;
    d->value = value;
    d->visible = true;
    d->storable = true;
    d->readOnly = false;
    d->changed = false;
    d->autosync = -1;
    d->type = type;
    d->valueList = new QMap<QString, QVariant>();
    *(d->valueList) = valueList;

    d->custom = Factory::getInstance()->customPropertyForProperty(this);
}

Property::Property()
{
    d = new PropertyPrivate();

    d->visible = true;
    d->storable = true;
    d->readOnly = false;
    d->changed = false;
    d->autosync = -1;
}

Property::Property(const Property &prop)
{
    d = new PropertyPrivate();
    *this = prop;
}

Property::~Property()
{
    delete d;
}

QCString
Property::name() const
{
    return d->name;
}

void
Property::setName(const QCString &name)
{
    d->name = name;
}

QString
Property::caption() const
{
    return d->caption;
}

void
Property::setCaption(const QString &caption)
{
    d->caption = caption;
}

QString
Property::description() const
{
    return d->description;
}

void
Property::setDescription(const QString &desc)
{
    d->description = desc;
}

int
Property::type() const
{
    return d->type;
}

void
Property::setType(int type)
{
    d->type = type;
}

QString
Property::icon() const
{
    return d->icon;
}

void
Property::setIcon(const QString &icon)
{
    d->icon = icon;
}

QVariant
Property::value() const
{
    if(d->custom && d->custom->handleValue())
        return d->custom->value();
    return d->value;
}

QVariant
Property::oldValue() const
{
    if(d->oldValue.isNull())
        return d->value;
    else
        return d->oldValue;
}

void
Property::setValue(const QVariant &value, bool rememberOldValue, bool useCustomProperty)
{
    kdDebug(100300) << d->name << " : setValue('" << value.toString() << "' type=" << type() << ")" << endl;
    if (d->value.type() != value.type() && !d->value.isNull() && !value.isNull()
         && !( (d->value.type()==QVariant::Int && value.type()==QVariant::UInt)
               || (d->value.type()==QVariant::UInt && value.type()==QVariant::Int)
               || (d->value.type()==QVariant::CString && value.type()==QVariant::String)
               || (d->value.type()==QVariant::String && value.type()==QVariant::CString)
         )) {
        kdDebug(100300) << "INCOMPAT TYPES! " << d->value.typeName() << " and " << value.typeName() << endl;
    }

    //1. Check if the value should be changed
    bool ch;
    if (d->value.type() == QVariant::DateTime
        || d->value.type() == QVariant::Time) {
        //for date and datetime types: compare with strings, because there
        //can be miliseconds difference
        ch = (d->value.toString() != value.toString());
    }
    else if (d->value.type() == QVariant::String || d->value.type()==QVariant::CString) {
        //property is changed for string type,
        //if one of value is empty and other isn't..
        ch = ( (d->value.toString().isEmpty() != value.toString().isEmpty())
        //..or both are not empty and values differ
            || (!d->value.toString().isEmpty() && !value.toString().isEmpty() && d->value != value) );
    }
    else
        ch = (d->value != value);

    if (!ch)
        return;

    //2. Then change it, and store old value if necessary
    if(rememberOldValue) {
        if(!d->changed)
            d->oldValue = d->value;
        d->changed = true;
    }
    else {
        d->oldValue = QVariant(); // clear old value
        d->changed = false;
    }
    if(d->custom && useCustomProperty)
        d->custom->setValue(value, rememberOldValue);
    d->value = value;

    QValueList<Set*>::ConstIterator endIt = d->lists.constEnd();
    for(QValueList<Set*>::ConstIterator it = d->lists.constBegin(); it != endIt; ++it) {
        emit (*it)->propertyChanged(this, *it);
        emit (*it)->propertyChanged();
    }
}

void
Property::resetValue()
{
    d->changed = false;
    setValue(oldValue(), false);
    QValueList<Set*>::ConstIterator endIt = d->lists.constEnd();
    for(QValueList<Set*>::ConstIterator it = d->lists.constBegin(); it != endIt; ++it)
        emit (*it)->propertyReset(this, *it);
}

const QMap<QString, QVariant>*
Property::valueList() const
{
    return d->valueList;
}

void
Property::setValueList(const QMap<QString, QVariant> &list)
{
    if(!d->valueList)
        d->valueList = new QMap<QString, QVariant>();
    *(d->valueList) = list;
}

////////////////////////////////////////////////////////////////

bool
Property::isNull() const
{
    return d->name.isEmpty();
}

bool
Property::isModified() const
{
    return d->changed;
}

bool
Property::isReadOnly() const
{
    return d->readOnly;
}

void
Property::setReadOnly(const bool readOnly)
{
    d->readOnly = readOnly;
}

bool
Property::isVisible() const
{
    return d->visible;
}

void
Property::setVisible(const bool visible)
{
    d->visible = visible;
}

int
Property::isAutoSync() const
{
    return d->autosync;
}

void
Property::setAutoSync(int sync)
{
    d->autosync = sync;
}

bool
Property::isStorable() const
{
    return d->storable;
}

void
Property::setStorable(bool storable)
{
    d->storable = storable;
}

void
Property::setOption(const char* name, const QVariant& val)
{
    d->options[name] = val;
}

QVariant
Property::option(const char* name) const
{
    if (d->options.contains(name))
        return d->options[name];
    return QVariant();
}

bool
Property::hasOptions() const
{
    return !d->options.isEmpty();
}

/////////////////////////////////////////////////////////////////

Property::operator bool () const
{
    return !isNull();
}

const Property&
Property::operator= (const QVariant& val)
{
    setValue(val);
    return *this;
}

const Property&
Property::operator= (const Property &property)
{
    if(&property == this)
        return *this;

    if(d->valueList) {
        delete d->valueList;
        d->valueList = 0;
    }
    if(d->children) {
        delete d->children;
        d->children = 0;
    }
    if(d->relatedProperties) {
        delete d->relatedProperties;
        d->relatedProperties = 0;
    }
    if(d->custom) {
        delete d->custom;
        d->custom = 0;
    }

    d->name = property.d->name;
    d->caption = property.d->caption;
    d->description = property.d->description;
    d->type = property.d->type;
    d->value = property.d->value;

    d->icon = property.d->icon;
    d->autosync = property.d->autosync;
    d->visible = property.d->visible;
    d->storable = property.d->storable;
    d->readOnly = property.d->readOnly;
    d->options = property.d->options;

    if(property.d->valueList) {
        d->valueList = new QMap<QString, QVariant>(*(property.d->valueList));
    }
    if(property.d->children) {
        if(property.d->custom) {
            d->custom = Factory::getInstance()->customPropertyForProperty(this);
            // updates all children value, using CustomProperty
            setValue(property.d->value);
        }
        else {
            // no CustomProperty (should never happen), simply copy all children
            QValueList<Property*>::ConstIterator endIt = d->children->constEnd();
            for(QValueList<Property*>::ConstIterator it = d->children->constBegin(); it != endIt; ++it) {
                Property *child = new Property( *(*it) );
                addChild(child);
            }
        }
    }

    if(property.d->relatedProperties) {
        d->relatedProperties = new QValueList<Property*>( *(property.d->relatedProperties));
    }

    // update these later because they may have been changed when creating children
    d->oldValue = property.d->oldValue;
    d->changed = property.d->changed;

    return *this;
}

bool
Property::operator ==(const Property &prop) const
{
    return ((d->name == prop.d->name) && (d->value == prop.d->value));
}

/////////////////////////////////////////////////////////////////

const QValueList<Property*>*
Property::children() const
{
    return d->children;
}

Property*
Property::child(const QCString &name)
{
    QValueList<Property*>::ConstIterator endIt = d->children->constEnd();
    for(QValueList<Property*>::ConstIterator it = d->children->constBegin(); it != endIt; ++it) {
        if((*it)->name() == name)
            return *it;
    }

    return 0;
}

Property*
Property::parent() const
{
    return d->parent;
}

void
Property::addChild(Property *prop)
{
    if(!d->children)
        d->children = new QValueList<Property*>();

    QValueList<Property*>::ConstIterator it = qFind( d->children->begin(), d->children->end(), prop);
    if(it == d->children->end()) // not in our list
        d->children->append(prop);
    prop->d->parent = this;
}

QValueList<Set*>
Property::lists() const
{
    return d->lists;
}

void
Property::addList(Set *list)
{
    QValueList<Set*>::iterator it = qFind( d->lists.begin(), d->lists.end(), list);
    if(it == d->lists.end()) // not in our list
        d->lists.append(list);
}

const QValueList<Property*>*
Property::related() const
{
    return d->relatedProperties;
}

void
Property::addRelatedProperty(Property *property)
{
    if(!d->relatedProperties)
        d->relatedProperties = new QValueList<Property*>();

    QValueList<Property*>::iterator it = qFind( d->relatedProperties->begin(), d->relatedProperties->end(), property);
    if(it == d->relatedProperties->end()) // not in our list
        d->relatedProperties->append(property);
}

void
Property::setCustomProperty(CustomProperty *prop)
{
    d->custom = prop;
}

/////////////////////////////////////////////////////////////////

void
Property::debug()
{
    QString dbg = "KexiProperty( name='" + QString(d->name) + "' desc='" + d->description
        + "' val=" + (d->value.isValid() ? d->value.toString() : "<INVALID>");
    if (!d->oldValue.isValid())
        dbg += (", oldVal='" + d->oldValue.toString() + "'");
    dbg += (QString(d->changed ? " " : " un") + "changed");
    dbg += (d->visible ? " visible" : " hidden");
    dbg+=" )";

    kdDebug(100300) << dbg << endl;
}

}

/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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

namespace KoProperty {

QT_STATIC_CONST_IMPL Property Property::null;

//! @internal
class PropertyPrivate
{
	public:
		PropertyPrivate()
		:  listData(0), changed(false), storable(true), readOnly(false), visible(true),
		 autosync(-1), custom(0), useCustomProperty(true),
		 parent(0), children(0), relatedProperties(0),
		 sortingKey(0)
		{
		}

		~PropertyPrivate()
		{
			delete children;
			delete relatedProperties;
			delete custom;
		}

	int type;
	QCString name;
	QString caption;
	QString description;
	QVariant value;
	QVariant oldValue;
	/*! The string-to-value correspondence list of the property.*/
	Property::ListData* listData;
//	QMap<QString, QVariant> *valueList;
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

	QValueList<Set*>  sets;
	Property  *parent;
	QValueList<Property*>  *children;
	//! list of properties with the same name (when intersecting buffers)
	QValueList<Property*>  *relatedProperties;

	int sortingKey;
};
}

using namespace KoProperty;

/////////////////////////////////////////////////////////////////

Property::ListData::ListData(const QStringList& keys_, const QStringList& names_)
 : names(names_)
// , fixed(true)
{
	setKeysAsStringList(keys_);
}

Property::ListData::ListData(const QValueList<QVariant> keys_, const QStringList& names_)
 : keys(keys_), names(names_)
// , fixed(true)
{
}

Property::ListData::ListData()
// : fixed(true)
{
}

Property::ListData::~ListData()
{
}

void Property::ListData::setKeysAsStringList(const QStringList& list)
{
	keys.clear();
	for (QStringList::ConstIterator it = list.constBegin(); it!=list.constEnd(); ++it) {
		keys.append(*it);
	}
}

QStringList Property::ListData::keysAsStringList() const
{
	QStringList result;
	for (QValueList<QVariant>::ConstIterator it = keys.constBegin(); it!=keys.constEnd(); ++it) {
		result.append((*it).toString());
	}
	return result;
}

/////////////////////////////////////////////////////////////////

/*
KOPROPERTY_EXPORT QMap<QString, QVariant>
KoProperty::createValueListFromStringLists(const QStringList &keys, const QStringList &values)
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
*/


Property::Property(const QCString &name, const QVariant &value,
	const QString &caption, const QString &description,
	int type)
 : d( new PropertyPrivate() )
{
	d->name = name;
	d->caption = caption;
	d->description = description;
	d->value = value;

	if(type == Auto)
		d->type = value.type();
	else
		d->type = type;

	d->custom = Factory::getInstance()->customPropertyForProperty(this);
}

Property::Property(const QCString &name, const QStringList &keys, const QStringList &strings,
	const QVariant &value, const QString &caption, const QString &description, int type)
 : d( new PropertyPrivate() )
{
	d->name = name;
	d->caption = caption;
	d->description = description;
	d->value = value;
	d->type = type;
	setListData(keys, strings);
//	d->valueList = new QMap<QString, QVariant>();
//	*(d->valueList) = createValueListFromStringLists(keys, values);

	d->custom = Factory::getInstance()->customPropertyForProperty(this);
}

Property::Property(const QCString &name, ListData* listData, //const QMap<QString, QVariant> &v_valueList,
	const QVariant &value, const QString &caption, const QString &description, int type)
 : d( new PropertyPrivate() )
{
	d->name = name;
	d->caption = caption;
	d->description = description;
	d->value = value;
	d->type = type;
	d->listData = listData;
//	d->valueList = new QMap<QString, QVariant>();
//	*(d->valueList) = v_valueList;

	d->custom = Factory::getInstance()->customPropertyForProperty(this);
}

Property::Property()
 : d( new PropertyPrivate() )
{
}

Property::Property(const Property &prop)
 : d( new PropertyPrivate() )
{
	*this = prop;
}

Property::~Property()
{
	delete d;
	d = 0;
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
	if (d->name.isEmpty()) {
		kopropertywarn << "Property::setValue(): COULD NOT SET value to a null property" << endl;
		return;
	}

//	kopropertydbg << d->name << " : setValue('" << value.toString() << "' type=" << type() << ")" << endl;
	if (d->value.type() != value.type() && !d->value.isNull() && !value.isNull()
		 && !( (d->value.type()==QVariant::Int && value.type()==QVariant::UInt)
			   || (d->value.type()==QVariant::UInt && value.type()==QVariant::Int)
			   || (d->value.type()==QVariant::CString && value.type()==QVariant::String)
			   || (d->value.type()==QVariant::String && value.type()==QVariant::CString)
		 )) {
		kopropertywarn << "Property::setValue(): INCOMPAT TYPES! " << d->value.typeName() << " and " << value.typeName() << endl;
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
	QVariant prevValue;
	if(d->custom && useCustomProperty) {
		d->custom->setValue(value, rememberOldValue);
		prevValue = d->custom->value();
	}
	else
		prevValue = d->value;
	d->value = value;

	QValueList<Set*>::ConstIterator endIt = d->sets.constEnd();
	for(QValueList<Set*>::ConstIterator it = d->sets.constBegin(); it != endIt; ++it) {
		emit (*it)->propertyChanged(**it, *this, prevValue);
		emit (*it)->propertyChanged(**it, *this);
		emit (*it)->propertyChanged();
	}
}

void
Property::resetValue()
{
	d->changed = false;
	setValue(oldValue(), false);
	// maybe parent  prop is also unchanged now
	if(d->parent && d->parent->value() == d->parent->oldValue())
		d->parent->d->changed = false;

	QValueList<Set*>::ConstIterator endIt = d->sets.constEnd();
	for(QValueList<Set*>::ConstIterator it = d->sets.constBegin(); it != endIt; ++it)
		emit (*it)->propertyReset(**it, *this);
}

//const QMap<QString, QVariant>*
Property::ListData*
Property::listData() const
{
	return d->listData;
}

void
Property::setListData(ListData* list) //const QMap<QString, QVariant> &list)
{
//	if(!d->valueList)
//		d->valueList = new QMap<QString, QVariant>();
	if (list == d->listData)
		return;
	delete d->listData;
	d->listData = list;
}

void
Property::setListData(const QStringList &keys, const QStringList &names)
{
	ListData* list = new ListData(keys, names);
	setListData(list);

//	if(!d->valueList)
//		d->valueList = new QMap<QString, QVariant>();
//	*(d->valueList) = createValueListFromStringLists(keys, values);
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
Property::autoSync() const
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

	if(d->listData) {
		delete d->listData;
		d->listData = 0;
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

	if(property.d->listData) {
		d->listData = new ListData(*property.d->listData); //QMap<QString, QVariant>(*(property.d->valueList));
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
	if(it == d->children->end()) { // not in our list
		d->children->append(prop);
		prop->setSortingKey(d->children->count());
	}
	prop->d->parent = this;
}

QValueList<Set*>
Property::sets() const
{
	return d->sets;
}

void
Property::addSet(Set *set)
{
	QValueList<Set*>::iterator it = qFind( d->sets.begin(), d->sets.end(), set);
	if(it == d->sets.end()) // not in our list
		d->sets.append(set);
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

int Property::sortingKey() const
{
	return d->sortingKey;
}

void Property::setSortingKey(int key)
{
	d->sortingKey = key;
}

/////////////////////////////////////////////////////////////////

void
Property::debug()
{
	QString dbg = "Property( name='" + QString(d->name) + "' desc='" + d->description
		+ "' val=" + (d->value.isValid() ? d->value.toString() : "<INVALID>");
	if (!d->oldValue.isValid())
		dbg += (", oldVal='" + d->oldValue.toString() + "'");
	dbg += (QString(d->changed ? " " : " un") + "changed");
	dbg += (d->visible ? " visible" : " hidden");
	dbg+=" )";

	kopropertydbg << dbg << endl;
}

/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "set.h"
#include "property.h"

#include <qapplication.h>
#include <qasciidict.h>
//#include <qvaluelist.h>

#ifdef QT_ONLY
// \todo
#else
#include <kdebug.h>
#include <klocale.h>
#endif

namespace KoProperty {

//! @internal
static Property Set_nonConstNull;

//! @internal
class SetPrivate
{
	public:
		SetPrivate() : dict(101, false), readOnly(false) {}
		~SetPrivate(){}

	//dict of properties in form name: property
	Property::Dict dict;
//	PropertyList properties;
	//groups of properties:
	// list of group name: (list of property names)
	StringListMap propertiesOfGroup;
	QMap<QCString, QString>  groupsDescription;
	// map of property: group
	QMap<Property*, QCString> groupForProperty;

	bool ownProperty : 1;
	bool readOnly : 1;
//	static Property nonConstNull;
	QCString prevSelection;
	QString typeName;

	inline KoProperty::Property& property(const QCString &name) const
	{
		KoProperty::Property *p = dict.find(name);
		if (p)
			return *p;
		Set_nonConstNull.setName(0); //to ensure returned property is null
		kopropertywarn << "Set::property(): PROPERTY \"" << name << "\" NOT FOUND" << endl;
		return Set_nonConstNull;
	}
};

}

using namespace KoProperty;

//Set::Iterator class
Set::Iterator::Iterator(const Set &set)
{
	iterator = new Property::DictIterator(set.d->dict);
}

Set::Iterator::~Iterator()
{
}

void
Set::Iterator::operator ++()
{
	++(*iterator);
}

Property*
Set::Iterator::operator *()
{
	return current();
}

QCString
Set::Iterator::currentKey()
{
	if (iterator)
		return iterator->currentKey();

	return QCString();
}

Property*
Set::Iterator::current()
{
	if(iterator)
		return iterator->current();

	return 0;
}

 //////////////////////////////////////////////

Set::Set(QObject *parent, const QString &typeName)
: QObject(parent, typeName.latin1())
{
	d = new SetPrivate();
	d->ownProperty = true;
	d->groupsDescription.insert("common", i18n("General properties", "General"));
	d->typeName = typeName;
}


Set::Set(const Set &l)
 : QObject(l.parent(), l.name())
{
	d = new SetPrivate();
	*this = l;
}

Set::Set(bool propertyOwner)
 : QObject(0, 0)
{
	d = new SetPrivate();
	d->ownProperty = propertyOwner;
	d->groupsDescription.insert("common", i18n("General properties", "General"));
}

Set::~Set()
{
	emit aboutToBeCleared();
	emit aboutToBeDeleted();
	clear();
	delete d;
}

/////////////////////////////////////////////////////

void
Set::addProperty(Property *property, QCString group)
{
	if (group.isEmpty())
		group = "common";
	if (property == 0) {
		kopropertywarn << "Set::addProperty(): property == 0" << endl; 
		return;
	}
	if (property->name().isEmpty()) {
		kopropertywarn << "Set::addProperty(): COULD NOT ADD NULL PROPERTY" << endl; 
		return;
	}

	if(d->dict.find(property->name())) {
		Property *p = d->dict[property->name()];
		p->addRelatedProperty(property);
	}
	else {
		d->dict.insert(property->name(), property);
		addToGroup(group, property);
	}

	property->addSet(this);
	property->setSortingKey( d->dict.count() );
}

void
Set::removeProperty(Property *property)
{
	if(!property)
		return;

	Property *p = d->dict.take(property->name());
	removeFromGroup(p);
	if(d->ownProperty) {
		emit aboutToDeleteProperty(*this, *p);
		delete p;
	}
}

void
Set::removeProperty(const QCString &name)
{
	if(name.isNull())
		return;

	Property *p = d->dict.find(name);
	removeProperty(p);
}

void
Set::clear()
{
	aboutToBeCleared();
	Property::DictIterator it(d->dict);
	while (it.current())
		removeProperty( it.current() );
}

/////////////////////////////////////////////////////

void
Set::addToGroup(const QCString &group, Property *property)
{
	if(!property)
		return;

	//do not add same property to the group twice
	if(d->groupForProperty.contains(property) && (d->groupForProperty[property] == group))
		return;

	if(!d->propertiesOfGroup.contains(group)) { // group doesn't exist
		QValueList<QCString> l;
		l.append(property->name());
		d->propertiesOfGroup.insert(group, l);
	}
	else {
		d->propertiesOfGroup[group].append(property->name());
	}
	d->groupForProperty.insert(property, group);
}

void
Set::removeFromGroup(Property *property)
{
	if(!property)
		return;
	QCString group = d->groupForProperty[property];
	d->propertiesOfGroup[group].remove(property->name());
	d->groupForProperty.remove(property);
}

const StringListMap&
Set::groups()
{
	return d->propertiesOfGroup;
}

void
Set::setGroupDescription(const QCString &group, const QString desc)
{
	d->groupsDescription[group] = desc;
}

QString
Set::groupDescription(const QCString &group)
{
	if(d->groupsDescription.contains(group))
		return d->groupsDescription[group];
	return group;
}

/////////////////////////////////////////////////////

uint
Set::count() const
{
	return d->dict.count();
}

bool
Set::isEmpty() const
{
	return d->dict.isEmpty();
}

bool
Set::isReadOnly() const
{
	return d->readOnly;
}

void
Set::setReadOnly(bool readOnly)
{
	d->readOnly = readOnly;
}

bool
Set::contains(const QCString &name) const
{
	return d->dict.find(name);
}

Property&
Set::property(const QCString &name) const
{
	return d->property(name);
}

Property&
Set::operator[](const QCString &name) const
{
	return d->property(name);
}

const Set&
Set::operator= (const Set &l)
{
	if(&l == this)
		return *this;

	d->dict.clear();
	d->groupForProperty.clear();

	d->ownProperty = l.d->ownProperty;
	d->prevSelection = l.d->prevSelection;
	d->groupsDescription = l.d->groupsDescription;
	d->propertiesOfGroup = l.d->propertiesOfGroup;

	// Copy all properties in the list
	for(Property::DictIterator it(l.d->dict); it.current(); ++it) {
		Property *prop = new Property( *it.current() );
		addProperty(prop, l.d->groupForProperty[ it.current() ] );
	}

	return *this;
}

void
Set::changeProperty(const QCString &property, const QVariant &value)
{
	Property *p = d->dict[property];
	if(p)
		p->setValue(value);
}

/////////////////////////////////////////////////////

void
Set::debug()
{
	//kopropertydbg << "List: typeName='" << m_typeName << "'" << endl;
	if(d->dict.isEmpty()) {
		kopropertydbg << "<EMPTY>" << endl;
		return;
	}
	kopropertydbg << d->dict.count() << " properties:" << endl;

	for(Property::DictIterator it(d->dict); it.current(); ++it)
		it.current()->debug();
}

QCString
Set::prevSelection() const
{
	return d->prevSelection;
}

void
Set::setPrevSelection(const QCString &prevSelection)
{
	d->prevSelection = prevSelection;
}

QString
Set::typeName() const
{
	return d->typeName;
}

/////////////////////////////////////////////////////

Buffer::Buffer()
	:Set(false)
{
	connect( this, SIGNAL( propertyChanged( KoProperty::Set&, KoProperty::Property& ) ),
	         this, SLOT(intersectedChanged( KoProperty::Set&, KoProperty::Property& ) ) );

	connect( this, SIGNAL( propertyReset( KoProperty::Set&, KoProperty::Property& ) ),
	         this, SLOT(intersectedReset( KoProperty::Set&, KoProperty::Property& ) ) );
}

Buffer::Buffer(const Set *set)
	:Set(false)
{
	connect( this, SIGNAL( propertyChanged( KoProperty::Set&, KoProperty::Property& ) ),
	         this, SLOT(intersectedChanged( KoProperty::Set&, KoProperty::Property& ) ) );

	connect( this, SIGNAL( propertyReset( KoProperty::Set&, KoProperty::Property& ) ),
	         this, SLOT(intersectedReset( KoProperty::Set&, KoProperty::Property& ) ) );

	initialSet( set );
}

void Buffer::initialSet(const Set *set)
{
	//deep copy of set
	for(Property::DictIterator it(set->d->dict); it.current(); ++it) {
		Property *prop = new Property( *it.current() );
		QCString group = set->d->groupForProperty[it.current()];
		QString groupDesc = set->d->groupsDescription[ group ];
		setGroupDescription( group, groupDesc );
		addProperty( prop, group );
		prop->addRelatedProperty( it.current() );
	}
}

void Buffer::intersect(const Set *set)
{
	if ( d->dict.isEmpty() )
	{
		initialSet( set );
		return;
	}

	 for(Property::DictIterator it(d->dict); it.current(); ++it) {
		const char* key = it.current()->name();
		if ( Property *property =  set->d->dict[ key ] )
		{
				blockSignals( true );
				it.current()->resetValue();
				it.current()->addRelatedProperty( property );
				blockSignals( false );
		}
		else
			removeProperty( key );
	}
}

void Buffer::intersectedChanged(KoProperty::Set& set, KoProperty::Property& prop)
{
	Q_UNUSED(set);
	QCString propertyName = prop.name();
	if ( !contains( propertyName ) )
		return;

	const QValueList<Property*> *props = prop.related();
	QValueList<Property*>::const_iterator it = props->begin();
	for ( ; it != props->end(); ++it ) {
		( *it )->setValue( prop.value(), false );
	}
}

void Buffer::intersectedReset(KoProperty::Set& set, KoProperty::Property& prop)
{
	Q_UNUSED(set);
	QCString propertyName = prop.name();
	if ( !contains( propertyName ) )
		return;

	const QValueList<Property*> *props = prop.related();
	QValueList<Property*>::const_iterator it = props->begin();
	for ( ; it != props->end(); ++it )  {
		( *it )->setValue( prop.value(), false );
	}
}

#include "set.moc"

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

#include "set.h"
#include "property.h"

#include <qasciidict.h>
//#include <qvaluelist.h>

#ifdef QT_ONLY
// \todo
#else
#include <kdebug.h>
#include <klocale.h>
#endif

namespace KOProperty {

class KPROPERTY_EXPORT SetPrivate
{
    public:
        SetPrivate() : dict(101, false){}
        ~SetPrivate(){}

    //dict of properties in form name: property
     Property::Dict dict;
//    PropertyList properties;
    //groups of properties:
    // list of group name: (list of property names)
    StringListMap propertiesOfGroup;
    QMap<QCString, QString>  groupsDescription;
    // map of property: group
    QMap<Property*, QCString> groupForProperty;

    bool ownProperty : 1;
    static Property nonConstNull;
    QCString prevSelection;
    QString typeName;
/*
    bool contains(const QCString &name)
    {
        PropertyList::iterator it = properties.begin();
        for( ; it != properties.end(); ++it )
            if ( ( *it )->name() == name )
                return true;

        return false;
    }

    Property* operator[](const QCString &name)
    {
        PropertyList::iterator it = properties.begin();
        for( ; it != properties.end(); ++it )
            if ( ( *it )->name() == name )
                return ( *it );

        return 0L;
    }

    Property* take(const QCString &name)
    {
        Property *p = 0L;
        PropertyList::iterator it = properties.begin();
        for( ; it != properties.end(); ++it )
            if ( ( *it )->name() == name )
            {
                p = ( *it );
                properties.remove( it );
            }
        return p;
    }
*/
};

//Set::Iterator class
Set::Iterator::Iterator(const Set &list)
{
    iterator = new Property::DictIterator(list.d->dict);
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
    else
        return QCString();
}

Property*
Set::Iterator::current()
{
    if(iterator)
        return iterator->current();
    else
        return 0;
}

 //////////////////////////////////////////////

Set::Set(QObject *parent, const QString &typeName)
: QObject(parent, typeName.latin1())
{
    d = new SetPrivate();
    d->ownProperty = true;
    d->groupsDescription.insert("common", i18n("General"));
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
    d->groupsDescription.insert("common", i18n("General"));
}

Set::~Set()
{
    emit aboutToBeDeleted();
    clear();
    delete d;
}

/////////////////////////////////////////////////////

void
Set::addProperty(Property *property, QCString group)
{
    if (property == 0)
        return;

     if(d->dict.find(property->name())) {
         Property *p = d->dict[property->name()];
        p->addRelatedProperty(property);
    }
    else {
        d->dict.insert(property->name(), property);
        addToGroup(group, property);
    }

    property->addList(this);
}

void
Set::removeProperty(Property *property)
{
    if(!property)
        return;

    removeProperty(property->name());
}

void
Set::removeProperty(const QCString &name)
{
    if(name.isNull())
        return;

    Property *p = d->dict.take(name);
    removeFromGroup(p);
    if(d->ownProperty) {
        emit aboutToDeleteProperty(p, this);
        delete p;
    }
}

void
Set::clear()
{
    for(Property::DictIterator it(d->dict); it.current(); ++it)
        removeProperty( it.current()->name() );
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
Set::contains(const QCString &name)
{
    return d->dict.find(name);
}

Property&
Set::property(const QCString &name)
{
    Property *p = d->dict[name];
    if(!p) {
        p = new Property();
        //addProperty(p); // maybe just return a null property
    }

    return *p;
}

Property&
Set::operator[](const QCString &name)
{
    return property(name);
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
    //kdDebug(45000) << "List: typeName='" << m_typeName << "'" << endl;
    if(d->dict.isEmpty()) {
        kdDebug(100300) << "<EMPTY>" << endl;
        return;
    }
    kdDebug(100300) << d->dict.count() << " properties:" << endl;

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


Buffer::Buffer()
    :Set(false)
{
    connect( this, SIGNAL( propertyChanged( Property*, Set* ) ),
              this, SLOT(intersectedChanged( Property*, Set* ) ) );

    connect( this, SIGNAL( propertyReset( Property*, Set* ) ),
             this, SLOT(intersectedReset( Property*, Set* ) ) );
}

Buffer::Buffer(const Set *list)
    :Set(false)
{
    connect( this, SIGNAL( propertyChanged( Property*, Set* ) ),
             this, SLOT(intersectedChanged( Property*, Set* ) ) );

    connect( this, SIGNAL( propertyReset( Property*, Set* ) ),
             this, SLOT(intersectedReset( Property*, Set* ) ) );

    initialList( list );
}

void Buffer::initialList(const Set *list)
{
    //deep copy of m_list
    for(Property::DictIterator it(list->d->dict); it.current(); ++it) {
        Property *prop = new Property( *it.current() );
        QCString group = list->d->groupForProperty[it.current()];
        QString groupDesc = list->d->groupsDescription[ group ];
        setGroupDescription( group, groupDesc );
        addProperty( prop, group );
        prop->addRelatedProperty( it.current() );
    }
}

void Buffer::intersect(const Set *list)
{
    if ( d->dict.isEmpty() )
    {
        initialList( list );
        return;
    }

     for(Property::DictIterator it(d->dict); it.current(); ++it) {
        const char* key = it.current()->name();
        if ( Property *property =   list->d->dict[ key ] )
        {
            if ( ( it.current() == property ) &&
                 ( list->d->groupForProperty[ property ] ==
                   d->groupForProperty[ it.current() ] ) ) {
                it.current()->addRelatedProperty( property );
                continue;
            }
        }
        else
            removeProperty( key );
    }
}

void Buffer::intersectedChanged(Property *prop, Set *list)
{
    QCString propertyName = prop->name();
    if ( !contains( propertyName ) )
        return;

    const QValueList<Property*> *props = prop->related();
    QValueList<Property*>::const_iterator it = props->begin();
    for ( ; it != props->end(); ++it ) {
        ( *it )->setValue( prop->value(), false );
    }
}

void Buffer::intersectedReset(Property *prop, Set *list)
{
    QCString propertyName = prop->name();
    if ( !contains( propertyName ) )
        return;

    const QValueList<Property*> *props = prop->related();
    QValueList<Property*>::const_iterator it = props->begin();
    for ( ; it != props->end(); ++it )  {
        ( *it )->setValue( prop->value(), false );
    }
}

}

#include "set.moc"

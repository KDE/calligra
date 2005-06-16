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

#include "list.h"
#include "property.h"

#include <qasciidict.h>

#ifdef QT_ONLY
// \todo
#else
#include <kdebug.h>
#include <klocale.h>
#endif

namespace KOProperty {

class KPROPERTY_EXPORT PtrListPrivate
{
    public:
        PtrListPrivate() : dict(101, false){}
        ~PtrListPrivate(){}

    //dict of properties in form name: property
    Property::Dict dict;
    //groups of properties:
    // list of group name: (list of property names)
    StringListMap propertiesOfGroup;
    QMap<QCString, QString>  groupsDescription;
    // map of property: group
    QMap<Property*, QCString> groupForProperty;

    bool ownProperty : 1;
    static Property nonConstNull;
    QCString prevSelection;
    //QString typeName;
};

//PtrList::Iterator class
PtrList::Iterator::Iterator(const PtrList &list)
{
    iterator = new Property::DictIterator(list.d->dict);
}

PtrList::Iterator::~Iterator()
{
    delete iterator;
}

void
PtrList::Iterator::operator ++()
{
    ++(*iterator);
}

Property*
PtrList::Iterator::operator *()
{
    return current();
}

QString
PtrList::Iterator::currentKey()
{
    return iterator->currentKey();
}

Property*
PtrList::Iterator::current()
{
    return iterator->current();
}

 //////////////////////////////////////////////

PtrList::PtrList(QObject *parent, const char *name)
: QObject(parent, name)
{
    d = new PtrListPrivate();
    d->ownProperty = true;
    d->groupsDescription.insert("common", i18n("General"));
}


PtrList::PtrList(const PtrList &l)
 : QObject(l.parent(), l.name())
{
    d = new PtrListPrivate();
    *this = l;
}

PtrList::PtrList(bool propertyOwner)
 : QObject(0, 0)
{
    d = new PtrListPrivate();
    d->ownProperty = propertyOwner;
    d->groupsDescription.insert("common", i18n("General"));
}

PtrList::~PtrList()
{
    emit aboutToBeDeleted();
    clear();
    delete d;
}

/////////////////////////////////////////////////////

void
PtrList::addProperty(Property *property, QCString group)
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
PtrList::removeProperty(Property *property)
{
    if(!property)
        return;

    removeProperty(property->name());
}

void
PtrList::removeProperty(const QCString &name)
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
PtrList::clear()
{
    for(Property::DictIterator it(d->dict); it.current(); ++it)
        removeProperty(it.current()->name());
}

/////////////////////////////////////////////////////

void
PtrList::addToGroup(const QCString &group, Property *property)
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
PtrList::removeFromGroup(Property *property)
{
    QCString group = d->groupForProperty[property];
    d->propertiesOfGroup[group].remove(property->name());
    d->groupForProperty.remove(property);
}

const StringListMap&
PtrList::groups()
{
    return d->propertiesOfGroup;
}

void
PtrList::setGroupDescription(const QCString &group, const QString desc)
{
    d->groupsDescription[group] = desc;
}

QString
PtrList::groupDescription(const QCString &group)
{
    if(d->groupsDescription.contains(group))
        return d->groupsDescription[group];
    return group;
}

/////////////////////////////////////////////////////

uint
PtrList::count() const
{
    return d->dict.count();
}

bool
PtrList::isEmpty() const
{
    return d->dict.count() == 0;
}

bool
PtrList::contains(const QCString &name)
{
    return d->dict.find(name);
}

Property&
PtrList::property(const QCString &name)
{
    Property *p = d->dict.find(name);
    if(!p) {
        p = new Property(name);
        addProperty(p);
    }

    return *p;
}

Property&
PtrList::operator[](const QCString &name)
{
    return property(name);
}

const PtrList&
PtrList::operator= (const PtrList &l)
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
PtrList::changeProperty(const QCString &property, const QVariant &value)
{
    Property *p = d->dict.find(property);
    if(p)
        p->setValue(value);
}

/////////////////////////////////////////////////////

void
PtrList::debug()
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
PtrList::prevSelection() const
{
    return d->prevSelection;
}

void
PtrList::setPrevSelection(const QCString &prevSelection)
{
    d->prevSelection = prevSelection;
}


Buffer::Buffer()
    :PtrList(false)
{
}

Buffer::Buffer(PtrList *list)
    :PtrList(false)
{
    //deep copy of m_list
    for(Property::DictIterator it( list->d->dict ); it.current(); ++it)
    {
        Property *prop = new Property( *it.current() );
        QCString group = list->d->groupForProperty[ it.current() ];
        QString groupDesc = list->d->groupsDescription[ group ];
        setGroupDescription( group, groupDesc );
        addProperty( prop, group );
        prop->addRelatedProperty( it.current() );
    }
    connect( this, SIGNAL( propertyChanged( Property*, PtrList* ) ),
             this, SLOT(intersectedChanged( Property*, PtrList* ) ) );

    connect( this, SIGNAL( propertyReset( Property*, PtrList* ) ),
             this, SLOT(intersectedReset( Property*, PtrList* ) ) );
}

void Buffer::intersect(const PtrList *list)
{
    for(Property::DictIterator it( d->dict ); it.current(); ++it)
    {
        const char* key = it.current()->name();
        if ( Property *property = list->d->dict.find( key ) )
        {
            if ( ( ( it.current() ) == property ) &&
                 ( list->d->groupForProperty[ property ] ==
                   d->groupForProperty[ it.current() ] ) )
            {
                it.current()->addRelatedProperty( property );
                continue;
            }
        }

//         --it;
        removeProperty( key );
    }
    connect( this, SIGNAL( propertyChanged( Property*, PtrList* ) ),
             this, SLOT( intersectedChanged( Property*, PtrList* ) ) );

    connect( this, SIGNAL( propertyReset( Property*, PtrList* ) ),
             this, SLOT(intersectedReset( Property*, PtrList* ) ) );
}

void Buffer::intersectedChanged(Property *prop, PtrList *list)
{
    QCString propertyName = prop->name();
    if ( !contains( propertyName ) )
        return;

    const QValueList<Property*> *props = prop->related();
    QValueList<Property*>::const_iterator it = props->begin();
    for ( ; it != props->end(); ++it )
    {
        ( *it )->setValue( prop->value(), false );
    }
}

void Buffer::intersectedReset(Property *prop, PtrList *list)
{
    QCString propertyName = prop->name();
    if ( !contains( propertyName ) )
        return;

    const QValueList<Property*> *props = prop->related();
    QValueList<Property*>::const_iterator it = props->begin();
    for ( ; it != props->end(); ++it )
    {
        ( *it )->setValue( prop->value(), false );
    }
}

}

#include "list.moc"

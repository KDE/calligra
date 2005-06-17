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

// #include <qasciidict.h>
#include <qvaluelist.h>

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
        PtrListPrivate() /*: dict(101, false)*/{}
        ~PtrListPrivate(){}

    //dict of properties in form name: property
//     Property::Dict dict;
    PropertyList properties;
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

};

//PtrList::Iterator class
PtrList::Iterator::Iterator(const PtrList &list)
{
    iterator = list.d->properties.begin();
    end = list.d->properties.end();
}

PtrList::Iterator::~Iterator()
{
}

void
PtrList::Iterator::operator ++()
{
    ++(iterator);
}

Property*
PtrList::Iterator::operator *()
{
    return (*iterator);
}

QString
PtrList::Iterator::currentKey()
{
    if ((*iterator))
        return (*iterator)->name();
    else
        return QString::null;
}

Property*
PtrList::Iterator::current()
{
    if ( iterator != end )
        return (*iterator);
    else
        return 0L;
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

    if(d->contains(property->name())) {
        Property *p = (*d)[property->name()];
        p->addRelatedProperty(property);
    }
    else {
        d->properties.append(property);
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

    Property *p = d->take(name);
    removeFromGroup(p);
    if(d->ownProperty) {
        emit aboutToDeleteProperty(p, this);
        delete p;
    }
}

void
PtrList::clear()
{
    PropertyList::iterator it = d->properties.begin();
    for( ; it != d->properties.end(); ++it)
        removeProperty( ( *it ) ->name() );
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
    return d->properties.count();
}

bool
PtrList::isEmpty() const
{
    return d->properties.count() == 0;
}

bool
PtrList::contains(const QCString &name)
{
    return d->contains(name);
}

Property&
PtrList::property(const QCString &name)
{
    Property *p = (*d)[name];
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

    d->properties.clear();
    d->groupForProperty.clear();

    d->ownProperty = l.d->ownProperty;
    d->prevSelection = l.d->prevSelection;
    d->groupsDescription = l.d->groupsDescription;
    d->propertiesOfGroup = l.d->propertiesOfGroup;

    // Copy all properties in the list
    PropertyList::iterator it = d->properties.begin();
    for( ; it != d->properties.end(); ++it)
    {
        Property *prop = new Property( *( ( *it ) ) );
        addProperty(prop, l.d->groupForProperty[ ( *it ) ] );
    }

    return *this;
}

void
PtrList::changeProperty(const QCString &property, const QVariant &value)
{
    Property *p = (*d)[property];
    if(p)
        p->setValue(value);
}

/////////////////////////////////////////////////////

void
PtrList::debug()
{
    //kdDebug(45000) << "List: typeName='" << m_typeName << "'" << endl;
    if(d->properties.isEmpty()) {
        kdDebug(100300) << "<EMPTY>" << endl;
        return;
    }
    kdDebug(100300) << d->properties.count() << " properties:" << endl;

    PropertyList::iterator it = d->properties.begin();
    for( ; it != d->properties.end(); ++it)
        (* it )->debug();
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
    connect( this, SIGNAL( propertyChanged( Property*, PtrList* ) ),
              this, SLOT(intersectedChanged( Property*, PtrList* ) ) );

    connect( this, SIGNAL( propertyReset( Property*, PtrList* ) ),
             this, SLOT(intersectedReset( Property*, PtrList* ) ) );
}

Buffer::Buffer(const PtrList *list)
    :PtrList(false)
{
    connect( this, SIGNAL( propertyChanged( Property*, PtrList* ) ),
             this, SLOT(intersectedChanged( Property*, PtrList* ) ) );

    connect( this, SIGNAL( propertyReset( Property*, PtrList* ) ),
             this, SLOT(intersectedReset( Property*, PtrList* ) ) );

    initialList( list );
}

void Buffer::initialList(const PtrList *list)
{
    //deep copy of m_list
    PropertyList::iterator it = list->d->properties.begin();
    for( ; it != list->d->properties.end(); ++it)
    {
        Property *prop = new Property( *( *it ) );
        QCString group = list->d->groupForProperty[ ( *it ) ];
        QString groupDesc = list->d->groupsDescription[ group ];
        setGroupDescription( group, groupDesc );
        addProperty( prop, group );
        prop->addRelatedProperty( ( *it ) );
    }
}

void Buffer::intersect(const PtrList *list)
{
    if ( d->properties.isEmpty() )
    {
        initialList( list );
        return;
    }

    PropertyList::iterator it = d->properties.begin();
    for( ; it != d->properties.end(); ++it)
    {
        const char* key = ( *it )->name();
        if ( Property *property = ( *( list->d ) )[ key ] )
        {
            if ( ( ( ( *it ) ) == property ) &&
                 ( list->d->groupForProperty[ property ] ==
                   d->groupForProperty[ ( *it ) ] ) )
            {
                ( *it )->addRelatedProperty( property );
                continue;
            }
        }
        else
            removeProperty( key );
    }
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

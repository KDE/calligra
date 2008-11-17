/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2004-2008 Jarosław Staniek <staniek@kde.org>

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

#include "Set.h"
#include "Property.h"
#include "Utils.h"

#include <qapplication.h>
#include <QByteArray>

#include <kdebug.h>
#include <klocale.h>

typedef QMap<QByteArray, QList<QByteArray>* > StringListMap;

namespace KoProperty
{

//! @internal
static Property Set_nonConstNull;

//! @internal
class Set::Private
{
public:
    Private(KoProperty::Set *set) :
            q(set),
            readOnly(false),
            informAboutClearing(0) {}
    ~Private() {}

    Set *q;
// PropertyList properties;
    //groups of properties:
    // list of group name: (list of property names)
    StringListMap propertiesOfGroup;
    QList<QByteArray>  groupNames;
    QHash<QByteArray, QString>  groupDescriptions;
    QHash<QByteArray, QString>  groupIcons;
    // map of property: group

    bool ownProperty : 1;
    bool readOnly : 1;
// static Property nonConstNull;
    QByteArray prevSelection;
    QString typeName;

    //! Used in Set::informAboutClearing(Property*) to declare that the property wants
    //! to be informed that the set has been cleared (all properties are deleted)
    bool* informAboutClearing;

    inline Property* property(const QByteArray &name) const {
        return hash.value(name.toLower());
    }

    inline Property& propertyOrNull(const QByteArray &name) const {
        Property *p = property(name);
        if (p)
            return *p;
        Set_nonConstNull.setName(0); //to ensure returned property is null
        kWarning() << "PROPERTY \"" << name << "\" NOT FOUND";
        return Set_nonConstNull;
    }

/*    inline void add(Property* property) {
        return hash.insert(property->name().toLower(), property);
    }*/

    void addProperty(Property *property, QByteArray group, bool updateSortingKey)
    {
        if (!property) {
            kWarning() << "property == 0";
            return;
        }
        if (property->isNull()) {
            kWarning() << "COULD NOT ADD NULL PROPERTY";
            return;
        }
        if (group.isEmpty())
            group = "common";

        Property *p = this->property(property->name());
        if (p) {
            q->addRelatedProperty(p, property);
        } else {
            hash.insert(property->name().toLower(), property);
            q->addToGroup(group, property);
        }

        property->addSet(q);
        if (updateSortingKey)
            property->setSortingKey(count());
    }

    void removeProperty(Property *property)
    {
        if (!property)
            return;

        Property *p = hash.take(property->name());
        q->removeFromGroup(p);
        if (ownProperty) {
            emit q->aboutToDeleteProperty(*q, *p);
            delete p;
        }
    }

/*    inline void remove(Property* property) {
        hash.remove(property->name().toLower());
    }*/

    void clear() {
        if (informAboutClearing)
            *informAboutClearing = true;
        informAboutClearing = 0;
        emit q->aboutToBeCleared();
        qDeleteAll(propertiesOfGroup);
        propertiesOfGroup.clear();
        groupNames.clear();
        groupForProperties.clear();
        groupDescriptions.clear();
        groupIcons.clear();
        qDeleteAll(hash);
        hash.clear();
        /* old, inefficient
        Property::DictIterator it(d->dict);
        while (it.current())
            removeProperty(it.current());*/
    }

    inline int count() const { return hash.count(); }

    inline bool isEmpty() const { return hash.isEmpty(); }

    inline QByteArray groupForProperty(Property *property) const {
        return groupForProperties.value(property);
    }

    inline void addPropertyToGroup(Property *property, const QByteArray &groupLower) {
        groupForProperties.insert(property, groupLower);
    }

    inline void removePropertyFromGroup(Property *property) {
        groupForProperties.remove(property);
    }

    // Copy all properties from the other hash
    void copyPropertiesFrom(
        const QHash<QByteArray, Property*>::ConstIterator& constBegin,
        const QHash<QByteArray, Property*>::ConstIterator& constEnd, const Set & set)
    {
        for (QHash<QByteArray, Property*>::ConstIterator it(constBegin); it!=constEnd; ++it) {
            Property *prop = new Property(*it.value());
            addProperty(prop, set.groupForProperty( it.value() ),
                        false /* don't updateSortingKey, because the key is already 
                                 set in Property copy ctor.*/
                       );
        }
    }

    inline QHash<QByteArray, Property*>::ConstIterator hashConstIterator() const {
        return QHash<QByteArray, Property*>::ConstIterator( hash.constBegin() );
    }

    inline QHash<QByteArray, Property*>::ConstIterator hashConstEnd() const {
        return hash.constEnd();
    }
private:
    // a hash of properties in form name -> property
    QHash<QByteArray, Property*> hash;
    QHash<Property*, QByteArray> groupForProperties;
};

}

using namespace KoProperty;

//////////////////////////////////////////////

Set::PropertySelector::PropertySelector()
{
}

Set::PropertySelector::~PropertySelector()
{
}

//////////////////////////////////////////////

//Set::Iterator class
Set::Iterator::Iterator(const Set &set, PropertySelector *selector)
    : m_iterator( set.d->hashConstIterator() )
    , m_end( set.d->hashConstEnd() )
    , m_selector( selector )
{
    if (m_selector && current() && !(*m_selector)( *current() )) {
        // the first item is not acceptable by the selector
        ++(*this);
    }
}

Set::Iterator::~Iterator()
{
    delete m_selector;
}

void
Set::Iterator::operator ++()
{
    while (true) {
        ++m_iterator;
        if (!m_selector)
            return;
        // selector exists
        if (!current()) // end encountered
            return;
        if ((*m_selector)( *current() ))
            return;
    }
}

Property*
Set::Iterator::operator *() const
{
    return current();
}

Property*
Set::Iterator::current() const
{
    return m_iterator==m_end ? 0 : m_iterator.value();
}

//////////////////////////////////////////////

Set::Set(QObject *parent, const QString &typeName)
        : QObject(parent)
        , d(new Private(this))
{
    setObjectName(typeName.toLatin1());

    d->ownProperty = true;
    d->groupDescriptions.insert("common", i18nc("General properties", "General"));
    d->typeName = typeName;
}


Set::Set(const Set &set)
        : QObject(0 /* implicit sharing the parent is dangerous */)
        , d(new Private(this))
{
    setObjectName(set.objectName());
    *this = set;
}

Set::Set(bool propertyOwner)
        : QObject(0)
        , d(new Private(this))
{
    d->ownProperty = propertyOwner;
    d->groupDescriptions.insert("common", i18nc("General properties", "General"));
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
Set::addProperty(Property *property, QByteArray group)
{
    d->addProperty(property, group, true);
}

void
Set::removeProperty(Property *property)
{
    d->removeProperty(property);
}

void
Set::removeProperty(const QByteArray &name)
{
    Property *p = d->property(name);
    removeProperty(p);
}

void
Set::clear()
{
    d->clear();
}

void
Set::informAboutClearing(bool& cleared)
{
    cleared = false;
    d->informAboutClearing = &cleared;
}

/////////////////////////////////////////////////////

QByteArray Set::groupForProperty(Property *property) const
{
    return d->groupForProperty(property);
}

void
Set::addToGroup(const QByteArray &group, Property *property)
{
    if (!property || group.isEmpty())
        return;

    //do not add the same property to the group twice
    QByteArray groupLower(group.toLower());
    if (d->groupForProperty(property) == groupLower) {
        kWarning() << "Group" << group << "already contains property" << property->name();
        return;
    }
    QList<QByteArray> *list = d->propertiesOfGroup.value(groupLower);
    if (!list) {
        list = new QList<QByteArray>();
        d->propertiesOfGroup.insert(groupLower, list);
        d->groupNames.append(property->name());
    }
    d->addPropertyToGroup(property, groupLower);
}

void
Set::removeFromGroup(Property *property)
{
    if (!property)
        return;
    const QByteArray group( d->groupForProperty(property) );
    if (group.isEmpty())
        return;
    QList<QByteArray> *propertiesOfGroup = d->propertiesOfGroup.value(group);
    propertiesOfGroup->removeAt(propertiesOfGroup->indexOf(property->name()));
    if (propertiesOfGroup->isEmpty()) {
        //remove group as well
        d->propertiesOfGroup.remove(group);
        const int i = d->groupNames.indexOf(group);
        if (i != -1)
            d->groupNames.removeAt(i);
        delete propertiesOfGroup;
    }
    d->removePropertyFromGroup(property);
}

const QList<QByteArray>
Set::groupNames() const
{
    return d->groupNames;
}

const QList<QByteArray>
Set::propertyNamesForGroup(const QByteArray &group) const
{
    QList<QByteArray> *list = d->propertiesOfGroup.value(group);
    return list ? *list : QList<QByteArray>();
}

void
Set::setGroupDescription(const QByteArray &group, const QString desc)
{
    d->groupDescriptions.insert(group.toLower(), desc);
}

QString
Set::groupDescription(const QByteArray &group) const
{
    const QString result( d->groupDescriptions.value(group.toLower()) );
    if (!result.isEmpty())
        return result;
    return group;
}

void
Set::setGroupIcon(const QByteArray &group, const QString& icon)
{
    d->groupIcons.insert(group.toLower(), icon);
}

QString
Set::groupIcon(const QByteArray &group) const
{
    return d->groupIcons.value(group);
}


/////////////////////////////////////////////////////

uint
Set::count() const
{
    return d->count();
}

bool
Set::isEmpty() const
{
    return d->isEmpty();
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
Set::contains(const QByteArray &name) const
{
    return d->property(name);
}

Property&
Set::property(const QByteArray &name) const
{
    return d->propertyOrNull(name);
}

Property&
Set::operator[](const QByteArray &name) const
{
    return d->propertyOrNull(name);
}

const Set&
Set::operator= (const Set & set)
{
    if (&set == this)
        return *this;

    clear();

    d->ownProperty = set.d->ownProperty;
    d->prevSelection = set.d->prevSelection;
    d->groupDescriptions = set.d->groupDescriptions;
    d->copyPropertiesFrom(set.d->hashConstIterator(), set.d->hashConstEnd(), set);
    return *this;
}

void
Set::changeProperty(const QByteArray &property, const QVariant &value)
{
    Property *p = d->property(property);
    if (p)
        p->setValue(value);
}

/////////////////////////////////////////////////////

void
Set::debug()
{
    //kDebug() << "List: typeName='" << m_typeName << "'";
    if (d->isEmpty()) {
        kDebug() << "<EMPTY>";
        return;
    }
    kDebug() << d->count() << " properties:";

    for (QHash<QByteArray, Property*>::ConstIterator it(d->hashConstIterator()); it!=d->hashConstEnd(); ++it)
        it.value()->debug();
}

QByteArray
Set::previousSelection() const
{
    return d->prevSelection;
}

void
Set::setPreviousSelection(const QByteArray &prevSelection)
{
    d->prevSelection = prevSelection;
}

QString
Set::typeName() const
{
    return d->typeName;
}

void Set::addRelatedProperty(Property *p1, Property *p2) const
{
    p1->addRelatedProperty(p2);
}

/////////////////////////////////////////////////////

Buffer::Buffer()
        : Set(false)
{
    connect(this, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)),
            this, SLOT(intersectedChanged(KoProperty::Set&, KoProperty::Property&)));

    connect(this, SIGNAL(propertyReset(KoProperty::Set&, KoProperty::Property&)),
            this, SLOT(intersectedReset(KoProperty::Set&, KoProperty::Property&)));
}

Buffer::Buffer(const Set& set)
        : Set(false)
{
    connect(this, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)),
            this, SLOT(intersectedChanged(KoProperty::Set&, KoProperty::Property&)));

    connect(this, SIGNAL(propertyReset(KoProperty::Set&, KoProperty::Property&)),
            this, SLOT(intersectedReset(KoProperty::Set&, KoProperty::Property&)));

    init(set);
}

void Buffer::init(const Set& set)
{
    //deep copy of set
    for (QHash<QByteArray, Property*>::ConstIterator it(set.d->hashConstIterator()); 
        it!=set.d->hashConstEnd(); ++it)
    {
        Property *prop = new Property(*it.value());
        QByteArray group = set.groupForProperty(it.value());
        QString groupDesc = set.groupDescription( group );
        setGroupDescription(group, groupDesc);
        addProperty(prop, group);
        prop->addRelatedProperty(it.value());
    }
}

void Buffer::intersect(const Set& set)
{
    if (isEmpty()) {
        init(set);
        return;
    }

    for (QHash<QByteArray, Property*>::ConstIterator it(set.d->hashConstIterator()); 
        it!=set.d->hashConstEnd(); ++it)
    {
        const QByteArray key( it.value()->name() );
        Property *property = set.d->property( key );
        if (property) {
            blockSignals(true);
            it.value()->resetValue();
            it.value()->addRelatedProperty(property);
            blockSignals(false);
        } else {
            removeProperty(key);
        }
    }
}

void Buffer::intersectedChanged(Set& set, Property& prop)
{
    Q_UNUSED(set);
    if (!contains(prop.name()))
        return;

    const QList<Property*> *props = prop.related();
    for (QList<Property*>::ConstIterator it = props->constBegin(); it != props->constEnd(); ++it) {
        (*it)->setValue(prop.value(), false);
    }
}

void Buffer::intersectedReset(Set& set, Property& prop)
{
    Q_UNUSED(set);
    if (!contains(prop.name()))
        return;

    const QList<Property*> *props = prop.related();
    for (QList<Property*>::ConstIterator it = props->constBegin(); it != props->constEnd(); ++it)  {
        (*it)->setValue(prop.value(), false);
    }
}

//////////////////////////////////////////////

QHash<QByteArray, QVariant> KoProperty::propertyValues(const Set& set)
{
    QHash<QByteArray, QVariant> result;
    for (Set::Iterator it(set); it.current(); ++it) {
        result.insert(it.current()->name(), it.current()->value());
    }
    return result;
}

#include "Set.moc"

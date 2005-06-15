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

#ifndef KPROPERTY_LIST_H
#define KPROPERTY_LIST_H

#include <qobject.h>
#include <qasciidict.h>

/*! \brief
   \author Cedric Pasteur <cedric.pasteur@free.fr>
   \author Alexander Dymo <cloudtemple@mskat.net>
 */
namespace KOProperty {

class Property;
class PtrListPrivate;

typedef QMap<QCString, QValueList<QCString> > StringListMap ;
typedef QMapIterator<QCString, QStringList> StringListMapIterator;

/*! \brief Lists holding properties in groups

   \author Cedric Pasteur <cedric.pasteur@free.fr>
   \author Alexander Dymo <cloudtemple@mskat.net>
   \author Jaroslaw Staniek <js@iidea.pl>
 */
class KPROPERTY_EXPORT PtrList : public QObject
{
    Q_OBJECT

    public:
        /*! \brief A class to iterate over a PtrListPrivate
        It behaves as a QDictIterator. To use it:
        \code  PtrList::Iterator it(list);
                   for(; it.current(); ++it) { .... }
        \endcode
          \author Cedric Pasteur <cedric.pasteur@free.fr>
          \author Alexander Dymo <cloudtemple@mskat.net> */
        class Iterator {
            public:
                Iterator(const PtrList &list);
                ~Iterator();

                void operator ++();
                Property*  operator *();

                QString  currentKey();
                Property*  current();

            private:
                QAsciiDictIterator<Property> *iterator;
                friend class PtrList;
        };


        PtrList(QObject *parent=0, const char *name=0);
        PtrList(const PtrList&);
        ~PtrList();


        /*! Adds the property to the list, in the group. You can use any group name, except "common"
          (which is already used for basic group). */
        void addProperty(Property *property, QCString group = "common");
        /*! Removes property from the list. Emits aboutToDeleteProperty before removing.*/
        void removeProperty(Property *property);
        /*! Removes property with the given name from the list.
        Emits aboutToDeleteProperty() before removing.*/
        void removeProperty(const QCString &name);
        /*! Removes all properties from the buffer and destroys them. */
        virtual void clear();

        /*! Returns the number of items in the list. */
        uint count() const;

        /*! Returns TRUE if the list is empty, i.e. count() == 0; otherwise it returns FALSE. */
        bool isEmpty() const;

        bool  contains(const QCString &name);
        /*! \return property named with \a name. If no such property is found,
         null property (KexiProperty::null) is returned. */
        Property&  property( const QCString &name);
        /*! Accesses a property by it's name. All property modifications are allowed
        trough this method. For example, to set a value of a property, use:
        /code
        PropertyList list;
        ...
        list["My Property"].setValue("My Value");
        /endcode
        \return \ref Property with given name.*/
        Property&  operator[](const QCString &name);
        const PtrList& operator= (const PtrList &l);


        /*! Change the value of property whose key is \a property to \a value.
        By default, it only calls KexiProperty::setValue(). */
        void changeProperty(const QCString &property, const QVariant &value);

        /*! Sets the i18n'ed string that will be shown in Editor to represent this group. */
        void  setGroupDescription(const QCString &group, const QString desc);
        QString   groupDescription(const QCString &group);
        const StringListMap&   groups();

        /*! Used by property editor to preserve previous selection when this list is set again. */
        QCString prevSelection() const;
        void setPrevSelection(const QCString& prevSelection);

    protected:
        /*! Constructs a list which owns or does not own it's properties.*/
        PtrList(bool propertyOwner);

        /*! Adds property to a group.*/
        void addToGroup(const QCString &group, Property *property);
        /*! Removes property from a group.*/
        void removeFromGroup(Property *property);

        void debug();

    signals:
        /*! Emitted when the value of the property is changed.*/
        void propertyChanged(Property* property, PtrList *list);
        /*! Parameterless version of the above method. */
        void propertyChanged();
        void propertyReset(Property *property, PtrList *list);
        /*! Emitted when property is about to be deleted.*/
        void aboutToDeleteProperty(Property* property, PtrList *list);

        void aboutToBeDeleted();

    protected:
        PtrListPrivate   *d;

    friend class Property;
    friend class Buffer;
};

/*! \brief
    \todo find a better name to show it's a list that doesn't own property
   \author Cedric Pasteur <cedric.pasteur@free.fr>
   \author Alexander Dymo <cloudtemple@mskat.net>
   \author Adam Treat <treat@kde.org>
 */

class KPROPERTY_EXPORT Buffer : public PtrList
{
    Q_OBJECT

    public:
        Buffer();
        Buffer(PtrList *list);

        /*! Intersects with other PtrList.*/
        virtual void intersect(const PtrList *list);

    protected slots:
        void intersectedChanged(Property *prop, PtrList *list);
        void intersectedChanged();
        void intersectedReset(Property *prop, PtrList *list);
};


}

#endif

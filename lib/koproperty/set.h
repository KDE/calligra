/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KPROPERTY_SET_H
#define KPROPERTY_SET_H

#include <qobject.h>
#include <koffice_export.h>
#include <qasciidict.h>

/*! \brief
   \author Cedric Pasteur <cedric.pasteur@free.fr>
   \author Alexander Dymo <cloudtemple@mskat.net>
   \author Jaroslaw Staniek <js@iidea.pl>
 */
namespace KoProperty {

class Property;
class SetPrivate;

//typedef QValueList<Property*> PropertyList;
//typedef QValueListIterator<Property*> PropertyListIterator;
typedef QMap<QCString, QValueList<QCString> > StringListMap ;
typedef QMapIterator<QCString, QStringList> StringListMapIterator;

/*! \brief Lists holding properties in groups

   \author Cedric Pasteur <cedric.pasteur@free.fr>
   \author Alexander Dymo <cloudtemple@mskat.net>
   \author Jaroslaw Staniek <js@iidea.pl>
 */
class KOPROPERTY_EXPORT Set : public QObject
{
	Q_OBJECT

	public:
		/*! \brief A class to iterate over a SetPrivate
		It behaves as a QDictIterator. To use it:
		\code  Set::Iterator it(set);
		       for(; it.current(); ++it) { .... }
		\endcode
		  \author Cedric Pasteur <cedric.pasteur@free.fr>
		  \author Alexander Dymo <cloudtemple@mskat.net> */
		class KOPROPERTY_EXPORT Iterator {
			public:
				Iterator(const Set &set);
				~Iterator();

				void operator ++();
				Property*  operator *();

				QCString  currentKey();
				Property*  current();

			private:
				QAsciiDictIterator<Property> *iterator;
				friend class Set;
		};


		Set(QObject *parent=0, const QString &typeName=QString::null);
		Set(const Set&);
		~Set();

		/*! Adds the property to the set, in the group. You can use any group name, except "common"
		  (which is already used for basic group). */
		void addProperty(Property *property, QCString group = "common");

		/*! Removes property from the set. Emits aboutToDeleteProperty before removing.*/
		void removeProperty(Property *property);

		/*! Removes property with the given name from the set.
		Emits aboutToDeleteProperty() before removing.*/
		void removeProperty(const QCString &name);

		/*! Removes all properties from the buffer and destroys them. */
		virtual void clear();

		/*! Returns the number of items in the set. */
		uint count() const;

		/*! Returns TRUE if the set is empty, i.e. count() == 0; otherwise it returns FALSE. */
		bool isEmpty() const;

		bool contains(const QCString &name);

		/*! \return property named with \a name. If no such property is found,
		 null property (KexiProperty::null) is returned. */
		Property&  property( const QCString &name);

		/*! Accesses a property by it's name. All property modifications are allowed
		trough this method. For example, to set a value of a property, use:
		/code
		Set set;
		...
		set["myProperty"].setValue("My Value");
		/endcode
		\return \ref Property with given name.*/
		Property&  operator[](const QCString &name);

		const Set& operator= (const Set &l);

		/*! Change the value of property whose key is \a property to \a value.
		By default, it only calls KexiProperty::setValue(). */
		void changeProperty(const QCString &property, const QVariant &value);

		/*! Sets the i18n'ed string that will be shown in Editor to represent this group. */
		void setGroupDescription(const QCString &group, const QString desc);

		QString groupDescription(const QCString &group);

		const StringListMap& groups();

		/*! Used by property editor to preserve previous selection when this set is assigned again. */
		QCString prevSelection() const;

		void setPrevSelection(const QCString& prevSelection);

		/*! A name of this property set type, that is usable when
		 we want to know if two property set objects have the same type.
		 This avoids e.g. reloading of all Editor's contents.
		 Also, this allows to know if two property set objects are compatible
		 by their property sets.
		 For comparing purposes, type names are case insensitive.*/
		QString typeName() const;

		void debug();

	protected:
		/*! Constructs a set which owns or does not own it's properties.*/
		Set(bool propertyOwner);

		/*! Adds property to a group.*/
		void addToGroup(const QCString &group, Property *property);

		/*! Removes property from a group.*/
		void removeFromGroup(Property *property);

	signals:
		/*! Emitted when the value of the property is changed.*/
		void propertyChanged(KoProperty::Set& set, KoProperty::Property& property);

		/*! Parameterless version of the above method. */
		void propertyChanged();

		/*! Emitted when the value of the property is reset.*/
		void propertyReset(KoProperty::Set& set, KoProperty::Property& property);

		/*! Emitted when property is about to be deleted.*/
		void aboutToDeleteProperty(KoProperty::Set& set, KoProperty::Property& property);

		/*! Emitted when property set object is about to be cleared (using clear()). 
		 This signal is also emmited from destructor before emitting aboutToBeDeleted(). */
		void aboutToBeCleared();

		/*! Emitted when property set object is about to be deleted.*/
		void aboutToBeDeleted();

	protected:
		SetPrivate *d;

	friend class Property;
	friend class Buffer;
};

/*! \brief
	\todo find a better name to show it's a set that doesn't own property
   \author Cedric Pasteur <cedric.pasteur@free.fr>
   \author Alexander Dymo <cloudtemple@mskat.net>
   \author Adam Treat <treat@kde.org>
 */
class KOPROPERTY_EXPORT Buffer : public Set
{
	Q_OBJECT

	public:
		Buffer();
		Buffer(const Set *set);

		/*! Intersects with other Set.*/
		virtual void intersect(const Set *set);

	protected slots:
		void intersectedChanged(Set& set, Property& prop);
		void intersectedReset(Set& set, Property& prop);

	private:
		void initialSet(const Set *set);
};

}

#endif

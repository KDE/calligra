/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004   Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIPROPERTY_H
#define KEXIPROPERTY_H

#include <qvariant.h>
#include <qasciidict.h>
#include <qguardedptr.h>

class QObject;
class QString;
class QStringList;
class KexiPropertyBuffer;

/** This class holds a single property, which can be any of the types supported by QVariant.
    It includes support for QStringList properties, an i18n'ed label and stores an old value to allow undo.

    Create a property :
    \code
    property = KexiProperty(name, value); // name is a QCString, value is whatever type QVariant supports
    \endcode

    There are two exceptions :
    \code
    property = KexiProperty(name, QVariant(bool, 3));  // You must use QVariant(bool, int) to create a bool property
    // See QVariant doc for more details

    // To create a list property
    property = KexiProperty(name, "Current Value", list); // where list is the list of possible values for this property
    \endcode

 **/
class KEXICORE_EXPORT KexiProperty
{
	public:
		typedef QAsciiDict<KexiProperty> Dict;
		typedef QPtrList<KexiProperty> List;
		typedef QPtrListIterator<KexiProperty> ListIterator;
//		typedef QMap<QString,KexiProperty> Map;

//		/*! Creates a simple property with \a name as name and \a value as value. */
//		KexiProperty(const QString &name, QVariant value);

		/*! Creates a simple property with \a name as name, \a value as value
		 and \a as i18n'ed description. */
		KexiProperty(const QCString &name, QVariant value, const QString &desc = QString::null);

		/*! Creates a list property with \a name as name, \a value as value
		  and \a key_list as the list of all possible keys. Value must be
		  an element from \a key_list list.
		  \a name_list is a list of i18n'ed names that will be visible ont he screen,
		  instead of keys.
		  The user will be able to choose a value from \a key_list.
		*/
		KexiProperty(const QCString &name, const QString &value,
		 const QStringList &key_list, const QStringList &name_list,
		 const QString &desc = QString::null);

		//! Copy constructor.
		KexiProperty(const KexiProperty &property);

		//! Creates an empty property.
		KexiProperty();

		~KexiProperty();

		const KexiProperty& operator=(const KexiProperty &property);

		/*! Adds \a prop as a child of this property.
		 The children will be owned by this property */
		void addChild(KexiProperty *prop);

		/*! \return parent for this property or NULL if this property has not parent.
		 Properies that have parents, are called subproperties. For example property of type
		 "Rectangle" has 4 subproperties: x, y, width, height.
		 Any change made to any subproperty is transferred to its parent property (if present),
		 So both property and its parent becomes changed. */
		KexiProperty *parent() const { return m_parent; }

		/*! \return a map of all children for this property, or NULL of there
		 is no children for this property */
		KexiProperty::List* children() const { return m_children_list; }

		/*! \return a child property for \a name, or NULL if there is not property with that name. */
		KexiProperty *child(const QCString& name);

		//! \return property name.
		QCString		name() const { return m_name; }

		//! \return property value.
		QVariant	value() const;

		/*! \return property value converted to text that can be displayed.
		 If this is a list property, name for currently selected key (value) is returned,
		  otherwise value (converted to text) is just returned. */
		QString valueText() const;

		/*! Sets this property value to a new value \a v. If this is a first change,
		 and \a saveOldValue is true, an old value is saved, and can be later retrieved
		 using oldValue(). If \a saveOldValue if false, old value is cleared and the
		 property looks loke it was not changed.
		 It the property has a parent property, the parent has set "changed" flag when needed.
		*/
		void setValue(const QVariant &v, bool saveOldValue = true);

		/*! For property of type "list of values":
		 sets \a key_list as a new list of keys and \a name_list as a new list
		 of i18n'e names (corresponding to keys). Sometimes it's necessary to change
		 the list. You should ensure yourself that current value is a string that
		 is one of a new \a key_list.
		 \sa keys(), \a names() */
		void setList(const QStringList &key_list, const QStringList &name_list);

		/*! Resets this property value to its old value.
		 The property becames unchanged after that and old value becames null.
		 This method is equal to setValue( oldValue(), false ); */
		void resetValue();

		/*! \return old property value. This makes only sense when changed() is true.
		 The old value is saved on first change.
		*/
		QVariant	oldValue() const { return m_oldValue; }

		//! \return property i18n'ed description.
		QString		desc() const { return m_desc; }

		/*! \return the QVariant::Type of property value and QVariant::StringList if this is a list property. */
		QVariant::Type  type() const;

		/*! \return a pointer to the string list containing all possible keys for this property
		 or NULL if this is not a property of type stringlist. The values in this list are ordered,
		 so the first key element is associated with first element from the list returned by
		 KexiProperty::names(), and so on. */
		QStringList* keys() const;

		/*! \return a pointer to the string list containing all possible i18n'd names for this property
		 or NULL if this is not a property of type stringlist.
		 @sa keys() */
		QStringList* names() const;

		/*! \return 1 if the property should be synced automatically in Property Editor
		  as soon as editor contents change (e.g. when the user types text). If autoSync() == 0, property value
		  will be updated when the user presses Enter or when another editor gets the focus.
		  Property follow Property Editor global rule if autoSync() != 0 and 1 (default)
		*/
		int		autoSync() const { return m_autosync; }

		/*! if \a sync == true, then the property will be synced automatically in Property Editor
		  as soon as editor contents change (e.g. when the user types text). If \a sync == false, property value
		  will be updated when the user presses Enter or when another editor gets the focus.
		*/
		void setAutoSync(int sync) { m_autosync = sync; }

		//! \return true if this preperty value is changed.
		bool changed() const;

		/*! Marks this property as changed if \a set is true, or unchanged if \a set is true. */
		void setChanged(bool set);

		/*! \return visiblility of this property. Property can be hidden, what can usually mean that
		 it wont be available to the GUI (or scripting). After construction, property is visible.
		 Parent property's visibility has a priority over visiblitity of this property. */
		bool isVisible() const;

		/*! Sets visiblility of this property to \a v. \sa isVisible() */
		void setVisible( bool v ) { m_visible=v; }

		/*! \return the value text useful for displaying.
		 The value is formatted depending on its QVariant::Type.
		*/
		static QString format(const QVariant &v);

		KexiPropertyBuffer*  buffer() { return m_buf; }

		void debug();
	protected:
		/*! Internal: Works like setValue(const QVariant &v, bool saveOldValue),
		 but allows to skip updating values for children. */
		void setValue(const QVariant &v, bool updateChildren, bool saveOldValue);

		/*! Internal: Works like setValue(const QVariant &v, bool saveOldValue),
		 but \a v is set for the child named \a childName. */
		void setChildValue(const QCString& childName, const QVariant &v,
			bool saveOldValue);

		/*! Internal: Updates this property value using \a v value coming from a child
		 named \a childName. This methid is only called by a child of this property,
		 in setValue(), to update parent's value. This method has no meaning
		 for childless (atomic) properties.
		 Example: if Rect.x property is cahnged in the child,
		 the child calls parent->updateValueForChild("x", value_of_x).
		*/
		void updateValueForChild(const QCString& childName, const QVariant &v,
			bool saveOldValue);

//	private:
		void init(QVariant value);

		QCString		m_name;
		QString		m_desc;
		QVariant	m_value;
		QVariant	m_oldValue;
		class KexiPropertyListData;
		KexiPropertyListData *m_list;
		KexiProperty* m_parent;
		QGuardedPtr<KexiPropertyBuffer> m_buf;
		KexiProperty::Dict* m_children_dict;
		KexiProperty::List* m_children_list;

		int m_autosync;
		bool m_changed : 1;
		bool m_visible : 1;

	friend class KexiPropertyBuffer;
};

#endif

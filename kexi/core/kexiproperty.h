/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Cedric Pasteur <cedric.pasteur@free.fr>

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

class QObject;
class QString;
class QStringList;

/** This class holds a single property, which can be any of the types supported by QVariant.
    It includes support for QStringList properties, an i18n'ed label and stores an old value to allow undo.
    
    Create a property :
    \code
    property = KexiProperty(name, value); // name is a QString, value is whatever type QVariant supports
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
//		/*! Creates a simple property with \a name as name and \a value as value. */
//		KexiProperty(const QString &name, QVariant value);

		/*! Creates a simple property with \a name as name, \a value as value 
		 and \a as i18n'ed description. */
		KexiProperty(const QString &name, QVariant value, const QString &desc = QString::null);

		/*! Creates a list property with \a name as name, \a value as value 
		  and \a list as the list of all possible values for \a value.
		  The user will be able to choose a value in \a list.
		*/
		KexiProperty(const QString &name, QVariant value, const QStringList &list, 
		 const QString &desc = QString::null);

		//! Copy constructor.
		KexiProperty(const KexiProperty &property);

		//! Creates an empty property.
		KexiProperty();

		~KexiProperty();
		
		const KexiProperty& operator=(const KexiProperty &property);

		//! \return property name.
		QString		name() const { return m_name; }
		//! \return property value.
		QVariant	value() const { return m_value; }
		/*! Sets this property value to a new value \a v. If this is a first change, 
		 and \a saveOldValue is true, an old value is saved, and can be later retrieved
		 using oldValue(). If \a saveOldValue if false, old value is cleared and the 
		 property looks loke it was not changed.
		*/
		void setValue(const QVariant &v, bool saveOldValue = true);

		/*! \return old property value. This makes only sense when changed() is true. 
		 The old value is saved on first change.
		*/
		QVariant	oldValue() const { return m_oldValue; }
		//! \return property i18n'ed description.
		QString		desc() const { return m_desc; }
		/*! \return the QVariant::Type of property value and QVariant::StringList if this is a list property. */
		QVariant::Type  type() const;
		/*! \return a pointer to the QStringList containing all possible values for this property. */
		QStringList*	list() const{ return m_list;}
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

		//! \return true is this preperty value is changed. 
		bool changed() const { return m_changed; }
		/*! Marks this property as changed if \a set is true, or unchanged if \a set is true. */
		void setChanged(bool set);

		/*! \return visiblility of this property. Property can be hidden, what can usually mean that 
		 it wont be available to the GUI (or scripting). After construction, property is visible. */
		bool isVisible( bool v ) const { return m_visible; }

		/*! Sets visiblility of this property to \a v. */
		void setVisible( bool v ) { m_visible=v; }
	private:
		QString		m_name;
		QString		m_desc;
		QVariant	m_value;
		QVariant	m_oldValue;
		QStringList	*m_list;
		int		m_autosync;
		bool m_changed : 1;
		bool m_visible : 1;
};

#endif

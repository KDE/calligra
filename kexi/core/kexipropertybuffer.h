/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef PROPERTYBUFFER_H
#define PROPERTYBUFFER_H

#include <qobject.h>
#include <qdict.h>
#include <qptrvector.h>

#include <kdebug.h>

#include "kexiproperty.h"

class PixmapCollection;

/** This class is a QDict<KexiProperty> which holds properties to be shown in
    Property Editor. Properties are indexed by their names, case insensitively.
    \sa KexiPropertyEditor for help on how to use KexiPropertyBuffer.
    \sa KexiProperty to see how to create properties.
**/
class KEXICORE_EXPORT KexiPropertyBuffer : public QObject, protected KexiProperty::Dict
{
	Q_OBJECT

	public:
		typedef QPtrVector<KexiPropertyBuffer> Vector;

		/*! Creates an empty KexiPropertyBuffer, i.e. a QMap<QString, KexiProperty>.
		 \a type_name means a name of this property buffer type. See typeName() description
		 for more information on type names.
		*/
		KexiPropertyBuffer(QObject *parent, const QString &type_name);

		virtual ~KexiPropertyBuffer();

		/*! \return property named with \a name. If no such property is found,
		 null property (KexiProperty::null) is returned. */
		inline KexiProperty& property( const QCString& name ) {
			KexiProperty *p = find(name);
			if (p) return *p;
			m_nonConstNull.m_name = "";
			kexiwarn << "KexiPropertyBuffer::property(): \"" << name << "\" NOT FOUND" << endl;
			return m_nonConstNull;
		}

		inline bool hasProperty( const QCString& name ) const { return find(name)!=0; }

		/*! Equivalent to property(const QCString& name) */
		inline KexiProperty& operator[] ( const QCString& name ) { return property(name); }

		/*! \return an ordered list of properties */
		inline KexiProperty::List* list() { return &m_list; }

		/*! Add \a property to buffer with property->name() as key in QMap.
		 Same as insert(\a property->name(), \a property).
		 The \a property object will be owned by this buffer.
		*/
		void add(KexiProperty *property);

		/* Change the value of property whose key is \a property to \a value.
		  By default, it only calls KexiProperty::setValue().
		*/
		virtual void changeProperty(const QCString &property, const QVariant &value);

		/*! Marks all properties as changed if \a set is true, or unchanged if \a set is false. */
		void setAllChanged(bool set);

		/* A name of this property buffer type, that is usable when
		 we want to know if two property buffer objects have the same type.
		 For example, \a type_name may be "KexiDB::Field::Integer" for property buffer
		 for given selected field of type integer, in "Alter Table Dialog".
		 This avoids e.g. reloading of all KexiPropertyEditor's contents.
		 Also, this allows to know if two property-buffer objects are compatible
		 by their property sets.
		 For comparing purposes, type names are case insensitive.
		*/
		inline QString typeName() const { return m_typeName; }

		inline void setCollection(PixmapCollection *collection) { m_collection = collection; }
		inline PixmapCollection*  collection()  { return m_collection; }

		/*! The property \a prop value is now the pixmap in the PixmapCollection whose name is \a pixmapName.*/
		void addCollectionPixmap(KexiProperty *prop, const QString& pixmapName);

//		//! \return the name of the pixmap for the property whose name is \a name.
//		QString pixmapName(const char *name);

		/*! Removes all properties from the buffer and destroys them. */
		virtual void clear();

		void debug();

		/*! Used by property editor to preserve previous selection when this buffer is set again. */
		inline QCString prevSelection() const { return m_prevSelection; }
		inline void setPrevSelection(const QCString& prevSelection) { m_prevSelection = prevSelection; }

	signals:
		/*! This signal is emitted when \a property has changed
		   (i.e. when changeProperty() was called). */
		void propertyChanged(KexiPropertyBuffer &buf, KexiProperty &property);

		/*! Parameterless version of the above method. */
		void propertyChanged();

		void propertyReset(KexiPropertyBuffer &buf, KexiProperty &property);

		//! This signal is emitted when the user chooses an item in the PixmapCollection
		void collectionItemChoosed(KexiPropertyBuffer &buf, KexiProperty &property);

		// TMP until we have custom editors
		void propertyExecuted(KexiPropertyBuffer &buf, KexiProperty &prop, const QString &value);

		void destroying();

	protected:
		QString m_typeName;
		KexiProperty::List m_list;
		QCString m_prevSelection;
		PixmapCollection  *m_collection;
		
		static KexiProperty m_nonConstNull;

	friend class KexiProperty;
};

#endif

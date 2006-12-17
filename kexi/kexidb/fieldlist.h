/* This file is part of the KDE project
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDB_FIELDLIST_H
#define KEXIDB_FIELDLIST_H

#include <q3valuelist.h>
#include <q3dict.h>
#include <qstring.h>

#include <kexidb/field.h>
#include <kexidb/driver.h>

namespace KexiDB {

class Connection;

/*! Helper class that stores list of fields.
*/

class KEXI_DB_EXPORT FieldList
{
	public:
		/*! Creates empty list of fields. If \a owner is true, the list will be
		 owner of any added field, what means that these field 
		 will be removed on the list destruction. Otherwise, the list
		 just points any field that was added. 
		 \sa isOwner()
		*/
		FieldList(bool owner = false);

		/*! Copy constructor. */
		FieldList(const FieldList& fl);

		/*! Destroys the list. If the list owns fields (see constructor),
		 these are also deleted. */
		virtual ~FieldList();
		
		/*! \return number of fields in the list. */
		inline uint fieldCount() const { return m_fields.count(); }

		/*! Adds \a field at the and of field list. */
		FieldList& addField(Field *field);

		/*! Inserts \a field into a specified position (\a index).
		
		 Note: You can reimplement this method but you should still call 
		 this implementation in your subclass. */
		virtual FieldList& insertField(uint index, Field *field);
		
		/*! Removes field from the field list. Use with care. 
		 
		 Note: You can reimplement this method but you should still call 
		 this implementation in your subclass. */
		virtual void removeField(KexiDB::Field *field);

		/*! \return field id or NULL if there is no such a field. */
		inline Field* field(uint id) { return (id < m_fields.count()) ? m_fields.at(id) : 0; }
		
		/*! \return field with name \a name or NULL if there is no such a field. */
		virtual Field* field(const QString& name);

		/*! \return true if this list contains given \a field. */
		inline bool hasField(const Field* field) { return m_fields.findRef(field)!=-1; }

		/*! \return first occurrence of \a field in the list 
		 or -1 if this list does not contain this field. */
		inline int indexOf(const Field* field) { return m_fields.findRef(field); }

		/*! \return list of field names for this list. */
		QStringList names() const;
		
		Field::ListIterator fieldsIterator() const { return Field::ListIterator(m_fields); }

		inline Field::List* fields() { return &m_fields; }

		/*! \return list of autoincremented fields. The list is owned by this FieldList object. */
		Field::List* autoIncrementFields();

		/*! \return true if fields in the list are owned by this list. */
		inline bool isOwner() const { return m_fields.autoDelete(); }

		/*! Removes all fields from the list. */
		virtual void clear();

		/*! \return String for debugging purposes. */
		virtual QString debugString();
		
		/*! Shows debug information about all fields in the list. */
		void debug();

		/*! Creates and returns list that contain fields selected by name.
		 At least field (exising on this list) should be selected, otherwise NULL is
		 returned. Returned FieldList object is not owned by any parent (so you need 
		 to destroy yourself) and Field objects included in it are not owned by it 
		 (but still as before, by 'this' object).
		 Returned list can be usable e.g. as argument for Connection::insertRecord().
		*/
		FieldList* subList(const QString& n1, const QString& n2 = QString::null, 
			const QString& n3 = QString::null, const QString& n4 = QString::null,
			const QString& n5 = QString::null, const QString& n6 = QString::null,
			const QString& n7 = QString::null, const QString& n8 = QString::null,
			const QString& n9 = QString::null, const QString& n10 = QString::null,
			const QString& n11 = QString::null, const QString& n12 = QString::null,
			const QString& n13 = QString::null, const QString& n14 = QString::null,
			const QString& n15 = QString::null, const QString& n16 = QString::null,
			const QString& n17 = QString::null, const QString& n18 = QString::null
		);

		/*! Like above, but with a QStringList */
		FieldList* subList(const QStringList& list);
	
		/*! \return a string that is a result of all field names concatenated 
		 and with "," between. This is usable e.g. as argument like "field1,field2" 
		 for "INSERT INTO (xxx) ..". The result of this method is effectively cached,
		 and it is invalidated when set of fields changes (e.g. using clear() 
		 or addField()).
		*/
		QString sqlFieldsList(Driver *driver);

		/*! Like above, but thsi is convenient static function, so you can pass any \a list here. */
		static QString sqlFieldsList(Field::List* list, Driver *driver);

		/*! @internal Renames field \a oldName to \a newName. 
		 Do not use this for physical renaming columns. Use AlterTableHandler instead. */
		void renameField(const QString& oldName, const QString& newName);

		/*! @internal 
		 \overload void renameField(const QString& oldName, const QString& newName) */
		void renameField(KexiDB::Field *field, const QString& newName);

	protected:
		Field::List m_fields;
		Q3Dict<Field> m_fields_by_name; //!< Fields collected by name. Not used by QuerySchema.
		Field::List *m_autoinc_fields;
	
	private:
		//! cached
		QString m_sqlFields;
};

} //namespace KexiDB

#endif

/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDB_FIELDLIST_H
#define KEXIDB_FIELDLIST_H

#include <qvaluelist.h>
#include <qstring.h>

#include <kexidb/field.h>

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
		
		/*! Destroys the list. If the list owns fields (see constructor),
		 these are also deleted. */
		virtual ~FieldList();
		
		/*! \return nomber of fields in the list. */
		unsigned int fieldCount() const;
		
		/*! Adds field at the and of field list. */
		virtual FieldList& addField(Field *field);
		
		/*! \return field #id or NULL if there is no such a field. */
		KexiDB::Field* field(unsigned int id);

		Field::ListIterator fieldsIterator() { return Field::ListIterator(m_fields); }

		/*! \return true if fields in the list are owned by this list. */
		bool isOwner() const;

		/*! Removes all fields from the list, clears name. */
		virtual void clear();

		/*! Shows debug information about all fields in the list. */
		virtual void debug() const;
	protected:

	//js	QStringList m_primaryKeys;
//		QString m_name;
		Field::List m_fields;

//	friend class Connection;
};

} //namespace KexiDB

#endif
